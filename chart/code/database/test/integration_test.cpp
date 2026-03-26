#include <gtest/gtest.h>
#include "ogc/db/postgis_connection.h"
#include "ogc/db/sqlite_connection.h"
#include "ogc/db/wkb_converter.h"
#include "ogc/db/geojson_converter.h"
#include "ogc/db/connection_pool.h"
#include "ogc/db/async_connection.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include <iostream>
#include <memory>
#include <thread>

using namespace ogc;
using namespace ogc::db;

class IntegrationTestBase : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(IntegrationTestBase, PostGISConnection) {
    auto conn = PostGISConnection::Create();
    ASSERT_NE(conn, nullptr);
    EXPECT_FALSE(conn->IsConnected());
}

TEST_F(IntegrationTestBase, SpatiaLiteConnection) {
    auto conn = SpatiaLiteConnection::Create();
    ASSERT_NE(conn, nullptr);
    EXPECT_FALSE(conn->IsConnected());
}

TEST_F(IntegrationTestBase, WkbPointConversion) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::vector<uint8_t> pointWkb;
    WkbOptions options;
    options.includeSRID = true;
    
    ASSERT_TRUE(WkbConverter::GeometryToWkb(point.get(), pointWkb, options).IsSuccess());
    std::unique_ptr<Geometry> pointGeom;
    ASSERT_TRUE(WkbConverter::WkbToGeometry(pointWkb, pointGeom, options).IsSuccess());
}

TEST_F(IntegrationTestBase, WkbLineStringConversion) {
    CoordinateList coords = {{0, 0}, {1, 1}, {2, 0}};
    auto line = LineString::Create(coords);
    line->SetSRID(4326);
    
    std::vector<uint8_t> lineWkb;
    WkbOptions options;
    options.includeSRID = true;
    ASSERT_TRUE(WkbConverter::GeometryToWkb(line.get(), lineWkb, options).IsSuccess());
    
    std::unique_ptr<Geometry> lineGeom;
    ASSERT_TRUE(WkbConverter::WkbToGeometry(lineWkb, lineGeom, options).IsSuccess());
    ASSERT_EQ(lineGeom->GetGeometryType(), GeomType::kLineString);
}

TEST_F(IntegrationTestBase, ConnectionPoolCreation) {
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
}

TEST_F(IntegrationTestBase, GeometryToWkbToGeoJsonWorkflow) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::vector<uint8_t> wkb;
    WkbOptions wkbOptions;
    wkbOptions.includeSRID = true;
    
    Result toWkbResult = WkbConverter::GeometryToWkb(point.get(), wkb, wkbOptions);
    ASSERT_TRUE(toWkbResult.IsSuccess()) << "Geometry to WKB conversion should succeed";
    
    std::unique_ptr<Geometry> geomFromWkb;
    Result fromWkbResult = WkbConverter::WkbToGeometry(wkb, geomFromWkb, wkbOptions);
    ASSERT_TRUE(fromWkbResult.IsSuccess()) << "WKB to Geometry conversion should succeed";
    
    std::string json;
    GeoJsonOptions jsonOptions;
    jsonOptions.prettyPrint = false;
    
    Result toJsonResult = GeoJsonConverter::GeometryToJson(geomFromWkb.get(), json, jsonOptions);
    ASSERT_TRUE(toJsonResult.IsSuccess()) << "Geometry to GeoJSON conversion should succeed";
    EXPECT_NE(json.find("Point"), std::string::npos) << "JSON should contain Point type";
}

TEST_F(IntegrationTestBase, GeoJsonToWkbToGeometryWorkflow) {
    std::string json = "{\"type\":\"Point\",\"coordinates\":[116.4074,39.9042]}";
    
    std::unique_ptr<Geometry> geomFromJson;
    Result fromJsonResult = GeoJsonConverter::GeometryFromJson(json, geomFromJson);
    ASSERT_TRUE(fromJsonResult.IsSuccess()) << "GeoJSON to Geometry conversion should succeed";
    
    std::vector<uint8_t> wkb;
    WkbOptions wkbOptions;
    wkbOptions.includeSRID = true;
    
    Result toWkbResult = WkbConverter::GeometryToWkb(geomFromJson.get(), wkb, wkbOptions);
    ASSERT_TRUE(toWkbResult.IsSuccess()) << "Geometry to WKB conversion should succeed";
    
    std::unique_ptr<Geometry> geomFromWkb;
    Result fromWkbResult = WkbConverter::WkbToGeometry(wkb, geomFromWkb, wkbOptions);
    ASSERT_TRUE(fromWkbResult.IsSuccess()) << "WKB to Geometry conversion should succeed";
    
    EXPECT_EQ(geomFromWkb->GetGeometryType(), GeomType::kPoint);
}

TEST_F(IntegrationTestBase, MultipleGeometryTypesConversion) {
    std::vector<std::unique_ptr<Geometry>> geometries;
    
    geometries.push_back(Point::Create(1.0, 2.0));
    geometries.push_back(LineString::Create({{0, 0}, {1, 1}}));
    
    CoordinateList polyCoords = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}};
    auto ring = LinearRing::Create(polyCoords, true);
    geometries.push_back(Polygon::Create(std::move(ring)));
    
    auto mp = MultiPoint::Create();
    mp->AddPoint(Point::Create(1.0, 2.0));
    geometries.push_back(std::move(mp));
    
    for (const auto& geom : geometries) {
        std::vector<uint8_t> wkb;
        WkbOptions options;
        
        Result toWkb = WkbConverter::GeometryToWkb(geom.get(), wkb, options);
        ASSERT_TRUE(toWkb.IsSuccess()) << "WKB conversion should succeed for all types";
        
        std::string json;
        Result toJson = GeoJsonConverter::GeometryToJson(geom.get(), json);
        ASSERT_TRUE(toJson.IsSuccess()) << "GeoJSON conversion should succeed for all types";
    }
}

TEST_F(IntegrationTestBase, AsyncExecutorWithConverterWorkflow) {
    auto executor = DbAsyncExecutor::Create(2);
    ASSERT_NE(executor, nullptr);
    
    auto future = executor->ExecuteAsync([]() {
        auto point = Point::Create(116.4074, 39.9042);
        std::vector<uint8_t> wkb;
        WkbOptions options;
        
        Result result = WkbConverter::GeometryToWkb(point.get(), wkb, options);
        return result;
    });
    
    Result result = future.get();
    EXPECT_TRUE(result.IsSuccess()) << "Async conversion should succeed";
    
    executor->Shutdown();
}

TEST_F(IntegrationTestBase, ConnectionPoolWithAsyncExecutorWorkflow) {
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
    
    auto executor = DbAsyncExecutor::Create(4);
    ASSERT_NE(executor, nullptr);
    
    std::vector<std::future<Result>> futures;
    for (int i = 0; i < 5; ++i) {
        futures.push_back(executor->ExecuteAsync([i]() {
            auto point = Point::Create(i * 10.0, i * 20.0);
            std::vector<uint8_t> wkb;
            WkbOptions options;
            return WkbConverter::GeometryToWkb(point.get(), wkb, options);
        }));
    }
    
    for (auto& f : futures) {
        EXPECT_TRUE(f.get().IsSuccess()) << "All async conversions should succeed";
    }
    
    executor->Shutdown();
}

TEST_F(IntegrationTestBase, GeometryCollectionIntegration) {
    auto gc = GeometryCollection::Create();
    gc->AddGeometry(Point::Create(1.0, 2.0));
    gc->AddGeometry(LineString::Create({{0, 0}, {1, 1}}));
    
    CoordinateList polyCoords = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}};
    auto ring = LinearRing::Create(polyCoords, true);
    gc->AddGeometry(Polygon::Create(std::move(ring)));
    
    std::vector<uint8_t> wkb;
    WkbOptions wkbOptions;
    Result toWkb = WkbConverter::GeometryToWkb(gc.get(), wkb, wkbOptions);
    ASSERT_TRUE(toWkb.IsSuccess()) << "GeometryCollection WKB conversion should succeed";
    
    std::string json;
    Result toJson = GeoJsonConverter::GeometryToJson(gc.get(), json);
    ASSERT_TRUE(toJson.IsSuccess()) << "GeometryCollection GeoJSON conversion should succeed";
    EXPECT_NE(json.find("GeometryCollection"), std::string::npos);
}

TEST_F(IntegrationTestBase, RoundTripConsistency) {
    auto originalPoint = Point::Create(116.4074, 39.9042);
    originalPoint->SetSRID(4326);
    
    std::vector<uint8_t> wkb1;
    WkbOptions options;
    options.includeSRID = true;
    
    ASSERT_TRUE(WkbConverter::GeometryToWkb(originalPoint.get(), wkb1, options).IsSuccess());
    
    std::unique_ptr<Geometry> geomFromWkb;
    ASSERT_TRUE(WkbConverter::WkbToGeometry(wkb1, geomFromWkb, options).IsSuccess());
    
    std::vector<uint8_t> wkb2;
    ASSERT_TRUE(WkbConverter::GeometryToWkb(geomFromWkb.get(), wkb2, options).IsSuccess());
    
    EXPECT_EQ(wkb1, wkb2) << "Round trip WKB should be identical";
}

TEST_F(IntegrationTestBase, ConcurrentConversionOperations) {
    auto executor = DbAsyncExecutor::Create(4);
    
    std::atomic<int> successCount{0};
    std::atomic<int> failCount{0};
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&successCount, &failCount]() {
            for (int j = 0; j < 10; ++j) {
                auto point = Point::Create(j * 1.0, j * 2.0);
                std::vector<uint8_t> wkb;
                WkbOptions options;
                
                if (WkbConverter::GeometryToWkb(point.get(), wkb, options).IsSuccess()) {
                    successCount++;
                } else {
                    failCount++;
                }
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(successCount, 100) << "All concurrent conversions should succeed";
    EXPECT_EQ(failCount, 0) << "No conversions should fail";
    
    executor->Shutdown();
}

TEST_F(IntegrationTestBase, ErrorHandlingAcrossModules) {
    Result result = WkbConverter::GeometryToWkb(nullptr, std::vector<uint8_t>(), WkbOptions());
    EXPECT_FALSE(result.IsSuccess()) << "Null geometry should fail";
    
    std::unique_ptr<Geometry> geom;
    result = GeoJsonConverter::GeometryFromJson("invalid json", geom);
    EXPECT_FALSE(result.IsSuccess()) << "Invalid JSON should fail";
    
    auto pool = DbConnectionPool::Create();
    result = pool->Release(nullptr);
    EXPECT_FALSE(result.IsSuccess()) << "Releasing null connection should fail";
}
