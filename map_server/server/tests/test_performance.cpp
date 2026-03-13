#include <gtest/gtest.h>
#include "../src/service/map_service.h"
#include "../src/renderer/renderer.h"
#include "../src/database/sqlite_database.h"
#include "../src/encoder/png_encoder.h"
#include "../src/cache/memory_cache.h"
#include "../src/cache/disk_cache.h"
#include "../src/server/http_server.h"
#include "../src/utils/file_system.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <httplib.h>

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试组件
        database = std::make_shared<cycle::database::SqliteDatabase>(":memory:");
        database->Open();
        
        encoder = std::make_shared<cycle::encoder::PngEncoder>();
        encoder->Initialize();
        
        memoryCache = std::make_shared<cycle::cache::MemoryCache>(512); // 512MB
        
        // 创建磁盘缓存
        cycle::cache::DiskCacheConfig diskConfig;
        diskConfig.cache_dir = "./performance_cache";
        diskConfig.max_size_mb = 1024;
        diskConfig.compress = true;
        
        diskCache = std::make_shared<cycle::cache::DiskCache>(diskConfig);
        diskCache->Initialize();
        
        renderer = std::make_shared<cycle::renderer::Renderer>(database, encoder, memoryCache);
        
        config.server.thread_count = 8; // 高并发配置
        config.range_limit.enabled = false; // 禁用限制
        
        service = std::make_shared<cycle::service::MapService>(renderer, memoryCache, config);
    }
    
    void TearDown() override {
        if (cycle::utils::exists("./performance_cache")) {
            cycle::utils::remove_all("./performance_cache");
        }
    }
    
    std::shared_ptr<cycle::database::SqliteDatabase> database;