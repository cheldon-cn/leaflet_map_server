#include <gtest/gtest.h>
#include "ogc/geom/linestring.h"

namespace ogc {
namespace test {

class LineStringTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LineStringTest, Create_Default_ReturnsEmptyLineString) {
    auto line = LineString::Create();
    ASSERT_NE(line, nullptr);
    EXPECT_TRUE(line->IsEmpty());
    EXPECT_EQ(line->GetNumPoints(), 0);
}

TEST_F(LineStringTest, CreateFromCoordinates_ReturnsValidLineString) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    
    auto line = LineString::Create(coords);
    ASSERT_NE(line, nullptr);
    EXPECT_EQ(line->GetNumPoints(), 3);
    EXPECT_FALSE(line->IsEmpty());
}

TEST_F(LineStringTest, GetGeometryType_ReturnsLineString) {
    auto line = LineString::Create();
    EXPECT_EQ(line->GetGeometryType(), GeomType::kLineString);
}

TEST_F(LineStringTest, GetGeometryTypeString_ReturnsCorrectString) {
    auto line = LineString::Create();
    EXPECT_STREQ(line->GetGeometryTypeString(), "LINESTRING");
}

TEST_F(LineStringTest, GetDimension_ReturnsCurveDimension) {
    auto line = LineString::Create();
    EXPECT_EQ(line->GetDimension(), Dimension::Curve);
}

TEST_F(LineStringTest, Is3D_ReturnsCorrectValue) {
    CoordinateList coords2D;
    coords2D.emplace_back(0, 0);
    coords2D.emplace_back(10, 10);
    auto line2D = LineString::Create(coords2D);
    EXPECT_FALSE(line2D->Is3D());
    
    CoordinateList coords3D;
    coords3D.emplace_back(0, 0, 0);
    coords3D.emplace_back(10, 10, 10);
    auto line3D = LineString::Create(coords3D);
    EXPECT_TRUE(line3D->Is3D());
}

TEST_F(LineStringTest, IsMeasured_ReturnsCorrectValue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto line = LineString::Create(coords);
    EXPECT_FALSE(line->IsMeasured());
    
    coords[0].m = 0;
    coords[1].m = 10;
    auto measuredLine = LineString::Create(coords);
    EXPECT_TRUE(measuredLine->IsMeasured());
}

TEST_F(LineStringTest, IsClosed_ReturnsCorrectValue) {
    CoordinateList openCoords;
    openCoords.emplace_back(0, 0);
    openCoords.emplace_back(10, 10);
    auto openLine = LineString::Create(openCoords);
    EXPECT_FALSE(openLine->IsClosed());
    
    CoordinateList closedCoords;
    closedCoords.emplace_back(0, 0);
    closedCoords.emplace_back(10, 10);
    closedCoords.emplace_back(0, 0);
    auto closedLine = LineString::Create(closedCoords);
    EXPECT_TRUE(closedLine->IsClosed());
}

TEST_F(LineStringTest, IsRing_ReturnsCorrectValue) {
    CoordinateList ringCoords;
    ringCoords.emplace_back(0, 0);
    ringCoords.emplace_back(10, 0);
    ringCoords.emplace_back(10, 10);
    ringCoords.emplace_back(0, 10);
    ringCoords.emplace_back(0, 0);
    auto ring = LineString::Create(ringCoords);
    EXPECT_TRUE(ring->IsRing());
    
    auto openLine = LineString::Create();
    openLine->AddPoint(Coordinate(0, 0));
    openLine->AddPoint(Coordinate(10, 10));
    EXPECT_FALSE(openLine->IsRing());
}

TEST_F(LineStringTest, GetNumPoints_ReturnsCorrectValue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    
    auto line = LineString::Create(coords);
    EXPECT_EQ(line->GetNumPoints(), 3);
    EXPECT_EQ(line->GetNumCoordinates(), 3);
}

TEST_F(LineStringTest, GetCoordinateN_ReturnsCorrectCoordinate) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    
    auto line = LineString::Create(coords);
    
    EXPECT_DOUBLE_EQ(line->GetCoordinateN(0).x, 0.0);
    EXPECT_DOUBLE_EQ(line->GetCoordinateN(1).x, 10.0);
    EXPECT_DOUBLE_EQ(line->GetCoordinateN(2).x, 20.0);
}

TEST_F(LineStringTest, GetCoordinateN_InvalidIndex_ThrowsException) {
    auto line = LineString::Create();
    EXPECT_THROW(line->GetCoordinateN(0), GeometryException);
}

TEST_F(LineStringTest, GetPointN_ReturnsCorrectPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    
    auto line = LineString::Create(coords);
    
    Coordinate p0 = line->GetPointN(0);
    EXPECT_DOUBLE_EQ(p0.x, 0.0);
    EXPECT_DOUBLE_EQ(p0.y, 0.0);
    
    Coordinate p1 = line->GetPointN(1);
    EXPECT_DOUBLE_EQ(p1.x, 10.0);
    EXPECT_DOUBLE_EQ(p1.y, 10.0);
}

TEST_F(LineStringTest, GetStartPoint_ReturnsFirstPoint) {
    CoordinateList coords;
    coords.emplace_back(5, 10);
    coords.emplace_back(15, 20);
    
    auto line = LineString::Create(coords);
    Coordinate start = line->GetStartPoint();
    
    EXPECT_DOUBLE_EQ(start.x, 5.0);
    EXPECT_DOUBLE_EQ(start.y, 10.0);
}

TEST_F(LineStringTest, GetEndPoint_ReturnsLastPoint) {
    CoordinateList coords;
    coords.emplace_back(5, 10);
    coords.emplace_back(15, 20);
    
    auto line = LineString::Create(coords);
    Coordinate end = line->GetEndPoint();
    
    EXPECT_DOUBLE_EQ(end.x, 15.0);
    EXPECT_DOUBLE_EQ(end.y, 20.0);
}

TEST_F(LineStringTest, SetCoordinates_UpdatesCoordinates) {
    auto line = LineString::Create();
    
    CoordinateList newCoords;
    newCoords.emplace_back(0, 0);
    newCoords.emplace_back(10, 10);
    line->SetCoordinates(newCoords);
    
    EXPECT_EQ(line->GetNumPoints(), 2);
    EXPECT_DOUBLE_EQ(line->GetStartPoint().x, 0.0);
}

TEST_F(LineStringTest, AddPoint_AddsCoordinate) {
    auto line = LineString::Create();
    line->AddPoint(Coordinate(0, 0));
    line->AddPoint(Coordinate(10, 10));
    
    EXPECT_EQ(line->GetNumPoints(), 2);
}

TEST_F(LineStringTest, InsertPoint_InsertsCoordinate) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(20, 0);
    auto line = LineString::Create(coords);
    
    line->InsertPoint(1, Coordinate(10, 10));
    
    EXPECT_EQ(line->GetNumPoints(), 3);
    EXPECT_DOUBLE_EQ(line->GetPointN(1).x, 10.0);
}

TEST_F(LineStringTest, RemovePoint_RemovesCoordinate) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    auto line = LineString::Create(coords);
    
    line->RemovePoint(1);
    
    EXPECT_EQ(line->GetNumPoints(), 2);
    EXPECT_DOUBLE_EQ(line->GetPointN(1).x, 20.0);
}

TEST_F(LineStringTest, Clear_RemovesAllCoordinates) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto line = LineString::Create(coords);
    
    line->Clear();
    
    EXPECT_TRUE(line->IsEmpty());
    EXPECT_EQ(line->GetNumPoints(), 0);
}

TEST_F(LineStringTest, GetNumSegments_ReturnsCorrectValue) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    coords.emplace_back(30, 10);
    auto line = LineString::Create(coords);
    
    EXPECT_EQ(line->GetNumSegments(), 3);
}

TEST_F(LineStringTest, GetSegment_ReturnsCorrectSegment) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    auto line = LineString::Create(coords);
    
    auto seg = line->GetSegment(0);
    EXPECT_DOUBLE_EQ(seg.start.x, 0.0);
    EXPECT_DOUBLE_EQ(seg.end.x, 10.0);
    EXPECT_DOUBLE_EQ(seg.length, std::sqrt(200.0));
}

TEST_F(LineStringTest, Length_CalculatesCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(3, 4);
    coords.emplace_back(7, 4);
    auto line = LineString::Create(coords);
    
    double expectedLength = 5.0 + 4.0;
    EXPECT_NEAR(line->Length(), expectedLength, DEFAULT_TOLERANCE);
}

TEST_F(LineStringTest, Length3D_CalculatesCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 0, 0);
    coords.emplace_back(3, 4, 0);
    auto line = LineString::Create(coords);
    
    EXPECT_NEAR(line->Length3D(), 5.0, DEFAULT_TOLERANCE);
}

TEST_F(LineStringTest, GetCumulativeLength_ReturnsCorrectValues) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(3, 4);
    coords.emplace_back(7, 4);
    auto line = LineString::Create(coords);
    
    auto lengths = line->GetCumulativeLength();
    ASSERT_EQ(lengths.size(), 3);
    EXPECT_NEAR(lengths[0], 0.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(lengths[1], 5.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(lengths[2], 9.0, DEFAULT_TOLERANCE);
}

TEST_F(LineStringTest, Interpolate_ReturnsInterpolatedPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    auto line = LineString::Create(coords);
    
    auto point = line->Interpolate(5.0);
    EXPECT_DOUBLE_EQ(point.x, 5.0);
    EXPECT_DOUBLE_EQ(point.y, 0.0);
}

TEST_F(LineStringTest, Interpolate_BeyondLength_ReturnsEndPoint) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    auto line = LineString::Create(coords);
    
    auto point = line->Interpolate(20.0);
    EXPECT_DOUBLE_EQ(point.x, 10.0);
    EXPECT_DOUBLE_EQ(point.y, 0.0);
}

TEST_F(LineStringTest, GetSubLine_ReturnsCorrectSubLine) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 0);
    coords.emplace_back(20, 0);
    coords.emplace_back(30, 0);
    auto line = LineString::Create(coords);
    
    auto subLine = line->GetSubLine(5.0, 25.0);
    
    EXPECT_GE(subLine->GetNumPoints(), 2);
}

TEST_F(LineStringTest, RemoveDuplicatePoints_RemovesDuplicates) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 20);
    auto line = LineString::Create(coords);
    
    line->RemoveDuplicatePoints();
    
    EXPECT_EQ(line->GetNumPoints(), 3);
}

TEST_F(LineStringTest, IsStraight_ReturnsCorrectValue) {
    CoordinateList straightCoords;
    straightCoords.emplace_back(0, 0);
    straightCoords.emplace_back(10, 10);
    straightCoords.emplace_back(20, 20);
    auto straightLine = LineString::Create(straightCoords);
    EXPECT_TRUE(straightLine->IsStraight());
    
    CoordinateList curvedCoords;
    curvedCoords.emplace_back(0, 0);
    curvedCoords.emplace_back(10, 10);
    curvedCoords.emplace_back(15, 5);
    curvedCoords.emplace_back(20, 20);
    auto curvedLine = LineString::Create(curvedCoords);
    EXPECT_FALSE(curvedLine->IsStraight());
}

TEST_F(LineStringTest, AsText_ReturnsCorrectWKT) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto line = LineString::Create(coords);
    
    std::string wkt = line->AsText();
    EXPECT_TRUE(wkt.find("LINESTRING") != std::string::npos);
    EXPECT_TRUE(wkt.find("0") != std::string::npos);
    EXPECT_TRUE(wkt.find("10") != std::string::npos);
}

TEST_F(LineStringTest, AsText_EmptyLine_ReturnsEmptyWKT) {
    auto line = LineString::Create();
    EXPECT_EQ(line->AsText(), "LINESTRING EMPTY");
}

TEST_F(LineStringTest, Clone_ReturnsIdenticalLineString) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    auto original = LineString::Create(coords);
    
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedLine = static_cast<LineString*>(clone.get());
    EXPECT_EQ(clonedLine->GetNumPoints(), 2);
    EXPECT_DOUBLE_EQ(clonedLine->GetPointN(0).x, 0.0);
}

TEST_F(LineStringTest, CloneEmpty_ReturnsEmptyLineString) {
    auto line = LineString::Create();
    line->AddPoint(Coordinate(0, 0));
    
    auto emptyClone = line->CloneEmpty();
    ASSERT_NE(emptyClone, nullptr);
    EXPECT_TRUE(emptyClone->IsEmpty());
}

TEST_F(LineStringTest, GetEnvelope_ReturnsCorrectEnvelope) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    auto line = LineString::Create(coords);
    
    const Envelope& env = line->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 20.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 10.0);
}

TEST_F(LineStringTest, Iterator_BeginEnd_WorksCorrectly) {
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    coords.emplace_back(20, 0);
    auto line = LineString::Create(coords);
    
    size_t count = 0;
    for (const auto& coord : *line) {
        (void)coord;
        count++;
    }
    EXPECT_EQ(count, 3);
}

}
}
