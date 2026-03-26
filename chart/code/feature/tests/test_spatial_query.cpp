#include "gtest/gtest.h"
#include "ogc/feature/spatial_query.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/geometry.h"

using namespace ogc;

class SpatialQueryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SpatialQueryTest, DefaultConstructor) {
    CNSpatialQuery query;
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kIntersects);
    EXPECT_EQ(query.GetBufferDistance(), 0.0);
    EXPECT_EQ(query.GetSRID(), 0);
}

TEST_F(SpatialQueryTest, SetGeometry) {
    CNSpatialQuery query;
    auto point = Point::Create(100.0, 200.0);
    
    query.SetGeometry(std::move(point));
    GeometryPtr geom = query.GetGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kPoint);
}

TEST_F(SpatialQueryTest, SetOperation) {
    CNSpatialQuery query;
    
    query.SetOperation(SpatialOperation::kContains);
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kContains);
    
    query.SetOperation(SpatialOperation::kWithin);
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kWithin);
}

TEST_F(SpatialQueryTest, SetBufferDistance) {
    CNSpatialQuery query;
    
    query.SetBufferDistance(100.0);
    EXPECT_DOUBLE_EQ(query.GetBufferDistance(), 100.0);
    
    query.SetBufferDistance(0.0);
    EXPECT_DOUBLE_EQ(query.GetBufferDistance(), 0.0);
}

TEST_F(SpatialQueryTest, SetSRID) {
    CNSpatialQuery query;
    
    query.SetSRID(4326);
    EXPECT_EQ(query.GetSRID(), 4326);
    
    query.SetSRID(3857);
    EXPECT_EQ(query.GetSRID(), 3857);
}

TEST_F(SpatialQueryTest, StaticIntersects) {
    auto point = Point::Create(0.0, 0.0);
    CNSpatialQuery query = CNSpatialQuery::Intersects(std::move(point));
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kIntersects);
    ASSERT_NE(query.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, StaticContains) {
    auto polygon = Polygon::Create();
    CNSpatialQuery query = CNSpatialQuery::Contains(std::move(polygon));
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kContains);
    ASSERT_NE(query.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, StaticWithin) {
    auto point = Point::Create(10.0, 20.0);
    CNSpatialQuery query = CNSpatialQuery::Within(std::move(point));
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kWithin);
    ASSERT_NE(query.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, StaticOverlaps) {
    auto line = LineString::Create();
    CNSpatialQuery query = CNSpatialQuery::Overlaps(std::move(line));
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kOverlaps);
    ASSERT_NE(query.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, StaticWithinDistance) {
    auto point = Point::Create(0.0, 0.0);
    CNSpatialQuery query = CNSpatialQuery::WithinDistance(std::move(point), 50.0);
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kIntersects);
    EXPECT_DOUBLE_EQ(query.GetBufferDistance(), 50.0);
    ASSERT_NE(query.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, MoveConstructor) {
    CNSpatialQuery query1;
    auto point = Point::Create(100.0, 200.0);
    query1.SetGeometry(std::move(point));
    query1.SetOperation(SpatialOperation::kContains);
    
    CNSpatialQuery query2(std::move(query1));
    EXPECT_EQ(query2.GetOperation(), SpatialOperation::kContains);
    ASSERT_NE(query2.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, MoveAssignment) {
    CNSpatialQuery query1;
    auto point = Point::Create(50.0, 100.0);
    query1.SetGeometry(std::move(point));
    query1.SetSRID(4326);
    
    CNSpatialQuery query2;
    query2 = std::move(query1);
    EXPECT_EQ(query2.GetSRID(), 4326);
    ASSERT_NE(query2.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, ToWKT) {
    CNSpatialQuery query;
    auto point = Point::Create(0.0, 0.0);
    query.SetGeometry(std::move(point));
    
    std::string wkt = query.ToWKT();
    EXPECT_FALSE(wkt.empty());
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(SpatialQueryTest, NullGeometry) {
    CNSpatialQuery query;
    GeometryPtr nullGeom;
    query.SetGeometry(std::move(nullGeom));
    EXPECT_EQ(query.GetGeometry(), nullptr);
}

TEST_F(SpatialQueryTest, AllOperations) {
    CNSpatialQuery query;
    
    std::vector<SpatialOperation> operations = {
        SpatialOperation::kIntersects,
        SpatialOperation::kContains,
        SpatialOperation::kWithin,
        SpatialOperation::kOverlaps,
        SpatialOperation::kTouches,
        SpatialOperation::kCrosses,
        SpatialOperation::kDisjoint,
        SpatialOperation::kEquals
    };
    
    for (auto op : operations) {
        query.SetOperation(op);
        EXPECT_EQ(query.GetOperation(), op);
    }
}

TEST_F(SpatialQueryTest, NegativeBufferDistance) {
    CNSpatialQuery query;
    query.SetBufferDistance(-10.0);
    EXPECT_DOUBLE_EQ(query.GetBufferDistance(), -10.0);
}

TEST_F(SpatialQueryTest, LargeSRID) {
    CNSpatialQuery query;
    query.SetSRID(999999);
    EXPECT_EQ(query.GetSRID(), 999999);
}

TEST_F(SpatialQueryTest, ChainedOperations) {
    auto point = Point::Create(0.0, 0.0);
    
    CNSpatialQuery query;
    query.SetGeometry(std::move(point))
         .SetOperation(SpatialOperation::kWithin)
         .SetBufferDistance(100.0)
         .SetSRID(4326);
    
    EXPECT_EQ(query.GetOperation(), SpatialOperation::kWithin);
    EXPECT_DOUBLE_EQ(query.GetBufferDistance(), 100.0);
    EXPECT_EQ(query.GetSRID(), 4326);
}
