#include <gtest/gtest.h>
#include "ogc/draw/engine_type.h"

TEST(EngineTypeTest, AllValues) {
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kUnknown), 0);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kSimple2D), 1);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kTileBased), 2);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kVectorTile), 3);
    EXPECT_EQ(static_cast<int>(ogc::draw::EngineType::kGPUAccelerated), 4);
}

TEST(EngineTypeTest, GetEngineTypeString) {
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kUnknown), "Unknown");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kSimple2D), "Simple2D");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kTileBased), "TileBased");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kVectorTile), "VectorTile");
    EXPECT_EQ(ogc::draw::GetEngineTypeString(ogc::draw::EngineType::kGPUAccelerated), "GPUAccelerated");
}

TEST(EngineTypeTest, GetEngineTypeDescription) {
    EXPECT_FALSE(ogc::draw::GetEngineTypeDescription(ogc::draw::EngineType::kSimple2D).empty());
    EXPECT_FALSE(ogc::draw::GetEngineTypeDescription(ogc::draw::EngineType::kTileBased).empty());
}
