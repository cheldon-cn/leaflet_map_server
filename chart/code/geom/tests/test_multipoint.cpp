#include <gtest/gtest.h>
#include "ogc/geom/multipoint.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class MultiPointTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MultiPointTest, Create_Default_ReturnsEmptyMultiPoint) {
    auto multiPoint = MultiPoint::Create();
    ASSERT_NE(multiPoint, nullptr);
    EXPECT_TRUE(multiPoint->IsEmpty());
    EXPECT_EQ(multiPoint->GetNumPoints(), 0);
}

TEST_F(MultiPointTest, CreateFromCoordinates_ReturnsValidMultiPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 20);
    
    auto multiPoint = MultiPoint::Create(coords);
    ASSERT_NE(multiPoint, nullptr);
    EXPECT_EQ(multiPoint->GetNumPoints(), 3);
    EXPECT_FALSE(multiPoint->IsEmpty());
}

TEST_F(MultiPointTest, GetGeometryType_ReturnsMultiPoint) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_EQ(multiPoint->GetGeometryType(), GeomType::kMultiPoint);
}

TEST_F(MultiPointTest, GetGeometryTypeString_ReturnsCorrectString) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_STREQ(multiPoint->GetGeometryTypeString(), "MULTIPOINT");
}

TEST_F(MultiPointTest, GetDimension_ReturnsPointDimension) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_EQ(multiPoint->GetDimension(), Dimension::Point);
}

TEST_F(MultiPointTest, GetCoordinateDimension_ReturnsCorrectValue) {
    CoordinateList coords2D;
    coords2D.emplace_back(0, 0);
    coords2D.emplace_back(10, 10);
    auto multiPoint2D = MultiPoint::Create(coords2D);
    EXPECT_EQ(multiPoint2D->GetCoordinateDimension(), 2);
    
    CoordinateList coords3D;
    coords3D.emplace_back(0, 0, 0);
    coords3D.emplace_back(10, 10, 10);
    auto multiPoint3D = MultiPoint::Create(coords3D);
    EXPECT_EQ(multiPoint3D->GetCoordinateDimension(), 3);
}

TEST_F(MultiPointTest, Is3D_ReturnsCorrectValue) {
    CoordinateList coords2D;
    coords2D.emplace_back(0, 0);
    coords2D.emplace_back(10, 10);
    auto multiPoint2D = MultiPoint::Create(coords2D);
    EXPECT_FALSE(multiPoint2D->Is3D());
    
    CoordinateList coords3D;
    coords3D.emplace_back(0, 0, 0);
    coords3D.emplace_back(10, 10, 10);
    auto multiPoint3D = MultiPoint::Create(coords3D);
    EXPECT_TRUE(multiPoint3D->Is3D());
}

TEST_F(MultiPointTest, IsMeasured_ReturnsCorrectValue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    EXPECT_FALSE(multiPoint->IsMeasured());
    
    CoordinateList measuredCoords;
    measuredCoords.emplace_back(0, 0, 0, 0);
    measuredCoords.emplace_back(10, 10, 10, 10);
    auto measuredMultiPoint = MultiPoint::Create(measuredCoords);
    EXPECT_TRUE(measuredMultiPoint->IsMeasured());
}

TEST_F(MultiPointTest, AddPoint_PointPtr_AddsPoint) {
    auto multiPoint = MultiPoint::Create();
    multiPoint->AddPoint(Point::Create(0, 0));
    multiPoint->AddPoint(Point::Create(10, 10));
    
    EXPECT_EQ(multiPoint->GetNumPoints(), 2);
}

TEST_F(MultiPointTest, AddPoint_Coordinate_AddsPoint) {
    auto multiPoint = MultiPoint::Create();
    multiPoint->AddPoint(Coordinate(0, 0));
    multiPoint->AddPoint(Coordinate(10, 10));
    
    EXPECT_EQ(multiPoint->GetNumPoints(), 2);
}

TEST_F(MultiPointTest, RemovePoint_RemovesCorrectPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 20);
    auto multiPoint = MultiPoint::Create(coords);
    
    multiPoint->RemovePoint(1);
    
    EXPECT_EQ(multiPoint->GetNumPoints(), 2);
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(1)->GetX(), 20.0);
}

TEST_F(MultiPointTest, RemovePoint_InvalidIndex_ThrowsException) {
    auto multiPoint = MultiPoint::Create();
    multiPoint->AddPoint(Coordinate(0, 0));
    
    EXPECT_THROW(multiPoint->RemovePoint(5), GeometryException);
}

TEST_F(MultiPointTest, Clear_RemovesAllPoints) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    multiPoint->Clear();
    
    EXPECT_TRUE(multiPoint->IsEmpty());
    EXPECT_EQ(multiPoint->GetNumPoints(), 0);
}

TEST_F(MultiPointTest, GetPointN_ReturnsCorrectPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    const Point* p0 = multiPoint->GetPointN(0);
    ASSERT_NE(p0, nullptr);
    EXPECT_DOUBLE_EQ(p0->GetX(), 0.0);
    
    const Point* p1 = multiPoint->GetPointN(1);
    ASSERT_NE(p1, nullptr);
    EXPECT_DOUBLE_EQ(p1->GetX(), 10.0);
}

TEST_F(MultiPointTest, GetPointN_InvalidIndex_ThrowsException) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_THROW(multiPoint->GetPointN(0), GeometryException);
}

TEST_F(MultiPointTest, GetNumCoordinates_ReturnsCorrectValue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 20);
    auto multiPoint = MultiPoint::Create(coords);
    
    EXPECT_EQ(multiPoint->GetNumCoordinates(), 3);
}

TEST_F(MultiPointTest, GetCoordinateN_ReturnsCorrectCoordinate) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    Coordinate coord0 = multiPoint->GetCoordinateN(0);
    EXPECT_DOUBLE_EQ(coord0.x, 0.0);
    
    Coordinate coord1 = multiPoint->GetCoordinateN(1);
    EXPECT_DOUBLE_EQ(coord1.x, 10.0);
}

TEST_F(MultiPointTest, GetCoordinateN_InvalidIndex_ThrowsException) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_THROW(multiPoint->GetCoordinateN(0), GeometryException);
}

TEST_F(MultiPointTest, GetCoordinates_ReturnsAllCoordinates) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    CoordinateList result = multiPoint->GetCoordinates();
    EXPECT_EQ(result.size(), 2);
}

TEST_F(MultiPointTest, Merge_CombinesMultiPoints) {
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    auto multiPoint1 = MultiPoint::Create(coords1);
    
    CoordinateList coords2;
    coords2.emplace_back(20, 20);
    coords2.emplace_back(30, 30);
    auto multiPoint2 = MultiPoint::Create(coords2);
    
    multiPoint1->Merge(multiPoint2.get());
    
    EXPECT_EQ(multiPoint1->GetNumPoints(), 4);
}

TEST_F(MultiPointTest, UniquePoints_RemovesDuplicates) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    multiPoint->UniquePoints();
    
    EXPECT_EQ(multiPoint->GetNumPoints(), 2);
}

TEST_F(MultiPointTest, ContainsPoint_ReturnsCorrectValue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 20);
    auto multiPoint = MultiPoint::Create(coords);
    
    EXPECT_TRUE(multiPoint->ContainsPoint(Coordinate(0, 0)));
    EXPECT_TRUE(multiPoint->ContainsPoint(Coordinate(10, 10)));
    EXPECT_FALSE(multiPoint->ContainsPoint(Coordinate(5, 5)));
}

TEST_F(MultiPointTest, ContainsMultiPoint_ReturnsCorrectValue) {
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    coords1.emplace_back(20, 20);
    auto multiPoint1 = MultiPoint::Create(coords1);
    
    CoordinateList coords2;
    coords2.emplace_back(0, 0);
    coords2.emplace_back(10, 10);
    auto multiPoint2 = MultiPoint::Create(coords2);
    
    EXPECT_TRUE(multiPoint1->ContainsMultiPoint(multiPoint2.get()));
    
    CoordinateList coords3;
    coords3.emplace_back(5, 5);
    auto multiPoint3 = MultiPoint::Create(coords3);
    
    EXPECT_FALSE(multiPoint1->ContainsMultiPoint(multiPoint3.get()));
}

TEST_F(MultiPointTest, SortByX_SortsCorrectly) {
    CoordinateList coords;
    coords.emplace_back(20, 0);
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    auto multiPoint = MultiPoint::Create(coords);
    
    multiPoint->SortByX();
    
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(0)->GetX(), 0.0);
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(1)->GetX(), 10.0);
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(2)->GetX(), 20.0);
}

TEST_F(MultiPointTest, SortByY_SortsCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 20);
    coords.emplace_back(0, 0);
    coords.emplace_back(0, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    multiPoint->SortByY();
    
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(0)->GetY(), 0.0);
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(1)->GetY(), 10.0);
    EXPECT_DOUBLE_EQ(multiPoint->GetPointN(2)->GetY(), 20.0);
}

TEST_F(MultiPointTest, AsText_ReturnsCorrectWKT) {
    CoordinateList coords;
    coords.emplace_back(1.5, 2.5);
    coords.emplace_back(3.5, 4.5);
    auto multiPoint = MultiPoint::Create(coords);
    
    std::string wkt = multiPoint->AsText();
    EXPECT_TRUE(wkt.find("MULTIPOINT") != std::string::npos);
    EXPECT_TRUE(wkt.find("1.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("2.5") != std::string::npos);
}

TEST_F(MultiPointTest, AsText_EmptyMultiPoint_ReturnsEmptyWKT) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_EQ(multiPoint->AsText(), "MULTIPOINT EMPTY");
}

TEST_F(MultiPointTest, Clone_ReturnsIdenticalMultiPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto original = MultiPoint::Create(coords);
    
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedMultiPoint = static_cast<MultiPoint*>(clone.get());
    EXPECT_EQ(clonedMultiPoint->GetNumPoints(), 2);
}

TEST_F(MultiPointTest, CloneEmpty_ReturnsEmptyMultiPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    auto multiPoint = MultiPoint::Create(coords);
    
    auto emptyClone = multiPoint->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyMultiPoint = static_cast<MultiPoint*>(emptyClone.get());
    EXPECT_TRUE(emptyMultiPoint->IsEmpty());
}

TEST_F(MultiPointTest, GetEnvelope_ReturnsCorrectEnvelope) {
    CoordinateList coords;
    coords.emplace_back(5, 10);
    coords.emplace_back(15, 5);
    coords.emplace_back(10, 20);
    auto multiPoint = MultiPoint::Create(coords);
    
    const Envelope& env = multiPoint->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 20.0);
}

TEST_F(MultiPointTest, GetEnvelope_EmptyMultiPoint_ReturnsNullEnvelope) {
    auto multiPoint = MultiPoint::Create();
    const Envelope& env = multiPoint->GetEnvelope();
    EXPECT_TRUE(env.IsNull());
}

TEST_F(MultiPointTest, GetGeometryN_ReturnsCorrectGeometry) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto multiPoint = MultiPoint::Create(coords);
    
    const Geometry* geom0 = multiPoint->GetGeometryN(0);
    ASSERT_NE(geom0, nullptr);
    EXPECT_EQ(geom0->GetGeometryType(), GeomType::kPoint);
}

TEST_F(MultiPointTest, GetGeometryN_InvalidIndex_ThrowsException) {
    auto multiPoint = MultiPoint::Create();
    EXPECT_THROW(multiPoint->GetGeometryN(0), GeometryException);
}

}
}
