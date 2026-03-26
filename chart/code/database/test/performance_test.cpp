#include <gtest/gtest.h>
#include "ogc/db/wkb_converter.h"
#include "ogc/db/geojson_converter.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <random>

using namespace ogc;
using namespace ogc::db;

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    
    template<typename Func>
    double MeasureTimeMs(Func&& func, int iterations = 1) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count() / iterations;
    }
};

TEST_F(PerformanceTest, WkbPointConversionSpeed) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    double avgTime = MeasureTimeMs([&]() {
        wkb.clear();
        WkbConverter::GeometryToWkb(point.get(), wkb, options);
    }, 1000);
    
    std::cout << "WKB Point Conversion: " << avgTime << " ms/op" << std::endl;
    EXPECT_LT(avgTime, 1.0);
}

TEST_F(PerformanceTest, WkbLineStringConversionSpeed) {
    CoordinateList coords;
    for (int i = 0; i < 1000; ++i) {
        coords.push_back({static_cast<double>(i), static_cast<double>(i * 2)});
    }
    auto line = LineString::Create(coords);
    line->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    double avgTime = MeasureTimeMs([&]() {
        wkb.clear();
        WkbConverter::GeometryToWkb(line.get(), wkb, options);
    }, 100);
    
    std::cout << "WKB LineString (1000 points) Conversion: " << avgTime << " ms/op" << std::endl;
    EXPECT_LT(avgTime, 10.0);
}

TEST_F(PerformanceTest, WkbPolygonConversionSpeed) {
    CoordinateList exteriorCoords;
    for (int i = 0; i < 100; ++i) {
        double angle = 2.0 * 3.14159 * i / 100;
        exteriorCoords.push_back({std::cos(angle), std::sin(angle)});
    }
    auto exterior = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exterior));
    polygon->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    double avgTime = MeasureTimeMs([&]() {
        wkb.clear();
        WkbConverter::GeometryToWkb(polygon.get(), wkb, options);
    }, 100);
    
    std::cout << "WKB Polygon (100 points) Conversion: " << avgTime << " ms/op" << std::endl;
    EXPECT_LT(avgTime, 5.0);
}

TEST_F(PerformanceTest, WkbRoundTripSpeed) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    double avgTime = MeasureTimeMs([&]() {
        wkb.clear();
        WkbConverter::GeometryToWkb(point.get(), wkb, options);
        
        std::unique_ptr<Geometry> geometry;
        WkbConverter::WkbToGeometry(wkb, geometry, options);
    }, 1000);
    
    std::cout << "WKB Round Trip: " << avgTime << " ms/op" << std::endl;
    EXPECT_LT(avgTime, 2.0);
}

TEST_F(PerformanceTest, GeoJsonPointConversionSpeed) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::string json;
    GeoJsonOptions options;
    options.prettyPrint = false;
    options.precision = 6;
    
    double avgTime = MeasureTimeMs([&]() {
        json.clear();
        GeoJsonConverter::GeometryToJson(point.get(), json, options);
    }, 1000);
    
    std::cout << "GeoJSON Point Conversion: " << avgTime << " ms/op" << std::endl;
    EXPECT_LT(avgTime, 1.0);
}

TEST_F(PerformanceTest, GeoJsonLineStringConversionSpeed) {
    CoordinateList coords;
    for (int i = 0; i < 1000; ++i) {
        coords.push_back({static_cast<double>(i), static_cast<double>(i * 2)});
    }
    auto line = LineString::Create(coords);
    line->SetSRID(4326);
    
    std::string json;
    GeoJsonOptions options;
    options.precision = 6;
    
    double avgTime = MeasureTimeMs([&]() {
        json.clear();
        GeoJsonConverter::GeometryToJson(line.get(), json, options);
    }, 100);
    
    std::cout << "GeoJSON LineString (1000 points) Conversion: " << avgTime << " ms/op" << std::endl;
    EXPECT_LT(avgTime, 10.0);
}

TEST_F(PerformanceTest, GeoJsonPrettyPrintOverhead) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::string jsonCompact, jsonPretty;
    GeoJsonOptions compactOptions, prettyOptions;
    compactOptions.prettyPrint = false;
    prettyOptions.prettyPrint = true;
    prettyOptions.indentSpaces = 2;
    
    double compactTime = MeasureTimeMs([&]() {
        jsonCompact.clear();
        GeoJsonConverter::GeometryToJson(point.get(), jsonCompact, compactOptions);
    }, 1000);
    
    double prettyTime = MeasureTimeMs([&]() {
        jsonPretty.clear();
        GeoJsonConverter::GeometryToJson(point.get(), jsonPretty, prettyOptions);
    }, 1000);
    
    std::cout << "GeoJSON Compact: " << compactTime << " ms/op" << std::endl;
    std::cout << "GeoJSON Pretty: " << prettyTime << " ms/op" << std::endl;
    std::cout << "Pretty Print Overhead: " << (prettyTime / compactTime) << "x" << std::endl;
    
    EXPECT_GT(jsonPretty.length(), jsonCompact.length());
}

TEST_F(PerformanceTest, LargePointBatchConversion) {
    const int batchSize = 10000;
    std::vector<std::unique_ptr<Point>> points;
    points.reserve(batchSize);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-180, 180);
    
    for (int i = 0; i < batchSize; ++i) {
        points.push_back(Point::Create(dis(gen), dis(gen)));
        points.back()->SetSRID(4326);
    }
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < batchSize; ++i) {
        wkb.clear();
        WkbConverter::GeometryToWkb(points[i].get(), wkb, options);
    }
    auto end = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double, std::milli>(end - start).count();
    
    std::cout << "Batch WKB Conversion (" << batchSize << " points): " << totalTime << " ms" << std::endl;
    std::cout << "Average: " << (totalTime / batchSize) << " ms/op" << std::endl;
    
    EXPECT_LT(totalTime, 5000.0);
}

TEST_F(PerformanceTest, MemoryAllocation) {
    auto point = Point::Create(1.0, 2.0);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    size_t beforeSize = 0;
    size_t afterSize = 0;
    
    WkbConverter::GeometryToWkb(point.get(), wkb, options);
    
    std::cout << "WKB Size for Point: " << wkb.size() << " bytes" << std::endl;
    
    EXPECT_GT(wkb.size(), 0);
    EXPECT_LT(wkb.size(), 100);
}

TEST_F(PerformanceTest, HexWkbConversionSpeed) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::string hexWkb;
    WkbOptions options;
    options.includeSRID = true;
    
    double toHexTime = MeasureTimeMs([&]() {
        hexWkb.clear();
        WkbConverter::GeometryToHexWkb(point.get(), hexWkb, options);
    }, 1000);
    
    std::cout << "WKB to Hex: " << toHexTime << " ms/op" << std::endl;
    
    double fromHexTime = MeasureTimeMs([&]() {
        std::unique_ptr<Geometry> geometry;
        WkbConverter::HexWkbToGeometry(hexWkb, geometry, options);
    }, 1000);
    
    std::cout << "Hex to WKB: " << fromHexTime << " ms/op" << std::endl;
    
    EXPECT_LT(toHexTime, 2.0);
    EXPECT_LT(fromHexTime, 2.0);
}
