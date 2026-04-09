#include <gtest/gtest.h>
#include "ogc/geom/envelope.h"

namespace ogc {
namespace test {

class EnvelopeTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EnvelopeTest, DefaultConstructor_CreatesNullEnvelope) {
    Envelope env;
    EXPECT_TRUE(env.IsNull());
    EXPECT_TRUE(env.IsEmpty());
}

TEST_F(EnvelopeTest, ConstructorFromBounds_CreatesValidEnvelope) {
    Envelope env(0, 0, 10, 20);
    EXPECT_FALSE(env.IsNull());
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 20.0);
}

TEST_F(EnvelopeTest, ConstructorFromBounds_NormalizesBounds) {
    Envelope env(10, 20, 0, 0);
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 20.0);
}

TEST_F(EnvelopeTest, ConstructorFromCoordinates_CreatesValidEnvelope) {
    Coordinate p1(0, 10);
    Coordinate p2(5, 0);
    Envelope env(p1, p2);
    
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 10.0);
}

TEST_F(EnvelopeTest, ConstructorFromCoordinateList_CreatesValidEnvelope) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 5);
    coords.emplace_back(5, 15);
    
    Envelope env(coords);
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 15.0);
}

TEST_F(EnvelopeTest, GetWidth_ReturnsCorrectValue) {
    Envelope env(0, 0, 10, 20);
    EXPECT_DOUBLE_EQ(env.GetWidth(), 10.0);
    
    Envelope nullEnv;
    EXPECT_DOUBLE_EQ(nullEnv.GetWidth(), 0.0);
}

TEST_F(EnvelopeTest, GetHeight_ReturnsCorrectValue) {
    Envelope env(0, 0, 10, 20);
    EXPECT_DOUBLE_EQ(env.GetHeight(), 20.0);
    
    Envelope nullEnv;
    EXPECT_DOUBLE_EQ(nullEnv.GetHeight(), 0.0);
}

TEST_F(EnvelopeTest, GetArea_ReturnsCorrectValue) {
    Envelope env(0, 0, 10, 20);
    EXPECT_DOUBLE_EQ(env.GetArea(), 200.0);
    
    Envelope nullEnv;
    EXPECT_DOUBLE_EQ(nullEnv.GetArea(), 0.0);
}

TEST_F(EnvelopeTest, GetPerimeter_ReturnsCorrectValue) {
    Envelope env(0, 0, 10, 20);
    EXPECT_DOUBLE_EQ(env.GetPerimeter(), 60.0);
    
    Envelope nullEnv;
    EXPECT_DOUBLE_EQ(nullEnv.GetPerimeter(), 0.0);
}

TEST_F(EnvelopeTest, GetCentre_ReturnsCorrectCoordinate) {
    Envelope env(0, 0, 10, 20);
    Coordinate centre = env.GetCentre();
    
    EXPECT_DOUBLE_EQ(centre.x, 5.0);
    EXPECT_DOUBLE_EQ(centre.y, 10.0);
}

TEST_F(EnvelopeTest, GetCentre_NullEnvelope_ReturnsEmpty) {
    Envelope env;
    EXPECT_TRUE(env.GetCentre().IsEmpty());
}

TEST_F(EnvelopeTest, IsPoint_ReturnsCorrectValue) {
    Envelope point(5, 5, 5, 5);
    EXPECT_TRUE(point.IsPoint());
    
    Envelope area(0, 0, 10, 10);
    EXPECT_FALSE(area.IsPoint());
    
    Envelope null;
    EXPECT_FALSE(null.IsPoint());
}

TEST_F(EnvelopeTest, SetNull_ResetsEnvelope) {
    Envelope env(0, 0, 10, 10);
    env.SetNull();
    EXPECT_TRUE(env.IsNull());
}

TEST_F(EnvelopeTest, ExpandToInclude_Coordinate_ExpandsEnvelope) {
    Envelope env(0, 0, 10, 10);
    env.ExpandToInclude(Coordinate(15, 5));
    
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
}

TEST_F(EnvelopeTest, ExpandToInclude_Coordinate_NullEnvelope) {
    Envelope env;
    env.ExpandToInclude(Coordinate(5, 10));
    
    EXPECT_FALSE(env.IsNull());
    EXPECT_DOUBLE_EQ(env.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 5.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 10.0);
}

TEST_F(EnvelopeTest, ExpandToInclude_CoordinateList_ExpandsEnvelope) {
    Envelope env;
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(5, 15);
    
    env.ExpandToInclude(coords);
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 15.0);
}

TEST_F(EnvelopeTest, ExpandToInclude_Envelope_ExpandsEnvelope) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    
    env1.ExpandToInclude(env2);
    EXPECT_DOUBLE_EQ(env1.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env1.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env1.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(env1.GetMaxY(), 15.0);
}

TEST_F(EnvelopeTest, ExpandBy_ExpandsEnvelope) {
    Envelope env(5, 5, 10, 10);
    env.ExpandBy(2, 3);
    
    EXPECT_DOUBLE_EQ(env.GetMinX(), 3.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 2.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 12.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 13.0);
}

TEST_F(EnvelopeTest, Contains_Coordinate_ReturnsCorrectValue) {
    Envelope env(0, 0, 10, 10);
    
    EXPECT_TRUE(env.Contains(Coordinate(5, 5)));
    EXPECT_TRUE(env.Contains(Coordinate(0, 0)));
    EXPECT_TRUE(env.Contains(Coordinate(10, 10)));
    EXPECT_FALSE(env.Contains(Coordinate(15, 5)));
    EXPECT_FALSE(env.Contains(Coordinate(-1, 5)));
}

TEST_F(EnvelopeTest, Contains_Envelope_ReturnsCorrectValue) {
    Envelope outer(0, 0, 20, 20);
    Envelope inner(5, 5, 15, 15);
    Envelope partial(5, 5, 25, 15);
    
    EXPECT_TRUE(outer.Contains(inner));
    EXPECT_FALSE(outer.Contains(partial));
    EXPECT_FALSE(inner.Contains(outer));
}

TEST_F(EnvelopeTest, Intersects_ReturnsCorrectValue) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    Envelope env3(20, 20, 30, 30);
    Envelope env4(10, 0, 20, 10);
    
    EXPECT_TRUE(env1.Intersects(env2));
    EXPECT_FALSE(env1.Intersects(env3));
    EXPECT_TRUE(env1.Intersects(env4));
}

TEST_F(EnvelopeTest, Overlaps_ReturnsCorrectValue) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    Envelope env3(0, 0, 5, 5);
    
    EXPECT_TRUE(env1.Overlaps(env2));
    EXPECT_FALSE(env1.Overlaps(env3));
}

TEST_F(EnvelopeTest, Equals_ReturnsCorrectValue) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(0, 0, 10, 10);
    Envelope env3(0, 0, 10, 10.00001);
    
    EXPECT_TRUE(env1.Equals(env2));
    EXPECT_FALSE(env1.Equals(env3));
    EXPECT_TRUE(env1.Equals(env3, 0.001));
}

TEST_F(EnvelopeTest, Intersection_ReturnsCorrectEnvelope) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    
    Envelope result = env1.Intersection(env2);
    EXPECT_DOUBLE_EQ(result.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(result.GetMinY(), 5.0);
    EXPECT_DOUBLE_EQ(result.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(result.GetMaxY(), 10.0);
}

TEST_F(EnvelopeTest, Intersection_NoIntersection_ReturnsNull) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(20, 20, 30, 30);
    
    Envelope result = env1.Intersection(env2);
    EXPECT_TRUE(result.IsNull());
}

TEST_F(EnvelopeTest, Union_ReturnsCorrectEnvelope) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    
    Envelope result = env1.Union(env2);
    EXPECT_DOUBLE_EQ(result.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(result.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(result.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(result.GetMaxY(), 15.0);
}

TEST_F(EnvelopeTest, Union_WithNull_ReturnsOther) {
    Envelope env1(0, 0, 10, 10);
    Envelope nullEnv;
    
    Envelope result = env1.Union(nullEnv);
    EXPECT_TRUE(result.Equals(env1));
    
    Envelope result2 = nullEnv.Union(env1);
    EXPECT_TRUE(result2.Equals(env1));
}

TEST_F(EnvelopeTest, Distance_Coordinate_ReturnsCorrectValue) {
    Envelope env(0, 0, 10, 10);
    
    EXPECT_DOUBLE_EQ(env.Distance(Coordinate(5, 5)), 0.0);
    EXPECT_DOUBLE_EQ(env.Distance(Coordinate(15, 5)), 5.0);
    EXPECT_DOUBLE_EQ(env.Distance(Coordinate(5, 15)), 5.0);
    EXPECT_DOUBLE_EQ(env.Distance(Coordinate(15, 15)), std::sqrt(50.0));
}

TEST_F(EnvelopeTest, Distance_Envelope_ReturnsCorrectValue) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(15, 0, 20, 10);
    Envelope env3(0, 15, 10, 20);
    
    EXPECT_DOUBLE_EQ(env1.Distance(env2), 5.0);
    EXPECT_DOUBLE_EQ(env1.Distance(env3), 5.0);
}

TEST_F(EnvelopeTest, Distance_IntersectingEnvelopes_ReturnsZero) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    
    EXPECT_DOUBLE_EQ(env1.Distance(env2), 0.0);
}

TEST_F(EnvelopeTest, GetLowerLeft_GetUpperRight_ReturnCorrectCoordinates) {
    Envelope env(0, 0, 10, 10);
    
    Coordinate ll = env.GetLowerLeft();
    EXPECT_DOUBLE_EQ(ll.x, 0.0);
    EXPECT_DOUBLE_EQ(ll.y, 0.0);
    
    Coordinate ur = env.GetUpperRight();
    EXPECT_DOUBLE_EQ(ur.x, 10.0);
    EXPECT_DOUBLE_EQ(ur.y, 10.0);
}

TEST_F(EnvelopeTest, OperatorEqualEqual_ReturnsCorrectValue) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(0, 0, 10, 10);
    Envelope env3(0, 0, 10, 20);
    
    EXPECT_TRUE(env1 == env2);
    EXPECT_FALSE(env1 == env3);
}

TEST_F(EnvelopeTest, OperatorNotEqual_ReturnsCorrectValue) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(0, 0, 10, 10);
    Envelope env3(0, 0, 10, 20);
    
    EXPECT_FALSE(env1 != env2);
    EXPECT_TRUE(env1 != env3);
}

}
}
