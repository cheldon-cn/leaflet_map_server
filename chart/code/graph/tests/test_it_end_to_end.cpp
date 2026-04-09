#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <chrono>
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/coordinate.h"
#include "ogc/geom/envelope.h"
#include <ogc/proj/coordinate_transform.h>
#include "ogc/graph/render/draw_facade.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace ogc;
using namespace ogc::draw;

class EndToEndIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        DrawFacade::Instance().Initialize();
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
};

TEST_F(EndToEndIntegrationTest, FullPipeline_PointToRender) {
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    Coordinate wgs84(116.397428, 39.90923);
    Coordinate transformed = transform->Transform(wgs84);
    
    auto point = ogc::Point::Create(transformed);
    EXPECT_TRUE(point->IsValid());
    
    auto& facade = DrawFacade::Instance();
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
    ASSERT_NE(device, nullptr);
    
    Envelope extent(transformed.x - 0.01, transformed.y - 0.01, transformed.x + 0.01, transformed.y + 0.01);
    auto params = facade.CreateDrawParams(extent, 800, 600, 96.0);
    
    EXPECT_TRUE(extent.Contains(transformed));
}

TEST_F(EndToEndIntegrationTest, FullPipeline_LineStringToRender) {
    std::vector<Coordinate> coords = {
        Coordinate(116.0, 39.0),
        Coordinate(116.5, 39.5),
        Coordinate(117.0, 40.0)
    };
    
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    std::vector<Coordinate> transformedCoords;
    for (const auto& coord : coords) {
        transformedCoords.push_back(transform->Transform(coord));
    }
    
    auto lineString = LineString::Create(transformedCoords);
    EXPECT_TRUE(lineString->IsValid());
    
    auto& facade = DrawFacade::Instance();
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 1024, 768);
    ASSERT_NE(device, nullptr);
    
    Envelope extent = lineString->GetEnvelope();
    extent.ExpandToInclude(Envelope(extent.GetMinX() - 0.01, extent.GetMinY() - 0.01,
                                    extent.GetMaxX() + 0.01, extent.GetMaxY() + 0.01));
    
    auto params = facade.CreateDrawParams(extent, 1024, 768, 96.0);
    
    auto strokeStyle = facade.CreateStrokeStyle(0xFF0000FF, 2.0);
    EXPECT_EQ(strokeStyle.pen.color.GetRGBA(), 0xFF0000FF);
}

TEST_F(EndToEndIntegrationTest, FullPipeline_PolygonToRender) {
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
    EXPECT_TRUE(polygon->IsValid());
    
    auto& facade = DrawFacade::Instance();
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 1200, 900);
    ASSERT_NE(device, nullptr);
    
    Envelope extent = polygon->GetEnvelope();
    auto params = facade.CreateDrawParams(extent, 1200, 900, 96.0);
    
    auto strokeFillStyle = facade.CreateStrokeFillStyle(0xFF0000FF, 1.0, 0x00FF0080);
    EXPECT_EQ(strokeFillStyle.pen.color.GetRGBA(), 0xFF0000FF);
    EXPECT_EQ(strokeFillStyle.brush.color.GetRGBA(), 0x00FF0080);
}

TEST_F(EndToEndIntegrationTest, FullPipeline_MultiGeometryRender) {
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    std::vector<ogc::PointPtr> points;
    for (int i = 0; i < 5; ++i) {
        Coordinate coord(116.0 + i * 0.1, 39.0 + i * 0.1);
        Coordinate transformed = transform->Transform(coord);
        points.push_back(ogc::Point::Create(transformed));
    }
    
    for (const auto& point : points) {
        EXPECT_TRUE(point->IsValid());
    }
    
    auto& facade = DrawFacade::Instance();
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
    ASSERT_NE(device, nullptr);
    
    Envelope totalExtent;
    for (const auto& point : points) {
        totalExtent.ExpandToInclude(point->GetCoordinate());
    }
    
    auto params = facade.CreateDrawParams(totalExtent, 800, 600, 96.0);
    EXPECT_GT(params.pixel_width, 0);
    EXPECT_GT(params.pixel_height, 0);
}

TEST_F(EndToEndIntegrationTest, PerformanceTest_LargeDataset) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<LineStringPtr> lines;
    lines.reserve(100);
    
    for (int i = 0; i < 100; ++i) {
        std::vector<Coordinate> coords;
        coords.reserve(100);
        for (int j = 0; j < 100; ++j) {
            coords.push_back(Coordinate(116.0 + i * 0.01, 39.0 + j * 0.01));
        }
        lines.push_back(LineString::Create(coords));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(lines.size(), 100u);
    EXPECT_LT(duration.count(), 1000);
    
    auto& facade = DrawFacade::Instance();
    Envelope totalExtent;
    for (const auto& line : lines) {
        totalExtent.ExpandToInclude(line->GetEnvelope());
    }
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 1024, 768);
    ASSERT_NE(device, nullptr);
    
    auto params = facade.CreateDrawParams(totalExtent, 1024, 768, 96.0);
}
