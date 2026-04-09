#pragma once

/**
 * @file envelope.h
 * @brief 包络类定义，用于表示几何对象的边界框
 */

#include "common.h"
#include "coordinate.h"
#include <algorithm>
#include <string>

namespace ogc {

/**
 * @brief 包络类，表示几何对象的最小边界矩形（MBR�?
 * 
 * 用于空间索引和快速空间关系判�?
 */
class OGC_GEOM_API Envelope {
public:
    Envelope() noexcept : m_isNull(true) {}
    
    /**
     * @brief 从边界值构造包�?
     */
    Envelope(double minX, double minY, double maxX, double maxY) noexcept
        : m_minX(minX), m_minY(minY), m_maxX(maxX), m_maxY(maxY), m_isNull(false) {
        Normalize();
    }
    
    /**
     * @brief 从两个坐标点构造包�?
     */
    Envelope(const Coordinate& p1, const Coordinate& p2) noexcept
        : Envelope(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                   std::max(p1.x, p2.x), std::max(p1.y, p2.y)) {}
    
    /**
     * @brief 从坐标列表构造包�?
     */
    explicit Envelope(const CoordinateList& coords) noexcept {
        ExpandToInclude(coords);
    }
    
    double GetMinX() const noexcept { return m_minX; }
    double GetMinY() const noexcept { return m_minY; }
    double GetMaxX() const noexcept { return m_maxX; }
    double GetMaxY() const noexcept { return m_maxY; }
    
    /**
     * @brief 获取包络宽度
     */
    double GetWidth() const noexcept {
        return m_isNull ? 0.0 : m_maxX - m_minX;
    }
    
    /**
     * @brief 获取包络高度
     */
    double GetHeight() const noexcept {
        return m_isNull ? 0.0 : m_maxY - m_minY;
    }
    
    /**
     * @brief 获取包络面积
     */
    double GetArea() const noexcept {
        return m_isNull ? 0.0 : GetWidth() * GetHeight();
    }
    
    /**
     * @brief 获取包络周长
     */
    double GetPerimeter() const noexcept {
        return m_isNull ? 0.0 : 2.0 * (GetWidth() + GetHeight());
    }
    
    /**
     * @brief 获取包络中心坐标
     */
    Coordinate GetCentre() const noexcept {
        if (m_isNull) return Coordinate::Empty();
        return Coordinate((m_minX + m_maxX) / 2.0, (m_minY + m_maxY) / 2.0);
    }
    
    bool IsNull() const noexcept { return m_isNull; }
    bool IsPoint() const noexcept { return !m_isNull && m_minX == m_maxX && m_minY == m_maxY; }
    bool IsEmpty() const noexcept { return m_isNull; }
    
    void SetNull() noexcept {
        m_isNull = true;
        m_minX = m_minY = m_maxX = m_maxY = 0.0;
    }
    
    /**
     * @brief 扩展包络以包含指定坐�?
     */
    void ExpandToInclude(const Coordinate& coord) noexcept {
        if (coord.IsEmpty()) return;
        
        if (m_isNull) {
            m_minX = m_maxX = coord.x;
            m_minY = m_maxY = coord.y;
            m_isNull = false;
        } else {
            m_minX = std::min(m_minX, coord.x);
            m_maxX = std::max(m_maxX, coord.x);
            m_minY = std::min(m_minY, coord.y);
            m_maxY = std::max(m_maxY, coord.y);
        }
    }
    
    /**
     * @brief 扩展包络以包含坐标列�?
     */
    void ExpandToInclude(const CoordinateList& coords) noexcept {
        for (const auto& coord : coords) {
            ExpandToInclude(coord);
        }
    }
    
    /**
     * @brief 扩展包络以包含另一个包�?
     */
    void ExpandToInclude(const Envelope& other) noexcept {
        if (other.m_isNull) return;
        
        if (m_isNull) {
            *this = other;
        } else {
            m_minX = std::min(m_minX, other.m_minX);
            m_maxX = std::max(m_maxX, other.m_maxX);
            m_minY = std::min(m_minY, other.m_minY);
            m_maxY = std::max(m_maxY, other.m_maxY);
        }
    }
    
    /**
     * @brief 按指定量扩展包络
     */
    void ExpandBy(double deltaX, double deltaY) noexcept {
        if (m_isNull) return;
        
        m_minX -= deltaX;
        m_maxX += deltaX;
        m_minY -= deltaY;
        m_maxY += deltaY;
    }
    
    /**
     * @brief 判断是否包含指定坐标
     */
    bool Contains(const Coordinate& coord) const noexcept {
        if (m_isNull || coord.IsEmpty()) return false;
        
        return coord.x >= m_minX && coord.x <= m_maxX &&
               coord.y >= m_minY && coord.y <= m_maxY;
    }
    
    /**
     * @brief 判断是否包含另一个包�?
     */
    bool Contains(const Envelope& other) const noexcept {
        if (m_isNull || other.m_isNull) return false;
        
        return other.m_minX >= m_minX && other.m_maxX <= m_maxX &&
               other.m_minY >= m_minY && other.m_maxY <= m_maxY;
    }
    
    /**
     * @brief 判断是否与另一个包络相�?
     */
    bool Intersects(const Envelope& other) const noexcept {
        if (m_isNull || other.m_isNull) return false;
        
        return !(other.m_maxX < m_minX || other.m_minX > m_maxX ||
                 other.m_maxY < m_minY || other.m_minY > m_maxY);
    }
    
    /**
     * @brief 判断是否与另一个包络重�?
     */
    bool Overlaps(const Envelope& other) const noexcept {
        return Intersects(other) && !Contains(other) && !other.Contains(*this);
    }
    
    /**
     * @brief 判断两个包络是否相等
     */
    bool Equals(const Envelope& other, double tolerance = DEFAULT_TOLERANCE) const noexcept {
        if (m_isNull && other.m_isNull) return true;
        if (m_isNull || other.m_isNull) return false;
        
        return std::abs(m_minX - other.m_minX) <= tolerance &&
               std::abs(m_minY - other.m_minY) <= tolerance &&
               std::abs(m_maxX - other.m_maxX) <= tolerance &&
               std::abs(m_maxY - other.m_maxY) <= tolerance;
    }
    
    /**
     * @brief 计算与另一个包络的交集
     */
    Envelope Intersection(const Envelope& other) const noexcept {
        if (!Intersects(other)) return Envelope();
        
        return Envelope(
            std::max(m_minX, other.m_minX),
            std::max(m_minY, other.m_minY),
            std::min(m_maxX, other.m_maxX),
            std::min(m_maxY, other.m_maxY)
        );
    }
    
    /**
     * @brief 计算与另一个包络的并集
     */
    Envelope Union(const Envelope& other) const noexcept {
        if (m_isNull) return other;
        if (other.m_isNull) return *this;
        
        return Envelope(
            std::min(m_minX, other.m_minX),
            std::min(m_minY, other.m_minY),
            std::max(m_maxX, other.m_maxX),
            std::max(m_maxY, other.m_maxY)
        );
    }
    
    /**
     * @brief 计算到指定坐标的距离
     */
    double Distance(const Coordinate& coord) const noexcept {
        if (Contains(coord)) return 0.0;
        
        double dx = 0.0, dy = 0.0;
        
        if (coord.x < m_minX) dx = m_minX - coord.x;
        else if (coord.x > m_maxX) dx = coord.x - m_maxX;
        
        if (coord.y < m_minY) dy = m_minY - coord.y;
        else if (coord.y > m_maxY) dy = coord.y - m_maxY;
        
        return std::sqrt(dx * dx + dy * dy);
    }
    
    /**
     * @brief 计算到另一个包络的距离
     */
    double Distance(const Envelope& other) const noexcept {
        if (Intersects(other)) return 0.0;
        
        double dx = 0.0, dy = 0.0;
        
        if (m_maxX < other.m_minX) dx = other.m_minX - m_maxX;
        else if (m_minX > other.m_maxX) dx = m_minX - other.m_maxX;
        
        if (m_maxY < other.m_minY) dy = other.m_minY - m_maxY;
        else if (m_minY > other.m_maxY) dy = m_minY - other.m_maxY;
        
        return std::sqrt(dx * dx + dy * dy);
    }
    
    Coordinate GetLowerLeft() const noexcept {
        return m_isNull ? Coordinate::Empty() : Coordinate(m_minX, m_minY);
    }
    
    Coordinate GetUpperRight() const noexcept {
        return m_isNull ? Coordinate::Empty() : Coordinate(m_maxX, m_maxY);
    }
    
    std::string ToWKT() const;
    std::string ToString() const;
    
    bool operator==(const Envelope& rhs) const noexcept { return Equals(rhs); }
    bool operator!=(const Envelope& rhs) const noexcept { return !Equals(rhs); }

protected:
    double m_minX = 0.0;
    double m_minY = 0.0;
    double m_maxX = 0.0;
    double m_maxY = 0.0;
    bool m_isNull = true;
    
    void Normalize() noexcept {
        if (m_minX > m_maxX) std::swap(m_minX, m_maxX);
        if (m_minY > m_maxY) std::swap(m_minY, m_maxY);
    }
};

}
