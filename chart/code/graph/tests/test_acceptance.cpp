#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <chrono>
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

class AcceptanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        DrawFacade::Instance().Initialize();
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
};

TEST_F(AcceptanceTest, GeometryModule_PointOperations) {
    auto point = ogc::Point::Create(Coordinate(116.397428, 39.90923));
    ASSERT_NE(point, nullptr);
    EXPECT_TRUE(point->IsValid());
    
    Envelope env = point->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 116.397428);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 39.90923);
    
    std::string wkt = point->AsText();
    EXPECT_EQ(wkt.substr(0, 5), "POINT");
    
    auto clone = point->Clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_TRUE(clone->Equals(point.get()));
}

TEST_F(AcceptanceTest, GeometryModule_LineStringOperations) {
    std::vector<Coordinate> coords = {
        Coordinate(116.0, 39.0),
        Coordinate(117.0, 39.0),
        Coordinate(117.0, 40.0),
        Coordinate(116.0, 40.0)
    };
    
    auto lineString = LineString::Create(coords);
    ASSERT_NE(lineString, nullptr);
    EXPECT_TRUE(lineString->IsValid());
    EXPECT_EQ(lineString->GetNumPoints(), 4u);
    
    Envelope env = lineString->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 116.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 117.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 39.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 40.0);
    
    std::string wkt = lineString->AsText();
    EXPECT_EQ(wkt.substr(0, 10), "LINESTRING");
}

TEST_F(AcceptanceTest, GeometryModule_PolygonOperations) {
    std::vector<Coordinate> outerRing = {
        Coordinate(0, 0),
        Coordinate(10, 0),
        Coordinate(10, 10),
        Coordinate(0, 10),
        Coordinate(0, 0)
    };
    
    auto linearRing = LinearRing::Create(outerRing);
    auto polygon = Polygon::Create(std::move(linearRing));
    ASSERT_NE(polygon, nullptr);
    EXPECT_TRUE(polygon->IsValid());
    
    Envelope env = polygon->GetEnvelope();
    EXPECT_DOUBLE_EQ(env.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(env.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(env.GetMaxY(), 10.0);
    
    std::string wkt = polygon->AsText();
    EXPECT_EQ(wkt.substr(0, 7), "POLYGON");
}

TEST_F(AcceptanceTest, GeometryModule_MultiGeometryOperations) {
    std::vector<Coordinate> coords = {
        Coordinate(0, 0),
        Coordinate(1, 1),
        Coordinate(2, 2)
    };
    auto multiPoint = MultiPoint::Create(coords);
    ASSERT_NE(multiPoint, nullptr);
    EXPECT_TRUE(multiPoint->IsValid());
    EXPECT_EQ(multiPoint->GetNumGeometries(), 3u);
    
    std::vector<LineStringPtr> lines;
    std::vector<Coordinate> line1Coords = {Coordinate(0, 0), Coordinate(1, 1)};
    std::vector<Coordinate> line2Coords = {Coordinate(2, 2), Coordinate(3, 3)};
    lines.push_back(std::move(LineString::Create(line1Coords)));
    lines.push_back(std::move(LineString::Create(line2Coords)));
    
    auto multiLineString = MultiLineString::Create(std::move(lines));
    ASSERT_NE(multiLineString, nullptr);
    EXPECT_TRUE(multiLineString->IsValid());
    EXPECT_EQ(multiLineString->GetNumGeometries(), 2u);
}

TEST_F(AcceptanceTest, RenderingModule_DeviceCreation) {
    auto& facade = DrawFacade::Instance();
    
    auto rasterDevice1 = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
    ASSERT_NE(rasterDevice1, nullptr);
    
    auto rasterDevice2 = facade.CreateDevice(DeviceType::kRasterImage, 1024, 768);
    ASSERT_NE(rasterDevice2, nullptr);
}

TEST_F(AcceptanceTest, RenderingModule_StyleCreation) {
    auto& facade = DrawFacade::Instance();
    
    auto strokeStyle = facade.CreateStrokeStyle(0xFF0000FF, 2.0);
    EXPECT_EQ(strokeStyle.pen.color.GetRGBA(), 0xFF0000FF);
    EXPECT_DOUBLE_EQ(strokeStyle.pen.width, 2.0);
    
    auto strokeFillStyle = facade.CreateStrokeFillStyle(0xFF0000FF, 1.0, 0x00FF0080);
    EXPECT_EQ(strokeFillStyle.pen.color.GetRGBA(), 0xFF0000FF);
    EXPECT_EQ(strokeFillStyle.brush.color.GetRGBA(), 0x00FF0080);
}

TEST_F(AcceptanceTest, RenderingModule_DrawParamsCreation) {
    auto& facade = DrawFacade::Instance();
    
    Envelope extent(116.0, 39.0, 117.0, 40.0);
    auto params = facade.CreateDrawParams(extent, 800, 600, 96.0);
    
    EXPECT_GT(params.pixel_width, 0);
    EXPECT_GT(params.pixel_height, 0);
}

TEST_F(AcceptanceTest, CoordinateTransform_Module) {
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    Coordinate wgs84Coord(116.397428, 39.90923);
    Coordinate transformed = transform->Transform(wgs84Coord);
    
    auto point = ogc::Point::Create(wgs84Coord);
    auto transformedGeom = transform->Transform(point.get());
    ASSERT_NE(transformedGeom, nullptr);
    
    ogc::Point* transformedPoint = dynamic_cast<ogc::Point*>(transformedGeom.get());
    ASSERT_NE(transformedPoint, nullptr);
    EXPECT_TRUE(transformedPoint->IsValid());
}

TEST_F(AcceptanceTest, Envelope_Operations) {
    Envelope env1(0, 0, 10, 10);
    Envelope env2(5, 5, 15, 15);
    
    EXPECT_TRUE(env1.Intersects(env2));
    EXPECT_TRUE(env2.Intersects(env1));
    
    Envelope intersection = env1.Intersection(env2);
    EXPECT_DOUBLE_EQ(intersection.GetMinX(), 5.0);
    EXPECT_DOUBLE_EQ(intersection.GetMinY(), 5.0);
    EXPECT_DOUBLE_EQ(intersection.GetMaxX(), 10.0);
    EXPECT_DOUBLE_EQ(intersection.GetMaxY(), 10.0);
    
    EXPECT_TRUE(env1.Contains(Coordinate(5, 5)));
    EXPECT_FALSE(env1.Contains(Coordinate(15, 15)));
    
    Envelope expanded = env1;
    expanded.ExpandToInclude(env2);
    EXPECT_DOUBLE_EQ(expanded.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(expanded.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(expanded.GetMaxX(), 15.0);
    EXPECT_DOUBLE_EQ(expanded.GetMaxY(), 15.0);
}

TEST_F(AcceptanceTest, EndToEnd_PipelineTest) {
    std::vector<Coordinate> ring = {
        Coordinate(116.0, 39.0),
        Coordinate(117.0, 39.0),
        Coordinate(117.0, 40.0),
        Coordinate(116.0, 40.0),
        Coordinate(116.0, 39.0)
    };
    
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    std::vector<Coordinate> transformedRing;
    for (const auto& coord : ring) {
        transformedRing.push_back(transform->Transform(coord));
    }
    
    auto linearRing = LinearRing::Create(transformedRing);
    auto polygon = Polygon::Create(std::move(linearRing));
    ASSERT_NE(polygon, nullptr);
    EXPECT_TRUE(polygon->IsValid());
    
    auto& facade = DrawFacade::Instance();
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 1200, 900);
    ASSERT_NE(device, nullptr);
    
    Envelope extent = polygon->GetEnvelope();
    auto params = facade.CreateDrawParams(extent, 1200, 900, 96.0);
    
    auto style = facade.CreateStrokeFillStyle(0xFF0000FF, 1.0, 0x00FF0080);
    EXPECT_EQ(style.pen.color.GetRGBA(), 0xFF0000FF);
    EXPECT_EQ(style.brush.color.GetRGBA(), 0x00FF0080);
}

TEST_F(AcceptanceTest, Performance_Requirements) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<ogc::PointPtr> points;
    points.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        points.push_back(ogc::Point::Create(Coordinate(i * 0.001, i * 0.001)));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(points.size(), 10000u);
    EXPECT_LT(duration.count(), 500);
}

TEST_F(AcceptanceTest, DataIntegrity_GeometryValidation) {
    auto validPoint = ogc::Point::Create(Coordinate(116.0, 39.0));
    EXPECT_TRUE(validPoint->IsValid());
    
    std::vector<Coordinate> validCoords = {
        Coordinate(0, 0),
        Coordinate(1, 1),
        Coordinate(2, 2)
    };
    auto validLine = LineString::Create(validCoords);
    EXPECT_TRUE(validLine->IsValid());
    
    auto emptyPoint = ogc::Point::Create(Coordinate::Empty());
    EXPECT_TRUE(emptyPoint->IsEmpty());
    
    std::vector<Coordinate> emptyCoords;
    auto emptyLine = LineString::Create(emptyCoords);
    EXPECT_TRUE(emptyLine->IsEmpty());
}

TEST_F(AcceptanceTest, SystemIntegration_AllModules) {
    auto& facade = DrawFacade::Instance();
    auto transform = CoordinateTransform::Create();
    
    std::vector<GeometryPtr> geometries;
    
    for (int i = 0; i < 10; ++i) {
        auto point = ogc::Point::Create(Coordinate(116.0 + i * 0.1, 39.0 + i * 0.1));
        auto transformed = transform->Transform(point.get());
        geometries.push_back(std::move(transformed));
    }
    
    for (int i = 0; i < 5; ++i) {
        std::vector<Coordinate> coords;
        coords.push_back(Coordinate(116.0 + i * 0.1, 39.0));
        coords.push_back(Coordinate(116.0 + i * 0.1 + 0.5, 39.5));
        auto line = LineString::Create(coords);
        geometries.push_back(std::move(line));
    }
    
    for (int i = 0; i < 3; ++i) {
        std::vector<Coordinate> ring;
        ring.push_back(Coordinate(116.0 + i * 0.2, 39.0 + i * 0.2));
        ring.push_back(Coordinate(116.5 + i * 0.2, 39.0 + i * 0.2));
        ring.push_back(Coordinate(116.5 + i * 0.2, 39.5 + i * 0.2));
        ring.push_back(Coordinate(116.0 + i * 0.2, 39.5 + i * 0.2));
        ring.push_back(Coordinate(116.0 + i * 0.2, 39.0 + i * 0.2));
        auto linearRing = LinearRing::Create(ring);
        auto polygon = Polygon::Create(std::move(linearRing));
        geometries.push_back(std::move(polygon));
    }
    
    EXPECT_EQ(geometries.size(), 18u);
    
    Envelope totalExtent;
    for (const auto& geom : geometries) {
        totalExtent.ExpandToInclude(geom->GetEnvelope());
    }
    
    EXPECT_TRUE(totalExtent.GetWidth() > 0);
    EXPECT_TRUE(totalExtent.GetHeight() > 0);
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 1024, 768);
    ASSERT_NE(device, nullptr);
    
    auto params = facade.CreateDrawParams(totalExtent, 1024, 768, 96.0);
    EXPECT_GT(params.pixel_width, 0);
    EXPECT_GT(params.pixel_height, 0);
}
