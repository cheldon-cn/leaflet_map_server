#include <gtest/gtest.h>
#include <ogc/draw/draw_result.h>

TEST(DrawResultTest, DefaultConstructor) {
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawResultTest, AllValues) {
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kSuccess), 0);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kFailed), 1);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kInvalidParameter), 2);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kDeviceNotReady), 9);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kEngineNotReady), 11);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kOutOfMemory), 3);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kFileNotFound), 15);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kUnsupportedFormat), 18);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kCancelled), 7);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kTimeout), 8);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kBufferOverflow), 19);
    EXPECT_EQ(static_cast<int>(ogc::draw::DrawResult::kAccessDenied), 20);
}

TEST(DrawResultTest, GetResultString) {
    std::string str = ogc::draw::GetResultString(ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(str.empty());
    
    str = ogc::draw::GetResultString(ogc::draw::DrawResult::kInvalidParameter);
    EXPECT_FALSE(str.empty());
}

TEST(DrawResultTest, GetResultDescription) {
    std::string desc = ogc::draw::GetResultDescription(ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(desc.empty());
    
    desc = ogc::draw::GetResultDescription(ogc::draw::DrawResult::kOutOfMemory);
    EXPECT_FALSE(desc.empty());
}
