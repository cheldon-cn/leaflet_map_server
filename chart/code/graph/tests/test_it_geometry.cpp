#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipoint.h"
#include "ogc/geom/multilinestring.h"
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace ogc;

class GeometryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(GeometryIntegrationTest, PointCreationAndValidation) {
    Coordinate coord(116.397428, 39.90923);
    auto point = Point::Create(coord);
    
    EXPECT_TRUE(point->IsValid());
    EXPECT_DOUBLE_EQ(point->GetX(), 116.397428);
    EXPECT_DOUBLE_EQ(point->GetY(), 39.90923);
}

TEST_F(GeometryIntegrationTest, LineStringEnvelopeIntersection) {
    std::vector<Coordinate> coords1 = {
        Coordinate(0, 0),
        Coordinate(10, 10),
        Coordinate(20, 20)
    };
    auto line1 = LineString::Create(coords1);
    
    std::vector<Coordinate> coords2 = {
        Coordinate(5, 5),
        Coordinate(15, 15),
        Coordinate(25, 25)
    };
    auto line2 = LineString::Create(coords2);
    
    EXPECT_TRUE(line1->IsValid());
    EXPECT_TRUE(line2->IsValid());
    
    Envelope env1 = line1->GetEnvelope();
    Envelope env2 = line2->GetEnvelope();
    
    EXPECT_TRUE(env1.Intersects(env2));
    
    Envelope queryEnv(5, 5, 15, 15);
    EXPECT_TRUE(env1.Intersects(queryEnv));
    EXPECT_TRUE(env2.Intersects(queryEnv));
}

TEST_F(GeometryIntegrationTest, PolygonContainsPoint) {
    std::vector<Coordinate> outerRing = {
        Coordinate(0, 0),
        Coordinate(10, 0),
        Coordinate(10, 10),
        Coordinate(0, 10),
        Coordinate(0, 0)
    };
    
    auto linearRing = LinearRing::Create(outerRing);
    auto polygon = Polygon::Create(std::move(linearRing));
    EXPECT_TRUE(polygon->IsValid());
    
    Coordinate insidePoint(5, 5);
    Coordinate outsidePoint(15, 15);
    Coordinate boundaryPoint(0, 5);
    
    auto insidePt = Point::Create(insidePoint);
    auto outsidePt = Point::Create(outsidePoint);
    auto boundaryPt = Point::Create(boundaryPoint);
    
    Envelope polyEnv = polygon->GetEnvelope();
    EXPECT_TRUE(polyEnv.Contains(insidePoint));
    EXPECT_FALSE(polyEnv.Contains(outsidePoint));
    EXPECT_TRUE(polyEnv.Contains(boundaryPoint));
}

TEST_F(GeometryIntegrationTest, MultiGeometryOperations) {
    std::vector<Coordinate> pointCoords;
    for (int i = 0; i < 5; ++i) {
        pointCoords.push_back(Coordinate(i * 10.0, i * 10.0));
    }
    
    auto multiPoint = MultiPoint::Create(pointCoords);
    EXPECT_TRUE(multiPoint->IsValid());
    EXPECT_EQ(multiPoint->GetNumGeometries(), 5u);
    
    std::vector<LineStringPtr> lines;
    for (int i = 0; i < 3; ++i) {
        std::vector<Coordinate> coords = {
            Coordinate(i * 10.0, 0),
            Coordinate(i * 10.0 + 5, 5)
        };
        lines.push_back(std::move(LineString::Create(coords)));
    }
    
    auto multiLineString = MultiLineString::Create(std::move(lines));
    EXPECT_TRUE(multiLineString->IsValid());
    EXPECT_EQ(multiLineString->GetNumGeometries(), 3u);
    
    Envelope multiPointEnv = multiPoint->GetEnvelope();
    Envelope multiLineEnv = multiLineString->GetEnvelope();
    
    EXPECT_TRUE(multiPointEnv.Intersects(multiLineEnv));
}

TEST_F(GeometryIntegrationTest, GeometryEnvelopeConsistency) {
    std::vector<Coordinate> coords = {
        Coordinate(100.0, 20.0),
        Coordinate(110.0, 25.0),
        Coordinate(120.0, 30.0),
        Coordinate(115.0, 35.0)
    };
    
    auto lineString = LineString::Create(coords);
    EXPECT_TRUE(lineString->IsValid());
    
    Envelope env = lineString->GetEnvelope();
    
    EXPECT_DOUBLE_EQ(env.GetMinX(), 100.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 120.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 20.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 35.0);
    
    for (const auto& coord : coords) {
        EXPECT_TRUE(env.Contains(coord));
    }
}

TEST_F(GeometryIntegrationTest, GeometryCloneAndCompare) {
    std::vector<Coordinate> coords = {
        Coordinate(0, 0),
        Coordinate(10, 10),
        Coordinate(20, 5)
    };
    
    auto original = LineString::Create(coords);
    EXPECT_TRUE(original->IsValid());
    
    auto cloned = original->Clone();
    EXPECT_TRUE(cloned->IsValid());
    
    LineString* clonedLine = dynamic_cast<LineString*>(cloned.get());
    ASSERT_NE(clonedLine, nullptr);
    EXPECT_EQ(original->GetNumPoints(), clonedLine->GetNumPoints());
    
    Envelope originalEnv = original->GetEnvelope();
    Envelope clonedEnv = clonedLine->GetEnvelope();
    
    EXPECT_DOUBLE_EQ(originalEnv.GetMinX(), clonedEnv.GetMinX());
    EXPECT_DOUBLE_EQ(originalEnv.GetMaxX(), clonedEnv.GetMaxX());
    EXPECT_DOUBLE_EQ(originalEnv.GetMinY(), clonedEnv.GetMinY());
    EXPECT_DOUBLE_EQ(originalEnv.GetMaxY(), clonedEnv.GetMaxY());
}

TEST_F(GeometryIntegrationTest, GeometryTypeConsistency) {
    auto point = Point::Create(Coordinate(10, 20));
    EXPECT_EQ(point->GetGeometryType(), GeomType::kPoint);
    
    std::vector<Coordinate> lineCoords = {Coordinate(0, 0), Coordinate(10, 10)};
    auto lineString = LineString::Create(lineCoords);
    EXPECT_EQ(lineString->GetGeometryType(), GeomType::kLineString);
    
    std::vector<Coordinate> polyCoords = {
        Coordinate(0, 0), Coordinate(10, 0),
        Coordinate(10, 10), Coordinate(0, 10), Coordinate(0, 0)
    };
    auto ring = LinearRing::Create(polyCoords);
    auto polygon = Polygon::Create(std::move(ring));
    EXPECT_EQ(polygon->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(GeometryIntegrationTest, EmptyGeometryHandling) {
    auto emptyPoint = Point::Create(Coordinate::Empty());
    EXPECT_TRUE(emptyPoint->IsEmpty());
    
    std::vector<Coordinate> emptyCoords;
    auto emptyLineString = LineString::Create(emptyCoords);
    EXPECT_TRUE(emptyLineString->IsEmpty());
    
    auto emptyPolygon = Polygon::Create();
    EXPECT_TRUE(emptyPolygon->IsEmpty());
}

TEST_F(GeometryIntegrationTest, LargeGeometryPerformance) {
    std::vector<Coordinate> coords;
    coords.reserve(10000);
    
    for (int i = 0; i < 10000; ++i) {
        coords.push_back(Coordinate(i * 0.001, i * 0.001));
    }
    
    auto largeLineString = LineString::Create(coords);
    EXPECT_TRUE(largeLineString->IsValid());
    EXPECT_EQ(largeLineString->GetNumPoints(), 10000u);
    
    Envelope env = largeLineString->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_NEAR(env.GetMaxX(), 9.999, 0.001);
    EXPECT_NEAR(env.GetMaxY(), 9.999, 0.001);
}

TEST_F(GeometryIntegrationTest, EnvelopeOperations) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    
    EXPECT_TRUE(env1.Intersects(env2));
    
    Envelope intersection = env1.Intersection(env2);
    EXPECT_DOUBLE_EQ(intersection.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(intersection.GetMinY(), 5.0);
    EXPECT_DOUBLE_EQ(intersection.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(intersection.GetMaxY(), 10.0);
    
    Envelope expanded = env1;
    expanded.ExpandToInclude(env2);
    EXPECT_DOUBLE_EQ(expanded.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(expanded.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(expanded.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(expanded.GetMaxY(), 15.0);
}

TEST_F(GeometryIntegrationTest, CoordinateOperations) {
    Coordinate c1(0, 0);
    Coordinate c2(3, 4);
    
    double distance = c1.Distance(c2);
    EXPECT_DOUBLE_EQ(distance, 5.0);
}
