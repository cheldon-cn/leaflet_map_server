#include "gtest/gtest.h"
#include "ogc/feature/feature_guard.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"

using namespace ogc;

class FeatureGuardTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("test");
    }
    
    void TearDown() override {
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
};

TEST_F(FeatureGuardTest, DefaultConstructor) {
    CNFeatureGuard guard;
    EXPECT_EQ(guard.Get(), nullptr);
}

TEST_F(FeatureGuardTest, PointerConstructor) {
    CNFeature* feature = new CNFeature(defn_);
    CNFeatureGuard guard(feature);
    
    EXPECT_EQ(guard.Get(), feature);
}

TEST_F(FeatureGuardTest, DestructorReleasesFeature) {
    CNFeature* feature = new CNFeature(defn_);
    feature->SetFID(123);
    
    {
        CNFeatureGuard guard(feature);
        EXPECT_EQ(guard.Get(), feature);
    }
    
    // Feature should be deleted after guard goes out of scope
}

TEST_F(FeatureGuardTest, Get) {
    CNFeature* feature = new CNFeature(defn_);
    CNFeatureGuard guard(feature);
    
    CNFeature* retrieved = guard.Get();
    EXPECT_EQ(retrieved, feature);
    EXPECT_EQ(retrieved->GetFID(), 0);
}

TEST_F(FeatureGuardTest, Release) {
    CNFeature* feature = new CNFeature(defn_);
    feature->SetFID(456);
    
    CNFeatureGuard guard(feature);
    CNFeature* released = guard.Release();
    
    EXPECT_EQ(released, feature);
    EXPECT_EQ(released->GetFID(), 456);
    EXPECT_EQ(guard.Get(), nullptr);
    
    delete released;
}

TEST_F(FeatureGuardTest, Reset) {
    CNFeature* feature1 = new CNFeature(defn_);
    feature1->SetFID(1);
    
    CNFeatureGuard guard(feature1);
    EXPECT_EQ(guard.Get()->GetFID(), 1);
    
    CNFeature* feature2 = new CNFeature(defn_);
    feature2->SetFID(2);
    
    guard.Reset(feature2);
    EXPECT_EQ(guard.Get()->GetFID(), 2);
}

TEST_F(FeatureGuardTest, ResetNull) {
    CNFeature* feature = new CNFeature(defn_);
    CNFeatureGuard guard(feature);
    
    guard.Reset(nullptr);
    EXPECT_EQ(guard.Get(), nullptr);
}

TEST_F(FeatureGuardTest, MoveConstructor) {
    CNFeature* feature = new CNFeature(defn_);
    feature->SetFID(789);
    
    CNFeatureGuard guard1(feature);
    CNFeatureGuard guard2(std::move(guard1));
    
    EXPECT_EQ(guard1.Get(), nullptr);
    EXPECT_EQ(guard2.Get(), feature);
    EXPECT_EQ(guard2.Get()->GetFID(), 789);
}

TEST_F(FeatureGuardTest, MoveAssignment) {
    CNFeature* feature1 = new CNFeature(defn_);
    feature1->SetFID(100);
    
    CNFeature* feature2 = new CNFeature(defn_);
    feature2->SetFID(200);
    
    CNFeatureGuard guard1(feature1);
    CNFeatureGuard guard2(feature2);
    
    guard2 = std::move(guard1);
    
    EXPECT_EQ(guard1.Get(), nullptr);
    EXPECT_EQ(guard2.Get(), feature1);
    EXPECT_EQ(guard2.Get()->GetFID(), 100);
}

TEST_F(FeatureGuardTest, ArrowOperator) {
    CNFeature* feature = new CNFeature(defn_);
    feature->SetFID(999);
    
    CNFeatureGuard guard(feature);
    EXPECT_EQ(guard->GetFID(), 999);
}

TEST_F(FeatureGuardTest, DereferenceOperator) {
    CNFeature* feature = new CNFeature(defn_);
    feature->SetFID(888);
    
    CNFeatureGuard guard(feature);
    EXPECT_EQ((*guard).GetFID(), 888);
}

TEST_F(FeatureGuardTest, BoolConversion) {
    CNFeatureGuard guard1;
    EXPECT_FALSE(static_cast<bool>(guard1));
    
    CNFeature* feature = new CNFeature(defn_);
    CNFeatureGuard guard2(feature);
    EXPECT_TRUE(static_cast<bool>(guard2));
}

TEST_F(FeatureGuardTest, NullPointerHandling) {
    CNFeatureGuard guard(nullptr);
    EXPECT_EQ(guard.Get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(guard));
}

TEST_F(FeatureGuardTest, Swap) {
    CNFeature* feature1 = new CNFeature(defn_);
    feature1->SetFID(1);
    
    CNFeature* feature2 = new CNFeature(defn_);
    feature2->SetFID(2);
    
    CNFeatureGuard guard1(feature1);
    CNFeatureGuard guard2(feature2);
    
    guard1.Swap(guard2);
    
    EXPECT_EQ(guard1.Get()->GetFID(), 2);
    EXPECT_EQ(guard2.Get()->GetFID(), 1);
}

TEST_F(FeatureGuardTest, SelfAssignment) {
    CNFeature* feature = new CNFeature(defn_);
    CNFeatureGuard guard(feature);
    
    guard = std::move(guard);
    EXPECT_EQ(guard.Get(), feature);
}

TEST_F(FeatureGuardTest, MultipleResets) {
    CNFeatureGuard guard;
    
    for (int i = 0; i < 5; ++i) {
        CNFeature* feature = new CNFeature(defn_);
        feature->SetFID(i);
        guard.Reset(feature);
        EXPECT_EQ(guard.Get()->GetFID(), i);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
