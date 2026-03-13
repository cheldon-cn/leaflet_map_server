#include <gtest/gtest.h>
#include "../src/config/config.h"
#include "../src/utils/logger.h"
#include "../src/database/database_factory.h"
#include "../src/encoder/encoder_factory.h"
#include "../src/cache/memory_cache.h"
#include "../src/renderer/renderer.h"
#include "../src/service/map_service.h"
#include "../src/server/http_server.h"
#include "../src/utils/file_system.h"
#include <thread>
#include <chrono>
#include <httplib.h>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
        cycle::Logger::SetLogFile("integration_test.log");
        cycle::Logger::SetLogLevel(cycle::LogLevel::INFO);
        
        // 创建测试配置
        config.server.host = "127.0.0.1";
        config.server.port = 8081; // 使用不同的端口避免冲突
        config.server.thread_count = 2;
        
        config.database.type = "sqlite";
        config.database.sqlite_file = ":memory:";
        
        config.cache.enabled = true;
        config.cache.memory_size_mb = 64;
        
        config.range_limit.enabled = false; // 禁用限制以简化测试
    }
    
    void TearDown() override {
        if (serverThread.joinable()) {
            server->Stop();
            serverThread.join();
        }
        
        // 清理日志文件
        if (cycle::utils::exists("integration_test.log")) {
            cycle::utils::remove("integration_test.log");
        }
    }
    
    void StartServer() {
        server = std::make_unique<cycle::server::HttpServer>(config);
        
        // 创建完整的服务链