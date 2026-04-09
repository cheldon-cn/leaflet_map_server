#include <gtest/gtest.h>
#include "ogc/geom/linearring.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class LinearRingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LinearRingTest, Create_Default_ReturnsEmptyRing) {
    auto ring = LinearRing::Create();
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsEmpty());
}

TEST_F(LinearRingTest, CreateFromCoordinates_ReturnsValidRing) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    
    auto ring = LinearRing::Create(coords);
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsClosed());
}

TEST_F(LinearRingTest, CreateFromCoordinates_AutoClose_ClosesRing) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    
    auto ring = LinearRing::Create(coords, true);
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsClosed());
    EXPECT_EQ(ring->GetNumPoints(), 5);
}

TEST_F(LinearRingTest, GetGeometryType_ReturnsLineString) {
    auto ring = LinearRing::Create();
    EXPECT_EQ(ring->GetGeometryType(), GeomType::kLineString);
}

TEST_F(LinearRingTest, GetGeometryTypeString_ReturnsLinearRing) {
    auto ring = LinearRing::Create();
    EXPECT_STREQ(ring->GetGeometryTypeString(), "LINEARRING");
}

TEST_F(LinearRingTest, IsValidRing_ValidRing_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_TRUE(ring->IsValidRing());
}

TEST_F(LinearRingTest, IsValidRing_InvalidRing_ReturnsFalse) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_FALSE(ring->IsValidRing());
}

TEST_F(LinearRingTest, IsSimpleRing_SimpleRing_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_TRUE(ring->IsSimpleRing());
}

TEST_F(LinearRingTest, Area_CalculatesCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_DOUBLE_EQ(ring->Area(), 100.0);
}

TEST_F(LinearRingTest, GetPerimeter_CalculatesCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_DOUBLE_EQ(ring->GetPerimeter(), 40.0);
}

TEST_F(LinearRingTest, IsConvex_ConvexRing_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_TRUE(ring->IsConvex());
}

TEST_F(LinearRingTest, IsConvex_ConcaveRing_ReturnsFalse) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(5, 5);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_FALSE(ring->IsConvex());
}

TEST_F(LinearRingTest, IsClockwise_ClockwiseRing_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(0, 10);
    coords.emplace_back(10, 10);
    coords.emplace_back(10, 0);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_TRUE(ring->IsClockwise());
}

TEST_F(LinearRingTest, IsCounterClockwise_CcwRing_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    EXPECT_TRUE(ring->IsCounterClockwise());
}

TEST_F(LinearRingTest, Reverse_ChangesOrientation) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    bool wasCcw = ring->IsCounterClockwise();
    
    ring->Reverse();
    
    EXPECT_NE(ring->IsCounterClockwise(), wasCcw);
}

TEST_F(LinearRingTest, ForceClockwise_CcwRing_BecomesClockwise) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    ring->ForceClockwise();
    
    EXPECT_TRUE(ring->IsClockwise());
}

TEST_F(LinearRingTest, ForceCounterClockwise_CwRing_BecomesCcw) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(0, 10);
    coords.emplace_back(10, 10);
    coords.emplace_back(10, 0);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    ring->ForceCounterClockwise();
    
    EXPECT_TRUE(ring->IsCounterClockwise());
}

TEST_F(LinearRingTest, ContainsPoint_InsidePoint_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    
    EXPECT_TRUE(ring->ContainsPoint(Coordinate(5, 5)));
}

TEST_F(LinearRingTest, ContainsPoint_OutsidePoint_ReturnsFalse) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    
    EXPECT_FALSE(ring->ContainsPoint(Coordinate(15, 15)));
}

TEST_F(LinearRingTest, IsPointOnBoundary_OnBoundary_ReturnsTrue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    
    EXPECT_TRUE(ring->IsPointOnBoundary(Coordinate(5, 0)));
    EXPECT_TRUE(ring->IsPointOnBoundary(Coordinate(10, 5)));
}

TEST_F(LinearRingTest, IsPointOnBoundary_NotOnBoundary_ReturnsFalse) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    
    EXPECT_FALSE(ring->IsPointOnBoundary(Coordinate(5, 5)));
}

TEST_F(LinearRingTest, Offset_ReturnsOffsetRing) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    auto offsetRing = ring->Offset(1.0);
    
    ASSERT_NE(offsetRing, nullptr);
    EXPECT_TRUE(offsetRing->IsClosed());
}

TEST_F(LinearRingTest, Normalize_StandardizesRing) {
    CoordinateList coords;
    coords.emplace_back(5, 5);
    coords.emplace_back(15, 5);
    coords.emplace_back(15, 15);
    coords.emplace_back(5, 15);
    coords.emplace_back(5, 5);
    
    auto ring = LinearRing::Create(coords);
    ring->Normalize();
    
    EXPECT_TRUE(ring->IsClosed());
}

TEST_F(LinearRingTest, Triangulate_ReturnsTriangles) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    auto triangles = ring->Triangulate();
    
    EXPECT_GT(triangles.size(), 0);
}

TEST_F(LinearRingTest, CreateRectangle_ReturnsValidRing) {
    auto ring = LinearRing::CreateRectangle(0, 0, 10, 20);
    
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsClosed());
    EXPECT_DOUBLE_EQ(ring->Area(), 200.0);
}

TEST_F(LinearRingTest, CreateCircle_ReturnsValidRing) {
    auto ring = LinearRing::CreateCircle(0, 0, 10, 32);
    
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsClosed());
    EXPECT_NEAR(ring->Area(), PI * 100.0, 5.0);
}

TEST_F(LinearRingTest, CreateRegularPolygon_ReturnsValidRing) {
    auto ring = LinearRing::CreateRegularPolygon(0, 0, 10, 6);
    
    ASSERT_NE(ring, nullptr);
    EXPECT_TRUE(ring->IsClosed());
    EXPECT_EQ(ring->GetNumPoints(), 7);
}

TEST_F(LinearRingTest, Clone_ReturnsIdenticalRing) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto original = LinearRing::Create(coords);
    
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedRing = static_cast<LinearRing*>(clone.get());
    EXPECT_DOUBLE_EQ(clonedRing->Area(), 100.0);
}

TEST_F(LinearRingTest, CloneEmpty_ReturnsEmptyRing) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(0, 10);
    coords.emplace_back(0, 0);
    
    auto ring = LinearRing::Create(coords);
    
    auto emptyClone = ring->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyRing = static_cast<LinearRing*>(emptyClone.get());
    EXPECT_TRUE(emptyRing->IsEmpty());
}

}
}
