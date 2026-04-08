#ifndef OGC_DRAW_RENDER_MEMORY_POOL_H
#define OGC_DRAW_RENDER_MEMORY_POOL_H

#include <ogc/draw/export.h>
#include <ogc/draw/gpu_resource_manager.h>
#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>

namespace ogc {
namespace draw {

using BufferHandle = GPUHandle;

struct BufferBlock {
    BufferHandle handle;
    size_t offset = 0;
    size_t size = 0;
    bool isFree = true;
};

struct MemoryPoolStats {
    size_t totalAllocated = 0;
    size_t totalCapacity = 0;
    size_t freeBlockCount = 0;
    size_t usedBlockCount = 0;
    float fragmentationRatio = 0.0f;
    size_t vertexBufferCount = 0;
    size_t indexBufferCount = 0;
    size_t uniformBufferCount = 0;
};

class OGC_DRAW_API RenderMemoryPool {
public:
    static RenderMemoryPool& Instance();
    
    BufferHandle AllocateVertexBuffer(size_t size);
    BufferHandle AllocateIndexBuffer(size_t size);
    BufferHandle AllocateUniformBuffer(size_t size);
    
    void Recycle(BufferHandle handle);
    void RecycleAll();
    
    void Compact();
    void Defragment();
    
    size_t GetMemoryUsage() const;
    size_t GetPoolCapacity() const;
    float GetFragmentationRatio() const;
    MemoryPoolStats GetStats() const;
    
    void SetMaxPoolSize(size_t maxSize);
    void SetGrowthFactor(float factor);
    
    void SetAvailable(bool available);
    bool IsAvailable() const;

private:
    RenderMemoryPool();
    ~RenderMemoryPool();
    
    RenderMemoryPool(const RenderMemoryPool&) = delete;
    RenderMemoryPool& operator=(const RenderMemoryPool&) = delete;
    
    BufferHandle Allocate(BufferType type, size_t size);
    BufferHandle AllocateFromFreeBlock(BufferType type, size_t size);
    BufferHandle AllocateNewBlock(BufferType type, size_t size);
    
    void MergeFreeBlocks(BufferType type);
    float CalculateFragmentation() const;
    
    std::map<BufferType, std::vector<BufferBlock>> m_freeBlocks;
    std::unordered_map<BufferHandle, BufferBlock> m_allocatedBlocks;
    std::unordered_map<BufferHandle, BufferType> m_blockTypes;
    
    size_t m_totalAllocated = 0;
    size_t m_maxPoolSize = 512 * 1024 * 1024;
    float m_growthFactor = 1.5f;
    bool m_available = false;
    
    mutable std::mutex m_mutex;
    std::atomic<uint32_t> m_nextHandle{1};
};

}
}

#endif
