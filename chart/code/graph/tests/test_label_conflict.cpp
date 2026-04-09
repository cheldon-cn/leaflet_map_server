#include <gtest/gtest.h>
#include "ogc/graph/label/label_conflict.h"
#include "ogc/graph/label/label_engine.h"
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::graph;
using namespace ogc;

class LabelConflictTest : public ::testing::Test {
protected:
    void SetUp() override {
        resolver = LabelConflictResolver::Create();
        ASSERT_NE(resolver, nullptr);
    }
    
    void TearDown() override {
        resolver.reset();
    }
    
    LabelConflictResolverPtr resolver;
};

TEST_F(LabelConflictTest, DefaultConstructor) {
    LabelConflictResolverPtr defaultResolver = LabelConflictResolver::Create();
    EXPECT_NE(defaultResolver, nullptr);
}

TEST_F(LabelConflictTest, SetMinDistance) {
    resolver->SetMinDistance(10.0);
    EXPECT_DOUBLE_EQ(resolver->GetMinDistance(), 10.0);
    
    resolver->SetMinDistance(20.0);
    EXPECT_DOUBLE_EQ(resolver->GetMinDistance(), 20.0);
}

TEST_F(LabelConflictTest, SetAllowOverlap) {
    resolver->SetAllowOverlap(true);
    EXPECT_TRUE(resolver->GetAllowOverlap());
    
    resolver->SetAllowOverlap(false);
    EXPECT_FALSE(resolver->GetAllowOverlap());
}

TEST_F(LabelConflictTest, SetPriorityMode) {
    resolver->SetPriorityMode("LENGTH");
    EXPECT_EQ(resolver->GetPriorityMode(), "LENGTH");
    
    resolver->SetPriorityMode("PRIORITY");
    EXPECT_EQ(resolver->GetPriorityMode(), "PRIORITY");
}

TEST_F(LabelConflictTest, HasConflictNoOverlap) {
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    
    LabelInfo label2;
    label2.x = 100;
    label2.y = 100;
    label2.width = 50;
    label2.height = 20;
    
    EXPECT_FALSE(resolver->HasConflict(label1, label2));
}

TEST_F(LabelConflictTest, HasConflictWithOverlap) {
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    
    LabelInfo label2;
    label2.x = 25;
    label2.y = 10;
    label2.width = 50;
    label2.height = 20;
    
    EXPECT_TRUE(resolver->HasConflict(label1, label2));
}

TEST_F(LabelConflictTest, CalculateOverlapAreaNoOverlap) {
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    
    LabelInfo label2;
    label2.x = 100;
    label2.y = 100;
    label2.width = 50;
    label2.height = 20;
    
    double area = resolver->CalculateOverlapArea(label1, label2);
    EXPECT_DOUBLE_EQ(area, 0.0);
}

TEST_F(LabelConflictTest, CalculateOverlapAreaWithOverlap) {
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    
    LabelInfo label2;
    label2.x = 25;
    label2.y = 10;
    label2.width = 50;
    label2.height = 20;
    
    double area = resolver->CalculateOverlapArea(label1, label2);
    EXPECT_GT(area, 0.0);
}

TEST_F(LabelConflictTest, DetectConflictsEmpty) {
    std::vector<LabelInfo> labels;
    std::vector<ConflictInfo> conflicts = resolver->DetectConflicts(labels);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(LabelConflictTest, DetectConflictsNoConflict) {
    std::vector<LabelInfo> labels;
    
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    labels.push_back(label1);
    
    LabelInfo label2;
    label2.x = 100;
    label2.y = 100;
    label2.width = 50;
    label2.height = 20;
    labels.push_back(label2);
    
    std::vector<ConflictInfo> conflicts = resolver->DetectConflicts(labels);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(LabelConflictTest, DetectConflictsWithConflict) {
    std::vector<LabelInfo> labels;
    
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    labels.push_back(label1);
    
    LabelInfo label2;
    label2.x = 25;
    label2.y = 10;
    label2.width = 50;
    label2.height = 20;
    labels.push_back(label2);
    
    std::vector<ConflictInfo> conflicts = resolver->DetectConflicts(labels);
    EXPECT_FALSE(conflicts.empty());
}

TEST_F(LabelConflictTest, ResolveConflictsEmpty) {
    std::vector<LabelInfo> labels;
    std::vector<LabelInfo> resolved = resolver->ResolveConflicts(labels);
    EXPECT_TRUE(resolved.empty());
}

TEST_F(LabelConflictTest, ResolveConflictsNoConflict) {
    std::vector<LabelInfo> labels;
    
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    label1.priority = 1;
    labels.push_back(label1);
    
    LabelInfo label2;
    label2.x = 100;
    label2.y = 100;
    label2.width = 50;
    label2.height = 20;
    label2.priority = 2;
    labels.push_back(label2);
    
    std::vector<LabelInfo> resolved = resolver->ResolveConflicts(labels);
    EXPECT_EQ(resolved.size(), 2);
}

TEST_F(LabelConflictTest, Clear) {
    std::vector<LabelInfo> labels;
    
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    labels.push_back(label1);
    
    LabelInfo label2;
    label2.x = 25;
    label2.y = 10;
    label2.width = 50;
    label2.height = 20;
    labels.push_back(label2);
    
    std::vector<ConflictInfo> conflicts = resolver->DetectConflicts(labels);
    EXPECT_FALSE(conflicts.empty());
    
    resolver->Clear();
}

TEST_F(LabelConflictTest, ConflictInfoDefaultValues) {
    ConflictInfo info;
    
    EXPECT_EQ(info.labelIndex1, -1);
    EXPECT_EQ(info.labelIndex2, -1);
    EXPECT_DOUBLE_EQ(info.overlapArea, 0.0);
    EXPECT_FALSE(info.resolved);
}

TEST_F(LabelConflictTest, MinDistanceAffectsConflict) {
    LabelInfo label1;
    label1.x = 0;
    label1.y = 0;
    label1.width = 50;
    label1.height = 20;
    
    LabelInfo label2;
    label2.x = 60;
    label2.y = 0;
    label2.width = 50;
    label2.height = 20;
    
    resolver->SetMinDistance(0.0);
    EXPECT_FALSE(resolver->HasConflict(label1, label2));
    
    resolver->SetMinDistance(20.0);
    EXPECT_TRUE(resolver->HasConflict(label1, label2));
}
