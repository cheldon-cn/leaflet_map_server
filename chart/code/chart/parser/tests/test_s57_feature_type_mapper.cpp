#include <gtest/gtest.h>
#include "chart_parser/s57_feature_type_mapper.h"

using namespace chart::parser;

class S57FeatureTypeMapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        mapper_ = new S57FeatureTypeMapper();
    }
    
    void TearDown() override {
        delete mapper_;
    }
    
    S57FeatureTypeMapper* mapper_;
};

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_DEPARE_ReturnsDepare) {
    FeatureType type = mapper_->MapFeatureType("DEPARE");
    EXPECT_EQ(type, FeatureType::DEPARE);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_DEPCNT_ReturnsDepcnt) {
    FeatureType type = mapper_->MapFeatureType("DEPCNT");
    EXPECT_EQ(type, FeatureType::DEPCNT);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_COALNE_ReturnsCoalne) {
    FeatureType type = mapper_->MapFeatureType("COALNE");
    EXPECT_EQ(type, FeatureType::COALNE);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_LNDARE_ReturnsLndare) {
    FeatureType type = mapper_->MapFeatureType("LNDARE");
    EXPECT_EQ(type, FeatureType::LNDARE);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_LIGHTS_ReturnsLights) {
    FeatureType type = mapper_->MapFeatureType("LIGHTS");
    EXPECT_EQ(type, FeatureType::LIGHTS);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_BOYCAR_ReturnsUnknown) {
    FeatureType type = mapper_->MapFeatureType("BOYCAR");
    EXPECT_EQ(type, FeatureType::Unknown);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_BCNSAW_ReturnsBcnsaw) {
    FeatureType type = mapper_->MapFeatureType("BCNSAW");
    EXPECT_EQ(type, FeatureType::BCNSAW);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_WRECKS_ReturnsWrecks) {
    FeatureType type = mapper_->MapFeatureType("WRECKS");
    EXPECT_EQ(type, FeatureType::WRECKS);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_OBSTRN_ReturnsObstrn) {
    FeatureType type = mapper_->MapFeatureType("OBSTRN");
    EXPECT_EQ(type, FeatureType::OBSTRN);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_UWTROC_ReturnsUwtroc) {
    FeatureType type = mapper_->MapFeatureType("UWTROC");
    EXPECT_EQ(type, FeatureType::UWTROC);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_SBDARE_ReturnsSbdare) {
    FeatureType type = mapper_->MapFeatureType("SBDARE");
    EXPECT_EQ(type, FeatureType::SBDARE);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_SOUNDG_ReturnsSoundg) {
    FeatureType type = mapper_->MapFeatureType("SOUNDG");
    EXPECT_EQ(type, FeatureType::SOUNDG);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_UNKNOWN_ReturnsUnknown) {
    FeatureType type = mapper_->MapFeatureType("UNKNOWN_CLASS");
    EXPECT_EQ(type, FeatureType::Unknown);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_EmptyString_ReturnsUnknown) {
    FeatureType type = mapper_->MapFeatureType("");
    EXPECT_EQ(type, FeatureType::Unknown);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_CaseSensitive) {
    FeatureType type1 = mapper_->MapFeatureType("DEPARE");
    FeatureType type2 = mapper_->MapFeatureType("depare");
    
    EXPECT_EQ(type1, FeatureType::DEPARE);
    EXPECT_EQ(type2, FeatureType::Unknown);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_MultipleCallsSameResult) {
    FeatureType type1 = mapper_->MapFeatureType("DEPARE");
    FeatureType type2 = mapper_->MapFeatureType("DEPARE");
    FeatureType type3 = mapper_->MapFeatureType("DEPARE");
    
    EXPECT_EQ(type1, type2);
    EXPECT_EQ(type2, type3);
}

TEST_F(S57FeatureTypeMapperTest, MapFeatureType_AllKnownTypes) {
    struct TestCase {
        std::string className;
        FeatureType expected;
    };
    
    std::vector<TestCase> testCases = {
        {"DEPARE", FeatureType::DEPARE},
        {"DEPCNT", FeatureType::DEPCNT},
        {"COALNE", FeatureType::COALNE},
        {"LNDARE", FeatureType::LNDARE},
        {"LIGHTS", FeatureType::LIGHTS},
        {"BOYLAT", FeatureType::BOYLAT},
        {"BCNSAW", FeatureType::BCNSAW},
        {"WRECKS", FeatureType::WRECKS},
        {"OBSTRN", FeatureType::OBSTRN},
        {"UWTROC", FeatureType::UWTROC},
        {"SBDARE", FeatureType::SBDARE},
        {"SOUNDG", FeatureType::SOUNDG}
    };
    
    for (const auto& tc : testCases) {
        FeatureType result = mapper_->MapFeatureType(tc.className);
        EXPECT_EQ(result, tc.expected) << "Failed for class: " << tc.className;
    }
}

TEST_F(S57FeatureTypeMapperTest, Constructor_InitializesMapping) {
    S57FeatureTypeMapper newMapper;
    FeatureType type = newMapper.MapFeatureType("DEPARE");
    EXPECT_EQ(type, FeatureType::DEPARE);
}
