#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <memory>
#include <fstream>
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/linearring.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include "ogc/draw/draw_facade.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/coordinate_transform.h"

using namespace ogc;
using namespace ogc::draw;

class PerformanceBenchmarkTest : public ::testing::Test {
protected:
    void SetUp() override {
        DrawFacade::Instance().Initialize();
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
    
    std::string FormatDuration(std::chrono::microseconds us) {
        return std::to_string(us.count() / 1000.0) + " ms";
    }
};

TEST_F(PerformanceBenchmarkTest, PointCreationBenchmark) {
    const int COUNT = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<ogc::PointPtr> points;
    points.reserve(COUNT);
    for (int i = 0; i < COUNT; ++i) {
        points.push_back(ogc::Point::Create(Coordinate(i * 0.001, i * 0.001)));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(points.size(), COUNT);
    
    double avgTime = duration.count() / static_cast<double>(COUNT);
    EXPECT_LT(avgTime, 10.0);
    
    std::ofstream log("benchmark_point_creation.log", std::ios::app);
    log << "Point Creation Benchmark: " << COUNT << " points in " 
        << FormatDuration(duration) << " (avg: " << avgTime << " us/point)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, LineStringCreationBenchmark) {
    const int LINE_COUNT = 10000;
    const int POINTS_PER_LINE = 100;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<LineStringPtr> lines;
    lines.reserve(LINE_COUNT);
    for (int i = 0; i < LINE_COUNT; ++i) {
        std::vector<Coordinate> coords;
        coords.reserve(POINTS_PER_LINE);
        for (int j = 0; j < POINTS_PER_LINE; ++j) {
            coords.push_back(Coordinate(i * 0.01 + j * 0.001, j * 0.001));
        }
        lines.push_back(LineString::Create(coords));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(lines.size(), LINE_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(LINE_COUNT);
    EXPECT_LT(avgTime, 1000.0);
    
    std::ofstream log("benchmark_linestring_creation.log", std::ios::app);
    log << "LineString Creation Benchmark: " << LINE_COUNT << " lines (" << POINTS_PER_LINE 
        << " points each) in " << FormatDuration(duration) << " (avg: " << avgTime 
        << " us/linestring)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, PolygonCreationBenchmark) {
    const int POLYGON_COUNT = 5000;
    const int VERTICES_PER_POLYGON = 100;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<PolygonPtr> polygons;
    polygons.reserve(POLYGON_COUNT);
    for (int i = 0; i < POLYGON_COUNT; ++i) {
        std::vector<Coordinate> ring;
        ring.reserve(VERTICES_PER_POLYGON + 1);
        for (int j = 0; j < VERTICES_PER_POLYGON; ++j) {
            double angle = j * 2 * 3.14159265359 / VERTICES_PER_POLYGON;
            ring.push_back(Coordinate(i * 0.01 + cos(angle) * 0.005, 
                                      i * 0.01 + sin(angle) * 0.005));
        }
        ring.push_back(ring[0]);
        
        auto linearRing = LinearRing::Create(ring);
        polygons.push_back(Polygon::Create(std::move(linearRing)));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(polygons.size(), POLYGON_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(POLYGON_COUNT);
    EXPECT_LT(avgTime, 2000.0);
    
    std::ofstream log("benchmark_polygon_creation.log", std::ios::app);
    log << "Polygon Creation Benchmark: " << POLYGON_COUNT << " polygons (" 
        << VERTICES_PER_POLYGON << " vertices each) in " << FormatDuration(duration) 
        << " (avg: " << avgTime << " us/polygon)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, EnvelopeCalculationBenchmark) {
    const int GEOMETRY_COUNT = 10000;
    const int POINTS_PER_GEOMETRY = 100;
    
    std::vector<LineStringPtr> lines;
    lines.reserve(GEOMETRY_COUNT);
    for (int i = 0; i < GEOMETRY_COUNT; ++i) {
        std::vector<Coordinate> coords;
        coords.reserve(POINTS_PER_GEOMETRY);
        for (int j = 0; j < POINTS_PER_GEOMETRY; ++j) {
            coords.push_back(Coordinate(i * 0.01 + j * 0.001, j * 0.001));
        }
        lines.push_back(LineString::Create(coords));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<Envelope> envelopes;
    envelopes.reserve(GEOMETRY_COUNT);
    for (const auto& line : lines) {
        envelopes.push_back(line->GetEnvelope());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(envelopes.size(), GEOMETRY_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(GEOMETRY_COUNT);
    EXPECT_LT(avgTime, 100.0);
    
    std::ofstream log("benchmark_envelope_calculation.log", std::ios::app);
    log << "Envelope Calculation Benchmark: " << GEOMETRY_COUNT << " geometries in " 
        << FormatDuration(duration) << " (avg: " << avgTime << " us/geometry)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, CoordinateTransformBenchmark) {
    const int TRANSFORM_COUNT = 100000;
    
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    std::vector<Coordinate> coords;
    coords.reserve(TRANSFORM_COUNT);
    for (int i = 0; i < TRANSFORM_COUNT; ++i) {
        coords.push_back(Coordinate(116.0 + i * 0.0001, 39.0 + i * 0.0001));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<Coordinate> transformed;
    transformed.reserve(TRANSFORM_COUNT);
    for (const auto& coord : coords) {
        transformed.push_back(transform->Transform(coord));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(transformed.size(), TRANSFORM_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(TRANSFORM_COUNT);
    EXPECT_LT(avgTime, 5.0);
    
    std::ofstream log("benchmark_coordinate_transform.log", std::ios::app);
    log << "Coordinate Transform Benchmark: " << TRANSFORM_COUNT << " transforms in " 
        << FormatDuration(duration) << " (avg: " << avgTime << " us/transform)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, GeometryCloneBenchmark) {
    const int CLONE_COUNT = 10000;
    
    std::vector<Coordinate> coords;
    coords.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        coords.push_back(Coordinate(i * 0.001, i * 0.001));
    }
    auto original = LineString::Create(coords);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<GeometryPtr> clones;
    clones.reserve(CLONE_COUNT);
    for (int i = 0; i < CLONE_COUNT; ++i) {
        clones.push_back(original->Clone());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(clones.size(), CLONE_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(CLONE_COUNT);
    EXPECT_LT(avgTime, 500.0);
    
    std::ofstream log("benchmark_geometry_clone.log", std::ios::app);
    log << "Geometry Clone Benchmark: " << CLONE_COUNT << " clones (1000-point LineString) in " 
        << FormatDuration(duration) << " (avg: " << avgTime << " us/clone)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, WKTSerializationBenchmark) {
    const int SERIALIZE_COUNT = 5000;
    
    std::vector<PolygonPtr> polygons;
    polygons.reserve(SERIALIZE_COUNT);
    for (int i = 0; i < SERIALIZE_COUNT; ++i) {
        std::vector<Coordinate> ring;
        ring.reserve(50);
        for (int j = 0; j < 50; ++j) {
            double angle = j * 2 * 3.14159265359 / 50;
            ring.push_back(Coordinate(116.0 + i * 0.001 + cos(angle) * 0.0005, 
                                      39.0 + i * 0.001 + sin(angle) * 0.0005));
        }
        ring.push_back(ring[0]);
        auto linearRing = LinearRing::Create(ring);
        polygons.push_back(Polygon::Create(std::move(linearRing)));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::string> wktStrings;
    wktStrings.reserve(SERIALIZE_COUNT);
    for (const auto& polygon : polygons) {
        wktStrings.push_back(polygon->AsText());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(wktStrings.size(), SERIALIZE_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(SERIALIZE_COUNT);
    EXPECT_LT(avgTime, 1000.0);
    
    std::ofstream log("benchmark_wkt_serialization.log", std::ios::app);
    log << "WKT Serialization Benchmark: " << SERIALIZE_COUNT << " polygons (50 vertices) in " 
        << FormatDuration(duration) << " (avg: " << avgTime << " us/serialize)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, MultiPointCreationBenchmark) {
    const int MULTI_COUNT = 1000;
    const int POINTS_PER_MULTI = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<MultiPointPtr> multiPoints;
    multiPoints.reserve(MULTI_COUNT);
    for (int i = 0; i < MULTI_COUNT; ++i) {
        std::vector<Coordinate> coords;
        coords.reserve(POINTS_PER_MULTI);
        for (int j = 0; j < POINTS_PER_MULTI; ++j) {
            coords.push_back(Coordinate(i * 0.01 + j * 0.001, j * 0.001));
        }
        multiPoints.push_back(MultiPoint::Create(coords));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_EQ(multiPoints.size(), MULTI_COUNT);
    
    double avgTime = duration.count() / static_cast<double>(MULTI_COUNT);
    EXPECT_LT(avgTime, 5000.0);
    
    std::ofstream log("benchmark_multipoint_creation.log", std::ios::app);
    log << "MultiPoint Creation Benchmark: " << MULTI_COUNT << " MultiPoints (" 
        << POINTS_PER_MULTI << " points each) in " << FormatDuration(duration) 
        << " (avg: " << avgTime << " us/multipoint)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, DeviceCreationBenchmark) {
    const int DEVICE_COUNT = 100;
    
    auto& facade = DrawFacade::Instance();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < DEVICE_COUNT; ++i) {
        auto device = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
        EXPECT_NE(device, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avgTime = duration.count() / static_cast<double>(DEVICE_COUNT);
    EXPECT_LT(avgTime, 5000.0);
    
    std::ofstream log("benchmark_device_creation.log", std::ios::app);
    log << "Device Creation Benchmark: " << DEVICE_COUNT << " devices in " 
        << FormatDuration(duration) << " (avg: " << avgTime << " us/device)" << std::endl;
}

TEST_F(PerformanceBenchmarkTest, MemoryUsageBenchmark) {
    const int LARGE_COUNT = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<ogc::PointPtr> points;
    points.reserve(LARGE_COUNT);
    for (int i = 0; i < LARGE_COUNT; ++i) {
        points.push_back(ogc::Point::Create(Coordinate(i * 0.0001, i * 0.0001)));
    }
    
    auto mid = std::chrono::high_resolution_clock::now();
    
    points.clear();
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto creationDuration = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
    auto destructionDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - mid);
    
    double creationAvg = creationDuration.count() / static_cast<double>(LARGE_COUNT);
    double destructionAvg = destructionDuration.count() / static_cast<double>(LARGE_COUNT);
    
    EXPECT_LT(creationAvg, 10.0);
    EXPECT_LT(destructionAvg, 5.0);
    
    std::ofstream log("benchmark_memory_usage.log", std::ios::app);
    log << "Memory Usage Benchmark: " << LARGE_COUNT << " points" << std::endl;
    log << "  Creation: " << FormatDuration(creationDuration) << " (avg: " << creationAvg << " us/point)" << std::endl;
    log << "  Destruction: " << FormatDuration(destructionDuration) << " (avg: " << destructionAvg << " us/point)" << std::endl;
}
