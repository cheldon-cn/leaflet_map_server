#include <gtest/gtest.h>
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class CommonTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CommonTest, GeomType_AllTypesHaveNames) {
    EXPECT_STREQ(GetGeomTypeName(GeomType::kUnknown), "Unknown");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kPoint), "Point");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kLineString), "LineString");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kPolygon), "Polygon");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kMultiPoint), "MultiPoint");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kMultiLineString), "MultiLineString");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kMultiPolygon), "MultiPolygon");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kGeometryCollection), "GeometryCollection");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kCircularString), "CircularString");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kCompoundCurve), "CompoundCurve");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kCurvePolygon), "CurvePolygon");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kMultiCurve), "MultiCurve");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kMultiSurface), "MultiSurface");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kPolyhedralSurface), "PolyhedralSurface");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kTIN), "TIN");
    EXPECT_STREQ(GetGeomTypeName(GeomType::kTriangle), "Triangle");
}

TEST_F(CommonTest, GeomResult_AllCodesHaveDescriptions) {
    EXPECT_STREQ(GetResultString(GeomResult::kSuccess), "Success");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidGeometry), "Invalid geometry");
    EXPECT_STREQ(GetResultString(GeomResult::kEmptyGeometry), "Empty geometry");
    EXPECT_STREQ(GetResultString(GeomResult::kNullGeometry), "Null geometry");
    EXPECT_STREQ(GetResultString(GeomResult::kTopologyError), "Topology error");
    EXPECT_STREQ(GetResultString(GeomResult::kSelfIntersection), "Self intersection");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidDimension), "Invalid dimension");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidCoordinate), "Invalid coordinate");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidRing), "Invalid ring");
    EXPECT_STREQ(GetResultString(GeomResult::kNotClosedRing), "Ring not closed");
    EXPECT_STREQ(GetResultString(GeomResult::kDegenerateGeometry), "Degenerate geometry");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidPolygonShell), "Invalid polygon shell");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidHole), "Invalid hole");
    EXPECT_STREQ(GetResultString(GeomResult::kHoleOutsideShell), "Hole outside shell");
    EXPECT_STREQ(GetResultString(GeomResult::kNestedHoles), "Nested holes");
    EXPECT_STREQ(GetResultString(GeomResult::kDisconnectedInterior), "Disconnected interior");
    EXPECT_STREQ(GetResultString(GeomResult::kRepeatedPoint), "Repeated point");
    EXPECT_STREQ(GetResultString(GeomResult::kTooFewPoints), "Too few points");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidParameters), "Invalid parameters");
    EXPECT_STREQ(GetResultString(GeomResult::kOutOfRange), "Index out of range");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidSRID), "Invalid SRID");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidTolerance), "Invalid tolerance");
    EXPECT_STREQ(GetResultString(GeomResult::kNullPointer), "Null pointer");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidIndex), "Invalid index");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidWidth), "Invalid width");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidPrecision), "Invalid precision");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidBufferSize), "Invalid buffer size");
    EXPECT_STREQ(GetResultString(GeomResult::kNotSupported), "Operation not supported");
    EXPECT_STREQ(GetResultString(GeomResult::kNotImplemented), "Operation not implemented");
    EXPECT_STREQ(GetResultString(GeomResult::kOperationFailed), "Operation failed");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidOperation), "Invalid operation");
    EXPECT_STREQ(GetResultString(GeomResult::kTypeMismatch), "Type mismatch");
    EXPECT_STREQ(GetResultString(GeomResult::kReadOnly), "Read only");
    EXPECT_STREQ(GetResultString(GeomResult::kOutOfMemory), "Out of memory");
    EXPECT_STREQ(GetResultString(GeomResult::kBufferOverflow), "Buffer overflow");
    EXPECT_STREQ(GetResultString(GeomResult::kResourceExhausted), "Resource exhausted");
    EXPECT_STREQ(GetResultString(GeomResult::kIOError), "IO error");
    EXPECT_STREQ(GetResultString(GeomResult::kParseError), "Parse error");
    EXPECT_STREQ(GetResultString(GeomResult::kFormatError), "Format error");
    EXPECT_STREQ(GetResultString(GeomResult::kFileNotFound), "File not found");
    EXPECT_STREQ(GetResultString(GeomResult::kWriteFailed), "Write failed");
    EXPECT_STREQ(GetResultString(GeomResult::kReadFailed), "Read failed");
    EXPECT_STREQ(GetResultString(GeomResult::kTransformError), "Transform error");
    EXPECT_STREQ(GetResultString(GeomResult::kCRSNotFound), "CRS not found");
    EXPECT_STREQ(GetResultString(GeomResult::kCRSConversionError), "CRS conversion error");
    EXPECT_STREQ(GetResultString(GeomResult::kInvalidProjection), "Invalid projection");
    EXPECT_STREQ(GetResultString(GeomResult::kCoordinateOutOfRange), "Coordinate out of range");
}

TEST_F(CommonTest, IsEqual_EqualValues) {
    EXPECT_TRUE(IsEqual(1.0, 1.0));
    EXPECT_TRUE(IsEqual(0.0, 0.0));
    EXPECT_TRUE(IsEqual(-1.0, -1.0));
    EXPECT_TRUE(IsEqual(1e-10, 1e-10));
}

TEST_F(CommonTest, IsEqual_WithinTolerance) {
    EXPECT_TRUE(IsEqual(1.0, 1.0 + DEFAULT_TOLERANCE / 2));
    EXPECT_TRUE(IsEqual(1.0, 1.0 - DEFAULT_TOLERANCE / 2));
    EXPECT_TRUE(IsEqual(0.0, DEFAULT_TOLERANCE));
    EXPECT_TRUE(IsEqual(0.0, -DEFAULT_TOLERANCE));
}

TEST_F(CommonTest, IsEqual_OutsideTolerance) {
    EXPECT_FALSE(IsEqual(1.0, 1.0 + DEFAULT_TOLERANCE * 2));
    EXPECT_FALSE(IsEqual(1.0, 1.0 - DEFAULT_TOLERANCE * 2));
    EXPECT_FALSE(IsEqual(0.0, DEFAULT_TOLERANCE * 2));
}

TEST_F(CommonTest, IsEqual_CustomTolerance) {
    EXPECT_TRUE(IsEqual(1.0, 1.1, 0.2));
    EXPECT_FALSE(IsEqual(1.0, 1.1, 0.05));
    EXPECT_TRUE(IsEqual(100.0, 101.0, 2.0));
}

TEST_F(CommonTest, IsZero_ZeroValues) {
    EXPECT_TRUE(IsZero(0.0));
    EXPECT_TRUE(IsZero(DEFAULT_TOLERANCE));
    EXPECT_TRUE(IsZero(-DEFAULT_TOLERANCE));
    EXPECT_TRUE(IsZero(1e-15));
}

TEST_F(CommonTest, IsZero_NonZeroValues) {
    EXPECT_FALSE(IsZero(DEFAULT_TOLERANCE * 2));
    EXPECT_FALSE(IsZero(-DEFAULT_TOLERANCE * 2));
    EXPECT_FALSE(IsZero(1.0));
    EXPECT_FALSE(IsZero(-1.0));
}

TEST_F(CommonTest, IsZero_CustomTolerance) {
    EXPECT_TRUE(IsZero(0.1, 0.2));
    EXPECT_FALSE(IsZero(0.1, 0.05));
    EXPECT_TRUE(IsZero(-0.1, 0.2));
}

TEST_F(CommonTest, Constants_DefaultValues) {
    EXPECT_DOUBLE_EQ(DEFAULT_TOLERANCE, 1e-9);
    EXPECT_EQ(DEFAULT_WKT_PRECISION, 15);
}

TEST_F(CommonTest, GeomType_UnderlyingType) {
    static_assert(std::is_same<std::underlying_type<GeomType>::type, uint8_t>::value,
                  "GeomType should have uint8_t underlying type");
}

TEST_F(CommonTest, Dimension_UnderlyingType) {
    static_assert(std::is_same<std::underlying_type<Dimension>::type, uint8_t>::value,
                  "Dimension should have uint8_t underlying type");
}

TEST_F(CommonTest, GeomResult_UnderlyingType) {
    static_assert(std::is_same<std::underlying_type<GeomResult>::type, int32_t>::value,
                  "GeomResult should have int32_t underlying type");
}

}
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
