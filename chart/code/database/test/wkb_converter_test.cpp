#include <gtest/gtest.h>
#include "ogc/db/wkb_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/envelope.h"

using namespace ogc;
using namespace ogc::db;

class WkbConverterTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(WkbConverterTest, PointToWkb) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result result = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_GT(wkb.size(), 0);
}

TEST_F(WkbConverterTest, PointFromWkb) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result toResult = WkbConverter::GeometryToWkb(point.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
    auto* outPoint = static_cast<Point*>(geometry.get());
    EXPECT_DOUBLE_EQ(outPoint->GetX(), 1.0);
    EXPECT_DOUBLE_EQ(outPoint->GetY(), 2.0);
    EXPECT_EQ(geometry->GetSRID(), 4326);
}

TEST_F(WkbConverterTest, LineStringToWkb) {
    CoordinateList coords = {{0, 0}, {1, 1}, {2, 0}};
    auto line = LineString::Create(coords);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(line.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_GT(wkb.size(), 0);
}

TEST_F(WkbConverterTest, LineStringFromWkb) {
    CoordinateList coords = {{0, 0}, {1, 1}, {2, 0}};
    auto line = LineString::Create(coords);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(line.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kLineString);
    auto* outLine = static_cast<LineString*>(geometry.get());
    EXPECT_EQ(outLine->GetNumPoints(), 3);
}

TEST_F(WkbConverterTest, PolygonToWkb) {
    CoordinateList exteriorCoords = {{0, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(polygon.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_GT(wkb.size(), 0);
}

TEST_F(WkbConverterTest, PolygonFromWkb) {
    CoordinateList exteriorCoords = {{0, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result toResult = WkbConverter::GeometryToWkb(polygon.get(), wkb, options);
    ASSERT_TRUE(toResult.IsSuccess());
    
    std::unique_ptr<Geometry> geometry;
    Result fromResult = WkbConverter::WkbToGeometry(wkb, geometry, options);
    ASSERT_TRUE(fromResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPolygon);
}

TEST_F(WkbConverterTest, HexWkbConversion) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::string hexWkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result toHexResult = WkbConverter::GeometryToHexWkb(point.get(), hexWkb, options);
    ASSERT_TRUE(toHexResult.IsSuccess());
    EXPECT_GT(hexWkb.size(), 0);
    
    std::unique_ptr<Geometry> geometry;
    Result fromHexResult = WkbConverter::HexWkbToGeometry(hexWkb, geometry, options);
    ASSERT_TRUE(fromHexResult.IsSuccess());
    ASSERT_NE(geometry, nullptr);
    
    EXPECT_EQ(geometry->GetGeometryType(), GeomType::kPoint);
}

TEST_F(WkbConverterTest, NullGeometry) {
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(nullptr, wkb, options);
    EXPECT_FALSE(result.IsSuccess());
}

TEST_F(WkbConverterTest, EmptyGeometry) {
    auto line = LineString::Create();
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    
    Result result = WkbConverter::GeometryToWkb(line.get(), wkb, options);
    EXPECT_TRUE(result.IsSuccess());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
