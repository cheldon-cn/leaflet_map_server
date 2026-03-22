#include <gtest/gtest.h>
#include "ogc/draw/draw_error.h"

TEST(DrawErrorTest, DefaultConstructor) {
    ogc::draw::DrawError error;
    EXPECT_EQ(error.GetResult(), ogc::draw::DrawResult::kFailed);
    EXPECT_TRUE(error.GetMessage().empty());
}

TEST(DrawErrorTest, ParameterizedConstructor) {
    ogc::draw::DrawError error(ogc::draw::DrawResult::kInvalidParams, "Invalid parameter");
    EXPECT_EQ(error.GetResult(), ogc::draw::DrawResult::kInvalidParams);
    EXPECT_EQ(error.GetMessage(), "Invalid parameter");
}

TEST(DrawErrorTest, SetResult) {
    ogc::draw::DrawError error;
    error.SetResult(ogc::draw::DrawResult::kDeviceNotReady);
    EXPECT_EQ(error.GetResult(), ogc::draw::DrawResult::kDeviceNotReady);
}

TEST(DrawErrorTest, SetMessage) {
    ogc::draw::DrawError error;
    error.SetMessage("Test error message");
    EXPECT_EQ(error.GetMessage(), "Test error message");
}

TEST(DrawErrorTest, ToString) {
    ogc::draw::DrawError error(ogc::draw::DrawResult::kOutOfMemory, "Memory allocation failed");
    std::string str = error.ToString();
    EXPECT_FALSE(str.empty());
    EXPECT_NE(str.find("Memory allocation failed"), std::string::npos);
}

TEST(DrawErrorTest, IsSuccess) {
    ogc::draw::DrawError success = ogc::draw::DrawError::Success();
    EXPECT_TRUE(success.IsSuccess());
    EXPECT_FALSE(success.IsError());
}

TEST(DrawErrorTest, IsError) {
    ogc::draw::DrawError error = ogc::draw::DrawError::Failed("Test failure");
    EXPECT_TRUE(error.IsError());
    EXPECT_FALSE(error.IsSuccess());
}

TEST(DrawErrorTest, StaticFactoryMethods) {
    auto error1 = ogc::draw::DrawError::InvalidParams("test");
    EXPECT_EQ(error1.GetResult(), ogc::draw::DrawResult::kInvalidParams);
    
    auto error2 = ogc::draw::DrawError::DeviceNotReady("test_device");
    EXPECT_EQ(error2.GetResult(), ogc::draw::DrawResult::kDeviceNotReady);
    
    auto error3 = ogc::draw::DrawError::OutOfMemory("test_context");
    EXPECT_EQ(error3.GetResult(), ogc::draw::DrawResult::kOutOfMemory);
}

TEST(DrawErrorTest, Reset) {
    ogc::draw::DrawError error(ogc::draw::DrawResult::kInvalidParams, "test");
    error.Reset();
    EXPECT_EQ(error.GetResult(), ogc::draw::DrawResult::kFailed);
    EXPECT_TRUE(error.GetMessage().empty());
}
