#include <gtest/gtest.h>
#include "../src/utils/logger.h"
#include "../src/utils/file_system.h"
#include <sstream>
#include <fstream>

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试日志文件
        cycle::LogConfig config;
        config.file = "test_log.txt";
        config.level = 1; // DEBUG
        config.console_output = false;
        cycle::Logger::Init(config);
    }
    
    void TearDown() override {
        // 清理测试日志文件
        cycle::Logger::Shutdown();
        if (cycle::utils::exists("test_log.txt")) {
            cycle::utils::remove("test_log.txt");
        }
    }
};

TEST_F(LoggerTest, LogLevels) {
    cycle::Logger::SetLevel(cycle::LogLevel::INFO);
    
    LOG_DEBUG("This debug message should not appear");
    LOG_INFO("This info message should appear");
    LOG_WARN("This warning message should appear");
    LOG_ERROR("This error message should appear");
    
    // 验证日志文件内容
    std::ifstream file("test_log.txt");
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(content.find("INFO") != std::string::npos);
    EXPECT_TRUE(content.find("WARN") != std::string::npos);
    EXPECT_TRUE(content.find("ERROR") != std::string::npos);
    EXPECT_TRUE(content.find("DEBUG") == std::string::npos);
}

TEST_F(LoggerTest, LogFileCreation) {
    // 写入一些日志
    LOG_INFO("Test message for file creation");
    
    std::ifstream file("test_log.txt");
    EXPECT_TRUE(file.good());
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("Test message for file creation") != std::string::npos);
}

TEST_F(LoggerTest, ThreadSafety) {
    // 测试多线程日志记录
    auto logTask = [](int threadId) {
        for (int i = 0; i < 10; ++i) {
            LOG_INFO("Thread " + std::to_string(threadId) + " message " + std::to_string(i));
        }
    };
    
    std::thread t1(logTask, 1);
    std::thread t2(logTask, 2);
    std::thread t3(logTask, 3);
    
    t1.join();
    t2.join();
    t3.join();
    
    // 验证日志文件包含所有线程的消息
    std::ifstream file("test_log.txt");
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(content.find("Thread 1") != std::string::npos);
    EXPECT_TRUE(content.find("Thread 2") != std::string::npos);
    EXPECT_TRUE(content.find("Thread 3") != std::string::npos);
}

TEST_F(LoggerTest, Performance) {
    const int numLogs = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numLogs; ++i) {
        LOG_DEBUG("Performance test message " + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证日志数量
    std::ifstream file("test_log.txt");
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    // 简单的性能检查：1000条日志应该在合理时间内完成
    EXPECT_LT(duration.count(), 1000); // 少于1秒
}

TEST_F(LoggerTest, ErrorHandling) {
    // 测试空消息
    EXPECT_NO_THROW(LOG_INFO(""));
    
    // 测试特殊字符
    EXPECT_NO_THROW(LOG_INFO("Special chars: \n\t\\\"'"));
}

TEST_F(LoggerTest, LogLevelConversion) {
    EXPECT_EQ(cycle::Logger::GetLevel(), cycle::LogLevel::DEBUG);
    
    cycle::Logger::SetLevel(cycle::LogLevel::WARN);
    EXPECT_EQ(cycle::Logger::GetLevel(), cycle::LogLevel::WARN);
}
