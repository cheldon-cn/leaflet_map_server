#include <gtest/gtest.h>
#include "ogc/geom/coordinate.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class CoordinateTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CoordinateTest, DefaultConstructor_CreatesEmptyCoordinate) {
    Coordinate coord;
    EXPECT_TRUE(std::isnan(coord.x));
    EXPECT_TRUE(std::isnan(coord.y));
    EXPECT_TRUE(std::isnan(coord.z));
    EXPECT_TRUE(std::isnan(coord.m));
}

TEST_F(CoordinateTest, Constructor2D_Creates2DCoordinate) {
    Coordinate coord(1.0, 2.0);
    EXPECT_DOUBLE_EQ(coord.x, 1.0);
    EXPECT_DOUBLE_EQ(coord.y, 2.0);
    EXPECT_TRUE(std::isnan(coord.z));
    EXPECT_TRUE(std::isnan(coord.m));
    EXPECT_FALSE(coord.Is3D());
    EXPECT_FALSE(coord.IsMeasured());
}

TEST_F(CoordinateTest, Constructor3D_Creates3DCoordinate) {
    Coordinate coord(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(coord.x, 1.0);
    EXPECT_DOUBLE_EQ(coord.y, 2.0);
    EXPECT_DOUBLE_EQ(coord.z, 3.0);
    EXPECT_TRUE(std::isnan(coord.m));
    EXPECT_TRUE(coord.Is3D());
    EXPECT_FALSE(coord.IsMeasured());
}

TEST_F(CoordinateTest, Constructor4D_Creates4DCoordinate) {
    Coordinate coord(1.0, 2.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(coord.x, 1.0);
    EXPECT_DOUBLE_EQ(coord.y, 2.0);
    EXPECT_DOUBLE_EQ(coord.z, 3.0);
    EXPECT_DOUBLE_EQ(coord.m, 4.0);
    EXPECT_TRUE(coord.Is3D());
    EXPECT_TRUE(coord.IsMeasured());
}

TEST_F(CoordinateTest, Is3D_ReturnsCorrectValue) {
    EXPECT_FALSE(Coordinate(1, 2).Is3D());
    EXPECT_TRUE(Coordinate(1, 2, 3).Is3D());
    EXPECT_TRUE(Coordinate(1, 2, 3, 4).Is3D());
}

TEST_F(CoordinateTest, IsMeasured_ReturnsCorrectValue) {
    EXPECT_FALSE(Coordinate(1, 2).IsMeasured());
    EXPECT_FALSE(Coordinate(1, 2, 3).IsMeasured());
    EXPECT_TRUE(Coordinate(1, 2, 3, 4).IsMeasured());
}

TEST_F(CoordinateTest, IsEmpty_ReturnsCorrectValue) {
    EXPECT_TRUE(Coordinate().IsEmpty());
    EXPECT_TRUE(Coordinate::Empty().IsEmpty());
    EXPECT_FALSE(Coordinate(1, 2).IsEmpty());
    EXPECT_FALSE(Coordinate(1, 2, 3).IsEmpty());
}

TEST_F(CoordinateTest, Distance_CalculatesCorrectly) {
    Coordinate p1(0, 0);
    Coordinate p2(3, 4);
    EXPECT_DOUBLE_EQ(p1.Distance(p2), 5.0);
    EXPECT_DOUBLE_EQ(p2.Distance(p1), 5.0);
    
    Coordinate p3(0, 0, 0);
    Coordinate p4(1, 2, 2);
    EXPECT_DOUBLE_EQ(p3.Distance(p4), std::sqrt(5.0));
}

TEST_F(CoordinateTest, Distance3D_CalculatesCorrectly) {
    Coordinate p1(0, 0, 0);
    Coordinate p2(3, 4, 0);
    EXPECT_DOUBLE_EQ(p1.Distance3D(p2), 5.0);
    
    Coordinate p3(0, 0, 0);
    Coordinate p4(1, 2, 2);
    EXPECT_DOUBLE_EQ(p3.Distance3D(p4), 3.0);
}

TEST_F(CoordinateTest, Equals_ReturnsCorrectValue) {
    EXPECT_TRUE(Coordinate(1, 2).Equals(Coordinate(1, 2)));
    EXPECT_TRUE(Coordinate(1, 2, 3).Equals(Coordinate(1, 2, 3)));
    EXPECT_FALSE(Coordinate(1, 2).Equals(Coordinate(1, 3)));
    EXPECT_FALSE(Coordinate(1, 2).Equals(Coordinate(1, 2, 3)));
    
    EXPECT_TRUE(Coordinate::Empty().Equals(Coordinate::Empty()));
    EXPECT_FALSE(Coordinate::Empty().Equals(Coordinate(1, 2)));
}

TEST_F(CoordinateTest, OperatorPlus_AddsCoordinates) {
    Coordinate p1(1, 2);
    Coordinate p2(3, 4);
    Coordinate result = p1 + p2;
    
    EXPECT_DOUBLE_EQ(result.x, 4.0);
    EXPECT_DOUBLE_EQ(result.y, 6.0);
    EXPECT_FALSE(result.Is3D());
}

TEST_F(CoordinateTest, OperatorPlus_3DCoordinates) {
    Coordinate p1(1, 2, 3);
    Coordinate p2(4, 5, 6);
    Coordinate result = p1 + p2;
    
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 7.0);
    EXPECT_DOUBLE_EQ(result.z, 9.0);
}

TEST_F(CoordinateTest, OperatorMinus_SubtractsCoordinates) {
    Coordinate p1(5, 7);
    Coordinate p2(2, 3);
    Coordinate result = p1 - p2;
    
    EXPECT_DOUBLE_EQ(result.x, 3.0);
    EXPECT_DOUBLE_EQ(result.y, 4.0);
}

TEST_F(CoordinateTest, OperatorMultiply_ScalesCoordinate) {
    Coordinate p(1, 2, 3);
    Coordinate result = p * 2.0;
    
    EXPECT_DOUBLE_EQ(result.x, 2.0);
    EXPECT_DOUBLE_EQ(result.y, 4.0);
    EXPECT_DOUBLE_EQ(result.z, 6.0);
}

TEST_F(CoordinateTest, OperatorEqualEqual_ReturnsCorrectValue) {
    EXPECT_TRUE(Coordinate(1, 2) == Coordinate(1, 2));
    EXPECT_FALSE(Coordinate(1, 2) == Coordinate(1, 3));
}

TEST_F(CoordinateTest, OperatorNotEqual_ReturnsCorrectValue) {
    EXPECT_FALSE(Coordinate(1, 2) != Coordinate(1, 2));
    EXPECT_TRUE(Coordinate(1, 2) != Coordinate(1, 3));
}

TEST_F(CoordinateTest, Dot_CalculatesCorrectly) {
    Coordinate p1(1, 0);
    Coordinate p2(0, 1);
    EXPECT_DOUBLE_EQ(p1.Dot(p2), 0.0);
    
    Coordinate p3(1, 2);
    Coordinate p4(3, 4);
    EXPECT_DOUBLE_EQ(p3.Dot(p4), 11.0);
    
    Coordinate p5(1, 2, 3);
    Coordinate p6(4, 5, 6);
    EXPECT_DOUBLE_EQ(p5.Dot(p6), 32.0);
}

TEST_F(CoordinateTest, Cross_CalculatesCorrectly) {
    Coordinate p1(1, 0);
    Coordinate p2(0, 1);
    Coordinate result = p1.Cross(p2);
    EXPECT_DOUBLE_EQ(result.z, 1.0);
    
    Coordinate p3(3, 0);
    Coordinate p4(0, 4);
    Coordinate result2 = p3.Cross(p4);
    EXPECT_DOUBLE_EQ(result2.z, 12.0);
}

TEST_F(CoordinateTest, Length_CalculatesCorrectly) {
    EXPECT_DOUBLE_EQ(Coordinate(3, 4).Length(), 5.0);
    EXPECT_DOUBLE_EQ(Coordinate(0, 0).Length(), 0.0);
    EXPECT_DOUBLE_EQ(Coordinate(1, 0).Length(), 1.0);
}

TEST_F(CoordinateTest, Normalize_ReturnsUnitVector) {
    Coordinate p(3, 4);
    Coordinate normalized = p.Normalize();
    
    EXPECT_DOUBLE_EQ(normalized.x, 0.6);
    EXPECT_DOUBLE_EQ(normalized.y, 0.8);
    EXPECT_DOUBLE_EQ(normalized.Length(), 1.0);
}

TEST_F(CoordinateTest, Normalize_ZeroVector_ReturnsSame) {
    Coordinate p(0, 0);
    Coordinate normalized = p.Normalize();
    EXPECT_DOUBLE_EQ(normalized.x, 0.0);
    EXPECT_DOUBLE_EQ(normalized.y, 0.0);
}

TEST_F(CoordinateTest, FromPolar_CreatesCorrectCoordinate) {
    Coordinate p = Coordinate::FromPolar(1.0, 0.0);
    EXPECT_NEAR(p.x, 1.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(p.y, 0.0, DEFAULT_TOLERANCE);
    
    Coordinate p2 = Coordinate::FromPolar(1.0, PI / 2);
    EXPECT_NEAR(p2.x, 0.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(p2.y, 1.0, DEFAULT_TOLERANCE);
}

TEST_F(CoordinateTest, ToWKT_ReturnsCorrectFormat) {
    Coordinate p(1.5, 2.5);
    std::string wkt = p.ToWKT(1);
    EXPECT_TRUE(wkt.find("1.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("2.5") != std::string::npos);
    
    Coordinate p2(1.5, 2.5, 3.5);
    wkt = p2.ToWKT(1);
    EXPECT_TRUE(wkt.find("1.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("2.5") != std::string::npos);
    EXPECT_TRUE(wkt.find("3.5") != std::string::npos);
}

TEST_F(CoordinateTest, Empty_CreatesEmptyCoordinate) {
    Coordinate empty = Coordinate::Empty();
    EXPECT_TRUE(empty.IsEmpty());
}

}
}
