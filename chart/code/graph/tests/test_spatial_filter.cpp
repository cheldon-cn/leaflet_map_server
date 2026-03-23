#include <gtest/gtest.h>
#include "ogc/draw/spatial_filter.h"
#include "ogc/draw/filter.h"
#include "ogc/point.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class SpatialFilterTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SpatialFilterTest, FilterType) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    EXPECT_EQ(filter.GetType(), FilterType::kSpatial);
}

TEST_F(SpatialFilterTest, IntersectsOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kIntersects);
}

TEST_F(SpatialFilterTest, WithinOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kWithin, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kWithin);
}

TEST_F(SpatialFilterTest, ContainsOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kContains, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kContains);
}

TEST_F(SpatialFilterTest, EqualsOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kEquals, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kEquals);
}

TEST_F(SpatialFilterTest, OverlapsOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kOverlaps, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kOverlaps);
}

TEST_F(SpatialFilterTest, TouchesOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kTouches, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kTouches);
}

TEST_F(SpatialFilterTest, CrossesOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kCrosses, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kCrosses);
}

TEST_F(SpatialFilterTest, DisjointOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kDisjoint, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kDisjoint);
}

TEST_F(SpatialFilterTest, BboxOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kBbox, geom.get());
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kBbox);
}

TEST_F(SpatialFilterTest, SetOperator) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    filter.SetOperator(SpatialOperator::kWithin);
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kWithin);
}

TEST_F(SpatialFilterTest, ConstructorWithEnvelope) {
    Envelope env(0, 0, 100, 100);
    SpatialFilter filter(SpatialOperator::kBbox, env);
    EXPECT_EQ(filter.GetOperator(), SpatialOperator::kBbox);
    EXPECT_TRUE(filter.HasEnvelope());
    EXPECT_FALSE(filter.HasGeometry());
}

TEST_F(SpatialFilterTest, HasGeometry) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    EXPECT_TRUE(filter.HasGeometry());
    EXPECT_FALSE(filter.HasEnvelope());
}

TEST_F(SpatialFilterTest, SetGeometry) {
    SpatialFilter filter(SpatialOperator::kIntersects, Envelope(0, 0, 10, 10));
    auto geom = Point::Create(50, 50);
    filter.SetGeometry(geom.get());
    EXPECT_TRUE(filter.HasGeometry());
}

TEST_F(SpatialFilterTest, SetEnvelope) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    Envelope env(0, 0, 100, 100);
    filter.SetEnvelope(env);
    EXPECT_TRUE(filter.HasEnvelope());
}

TEST_F(SpatialFilterTest, GetEnvelope) {
    Envelope env(0, 0, 100, 100);
    SpatialFilter filter(SpatialOperator::kBbox, env);
    const Envelope& retrieved = filter.GetEnvelope();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMinY(), 0);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 100);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxY(), 100);
}

TEST_F(SpatialFilterTest, Clone) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    auto cloned = filter.Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetType(), FilterType::kSpatial);
    
    auto* clonedFilter = dynamic_cast<SpatialFilter*>(cloned.get());
    ASSERT_NE(clonedFilter, nullptr);
    EXPECT_EQ(clonedFilter->GetOperator(), SpatialOperator::kIntersects);
}

TEST_F(SpatialFilterTest, OperatorToString) {
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kBbox), "BBOX");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kIntersects), "Intersects");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kWithin), "Within");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kContains), "Contains");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kEquals), "Equals");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kOverlaps), "Overlaps");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kTouches), "Touches");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kCrosses), "Crosses");
    EXPECT_EQ(SpatialFilter::OperatorToString(SpatialOperator::kDisjoint), "Disjoint");
}

TEST_F(SpatialFilterTest, StringToOperator) {
    EXPECT_EQ(SpatialFilter::StringToOperator("BBOX"), SpatialOperator::kBbox);
    EXPECT_EQ(SpatialFilter::StringToOperator("Intersects"), SpatialOperator::kIntersects);
    EXPECT_EQ(SpatialFilter::StringToOperator("Within"), SpatialOperator::kWithin);
    EXPECT_EQ(SpatialFilter::StringToOperator("Contains"), SpatialOperator::kContains);
    EXPECT_EQ(SpatialFilter::StringToOperator("Equals"), SpatialOperator::kEquals);
    EXPECT_EQ(SpatialFilter::StringToOperator("Overlaps"), SpatialOperator::kOverlaps);
    EXPECT_EQ(SpatialFilter::StringToOperator("Touches"), SpatialOperator::kTouches);
    EXPECT_EQ(SpatialFilter::StringToOperator("Crosses"), SpatialOperator::kCrosses);
    EXPECT_EQ(SpatialFilter::StringToOperator("Disjoint"), SpatialOperator::kDisjoint);
}

TEST_F(SpatialFilterTest, ToString) {
    auto geom = Point::Create(0, 0);
    SpatialFilter filter(SpatialOperator::kIntersects, geom.get());
    std::string str = filter.ToString();
    EXPECT_TRUE(str.find("Intersects") != std::string::npos);
}
