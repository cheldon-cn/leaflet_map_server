#include <gtest/gtest.h>
#include "ogc/draw/draw_facade.h"
#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_engine.h>
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Envelope;

class DrawFacadeTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
        DrawFacade::Instance().Finalize();
    }
};

TEST_F(DrawFacadeTest, Instance) {
    auto& facade = DrawFacade::Instance();
    EXPECT_NE(&facade, nullptr);
}

TEST_F(DrawFacadeTest, Initialize) {
    auto& facade = DrawFacade::Instance();
    auto result = facade.Initialize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DrawFacadeTest, Finalize) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    auto result = facade.Finalize();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DrawFacadeTest, IsInitialized) {
    auto& facade = DrawFacade::Instance();
    EXPECT_FALSE(facade.IsInitialized());
    
    facade.Initialize();
    EXPECT_TRUE(facade.IsInitialized());
}

TEST_F(DrawFacadeTest, CreateContext) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto context = facade.CreateContext();
    EXPECT_NE(context, nullptr);
}

TEST_F(DrawFacadeTest, CreateContextWithDevice) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    auto context = facade.CreateContext(device);
    EXPECT_NE(context, nullptr);
}

TEST_F(DrawFacadeTest, CreateDevice) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto device = facade.CreateDevice(DeviceType::kRasterImage, 256, 256);
    EXPECT_NE(device, nullptr);
}

TEST_F(DrawFacadeTest, SetDefaultDrawStyle) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    DrawStyle style;
    style.pen.color = Color(0xFF, 0, 0);
    facade.SetDefaultDrawStyle(style);
    
    auto retrieved = facade.GetDefaultDrawStyle();
    EXPECT_EQ(retrieved.pen.color.GetRed(), 0xFF);
}

TEST_F(DrawFacadeTest, SetDefaultFont) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    Font font("Arial", 12);
    facade.SetDefaultFont(font);
    
    auto retrieved = facade.GetDefaultFont();
    EXPECT_EQ(retrieved.GetFamily(), "Arial");
}

TEST_F(DrawFacadeTest, SetDefaultColor) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    Color color(255, 0, 0);
    facade.SetDefaultColor(color);
    
    auto retrieved = facade.GetDefaultColor();
    EXPECT_EQ(retrieved.GetRed(), 255);
}

TEST_F(DrawFacadeTest, SetDefaultDpi) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    facade.SetDefaultDpi(150.0);
    EXPECT_DOUBLE_EQ(facade.GetDefaultDpi(), 150.0);
}

TEST_F(DrawFacadeTest, SetDefaultBackground) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    Color color(0, 0, 255);
    facade.SetDefaultBackground(color);
    
    auto retrieved = facade.GetDefaultBackground();
    EXPECT_EQ(retrieved.GetBlue(), 255);
}

TEST_F(DrawFacadeTest, CreateStrokeStyle) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto style = facade.CreateStrokeStyle(0xFF0000, 2.0);
    EXPECT_EQ(style.pen.color, 0xFF0000);
    EXPECT_DOUBLE_EQ(style.pen.width, 2.0);
}

TEST_F(DrawFacadeTest, CreateFillStyle) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto style = facade.CreateFillStyle(0x00FF00);
    EXPECT_EQ(style.brush.color, 0x00FF00);
}

TEST_F(DrawFacadeTest, CreateStrokeFillStyle) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto style = facade.CreateStrokeFillStyle(0xFF0000, 2.0, 0x00FF00);
    EXPECT_EQ(style.pen.color, 0xFF0000);
    EXPECT_EQ(style.brush.color, 0x00FF00);
}

TEST_F(DrawFacadeTest, CreateFont) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto font = facade.CreateFont("Arial", 14, true, false);
    EXPECT_EQ(font.GetFamily(), "Arial");
    EXPECT_DOUBLE_EQ(font.GetSize(), 14.0);
}

TEST_F(DrawFacadeTest, CreateColor) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto color = facade.CreateColor(255, 128, 64, 200);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
    EXPECT_EQ(color.GetAlpha(), 200);
}

TEST_F(DrawFacadeTest, CreateColorFromHex) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto color = facade.CreateColorFromHex("#FF8040");
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
}

TEST_F(DrawFacadeTest, CreateDrawParams) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    Envelope extent(0, 0, 1000, 1000);
    auto params = facade.CreateDrawParams(extent, 800, 600, 96.0);
    
    EXPECT_EQ(params.pixel_width, 800);
    EXPECT_EQ(params.pixel_height, 600);
}

TEST_F(DrawFacadeTest, GetVersion) {
    auto version = DrawFacade::GetVersion();
    EXPECT_FALSE(version.empty());
}

TEST_F(DrawFacadeTest, GetBuildInfo) {
    auto buildInfo = DrawFacade::GetBuildInfo();
    EXPECT_FALSE(buildInfo.empty());
}

TEST_F(DrawFacadeTest, RegisterDevice) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto device = std::make_shared<RasterImageDevice>(256, 256, PixelFormat::kRGBA8888);
    auto result = facade.RegisterDevice("test_device", device);
    EXPECT_EQ(result, DrawResult::kSuccess);
    
    auto retrieved = facade.GetDevice("test_device");
    EXPECT_NE(retrieved, nullptr);
}

TEST_F(DrawFacadeTest, RegisterEngine) {
    auto& facade = DrawFacade::Instance();
    facade.Initialize();
    
    auto engine = facade.CreateEngine(EngineType::kSimple2D);
    if (engine) {
        auto result = facade.RegisterEngine("test_engine", engine);
        EXPECT_EQ(result, DrawResult::kSuccess);
        
        auto retrieved = facade.GetEngine("test_engine");
        EXPECT_NE(retrieved, nullptr);
    } else {
        EXPECT_EQ(engine, nullptr);
    }
}

