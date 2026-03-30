#include <gtest/gtest.h>
#include "ogc/draw/gpu_resource_wrapper.h"
#include "ogc/draw/gpu_resource_manager.h"

using namespace ogc::draw;

class GPUResourceWrapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        GPUResourceManager::Instance().SetAvailable(true);
    }
    
    void TearDown() override {
        GPUResourceManager::Instance().SetAvailable(false);
    }
};

TEST_F(GPUResourceWrapperTest, ScopedVertexBufferCreate) {
    ScopedVertexBuffer vb(1024);
    EXPECT_TRUE(vb.IsValid());
    EXPECT_EQ(vb.GetSize(), 1024u);
}

TEST_F(GPUResourceWrapperTest, ScopedVertexBufferMove) {
    ScopedVertexBuffer vb1(1024);
    EXPECT_TRUE(vb1.IsValid());
    
    ScopedVertexBuffer vb2 = std::move(vb1);
    EXPECT_TRUE(vb2.IsValid());
    EXPECT_FALSE(vb1.IsValid());
    EXPECT_EQ(vb2.GetSize(), 1024u);
}

TEST_F(GPUResourceWrapperTest, ScopedVertexBufferReset) {
    ScopedVertexBuffer vb(1024);
    EXPECT_TRUE(vb.IsValid());
    
    vb.Reset();
    EXPECT_FALSE(vb.IsValid());
    EXPECT_EQ(vb.GetSize(), 0u);
}

TEST_F(GPUResourceWrapperTest, ScopedIndexBufferCreate) {
    ScopedIndexBuffer ib(512);
    EXPECT_TRUE(ib.IsValid());
    EXPECT_EQ(ib.GetSize(), 512u);
}

TEST_F(GPUResourceWrapperTest, ScopedIndexBufferMove) {
    ScopedIndexBuffer ib1(512);
    EXPECT_TRUE(ib1.IsValid());
    
    ScopedIndexBuffer ib2 = std::move(ib1);
    EXPECT_TRUE(ib2.IsValid());
    EXPECT_FALSE(ib1.IsValid());
    EXPECT_EQ(ib2.GetSize(), 512u);
}

TEST_F(GPUResourceWrapperTest, ScopedIndexBufferReset) {
    ScopedIndexBuffer ib(512);
    EXPECT_TRUE(ib.IsValid());
    
    ib.Reset();
    EXPECT_FALSE(ib.IsValid());
    EXPECT_EQ(ib.GetSize(), 0u);
}

TEST_F(GPUResourceWrapperTest, ScopedTextureCreate) {
    ScopedTexture tex(256, 256, TextureFormat::kRGBA8);
    EXPECT_TRUE(tex.IsValid());
    EXPECT_EQ(tex.GetWidth(), 256);
    EXPECT_EQ(tex.GetHeight(), 256);
}

TEST_F(GPUResourceWrapperTest, ScopedTextureMove) {
    ScopedTexture tex1(256, 256, TextureFormat::kRGBA8);
    EXPECT_TRUE(tex1.IsValid());
    
    ScopedTexture tex2 = std::move(tex1);
    EXPECT_TRUE(tex2.IsValid());
    EXPECT_FALSE(tex1.IsValid());
    EXPECT_EQ(tex2.GetWidth(), 256);
    EXPECT_EQ(tex2.GetHeight(), 256);
}

TEST_F(GPUResourceWrapperTest, ScopedTextureReset) {
    ScopedTexture tex(256, 256, TextureFormat::kRGBA8);
    EXPECT_TRUE(tex.IsValid());
    
    tex.Reset();
    EXPECT_FALSE(tex.IsValid());
    EXPECT_EQ(tex.GetWidth(), 0);
    EXPECT_EQ(tex.GetHeight(), 0);
}

TEST_F(GPUResourceWrapperTest, ScopedFramebufferCreate) {
    ScopedFramebuffer fb(800, 600, true, false);
    EXPECT_TRUE(fb.IsValid());
    EXPECT_EQ(fb.GetWidth(), 800);
    EXPECT_EQ(fb.GetHeight(), 600);
}

TEST_F(GPUResourceWrapperTest, ScopedFramebufferMove) {
    ScopedFramebuffer fb1(800, 600);
    EXPECT_TRUE(fb1.IsValid());
    
    ScopedFramebuffer fb2 = std::move(fb1);
    EXPECT_TRUE(fb2.IsValid());
    EXPECT_FALSE(fb1.IsValid());
    EXPECT_EQ(fb2.GetWidth(), 800);
    EXPECT_EQ(fb2.GetHeight(), 600);
}

TEST_F(GPUResourceWrapperTest, ScopedFramebufferReset) {
    ScopedFramebuffer fb(800, 600);
    EXPECT_TRUE(fb.IsValid());
    
    fb.Reset();
    EXPECT_FALSE(fb.IsValid());
    EXPECT_EQ(fb.GetWidth(), 0);
    EXPECT_EQ(fb.GetHeight(), 0);
}

TEST_F(GPUResourceWrapperTest, ScopedShaderCreate) {
    ScopedShader shader(ShaderType::kVertex, "void main() {}");
    EXPECT_TRUE(shader.IsValid());
    EXPECT_EQ(shader.GetType(), ShaderType::kVertex);
}

TEST_F(GPUResourceWrapperTest, ScopedShaderMove) {
    ScopedShader shader1(ShaderType::kVertex, "void main() {}");
    EXPECT_TRUE(shader1.IsValid());
    
    ScopedShader shader2 = std::move(shader1);
    EXPECT_TRUE(shader2.IsValid());
    EXPECT_FALSE(shader1.IsValid());
}

TEST_F(GPUResourceWrapperTest, ScopedShaderReset) {
    ScopedShader shader(ShaderType::kFragment, "void main() {}");
    EXPECT_TRUE(shader.IsValid());
    
    shader.Reset();
    EXPECT_FALSE(shader.IsValid());
}

TEST_F(GPUResourceWrapperTest, ScopedShaderProgramCreate) {
    ScopedShaderProgram program("void main() {}", "void main() {}");
    EXPECT_TRUE(program.IsValid());
}

TEST_F(GPUResourceWrapperTest, ScopedShaderProgramMove) {
    ScopedShaderProgram program1("void main() {}", "void main() {}");
    EXPECT_TRUE(program1.IsValid());
    
    ScopedShaderProgram program2 = std::move(program1);
    EXPECT_TRUE(program2.IsValid());
    EXPECT_FALSE(program1.IsValid());
}

TEST_F(GPUResourceWrapperTest, ScopedShaderProgramReset) {
    ScopedShaderProgram program("void main() {}", "void main() {}");
    EXPECT_TRUE(program.IsValid());
    
    program.Reset();
    EXPECT_FALSE(program.IsValid());
}

TEST_F(GPUResourceWrapperTest, MultipleResources) {
    ScopedVertexBuffer vb(1024);
    ScopedIndexBuffer ib(512);
    ScopedTexture tex(256, 256, TextureFormat::kRGBA8);
    ScopedFramebuffer fb(800, 600);
    
    EXPECT_TRUE(vb.IsValid());
    EXPECT_TRUE(ib.IsValid());
    EXPECT_TRUE(tex.IsValid());
    EXPECT_TRUE(fb.IsValid());
}

TEST_F(GPUResourceWrapperTest, ResourceAutoCleanup) {
    GPUHandle vbHandle;
    GPUHandle texHandle;
    
    {
        ScopedVertexBuffer vb(1024);
        vbHandle = vb.GetHandle();
        
        ScopedTexture tex(128, 128, TextureFormat::kRGBA8);
        texHandle = tex.GetHandle();
        
        EXPECT_TRUE(vb.IsValid());
        EXPECT_TRUE(tex.IsValid());
    }
    
    auto& mgr = GPUResourceManager::Instance();
    auto stats = mgr.GetStats();
    
    EXPECT_LT(stats.vertexBufferCount, 10u);
}
