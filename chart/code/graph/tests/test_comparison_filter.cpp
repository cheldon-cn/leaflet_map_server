#include <gtest/gtest.h>
#include "ogc/draw/comparison_filter.h"
#include "ogc/feature/feature.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class ComparisonFilterTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(ComparisonFilterTest, FilterType) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    EXPECT_EQ(filter.GetType(), FilterType::kComparison);
}

TEST_F(ComparisonFilterTest, EqualOperator) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kEqual);
    EXPECT_EQ(filter.GetPropertyName(), "name");
    EXPECT_EQ(filter.GetLiteral(), "test");
}

TEST_F(ComparisonFilterTest, NotEqualOperator) {
    ComparisonFilter filter(ComparisonOperator::kNotEqual, "type", "building");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kNotEqual);
}

TEST_F(ComparisonFilterTest, LessThanOperator) {
    ComparisonFilter filter(ComparisonOperator::kLessThan, "value", "100");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kLessThan);
}

TEST_F(ComparisonFilterTest, GreaterThanOperator) {
    ComparisonFilter filter(ComparisonOperator::kGreaterThan, "value", "50");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kGreaterThan);
}

TEST_F(ComparisonFilterTest, LessThanOrEqualOperator) {
    ComparisonFilter filter(ComparisonOperator::kLessThanOrEqual, "value", "100");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kLessThanOrEqual);
}

TEST_F(ComparisonFilterTest, GreaterThanOrEqualOperator) {
    ComparisonFilter filter(ComparisonOperator::kGreaterThanOrEqual, "value", "50");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kGreaterThanOrEqual);
}

TEST_F(ComparisonFilterTest, LikeOperator) {
    ComparisonFilter filter(ComparisonOperator::kLike, "name", "%test%");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kLike);
}

TEST_F(ComparisonFilterTest, IsNullOperator) {
    ComparisonFilter filter(ComparisonOperator::kIsNull, "name", "");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kIsNull);
}

TEST_F(ComparisonFilterTest, BetweenOperator) {
    ComparisonFilter filter(ComparisonOperator::kBetween, "value", "10", "100");
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kBetween);
    EXPECT_EQ(filter.GetPropertyName(), "value");
    EXPECT_EQ(filter.GetLiteral(), "10");
    EXPECT_EQ(filter.GetLiteral2(), "100");
}

TEST_F(ComparisonFilterTest, SetOperator) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    filter.SetOperator(ComparisonOperator::kNotEqual);
    EXPECT_EQ(filter.GetOperator(), ComparisonOperator::kNotEqual);
}

TEST_F(ComparisonFilterTest, SetPropertyName) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    filter.SetPropertyName("type");
    EXPECT_EQ(filter.GetPropertyName(), "type");
}

TEST_F(ComparisonFilterTest, SetLiteral) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    filter.SetLiteral("newvalue");
    EXPECT_EQ(filter.GetLiteral(), "newvalue");
}

TEST_F(ComparisonFilterTest, SetLiteral2) {
    ComparisonFilter filter(ComparisonOperator::kBetween, "value", "10", "100");
    filter.SetLiteral2("200");
    EXPECT_EQ(filter.GetLiteral2(), "200");
}

TEST_F(ComparisonFilterTest, Clone) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    auto cloned = filter.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetType(), FilterType::kComparison);
    
    auto* clonedFilter = dynamic_cast<ComparisonFilter*>(cloned.get());
    ASSERT_NE(clonedFilter, nullptr);
    EXPECT_EQ(clonedFilter->GetOperator(), ComparisonOperator::kEqual);
    EXPECT_EQ(clonedFilter->GetPropertyName(), "name");
    EXPECT_EQ(clonedFilter->GetLiteral(), "test");
}

TEST_F(ComparisonFilterTest, OperatorToString) {
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kEqual), "=");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kNotEqual), "<>");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kLessThan), "<");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kGreaterThan), ">");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kLessThanOrEqual), "<=");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kGreaterThanOrEqual), ">=");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kLike), "LIKE");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kIsNull), "IS NULL");
    EXPECT_EQ(ComparisonFilter::OperatorToString(ComparisonOperator::kBetween), "BETWEEN");
}

TEST_F(ComparisonFilterTest, StringToOperator) {
    EXPECT_EQ(ComparisonFilter::StringToOperator("="), ComparisonOperator::kEqual);
    EXPECT_EQ(ComparisonFilter::StringToOperator("<>"), ComparisonOperator::kNotEqual);
    EXPECT_EQ(ComparisonFilter::StringToOperator("<"), ComparisonOperator::kLessThan);
    EXPECT_EQ(ComparisonFilter::StringToOperator(">"), ComparisonOperator::kGreaterThan);
    EXPECT_EQ(ComparisonFilter::StringToOperator("<="), ComparisonOperator::kLessThanOrEqual);
    EXPECT_EQ(ComparisonFilter::StringToOperator(">="), ComparisonOperator::kGreaterThanOrEqual);
    EXPECT_EQ(ComparisonFilter::StringToOperator("LIKE"), ComparisonOperator::kLike);
    EXPECT_EQ(ComparisonFilter::StringToOperator("IS NULL"), ComparisonOperator::kIsNull);
    EXPECT_EQ(ComparisonFilter::StringToOperator("BETWEEN"), ComparisonOperator::kBetween);
}

TEST_F(ComparisonFilterTest, ToString) {
    ComparisonFilter filter(ComparisonOperator::kEqual, "name", "test");
    std::string str = filter.ToString();
    EXPECT_TRUE(str.find("name") != std::string::npos);
    EXPECT_TRUE(str.find("test") != std::string::npos);
}
