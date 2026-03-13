#include <gtest/gtest.h>
#include "../src/utils/logger.h"
#include <fstream>

class LoggerMinimalTest : public ::testing::Test {
protected:
    void SetUp() override {
        cycle::LogConfig config;
        config.file = "test_log_minimal.txt";
        config.level = 1;
        config.console_output = false;
        cycle::Logger::Init(config);
    }
    
    void TearDown() override {
        cycle::Logger::Shutdown();
    }
};

TEST_F(LoggerMinimalTest, BasicLogging) {
    cycle::Logger::Info("Basic test message");
    
    std::ifstream file("test_log_minimal.txt");
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(content.find("Basic test message") != std::string::npos);
    EXPECT_TRUE(content.find("INFO") != std::string::npos);
}

TEST_F(LoggerMinimalTest, LogLevelFiltering) {
    cycle::Logger::SetLevel(cycle::LogLevel::WARN);
    
    cycle::Logger::Debug("Debug message");
    cycle::Logger::Info("Info message");
    cycle::Logger::Warn("Warn message");
    cycle::Logger::Error("Error message");
    
    std::ifstream file("test_log_minimal.txt");
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(content.find("Warn") != std::string::npos);
    EXPECT_TRUE(content.find("Error") != std::string::npos);
    EXPECT_TRUE(content.find("Debug") == std::string::npos);
    EXPECT_TRUE(content.find("Info") == std::string::npos);
}

TEST_F(LoggerMinimalTest, ThreadSafety) {
    auto logTask = [](int threadId) {
        for (int i = 0; i < 5; ++i) {
            cycle::Logger::Info("Thread " + std::to_string(threadId) + " msg " + std::to_string(i));
        }
    };
    
    std::thread t1(logTask, 1);
    std::thread t2(logTask, 2);
    
    t1.join();
    t2.join();
    
    std::ifstream file("test_log_minimal.txt");
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(content.find("Thread 1") != std::string::npos);
    EXPECT_TRUE(content.find("Thread 2") != std::string::npos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
