#include <gtest/gtest.h>
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/factory.h"
#include "ogc/spatial_index.h"
#include "ogc/precision.h"
#include "ogc/visitor.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_factory = &GeometryFactory::GetInstance();
    }
    
    void TearDown() override {}
    
    GeometryFactory* m_factory;
};

TEST_F(IntegrationTest, FactoryToPointToWKT_CompleteFlow) {
    auto point = m_factory->CreatePoint(Coordinate(10.5, 20.5));
    
    ASSERT_NE(point, nullptr);
    EXPECT_EQ(point->GetGeometryType(), GeomType::kPoint);
    
    std::string wkt = point->AsText();
    EXPECT_TRUE(wkt.find("POINT") != std::string::npos);
    EXPECT_TRUE(wkt.find("10.5") != std::string::npos);
}

TEST_F(IntegrationTest, FactoryToLineString_CompleteFlow) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto lineString = m_factory->CreateLineString(coords);
    ASSERT_NE(lineString, nullptr);
    EXPECT_EQ(lineString->GetGeometryType(), GeomType::kLineString);
    EXPECT_NEAR(lineString->Length(), 40.0, 0.001);
}

TEST_F(IntegrationTest, MultiGeometryContainsCheck_Integration) {
    auto multiPolygon = MultiPolygon::Create();
    
    auto poly1 = Polygon::CreateRectangle(0, 0, 10, 10);
    auto poly2 = Polygon::CreateRectangle(20, 20, 30, 30);
    
    multiPolygon->AddPolygon(std::move(poly1));
    multiPolygon->AddPolygon(std::move(poly2));
    
    auto point1 = Point::Create(5, 5);
    auto point2 = Point::Create(25, 25);
    auto point3 = Point::Create(15, 15);
    
    EXPECT_TRUE(multiPolygon->Contains(point1.get()));
    EXPECT_TRUE(multiPolygon->Contains(point2.get()));
    EXPECT_FALSE(multiPolygon->Contains(point3.get()));
}

TEST_F(IntegrationTest, GeometryCollectionMixedTypes_Integration) {
    auto collection = GeometryCollection::Create();
    
    collection->AddGeometry(Point::Create(0, 0));
    collection->AddGeometry(LineString::Create({Coordinate(0, 0), Coordinate(10, 10)}));
    collection->AddGeometry(Polygon::CreateRectangle(0, 0, 5, 5));
    
    EXPECT_EQ(collection->GetNumGeometries(), static_cast<size_t>(3));
    
    auto point = collection->GetGeometryN(0);
    EXPECT_EQ(point->GetGeometryType(), GeomType::kPoint);
    
    auto line = collection->GetGeometryN(1);
    EXPECT_EQ(line->GetGeometryType(), GeomType::kLineString);
    
    auto polygon = collection->GetGeometryN(2);
    EXPECT_EQ(polygon->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(IntegrationTest, SpatialIndexWithGeometry_Integration) {
    RTree<std::shared_ptr<Point>> tree;
    
    for (int i = 0; i < 100; ++i) {
        auto point = Point::Create(i * 1.0, i * 1.0);
        Envelope env(i, i, i + 1, i + 1);
        tree.Insert(env, std::shared_ptr<Point>(point.release()));
    }
    
    Envelope queryEnv(45, 45, 55, 55);
    auto results = tree.Query(queryEnv);
    
    EXPECT_GE(results.size(), static_cast<size_t>(10));
}

TEST_F(IntegrationTest, PrecisionModelWithGeometry_Integration) {
    PrecisionModel model(100.0);
    
    Coordinate coord(10.123456, 20.654321);
    coord = model.MakePrecise(coord);
    
    EXPECT_NEAR(coord.x, 10.12, 0.001);
    EXPECT_NEAR(coord.y, 20.65, 0.001);
}

TEST_F(IntegrationTest, VisitorWithMultiGeometry_Integration) {
    auto multiPolygon = MultiPolygon::Create();
    multiPolygon->AddPolygon(Polygon::CreateRectangle(0, 0, 10, 10));
    multiPolygon->AddPolygon(Polygon::CreateRectangle(20, 20, 30, 30));
    
    GeometryStatistics visitor;
    multiPolygon->Apply(visitor);
    
    EXPECT_GT(visitor.GetTotalCount(), static_cast<size_t>(0));
}

TEST_F(IntegrationTest, EnvelopeCalculation_Integration) {
    auto lineString = LineString::Create({
        Coordinate(0, 0),
        Coordinate(10, 5),
        Coordinate(5, 15),
        Coordinate(-5, 10)
    });
    
    Envelope env = lineString->GetEnvelope();
    
    EXPECT_NEAR(env.GetMinX(), -5.0, 0.001);
    EXPECT_NEAR(env.GetMaxX(), 10.0, 0.001);
    EXPECT_NEAR(env.GetMinY(), 0.0, 0.001);
    EXPECT_NEAR(env.GetMaxY(), 15.0, 0.001);
}

TEST_F(IntegrationTest, GeometryClone_Integration) {
    auto original = Polygon::CreateRectangle(0, 0, 10, 10);
    auto cloned = original->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetGeometryType(), original->GetGeometryType());
}

TEST_F(IntegrationTest, MultiGeometryClone_Integration) {
    auto original = MultiPolygon::Create();
    original->AddPolygon(Polygon::CreateRectangle(0, 0, 10, 10));
    original->AddPolygon(Polygon::CreateRectangle(20, 20, 30, 30));
    
    auto cloned = original->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetGeometryType(), GeomType::kMultiPolygon);
}

TEST_F(IntegrationTest, GeometryEquality_Integration) {
    auto point1 = Point::Create(10.0, 20.0);
    auto point2 = Point::Create(10.0, 20.0);
    auto point3 = Point::Create(10.1, 20.0);
    
    EXPECT_TRUE(point1->Equals(point2.get()));
    EXPECT_FALSE(point1->Equals(point3.get()));
}

TEST_F(IntegrationTest, SpatialRelationships_Integration) {
    auto poly1 = Polygon::CreateRectangle(0, 0, 10, 10);
    auto poly2 = Polygon::CreateRectangle(5, 5, 15, 15);
    auto poly3 = Polygon::CreateRectangle(20, 20, 30, 30);
    
    EXPECT_TRUE(poly1->Intersects(poly2.get()));
    EXPECT_FALSE(poly1->Intersects(poly3.get()));
    
    auto point1 = Point::Create(5, 5);
    auto point2 = Point::Create(25, 25);
    
    EXPECT_TRUE(poly1->Contains(point1.get()));
    EXPECT_FALSE(poly1->Contains(point2.get()));
}

TEST_F(IntegrationTest, FactoryCreateShapes_Integration) {
    auto circle = m_factory->CreateCircle(0, 0, 10.0, 32);
    ASSERT_NE(circle, nullptr);
    EXPECT_NEAR(circle->Area(), 314.0, 10.0);
    
    auto triangle = m_factory->CreateTriangle(Coordinate(0, 0), Coordinate(10, 0), Coordinate(5, 10));
    ASSERT_NE(triangle, nullptr);
    EXPECT_NEAR(triangle->Area(), 50.0, 1.0);
    
    auto regularPoly = m_factory->CreateRegularPolygon(0, 0, 10.0, 6);
    ASSERT_NE(regularPoly, nullptr);
    EXPECT_GT(regularPoly->Area(), 0);
}

TEST_F(IntegrationTest, EmptyGeometryHandling_Integration) {
    auto emptyLine = LineString::Create();
    EXPECT_TRUE(emptyLine->IsEmpty());
    
    auto emptyPolygon = Polygon::Create();
    EXPECT_TRUE(emptyPolygon->IsEmpty());
    
    auto emptyMulti = MultiPoint::Create();
    EXPECT_TRUE(emptyMulti->IsEmpty());
}

TEST_F(IntegrationTest, GeometryTypeConversion_Integration) {
    Coordinate coord2d(10, 20);
    Coordinate coord3d(10, 20, 30);
    
    EXPECT_FALSE(coord2d.Is3D());
    EXPECT_TRUE(coord3d.Is3D());
    
    auto point2d = Point::Create(coord2d);
    auto point3d = Point::Create(coord3d);
    
    EXPECT_EQ(point2d->GetCoordinateDimension(), 2);
    EXPECT_EQ(point3d->GetCoordinateDimension(), 3);
}

TEST_F(IntegrationTest, QuadtreeIntegration_CompleteFlow) {
    Quadtree<std::string>::Config config;
    config.bounds = Envelope(0, 0, 40, 40);
    Quadtree<std::string> tree(config);
    
    tree.Insert(Envelope(0, 0, 10, 10), "A");
    tree.Insert(Envelope(10, 10, 20, 20), "B");
    tree.Insert(Envelope(20, 20, 30, 30), "C");
    
    auto results = tree.Query(Envelope(5, 5, 25, 25));
    EXPECT_GE(results.size(), static_cast<size_t>(2));
}

TEST_F(IntegrationTest, GridIndexIntegration_CompleteFlow) {
    typename GridIndex<int>::Config config;
    config.gridX = 100;
    config.gridY = 100;
    config.bounds = Envelope(0, 0, 100, 100);
    GridIndex<int> index(config);
    
    index.Insert(Envelope(5, 5, 15, 15), 1);
    index.Insert(Envelope(25, 25, 35, 35), 2);
    index.Insert(Envelope(55, 55, 65, 65), 3);
    
    auto results = index.Query(Envelope(0, 0, 20, 20));
    EXPECT_GE(results.size(), static_cast<size_t>(1));
}

TEST_F(IntegrationTest, GeometryValidity_Integration) {
    auto validPolygon = Polygon::CreateRectangle(0, 0, 10, 10);
    EXPECT_TRUE(validPolygon->IsValid());
    
    auto validRing = LinearRing::Create({
        Coordinate(0, 0),
        Coordinate(10, 0),
        Coordinate(10, 10),
        Coordinate(0, 10),
        Coordinate(0, 0)
    });
    EXPECT_TRUE(validRing->IsValid());
}

TEST_F(IntegrationTest, GeometryDimension_Integration) {
    auto point = Point::Create(0, 0);
    EXPECT_EQ(static_cast<int>(point->GetDimension()), 0);
    
    auto line = LineString::Create({Coordinate(0, 0), Coordinate(10, 10)});
    EXPECT_EQ(static_cast<int>(line->GetDimension()), 1);
    
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    EXPECT_EQ(static_cast<int>(polygon->GetDimension()), 2);
}

}
}
