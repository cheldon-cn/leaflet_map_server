#include <gtest/gtest.h>
#include "ogc/draw/logical_filter.h"
#include "ogc/draw/comparison_filter.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class LogicalFilterTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(LogicalFilterTest, FilterType) {
    LogicalFilter filter(LogicalOperator::kAnd);
    EXPECT_EQ(filter.GetType(), FilterType::kLogical);
}

TEST_F(LogicalFilterTest, AndOperator) {
    LogicalFilter filter(LogicalOperator::kAnd);
    EXPECT_EQ(filter.GetOperator(), LogicalOperator::kAnd);
}

TEST_F(LogicalFilterTest, OrOperator) {
    LogicalFilter filter(LogicalOperator::kOr);
    EXPECT_EQ(filter.GetOperator(), LogicalOperator::kOr);
}

TEST_F(LogicalFilterTest, NotOperator) {
    LogicalFilter filter(LogicalOperator::kNot);
    EXPECT_EQ(filter.GetOperator(), LogicalOperator::kNot);
}

TEST_F(LogicalFilterTest, SetOperator) {
    LogicalFilter filter(LogicalOperator::kAnd);
    filter.SetOperator(LogicalOperator::kOr);
    EXPECT_EQ(filter.GetOperator(), LogicalOperator::kOr);
}

TEST_F(LogicalFilterTest, AddFilter) {
    LogicalFilter filter(LogicalOperator::kAnd);
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    filter.AddFilter(f1);
    EXPECT_EQ(filter.GetFilterCount(), 1);
}

TEST_F(LogicalFilterTest, AddMultipleFilters) {
    LogicalFilter filter(LogicalOperator::kAnd);
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    filter.AddFilter(f1);
    filter.AddFilter(f2);
    EXPECT_EQ(filter.GetFilterCount(), 2);
}

TEST_F(LogicalFilterTest, RemoveFilter) {
    LogicalFilter filter(LogicalOperator::kAnd);
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    filter.AddFilter(f1);
    filter.AddFilter(f2);
    filter.RemoveFilter(f1);
    EXPECT_EQ(filter.GetFilterCount(), 1);
}

TEST_F(LogicalFilterTest, ClearFilters) {
    LogicalFilter filter(LogicalOperator::kAnd);
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    filter.AddFilter(f1);
    filter.AddFilter(f2);
    filter.ClearFilters();
    EXPECT_EQ(filter.GetFilterCount(), 0);
}

TEST_F(LogicalFilterTest, GetFilter) {
    LogicalFilter filter(LogicalOperator::kAnd);
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    filter.AddFilter(f1);
    filter.AddFilter(f2);
    
    auto retrieved = filter.GetFilter(0);
    EXPECT_EQ(retrieved, f1);
    
    retrieved = filter.GetFilter(1);
    EXPECT_EQ(retrieved, f2);
}

TEST_F(LogicalFilterTest, GetFilters) {
    LogicalFilter filter(LogicalOperator::kAnd);
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    filter.AddFilter(f1);
    filter.AddFilter(f2);
    
    const auto& filters = filter.GetFilters();
    EXPECT_EQ(filters.size(), 2);
}

TEST_F(LogicalFilterTest, ConstructorWithVector) {
    std::vector<FilterPtr> filters;
    filters.push_back(std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test"));
    filters.push_back(std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building"));
    
    LogicalFilter filter(LogicalOperator::kAnd, filters);
    EXPECT_EQ(filter.GetFilterCount(), 2);
}

TEST_F(LogicalFilterTest, ConstructorWithTwoFilters) {
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    
    LogicalFilter filter(LogicalOperator::kAnd, f1, f2);
    EXPECT_EQ(filter.GetFilterCount(), 2);
}

TEST_F(LogicalFilterTest, Clone) {
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    
    LogicalFilter filter(LogicalOperator::kAnd, f1, f2);
    auto cloned = filter.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetType(), FilterType::kLogical);
    
    auto* clonedFilter = dynamic_cast<LogicalFilter*>(cloned.get());
    ASSERT_NE(clonedFilter, nullptr);
    EXPECT_EQ(clonedFilter->GetOperator(), LogicalOperator::kAnd);
    EXPECT_EQ(clonedFilter->GetFilterCount(), 2);
}

TEST_F(LogicalFilterTest, OperatorToString) {
    EXPECT_EQ(LogicalFilter::OperatorToString(LogicalOperator::kAnd), "AND");
    EXPECT_EQ(LogicalFilter::OperatorToString(LogicalOperator::kOr), "OR");
    EXPECT_EQ(LogicalFilter::OperatorToString(LogicalOperator::kNot), "NOT");
}

TEST_F(LogicalFilterTest, StringToOperator) {
    EXPECT_EQ(LogicalFilter::StringToOperator("AND"), LogicalOperator::kAnd);
    EXPECT_EQ(LogicalFilter::StringToOperator("OR"), LogicalOperator::kOr);
    EXPECT_EQ(LogicalFilter::StringToOperator("NOT"), LogicalOperator::kNot);
}

TEST_F(LogicalFilterTest, ToString) {
    auto f1 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "name", "test");
    auto f2 = std::make_shared<ComparisonFilter>(ComparisonOperator::kEqual, "type", "building");
    
    LogicalFilter filter(LogicalOperator::kAnd, f1, f2);
    std::string str = filter.ToString();
    EXPECT_TRUE(str.find("AND") != std::string::npos);
}
