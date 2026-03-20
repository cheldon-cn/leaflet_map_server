#include <gtest/gtest.h>
#include "ogc/point.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class PointTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PointTest, Create2D_ReturnsValidPoint) {
    auto point = Point::Create(1.0, 2.0);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_FALSE(point->Is3D());
    EXPECT_FALSE(point->IsMeasured());
}

TEST_F(PointTest, Create3D_ReturnsValidPoint) {
    auto point = Point::Create(1.0, 2.0, 3.0);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(point->GetZ(), 3.0);
    EXPECT_TRUE(point->Is3D());
    EXPECT_FALSE(point->IsMeasured());
}

TEST_F(PointTest, Create4D_ReturnsValidPoint) {
    auto point = Point::Create(1.0, 2.0, 3.0, 4.0);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(point->GetZ(), 3.0);
    EXPECT_DOUBLE_EQ(point->GetM(), 4.0);
    EXPECT_TRUE(point->Is3D());
    EXPECT_TRUE(point->IsMeasured());
}

TEST_F(PointTest, CreateFromCoordinate_ReturnsValidPoint) {
    Coordinate coord(1.0, 2.0, 3.0);
    auto point = Point::Create(coord);
    ASSERT_NE(point, nullptr);
    EXPECT_DOUBLE_EQ(point->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(point->GetZ(), 3.0);
}

TEST_F(PointTest, GetGeometryType_ReturnsPoint) {
    auto point = Point::Create(1, 2);
    EXPECT_EQ(point->GetGeometryType(), GeomType::kPoint);
}

TEST_F(PointTest, GetGeometryTypeString_ReturnsPointString) {
    auto point = Point::Create(1, 2);
    EXPECT_STREQ(point->GetGeometryTypeString(), "POINT");
}

TEST_F(PointTest, GetDimension_ReturnsPointDimension) {
    auto point = Point::Create(1, 2);
    EXPECT_EQ(point->GetDimension(), Dimension::Point);
}

TEST_F(PointTest, GetCoordinateDimension_ReturnsCorrectValue) {
    auto point2D = Point::Create(1, 2);
    EXPECT_EQ(point2D->GetCoordinateDimension(), 2);
    
    auto point3D = Point::Create(1, 2, 3);
    EXPECT_EQ(point3D->GetCoordinateDimension(), 3);
    
    auto point4D = Point::Create(1, 2, 3, 4);
    EXPECT_EQ(point4D->GetCoordinateDimension(), 4);
}

TEST_F(PointTest, IsEmpty_ReturnsCorrectValue) {
    auto emptyPoint = Point::Create(Coordinate::Empty());
    EXPECT_TRUE(emptyPoint->IsEmpty());
    
    auto point = Point::Create(1, 2);
    EXPECT_FALSE(point->IsEmpty());
}

TEST_F(PointTest, SetCoordinate_UpdatesPoint) {
    auto point = Point::Create(1, 2);
    point->SetCoordinate(Coordinate(3, 4));
    
    EXPECT_DOUBLE_EQ(point->GetX(), 3.0);
    EXPECT_DOUBLE_EQ(point->GetY(), 4.0);
}

TEST_F(PointTest, SetX_SetY_SetZ_SetM_UpdateCoordinates) {
    auto point = Point::Create(1, 2);
    
    point->SetX(10);
    EXPECT_DOUBLE_EQ(point->GetX(), 10.0);
    
    point->SetY(20);
    EXPECT_DOUBLE_EQ(point->GetY(), 20.0);
    
    point->SetZ(30);
    EXPECT_TRUE(point->Is3D());
    EXPECT_DOUBLE_EQ(point->GetZ(), 30.0);
    
    point->SetM(40);
    EXPECT_TRUE(point->IsMeasured());
    EXPECT_DOUBLE_EQ(point->GetM(), 40.0);
}

TEST_F(PointTest, OperatorPlus_AddsPoints) {
    auto p1 = Point::Create(1, 2);
    auto p2 = Point::Create(3, 4);
    auto result = *p1 + *p2;
    
    ASSERT_NE(result, nullptr);
    EXPECT_DOUBLE_EQ(result->GetX(), 4.0);
    EXPECT_DOUBLE_EQ(result->GetY(), 6.0);
}

TEST_F(PointTest, OperatorMinus_SubtractsPoints) {
    auto p1 = Point::Create(5, 7);
    auto p2 = Point::Create(2, 3);
    auto result = *p1 - *p2;
    
    ASSERT_NE(result, nullptr);
    EXPECT_DOUBLE_EQ(result->GetX(), 3.0);
    EXPECT_DOUBLE_EQ(result->GetY(), 4.0);
}

TEST_F(PointTest, OperatorMultiply_ScalesPoint) {
    auto p = Point::Create(2, 3);
    auto result = *p * 2.0;
    
    ASSERT_NE(result, nullptr);
    EXPECT_DOUBLE_EQ(result->GetX(), 4.0);
    EXPECT_DOUBLE_EQ(result->GetY(), 6.0);
}

TEST_F(PointTest, Dot_CalculatesCorrectly) {
    auto p1 = Point::Create(1, 2);
    auto p2 = Point::Create(3, 4);
    
    EXPECT_DOUBLE_EQ(p1->Dot(*p2), 11.0);
}

TEST_F(PointTest, Cross_CalculatesCorrectly) {
    auto p1 = Point::Create(3, 0);
    auto p2 = Point::Create(0, 4);
    
    EXPECT_DOUBLE_EQ(p1->Cross(*p2), 12.0);
}

TEST_F(PointTest, IsCollinear_ReturnsCorrectValue) {
    auto p1 = Point::Create(0, 0);
    auto p2 = Point::Create(1, 1);
    auto p3 = Point::Create(2, 2);
    auto p4 = Point::Create(2, 3);
    
    EXPECT_TRUE(p2->IsCollinear(*p1, *p3));
    EXPECT_FALSE(p2->IsCollinear(*p1, *p4));
}

TEST_F(PointTest, IsBetween_ReturnsCorrectValue) {
    auto start = Point::Create(0, 0);
    auto middle = Point::Create(5, 5);
    auto end = Point::Create(10, 10);
    auto outside = Point::Create(15, 15);
    
    EXPECT_TRUE(middle->IsBetween(*start, *end));
    EXPECT_FALSE(outside->IsBetween(*start, *end));
}

TEST_F(PointTest, Normalize_ReturnsUnitPoint) {
    auto p = Point::Create(3, 4);
    auto normalized = p->Normalize();
    
    ASSERT_NE(normalized, nullptr);
    EXPECT_DOUBLE_EQ(normalized->GetX(), 0.6);
    EXPECT_DOUBLE_EQ(normalized->GetY(), 0.8);
}

TEST_F(PointTest, FromPolar_CreatesCorrectPoint) {
    auto p = Point::FromPolar(1.0, 0.0);
    EXPECT_NEAR(p->GetX(), 1.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(p->GetY(), 0.0, DEFAULT_TOLERANCE);
    
    auto p2 = Point::FromPolar(1.0, PI / 2);
    EXPECT_NEAR(p2->GetX(), 0.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(p2->GetY(), 1.0, DEFAULT_TOLERANCE);
}

TEST_F(PointTest, Centroid_CalculatesCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    
    auto centroid = Point::Centroid(coords);
    EXPECT_DOUBLE_EQ(centroid->GetX(), 5.0);
    EXPECT_DOUBLE_EQ(centroid->GetY(), 5.0);
}

TEST_F(PointTest, Centroid_EmptyList_ReturnsEmptyPoint) {
    CoordinateList empty;
    auto centroid = Point::Centroid(empty);
    EXPECT_TRUE(centroid->IsEmpty());
}

TEST_F(PointTest, GetNumCoordinates_ReturnsCorrectValue) {
    auto point = Point::Create(1, 2);
    EXPECT_EQ(point->GetNumCoordinates(), 1);
    
    auto emptyPoint = Point::Create(Coordinate::Empty());
    EXPECT_EQ(emptyPoint->GetNumCoordinates(), 0);
}

TEST_F(PointTest, GetCoordinateN_ReturnsCorrectCoordinate) {
    auto point = Point::Create(1, 2, 3);
    Coordinate coord = point->GetCoordinateN(0);
    
    EXPECT_DOUBLE_EQ(coord.x, 1.0);
    EXPECT_DOUBLE_EQ(coord.y, 2.0);
    EXPECT_DOUBLE_EQ(coord.z, 3.0);
}

TEST_F(PointTest, GetCoordinateN_InvalidIndex_ThrowsException) {
    auto point = Point::Create(1, 2);
    EXPECT_THROW(point->GetCoordinateN(1), GeometryException);
}

TEST_F(PointTest, GetCoordinates_ReturnsCoordinateList) {
    auto point = Point::Create(1, 2);
    CoordinateList coords = point->GetCoordinates();
    
    ASSERT_EQ(coords.size(), 1);
    EXPECT_DOUBLE_EQ(coords[0].x, 1.0);
    EXPECT_DOUBLE_EQ(coords[0].y, 2.0);
}

TEST_F(PointTest, AsText_ReturnsCorrectWKT) {
    auto point = Point::Create(1.5, 2.5);
    std::string wkt = point->AsText();
    EXPECT_TRUE(wkt.find("POINT") != std::string::npos);
    EXPECT_TRUE(wkt.find("1.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("2.5") != std::string::npos);
    
    auto point3D = Point::Create(1.5, 2.5, 3.5);
    wkt = point3D->AsText();
    EXPECT_TRUE(wkt.find("1.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("2.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("3.5") != std::string::npos);
}

TEST_F(PointTest, AsText_EmptyPoint_ReturnsEmptyWKT) {
    auto point = Point::Create(Coordinate::Empty());
    EXPECT_EQ(point->AsText(), "POINT EMPTY");
}

TEST_F(PointTest, Clone_ReturnsIdenticalPoint) {
    auto original = Point::Create(1, 2, 3);
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->GetGeometryType(), GeomType::kPoint);
    
    auto clonedPoint = static_cast<Point*>(clone.get());
    EXPECT_DOUBLE_EQ(clonedPoint->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(clonedPoint->GetY(), 2.0);
    EXPECT_DOUBLE_EQ(clonedPoint->GetZ(), 3.0);
}

TEST_F(PointTest, CloneEmpty_ReturnsEmptyPoint) {
    auto point = Point::Create(1, 2);
    auto emptyClone = point->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyPoint = static_cast<Point*>(emptyClone.get());
    EXPECT_TRUE(emptyPoint->IsEmpty());
}

TEST_F(PointTest, GetEnvelope_ReturnsCorrectEnvelope) {
    auto point = Point::Create(5, 10);
    const Envelope& env = point->GetEnvelope();
    
    EXPECT_DOUBLE_EQ(env.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 10.0);
}

TEST_F(PointTest, GetEnvelope_EmptyPoint_ReturnsNullEnvelope) {
    auto point = Point::Create(Coordinate::Empty());
    const Envelope& env = point->GetEnvelope();
    EXPECT_TRUE(env.IsNull());
}

}
}
