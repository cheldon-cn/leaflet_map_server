#include <gtest/gtest.h>
#include "ogc/multilinestring.h"
#include "ogc/common.h"

namespace ogc {
namespace test {

class MultiLineStringTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MultiLineStringTest, Create_Default_ReturnsEmptyMultiLineString) {
    auto multiLine = MultiLineString::Create();
    ASSERT_NE(multiLine, nullptr);
    EXPECT_TRUE(multiLine->IsEmpty());
    EXPECT_EQ(multiLine->GetNumLineStrings(), 0);
}

TEST_F(MultiLineStringTest, CreateFromLineStrings_ReturnsValidMultiLineString) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(20, 20);
    coords2.emplace_back(30, 30);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    ASSERT_NE(multiLine, nullptr);
    EXPECT_EQ(multiLine->GetNumLineStrings(), 2);
    EXPECT_FALSE(multiLine->IsEmpty());
}

TEST_F(MultiLineStringTest, GetGeometryType_ReturnsMultiLineString) {
    auto multiLine = MultiLineString::Create();
    EXPECT_EQ(multiLine->GetGeometryType(), GeomType::kMultiLineString);
}

TEST_F(MultiLineStringTest, GetGeometryTypeString_ReturnsCorrectString) {
    auto multiLine = MultiLineString::Create();
    EXPECT_STREQ(multiLine->GetGeometryTypeString(), "MULTILINESTRING");
}

TEST_F(MultiLineStringTest, GetDimension_ReturnsCurveDimension) {
    auto multiLine = MultiLineString::Create();
    EXPECT_EQ(multiLine->GetDimension(), Dimension::Curve);
}

TEST_F(MultiLineStringTest, GetCoordinateDimension_ReturnsCorrectValue) {
    std::vector<LineStringPtr> lines2D;
    CoordinateList coords2D;
    coords2D.emplace_back(0, 0);
    coords2D.emplace_back(10, 10);
    lines2D.push_back(LineString::Create(coords2D));
    auto multiLine2D = MultiLineString::Create(std::move(lines2D));
    EXPECT_EQ(multiLine2D->GetCoordinateDimension(), 2);
    
    std::vector<LineStringPtr> lines3D;
    CoordinateList coords3D;
    coords3D.emplace_back(0, 0, 0);
    coords3D.emplace_back(10, 10, 10);
    lines3D.push_back(LineString::Create(coords3D));
    auto multiLine3D = MultiLineString::Create(std::move(lines3D));
    EXPECT_EQ(multiLine3D->GetCoordinateDimension(), 3);
}

TEST_F(MultiLineStringTest, Is3D_ReturnsCorrectValue) {
    std::vector<LineStringPtr> lines2D;
    CoordinateList coords2D;
    coords2D.emplace_back(0, 0);
    coords2D.emplace_back(10, 10);
    lines2D.push_back(LineString::Create(coords2D));
    auto multiLine2D = MultiLineString::Create(std::move(lines2D));
    EXPECT_FALSE(multiLine2D->Is3D());
    
    std::vector<LineStringPtr> lines3D;
    CoordinateList coords3D;
    coords3D.emplace_back(0, 0, 0);
    coords3D.emplace_back(10, 10, 10);
    lines3D.push_back(LineString::Create(coords3D));
    auto multiLine3D = MultiLineString::Create(std::move(lines3D));
    EXPECT_TRUE(multiLine3D->Is3D());
}

TEST_F(MultiLineStringTest, IsMeasured_ReturnsCorrectValue) {
    std::vector<LineStringPtr> lines;
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    auto multiLine = MultiLineString::Create(std::move(lines));
    EXPECT_FALSE(multiLine->IsMeasured());
}

TEST_F(MultiLineStringTest, AddLineString_AddsLine) {
    auto multiLine = MultiLineString::Create();
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    multiLine->AddLineString(LineString::Create(coords));
    
    EXPECT_EQ(multiLine->GetNumLineStrings(), 1);
}

TEST_F(MultiLineStringTest, RemoveLineString_RemovesCorrectLine) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(20, 20);
    coords2.emplace_back(30, 30);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    multiLine->RemoveLineString(0);
    
    EXPECT_EQ(multiLine->GetNumLineStrings(), 1);
}

TEST_F(MultiLineStringTest, RemoveLineString_InvalidIndex_ThrowsException) {
    auto multiLine = MultiLineString::Create();
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    multiLine->AddLineString(LineString::Create(coords));
    
    EXPECT_THROW(multiLine->RemoveLineString(5), GeometryException);
}

TEST_F(MultiLineStringTest, Clear_RemovesAllLines) {
    std::vector<LineStringPtr> lines;
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    multiLine->Clear();
    
    EXPECT_TRUE(multiLine->IsEmpty());
    EXPECT_EQ(multiLine->GetNumLineStrings(), 0);
}

TEST_F(MultiLineStringTest, GetLineStringN_ReturnsCorrectLine) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(20, 20);
    coords2.emplace_back(30, 30);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    const LineString* line0 = multiLine->GetLineStringN(0);
    ASSERT_NE(line0, nullptr);
    EXPECT_DOUBLE_EQ(line0->GetStartPoint().x, 0.0);
    
    const LineString* line1 = multiLine->GetLineStringN(1);
    ASSERT_NE(line1, nullptr);
    EXPECT_DOUBLE_EQ(line1->GetStartPoint().x, 20.0);
}

TEST_F(MultiLineStringTest, GetLineStringN_InvalidIndex_ThrowsException) {
    auto multiLine = MultiLineString::Create();
    EXPECT_THROW(multiLine->GetLineStringN(0), GeometryException);
}

TEST_F(MultiLineStringTest, Length_CalculatesCorrectly) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(3, 4);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(0, 0);
    coords2.emplace_back(6, 8);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    EXPECT_DOUBLE_EQ(multiLine->Length(), 5.0 + 10.0);
}

TEST_F(MultiLineStringTest, Length3D_CalculatesCorrectly) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords;
    coords.emplace_back(0, 0, 0);
    coords.emplace_back(3, 4, 0);
    lines.push_back(LineString::Create(coords));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    EXPECT_DOUBLE_EQ(multiLine->Length3D(), 5.0);
}

TEST_F(MultiLineStringTest, IsClosed_ReturnsCorrectValue) {
    std::vector<LineStringPtr> openLines;
    CoordinateList openCoords;
    openCoords.emplace_back(0, 0);
    openCoords.emplace_back(10, 10);
    openLines.push_back(LineString::Create(openCoords));
    auto openMultiLine = MultiLineString::Create(std::move(openLines));
    EXPECT_FALSE(openMultiLine->IsClosed());
    
    std::vector<LineStringPtr> closedLines;
    CoordinateList closedCoords;
    closedCoords.emplace_back(0, 0);
    closedCoords.emplace_back(10, 10);
    closedCoords.emplace_back(0, 0);
    closedLines.push_back(LineString::Create(closedCoords));
    auto closedMultiLine = MultiLineString::Create(std::move(closedLines));
    EXPECT_TRUE(closedMultiLine->IsClosed());
}

TEST_F(MultiLineStringTest, Merge_ReturnsSingleLineString) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(10, 10);
    coords2.emplace_back(20, 20);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    auto merged = multiLine->Merge();
    
    ASSERT_NE(merged, nullptr);
    EXPECT_EQ(merged->GetNumCoordinates(), 3);
}

TEST_F(MultiLineStringTest, GetNumCoordinates_ReturnsCorrectValue) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(20, 20);
    coords2.emplace_back(30, 30);
    coords2.emplace_back(40, 40);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    EXPECT_EQ(multiLine->GetNumCoordinates(), 5);
}

TEST_F(MultiLineStringTest, GetCoordinateN_ReturnsCorrectCoordinate) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    Coordinate coord0 = multiLine->GetCoordinateN(0);
    EXPECT_DOUBLE_EQ(coord0.x, 0.0);
    
    Coordinate coord1 = multiLine->GetCoordinateN(1);
    EXPECT_DOUBLE_EQ(coord1.x, 10.0);
}

TEST_F(MultiLineStringTest, GetCoordinateN_InvalidIndex_ThrowsException) {
    auto multiLine = MultiLineString::Create();
    EXPECT_THROW(multiLine->GetCoordinateN(0), GeometryException);
}

TEST_F(MultiLineStringTest, GetCoordinates_ReturnsAllCoordinates) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(0, 0);
    coords1.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords1));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    CoordinateList result = multiLine->GetCoordinates();
    EXPECT_EQ(result.size(), 2);
}

TEST_F(MultiLineStringTest, AsText_ReturnsCorrectWKT) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    std::string wkt = multiLine->AsText();
    EXPECT_TRUE(wkt.find("MULTILINESTRING") != std::string::npos);
}

TEST_F(MultiLineStringTest, AsText_EmptyMultiLineString_ReturnsEmptyWKT) {
    auto multiLine = MultiLineString::Create();
    EXPECT_EQ(multiLine->AsText(), "MULTILINESTRING EMPTY");
}

TEST_F(MultiLineStringTest, Clone_ReturnsIdenticalMultiLineString) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    
    auto original = MultiLineString::Create(std::move(lines));
    
    auto clone = original->Clone();
    
    ASSERT_NE(clone, nullptr);
    auto clonedMultiLine = static_cast<MultiLineString*>(clone.get());
    EXPECT_EQ(clonedMultiLine->GetNumLineStrings(), 1);
}

TEST_F(MultiLineStringTest, CloneEmpty_ReturnsEmptyMultiLineString) {
    std::vector<LineStringPtr> lines;
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    auto emptyClone = multiLine->CloneEmpty();
    
    ASSERT_NE(emptyClone, nullptr);
    auto emptyMultiLine = static_cast<MultiLineString*>(emptyClone.get());
    EXPECT_TRUE(emptyMultiLine->IsEmpty());
}

TEST_F(MultiLineStringTest, GetEnvelope_ReturnsCorrectEnvelope) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords1;
    coords1.emplace_back(5, 10);
    coords1.emplace_back(15, 5);
    lines.push_back(LineString::Create(coords1));
    
    CoordinateList coords2;
    coords2.emplace_back(0, 20);
    coords2.emplace_back(25, 0);
    lines.push_back(LineString::Create(coords2));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    const Envelope& env = multiLine->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 25.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 20.0);
}

TEST_F(MultiLineStringTest, GetEnvelope_EmptyMultiLineString_ReturnsNullEnvelope) {
    auto multiLine = MultiLineString::Create();
    const Envelope& env = multiLine->GetEnvelope();
    EXPECT_TRUE(env.IsNull());
}

TEST_F(MultiLineStringTest, GetGeometryN_ReturnsCorrectGeometry) {
    std::vector<LineStringPtr> lines;
    
    CoordinateList coords;
    coords.emplace_back(0, 0);
    coords.emplace_back(10, 10);
    lines.push_back(LineString::Create(coords));
    
    auto multiLine = MultiLineString::Create(std::move(lines));
    
    const Geometry* geom0 = multiLine->GetGeometryN(0);
    ASSERT_NE(geom0, nullptr);
    EXPECT_EQ(geom0->GetGeometryType(), GeomType::kLineString);
}

TEST_F(MultiLineStringTest, GetGeometryN_InvalidIndex_ThrowsException) {
    auto multiLine = MultiLineString::Create();
    EXPECT_THROW(multiLine->GetGeometryN(0), GeometryException);
}

}
}
