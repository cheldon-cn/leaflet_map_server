#include <gtest/gtest.h>
#include "../src/utils/config_encryptor.h"
#include "../src/utils/logger.h"
#include "../src/utils/file_system.h"
#include <fstream>

using namespace cycle;
using namespace cycle::utils;

class ConfigEncryptionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试目录和文件
        cycle::utils::create_directories("./test_temp");
        
        // 创建测试配置文件
        std::ofstream config_file("./test_temp/test_config.json");
        config_file << R"({
    "database": {
        "type": "postgresql",
        "pg_host": "localhost",
        "pg_port": 5432,
        "pg_database": "testdb",
        "pg_username": "testuser",
        "pg_password": "testpassword123"
    },
    "server": {
        "host": "0.0.0.0",
        "port": 8080,
        "jwt_secret": "supersecretkey"
    }
})";
        config_file.close();
    }
    
    void TearDown() override {
        // 清理测试文件
        cycle::utils::remove_all("./test_temp");
    }
};

TEST_F(ConfigEncryptionTest, BasicEncryptionDecryption) {
    ConfigEncryptor encryptor;
    
    // 使用开发密钥初始化
    ASSERT_TRUE(encryptor.Initialize());
    
    std::string plaintext = "This is a secret configuration value";
    
    // 加密
    std::string encrypted = encryptor.EncryptConfig(plaintext);
    ASSERT_FALSE(encrypted.empty());
    
    // 解密
    std::string decrypted = encryptor.DecryptConfig(encrypted);
    ASSERT_FALSE(decrypted.empty());
    
    // 验证解密结果
    EXPECT_EQ(plaintext, decrypted);
}

TEST_F(ConfigEncryptionTest, FileEncryptionDecryption) {
    ConfigEncryptor encryptor;
    
    ASSERT_TRUE(encryptor.Initialize());
    
    // 加密配置文件
    ASSERT_TRUE(encryptor.EncryptConfigFile(
        "./test_temp/test_config.json", 
        "./test_temp/encrypted_config.enc"
    ));
    
    // 验证加密文件存在
    ASSERT_TRUE(cycle::utils::exists("./test_temp/encrypted_config.enc"));
    
    // 解密配置文件
    ASSERT_TRUE(encryptor.DecryptConfigFile(
        "./test_temp/encrypted_config.enc",
        "./test_temp/decrypted_config.json"
    ));
    
    // 验证解密文件存在
    ASSERT_TRUE(cycle::utils::exists("./test_temp/decrypted_config.json"));
    
    // 比较原始和解密后的文件内容
    std::ifstream original("./test_temp/test_config.json");
    std::ifstream decrypted("./test_temp/decrypted_config.json");
    
    std::string original_content((std::istreambuf_iterator<char>(original)),
                                std::istreambuf_iterator<char>());
    std::string decrypted_content((std::istreambuf_iterator<char>(decrypted)),
                                 std::istreambuf_iterator<char>());
    
    EXPECT_EQ(original_content, decrypted_content);
}

TEST_F(ConfigEncryptionTest, SensitiveFieldEncryption) {
    ConfigEncryptor encryptor;
    
    ASSERT_TRUE(encryptor.Initialize());
    
    std::string password = "verysecretpassword";
    
    // 加密敏感字段
    std::string encrypted_password = encryptor.EncryptSensitiveField(
        "database.password", password
    );
    
    ASSERT_FALSE(encrypted_password.empty());
    
    // 解密敏感字段
    std::string decrypted_password = encryptor.DecryptSensitiveField(
        "database.password", encrypted_password
    );
    
    ASSERT_FALSE(decrypted_password.empty());
    
    // 验证解密结果
    EXPECT_EQ(password, decrypted_password);
}

TEST_F(ConfigEncryptionTest, MasterKeyGeneration) {
    ConfigEncryptor encryptor;
    
    // 生成主密钥
    ASSERT_TRUE(encryptor.GenerateMasterKey("./test_temp/master.key"));
    
    // 验证密钥文件存在
    ASSERT_TRUE(cycle::utils::exists("./test_temp/master.key"));
    
    // 使用生成的密钥初始化
    ASSERT_TRUE(encryptor.LoadMasterKey("./test_temp/master.key"));
    
    // 测试加密解密
    std::string plaintext = "Test data for master key";
    std::string encrypted = encryptor.EncryptConfig(plaintext);
    std::string decrypted = encryptor.DecryptConfig(encrypted);
    
    EXPECT_EQ(plaintext, decrypted);
}

TEST_F(ConfigEncryptionTest, SecureConfigManager) {
    // 测试安全配置管理器
    auto& manager = SecureConfigManager::Instance();
    
    ASSERT_TRUE(manager.Initialize("./test_temp/test_config.json"));
    
    // 测试获取配置值
    std::string db_type = manager.GetSecureConfigValue("database.type");
    EXPECT_EQ(db_type, "postgresql");
    
    std::string db_host = manager.GetSecureConfigValue("database.pg_host");
    EXPECT_EQ(db_host, "localhost");
    
    // 测试设置敏感值
    std::string secret_value = "newsecretpassword";
    ASSERT_TRUE(manager.SetSecureConfigValue("database.password", secret_value, true));
    
    // 验证敏感值是否被加密
    std::string stored_value = manager.GetSecureConfigValue("database.password");
    EXPECT_NE(stored_value, secret_value); // 应该被加密
    
    // 验证安全配置
    EXPECT_TRUE(manager.ValidateConfigSecurity());
}

TEST_F(ConfigEncryptionTest, PerformanceTest) {
    ConfigEncryptor encryptor;
    
    ASSERT_TRUE(encryptor.Initialize());
    
    // 性能测试：加密大量数据
    const int iterations = 100;
    std::string test_data = "This is a test configuration value for performance testing";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::string encrypted = encryptor.EncryptConfig(test_data);
        std::string decrypted = encryptor.DecryptConfig(encrypted);
        
        EXPECT_EQ(test_data, decrypted);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证性能在可接受范围内（100次操作应在1秒内完成）
    EXPECT_LT(duration.count(), 1000);
    
    LOG_INFO("Performance test completed in " + std::to_string(duration.count()) + "ms");
}

TEST_F(ConfigEncryptionTest, ErrorHandling) {
    ConfigEncryptor encryptor;
    
    // 测试未初始化时的错误处理
    std::string encrypted = encryptor.EncryptConfig("test");
    EXPECT_TRUE(encrypted.empty());
    
    std::string decrypted = encryptor.DecryptConfig("invalid");
    EXPECT_TRUE(decrypted.empty());
    
    // 测试无效文件路径
    ASSERT_FALSE(encryptor.EncryptConfigFile("nonexistent.json", "output.enc"));
    ASSERT_FALSE(encryptor.DecryptConfigFile("nonexistent.enc", "output.json"));
    
    // 测试无效密钥文件
    ASSERT_FALSE(encryptor.LoadMasterKey("nonexistent.key"));
}

TEST_F(ConfigEncryptionTest, SecurityValidation) {
    ConfigEncryptor encryptor;
    
    ASSERT_TRUE(encryptor.Initialize());
    
    // 测试安全擦除功能
    encryptor.SecureClean();
    
    // 验证清理后无法使用
    std::string encrypted = encryptor.EncryptConfig("test");
    EXPECT_TRUE(encrypted.empty());
    
    // 重新初始化
    ASSERT_TRUE(encryptor.Initialize());
    
    // 测试密钥轮换
    ASSERT_TRUE(encryptor.RotateMasterKey("./test_temp/new_master.key"));
    ASSERT_TRUE(cycle::utils::exists("./test_temp/new_master.key"));
}
