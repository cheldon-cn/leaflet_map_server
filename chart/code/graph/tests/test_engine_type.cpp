#include <gtest/gtest.h>
#include <ogc/draw/engine_type.h>

TEST(EngineTypeTest, AllValues) {
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kUnknown), 0);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kSimple2D), 1);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kTile), 4);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kVector), 3);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kGPU), 2);
}

TEST(EngineTypeTest, GetEngineTypeString) {
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kUnknown), "Unknown");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kSimple2D), "Simple2D");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kTile), "Tile");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kVector), "Vector");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kGPU), "GPU");
}

TEST(EngineTypeTest, GetEngineTypeDescription) {
    EXPECT_FALSE(ogc::draw::GetEngineTypeDescription(ogc::draw::EngineType::kSimple2D).empty());
    EXPECT_FALSE(ogc::draw::GetEngineTypeDescription(ogc::draw::EngineType::kTile).empty());
}
