#ifndef OGC_DRAW_ENGINE_POOL_H
#define OGC_DRAW_ENGINE_POOL_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/engine_type.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace ogc {
namespace draw {

class OGC_DRAW_API EnginePool {
public:
    static EnginePool& Instance();
    
    void Initialize(EngineType type, int poolSize = 4);
    void Shutdown();
    
    std::shared_ptr<DrawEngine> Acquire(EngineType type);
    void Release(std::shared_ptr<DrawEngine> engine);
    
    void SetMaxPoolSize(EngineType type, int maxSize);
    int GetPoolSize(EngineType type) const;
    int GetAvailableCount(EngineType type) const;
    
    void Warmup(EngineType type);
    void Clear(EngineType type);
    
    void SetEngineFactory(EngineType type, 
                          std::function<std::unique_ptr<DrawEngine>()> factory);
    
    class OGC_DRAW_API EngineGuard {
    public:
        EngineGuard() : m_engine(nullptr), m_pool(nullptr), m_type(EngineType::kSimple2D), m_released(true) {}
        EngineGuard(std::shared_ptr<DrawEngine> engine, EnginePool* pool, EngineType type);
        ~EngineGuard();
        
        EngineGuard(const EngineGuard&) = delete;
        EngineGuard& operator=(const EngineGuard&) = delete;
        
        EngineGuard(EngineGuard&& other) noexcept;
        EngineGuard& operator=(EngineGuard&& other) noexcept;
        
        DrawEngine* get() const { return m_engine.get(); }
        DrawEngine* operator->() const { return m_engine.get(); }
        DrawEngine& operator*() const { return *m_engine; }
        explicit operator bool() const { return m_engine != nullptr; }
        
    private:
        std::shared_ptr<DrawEngine> m_engine;
        EnginePool* m_pool;
        EngineType m_type;
        bool m_released;
    };
    
    EngineGuard AcquireGuard(EngineType type);

private:
    EnginePool();
    ~EnginePool();
    
    EnginePool(const EnginePool&) = delete;
    EnginePool& operator=(const EnginePool&) = delete;
    
    struct PoolEntry {
        std::vector<std::shared_ptr<DrawEngine>> available;
        std::vector<std::weak_ptr<DrawEngine>> inUse;
        int maxSize;
        std::function<std::unique_ptr<DrawEngine>()> factory;
    };
    
    PoolEntry* GetPool(EngineType type);
    const PoolEntry* GetPool(EngineType type) const;
    
    std::unordered_map<EngineType, PoolEntry> m_pools;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_initialized;
};

}

}

#endif
