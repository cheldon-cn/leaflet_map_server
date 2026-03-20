#include <gtest/gtest.h>
#include "ogc/db/postgis_connection.h"
#include "ogc/db/sqlite_connection.h"
#include "ogc/db/wkb_converter.h"
#include "ogc/db/geojson_converter.h"
#include "ogc/db/connection_pool.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <iostream>
#include <memory>

using namespace ogc;
using namespace ogc::db;

TEST(DatabaseConnectionTest, PostGISConnection) {
    auto conn = PostGISConnection::Create();
    ASSERT_NE(conn, nullptr);
    ASSERT_FALSE(conn->IsConnected());
}

TEST(DatabaseConnectionTest, SpatiaLiteConnection) {
    auto conn = SpatiaLiteConnection::Create();
    ASSERT_NE(conn, nullptr);
    ASSERT_FALSE(conn->IsConnected());
}

TEST(WkbConverterTest, PointConversion) {
    auto point = Point::Create(116.4074, 39.9042);
    point->SetSRID(4326);
    
    std::vector<uint8_t> pointWkb;
    WkbOptions options;
    options.includeSRID = true;
    
    ASSERT_TRUE(WkbConverter::GeometryToWkb(point.get(), pointWkb, options).IsSuccess());
    std::unique_ptr<Geometry> pointGeom;
    ASSERT_TRUE(WkbConverter::WkbToGeometry(pointWkb, pointGeom, options).IsSuccess());
}

TEST(WkbConverterTest, LineStringConversion) {
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

TEST(ConnectionPoolTest, CreatePool) {
    auto pool = DbConnectionPool::Create();
    ASSERT_NE(pool, nullptr);
}
