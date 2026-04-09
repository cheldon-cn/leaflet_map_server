#include <gtest/gtest.h>
#include "ogc/geom/factory.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class GeometryFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(GeometryFactoryTest, GetInstance_ReturnsSingleton) {
    auto& factory1 = GeometryFactory::GetInstance();
    auto& factory2 = GeometryFactory::GetInstance();
    
    EXPECT_EQ(&factory1, &factory2);
}

TEST_F(GeometryFactoryTest, CreatePoint_2D_ReturnsValidPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto point = factory.CreatePoint(1.0, 2.0);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_FALSE(point->Is3D());
}

TEST_F(GeometryFactoryTest, CreatePoint_3D_ReturnsValidPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto point = factory.CreatePoint(1.0, 2.0, 3.0);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(point->GetZ(), 3.0);
    EXPECT_TRUE(point->Is3D());
}

TEST_F(GeometryFactoryTest, CreatePoint_FromCoordinate_ReturnsValidPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    Coordinate coord(1.0, 2.0, 3.0);
    auto point = factory.CreatePoint(coord);
    
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(point->GetZ(), 3.0);
}

TEST_F(GeometryFactoryTest, CreateLineString_ReturnsValidLineString) {
    auto& factory = GeometryFactory::GetInstance();
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    
    auto line = factory.CreateLineString(coords);
    ASSERT_NE(line, nullptr);
    EXPECT_EQ(line->GetNumPoints(), 3);
}

TEST_F(GeometryFactoryTest, CreateLinearRing_ReturnsValidLinearRing) {
    auto& factory = GeometryFactory::GetInstance();
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = factory.CreateLinearRing(coords);
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsClosed());
}

TEST_F(GeometryFactoryTest, CreatePolygon_FromExteriorRing_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto polygon = factory.CreatePolygon(coords);
    ASSERT_NE(polygon, nullptr);
    EXPECT_DOUBLE_EQ(polygon->Area(), 100.0);
}

TEST_F(GeometryFactoryTest, CreatePolygon_FromMultipleRings_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    std::vector<CoordinateList> rings;
    
    CoordinateList exterior;
    exterior.emplace_back(0, 0);
    exterior.emplace_back(20, 0);
    exterior.emplace_back(20, 20);
    exterior.emplace_back(0, 20);
    exterior.emplace_back(0, 0);
    rings.push_back(exterior);
    
    CoordinateList hole;
    hole.emplace_back(5, 5);
    hole.emplace_back(15, 5);
    hole.emplace_back(15, 15);
    hole.emplace_back(5, 15);
    hole.emplace_back(5, 5);
    rings.push_back(hole);
    
    auto polygon = factory.CreatePolygon(rings);
    ASSERT_NE(polygon, nullptr);
    EXPECT_EQ(polygon->GetNumInteriorRings(), 1);
}

TEST_F(GeometryFactoryTest, CreateMultiPoint_ReturnsValidMultiPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 20);
    
    auto multiPoint = factory.CreateMultiPoint(coords);
    ASSERT_NE(multiPoint, nullptr);
    EXPECT_EQ(multiPoint->GetNumPoints(), 3);
}

TEST_F(GeometryFactoryTest, CreateRectangle_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto polygon = factory.CreateRectangle(0, 0, 10, 20);
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_DOUBLE_EQ(polygon->Area(), 200.0);
    EXPECT_DOUBLE_EQ(polygon->GetPerimeter(), 60.0);
}

TEST_F(GeometryFactoryTest, CreateCircle_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto polygon = factory.CreateCircle(0, 0, 10, 32);
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_NEAR(polygon->Area(), PI * 100.0, 5.0);
}

TEST_F(GeometryFactoryTest, CreateRegularPolygon_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto hexagon = factory.CreateRegularPolygon(0, 0, 10, 6);
    
    ASSERT_NE(hexagon, nullptr);
    EXPECT_EQ(hexagon->GetNumCoordinates(), 7);
}

TEST_F(GeometryFactoryTest, CreateTriangle_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    Coordinate p1(0, 0);
    Coordinate p2(10, 0);
    Coordinate p3(5, 10);
    
    auto polygon = factory.CreateTriangle(p1, p2, p3);
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_DOUBLE_EQ(polygon->Area(), 50.0);
}

TEST_F(GeometryFactoryTest, CreateEmptyGeometry_Point_ReturnsEmptyPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto emptyPoint = factory.CreateEmptyGeometry(GeomType::kPoint);
    ASSERT_NE(emptyPoint, nullptr);
    EXPECT_TRUE(emptyPoint->IsEmpty());
    EXPECT_EQ(emptyPoint->GetGeometryType(), GeomType::kPoint);
}

TEST_F(GeometryFactoryTest, CreateEmptyGeometry_LineString_ReturnsEmptyLineString) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto emptyLine = factory.CreateEmptyGeometry(GeomType::kLineString);
    ASSERT_NE(emptyLine, nullptr);
    EXPECT_TRUE(emptyLine->IsEmpty());
    EXPECT_EQ(emptyLine->GetGeometryType(), GeomType::kLineString);
}

TEST_F(GeometryFactoryTest, CreateEmptyGeometry_Polygon_ReturnsEmptyPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto emptyPolygon = factory.CreateEmptyGeometry(GeomType::kPolygon);
    ASSERT_NE(emptyPolygon, nullptr);
    EXPECT_TRUE(emptyPolygon->IsEmpty());
    EXPECT_EQ(emptyPolygon->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(GeometryFactoryTest, SetDefaultSRID_UpdatesSRID) {
    auto& factory = GeometryFactory::GetInstance();
    
    int originalSRID = factory.GetDefaultSRID();
    factory.SetDefaultSRID(4326);
    EXPECT_EQ(factory.GetDefaultSRID(), 4326);
    
    factory.SetDefaultSRID(originalSRID);
}

TEST_F(GeometryFactoryTest, CreatePointFromWKT_ReturnsValidPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto point = factory.CreatePointFromWKT("POINT(1 2)");
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
}

TEST_F(GeometryFactoryTest, CreatePointFromWKT_3D_ReturnsValidPoint) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto point = factory.CreatePointFromWKT("POINT Z(1 2 3)");
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(point->GetZ(), 3.0);
    EXPECT_TRUE(point->Is3D());
}

TEST_F(GeometryFactoryTest, CreateLineStringFromWKT_ReturnsValidLineString) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto line = factory.CreateLineStringFromWKT("LINESTRING(0 0, 10 10, 20 0)");
    ASSERT_NE(line, nullptr);
    EXPECT_EQ(line->GetNumPoints(), 3);
}

TEST_F(GeometryFactoryTest, CreatePolygonFromWKT_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto polygon = factory.CreatePolygonFromWKT("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    ASSERT_NE(polygon, nullptr);
    EXPECT_DOUBLE_EQ(polygon->Area(), 100.0);
}

TEST_F(GeometryFactoryTest, CreatePolygonFromWKT_WithHole_ReturnsValidPolygon) {
    auto& factory = GeometryFactory::GetInstance();
    
    auto polygon = factory.CreatePolygonFromWKT(
        "POLYGON((0 0, 20 0, 20 20, 0 20, 0 0), (5 5, 15 5, 15 15, 5 15, 5 5))");
    ASSERT_NE(polygon, nullptr);
    EXPECT_EQ(polygon->GetNumInteriorRings(), 1);
}

TEST_F(GeometryFactoryTest, FromWKT_Point_ReturnsValidGeometry) {
    auto& factory = GeometryFactory::GetInstance();
    
    GeometryPtr geom;
    GeomResult result = factory.FromWKT("POINT(1 2)", geom);
    
    EXPECT_EQ(result, GeomResult::kSuccess);
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kPoint);
}

TEST_F(GeometryFactoryTest, FromWKT_LineString_ReturnsValidGeometry) {
    auto& factory = GeometryFactory::GetInstance();
    
    GeometryPtr geom;
    GeomResult result = factory.FromWKT("LINESTRING(0 0, 10 10)", geom);
    
    EXPECT_EQ(result, GeomResult::kSuccess);
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kLineString);
}

TEST_F(GeometryFactoryTest, FromWKT_Polygon_ReturnsValidGeometry) {
    auto& factory = GeometryFactory::GetInstance();
    
    GeometryPtr geom;
    GeomResult result = factory.FromWKT("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))", geom);
    
    EXPECT_EQ(result, GeomResult::kSuccess);
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(GeometryFactoryTest, FromWKT_InvalidWKT_ReturnsError) {
    auto& factory = GeometryFactory::GetInstance();
    
    GeometryPtr geom;
    GeomResult result = factory.FromWKT("INVALID WKT", geom);
    
    EXPECT_NE(result, GeomResult::kSuccess);
}

TEST_F(GeometryFactoryTest, FromWKT_EmptyString_ReturnsError) {
    auto& factory = GeometryFactory::GetInstance();
    
    GeometryPtr geom;
    GeomResult result = factory.FromWKT("", geom);
    
    EXPECT_NE(result, GeomResult::kSuccess);
}

}
}
