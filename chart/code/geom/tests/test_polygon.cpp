#include <gtest/gtest.h>
#include "ogc/geom/polygon.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class PolygonTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PolygonTest, Create_Default_ReturnsEmptyPolygon) {
    auto polygon = Polygon::Create();
    ASSERT_NE(polygon, nullptr);
    EXPECT_TRUE(polygon->IsEmpty());
}

TEST_F(PolygonTest, CreateWithExteriorRing_ReturnsValidPolygon) {
    auto ring = LinearRing::CreateRectangle(0, 0, 10, 10);
    auto polygon = Polygon::Create(std::move(ring));
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_FALSE(polygon->IsEmpty());
    EXPECT_EQ(polygon->GetNumInteriorRings(), 0);
}

TEST_F(PolygonTest, GetGeometryType_ReturnsPolygon) {
    auto polygon = Polygon::Create();
    EXPECT_EQ(polygon->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(PolygonTest, GetGeometryTypeString_ReturnsCorrectString) {
    auto polygon = Polygon::Create();
    EXPECT_STREQ(polygon->GetGeometryTypeString(), "POLYGON");
}

TEST_F(PolygonTest, GetDimension_ReturnsSurfaceDimension) {
    auto polygon = Polygon::Create();
    EXPECT_EQ(polygon->GetDimension(), Dimension::Surface);
}

TEST_F(PolygonTest, Is3D_ReturnsCorrectValue) {
    auto polygon2D = Polygon::CreateRectangle(0, 0, 10, 10);
    EXPECT_FALSE(polygon2D->Is3D());
    
    CoordinateList coords3D;
    coords3D.emplace_back(0, 0, 0);
    coords3D.emplace_back(10, 0, 0);
    coords3D.emplace_back(10, 10, 0);
    coords3D.emplace_back(0, 10, 0);
    coords3D.emplace_back(0, 0, 0);
    auto ring3D = LinearRing::Create(coords3D, false);
    auto polygon3D = Polygon::Create(std::move(ring3D));
    EXPECT_TRUE(polygon3D->Is3D());
}

TEST_F(PolygonTest, SetExteriorRing_UpdatesPolygon) {
    auto polygon = Polygon::Create();
    auto ring = LinearRing::CreateRectangle(0, 0, 10, 10);
    polygon->SetExteriorRing(std::move(ring));
    
    EXPECT_FALSE(polygon->IsEmpty());
    EXPECT_NE(polygon->GetExteriorRing(), nullptr);
}

TEST_F(PolygonTest, GetExteriorRing_ReturnsCorrectRing) {
    auto ring = LinearRing::CreateRectangle(0, 0, 10, 10);
    auto polygon = Polygon::Create(std::move(ring));
    
    const LinearRing* exterior = polygon->GetExteriorRing();
    ASSERT_NE(exterior, nullptr);
    EXPECT_DOUBLE_EQ(exterior->Area(), 100.0);
}

TEST_F(PolygonTest, AddInteriorRing_AddsHole) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    EXPECT_EQ(polygon->GetNumInteriorRings(), 1);
    EXPECT_EQ(polygon->GetNumRings(), 2);
}

TEST_F(PolygonTest, RemoveInteriorRing_RemovesHole) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    polygon->RemoveInteriorRing(0);
    EXPECT_EQ(polygon->GetNumInteriorRings(), 0);
}

TEST_F(PolygonTest, GetInteriorRingN_ReturnsCorrectRing) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    const LinearRing* interior = polygon->GetInteriorRingN(0);
    ASSERT_NE(interior, nullptr);
    EXPECT_DOUBLE_EQ(interior->Area(), 100.0);
}

TEST_F(PolygonTest, ClearRings_RemovesAllRings) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    polygon->ClearRings();
    EXPECT_TRUE(polygon->IsEmpty());
    EXPECT_EQ(polygon->GetNumRings(), 0);
}

TEST_F(PolygonTest, Area_CalculatesCorrectly) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    EXPECT_DOUBLE_EQ(polygon->Area(), 100.0);
}

TEST_F(PolygonTest, Area_WithHole_CalculatesCorrectly) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    double expectedArea = 400.0 - 100.0;
    EXPECT_DOUBLE_EQ(polygon->Area(), expectedArea);
}

TEST_F(PolygonTest, GetPerimeter_CalculatesCorrectly) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 20);
    EXPECT_DOUBLE_EQ(polygon->GetPerimeter(), 60.0);
}

TEST_F(PolygonTest, GetExteriorArea_ReturnsExteriorRingArea) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    EXPECT_DOUBLE_EQ(polygon->GetExteriorArea(), 400.0);
}

TEST_F(PolygonTest, GetInteriorArea_ReturnsHolesArea) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    EXPECT_DOUBLE_EQ(polygon->GetInteriorArea(), 100.0);
}

TEST_F(PolygonTest, GetSolidity_CalculatesCorrectly) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    double solidity = polygon->GetSolidity();
    EXPECT_NEAR(solidity, 300.0 / 400.0, DEFAULT_TOLERANCE);
}

TEST_F(PolygonTest, GetCompactness_CalculatesCorrectly) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    double compactness = polygon->GetCompactness();
    
    double expectedArea = 100.0;
    double expectedPerimeter = 40.0;
    double expectedCompactness = 4.0 * PI * expectedArea / (expectedPerimeter * expectedPerimeter);
    
    EXPECT_NEAR(compactness, expectedCompactness, DEFAULT_TOLERANCE);
}

TEST_F(PolygonTest, RemoveHoles_ReturnsPolygonWithoutHoles) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    auto noHoles = polygon->RemoveHoles();
    EXPECT_EQ(noHoles->GetNumInteriorRings(), 0);
    EXPECT_DOUBLE_EQ(noHoles->Area(), 400.0);
}

TEST_F(PolygonTest, CreateRectangle_ReturnsValidPolygon) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 20);
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_DOUBLE_EQ(polygon->Area(), 200.0);
    EXPECT_DOUBLE_EQ(polygon->GetPerimeter(), 60.0);
}

TEST_F(PolygonTest, CreateCircle_ReturnsValidPolygon) {
    auto polygon = Polygon::CreateCircle(0, 0, 10, 32);
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_NEAR(polygon->Area(), PI * 100.0, 5.0);
}

TEST_F(PolygonTest, CreateTriangle_ReturnsValidPolygon) {
    Coordinate p1(0, 0);
    Coordinate p2(10, 0);
    Coordinate p3(5, 10);
    
    auto polygon = Polygon::CreateTriangle(p1, p2, p3);
    
    ASSERT_NE(polygon, nullptr);
    EXPECT_DOUBLE_EQ(polygon->Area(), 50.0);
}

TEST_F(PolygonTest, GetNumCoordinates_ReturnsCorrectValue) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    EXPECT_EQ(polygon->GetNumCoordinates(), 5);
    
    auto hole = LinearRing::CreateRectangle(2, 2, 8, 8);
    polygon->AddInteriorRing(std::move(hole));
    EXPECT_EQ(polygon->GetNumCoordinates(), 10);
}

TEST_F(PolygonTest, GetCoordinates_ReturnsAllCoordinates) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    CoordinateList coords = polygon->GetCoordinates();
    
    EXPECT_EQ(coords.size(), 5);
}

TEST_F(PolygonTest, AsText_ReturnsCorrectWKT) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    std::string wkt = polygon->AsText();
    
    EXPECT_TRUE(wkt.find("POLYGON") != std::string::npos);
    EXPECT_TRUE(wkt.find("0") != std::string::npos);
    EXPECT_TRUE(wkt.find("10") != std::string::npos);
}

TEST_F(PolygonTest, AsText_WithHole_ReturnsCorrectWKT) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    polygon->AddInteriorRing(std::move(hole));
    
    std::string wkt = polygon->AsText();
    EXPECT_TRUE(wkt.find("POLYGON") != std::string::npos);
    EXPECT_TRUE(wkt.find(",") != std::string::npos);
}

TEST_F(PolygonTest, AsText_EmptyPolygon_ReturnsEmptyWKT) {
    auto polygon = Polygon::Create();
    EXPECT_EQ(polygon->AsText(), "POLYGON EMPTY");
}

TEST_F(PolygonTest, Clone_ReturnsIdenticalPolygon) {
    auto original = Polygon::CreateRectangle(0, 0, 10, 10);
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedPolygon = static_cast<Polygon*>(clone.get());
    EXPECT_DOUBLE_EQ(clonedPolygon->Area(), 100.0);
}

TEST_F(PolygonTest, Clone_WithHoles_ReturnsIdenticalPolygon) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto original = Polygon::Create(std::move(exterior));
    
    auto hole = LinearRing::CreateRectangle(5, 5, 15, 15);
    original->AddInteriorRing(std::move(hole));
    
    auto clone = original->Clone();
    auto clonedPolygon = static_cast<Polygon*>(clone.get());
    
    EXPECT_EQ(clonedPolygon->GetNumInteriorRings(), 1);
    EXPECT_DOUBLE_EQ(clonedPolygon->Area(), 300.0);
}

TEST_F(PolygonTest, CloneEmpty_ReturnsEmptyPolygon) {
    auto polygon = Polygon::CreateRectangle(0, 0, 10, 10);
    auto emptyClone = polygon->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyPolygon = static_cast<Polygon*>(emptyClone.get());
    EXPECT_TRUE(emptyPolygon->IsEmpty());
}

TEST_F(PolygonTest, GetEnvelope_ReturnsCorrectEnvelope) {
    auto polygon = Polygon::CreateRectangle(5, 10, 15, 30);
    const Envelope& env = polygon->GetEnvelope();
    
    EXPECT_DOUBLE_EQ(env.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 30.0);
}

TEST_F(PolygonTest, ContainsRing_ReturnsCorrectValue) {
    auto exterior = LinearRing::CreateRectangle(0, 0, 20, 20);
    auto polygon = Polygon::Create(std::move(exterior));
    
    auto insideRing = LinearRing::CreateRectangle(5, 5, 15, 15);
    EXPECT_TRUE(polygon->ContainsRing(insideRing.get()));
    
    auto outsideRing = LinearRing::CreateRectangle(25, 25, 35, 35);
    EXPECT_FALSE(polygon->ContainsRing(outsideRing.get()));
}

}
}
