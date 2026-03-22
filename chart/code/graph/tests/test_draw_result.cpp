#include <gtest/gtest.h>
#include "ogc/draw/draw_result.h"

TEST(DrawResultTest, DefaultConstructor) {
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawResultTest, AllValues) {
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kSuccess), 0);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kFailed), 1);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kInvalidParams), 2);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kDeviceNotReady), 3);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kEngineNotReady), 4);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kOutOfMemory), 5);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kFileNotFound), 6);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kUnsupportedFormat), 7);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kOperationCancelled), 8);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kTimeout), 9);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kBufferOverflow), 10);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kAccessDenied), 11);
}

TEST(DrawResultTest, GetResultString) {
    std::string str = ogc::draw::GetResultString(ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(str.empty());
    
    str = ogc::draw::GetResultString(ogc::draw::DrawResult::kInvalidParams);
    EXPECT_FALSE(str.empty());
}

TEST(DrawResultTest, GetResultDescription) {
    std::string desc = ogc::draw::GetResultDescription(ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(desc.empty());
    
    desc = ogc::draw::GetResultDescription(ogc::draw::DrawResult::kOutOfMemory);
    EXPECT_FALSE(desc.empty());
}
