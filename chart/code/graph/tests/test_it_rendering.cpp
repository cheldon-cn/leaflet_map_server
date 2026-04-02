#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "ogc/draw/draw_facade.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include "ogc/draw/coordinate_transform.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace ogc;
using namespace ogc::draw;

class RenderingIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        DrawFacade::Instance().Initialize();
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
};

TEST_F(RenderingIntegrationTest, GeometryToRenderPipeline) {
    auto& facade = DrawFacade::Instance();
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
    ASSERT_NE(device, nullptr);
    
    auto context = facade.CreateContext(device);
    ASSERT_NE(context, nullptr);
    
    std::vector<Coordinate> coords = {
        Coordinate(100, 100),
        Coordinate(200, 200),
        Coordinate(300, 150)
    };
    auto lineString = LineString::Create(coords);
    EXPECT_TRUE(lineString->IsValid());
    
    Envelope extent(0, 0, 800, 600);
    auto params = facade.CreateDrawParams(extent, 800, 600, 96.0);
    EXPECT_EQ(params.pixel_width, 800);
    EXPECT_EQ(params.pixel_height, 600);
}

TEST_F(RenderingIntegrationTest, MultiGeometryRendering) {
    auto& facade = DrawFacade::Instance();
    
    auto device = std::make_shared<RasterImageDevice>(1024, 768, PixelFormat::kRGBA8888);
    auto result = facade.RegisterDevice("multi_geom_device", device);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    std::vector<Coordinate> pointCoords;
    for (int i = 0; i < 10; ++i) {
        pointCoords.push_back(Coordinate(i * 100.0, i * 50.0));
    }
    auto multiPoint = MultiPoint::Create(pointCoords);
    EXPECT_TRUE(multiPoint->IsValid());
    
    std::vector<LineStringPtr> lines;
    for (int i = 0; i < 5; ++i) {
        std::vector<Coordinate> coords = {
            Coordinate(i * 100.0, 0),
            Coordinate(i * 100.0 + 50, 100)
        };
        lines.push_back(std::move(LineString::Create(coords)));
    }
    auto multiLineString = MultiLineString::Create(std::move(lines));
    EXPECT_TRUE(multiLineString->IsValid());
}

TEST_F(RenderingIntegrationTest, StyleApplication) {
    auto& facade = DrawFacade::Instance();
    
    auto strokeFillStyle = facade.CreateStrokeFillStyle(0xFF0000FF, 1.0, 0x00FF0080);
    EXPECT_EQ(strokeFillStyle.pen.color.GetRGBA(), 0xFF0000FF);
    EXPECT_EQ(strokeFillStyle.brush.color.GetRGBA(), 0x00FF0080);
    
    auto strokeStyle = facade.CreateStrokeStyle(0x0000FFFF, 2.0);
    EXPECT_EQ(strokeStyle.pen.color.GetRGBA(), 0x0000FFFF);
    EXPECT_DOUBLE_EQ(strokeStyle.pen.width, 2.0);
}

TEST_F(RenderingIntegrationTest, CoordinateTransformIntegration) {
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    
    Coordinate worldCoord(116.397428, 39.90923);
    Coordinate screenCoord = transform->Transform(worldCoord);
    
    auto point = ogc::Point::Create(worldCoord);
    EXPECT_TRUE(point->IsValid());
    
    auto transformedGeom = transform->Transform(point.get());
    ASSERT_NE(transformedGeom, nullptr);
    
    ogc::Point* transformedPoint = dynamic_cast<ogc::Point*>(transformedGeom.get());
    ASSERT_NE(transformedPoint, nullptr);
}

TEST_F(RenderingIntegrationTest, DeviceContextManagement) {
    auto& facade = DrawFacade::Instance();
    
    auto device1 = facade.CreateDevice(DeviceType::kRasterImage, 800, 600);
    ASSERT_NE(device1, nullptr);
    
    auto device2 = facade.CreateDevice(DeviceType::kRasterImage, 1024, 768);
    ASSERT_NE(device2, nullptr);
    
    auto context1 = facade.CreateContext(device1);
    ASSERT_NE(context1, nullptr);
    
    auto context2 = facade.CreateContext(device2);
    ASSERT_NE(context2, nullptr);
}

TEST_F(RenderingIntegrationTest, EnvelopeBasedRendering) {
    auto& facade = DrawFacade::Instance();
    
    std::vector<Coordinate> coords = {
        Coordinate(100, 100),
        Coordinate(200, 200),
        Coordinate(300, 150),
        Coordinate(400, 250)
    };
    auto lineString = LineString::Create(coords);
    EXPECT_TRUE(lineString->IsValid());
    
    Envelope geomEnv = lineString->GetEnvelope();
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 
                                      static_cast<int>(geomEnv.GetWidth()), 
                                      static_cast<int>(geomEnv.GetHeight()));
    ASSERT_NE(device, nullptr);
    
    auto params = facade.CreateDrawParams(geomEnv, 
                                          static_cast<int>(geomEnv.GetWidth()), 
                                          static_cast<int>(geomEnv.GetHeight()), 
                                          96.0);
    EXPECT_GE(params.pixel_width, 300);
    EXPECT_GE(params.pixel_height, 150);
}
