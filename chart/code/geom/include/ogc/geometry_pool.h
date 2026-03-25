#pragma once

#include "geometry.h"
#include "point.h"
#include "linestring.h"
#include "polygon.h"
#include "coordinate.h"
#include <memory>
#include <stack>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <typeindex>
#include <functional>

namespace ogc {

template<typename T>
class ObjectPool {
public:
    using FactoryFunc = std::function<std::unique_ptr<T>()>;
    
    explicit ObjectPool(FactoryFunc factory, size_t initialSize = 64, size_t maxSize = 1024)
        : m_factory(factory), m_maxSize(maxSize), m_created(0), m_reused(0) {
        for (size_t i = 0; i < initialSize; ++i) {
            m_pool.push(m_factory());
        }
    }
    
    ~ObjectPool() = default;
    
    std::unique_ptr<T> Acquire() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pool.empty()) {
            auto obj = std::move(m_pool.top());
            m_pool.pop();
            m_reused++;
            return obj;
        }
        
        m_created++;
        return m_factory();
    }
    
    void Release(std::unique_ptr<T> obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_pool.size() < m_maxSize) {
            obj->Reset();
            m_pool.push(std::move(obj));
        }
    }
    
    size_t Size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pool.size();
    }
    
    size_t MaxSize() const noexcept { return m_maxSize; }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_pool.empty()) {
            m_pool.pop();
        }
    }
    
    struct Statistics {
        size_t poolSize;
        size_t created;
        size_t reused;
        size_t maxSize;
    };
    
    Statistics GetStatistics() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return {m_pool.size(), m_created, m_reused, m_maxSize};
    }
    
private:
    mutable std::mutex m_mutex;
    std::stack<std::unique_ptr<T>> m_pool;
    FactoryFunc m_factory;
    size_t m_maxSize;
    std::atomic<size_t> m_created;
    std::atomic<size_t> m_reused;
};

class GeometryPool {
public:
    static GeometryPool& Instance() {
        static GeometryPool instance;
        return instance;
    }
    
    PointPtr AcquirePoint() {
        return m_pointPool.Acquire();
    }
    
    void ReleasePoint(PointPtr point) {
        m_pointPool.Release(std::move(point));
    }
    
    LineStringPtr AcquireLineString() {
        return m_lineStringPool.Acquire();
    }
    
    void ReleaseLineString(LineStringPtr lineString) {
        m_lineStringPool.Release(std::move(lineString));
    }
    
    PolygonPtr AcquirePolygon() {
        return m_polygonPool.Acquire();
    }
    
    void ReleasePolygon(PolygonPtr polygon) {
        m_polygonPool.Release(std::move(polygon));
    }
    
    template<typename T>
    std::unique_ptr<T> Acquire();
    
    template<typename T, typename... Args>
    std::unique_ptr<T> Create(Args&&... args) {
        auto obj = Acquire<T>();
        obj->Reset();
        obj->Initialize(std::forward<Args>(args)...);
        return obj;
    }
    
    template<typename T>
    void Release(std::unique_ptr<T> obj) {
        ReleaseInternal(std::move(obj));
    }
    
    void SetPoolSize(size_t size) {
        m_poolSize = size;
    }
    
    size_t GetPoolSize() const noexcept { return m_poolSize; }
    
    struct Statistics {
        size_t pointPoolSize;
        size_t lineStringPoolSize;
        size_t polygonPoolSize;
        size_t totalCreated;
        size_t totalReused;
    };
    
    Statistics GetStatistics() const {
        auto pointStats = m_pointPool.GetStatistics();
        auto lineStats = m_lineStringPool.GetStatistics();
        auto polyStats = m_polygonPool.GetStatistics();
        
        return {
            pointStats.poolSize,
            lineStats.poolSize,
            polyStats.poolSize,
            pointStats.created + lineStats.created + polyStats.created,
            pointStats.reused + lineStats.reused + polyStats.reused
        };
    }
    
    void Clear() {
        m_pointPool.Clear();
        m_lineStringPool.Clear();
        m_polygonPool.Clear();
    }
    
    GeometryPool(const GeometryPool&) = delete;
    GeometryPool& operator=(const GeometryPool&) = delete;
    
private:
    GeometryPool() : m_poolSize(64) {}
    
    template<typename T>
    void ReleaseInternal(std::unique_ptr<T> obj);
    
    ObjectPool<Point> m_pointPool{[]() { return Point::Create(Coordinate::Empty()); }, 64, 1024};
    ObjectPool<LineString> m_lineStringPool{[]() { return LineString::Create(); }, 32, 512};
    ObjectPool<Polygon> m_polygonPool{[]() { return Polygon::Create(); }, 16, 256};
    size_t m_poolSize;
};

template<>
inline PointPtr GeometryPool::Acquire<Point>() {
    return AcquirePoint();
}

template<>
inline LineStringPtr GeometryPool::Acquire<LineString>() {
    return AcquireLineString();
}

template<>
inline PolygonPtr GeometryPool::Acquire<Polygon>() {
    return AcquirePolygon();
}

template<>
inline void GeometryPool::ReleaseInternal<Point>(PointPtr obj) {
    ReleasePoint(std::move(obj));
}

template<>
inline void GeometryPool::ReleaseInternal<LineString>(LineStringPtr obj) {
    ReleaseLineString(std::move(obj));
}

template<>
inline void GeometryPool::ReleaseInternal<Polygon>(PolygonPtr obj) {
    ReleasePolygon(std::move(obj));
}

class ScopedGeometry {
public:
    explicit ScopedGeometry(PointPtr geom) 
        : m_type(GeomType::kPoint), m_point(std::move(geom)) {}
    
    explicit ScopedGeometry(LineStringPtr geom) 
        : m_type(GeomType::kLineString), m_lineString(std::move(geom)) {}
    
    explicit ScopedGeometry(PolygonPtr geom) 
        : m_type(GeomType::kPolygon), m_polygon(std::move(geom)) {}
    
    ~ScopedGeometry() {
        Release();
    }
    
    ScopedGeometry(ScopedGeometry&& other) noexcept 
        : m_type(other.m_type) {
        switch (m_type) {
            case GeomType::kPoint:
                m_point = std::move(other.m_point);
                break;
            case GeomType::kLineString:
                m_lineString = std::move(other.m_lineString);
                break;
            case GeomType::kPolygon:
                m_polygon = std::move(other.m_polygon);
                break;
            default:
                break;
        }
        other.m_type = GeomType::kUnknown;
    }
    
    ScopedGeometry& operator=(ScopedGeometry&& other) noexcept {
        if (this != &other) {
            Release();
            m_type = other.m_type;
            switch (m_type) {
                case GeomType::kPoint:
                    m_point = std::move(other.m_point);
                    break;
                case GeomType::kLineString:
                    m_lineString = std::move(other.m_lineString);
                    break;
                case GeomType::kPolygon:
                    m_polygon = std::move(other.m_polygon);
                    break;
                default:
                    break;
            }
            other.m_type = GeomType::kUnknown;
        }
        return *this;
    }
    
    ScopedGeometry(const ScopedGeometry&) = delete;
    ScopedGeometry& operator=(const ScopedGeometry&) = delete;
    
    Geometry* Get() const {
        switch (m_type) {
            case GeomType::kPoint:
                return m_point.get();
            case GeomType::kLineString:
                return m_lineString.get();
            case GeomType::kPolygon:
                return m_polygon.get();
            default:
                return nullptr;
        }
    }
    
    GeomType GetType() const noexcept { return m_type; }
    
private:
    void Release() {
        auto& pool = GeometryPool::Instance();
        switch (m_type) {
            case GeomType::kPoint:
                if (m_point) pool.ReleasePoint(std::move(m_point));
                break;
            case GeomType::kLineString:
                if (m_lineString) pool.ReleaseLineString(std::move(m_lineString));
                break;
            case GeomType::kPolygon:
                if (m_polygon) pool.ReleasePolygon(std::move(m_polygon));
                break;
            default:
                break;
        }
    }
    
    GeomType m_type = GeomType::kUnknown;
    union {
        PointPtr m_point;
        LineStringPtr m_lineString;
        PolygonPtr m_polygon;
    };
};

}
