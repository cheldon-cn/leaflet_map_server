#include <gtest/gtest.h>
#include "../src/config/config.h"
#include <fstream>

class ConfigMinimalTest : public ::testing::Test {
protected:
    void TearDown() override {
    }
};

TEST_F(ConfigMinimalTest, LoadFromString) {
    cycle::Config config;
    std::string json = R"({
        "server": {
            "host": "localhost",
            "port": 9090,
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
    
    EXPECT_TRUE(config.LoadFromString(json));
    EXPECT_EQ(config.server.host, "localhost");
    EXPECT_EQ(config.server.port, 9090);
    EXPECT_EQ(config.server.thread_count, 4);
}

TEST_F(ConfigMinimalTest, DefaultValues) {
    cycle::Config config;
    
    EXPECT_EQ(config.server.host, "127.0.0.1");
    EXPECT_EQ(config.server.port, 8080);
    EXPECT_EQ(config.server.thread_count, 4);
}

TEST_F(ConfigMinimalTest, ValidateBoundingBox) {
    cycle::Config config;
    cycle::BoundingBox bbox(0, 0, 180, 90);
    
    EXPECT_TRUE(config.range_limits.ValidateBoundingBox(bbox));
}

TEST_F(ConfigMinimalTest, ValidateInvalidBoundingBox) {
    cycle::Config config;
    cycle::BoundingBox bbox(-200, 0, 10, 10);
    
    EXPECT_FALSE(config.range_limits.ValidateBoundingBox(bbox));
}
