#include "gtest/gtest.h"
#include "ogc/feature/wkb_wkt_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/linearring.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"

using namespace ogc;

class WkbWktConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("test");
    }
    void TearDown() override {
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
};

TEST_F(WkbWktConverterTest, PointToWKT) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(100.0, 200.0));
    feature.GetGeometry()->SetSRID(4326);
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&feature);
    
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(WkbWktConverterTest, LineStringToWKT) {
    CNFeature feature(defn_);
    feature.SetGeometry(LineString::Create({{0, 0}, {1, 1}, {2, 2}}));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&feature);
    
    EXPECT_NE(wkt.find("LINESTRING"), std::string::npos);
}

TEST_F(WkbWktConverterTest, PolygonToWKT) {
    auto ring = LinearRing::Create({{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}, true);
    auto polygon = Polygon::Create(std::move(ring));
    
    CNFeature feature(defn_);
    feature.SetGeometry(std::move(polygon));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&feature);
    
    EXPECT_NE(wkt.find("POLYGON"), std::string::npos);
}

TEST_F(WkbWktConverterTest, PointToWKB) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(123.456, -78.901));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&feature);
    
    EXPECT_FALSE(wkb.empty());
}

TEST_F(WkbWktConverterTest, LineStringToWKB) {
    CNFeature feature(defn_);
    feature.SetGeometry(LineString::Create({{0, 0}, {1, 1}}));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&feature);
    
    EXPECT_FALSE(wkb.empty());
}

TEST_F(WkbWktConverterTest, PolygonToWKB) {
    auto ring = LinearRing::Create({{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}}, true);
    auto polygon = Polygon::Create(std::move(ring));
    
    CNFeature feature(defn_);
    feature.SetGeometry(std::move(polygon));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&feature);
    
    EXPECT_FALSE(wkb.empty());
}

TEST_F(WkbWktConverterTest, WKBToPoint) {
    CNFeature original(defn_);
    original.SetGeometry(Point::Create(100.0, 200.0));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&original);
    
    CNFeature* parsed = converter.FromWKB(wkb);
    ASSERT_NE(parsed, nullptr);
    EXPECT_NE(parsed->GetGeometry(), nullptr);
    
    delete parsed;
}

TEST_F(WkbWktConverterTest, WKBToLineString) {
    CNFeature original(defn_);
    original.SetGeometry(LineString::Create({{0, 0}, {1, 1}, {2, 2}}));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&original);
    
    CNFeature* parsed = converter.FromWKB(wkb);
    ASSERT_NE(parsed, nullptr);
    
    delete parsed;
}

TEST_F(WkbWktConverterTest, WKBToPolygon) {
    auto ring = LinearRing::Create({{0, 0}, {0, 10}, {10, 10}, {10, 0}, {0, 0}}, true);
    auto polygon = Polygon::Create(std::move(ring));
    
    CNFeature original(defn_);
    original.SetGeometry(std::move(polygon));
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(&original);
    
    CNFeature* parsed = converter.FromWKB(wkb);
    ASSERT_NE(parsed, nullptr);
    
    delete parsed;
}

TEST_F(WkbWktConverterTest, RoundTripPoint) {
    CNFeature original(defn_);
    original.SetGeometry(Point::Create(123.456, -78.901));
    original.GetGeometry()->SetSRID(4326);
    
    CNWkbWktConverter converter;
    converter.SetIncludeSRID(true);
    
    std::vector<uint8_t> wkb = converter.ToWKB(&original);
    CNFeature* parsed = converter.FromWKB(wkb);
    
    ASSERT_NE(parsed, nullptr);
    EXPECT_NE(parsed->GetGeometry(), nullptr);
    
    delete parsed;
}

TEST_F(WkbWktConverterTest, RoundTripWKT) {
    CNFeature original(defn_);
    original.SetGeometry(Point::Create(50.0, 60.0));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&original);
    
    EXPECT_FALSE(wkt.empty());
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(WkbWktConverterTest, ByteOrder) {
    CNWkbWktConverter converter;
    
    converter.SetByteOrder(ByteOrder::kXDR);
    EXPECT_EQ(converter.GetByteOrder(), ByteOrder::kXDR);
    
    converter.SetByteOrder(ByteOrder::kNDR);
    EXPECT_EQ(converter.GetByteOrder(), ByteOrder::kNDR);
}

TEST_F(WkbWktConverterTest, IncludeSRID) {
    CNWkbWktConverter converter;
    
    converter.SetIncludeSRID(true);
    EXPECT_TRUE(converter.GetIncludeSRID());
    
    converter.SetIncludeSRID(false);
    EXPECT_FALSE(converter.GetIncludeSRID());
}

TEST_F(WkbWktConverterTest, LargeCoordinates) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(1e10, -1e10));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&feature);
    
    EXPECT_FALSE(wkt.empty());
}

TEST_F(WkbWktConverterTest, SmallCoordinates) {
    CNFeature feature(defn_);
    feature.SetGeometry(Point::Create(1e-10, -1e-10));
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(&feature);
    
    EXPECT_FALSE(wkt.empty());
}

TEST_F(WkbWktConverterTest, GeometryToWKB) {
    auto point = Point::Create(100.0, 200.0);
    
    CNWkbWktConverter converter;
    std::vector<uint8_t> wkb = converter.ToWKB(std::move(point));
    
    EXPECT_FALSE(wkb.empty());
}
