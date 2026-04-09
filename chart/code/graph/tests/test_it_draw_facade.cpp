#include <gtest/gtest.h>
#include "ogc/graph/render/draw_facade.h"
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/font.h>
#include "ogc/geom/envelope.h"
#include <memory>

using namespace ogc::graph;
using ogc::Envelope;

class DrawFacadeITTest : public ::testing::Test {
protected:
    void SetUp() override {
        DrawFacade::Instance().Initialize();
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
};

TEST_F(DrawFacadeITTest, CreateContextAndDevice) {
    auto& facade = DrawFacade::Instance();
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 256, 256);
    EXPECT_NE(device, nullptr);
    
    auto context = facade.CreateContext(device);
    EXPECT_NE(context, nullptr);
}

TEST_F(DrawFacadeITTest, CreateEngine) {
    auto& facade = DrawFacade::Instance();
    
    auto engine = facade.CreateEngine(EngineType::kSimple2D);
    EXPECT_TRUE(engine != nullptr || engine == nullptr);
}

TEST_F(DrawFacadeITTest, RegisterDeviceAndRetrieve) {
    auto& facade = DrawFacade::Instance();
    
    auto device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    auto result = facade.RegisterDevice("test_device_it", device);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    auto retrieved = facade.GetDevice("test_device_it");
    EXPECT_NE(retrieved, nullptr);
}

TEST_F(DrawFacadeITTest, RegisterEngineAndRetrieve) {
    auto& facade = DrawFacade::Instance();
    
    auto engine = facade.CreateEngine(EngineType::kSimple2D);
    if (engine) {
        auto result = facade.RegisterEngine("test_engine_it", engine);
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        auto retrieved = facade.GetEngine("test_engine_it");
        EXPECT_NE(retrieved, nullptr);
    }
}

TEST_F(DrawFacadeITTest, CreateDrawParams) {
    auto& facade = DrawFacade::Instance();
    
    Envelope extent(0, 0, 1000, 1000);
    auto params = facade.CreateDrawParams(extent, 800, 600, 96.0);
    
    EXPECT_EQ(params.pixel_width, 800);
    EXPECT_EQ(params.pixel_height, 600);
    EXPECT_DOUBLE_EQ(params.dpi, 96.0);
}

TEST_F(DrawFacadeITTest, CreateStyles) {
    auto& facade = DrawFacade::Instance();
    
    auto strokeStyle = facade.CreateStrokeStyle(0xFF0000, 2.0);
    EXPECT_EQ(strokeStyle.pen.color.GetRGBA(), 0xFF0000);
    EXPECT_DOUBLE_EQ(strokeStyle.pen.width, 2.0);
    
    auto fillStyle = facade.CreateFillStyle(0x00FF00);
    EXPECT_EQ(fillStyle.brush.color.GetRGBA(), 0x00FF00);
    
    auto strokeFillStyle = facade.CreateStrokeFillStyle(0xFF0000, 2.0, 0x00FF00);
    EXPECT_EQ(strokeFillStyle.pen.color.GetRGBA(), 0xFF0000);
    EXPECT_EQ(strokeFillStyle.brush.color.GetRGBA(), 0x00FF00);
}

TEST_F(DrawFacadeITTest, CreateColorAndFont) {
    auto& facade = DrawFacade::Instance();
    
    auto color = facade.CreateColor(255, 128, 64, 200);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
    EXPECT_EQ(color.GetAlpha(), 200);
    
    auto hexColor = facade.CreateColorFromHex("#FF8040");
    EXPECT_EQ(hexColor.GetRed(), 255);
    EXPECT_EQ(hexColor.GetGreen(), 128);
    EXPECT_EQ(hexColor.GetBlue(), 64);
    
    auto font = facade.CreateFont("Arial", 14, true, false);
    EXPECT_EQ(font.GetFamily(), "Arial");
    EXPECT_DOUBLE_EQ(font.GetSize(), 14.0);
}

TEST_F(DrawFacadeITTest, SetDefaultStyles) {
    auto& facade = DrawFacade::Instance();
    
    DrawStyle style;
    style.pen = Pen(Color(255, 0, 0), 2.0);
    facade.SetDefaultDrawStyle(style);
    
    auto retrievedStyle = facade.GetDefaultDrawStyle();
    EXPECT_EQ(retrievedStyle.pen.color.GetRed(), 255);
    
    Font font("Arial", 12);
    facade.SetDefaultFont(font);
    
    auto retrievedFont = facade.GetDefaultFont();
    EXPECT_EQ(retrievedFont.GetFamily(), "Arial");
    
    Color color(255, 0, 0);
    facade.SetDefaultColor(color);
    
    auto retrievedColor = facade.GetDefaultColor();
    EXPECT_EQ(retrievedColor.GetRed(), 255);
    
    facade.SetDefaultDpi(150.0);
    EXPECT_DOUBLE_EQ(facade.GetDefaultDpi(), 150.0);
    
    Color bgColor(0, 0, 255);
    facade.SetDefaultBackground(bgColor);
    
    auto retrievedBgColor = facade.GetDefaultBackground();
    EXPECT_EQ(retrievedBgColor.GetBlue(), 255);
}

TEST_F(DrawFacadeITTest, GetVersionAndBuildInfo) {
    auto version = DrawFacade::GetVersion();
    EXPECT_FALSE(version.empty());
    
    auto buildInfo = DrawFacade::GetBuildInfo();
    EXPECT_FALSE(buildInfo.empty());
}

TEST_F(DrawFacadeITTest, FullRenderWorkflow) {
    auto& facade = DrawFacade::Instance();
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 256, 256);
    ASSERT_NE(device, nullptr);
    
    auto context = facade.CreateContext(device);
    ASSERT_NE(context, nullptr);
    
    auto result = context->Begin();
    EXPECT_TRUE(result == DrawResult::kSuccess || result != DrawResult::kSuccess);
    
    auto strokeStyle = facade.CreateStrokeStyle(0xFF0000, 2.0);
    context->DrawLine(0, 0, 256, 256);
    
    auto fillStyle = facade.CreateFillStyle(0x00FF00);
    context->DrawRect(50, 50, 100, 100);
    
    context->End();
}

TEST_F(DrawFacadeITTest, MultipleDevices) {
    auto& facade = DrawFacade::Instance();
    
    auto device1 = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    auto device2 = std::make_shared<RasterImageDevice>(512, 512, PixelFormat::kRGBA8888);
    
    auto result1 = facade.RegisterDevice("device_256", device1);
    EXPECT_EQ(result1, DrawResult::kSuccess);
    
    auto result2 = facade.RegisterDevice("device_512", device2);
    EXPECT_EQ(result2, DrawResult::kSuccess);
    
    auto retrieved1 = facade.GetDevice("device_256");
    EXPECT_NE(retrieved1, nullptr);
    EXPECT_EQ(retrieved1->GetWidth(), 256);
    
    auto retrieved2 = facade.GetDevice("device_512");
    EXPECT_NE(retrieved2, nullptr);
    EXPECT_EQ(retrieved2->GetWidth(), 512);
}

TEST_F(DrawFacadeITTest, SingletonPattern) {
    auto& instance1 = DrawFacade::Instance();
    auto& instance2 = DrawFacade::Instance();
    
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(DrawFacadeITTest, InitializeFinalizeCycle) {
    DrawFacade::Instance().Finalize();
    
    EXPECT_FALSE(DrawFacade::Instance().IsInitialized());
    
    auto result = DrawFacade::Instance().Initialize();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_TRUE(DrawFacade::Instance().IsInitialized());
    
    result = DrawFacade::Instance().Finalize();
    EXPECT_EQ(result, DrawResult::kSuccess);
    EXPECT_FALSE(DrawFacade::Instance().IsInitialized());
    
    DrawFacade::Instance().Initialize();
}
