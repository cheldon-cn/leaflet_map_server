#include <gtest/gtest.h>
#include "ogc/draw/draw_result.h"

using namespace ogc::draw;

TEST(DrawResultTest, DrawResultToString) {
    EXPECT_STREQ(DrawResultToString(DrawResult::kSuccess), "Success");
    EXPECT_STREQ(DrawResultToString(DrawResult::kInvalidParameter), "Invalid Parameter");
    EXPECT_STREQ(DrawResultToString(DrawResult::kOutOfMemory), "Out Of Memory");
    EXPECT_STREQ(DrawResultToString(DrawResult::kDeviceError), "Device Error");
    EXPECT_STREQ(DrawResultToString(DrawResult::kEngineError), "Engine Error");
    EXPECT_STREQ(DrawResultToString(DrawResult::kNotImplemented), "Not Implemented");
    EXPECT_STREQ(DrawResultToString(DrawResult::kCancelled), "Cancelled");
    EXPECT_STREQ(DrawResultToString(DrawResult::kTimeout), "Timeout");
}

TEST(DrawResultTest, IsSuccess) {
    EXPECT_TRUE(IsSuccess(DrawResult::kSuccess));
    EXPECT_FALSE(IsSuccess(DrawResult::kInvalidParameter));
    EXPECT_FALSE(IsSuccess(DrawResult::kDeviceError));
}

TEST(DrawResultTest, IsError) {
    EXPECT_FALSE(IsError(DrawResult::kSuccess));
    EXPECT_TRUE(IsError(DrawResult::kInvalidParameter));
    EXPECT_TRUE(IsError(DrawResult::kDeviceError));
}

TEST(DrawErrorTest, DefaultConstructor) {
    DrawError error;
    EXPECT_EQ(error.code, DrawResult::kSuccess);
    EXPECT_TRUE(error.message.empty());
    EXPECT_TRUE(error.context.empty());
    EXPECT_TRUE(error.file.empty());
    EXPECT_EQ(error.line, 0);
}

TEST(DrawErrorTest, ParameterizedConstructor) {
    DrawError error(DrawResult::kInvalidParameter, "Test message", "Test context", "test.cpp", 10);
    
    EXPECT_EQ(error.code, DrawResult::kInvalidParameter);
    EXPECT_EQ(error.message, "Test message");
    EXPECT_EQ(error.context, "Test context");
    EXPECT_EQ(error.file, "test.cpp");
    EXPECT_EQ(error.line, 10);
}

TEST(DrawErrorTest, FromResult) {
    DrawError error = DrawError::FromResult(DrawResult::kDeviceError, "Device context");
    
    EXPECT_EQ(error.code, DrawResult::kDeviceError);
    EXPECT_EQ(error.context, "Device context");
}

TEST(DrawErrorTest, ToString) {
    DrawError error(DrawResult::kInvalidParameter, "Test message", "Test context", "test.cpp", 10);
    std::string str = error.ToString();
    
    EXPECT_TRUE(str.find("Invalid Parameter") != std::string::npos);
    EXPECT_TRUE(str.find("Test message") != std::string::npos);
    EXPECT_TRUE(str.find("Test context") != std::string::npos);
    EXPECT_TRUE(str.find("test.cpp") != std::string::npos);
    EXPECT_TRUE(str.find("10") != std::string::npos);
}

TEST(DrawErrorTest, IsSuccess) {
    DrawError error1;
    EXPECT_TRUE(error1.IsSuccess());
    
    DrawError error2(DrawResult::kInvalidParameter);
    EXPECT_FALSE(error2.IsSuccess());
}

TEST(DrawErrorTest, IsError) {
    DrawError error1;
    EXPECT_FALSE(error1.IsError());
    
    DrawError error2(DrawResult::kInvalidParameter);
    EXPECT_TRUE(error2.IsError());
}

TEST(DrawResultMacroTest, DrawReturnIfError_Success) {
    auto testFunc = []() -> DrawResult {
        DRAW_RETURN_IF_ERROR(DrawResult::kSuccess);
        return DrawResult::kSuccess;
    };
    
    EXPECT_EQ(testFunc(), DrawResult::kSuccess);
}

TEST(DrawResultMacroTest, DrawReturnIfError_Error) {
    auto testFunc = []() -> DrawResult {
        DRAW_RETURN_IF_ERROR(DrawResult::kInvalidParameter);
        return DrawResult::kSuccess;
    };
    
    EXPECT_EQ(testFunc(), DrawResult::kInvalidParameter);
}

TEST(DrawResultMacroTest, DrawReturnErrorIf_True) {
    auto testFunc = []() -> DrawResult {
        DRAW_RETURN_ERROR_IF(true, DrawResult::kInvalidParameter);
        return DrawResult::kSuccess;
    };
    
    EXPECT_EQ(testFunc(), DrawResult::kInvalidParameter);
}

TEST(DrawResultMacroTest, DrawReturnErrorIf_False) {
    auto testFunc = []() -> DrawResult {
        DRAW_RETURN_ERROR_IF(false, DrawResult::kInvalidParameter);
        return DrawResult::kSuccess;
    };
    
    EXPECT_EQ(testFunc(), DrawResult::kSuccess);
}
