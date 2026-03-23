#include <gtest/gtest.h>
#include "ogc/draw/filter.h"
#include "ogc/point.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class FilterTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(FilterTest, NullFilterType) {
    NullFilter filter;
    EXPECT_EQ(filter.GetType(), FilterType::kNone);
}

TEST_F(FilterTest, NullFilterEvaluateFeature) {
    NullFilter filter;
    CNFeature feature;
    EXPECT_TRUE(filter.Evaluate(&feature));
}

TEST_F(FilterTest, NullFilterEvaluateGeometry) {
    NullFilter filter;
    auto geom = Point::Create(0, 0);
    EXPECT_TRUE(filter.Evaluate(geom.get()));
}

TEST_F(FilterTest, NullFilterToString) {
    NullFilter filter;
    EXPECT_EQ(filter.ToString(), "ALL");
}

TEST_F(FilterTest, NullFilterClone) {
    NullFilter filter;
    auto cloned = filter.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetType(), FilterType::kNone);
    EXPECT_EQ(cloned->ToString(), "ALL");
}

TEST_F(FilterTest, NoneFilterType) {
    NoneFilter filter;
    EXPECT_EQ(filter.GetType(), FilterType::kNone);
}

TEST_F(FilterTest, NoneFilterEvaluateFeature) {
    NoneFilter filter;
    CNFeature feature;
    EXPECT_FALSE(filter.Evaluate(&feature));
}

TEST_F(FilterTest, NoneFilterEvaluateGeometry) {
    NoneFilter filter;
    auto geom = Point::Create(0, 0);
    EXPECT_FALSE(filter.Evaluate(geom.get()));
}

TEST_F(FilterTest, NoneFilterToString) {
    NoneFilter filter;
    EXPECT_EQ(filter.ToString(), "NONE");
}

TEST_F(FilterTest, NoneFilterClone) {
    NoneFilter filter;
    auto cloned = filter.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetType(), FilterType::kNone);
    EXPECT_EQ(cloned->ToString(), "NONE");
}

TEST_F(FilterTest, CreateAll) {
    auto filter = Filter::CreateAll();
    ASSERT_NE(filter, nullptr);
    EXPECT_TRUE(filter->IsAll());
}

TEST_F(FilterTest, CreateNone) {
    auto filter = Filter::CreateNone();
    ASSERT_NE(filter, nullptr);
    EXPECT_TRUE(filter->IsNone());
}

TEST_F(FilterTest, NullFilterIsAll) {
    NullFilter filter;
    EXPECT_TRUE(filter.IsAll());
    EXPECT_FALSE(filter.IsNone());
}

TEST_F(FilterTest, NoneFilterIsNone) {
    NoneFilter filter;
    EXPECT_FALSE(filter.IsAll());
    EXPECT_TRUE(filter.IsNone());
}
