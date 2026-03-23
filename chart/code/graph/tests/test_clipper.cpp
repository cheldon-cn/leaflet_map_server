#include <gtest/gtest.h>
#include "ogc/draw/clipper.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using namespace ogc;

class ClipperTest : public ::testing::Test {
protected:
    void SetUp() override {
        clipper = Clipper::Create();
        ASSERT_NE(clipper, nullptr);
    }
    
    void TearDown() override {
        clipper.reset();
    }
    
    ClipperPtr clipper;
};

TEST_F(ClipperTest, DefaultConstructor) {
    ClipperPtr defaultClipper = Clipper::Create();
    EXPECT_NE(defaultClipper, nullptr);
    EXPECT_FALSE(defaultClipper->HasClipEnvelope());
}

TEST_F(ClipperTest, ConstructorWithEnvelope) {
    Envelope env(0, 0, 100, 100);
    ClipperPtr envClipper = Clipper::Create(env);
    EXPECT_NE(envClipper, nullptr);
    EXPECT_TRUE(envClipper->HasClipEnvelope());
    EXPECT_DOUBLE_EQ(envClipper->GetClipEnvelope().GetMinX(), 0);
    EXPECT_DOUBLE_EQ(envClipper->GetClipEnvelope().GetMinY(), 0);
    EXPECT_DOUBLE_EQ(envClipper->GetClipEnvelope().GetMaxX(), 100);
    EXPECT_DOUBLE_EQ(envClipper->GetClipEnvelope().GetMaxY(), 100);
}

TEST_F(ClipperTest, SetClipEnvelope) {
    Envelope env(10, 20, 50, 60);
    clipper->SetClipEnvelope(env);
    EXPECT_TRUE(clipper->HasClipEnvelope());
    
    Envelope retrieved = clipper->GetClipEnvelope();
    EXPECT_DOUBLE_EQ(retrieved.GetMinX(), 10);
    EXPECT_DOUBLE_EQ(retrieved.GetMinY(), 20);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxX(), 50);
    EXPECT_DOUBLE_EQ(retrieved.GetMaxY(), 60);
}

TEST_F(ClipperTest, ClearClipEnvelope) {
    Envelope env(0, 0, 100, 100);
    clipper->SetClipEnvelope(env);
    EXPECT_TRUE(clipper->HasClipEnvelope());
    
    clipper->ClearClipEnvelope();
    EXPECT_FALSE(clipper->HasClipEnvelope());
}

TEST_F(ClipperTest, TestPointInside) {
    Envelope env(0, 0, 100, 100);
    clipper->SetClipEnvelope(env);
    
    EXPECT_EQ(clipper->TestPoint(50, 50), ClipResult::kInside);
    EXPECT_EQ(clipper->TestPoint(0, 0), ClipResult::kInside);
    EXPECT_EQ(clipper->TestPoint(100, 100), ClipResult::kInside);
}

TEST_F(ClipperTest, TestPointOutside) {
    Envelope env(0, 0, 100, 100);
    clipper->SetClipEnvelope(env);
    
    EXPECT_EQ(clipper->TestPoint(150, 50), ClipResult::kOutside);
    EXPECT_EQ(clipper->TestPoint(50, 150), ClipResult::kOutside);
    EXPECT_EQ(clipper->TestPoint(-10, 50), ClipResult::kOutside);
}

TEST_F(ClipperTest, TestPointWithCoordinate) {
    Envelope env(0, 0, 100, 100);
    clipper->SetClipEnvelope(env);
    
    Coordinate inside(50, 50);
    Coordinate outside(150, 150);
    
    EXPECT_EQ(clipper->TestPoint(inside), ClipResult::kInside);
    EXPECT_EQ(clipper->TestPoint(outside), ClipResult::kOutside);
}

TEST_F(ClipperTest, TestEnvelopeInside) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    Envelope inside(10, 10, 90, 90);
    EXPECT_EQ(clipper->TestEnvelope(inside), ClipResult::kInside);
}

TEST_F(ClipperTest, TestEnvelopeOutside) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    Envelope outside(110, 110, 150, 150);
    EXPECT_EQ(clipper->TestEnvelope(outside), ClipResult::kOutside);
}

TEST_F(ClipperTest, TestEnvelopePartial) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    Envelope partial(50, 50, 150, 150);
    EXPECT_EQ(clipper->TestEnvelope(partial), ClipResult::kPartial);
}

TEST_F(ClipperTest, IsPointInside) {
    Envelope env(0, 0, 100, 100);
    clipper->SetClipEnvelope(env);
    
    EXPECT_TRUE(clipper->IsPointInside(50, 50));
    EXPECT_TRUE(clipper->IsPointInside(0, 0));
    EXPECT_TRUE(clipper->IsPointInside(100, 100));
    EXPECT_FALSE(clipper->IsPointInside(150, 150));
    EXPECT_FALSE(clipper->IsPointInside(-10, -10));
}

TEST_F(ClipperTest, SetTolerance) {
    clipper->SetTolerance(0.5);
    EXPECT_DOUBLE_EQ(clipper->GetTolerance(), 0.5);
    
    clipper->SetTolerance(1.0);
    EXPECT_DOUBLE_EQ(clipper->GetTolerance(), 1.0);
}

TEST_F(ClipperTest, ClipEnvelopeInside) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    Envelope inside(10, 10, 90, 90);
    Envelope clipped = clipper->ClipEnvelope(inside);
    
    EXPECT_FALSE(clipped.IsNull());
    EXPECT_DOUBLE_EQ(clipped.GetMinX(), 10);
    EXPECT_DOUBLE_EQ(clipped.GetMinY(), 10);
    EXPECT_DOUBLE_EQ(clipped.GetMaxX(), 90);
    EXPECT_DOUBLE_EQ(clipped.GetMaxY(), 90);
}

TEST_F(ClipperTest, ClipEnvelopeOutside) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    Envelope outside(110, 110, 150, 150);
    Envelope clipped = clipper->ClipEnvelope(outside);
    
    EXPECT_TRUE(clipped.IsNull());
}

TEST_F(ClipperTest, ClipEnvelopePartial) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    Envelope partial(50, 50, 150, 150);
    Envelope clipped = clipper->ClipEnvelope(partial);
    
    EXPECT_FALSE(clipped.IsNull());
    EXPECT_DOUBLE_EQ(clipped.GetMinX(), 50);
    EXPECT_DOUBLE_EQ(clipped.GetMinY(), 50);
    EXPECT_DOUBLE_EQ(clipped.GetMaxX(), 100);
    EXPECT_DOUBLE_EQ(clipped.GetMaxY(), 100);
}

TEST_F(ClipperTest, ClipPoint) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    PointPtr insidePoint = Point::Create(50, 50);
    GeometryPtr clippedInside = clipper->ClipGeometry(insidePoint.get());
    EXPECT_NE(clippedInside, nullptr);
    
    PointPtr outsidePoint = Point::Create(150, 150);
    GeometryPtr clippedOutside = clipper->ClipGeometry(outsidePoint.get());
    EXPECT_EQ(clippedOutside, nullptr);
}

TEST_F(ClipperTest, ClipLineString) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    CoordinateList coords;
    coords.push_back(Coordinate(-10, 50));
    coords.push_back(Coordinate(50, 50));
    coords.push_back(Coordinate(150, 50));
    
    LineStringPtr line = LineString::Create(coords);
    GeometryPtr clipped = clipper->ClipGeometry(line.get());
    
    EXPECT_NE(clipped, nullptr);
}

TEST_F(ClipperTest, ClipPolygon) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    PolygonPtr polygon = Polygon::CreateRectangle(50, 50, 150, 150);
    GeometryPtr clipped = clipper->ClipGeometry(polygon.get());
    
    EXPECT_NE(clipped, nullptr);
}

TEST_F(ClipperTest, ClipGeometriesEmpty) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    std::vector<const Geometry*> geometries;
    std::vector<GeometryPtr> clipped = clipper->ClipGeometries(geometries);
    
    EXPECT_TRUE(clipped.empty());
}

TEST_F(ClipperTest, ClipGeometriesMultiple) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    PointPtr p1 = Point::Create(50, 50);
    PointPtr p2 = Point::Create(150, 150);
    
    std::vector<const Geometry*> geometries;
    geometries.push_back(p1.get());
    geometries.push_back(p2.get());
    
    std::vector<GeometryPtr> clipped = clipper->ClipGeometries(geometries);
    EXPECT_GE(clipped.size(), 1);
}

TEST_F(ClipperTest, TestGeometryPoint) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    PointPtr insidePoint = Point::Create(50, 50);
    EXPECT_EQ(clipper->TestGeometry(insidePoint.get()), ClipResult::kInside);
    
    PointPtr outsidePoint = Point::Create(150, 150);
    EXPECT_EQ(clipper->TestGeometry(outsidePoint.get()), ClipResult::kOutside);
}

TEST_F(ClipperTest, TestGeometryLineString) {
    Envelope clipEnv(0, 0, 100, 100);
    clipper->SetClipEnvelope(clipEnv);
    
    CoordinateList insideCoords;
    insideCoords.push_back(Coordinate(10, 10));
    insideCoords.push_back(Coordinate(90, 90));
    LineStringPtr insideLine = LineString::Create(insideCoords);
    EXPECT_EQ(clipper->TestGeometry(insideLine.get()), ClipResult::kInside);
    
    CoordinateList partialCoords;
    partialCoords.push_back(Coordinate(-10, 50));
    partialCoords.push_back(Coordinate(150, 50));
    LineStringPtr partialLine = LineString::Create(partialCoords);
    EXPECT_EQ(clipper->TestGeometry(partialLine.get()), ClipResult::kPartial);
}
