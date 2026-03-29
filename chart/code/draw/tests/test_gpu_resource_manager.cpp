#include <gtest/gtest.h>
#include <ogc/draw/gpu_resource_manager.h>

class GPUResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_manager = &ogc::draw::GPUResourceManager::Instance();
        m_manager->ReleaseAll();
        m_manager->SetAvailable(true);
    }
    
    void TearDown() override {
        m_manager->ReleaseAll();
    }
    
    ogc::draw::GPUResourceManager* m_manager;
};

TEST_F(GPUResourceManagerTest, Instance) {
    ogc::draw::GPUResourceManager& instance1 = ogc::draw::GPUResourceManager::Instance();
    ogc::draw::GPUResourceManager& instance2 = ogc::draw::GPUResourceManager::Instance();
    
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(GPUResourceManagerTest, CreateVertexBuffer) {
    ogc::draw::VertexBuffer buffer = m_manager->CreateVertexBuffer(1024);
    
    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.GetSize(), 1024);
    EXPECT_GT(m_manager->GetResourceCount(), 0);
}

TEST_F(GPUResourceManagerTest, CreateIndexBuffer) {
    ogc::draw::IndexBuffer buffer = m_manager->CreateIndexBuffer(512);
    
    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.GetSize(), 512);
}

TEST_F(GPUResourceManagerTest, CreateUniformBuffer) {
    ogc::draw::UniformBuffer buffer = m_manager->CreateUniformBuffer(256);
    
    EXPECT_TRUE(buffer.IsValid());
    EXPECT_EQ(buffer.GetSize(), 256);
}

TEST_F(GPUResourceManagerTest, CreateTexture2D) {
    ogc::draw::Texture texture = m_manager->CreateTexture2D(256, 256, ogc::draw::TextureFormat::kRGBA8);
    
    EXPECT_TRUE(texture.IsValid());
    EXPECT_EQ(texture.GetWidth(), 256);
    EXPECT_EQ(texture.GetHeight(), 256);
}

TEST_F(GPUResourceManagerTest, CreateTexture2DFromImage) {
    std::vector<uint8_t> pixels(64 * 64 * 4, 255);
    ogc::draw::Texture texture = m_manager->CreateTexture2DFromImage(pixels.data(), 64, 64, 4);
    
    EXPECT_TRUE(texture.IsValid());
    EXPECT_EQ(texture.GetWidth(), 64);
    EXPECT_EQ(texture.GetHeight(), 64);
}

TEST_F(GPUResourceManagerTest, CreateShader) {
    const std::string vertexSource = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";
    
    ogc::draw::Shader shader = m_manager->CreateShader(ogc::draw::ShaderType::kVertex, vertexSource);
    
    EXPECT_TRUE(shader.IsValid());
    EXPECT_EQ(shader.GetType(), ogc::draw::ShaderType::kVertex);
}

TEST_F(GPUResourceManagerTest, CreateShaderProgram) {
    const std::string vertexSource = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";
    
    const std::string fragmentSource = 
        "#version 330 core\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    
    ogc::draw::ShaderProgram program = m_manager->CreateShaderProgram(vertexSource, fragmentSource);
    
    EXPECT_TRUE(program.IsValid());
}

TEST_F(GPUResourceManagerTest, CreateFramebuffer) {
    ogc::draw::Framebuffer fbo = m_manager->CreateFramebuffer(800, 600, true, false);
    
    EXPECT_TRUE(fbo.IsValid());
    EXPECT_EQ(fbo.GetWidth(), 800);
    EXPECT_EQ(fbo.GetHeight(), 600);
}

TEST_F(GPUResourceManagerTest, ResourceReset) {
    ogc::draw::VertexBuffer buffer = m_manager->CreateVertexBuffer(1024);
    EXPECT_TRUE(buffer.IsValid());
    
    buffer.Reset();
    EXPECT_FALSE(buffer.IsValid());
}

TEST_F(GPUResourceManagerTest, ResourceMove) {
    ogc::draw::VertexBuffer buffer1 = m_manager->CreateVertexBuffer(1024);
    EXPECT_TRUE(buffer1.IsValid());
    
    ogc::draw::VertexBuffer buffer2 = std::move(buffer1);
    
    EXPECT_FALSE(buffer1.IsValid());
    EXPECT_TRUE(buffer2.IsValid());
    EXPECT_EQ(buffer2.GetSize(), 1024);
}

TEST_F(GPUResourceManagerTest, ReleaseAll) {
    m_manager->SetAvailable(true);
    ogc::draw::VertexBuffer vb = m_manager->CreateVertexBuffer(1024);
    ogc::draw::IndexBuffer ib = m_manager->CreateIndexBuffer(512);
    ogc::draw::Texture tex = m_manager->CreateTexture2D(256, 256, ogc::draw::TextureFormat::kRGBA8);
    
    EXPECT_GT(m_manager->GetResourceCount(), 0);
    
    m_manager->ReleaseAll();
    
    EXPECT_EQ(m_manager->GetResourceCount(), 0);
    EXPECT_EQ(m_manager->GetTotalMemoryUsage(), 0);
}

TEST_F(GPUResourceManagerTest, GetStats) {
    m_manager->SetAvailable(true);
    ogc::draw::VertexBuffer vb = m_manager->CreateVertexBuffer(1024);
    ogc::draw::IndexBuffer ib = m_manager->CreateIndexBuffer(512);
    ogc::draw::UniformBuffer ub = m_manager->CreateUniformBuffer(256);
    ogc::draw::Texture tex = m_manager->CreateTexture2D(128, 128, ogc::draw::TextureFormat::kRGBA8);
    
    ogc::draw::GPUResourceStats stats = m_manager->GetStats();
    
    EXPECT_GT(stats.totalResourceCount, 0);
    EXPECT_GT(stats.totalMemory, 0);
}

TEST_F(GPUResourceManagerTest, MaxMemory) {
    m_manager->SetMaxMemory(1024);
    
    ogc::draw::VertexBuffer buffer1 = m_manager->CreateVertexBuffer(512);
    EXPECT_TRUE(buffer1.IsValid());
    
    ogc::draw::VertexBuffer buffer2 = m_manager->CreateVertexBuffer(512);
    EXPECT_TRUE(buffer2.IsValid());
    
    ogc::draw::VertexBuffer buffer3 = m_manager->CreateVertexBuffer(512);
    EXPECT_FALSE(buffer3.IsValid());
    
    m_manager->SetMaxMemory(1024 * 1024 * 1024);
}

TEST_F(GPUResourceManagerTest, UpdateBuffer) {
    ogc::draw::VertexBuffer buffer = m_manager->CreateVertexBuffer(1024);
    EXPECT_TRUE(buffer.IsValid());
    
    std::vector<float> data(256, 1.0f);
    m_manager->UpdateBuffer(buffer, data.data(), data.size() * sizeof(float));
    
    EXPECT_TRUE(buffer.IsValid());
}

TEST_F(GPUResourceManagerTest, UpdateTexture) {
    ogc::draw::Texture texture = m_manager->CreateTexture2D(64, 64, ogc::draw::TextureFormat::kRGBA8);
    EXPECT_TRUE(texture.IsValid());
    
    std::vector<uint8_t> data(64 * 64 * 4, 255);
    m_manager->UpdateTexture(texture, data.data());
    
    EXPECT_TRUE(texture.IsValid());
}

TEST_F(GPUResourceManagerTest, Availability) {
    m_manager->SetAvailable(false);
    EXPECT_FALSE(m_manager->IsAvailable());
    
    ogc::draw::VertexBuffer buffer = m_manager->CreateVertexBuffer(1024);
    EXPECT_FALSE(buffer.IsValid());
    
    m_manager->SetAvailable(true);
    EXPECT_TRUE(m_manager->IsAvailable());
}
