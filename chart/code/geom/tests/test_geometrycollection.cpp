#include <gtest/gtest.h>
#include "ogc/geom/geometrycollection.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class GeometryCollectionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(GeometryCollectionTest, Create_Default_ReturnsEmptyCollection) {
    auto collection = GeometryCollection::Create();
    ASSERT_NE(collection, nullptr);
    EXPECT_TRUE(collection->IsEmpty());
    EXPECT_EQ(collection->GetNumGeometries(), 0);
}

TEST_F(GeometryCollectionTest, CreateFromGeometries_ReturnsValidCollection) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    ASSERT_NE(collection, nullptr);
    EXPECT_EQ(collection->GetNumGeometries(), 2);
    EXPECT_FALSE(collection->IsEmpty());
}

TEST_F(GeometryCollectionTest, GetGeometryType_ReturnsGeometryCollection) {
    auto collection = GeometryCollection::Create();
    EXPECT_EQ(collection->GetGeometryType(), GeomType::kGeometryCollection);
}

TEST_F(GeometryCollectionTest, GetGeometryTypeString_ReturnsCorrectString) {
    auto collection = GeometryCollection::Create();
    EXPECT_STREQ(collection->GetGeometryTypeString(), "GEOMETRYCOLLECTION");
}

TEST_F(GeometryCollectionTest, GetDimension_ReturnsUnknownDimension) {
    auto collection = GeometryCollection::Create();
    EXPECT_EQ(collection->GetDimension(), Dimension::Unknown);
}

TEST_F(GeometryCollectionTest, GetDimension_WithPoints_ReturnsPointDimension) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    EXPECT_EQ(collection->GetDimension(), Dimension::Point);
}

TEST_F(GeometryCollectionTest, GetDimension_WithPolygons_ReturnsSurfaceDimension) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    EXPECT_EQ(collection->GetDimension(), Dimension::Surface);
}

TEST_F(GeometryCollectionTest, GetCoordinateDimension_ReturnsCorrectValue) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    EXPECT_EQ(collection->GetCoordinateDimension(), 2);
}

TEST_F(GeometryCollectionTest, Is3D_ReturnsCorrectValue) {
    std::vector<GeometryPtr> geometries2D;
    geometries2D.push_back(Point::Create(0, 0));
    auto collection2D = GeometryCollection::Create(std::move(geometries2D));
    EXPECT_FALSE(collection2D->Is3D());
    
    std::vector<GeometryPtr> geometries3D;
    geometries3D.push_back(Point::Create(0, 0, 0));
    auto collection3D = GeometryCollection::Create(std::move(geometries3D));
    EXPECT_TRUE(collection3D->Is3D());
}

TEST_F(GeometryCollectionTest, IsMeasured_ReturnsCorrectValue) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    EXPECT_FALSE(collection->IsMeasured());
}

TEST_F(GeometryCollectionTest, AddGeometry_AddsGeometry) {
    auto collection = GeometryCollection::Create();
    collection->AddGeometry(Point::Create(0, 0));
    collection->AddGeometry(Point::Create(10, 10));
    
    EXPECT_EQ(collection->GetNumGeometries(), 2);
}

TEST_F(GeometryCollectionTest, AddGeometry_MixedTypes_AddsCorrectly) {
    auto collection = GeometryCollection::Create();
    collection->AddGeometry(Point::Create(0, 0));
    collection->AddGeometry(LineString::Create());
    collection->AddGeometry(Polygon::CreateRectangle(0, 0, 10, 10));
    
    EXPECT_EQ(collection->GetNumGeometries(), 3);
}

TEST_F(GeometryCollectionTest, RemoveGeometry_RemovesCorrectGeometry) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    geometries.push_back(Point::Create(20, 20));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    collection->RemoveGeometry(1);
    
    EXPECT_EQ(collection->GetNumGeometries(), 2);
}

TEST_F(GeometryCollectionTest, RemoveGeometry_InvalidIndex_ThrowsException) {
    auto collection = GeometryCollection::Create();
    collection->AddGeometry(Point::Create(0, 0));
    
    EXPECT_THROW(collection->RemoveGeometry(5), GeometryException);
}

TEST_F(GeometryCollectionTest, Clear_RemovesAllGeometries) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    collection->Clear();
    
    EXPECT_TRUE(collection->IsEmpty());
    EXPECT_EQ(collection->GetNumGeometries(), 0);
}

TEST_F(GeometryCollectionTest, GetGeometryN_ReturnsCorrectGeometry) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    const Geometry* geom0 = collection->GetGeometryN(0);
    ASSERT_NE(geom0, nullptr);
    EXPECT_EQ(geom0->GetGeometryType(), GeomType::kPoint);
    
    const Geometry* geom1 = collection->GetGeometryN(1);
    ASSERT_NE(geom1, nullptr);
    EXPECT_EQ(geom1->GetGeometryType(), GeomType::kPoint);
}

TEST_F(GeometryCollectionTest, GetGeometryN_InvalidIndex_ThrowsException) {
    auto collection = GeometryCollection::Create();
    EXPECT_THROW(collection->GetGeometryN(0), GeometryException);
}

TEST_F(GeometryCollectionTest, GetGeometryNPtr_ReturnsCorrectGeometryPtr) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    GeometryPtr geomPtr = collection->GetGeometryNPtr(0);
    ASSERT_NE(geomPtr, nullptr);
    EXPECT_EQ(geomPtr->GetGeometryType(), GeomType::kPoint);
}

TEST_F(GeometryCollectionTest, Area_ReturnsCorrectValue) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    geometries.push_back(Polygon::CreateRectangle(0, 0, 20, 20));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    EXPECT_DOUBLE_EQ(collection->Area(), 100.0 + 400.0);
}

TEST_F(GeometryCollectionTest, Area_WithNonAreaGeometries_ReturnsSum) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Polygon::CreateRectangle(0, 0, 10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    EXPECT_DOUBLE_EQ(collection->Area(), 100.0);
}

TEST_F(GeometryCollectionTest, Length_ReturnsCorrectValue) {
    std::vector<GeometryPtr> geometries;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(3, 4);
    geometries.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(0, 0);
    coords2.emplace_back(6, 8);
    geometries.push_back(LineString::Create(coords2));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    EXPECT_DOUBLE_EQ(collection->Length(), 5.0 + 10.0);
}

TEST_F(GeometryCollectionTest, Flatten_ReturnsSingleLevelCollection) {
    auto outer = GeometryCollection::Create();
    outer->AddGeometry(Point::Create(0, 0));
    
    auto inner = GeometryCollection::Create();
    inner->AddGeometry(Point::Create(10, 10));
    outer->AddGeometry(std::move(inner));
    
    auto flattened = outer->Flatten();
    
    ASSERT_NE(flattened, nullptr);
    EXPECT_EQ(flattened->GetNumGeometries(), 2);
}

TEST_F(GeometryCollectionTest, GetNumCoordinates_ReturnsCorrectValue) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    EXPECT_EQ(collection->GetNumCoordinates(), 2);
}

TEST_F(GeometryCollectionTest, GetCoordinateN_ReturnsCorrectCoordinate) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    Coordinate coord0 = collection->GetCoordinateN(0);
    EXPECT_DOUBLE_EQ(coord0.x, 0.0);
    
    Coordinate coord1 = collection->GetCoordinateN(1);
    EXPECT_DOUBLE_EQ(coord1.x, 10.0);
}

TEST_F(GeometryCollectionTest, GetCoordinateN_InvalidIndex_ThrowsException) {
    auto collection = GeometryCollection::Create();
    EXPECT_THROW(collection->GetCoordinateN(0), GeometryException);
}

TEST_F(GeometryCollectionTest, GetCoordinates_ReturnsAllCoordinates) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    CoordinateList result = collection->GetCoordinates();
    EXPECT_EQ(result.size(), 2);
}

TEST_F(GeometryCollectionTest, AsText_ReturnsCorrectWKT) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    std::string wkt = collection->AsText();
    EXPECT_TRUE(wkt.find("GEOMETRYCOLLECTION") != std::string::npos);
    EXPECT_TRUE(wkt.find("POINT") != std::string::npos);
}

TEST_F(GeometryCollectionTest, AsText_EmptyCollection_ReturnsEmptyWKT) {
    auto collection = GeometryCollection::Create();
    EXPECT_EQ(collection->AsText(), "GEOMETRYCOLLECTION EMPTY");
}

TEST_F(GeometryCollectionTest, Clone_ReturnsIdenticalCollection) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    geometries.push_back(Point::Create(10, 10));
    
    auto original = GeometryCollection::Create(std::move(geometries));
    
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedCollection = static_cast<GeometryCollection*>(clone.get());
    EXPECT_EQ(clonedCollection->GetNumGeometries(), 2);
}

TEST_F(GeometryCollectionTest, CloneEmpty_ReturnsEmptyCollection) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(0, 0));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    auto emptyClone = collection->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyCollection = static_cast<GeometryCollection*>(emptyClone.get());
    EXPECT_TRUE(emptyCollection->IsEmpty());
}

TEST_F(GeometryCollectionTest, GetEnvelope_ReturnsCorrectEnvelope) {
    std::vector<GeometryPtr> geometries;
    geometries.push_back(Point::Create(5, 10));
    geometries.push_back(Point::Create(15, 5));
    geometries.push_back(Point::Create(10, 20));
    
    auto collection = GeometryCollection::Create(std::move(geometries));
    
    const Envelope& env = collection->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 20.0);
}

TEST_F(GeometryCollectionTest, GetEnvelope_EmptyCollection_ReturnsNullEnvelope) {
    auto collection = GeometryCollection::Create();
    const Envelope& env = collection->GetEnvelope();
    EXPECT_TRUE(env.IsNull());
}

}
}
