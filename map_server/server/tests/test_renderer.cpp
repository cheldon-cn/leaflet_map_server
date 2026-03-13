#include <gtest/gtest.h>
#include "../src/renderer/render_context.h"
#include "../src/renderer/renderer.h"
#include "../src/database/sqlite_database.h"
#include "../src/encoder/png_encoder.h"
#include "../src/cache/memory_cache.h"

class RenderContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        cycle::BoundingBox bounds = {0, 0, 100, 100};
        context = std::make_shared<cycle::renderer::RenderContext>(256, 256, bounds);
    }
    
    std::shared_ptr<cycle::renderer::RenderContext> context;
};

TEST_F(RenderContextTest, CoordinateTransformation) {
    // 测试世界坐标到屏幕坐标的转换
    auto screenPoint = context->ToScreen(50, 50);
    EXPECT_NEAR(screenPoint.x, 128, 1);
    EXPECT_NEAR(screenPoint.y, 128, 1);
    
    // 测试屏幕坐标到世界坐标的转换
    auto worldPoint = context->ToWorld(128, 128);
    EXPECT_NEAR(worldPoint.x, 50, 1);
    EXPECT_NEAR(worldPoint.y, 50, 1);
}

TEST_F(RenderContextTest, StateManagement) {
    // 测试状态保存和恢复
    cycle::Style originalStyle;
    originalStyle.fill_color = "#ff0000";
    context->SetStyle(originalStyle);
    
    context->PushState();
    
    cycle::Style newStyle;
    newStyle.fill_color = "#00ff00";
    context->SetStyle(newStyle);
    
    EXPECT_EQ(context->GetStyle().fill_color, "#00ff00");
    
    context->PopState();
    EXPECT_EQ(context->GetStyle().fill_color, "#ff0000");
}

TEST_F(RenderContextTest, Validation) {
    // 测试坐标验证
    EXPECT_TRUE(context->ValidatePoint(50, 50));
    EXPECT_FALSE(context->ValidatePoint(-10, 50));
    EXPECT_FALSE(context->ValidatePoint(150, 50));
    
    EXPECT_TRUE(context->ValidateScreenPoint(128, 128));
    EXPECT_FALSE(context->ValidateScreenPoint(-10, 128));
    EXPECT_FALSE(context->ValidateScreenPoint(300, 128));
}

class RendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试数据库
        database = std::make_shared<cycle::database::SqliteDatabase>(":memory:");
        database->Open();
        
        // 创建测试表
        database->Execute("CREATE TABLE test_features (id INTEGER, geometry BLOB)");
        
        // 创建编码器
        encoder = std::make_shared<cycle::encoder::PngEncoder>();
        encoder->Initialize();
        
        // 创建缓存
        cache = std::make_shared<cycle::cache::MemoryCache>(64);
        
        // 创建渲染器
        renderer = std::make_shared<cycle::renderer::Renderer>(database, encoder, cache);
    }
    
    std::shared_ptr<cycle::database::SqliteDatabase> database;
    std::shared_ptr<cycle::encoder::PngEncoder> encoder;
    std::shared_ptr<cycle::cache::MemoryCache> cache;
    std::shared_ptr<cycle::renderer::Renderer> renderer;
};

TEST_F(RendererTest, TileRendering) {
    // 测试瓦片渲染
    auto result = renderer->RenderTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.image_data.empty());
    EXPECT_FALSE(result.from_cache);
    
    // 测试缓存
    auto cachedResult = renderer->RenderTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_TRUE(cachedResult.success);
    EXPECT_TRUE(cachedResult.from_cache);
}

TEST_F(RendererTest, MapRendering) {
    cycle::renderer::RenderRequest request;
    request.bbox = {0, 0, 100, 100};
    request.width = 256;
    request.height = 256;
    request.format = cycle::encoder::ImageFormat::PNG32;
    request.background_color = "#ffffff";
    
    auto result = renderer->RenderMap(request);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.image_data.empty());
    EXPECT_GT(result.image_data.size(), 100); // 确保有实际图像数据
}

TEST_F(RendererTest, ErrorConditions) {
    // 测试无效的缩放级别
    auto result = renderer->RenderTile(-1, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_FALSE(result.success);
    
    // 测试无效的边界框
    cycle::renderer::RenderRequest invalidRequest;
    invalidRequest.bbox = {100, 100, 0, 0}; // 无效的边界框
    invalidRequest.width = 256;
    invalidRequest.height = 256;
    
    auto mapResult = renderer->RenderMap(invalidRequest);
    EXPECT_FALSE(mapResult.success);
}

TEST_F(RendererTest, Performance) {
    const int numRenders = 10;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numRenders; ++i) {
        auto result = renderer->RenderTile(1, i % 2, i % 2, cycle::encoder::ImageFormat::PNG32, 96);
        EXPECT_TRUE(result.success);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证渲染性能
    EXPECT_LT(duration.count(), 5000); // 10次渲染应在5秒内完成
    
    // 验证缓存命中率
    auto stats = cache->GetStats();
    EXPECT_GT(stats.GetHitRate(), 0.5); // 缓存命中率应高于50%
}

TEST(RendererIntegrationTest, FullPipeline) {
    // 测试完整的渲染流水线
    auto database = std::make_shared<cycle::database::SqliteDatabase>(":memory:");
    database->Open();
    
    // 添加测试数据
    database->Execute("CREATE TABLE buildings (id INTEGER, name TEXT, geometry BLOB)");
    
    auto encoder = std::make_shared<cycle::encoder::PngEncoder>();
    encoder->Initialize();
    
    auto cache = std::make_shared<cycle::cache::MemoryCache>(128);
    
    auto renderer = std::make_shared<cycle::renderer::Renderer>(database, encoder, cache);
    
    // 测试瓦片渲染
    auto tileResult = renderer->RenderTile(10, 512, 512, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_TRUE(tileResult.success);
    
    // 测试地图渲染
    cycle::renderer::RenderRequest mapRequest;
    mapRequest.bbox = {-180, -90, 180, 90};
    mapRequest.width = 800;
    mapRequest.height = 600;
    mapRequest.format = cycle::encoder::ImageFormat::PNG32;
    
    auto mapResult = renderer->RenderMap(mapRequest);
    EXPECT_TRUE(mapResult.success);
}