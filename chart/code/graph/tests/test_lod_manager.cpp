#include <gtest/gtest.h>
#include "ogc/draw/lod_manager.h"
#include "ogc/draw/lod.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class LODManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_manager = LODManager::Create();
        ASSERT_NE(m_manager, nullptr);
    }
    
    void TearDown() override {
        m_manager.reset();
    }
    
    LODManagerPtr m_manager;
};

TEST_F(LODManagerTest, Create) {
    EXPECT_NE(m_manager, nullptr);
}

TEST_F(LODManagerTest, CreateWithStrategy) {
    auto strategy = LODStrategy::CreateDefault();
    auto manager = LODManager::Create(strategy);
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(manager->GetStrategy(), strategy);
}

TEST_F(LODManagerTest, CreateWebMercator) {
    auto manager = LODManager::CreateWebMercator(0, 18);
    ASSERT_NE(manager, nullptr);
    EXPECT_GT(manager->GetLODCount(), 0);
}

TEST_F(LODManagerTest, SetGetStrategy) {
    auto strategy = LODStrategy::CreateDefault();
    m_manager->SetStrategy(strategy);
    EXPECT_EQ(m_manager->GetStrategy(), strategy);
}

TEST_F(LODManagerTest, AddLODLevel) {
    auto lod = LODLevel::Create(5, 1000.0, 10000.0);
    m_manager->AddLODLevel(lod);
    EXPECT_EQ(m_manager->GetLODCount(), 1);
}

TEST_F(LODManagerTest, AddMultipleLODLevels) {
    for (int i = 0; i < 5; i++) {
        auto lod = LODLevel::Create(i, 1000.0 * (i + 1), 10000.0 * (i + 1));
        m_manager->AddLODLevel(lod);
    }
    EXPECT_EQ(m_manager->GetLODCount(), 5);
}

TEST_F(LODManagerTest, RemoveLODLevel) {
    auto lod1 = LODLevel::Create(1, 1000.0, 10000.0);
    auto lod2 = LODLevel::Create(2, 2000.0, 20000.0);
    m_manager->AddLODLevel(lod1);
    m_manager->AddLODLevel(lod2);
    m_manager->RemoveLODLevel(1);
    EXPECT_EQ(m_manager->GetLODCount(), 1);
}

TEST_F(LODManagerTest, ClearLODLevels) {
    auto lod1 = LODLevel::Create(1, 1000.0, 10000.0);
    auto lod2 = LODLevel::Create(2, 2000.0, 20000.0);
    m_manager->AddLODLevel(lod1);
    m_manager->AddLODLevel(lod2);
    m_manager->ClearLODLevels();
    EXPECT_EQ(m_manager->GetLODCount(), 0);
}

TEST_F(LODManagerTest, GetLODLevel) {
    auto lod = LODLevel::Create(5, 1000.0, 10000.0);
    m_manager->AddLODLevel(lod);
    
    auto retrieved = m_manager->GetLODLevel(5);
    EXPECT_EQ(retrieved, lod);
}

TEST_F(LODManagerTest, GetLODLevels) {
    auto lod1 = LODLevel::Create(1, 1000.0, 10000.0);
    auto lod2 = LODLevel::Create(2, 2000.0, 20000.0);
    m_manager->AddLODLevel(lod1);
    m_manager->AddLODLevel(lod2);
    
    auto levels = m_manager->GetLODLevels();
    EXPECT_EQ(levels.size(), 2);
}

TEST_F(LODManagerTest, SelectLOD) {
    for (int i = 0; i < 5; i++) {
        auto lod = LODLevel::Create(i, 1000.0 * (i + 1), 10000.0 * (i + 1));
        m_manager->AddLODLevel(lod);
    }
    
    int lod = m_manager->SelectLOD(5000.0);
    EXPECT_GE(lod, 0);
}

TEST_F(LODManagerTest, SelectLODByResolution) {
    for (int i = 0; i < 5; i++) {
        auto lod = LODLevel::Create(i, 1000.0 * (i + 1), 10000.0 * (i + 1));
        lod->SetResolution(10.0 * (i + 1));
        m_manager->AddLODLevel(lod);
    }
    
    int lod = m_manager->SelectLODByResolution(15.0);
    EXPECT_GE(lod, 0);
}

TEST_F(LODManagerTest, SetGetMinLOD) {
    m_manager->SetMinLOD(2);
    EXPECT_EQ(m_manager->GetMinLOD(), 2);
}

TEST_F(LODManagerTest, SetGetMaxLOD) {
    m_manager->SetMaxLOD(10);
    EXPECT_EQ(m_manager->GetMaxLOD(), 10);
}

TEST_F(LODManagerTest, SetLODRange) {
    m_manager->SetLODRange(2, 10);
    EXPECT_EQ(m_manager->GetMinLOD(), 2);
    EXPECT_EQ(m_manager->GetMaxLOD(), 10);
}

TEST_F(LODManagerTest, IsLODVisible) {
    m_manager->SetLODRange(2, 10);
    EXPECT_TRUE(m_manager->IsLODVisible(5));
    EXPECT_FALSE(m_manager->IsLODVisible(1));
    EXPECT_FALSE(m_manager->IsLODVisible(15));
}

TEST_F(LODManagerTest, SetGetTransitionFactor) {
    m_manager->SetTransitionFactor(0.5);
    EXPECT_DOUBLE_EQ(m_manager->GetTransitionFactor(), 0.5);
}

TEST_F(LODManagerTest, SetEnableTransitions) {
    m_manager->SetEnableTransitions(true);
    EXPECT_TRUE(m_manager->IsTransitionsEnabled());
    
    m_manager->SetEnableTransitions(false);
    EXPECT_FALSE(m_manager->IsTransitionsEnabled());
}

TEST_F(LODManagerTest, CreateDefaultLODs) {
    m_manager->CreateDefaultLODs(0, 5, 1000.0);
    EXPECT_EQ(m_manager->GetLODCount(), 6);
}

TEST_F(LODManagerTest, CreateWebMercatorLODs) {
    m_manager->CreateWebMercatorLODs(0, 10);
    EXPECT_EQ(m_manager->GetLODCount(), 11);
}

TEST_F(LODManagerTest, GetResolutionForLevel) {
    m_manager->CreateWebMercatorLODs(0, 18);
    double resolution = m_manager->GetResolutionForLevel(0);
    EXPECT_GT(resolution, 0);
}

TEST_F(LODManagerTest, GetLevelForResolution) {
    m_manager->CreateWebMercatorLODs(0, 18);
    int level = m_manager->GetLevelForResolution(1000.0);
    EXPECT_GE(level, 0);
}

TEST_F(LODManagerTest, GetScaleForLevel) {
    m_manager->CreateWebMercatorLODs(0, 18);
    double scale = m_manager->GetScaleForLevel(5);
    EXPECT_GT(scale, 0);
}

TEST_F(LODManagerTest, GetLevelForScale) {
    m_manager->CreateWebMercatorLODs(0, 18);
    int level = m_manager->GetLevelForScale(100000.0);
    EXPECT_GE(level, 0);
}
