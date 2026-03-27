#include <gtest/gtest.h>
#include "ogc/visitor.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class TestVisitor : public GeometryConstVisitor {
public:
    void VisitPoint(const Point*) override { m_pointCount++; }
    void VisitLineString(const LineString*) override { m_lineCount++; }
    void VisitPolygon(const Polygon*) override { m_polygonCount++; }
    void VisitMultiPoint(const MultiPoint*) override { m_multiPointCount++; }
    void VisitMultiLineString(const MultiLineString*) override { m_multiLineCount++; }
    void VisitMultiPolygon(const MultiPolygon*) override { m_multiPolygonCount++; }
    void VisitGeometryCollection(const GeometryCollection*) override { m_collectionCount++; }
    
    int GetPointCount() const { return m_pointCount; }
    int GetLineCount() const { return m_lineCount; }
    int GetPolygonCount() const { return m_polygonCount; }
    int GetMultiPointCount() const { return m_multiPointCount; }
    int GetMultiLineCount() const { return m_multiLineCount; }
    int GetMultiPolygonCount() const { return m_multiPolygonCount; }
    int GetCollectionCount() const { return m_collectionCount; }
    
    void Reset() {
        m_pointCount = 0;
        m_lineCount = 0;
        m_polygonCount = 0;
        m_multiPointCount = 0;
        m_multiLineCount = 0;
        m_multiPolygonCount = 0;
        m_collectionCount = 0;
    }
    
private:
    int m_pointCount = 0;
    int m_lineCount = 0;
    int m_polygonCount = 0;
    int m_multiPointCount = 0;
    int m_multiLineCount = 0;
    int m_multiPolygonCount = 0;
    int m_collectionCount = 0;
};

class TestMutableVisitor : public GeometryVisitor {
public:
    void VisitPoint(Point* point) override { 
        m_visitedTypes.push_back(point->GetGeometryType());
    }
    void VisitLineString(LineString* line) override { 
        m_visitedTypes.push_back(line->GetGeometryType());
    }
    void VisitPolygon(Polygon* polygon) override { 
        m_visitedTypes.push_back(polygon->GetGeometryType());
    }
    
    size_t GetVisitedCount() const { return m_visitedTypes.size(); }
    const std::vector<GeomType>& GetVisitedTypes() const { return m_visitedTypes; }
    
private:
    std::vector<GeomType> m_visitedTypes;
};

class GeometryVisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_visitor.Reset();
    }
    
    void TearDown() override {}
    
    TestVisitor m_visitor;
    TestMutableVisitor m_mutableVisitor;
};

TEST_F(GeometryVisitorTest, VisitPoint_CallsCorrectMethod) {
    auto point = Point::Create(1, 2);
    point->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetPointCount(), 1);
    EXPECT_EQ(m_visitor.GetLineCount(), 0);
    EXPECT_EQ(m_visitor.GetPolygonCount(), 0);
}

TEST_F(GeometryVisitorTest, VisitLineString_CallsCorrectMethod) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto line = LineString::Create(coords);
    
    line->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetPointCount(), 0);
    EXPECT_EQ(m_visitor.GetLineCount(), 1);
    EXPECT_EQ(m_visitor.GetPolygonCount(), 0);
}

TEST_F(GeometryVisitorTest, VisitPolygon_CallsCorrectMethod) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    polygon->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetPointCount(), 0);
    EXPECT_EQ(m_visitor.GetLineCount(), 0);
    EXPECT_EQ(m_visitor.GetPolygonCount(), 1);
}

TEST_F(GeometryVisitorTest, VisitMultiPoint_CallsCorrectMethod) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    multiPoint->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetMultiPointCount(), 1);
}

TEST_F(GeometryVisitorTest, VisitMultiLineString_CallsCorrectMethod) {
    std::vector<LineStringPtr> lines;
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    multiLine->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetMultiLineCount(), 1);
}

TEST_F(GeometryVisitorTest, VisitMultiPolygon_CallsCorrectMethod) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    multiPolygon->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetMultiPolygonCount(), 1);
}

TEST_F(GeometryVisitorTest, VisitGeometryCollection_CallsCorrectMethod) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    collection->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetCollectionCount(), 1);
}

TEST_F(GeometryVisitorTest, VisitMultipleGeometries_CountsCorrectly) {
    auto point1 = Point::Create(1, 2);
    auto point2 = Point::Create(3, 4);
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    
    point1->Apply(m_visitor);
    point2->Apply(m_visitor);
    polygon->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetPointCount(), 2);
    EXPECT_EQ(m_visitor.GetPolygonCount(), 1);
}

TEST_F(GeometryVisitorTest, MutableVisitor_CanModifyGeometry) {
    auto point = Point::Create(1, 2);
    point->Apply(m_mutableVisitor);
    
    EXPECT_EQ(m_mutableVisitor.GetVisitedCount(), 1);
    EXPECT_EQ(m_mutableVisitor.GetVisitedTypes()[0], GeomType::kPoint);
}

TEST_F(GeometryVisitorTest, AreaCalculator_Point_ReturnsZero) {
    AreaCalculator calculator;
    auto point = Point::Create(1, 2);
    point->Apply(calculator);
    
    EXPECT_DOUBLE_EQ(calculator.GetArea(), 0.0);
}

TEST_F(GeometryVisitorTest, AreaCalculator_LineString_ReturnsZero) {
    AreaCalculator calculator;
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto line = LineString::Create(coords);
    
    line->Apply(calculator);
    
    EXPECT_DOUBLE_EQ(calculator.GetArea(), 0.0);
}

TEST_F(GeometryVisitorTest, AreaCalculator_Polygon_ReturnsArea) {
    AreaCalculator calculator;
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 20);
    
    polygon->Apply(calculator);
    
    EXPECT_DOUBLE_EQ(calculator.GetArea(), 200.0);
}

TEST_F(GeometryVisitorTest, AreaCalculator_MultiPolygon_ReturnsTotalArea) {
    AreaCalculator calculator;
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    multiPolygon->Apply(calculator);
    
    EXPECT_DOUBLE_EQ(calculator.GetArea(), 100.0 + 400.0);
}

TEST_F(GeometryVisitorTest, GeometryStatistics_CountsCorrectly) {
    GeometryStatistics stats;
    
    auto point = Point::Create(1, 2);
    auto line = LineString::Create();
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    
    point->Apply(stats);
    line->Apply(stats);
    polygon->Apply(stats);
    
    EXPECT_EQ(stats.GetTotalCount(), 3);
    EXPECT_EQ(stats.GetTypeCount(GeomType::kPoint), 1);
    EXPECT_EQ(stats.GetTypeCount(GeomType::kLineString), 1);
    EXPECT_EQ(stats.GetTypeCount(GeomType::kPolygon), 1);
}

TEST_F(GeometryVisitorTest, GeometryStatistics_UnknownType_ReturnsZero) {
    GeometryStatistics stats;
    
    EXPECT_EQ(stats.GetTypeCount(GeomType::kPoint), 0);
}

TEST_F(GeometryVisitorTest, VisitLinearRing_CallsLineStringMethod) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    auto ring = LinearRing::Create(coords);
    
    ring->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetLineCount(), 1);
}

TEST_F(GeometryVisitorTest, EmptyGeometry_HandlesCorrectly) {
    auto emptyPoint = Point::Create(Coordinate::Empty());
    emptyPoint->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetPointCount(), 1);
}

TEST_F(GeometryVisitorTest, NestedGeometryCollection_VisitsAll) {
    std::vector<GeometryPtr> innerGeometries;
    innerGeometries.push_back(Point::Create(0, 0));
    innerGeometries.push_back(Point::Create(10, 10));
    auto innerCollection = GeometryCollection::Create(std::move(innerGeometries));
    
    std::vector<GeometryPtr> outerGeometries;
    outerGeometries.push_back(std::move(innerCollection));
    outerGeometries.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    auto outerCollection = GeometryCollection::Create(std::move(outerGeometries));
    
    outerCollection->Apply(m_visitor);
    
    EXPECT_EQ(m_visitor.GetCollectionCount(), 2);
    EXPECT_EQ(m_visitor.GetPointCount(), 2);
    EXPECT_EQ(m_visitor.GetPolygonCount(), 1);
}

}
}
