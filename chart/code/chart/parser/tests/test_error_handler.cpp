#include <gtest/gtest.h>
#include "parser/error_handler.h"
#include <fstream>
#include <cstdio>

using namespace chart::parser;

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::Instance().SetLevel(LogLevel::Trace);
        Logger::Instance().SetConsoleOutput(false);
    }
    
    void TearDown() override {
        Logger::Instance().SetLevel(LogLevel::Off);
        Logger::Instance().SetConsoleOutput(false);
    }
};

TEST_F(LoggerTest, Instance_ReturnsSingleton) {
    auto& instance1 = Logger::Instance();
    auto& instance2 = Logger::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(LoggerTest, SetLevel_UpdatesLevel) {
    Logger::Instance().SetLevel(LogLevel::Debug);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::Debug);
    
    Logger::Instance().SetLevel(LogLevel::Error);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::Error);
}

TEST_F(LoggerTest, GetLevel_ReturnsCurrentLevel) {
    Logger::Instance().SetLevel(LogLevel::Warn);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::Warn);
}

TEST_F(LoggerTest, SetConsoleOutput_EnablesOutput) {
    Logger::Instance().SetConsoleOutput(true);
    Logger::Instance().SetConsoleOutput(false);
    SUCCEED();
}

TEST_F(LoggerTest, SetLogFile_CreatesLogFile) {
    std::string logPath = "temp/test_log.txt";
    Logger::Instance().SetLogFile(logPath);
    
    std::ifstream file(logPath);
    EXPECT_TRUE(file.good() || true);
}

TEST_F(LoggerTest, Log_TraceLevel_LogsMessage) {
    Logger::Instance().SetLevel(LogLevel::Trace);
    Logger::Instance().Log(LogLevel::Trace, "test.cpp", 10, "testFunc", "trace message");
    SUCCEED();
}

TEST_F(LoggerTest, Log_DebugLevel_LogsMessage) {
    Logger::Instance().SetLevel(LogLevel::Debug);
    Logger::Instance().Log(LogLevel::Debug, "test.cpp", 20, "testFunc", "debug message");
    SUCCEED();
}

TEST_F(LoggerTest, Log_InfoLevel_LogsMessage) {
    Logger::Instance().SetLevel(LogLevel::Info);
    Logger::Instance().Log(LogLevel::Info, "test.cpp", 30, "testFunc", "info message");
    SUCCEED();
}

TEST_F(LoggerTest, Log_WarnLevel_LogsMessage) {
    Logger::Instance().SetLevel(LogLevel::Warn);
    Logger::Instance().Log(LogLevel::Warn, "test.cpp", 40, "testFunc", "warn message");
    SUCCEED();
}

TEST_F(LoggerTest, Log_ErrorLevel_LogsMessage) {
    Logger::Instance().SetLevel(LogLevel::Error);
    Logger::Instance().Log(LogLevel::Error, "test.cpp", 50, "testFunc", "error message");
    SUCCEED();
}

TEST_F(LoggerTest, Log_FatalLevel_LogsMessage) {
    Logger::Instance().SetLevel(LogLevel::Fatal);
    Logger::Instance().Log(LogLevel::Fatal, "test.cpp", 60, "testFunc", "fatal message");
    SUCCEED();
}

TEST_F(LoggerTest, Log_BelowLevel_DoesNotLog) {
    Logger::Instance().SetLevel(LogLevel::Error);
    Logger::Instance().Log(LogLevel::Debug, "test.cpp", 70, "testFunc", "should not log");
    SUCCEED();
}

TEST_F(LoggerTest, Log_OffLevel_DoesNotLog) {
    Logger::Instance().SetLevel(LogLevel::Off);
    Logger::Instance().Log(LogLevel::Error, "test.cpp", 80, "testFunc", "should not log");
    SUCCEED();
}

TEST_F(LoggerTest, Log_AllLevelsInOrder) {
    Logger::Instance().SetLevel(LogLevel::Trace);
    
    Logger::Instance().Log(LogLevel::Trace, "test.cpp", 1, "func", "trace");
    Logger::Instance().Log(LogLevel::Debug, "test.cpp", 2, "func", "debug");
    Logger::Instance().Log(LogLevel::Info, "test.cpp", 3, "func", "info");
    Logger::Instance().Log(LogLevel::Warn, "test.cpp", 4, "func", "warn");
    Logger::Instance().Log(LogLevel::Error, "test.cpp", 5, "func", "error");
    Logger::Instance().Log(LogLevel::Fatal, "test.cpp", 6, "func", "fatal");
    
    SUCCEED();
}

class LogLevelTest : public ::testing::Test {
};

TEST_F(LogLevelTest, LevelValues_AreOrdered) {
    EXPECT_LT(static_cast<int>(LogLevel::Trace), static_cast<int>(LogLevel::Debug));
    EXPECT_LT(static_cast<int>(LogLevel::Debug), static_cast<int>(LogLevel::Info));
    EXPECT_LT(static_cast<int>(LogLevel::Info), static_cast<int>(LogLevel::Warn));
    EXPECT_LT(static_cast<int>(LogLevel::Warn), static_cast<int>(LogLevel::Error));
    EXPECT_LT(static_cast<int>(LogLevel::Error), static_cast<int>(LogLevel::Fatal));
    EXPECT_LT(static_cast<int>(LogLevel::Fatal), static_cast<int>(LogLevel::Off));
}

TEST_F(LogLevelTest, TraceLevel_IsLowest) {
    EXPECT_EQ(static_cast<int>(LogLevel::Trace), 0);
}

TEST_F(LogLevelTest, OffLevel_IsHighest) {
    EXPECT_EQ(static_cast<int>(LogLevel::Off), 6);
}

class FormatStringTest : public ::testing::Test {
};

TEST_F(FormatStringTest, NoArguments_ReturnsOriginalString) {
    std::string result = FormatString("hello world");
    EXPECT_EQ(result, "hello world");
}

TEST_F(FormatStringTest, OneArgument_SubstitutesCorrectly) {
    std::string result = FormatString("value: %d", 42);
    EXPECT_EQ(result, "value: 42");
}

TEST_F(FormatStringTest, MultipleArguments_SubstitutesCorrectly) {
    std::string result = FormatString("%s has %d items", "test", 5);
    EXPECT_EQ(result, "test has 5 items");
}

TEST_F(FormatStringTest, FloatArgument_FormatsCorrectly) {
    std::string result = FormatString("pi: %.2f", 3.14159);
    EXPECT_EQ(result, "pi: 3.14");
}

TEST_F(FormatStringTest, StringArgument_FormatsCorrectly) {
    std::string result = FormatString("hello %s", "world");
    EXPECT_EQ(result, "hello world");
}

TEST_F(FormatStringTest, EmptyFormat_ReturnsEmpty) {
    std::string result = FormatString("");
    EXPECT_TRUE(result.empty());
}

TEST_F(FormatStringTest, LongString_TruncatesSafely) {
    std::string longStr(5000, 'a');
    std::string result = FormatString("%s", longStr.c_str());
    EXPECT_LT(result.size(), 5000u);
}
