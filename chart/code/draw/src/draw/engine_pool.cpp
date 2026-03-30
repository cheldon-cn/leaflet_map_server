#include "ogc/draw/engine_pool.h"
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <algorithm>

namespace ogc {
namespace draw {

EnginePool& EnginePool::Instance() {
    static EnginePool instance;
    return instance;
}

EnginePool::EnginePool() : m_initialized(false) {
}

EnginePool::~EnginePool() {
    Shutdown();
}

void EnginePool::Initialize(EngineType type, int poolSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& pool = m_pools[type];
    pool.maxSize = poolSize;
    pool.available.clear();
    pool.inUse.clear();
    m_initialized = true;
}

void EnginePool::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& pair : m_pools) {
        pair.second.available.clear();
        pair.second.inUse.clear();
    }
    m_pools.clear();
    m_initialized = false;
}

std::shared_ptr<DrawEngine> EnginePool::Acquire(EngineType type) {
    std::unique_lock<std::mutex> lock(m_mutex);
    auto* pool = GetPool(type);
    if (!pool) {
        return nullptr;
    }
    if (!pool->available.empty()) {
        auto engine = pool->available.back();
        pool->available.pop_back();
        pool->inUse.push_back(engine);
        return engine;
    }
    if (pool->factory) {
        auto engine = pool->factory();
        if (engine) {
            auto shared = std::shared_ptr<DrawEngine>(engine.release());
            pool->inUse.push_back(shared);
            return shared;
        }
    }
    return nullptr;
}

void EnginePool::Release(std::shared_ptr<DrawEngine> engine) {
    if (!engine) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    EngineType type = engine->GetType();
    auto* pool = GetPool(type);
    if (!pool) return;
    auto it = std::find_if(pool->inUse.begin(), pool->inUse.end(),
        [&engine](const std::weak_ptr<DrawEngine>& weak) {
            auto locked = weak.lock();
            return locked && locked.get() == engine.get();
        });
    if (it != pool->inUse.end()) {
        pool->inUse.erase(it);
        if (static_cast<int>(pool->available.size()) < pool->maxSize) {
            pool->available.push_back(engine);
        }
    }
    m_cv.notify_one();
}

void EnginePool::SetMaxPoolSize(EngineType type, int maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto* pool = GetPool(type);
    if (pool) pool->maxSize = maxSize;
}

int EnginePool::GetPoolSize(EngineType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto* pool = GetPool(type);
    if (!pool) return 0;
    return static_cast<int>(pool->available.size() + pool->inUse.size());
}

int EnginePool::GetAvailableCount(EngineType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto* pool = GetPool(type);
    if (!pool) return 0;
    return static_cast<int>(pool->available.size());
}

void EnginePool::Warmup(EngineType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto* pool = GetPool(type);
    if (!pool || !pool->factory) return;
    while (static_cast<int>(pool->available.size()) < pool->maxSize) {
        auto engine = pool->factory();
        if (engine) {
            pool->available.push_back(std::shared_ptr<DrawEngine>(engine.release()));
        } else {
            break;
        }
    }
}

void EnginePool::Clear(EngineType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto* pool = GetPool(type);
    if (pool) pool->available.clear();
}

void EnginePool::SetEngineFactory(EngineType type, 
                                   std::function<std::unique_ptr<DrawEngine>()> factory) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& pool = m_pools[type];
    pool.factory = factory;
}

EnginePool::PoolEntry* EnginePool::GetPool(EngineType type) {
    auto it = m_pools.find(type);
    return it != m_pools.end() ? &it->second : nullptr;
}

const EnginePool::PoolEntry* EnginePool::GetPool(EngineType type) const {
    auto it = m_pools.find(type);
    return it != m_pools.end() ? &it->second : nullptr;
}

EnginePool::EngineGuard::EngineGuard(std::shared_ptr<DrawEngine> engine, 
                                      EnginePool* pool, EngineType type)
    : m_engine(engine), m_pool(pool), m_type(type), m_released(false) {
}

EnginePool::EngineGuard::~EngineGuard() {
    if (!m_released && m_engine && m_pool) {
        m_pool->Release(m_engine);
    }
}

EnginePool::EngineGuard::EngineGuard(EngineGuard&& other) noexcept
    : m_engine(std::move(other.m_engine))
    , m_pool(other.m_pool)
    , m_type(other.m_type)
    , m_released(other.m_released) {
    other.m_released = true;
}

EnginePool::EngineGuard& EnginePool::EngineGuard::operator=(EngineGuard&& other) noexcept {
    if (this != &other) {
        if (!m_released && m_engine && m_pool) {
            m_pool->Release(m_engine);
        }
        m_engine = std::move(other.m_engine);
        m_pool = other.m_pool;
        m_type = other.m_type;
        m_released = other.m_released;
        other.m_released = true;
    }
    return *this;
}

EnginePool::EngineGuard EnginePool::AcquireGuard(EngineType type) {
    return EngineGuard(Acquire(type), this, type);
}

}
}
