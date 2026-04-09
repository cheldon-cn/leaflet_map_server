#include <gtest/gtest.h>
#include "ogc/geom/multipolygon.h"
#include "ogc/geom/point.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class MultiPolygonTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MultiPolygonTest, Create_Default_ReturnsEmptyMultiPolygon) {
    auto multiPolygon = MultiPolygon::Create();
    ASSERT_NE(multiPolygon, nullptr);
    EXPECT_TRUE(multiPolygon->IsEmpty());
    EXPECT_EQ(multiPolygon->GetNumPolygons(), 0);
}

TEST_F(MultiPolygonTest, CreateFromPolygons_ReturnsValidMultiPolygon) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(20, 20, 30, 30));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    ASSERT_NE(multiPolygon, nullptr);
    EXPECT_EQ(multiPolygon->GetNumPolygons(), 2);
    EXPECT_FALSE(multiPolygon->IsEmpty());
}

TEST_F(MultiPolygonTest, GetGeometryType_ReturnsMultiPolygon) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_EQ(multiPolygon->GetGeometryType(), GeomType::kMultiPolygon);
}

TEST_F(MultiPolygonTest, GetGeometryTypeString_ReturnsCorrectString) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_STREQ(multiPolygon->GetGeometryTypeString(), "MULTIPOLYGON");
}

TEST_F(MultiPolygonTest, GetDimension_ReturnsSurfaceDimension) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_EQ(multiPolygon->GetDimension(), Dimension::Surface);
}

TEST_F(MultiPolygonTest, GetCoordinateDimension_ReturnsCorrectValue) {
    std::vector<PolygonPtr> polygons2D;
    polygons2D.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    auto multiPolygon2D = MultiPolygon::Create(std::move(polygons2D));
    EXPECT_EQ(multiPolygon2D->GetCoordinateDimension(), 2);
}

TEST_F(MultiPolygonTest, Is3D_ReturnsCorrectValue) {
    std::vector<PolygonPtr> polygons2D;
    polygons2D.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    auto multiPolygon2D = MultiPolygon::Create(std::move(polygons2D));
    EXPECT_FALSE(multiPolygon2D->Is3D());
}

TEST_F(MultiPolygonTest, IsMeasured_ReturnsCorrectValue) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    EXPECT_FALSE(multiPolygon->IsMeasured());
}

TEST_F(MultiPolygonTest, AddPolygon_AddsPolygon) {
    auto multiPolygon = MultiPolygon::Create();
    multiPolygon->AddPolygon(Polygon::CreateRectangle(0, 0, 10, 10));
    
    EXPECT_EQ(multiPolygon->GetNumPolygons(), 1);
}

TEST_F(MultiPolygonTest, RemovePolygon_RemovesCorrectPolygon) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(20, 20, 30, 30));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    multiPolygon->RemovePolygon(0);
    
    EXPECT_EQ(multiPolygon->GetNumPolygons(), 1);
}

TEST_F(MultiPolygonTest, RemovePolygon_InvalidIndex_ThrowsException) {
    auto multiPolygon = MultiPolygon::Create();
    multiPolygon->AddPolygon(Polygon::CreateRectangle(0, 0, 10, 10));
    
    EXPECT_THROW(multiPolygon->RemovePolygon(5), GeometryException);
}

TEST_F(MultiPolygonTest, Clear_RemovesAllPolygons) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    multiPolygon->Clear();
    
    EXPECT_TRUE(multiPolygon->IsEmpty());
    EXPECT_EQ(multiPolygon->GetNumPolygons(), 0);
}

TEST_F(MultiPolygonTest, GetPolygonN_ReturnsCorrectPolygon) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(20, 20, 30, 30));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    const Polygon* poly0 = multiPolygon->GetPolygonN(0);
    ASSERT_NE(poly0, nullptr);
    EXPECT_DOUBLE_EQ(poly0->Area(), 100.0);
    
    const Polygon* poly1 = multiPolygon->GetPolygonN(1);
    ASSERT_NE(poly1, nullptr);
    EXPECT_DOUBLE_EQ(poly1->Area(), 100.0);
}

TEST_F(MultiPolygonTest, GetPolygonN_InvalidIndex_ThrowsException) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_THROW(multiPolygon->GetPolygonN(0), GeometryException);
}

TEST_F(MultiPolygonTest, Area_CalculatesCorrectly) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    EXPECT_DOUBLE_EQ(multiPolygon->Area(), 100.0 + 400.0);
}

TEST_F(MultiPolygonTest, Length_CalculatesCorrectly) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    EXPECT_DOUBLE_EQ(multiPolygon->Length(), 40.0 + 80.0);
}

TEST_F(MultiPolygonTest, GetPerimeter_CalculatesCorrectly) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    EXPECT_DOUBLE_EQ(multiPolygon->GetPerimeter(), 40.0 + 80.0);
}

TEST_F(MultiPolygonTest, Contains_ReturnsCorrectValue) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    auto point = Point::Create(10, 10);
    EXPECT_TRUE(multiPolygon->Contains(point.get()));
    
    auto outsidePoint = Point::Create(30, 30);
    EXPECT_FALSE(multiPolygon->Contains(outsidePoint.get()));
}

TEST_F(MultiPolygonTest, Intersects_ReturnsCorrectValue) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    auto otherPolygon = Polygon::CreateRectangle(10, 10, 30, 30);
    EXPECT_TRUE(multiPolygon->Intersects(otherPolygon.get()));
    
    auto disjointPolygon = Polygon::CreateRectangle(30, 30, 40, 40);
    EXPECT_FALSE(multiPolygon->Intersects(disjointPolygon.get()));
}

TEST_F(MultiPolygonTest, Union_ReturnsCombinedPolygon) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(10, 0, 20, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    auto unionResult = multiPolygon->Union();
    
    ASSERT_NE(unionResult, nullptr);
}

TEST_F(MultiPolygonTest, RemoveHoles_ReturnsPolygonsWithoutHoles) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    std::vector<PolygonPtr> polygons;
    polygons.push_back(std::move(polygon));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    auto noHoles = multiPolygon->RemoveHoles();
    
    ASSERT_NE(noHoles, nullptr);
    const Polygon* firstPoly = noHoles->GetPolygonN(0);
    EXPECT_EQ(firstPoly->GetNumInteriorRings(), 0);
}

TEST_F(MultiPolygonTest, GetNumCoordinates_ReturnsCorrectValue) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(20, 20, 30, 30));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    EXPECT_EQ(multiPolygon->GetNumCoordinates(), 10);
}

TEST_F(MultiPolygonTest, GetCoordinateN_ReturnsCorrectCoordinate) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    Coordinate coord0 = multiPolygon->GetCoordinateN(0);
    EXPECT_DOUBLE_EQ(coord0.x, 0.0);
}

TEST_F(MultiPolygonTest, GetCoordinateN_InvalidIndex_ThrowsException) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_THROW(multiPolygon->GetCoordinateN(0), GeometryException);
}

TEST_F(MultiPolygonTest, GetCoordinates_ReturnsAllCoordinates) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    CoordinateList result = multiPolygon->GetCoordinates();
    EXPECT_EQ(result.size(), 5);
}

TEST_F(MultiPolygonTest, AsText_ReturnsCorrectWKT) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    std::string wkt = multiPolygon->AsText();
    EXPECT_TRUE(wkt.find("MULTIPOLYGON") != std::string::npos);
}

TEST_F(MultiPolygonTest, AsText_EmptyMultiPolygon_ReturnsEmptyWKT) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_EQ(multiPolygon->AsText(), "MULTIPOLYGON EMPTY");
}

TEST_F(MultiPolygonTest, Clone_ReturnsIdenticalMultiPolygon) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto original = MultiPolygon::Create(std::move(polygons));
    
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedMultiPolygon = static_cast<MultiPolygon*>(clone.get());
    EXPECT_EQ(clonedMultiPolygon->GetNumPolygons(), 1);
}

TEST_F(MultiPolygonTest, CloneEmpty_ReturnsEmptyMultiPolygon) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    auto emptyClone = multiPolygon->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyMultiPolygon = static_cast<MultiPolygon*>(emptyClone.get());
    EXPECT_TRUE(emptyMultiPolygon->IsEmpty());
}

TEST_F(MultiPolygonTest, GetEnvelope_ReturnsCorrectEnvelope) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    polygons.push_back(Polygon::CreateRectangle(20, 20, 30, 30));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    const Envelope& env = multiPolygon->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 30.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 30.0);
}

TEST_F(MultiPolygonTest, GetEnvelope_EmptyMultiPolygon_ReturnsNullEnvelope) {
    auto multiPolygon = MultiPolygon::Create();
    const Envelope& env = multiPolygon->GetEnvelope();
    EXPECT_TRUE(env.IsNull());
}

TEST_F(MultiPolygonTest, GetGeometryN_ReturnsCorrectGeometry) {
    std::vector<PolygonPtr> polygons;
    polygons.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto multiPolygon = MultiPolygon::Create(std::move(polygons));
    
    const Geometry* geom0 = multiPolygon->GetGeometryN(0);
    ASSERT_NE(geom0, nullptr);
    EXPECT_EQ(geom0->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(MultiPolygonTest, GetGeometryN_InvalidIndex_ThrowsException) {
    auto multiPolygon = MultiPolygon::Create();
    EXPECT_THROW(multiPolygon->GetGeometryN(0), GeometryException);
}

}
}
