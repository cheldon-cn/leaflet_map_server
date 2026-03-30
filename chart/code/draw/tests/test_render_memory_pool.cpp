#include <gtest/gtest.h>
#include <ogc/draw/render_memory_pool.h>

class RenderMemoryPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_pool = &ogc::draw::RenderMemoryPool::Instance();
        m_pool->RecycleAll();
        m_pool->SetAvailable(true);
        m_pool->SetMaxPoolSize(1024 * 1024 * 512);
    }
    
    void TearDown() override {
        m_pool->RecycleAll();
    }
    
    ogc::draw::RenderMemoryPool* m_pool;
};

TEST_F(RenderMemoryPoolTest, Instance) {
    ogc::draw::RenderMemoryPool& instance1 = ogc::draw::RenderMemoryPool::Instance();
    ogc::draw::RenderMemoryPool& instance2 = ogc::draw::RenderMemoryPool::Instance();
    
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(RenderMemoryPoolTest, AllocateVertexBuffer) {
    ogc::draw::BufferHandle handle = m_pool->AllocateVertexBuffer(1024);
    
    EXPECT_NE(handle, ogc::draw::INVALID_GPU_HANDLE);
    EXPECT_GT(m_pool->GetMemoryUsage(), 0);
}

TEST_F(RenderMemoryPoolTest, AllocateIndexBuffer) {
    ogc::draw::BufferHandle handle = m_pool->AllocateIndexBuffer(512);
    
    EXPECT_NE(handle, ogc::draw::INVALID_GPU_HANDLE);
}

TEST_F(RenderMemoryPoolTest, AllocateUniformBuffer) {
    ogc::draw::BufferHandle handle = m_pool->AllocateUniformBuffer(256);
    
    EXPECT_NE(handle, ogc::draw::INVALID_GPU_HANDLE);
}

TEST_F(RenderMemoryPoolTest, Recycle) {
    ogc::draw::BufferHandle handle = m_pool->AllocateVertexBuffer(1024);
    EXPECT_NE(handle, ogc::draw::INVALID_GPU_HANDLE);
    
    size_t usageBefore = m_pool->GetMemoryUsage();
    EXPECT_GT(usageBefore, 0);
    
    m_pool->Recycle(handle);
    
    ogc::draw::MemoryPoolStats stats = m_pool->GetStats();
    EXPECT_EQ(stats.usedBlockCount, 0);
}

TEST_F(RenderMemoryPoolTest, RecycleAll) {
    m_pool->AllocateVertexBuffer(1024);
    m_pool->AllocateIndexBuffer(512);
    m_pool->AllocateUniformBuffer(256);
    
    EXPECT_GT(m_pool->GetMemoryUsage(), 0);
    
    m_pool->RecycleAll();
    
    EXPECT_EQ(m_pool->GetMemoryUsage(), 0);
}

TEST_F(RenderMemoryPoolTest, MaxPoolSize) {
    m_pool->RecycleAll();
    m_pool->SetMaxPoolSize(1024);
    
    ogc::draw::BufferHandle handle1 = m_pool->AllocateVertexBuffer(512);
    EXPECT_NE(handle1, ogc::draw::INVALID_GPU_HANDLE);
    
    ogc::draw::BufferHandle handle2 = m_pool->AllocateVertexBuffer(256);
    EXPECT_NE(handle2, ogc::draw::INVALID_GPU_HANDLE);
    
    ogc::draw::BufferHandle handle3 = m_pool->AllocateVertexBuffer(512);
    EXPECT_EQ(handle3, ogc::draw::INVALID_GPU_HANDLE);
    
    m_pool->Recycle(handle1);
    m_pool->Recycle(handle2);
    m_pool->SetMaxPoolSize(1024 * 1024 * 512);
}

TEST_F(RenderMemoryPoolTest, GrowthFactor) {
    m_pool->SetGrowthFactor(2.0f);
    
    ogc::draw::BufferHandle handle = m_pool->AllocateVertexBuffer(1024);
    EXPECT_NE(handle, ogc::draw::INVALID_GPU_HANDLE);
    
    m_pool->SetGrowthFactor(1.5f);
}

TEST_F(RenderMemoryPoolTest, Compact) {
    ogc::draw::BufferHandle handle1 = m_pool->AllocateVertexBuffer(1024);
    ogc::draw::BufferHandle handle2 = m_pool->AllocateVertexBuffer(2048);
    
    m_pool->Recycle(handle1);
    m_pool->Recycle(handle2);
    
    m_pool->Compact();
    
    ogc::draw::MemoryPoolStats stats = m_pool->GetStats();
    EXPECT_GE(stats.freeBlockCount, 1);
}

TEST_F(RenderMemoryPoolTest, Defragment) {
    ogc::draw::BufferHandle handle1 = m_pool->AllocateVertexBuffer(1024);
    ogc::draw::BufferHandle handle2 = m_pool->AllocateVertexBuffer(2048);
    ogc::draw::BufferHandle handle3 = m_pool->AllocateVertexBuffer(512);
    
    m_pool->Recycle(handle1);
    m_pool->Recycle(handle3);
    
    m_pool->Defragment();
    
    EXPECT_GE(m_pool->GetFragmentationRatio(), 0);
}

TEST_F(RenderMemoryPoolTest, GetStats) {
    m_pool->AllocateVertexBuffer(1024);
    m_pool->AllocateIndexBuffer(512);
    m_pool->AllocateUniformBuffer(256);
    
    ogc::draw::MemoryPoolStats stats = m_pool->GetStats();
    
    EXPECT_GT(stats.totalAllocated, 0);
    EXPECT_EQ(stats.vertexBufferCount, 1);
    EXPECT_EQ(stats.indexBufferCount, 1);
    EXPECT_EQ(stats.uniformBufferCount, 1);
}

TEST_F(RenderMemoryPoolTest, FragmentationRatio) {
    m_pool->AllocateVertexBuffer(1024);
    
    float ratio = m_pool->GetFragmentationRatio();
    EXPECT_GE(ratio, 0);
    EXPECT_LE(ratio, 1);
}

TEST_F(RenderMemoryPoolTest, Availability) {
    m_pool->SetAvailable(false);
    EXPECT_FALSE(m_pool->IsAvailable());
    
    ogc::draw::BufferHandle handle = m_pool->AllocateVertexBuffer(1024);
    EXPECT_EQ(handle, ogc::draw::INVALID_GPU_HANDLE);
    
    m_pool->SetAvailable(true);
    EXPECT_TRUE(m_pool->IsAvailable());
}

TEST_F(RenderMemoryPoolTest, ReuseFreeBlock) {
    ogc::draw::BufferHandle handle1 = m_pool->AllocateVertexBuffer(1024);
    EXPECT_NE(handle1, ogc::draw::INVALID_GPU_HANDLE);
    
    m_pool->Recycle(handle1);
    
    ogc::draw::BufferHandle handle2 = m_pool->AllocateVertexBuffer(512);
    EXPECT_NE(handle2, ogc::draw::INVALID_GPU_HANDLE);
}

TEST_F(RenderMemoryPoolTest, PoolCapacity) {
    size_t capacity = m_pool->GetPoolCapacity();
    EXPECT_GT(capacity, 0);
    
    m_pool->SetMaxPoolSize(1024 * 1024);
    EXPECT_EQ(m_pool->GetPoolCapacity(), 1024 * 1024);
}
