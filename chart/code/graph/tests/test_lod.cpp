#include <gtest/gtest.h>
#include "ogc/graph/lod/lod.h"
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::graph;
using namespace ogc;

class LODLevelTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_lod = LODLevel::Create();
        ASSERT_NE(m_lod, nullptr);
    }
    
    void TearDown() override {
        m_lod.reset();
    }
    
    LODLevelPtr m_lod;
};

TEST_F(LODLevelTest, Create) {
    EXPECT_NE(m_lod, nullptr);
}

TEST_F(LODLevelTest, CreateWithParams) {
    auto lod = LODLevel::Create(5, 1000.0, 10000.0);
    ASSERT_NE(lod, nullptr);
    EXPECT_EQ(lod->GetLevel(), 5);
    EXPECT_DOUBLE_EQ(lod->GetMinScale(), 1000.0);
    EXPECT_DOUBLE_EQ(lod->GetMaxScale(), 10000.0);
}

TEST_F(LODLevelTest, SetGetLevel) {
    m_lod->SetLevel(10);
    EXPECT_EQ(m_lod->GetLevel(), 10);
}

TEST_F(LODLevelTest, SetGetMinScale) {
    m_lod->SetMinScale(500.0);
    EXPECT_DOUBLE_EQ(m_lod->GetMinScale(), 500.0);
}

TEST_F(LODLevelTest, SetGetMaxScale) {
    m_lod->SetMaxScale(50000.0);
    EXPECT_DOUBLE_EQ(m_lod->GetMaxScale(), 50000.0);
}

TEST_F(LODLevelTest, SetGetResolution) {
    m_lod->SetResolution(1.0);
    EXPECT_DOUBLE_EQ(m_lod->GetResolution(), 1.0);
}

TEST_F(LODLevelTest, SetGetExtent) {
    Envelope extent(0, 0, 100, 100);
    m_lod->SetExtent(extent);
    auto retrieved = m_lod->GetExtent();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 100);
}

TEST_F(LODLevelTest, SetGetName) {
    m_lod->SetName("LOD5");
    EXPECT_EQ(m_lod->GetName(), "LOD5");
}

TEST_F(LODLevelTest, SetGetDescription) {
    m_lod->SetDescription("Level 5 detail");
    EXPECT_EQ(m_lod->GetDescription(), "Level 5 detail");
}

TEST_F(LODLevelTest, IsVisibleAtScale) {
    m_lod->SetMinScale(1000.0);
    m_lod->SetMaxScale(10000.0);
    
    EXPECT_TRUE(m_lod->IsVisibleAtScale(5000.0));
    EXPECT_TRUE(m_lod->IsVisibleAtScale(1000.0));
    EXPECT_TRUE(m_lod->IsVisibleAtScale(10000.0));
    EXPECT_FALSE(m_lod->IsVisibleAtScale(500.0));
    EXPECT_FALSE(m_lod->IsVisibleAtScale(20000.0));
}

TEST_F(LODLevelTest, IsVisibleAtResolution) {
    m_lod->SetResolution(10.0);
    EXPECT_TRUE(m_lod->IsVisibleAtResolution(10.0));
    EXPECT_TRUE(m_lod->IsVisibleAtResolution(5.0));
}

TEST_F(LODLevelTest, IsValid) {
    m_lod->SetLevel(5);
    m_lod->SetMinScale(1000.0);
    m_lod->SetMaxScale(10000.0);
    EXPECT_TRUE(m_lod->IsValid());
}

TEST_F(LODLevelTest, IsValidFalseNoLevel) {
    EXPECT_FALSE(m_lod->IsValid());
}

class LODStrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_strategy = LODStrategy::CreateDefault();
        ASSERT_NE(m_strategy, nullptr);
        
        for (int i = 0; i < 5; i++) {
            auto lod = LODLevel::Create(i, 1000.0 * (i + 1), 10000.0 * (i + 1));
            m_levels.push_back(lod);
        }
    }
    
    void TearDown() override {
        m_strategy.reset();
        m_levels.clear();
    }
    
    LODStrategyPtr m_strategy;
    std::vector<LODLevelPtr> m_levels;
};

TEST_F(LODStrategyTest, CreateDefault) {
    EXPECT_NE(m_strategy, nullptr);
}

TEST_F(LODStrategyTest, SelectLOD) {
    int lod = m_strategy->SelectLOD(5000.0, m_levels);
    EXPECT_GE(lod, 0);
    EXPECT_LT(lod, static_cast<int>(m_levels.size()));
}

TEST_F(LODStrategyTest, SelectLODByResolution) {
    int lod = m_strategy->SelectLODByResolution(10.0, m_levels);
    EXPECT_GE(lod, 0);
}

TEST_F(LODStrategyTest, SelectLODByExtent) {
    Envelope extent(0, 0, 1000, 1000);
    int lod = m_strategy->SelectLODByExtent(extent, 256, 256, m_levels);
    EXPECT_GE(lod, 0);
}

TEST_F(LODStrategyTest, GetVisibleLODs) {
    auto visible = m_strategy->GetVisibleLODs(5000.0, m_levels);
    EXPECT_GE(visible.size(), 1);
}

TEST_F(LODStrategyTest, GetName) {
    auto name = m_strategy->GetName();
    EXPECT_FALSE(name.empty());
}

TEST_F(LODStrategyTest, GetDescription) {
    auto desc = m_strategy->GetDescription();
    EXPECT_FALSE(desc.empty());
}

TEST_F(LODStrategyTest, Clone) {
    auto cloned = m_strategy->Clone();
    ASSERT_NE(cloned, nullptr);
}
