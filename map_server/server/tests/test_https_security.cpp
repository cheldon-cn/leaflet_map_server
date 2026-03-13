#include <gtest/gtest.h>
#include "../src/server/http_server.h"
#include "../src/auth/jwt_auth.h"
#include "../src/config/config.h"
#include <httplib.h>
#include <thread>
#include <chrono>
#include <fstream>

class HTTPSecurityTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试配置
        config.server.host = "127.0.0.1";
        config.server.port = 8085;
        config.server.enable_https = true;
        config.server.https_port = 8445;
        config.server.ssl_cert_file = "./test_cert.pem";
        config.server.ssl_key_file = "./test_key.pem";
        
        // 创建测试证书和密钥文件
        CreateTestCertificates();
        
        server = std::make_unique<cycle::server::HttpServer>(config);
        
        // 初始化认证
        cycle::auth::JWTConfig jwtConfig;
        jwtConfig.secret_key = "test-secret";
        auth = std::make_shared<cycle::auth::JWTAuth>(jwtConfig);
        server->SetAuth(auth);
    }
    
    void TearDown() override {
        if (server) {
            server->Stop();
        }
        
        // 清理测试证书文件
        std::remove("./test_cert.pem");
        std::remove("./test_key.pem");
    }
    
    void CreateTestCertificates() {
        // 创建测试用的自签名证书（仅用于测试）
        std::ofstream certFile("./test_cert.pem");
        certFile << "-----BEGIN CERTIFICATE-----\n";
        certFile << "MIIDXTCCAkWgAwIBAgIJAKL7wQ8O0TBFMA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV\n";
        certFile << "BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5l\n";
        certFile << "dCBXaWRnaXRzcyBQdHkwHhcNMTUwMTAxMDAwMDAwWhcNMjAwMTE5MDAwMDAw\n";
        certFile << "WjBFMQswCQYDVQQGEwJBVTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UE\n";
        certFile << "CgwYSW50ZXJuZXQgV2lkZ2l0c3MgUHR5MIIBIjANBgkqhkiG9w0BAQEFAAOC\n";
        certFile << "AQ8AMIIBCgKCAQEA6d0Mkhv4d2j4wJgFQN1M2kQ7K1+U7J8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9\n";
        certFile << "wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7\n";
        certFile << "w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "zJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w\n";
        certFile << "9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K\n";
        certFile << "7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3\n";
        certFile << "K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8K\n";
        certFile << "j3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J\n";
        certFile << "8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L\n";
        certFile << "3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ\n";
        certFile << "8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9wzJ8L3J8Kj3K7w9w\n";
        certFile << "-----END CERTIFICATE-----\n";
        certFile.close();
        
        std::ofstream keyFile("./test_key.pem");
        keyFile << "-----BEGIN PRIVATE KEY-----\n";
        keyFile << "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDp3QySG/h3aPjA\n";
        keyFile << "mAVA3UzaRDsrX5TsnwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8n\n";
        keyFile << "wvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nw\n";
        keyFile << "vcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "cnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwvcnwqPcrvD3D8nwv\n";
        keyFile << "-----END PRIVATE KEY-----\n";
        keyFile.close();
    }
    
    cycle::Config config;
    std::unique_ptr<cycle::server::HttpServer> server;
    std::shared_ptr<cycle::auth::JWTAuth> auth;
};

TEST_F(HTTPSecurityTest, SSLConfiguration) {
    EXPECT_TRUE(server->EnableSSL(config.server.ssl_cert_file, 
                                 config.server.ssl_key_file));
    EXPECT_TRUE(server->IsSSLEnabled());
}

TEST_F(HTTPSecurityTest, SecurityHeaders) {
    // 测试安全头信息是否正确设置
    httplib::Client client(config.server.host, config.server.port);
    
    auto response = client.Get("/health");
    
    EXPECT_TRUE(response != nullptr);
    
    if (response) {
        EXPECT_EQ(response->get_header_value("X-Content-Type-Options"), "nosniff");
        EXPECT_EQ(response->get_header_value("X-Frame-Options"), "DENY");
        EXPECT_EQ(response->get_header_value("X-XSS-Protection"), "1; mode=block");
    }
}

TEST_F(HTTPSecurityTest, AuthenticationEndpoints) {
    // 测试认证端点
    httplib::Client client(config.server.host, config.server.port);
    
    // 测试登录端点
    nlohmann::json loginData;
    loginData["username"] = "admin";
    loginData["password"] = "password";
    
    auto loginResponse = client.Post("/auth/login", loginData.dump(), "application/json");
    
    EXPECT_TRUE(loginResponse != nullptr);
    EXPECT_EQ(loginResponse->status, 200);
    
    // 解析响应
    auto responseJson = nlohmann::json::parse(loginResponse->body);
    EXPECT_TRUE(responseJson.contains("token"));
    EXPECT_TRUE(responseJson.contains("user"));
}

TEST_F(HTTPSecurityTest, ProtectedResourceAccess) {
    httplib::Client client(config.server.host, config.server.port);
    
    // 先获取令牌
    nlohmann::json loginData;
    loginData["username"] = "admin";
    loginData["password"] = "password";
    
    auto loginResponse = client.Post("/auth/login", loginData.dump(), "application/json");
    auto responseJson = nlohmann::json::parse(loginResponse->body);
    std::string token = responseJson["token"];
    
    // 使用令牌访问受保护资源
    httplib::Headers headers = {{"Authorization", "Bearer " + token}};
    auto protectedResponse = client.Get("/metrics", headers);
    
    EXPECT_TRUE(protectedResponse != nullptr);
    EXPECT_EQ(protectedResponse->status, 200);
}

TEST_F(HTTPSecurityTest, UnauthorizedAccess) {
    httplib::Client client(config.server.host, config.server.port);
    
    // 尝试无令牌访问受保护资源
    auto response = client.Get("/metrics");
    
    EXPECT_TRUE(response != nullptr);
    EXPECT_EQ(response->status, 401); // 未授权
}

TEST_F(HTTPSecurityTest, TokenRefresh) {
    httplib::Client client(config.server.host, config.server.port);
    
    // 先获取令牌
    nlohmann::json loginData;
    loginData["username"] = "admin";
    loginData["password"] = "password";
    
    auto loginResponse = client.Post("/auth/login", loginData.dump(), "application/json");
    auto responseJson = nlohmann::json::parse(loginResponse->body);
    std::string token = responseJson["token"];
    
    // 刷新令牌
    httplib::Headers headers = {{"Authorization", "Bearer " + token}};
    auto refreshResponse = client.Post("/auth/refresh", "", "application/json", headers);
    
    EXPECT_TRUE(refreshResponse != nullptr);
    EXPECT_EQ(refreshResponse->status, 200);
    
    auto refreshJson = nlohmann::json::parse(refreshResponse->body);
    EXPECT_TRUE(refreshJson.contains("token"));
    EXPECT_NE(refreshJson["token"], token); // 新令牌应该不同
}

TEST_F(HTTPSecurityTest, SecurityMonitoring) {
    // 测试安全监控功能
    httplib::Client client(config.server.host, config.server.port);
    
    // 发送一些请求来测试监控
    for (int i = 0; i < 5; ++i) {
        client.Get("/health");
    }
    
    // 尝试一些无效请求
    client.Get("/invalid/path");
    client.Get("/../etc/passwd"); // 路径遍历尝试
    
    // 检查指标端点是否正常工作
    auto metricsResponse = client.Get("/metrics");
    
    EXPECT_TRUE(metricsResponse != nullptr);
    if (metricsResponse && metricsResponse->status == 200) {
        auto metricsJson = nlohmann::json::parse(metricsResponse->body);
        EXPECT_TRUE(metricsJson.contains("total_requests"));
        EXPECT_TRUE(metricsJson.contains("blocked_requests"));
    }
}

TEST_F(HTTPSecurityTest, RequestSizeLimitation) {
    httplib::Client client(config.server.host, config.server.port);
    
    // 创建超大请求体
    std::string hugeBody(11 * 1024 * 1024, 'A'); // 11MB，超过10MB限制
    
    auto response = client.Post("/auth/login", hugeBody, "application/json");
    
    EXPECT_TRUE(response != nullptr);
    EXPECT_EQ(response->status, 413); // 请求体过大
}

TEST_F(HTTPSecurityTest, PathTraversalProtection) {
    httplib::Client client(config.server.host, config.server.port);
    
    // 测试路径遍历攻击防护
    std::vector<std::string> maliciousPaths = {
        "/../config.json",
        "/../../etc/passwd", 
        "/..\\..\\windows\\system32",
        "/%2e%2e%2f%2e%2e%2fetc%2fpasswd"
    };
    
    for (const auto& path : maliciousPaths) {
        auto response = client.Get(path);
        EXPECT_TRUE(response != nullptr);
        EXPECT_EQ(response->status, 400); // 应该返回错误
    }
}

TEST_F(HTTPSecurityTest, ConcurrentSecurity) {
    const int numThreads = 10;
    std::atomic<int> successCount{0};
    std::atomic<int> errorCount{0};
    
    auto worker = [&](int threadId) {
        httplib::Client client(config.server.host, config.server.port);
        
        for (int i = 0; i < 5; ++i) {
            auto response = client.Get("/health");
            
            if (response && response->status == 200) {
                successCount++;
            } else {
                errorCount++;
            }
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 服务器应该能够处理并发请求
    EXPECT_GT(successCount, 0);
    EXPECT_LT(errorCount, numThreads * 5 * 0.1); // 错误率应低于10%
}

TEST_F(HTTPSecurityTest, UserInfoEndpoint) {
    httplib::Client client(config.server.host, config.server.port);
    
    // 先获取令牌
    nlohmann::json loginData;
    loginData["username"] = "admin";
    loginData["password"] = "password";
    
    auto loginResponse = client.Post("/auth/login", loginData.dump(), "application/json");
    auto responseJson = nlohmann::json::parse(loginResponse->body);
    std::string token = responseJson["token"];
    
    // 获取用户信息
    httplib::Headers headers = {{"Authorization", "Bearer " + token}};
    auto userResponse = client.Get("/auth/user", headers);
    
    EXPECT_TRUE(userResponse != nullptr);
    EXPECT_EQ(userResponse->status, 200);
    
    auto userJson = nlohmann::json::parse(userResponse->body);
    EXPECT_TRUE(userJson.contains("user"));
    EXPECT_EQ(userJson["user"]["username"], "admin");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "Running HTTPS and Security Tests..." << std::endl;
    std::cout << "====================================" << std::endl;
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "====================================" << std::endl;
    std::cout << "HTTPS and Security Tests Completed" << std::endl;
    
    return result;
}