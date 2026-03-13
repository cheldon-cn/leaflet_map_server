#include <gtest/gtest.h>
#include "../src/service/map_service.h"
#include "../src/renderer/renderer.h"
#include "../src/database/sqlite_database.h"
#include "../src/encoder/png_encoder.h"
#include "../src/cache/memory_cache.h"
#include "../src/config/config.h"

class MapServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试组件
        database = std::make_shared<cycle::database::SqliteDatabase>(":memory:");
        database->Open();
        
        encoder = std::make_shared<cycle::encoder::PngEncoder>();
        encoder->Initialize();
        
        cache = std::make_shared<cycle::cache::MemoryCache>(128);
        
        renderer = std::make_shared<cycle::renderer::Renderer>(database, encoder, cache);
        
        // 创建配置
        config.server.thread_count = 4;
        config.range_limit.enabled = true;
        config.range_limit.max_zoom = 18;
        config.range_limit.min_x = -180;
        config.range_limit.min_y = -90;
        config.range_limit.max_x = 180;
        config.range_limit.max_y = 90;
        config.range_limit.max_width = 4096;
        config.range_limit.max_height = 4096;
        
        // 创建地图服务
        service = std::make_shared<cycle::service::MapService>(renderer, cache, config);
    }
    
    std::shared_ptr<cycle::database::SqliteDatabase> database;
    std::shared_ptr<cycle::encoder::PngEncoder> encoder;
    std::shared_ptr<cycle::cache::MemoryCache> cache;
    std::shared_ptr<cycle::renderer::Renderer> renderer;
    cycle::Config config;
    std::shared_ptr<cycle::service::MapService> service;
};

TEST_F(MapServiceTest, TileRequest) {
    // 测试有效的瓦片请求
    auto result = service->GetTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.image_data.empty());
    EXPECT_FALSE(result.from_cache);
    EXPECT_GT(result.processing_time.count(), 0);
    
    // 测试缓存
    auto cachedResult = service->GetTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_TRUE(cachedResult.success);
    EXPECT_TRUE(cachedResult.from_cache);
}

TEST_F(MapServiceTest, MapRequest) {
    cycle::service::MapRequest request;
    request.bbox = {0, 0, 100, 100};
    request.width = 256;
    request.height = 256;
    request.format = cycle::encoder::ImageFormat::PNG32;
    request.background_color = "#ffffff";
    request.layers = {"default"};
    
    auto result = service->GenerateMap(request);
    
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.image_data.empty());
    EXPECT_FALSE(result.from_cache);
    EXPECT_GT(result.processing_time.count(), 0);
}

TEST_F(MapServiceTest, RequestValidation) {
    // 测试有效的瓦片坐标
    EXPECT_TRUE(service->ValidateTileRequest(0, 0, 0));
    EXPECT_TRUE(service->ValidateTileRequest(10, 512, 512));
    
    // 测试无效的瓦片坐标
    EXPECT_FALSE(service->ValidateTileRequest(-1, 0, 0));
    EXPECT_FALSE(service->ValidateTileRequest(19, 0, 0)); // 超过最大缩放级别
    EXPECT_FALSE(service->ValidateTileRequest(1, 2, 2)); // 无效的瓦片坐标
    
    // 测试地图请求验证
    cycle::service::MapRequest validRequest;
    validRequest.bbox = {-180, -90, 180, 90};
    validRequest.width = 800;
    validRequest.height = 600;
    validRequest.dpi = 96;
    validRequest.layers = {"default"};
    
    EXPECT_TRUE(service->ValidateMapRequest(validRequest));
    
    // 测试无效的地图请求
    cycle::service::MapRequest invalidRequest;
    invalidRequest.bbox = {-200, -100, 200, 100}; // 超出范围
    invalidRequest.width = 5000; // 超过最大宽度
    invalidRequest.height = 5000; // 超过最大高度
    
    EXPECT_FALSE(service->ValidateMapRequest(invalidRequest));
}

TEST_F(MapServiceTest, RateLimiting) {
    // 测试速率限制
    config.range_limit.enabled = true;
    
    // 快速发送多个请求
    int successCount = 0;
    for (int i = 0; i < 10; ++i) {
        auto result = service->GetTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
        if (result.success) {
            successCount++;
        }
    }
    
    // 由于速率限制，部分请求应该失败
    EXPECT_LT(successCount, 10);
    
    // 禁用速率限制后应该全部成功
    config.range_limit.enabled = false;
    service->SetCache(cache);
    
    successCount = 0;
    for (int i = 0; i < 10; ++i) {
        auto result = service->GetTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
        if (result.success) {
            successCount++;
        }
    }
    
    EXPECT_EQ(successCount, 10);
}

TEST_F(MapServiceTest, MetricsCollection) {
    // 测试指标收集
    auto initialMetrics = service->GetMetrics();
    
    // 执行一些操作
    service->GetTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    service->GetTile(1, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    
    auto metrics = service->GetMetrics();
    
    EXPECT_EQ(metrics.total_requests, 2);
    EXPECT_GT(metrics.total_processing_time_ms, 0);
    
    // 测试重置指标
    service->ResetMetrics();
    auto resetMetrics = service->GetMetrics();
    
    EXPECT_EQ(resetMetrics.total_requests, 0);
    EXPECT_EQ(resetMetrics.total_processing_time_ms, 0);
}

TEST_F(MapServiceTest, ErrorHandling) {
    // 测试无效的渲染器
    auto invalidService = std::make_shared<cycle::service::MapService>(nullptr, cache, config);
    
    auto result = invalidService->GetTile(0, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
    
    // 测试无效的缓存
    auto noCacheService = std::make_shared<cycle::service::MapService>(renderer, nullptr, config);
    
    cycle::service::MapRequest request;
    request.bbox = {0, 0, 100, 100};
    request.width = 256;
    request.height = 256;
    
    auto mapResult = noCacheService->GenerateMap(request);
    EXPECT_FALSE(mapResult.success);
}

TEST_F(MapServiceTest, Performance) {
    const int numRequests = 5;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numRequests; ++i) {
        auto result = service->GetTile(i % 3, 0, 0, cycle::encoder::ImageFormat::PNG32, 96);
        EXPECT_TRUE(result.success);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 3000); // 5次请求应在3秒内完成
    
    // 验证缓存效果
    auto metrics = service->GetMetrics();
    EXPECT_GT(metrics.GetHitRate(), 0.0);
}

TEST_F(MapServiceTest, ConcurrentAccess) {
    // 测试并发访问
    auto worker = [this](int id) {
        for (int i = 0; i < 5; ++i) {
            auto result = service->GetTile(id, i % 2, i % 2, cycle::encoder::ImageFormat::PNG32, 96);
            EXPECT_TRUE(result.success);
        }
    };
    
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);
    
    t1.join();
    t2.join();
    t3.join();
    
    // 验证没有数据竞争或死锁
    auto metrics = service->GetMetrics();
    EXPECT_EQ(metrics.total_requests, 15);
}

TEST(MapServiceIntegrationTest, EndToEnd) {
    // 端到端集成测试
    auto database = std::make_shared<cycle::database::SqliteDatabase>(":memory:");
    database->Open();
    
    auto encoder = std::make_shared<cycle::encoder::PngEncoder>();
    encoder->Initialize();
    
    auto cache = std::make_shared<cycle::cache::MemoryCache>(256);
    
    auto renderer = std::make_shared<cycle::renderer::Renderer>(database, encoder, cache);
    
    cycle::Config config;
    config.server.thread_count = 4;
    
    auto service = std::make_shared<cycle::service::MapService>(renderer, cache, config);
    
    // 测试瓦片服务
    auto tileResult = service->GetTile(10, 512, 512, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_TRUE(tileResult.success);
    
    // 测试地图服务
    cycle::service::MapRequest mapRequest;
    mapRequest.bbox = {-180, -90, 180, 90};
    mapRequest.width = 800;
    mapRequest.height = 600;
    mapRequest.layers = {"default"};
    
    auto mapResult = service->GenerateMap(mapRequest);
    EXPECT_TRUE(mapResult.success);
    
    // 验证缓存
    auto cachedTileResult = service->GetTile(10, 512, 512, cycle::encoder::ImageFormat::PNG32, 96);
    EXPECT_TRUE(cachedTileResult.from_cache);
}