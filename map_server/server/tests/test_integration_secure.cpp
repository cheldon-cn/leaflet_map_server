#include <gtest/gtest.h>
#include "../src/config/config.h"
#include "../src/server/http_server.h"
#include "../src/database/sqlite_database.h"
#include "../src/encoder/png_encoder.h"
#include "../src/cache/memory_cache.h"
#include "../src/renderer/renderer.h"
#include "../src/service/map_service.h"
#include "../src/auth/jwt_auth.h"
#include "../src/utils/file_system.h"
#include <httplib.h>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

class SecureIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
        cycle::Logger::SetLogFile("secure_integration_test.log");
        cycle::Logger::SetLogLevel(cycle::LogLevel::INFO);
        
        // 创建测试配置
        config.server.host = "127.0.0.1";
        config.server.port = 8086;
        config.server.enable_https = true;
        config.server.https_port = 8446;
        config.server.ssl_cert_file = "./test_cert.pem";
        config.server.ssl_key_file = "./test_key.pem";
        
        config.database.type = "sqlite";
        config.database.sqlite_file = ":memory:";
        
        config.cache.enabled = true;
        config.cache.memory_cache_size = 64 * 1024 * 1024;
        
        config.range_limit.enabled = false;
        
        // 创建测试证书文件
        CreateTestCertificates();
        
        // 初始化服务器
        server = std::make_unique<cycle::server::HttpServer>(config);
        
        // 初始化组件
        InitializeComponents();
        
        // 启动服务器
        StartServer();
    }
    
    void TearDown() override {
        if (serverThread.joinable()) {
            server->Stop();
            serverThread.join();
        }
        
        // 清理测试证书文件
        std::remove("./test_cert.pem");
        std::remove("./test_key.pem");
        
        // 清理日志文件
        if (cycle::utils::exists("secure_integration_test.log")) {
            cycle::utils::remove("secure_integration_test.log");
        }
    }
    
    void CreateTestCertificates() {
        // 创建简化的测试证书（仅用于测试）
        std::ofstream certFile("./test_cert.pem");
        certFile << "-----BEGIN CERTIFICATE-----\n";