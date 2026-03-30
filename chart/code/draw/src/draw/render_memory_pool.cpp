#include <ogc/draw/render_memory_pool.h>
#include <algorithm>
#include <cstring>

namespace ogc {
namespace draw {

RenderMemoryPool& RenderMemoryPool::Instance()
{
    static RenderMemoryPool instance;
    return instance;
}

RenderMemoryPool::RenderMemoryPool()
{
}

RenderMemoryPool::~RenderMemoryPool()
{
    RecycleAll();
}

BufferHandle RenderMemoryPool::AllocateVertexBuffer(size_t size)
{
    return Allocate(BufferType::kVertex, size);
}

BufferHandle RenderMemoryPool::AllocateIndexBuffer(size_t size)
{
    return Allocate(BufferType::kIndex, size);
}

BufferHandle RenderMemoryPool::AllocateUniformBuffer(size_t size)
{
    return Allocate(BufferType::kUniform, size);
}

BufferHandle RenderMemoryPool::Allocate(BufferType type, size_t size)
{
    if (!m_available || size == 0) {
        return INVALID_GPU_HANDLE;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    BufferHandle handle = AllocateFromFreeBlock(type, size);
    if (handle != INVALID_GPU_HANDLE) {
        return handle;
    }
    
    if (m_totalAllocated + size > m_maxPoolSize) {
        return INVALID_GPU_HANDLE;
    }
    
    return AllocateNewBlock(type, size);
}

BufferHandle RenderMemoryPool::AllocateFromFreeBlock(BufferType type, size_t size)
{
    auto& freeList = m_freeBlocks[type];
    
    auto it = std::find_if(freeList.begin(), freeList.end(),
        [size](const BufferBlock& block) {
            return block.isFree && block.size >= size;
        });
    
    if (it == freeList.end()) {
        return INVALID_GPU_HANDLE;
    }
    
    BufferBlock block;
    block.handle = m_nextHandle.fetch_add(1);
    block.offset = 0;
    block.size = size;
    block.isFree = false;
    
    if (it->size > size) {
        it->size -= size;
    } else {
        freeList.erase(it);
    }
    
    m_allocatedBlocks[block.handle] = block;
    m_blockTypes[block.handle] = type;
    m_totalAllocated += size;
    
    return block.handle;
}

BufferHandle RenderMemoryPool::AllocateNewBlock(BufferType type, size_t size)
{
    BufferBlock block;
    block.handle = m_nextHandle.fetch_add(1);
    block.offset = 0;
    block.size = size;
    block.isFree = false;
    
    m_allocatedBlocks[block.handle] = block;
    m_blockTypes[block.handle] = type;
    m_totalAllocated += size;
    
    return block.handle;
}

void RenderMemoryPool::Recycle(BufferHandle handle)
{
    if (handle == INVALID_GPU_HANDLE) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_allocatedBlocks.find(handle);
    if (it == m_allocatedBlocks.end()) {
        return;
    }
    
    BufferBlock block = it->second;
    block.isFree = true;
    block.handle = INVALID_GPU_HANDLE;
    
    BufferType type = m_blockTypes[handle];
    m_freeBlocks[type].push_back(block);
    
    m_totalAllocated -= block.size;
    m_allocatedBlocks.erase(it);
    m_blockTypes.erase(handle);
}

void RenderMemoryPool::RecycleAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_allocatedBlocks.clear();
    m_blockTypes.clear();
    m_freeBlocks.clear();
    m_totalAllocated = 0;
}

void RenderMemoryPool::Compact()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& pair : m_freeBlocks) {
        auto& freeList = pair.second;
        
        if (freeList.size() < 2) {
            continue;
        }
        
        size_t totalSize = 0;
        for (const auto& block : freeList) {
            totalSize += block.size;
        }
        
        freeList.clear();
        
        BufferBlock merged;
        merged.handle = INVALID_GPU_HANDLE;
        merged.offset = 0;
        merged.size = totalSize;
        merged.isFree = true;
        freeList.push_back(merged);
    }
}

void RenderMemoryPool::Defragment()
{
    Compact();
}

size_t RenderMemoryPool::GetMemoryUsage() const
{
    return m_totalAllocated;
}

size_t RenderMemoryPool::GetPoolCapacity() const
{
    return m_maxPoolSize;
}

float RenderMemoryPool::GetFragmentationRatio() const
{
    return CalculateFragmentation();
}

MemoryPoolStats RenderMemoryPool::GetStats() const
{
    MemoryPoolStats stats;
    stats.totalAllocated = m_totalAllocated;
    stats.totalCapacity = m_maxPoolSize;
    stats.fragmentationRatio = CalculateFragmentation();
    
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));
    
    for (const auto& pair : m_freeBlocks) {
        stats.freeBlockCount += pair.second.size();
    }
    
    stats.usedBlockCount = m_allocatedBlocks.size();
    
    for (const auto& pair : m_blockTypes) {
        switch (pair.second) {
            case BufferType::kVertex:
                stats.vertexBufferCount++;
                break;
            case BufferType::kIndex:
                stats.indexBufferCount++;
                break;
            case BufferType::kUniform:
                stats.uniformBufferCount++;
                break;
        }
    }
    
    return stats;
}

void RenderMemoryPool::SetMaxPoolSize(size_t maxSize)
{
    m_maxPoolSize = maxSize;
}

void RenderMemoryPool::SetGrowthFactor(float factor)
{
    if (factor >= 1.0f) {
        m_growthFactor = factor;
    }
}

void RenderMemoryPool::SetAvailable(bool available)
{
    m_available = available;
}

bool RenderMemoryPool::IsAvailable() const
{
    return m_available;
}

float RenderMemoryPool::CalculateFragmentation() const
{
    size_t totalFreeSize = 0;
    size_t freeBlockCount = 0;
    
    for (const auto& pair : m_freeBlocks) {
        for (const auto& block : pair.second) {
            if (block.isFree) {
                totalFreeSize += block.size;
                freeBlockCount++;
            }
        }
    }
    
    if (totalFreeSize == 0 || freeBlockCount <= 1) {
        return 0.0f;
    }
    
    size_t maxBlockSize = 0;
    for (const auto& pair : m_freeBlocks) {
        for (const auto& block : pair.second) {
            if (block.isFree && block.size > maxBlockSize) {
                maxBlockSize = block.size;
            }
        }
    }
    
    if (maxBlockSize == 0) {
        return 0.0f;
    }
    
    return 1.0f - (static_cast<float>(maxBlockSize) / static_cast<float>(totalFreeSize));
}

}
}
