#include <gtest/gtest.h>
#include "ogc/draw/log.h"
#include <memory>

using namespace ogc::draw;

class LogTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::Instance().SetLevel(LogLevel::kDebug);
    }
    
    void TearDown() override {
        Logger::Instance().Close();
    }
};

TEST_F(LogTest, Instance) {
    EXPECT_NE(&Logger::Instance(), nullptr);
}

TEST_F(LogTest, SetLevel) {
    Logger::Instance().SetLevel(LogLevel::kDebug);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::kDebug);
    
    Logger::Instance().SetLevel(LogLevel::kInfo);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::kInfo);
    
    Logger::Instance().SetLevel(LogLevel::kWarning);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::kWarning);
    
    Logger::Instance().SetLevel(LogLevel::kError);
    EXPECT_EQ(Logger::Instance().GetLevel(), LogLevel::kError);
}

TEST_F(LogTest, Debug) {
    Logger::Instance().SetLevel(LogLevel::kDebug);
    Logger::Instance().Debug("Debug message");
}

TEST_F(LogTest, Info) {
    Logger::Instance().SetLevel(LogLevel::kInfo);
    Logger::Instance().Info("Info message");
}

TEST_F(LogTest, Warning) {
    Logger::Instance().SetLevel(LogLevel::kWarning);
    Logger::Instance().Warning("Warning message");
}

TEST_F(LogTest, Error) {
    Logger::Instance().SetLevel(LogLevel::kError);
    Logger::Instance().Error("Error message");
}

TEST_F(LogTest, Fatal) {
    Logger::Instance().SetLevel(LogLevel::kError);
    Logger::Instance().Fatal("Fatal message");
}

TEST_F(LogTest, SetLogFile) {
    Logger::Instance().SetLogFile("test_log.txt");
    Logger::Instance().Info("Test message to file");
}

TEST_F(LogTest, SetConsoleOutput) {
    Logger::Instance().SetConsoleOutput(true);
    Logger::Instance().Info("Console output enabled");
    
    Logger::Instance().SetConsoleOutput(false);
    Logger::Instance().Info("Console output disabled");
}

TEST_F(LogTest, Log) {
    Logger::Instance().SetLevel(LogLevel::kInfo);
    Logger::Instance().Log(LogLevel::kInfo, "Log message");
}

TEST_F(LogTest, Flush) {
    Logger::Instance().SetLogFile("test_flush.txt");
    Logger::Instance().Info("Before flush");
    Logger::Instance().Flush();
    Logger::Instance().Info("After flush");
}

TEST_F(LogTest, LevelToString) {
    EXPECT_EQ(Logger::LevelToString(LogLevel::kDebug), "DEBUG");
    EXPECT_EQ(Logger::LevelToString(LogLevel::kInfo), "INFO");
    EXPECT_EQ(Logger::LevelToString(LogLevel::kWarning), "WARNING");
    EXPECT_EQ(Logger::LevelToString(LogLevel::kError), "ERROR");
    EXPECT_EQ(Logger::LevelToString(LogLevel::kFatal), "FATAL");
}

TEST_F(LogTest, StringToLevel) {
    EXPECT_EQ(Logger::StringToLevel("DEBUG"), LogLevel::kDebug);
    EXPECT_EQ(Logger::StringToLevel("INFO"), LogLevel::kInfo);
    EXPECT_EQ(Logger::StringToLevel("WARNING"), LogLevel::kWarning);
    EXPECT_EQ(Logger::StringToLevel("ERROR"), LogLevel::kError);
    EXPECT_EQ(Logger::StringToLevel("FATAL"), LogLevel::kFatal);
}

TEST_F(LogTest, LogHelper) {
    Logger::Instance().SetLevel(LogLevel::kDebug);
    LOG_INFO() << "Test info message";
    LOG_DEBUG() << "Test debug message";
    LOG_WARNING() << "Test warning message";
    LOG_ERROR() << "Test error message";
}

TEST_F(LogTest, LogHelperInt) {
    Logger::Instance().SetLevel(LogLevel::kInfo);
    LOG_INFO() << "Integer value: " << 42;
}

TEST_F(LogTest, LogHelperDouble) {
    Logger::Instance().SetLevel(LogLevel::kInfo);
    LOG_INFO() << "Double value: " << 3.14;
}
