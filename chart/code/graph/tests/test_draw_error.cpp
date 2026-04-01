#include <gtest/gtest.h>
#include <ogc/draw/draw_result.h>

TEST(DrawErrorTest, DefaultConstructor) {
    ogc::draw::DrawError error;
    EXPECT_EQ(error.code, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawErrorTest, ParameterizedConstructor) {
    ogc::draw::DrawError error(ogc::draw::DrawResult::kInvalidParameter, "Invalid parameter");
    EXPECT_EQ(error.code, ogc::draw::DrawResult::kInvalidParameter);
    EXPECT_EQ(error.message, "Invalid parameter");
}

TEST(DrawErrorTest, SetResult) {
    ogc::draw::DrawError error;
    error.code = ogc::draw::DrawResult::kDeviceNotReady;
    EXPECT_EQ(error.code, ogc::draw::DrawResult::kDeviceNotReady);
}

TEST(DrawErrorTest, SetMessage) {
    ogc::draw::DrawError error;
    error.message = "Test error message";
    EXPECT_EQ(error.message, "Test error message");
}

TEST(DrawErrorTest, ToString) {
    ogc::draw::DrawError error(ogc::draw::DrawResult::kOutOfMemory, "Memory allocation failed");
    std::string str = ogc::draw::GetResultDescription(error.code);
    EXPECT_FALSE(str.empty());
}

TEST(DrawErrorTest, IsSuccess) {
    ogc::draw::DrawError success;
    success.code = ogc::draw::DrawResult::kSuccess;
    EXPECT_EQ(success.code, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawErrorTest, IsError) {
    ogc::draw::DrawError error;
    error.code = ogc::draw::DrawResult::kFailed;
    EXPECT_NE(error.code, ogc::draw::DrawResult::kSuccess);
}
