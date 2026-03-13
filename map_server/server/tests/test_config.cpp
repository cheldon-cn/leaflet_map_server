#include <gtest/gtest.h>
#include "../src/config/config.h"
#include "../src/utils/file_system.h"
#include <fstream>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建临时配置文件
        std::ofstream file("test_config.json");
        file << R"({
    "server": {
        "host": "127.0.0.1",
        "port": 8080,
        "thread_count": 4
    },
    "database": {
        "type": "sqlite",
        "sqlite_file": "./test.db"
    },
    "cache": {
        "enabled": true,
        "memory_size_mb": 256
    }
})";
        file.close();
    }
    
    void TearDown() override {
        // 清理临时文件
        if (cycle::utils::exists("test_config.json")) {
            cycle::utils::remove("test_config.json");
        }
    }
    
    cycle::Config config;
};

TEST_F(ConfigTest, LoadFromFile) {
    EXPECT_TRUE(config.LoadFromFile("test_config.json"));
    EXPECT_EQ(config.server.host, "127.0.0.1");
    EXPECT_EQ(config.server.port, 8080);
    EXPECT_EQ(config.server.thread_count, 4);
}

TEST_F(ConfigTest, LoadFromString) {
    std::string json = R"({
        "server": {
            "host": "localhost",
            "port": 9090
        }
    })";
    
    EXPECT_TRUE(config.LoadFromString(json));
    EXPECT_EQ(config.server.host, "localhost");
    EXPECT_EQ(config.server.port, 9090);
}

TEST_F(ConfigTest, SaveToFile) {
    config.server.host = "0.0.0.0";
    config.server.port = 8081;
    
    EXPECT_TRUE(config.SaveToFile("test_save.json"));
    
    cycle::Config newConfig;
    EXPECT_TRUE(newConfig.LoadFromFile("test_save.json"));
    EXPECT_EQ(newConfig.server.host, "0.0.0.0");
    EXPECT_EQ(newConfig.server.port, 8081);
    
    cycle::utils::remove("test_save.json");
}

TEST_F(ConfigTest, DefaultValues) {
    cycle::Config defaultConfig;
    
    EXPECT_EQ(defaultConfig.server.host, "0.0.0.0");
    EXPECT_EQ(defaultConfig.server.port, 8080);
    EXPECT_EQ(defaultConfig.server.thread_count, 4);
    
    EXPECT_EQ(defaultConfig.database.type, "sqlite");
    EXPECT_EQ(defaultConfig.database.sqlite_file, "./data/map.db");
    
    EXPECT_TRUE(defaultConfig.cache.enabled);
    EXPECT_EQ(defaultConfig.cache.memory_size_mb, 512);
}

TEST_F(ConfigTest, InvalidJson) {
    std::string invalidJson = "{invalid json}";
    EXPECT_FALSE(config.LoadFromString(invalidJson));
}

TEST_F(ConfigTest, MissingFile) {
    EXPECT_FALSE(config.LoadFromFile("nonexistent.json"));
}

TEST_F(ConfigTest, ValidateConfig) {
    EXPECT_TRUE(config.LoadFromFile("test_config.json"));
    EXPECT_TRUE(config.Validate());
    
    // 测试无效配置
    cycle::Config invalidConfig;
    invalidConfig.server.port = -1;
    EXPECT_FALSE(invalidConfig.Validate());
}
