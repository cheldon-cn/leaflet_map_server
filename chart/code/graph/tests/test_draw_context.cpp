#include <gtest/gtest.h>
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/envelope.h"

TEST(DrawContextTest, DefaultConstructor) {
    ogc::draw::DrawContext context;
    EXPECT_FALSE(context.HasDevice());
    EXPECT_FALSE(context.HasEngine());
    EXPECT_FALSE(context.HasDriver());
    EXPECT_FALSE(context.IsInitialized());
    EXPECT_FALSE(context.IsDrawing());
}

TEST(DrawContextTest, ConstructorWithDevice) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    EXPECT_TRUE(context.HasDevice());
    EXPECT_FALSE(context.HasEngine());
    EXPECT_EQ(context.GetDevice(), device);
}

TEST(DrawContextTest, ConstructorWithDeviceAndEngine) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    auto engine = ogc::draw::DrawEngine::Create(ogc::draw::EngineType::kSimple2D);
    ogc::draw::DrawContext context(device, engine);
    EXPECT_TRUE(context.HasDevice());
    if (engine) {
        EXPECT_TRUE(context.HasEngine());
        EXPECT_EQ(context.GetEngine(), engine);
    }
}

TEST(DrawContextTest, SetGetDevice) {
    ogc::draw::DrawContext context;
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    context.SetDevice(device);
    EXPECT_TRUE(context.HasDevice());
    EXPECT_EQ(context.GetDevice(), device);
}

TEST(DrawContextTest, SetGetEngine) {
    ogc::draw::DrawContext context;
    auto engine = ogc::draw::DrawEngine::Create(ogc::draw::EngineType::kSimple2D);
    if (engine) {
        context.SetEngine(engine);
        EXPECT_TRUE(context.HasEngine());
        EXPECT_EQ(context.GetEngine(), engine);
    }
}

TEST(DrawContextTest, SetGetDriver) {
    ogc::draw::DrawContext context;
    auto driver = ogc::draw::DrawDriver::Create("RasterImageDriver");
    if (driver) {
        context.SetDriver(driver);
        EXPECT_TRUE(context.HasDriver());
        EXPECT_EQ(context.GetDriver(), driver);
    }
}

TEST(DrawContextTest, Initialize) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    auto result = context.Initialize();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_TRUE(context.IsInitialized());
}

TEST(DrawContextTest, Finalize) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    auto result = context.Finalize();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(context.IsInitialized());
}

TEST(DrawContextTest, BeginEndDraw) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    auto result = context.BeginDraw(params);
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_TRUE(context.IsDrawing());
    
    result = context.EndDraw();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(context.IsDrawing());
}

TEST(DrawContextTest, AbortDraw) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    auto result = context.AbortDraw();
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
    EXPECT_FALSE(context.IsDrawing());
}

TEST(DrawContextTest, TransformStack) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.PushTransform();
    context.Translate(10.0, 20.0);
    
    ogc::draw::TransformMatrix transform = context.GetTransform();
    EXPECT_DOUBLE_EQ(transform.Get(0, 2), 10.0);
    EXPECT_DOUBLE_EQ(transform.Get(1, 2), 20.0);
    
    context.PopTransform();
    transform = context.GetTransform();
    EXPECT_DOUBLE_EQ(transform.Get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(transform.Get(1, 2), 0.0);
}

TEST(DrawContextTest, Scale) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.Scale(2.0, 3.0);
    
    ogc::draw::TransformMatrix transform = context.GetTransform();
    EXPECT_DOUBLE_EQ(transform.Get(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(transform.Get(1, 1), 3.0);
}

TEST(DrawContextTest, Rotate) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.Rotate(0.0);
    
    ogc::draw::TransformMatrix transform = context.GetTransform();
    EXPECT_DOUBLE_EQ(transform.Get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(transform.Get(1, 1), 1.0);
}

TEST(DrawContextTest, ResetTransform) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.Translate(10.0, 20.0);
    context.Scale(2.0, 3.0);
    context.ResetTransform();
    
    ogc::draw::TransformMatrix transform = context.GetTransform();
    EXPECT_DOUBLE_EQ(transform.Get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(transform.Get(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(transform.Get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(transform.Get(1, 2), 0.0);
}

TEST(DrawContextTest, StyleStack) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawStyle style1;
    style1.fill.color = ogc::draw::Color::Red().GetRGBA();
    
    context.PushStyle(style1);
    ogc::draw::DrawStyle retrieved = context.GetStyle();
    EXPECT_EQ(retrieved.fill.color, ogc::draw::Color::Red().GetRGBA());
    
    context.PopStyle();
}

TEST(DrawContextTest, SetGetStyle) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Blue().GetRGBA();
    style.stroke.color = ogc::draw::Color::Black().GetRGBA();
    style.stroke.width = 2.0;
    
    context.SetStyle(style);
    ogc::draw::DrawStyle retrieved = context.GetStyle();
    EXPECT_EQ(retrieved.fill.color, ogc::draw::Color::Blue().GetRGBA());
    EXPECT_EQ(retrieved.stroke.color, ogc::draw::Color::Black().GetRGBA());
    EXPECT_DOUBLE_EQ(retrieved.stroke.width, 2.0);
}

TEST(DrawContextTest, OpacityStack) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.PushOpacity(0.5);
    EXPECT_DOUBLE_EQ(context.GetOpacity(), 0.5);
    
    context.PopOpacity();
    EXPECT_DOUBLE_EQ(context.GetOpacity(), 1.0);
}

TEST(DrawContextTest, SetGetOpacity) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.SetOpacity(0.7);
    EXPECT_DOUBLE_EQ(context.GetOpacity(), 0.7);
}

TEST(DrawContextTest, ClipRect) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.PushClipRect(10, 10, 50, 50);
    context.PopClipRect();
}

TEST(DrawContextTest, ClearClipRect) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    context.PushClipRect(10, 10, 50, 50);
    context.ClearClipRect();
}

TEST(DrawContextTest, Clear) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    context.Clear(ogc::draw::Color::White());
    context.EndDraw();
}

TEST(DrawContextTest, DrawPoint) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    auto result = context.DrawPoint(50, 50);
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawLine) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    auto result = context.DrawLine(10, 10, 90, 90);
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawRect) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    auto result = context.DrawRect(10, 10, 30, 30);
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawCircle) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    auto result = context.DrawCircle(50, 50, 20);
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawPolygon) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    double x[] = {10, 50, 90, 50};
    double y[] = {50, 10, 50, 90};
    
    context.BeginDraw(params);
    auto result = context.DrawPolygon(x, y, 4);
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawPolyline) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    double x[] = {10, 30, 50, 70, 90};
    double y[] = {50, 30, 50, 70, 50};
    
    context.BeginDraw(params);
    auto result = context.DrawPolyline(x, y, 5);
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawText) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    auto result = context.DrawText(50, 50, "Test");
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, DrawTextWithFont) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    ogc::draw::Font font("Arial", 12);
    
    context.BeginDraw(params);
    auto result = context.DrawText(50, 50, "Test", font, ogc::draw::Color::Black());
    context.EndDraw();
    
    EXPECT_EQ(result, ogc::draw::DrawResult::kSuccess);
}

TEST(DrawContextTest, WorldToScreen) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    
    double screenX, screenY;
    context.WorldToScreen(50, 50, screenX, screenY);
    
    EXPECT_DOUBLE_EQ(screenX, 50.0);
    EXPECT_DOUBLE_EQ(screenY, 50.0);
    
    context.EndDraw();
}

TEST(DrawContextTest, ScreenToWorld) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    
    double worldX, worldY;
    context.ScreenToWorld(50, 50, worldX, worldY);
    
    EXPECT_DOUBLE_EQ(worldX, 50.0);
    EXPECT_DOUBLE_EQ(worldY, 50.0);
    
    context.EndDraw();
}

TEST(DrawContextTest, GetVisibleExtent) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    ogc::Envelope extent = context.GetVisibleExtent();
    context.EndDraw();
    
    EXPECT_DOUBLE_EQ(extent.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(extent.GetMinY(), 0.0);
    EXPECT_DOUBLE_EQ(extent.GetMaxX(), 100.0);
    EXPECT_DOUBLE_EQ(extent.GetMaxY(), 100.0);
}

TEST(DrawContextTest, GetScale) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    ogc::draw::DrawParams params;
    params.pixel_width = 100;
    params.pixel_height = 100;
    params.extent = ogc::Envelope(0, 0, 100, 100);
    
    context.BeginDraw(params);
    double scale = context.GetScale();
    context.EndDraw();
    
    EXPECT_DOUBLE_EQ(scale, 1.0);
}

TEST(DrawContextTest, GetDpi) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    context.Initialize();
    
    double dpi = context.GetDpi();
    EXPECT_GT(dpi, 0.0);
}

TEST(DrawContextTest, SetGetBackground) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    
    context.SetBackground(ogc::draw::Color::White());
    ogc::draw::Color bg = context.GetBackground();
    EXPECT_EQ(bg.GetRed(), 255);
    EXPECT_EQ(bg.GetGreen(), 255);
    EXPECT_EQ(bg.GetBlue(), 255);
}

TEST(DrawContextTest, SetGetDefaultStyle) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    
    ogc::draw::DrawStyle style;
    style.fill.color = ogc::draw::Color::Green().GetRGBA();
    
    context.SetDefaultStyle(style);
    ogc::draw::DrawStyle retrieved = context.GetDefaultStyle();
    EXPECT_EQ(retrieved.fill.color, ogc::draw::Color::Green().GetRGBA());
}

TEST(DrawContextTest, SetGetDefaultFont) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    
    ogc::draw::Font font("Arial", 14);
    context.SetDefaultFont(font);
    
    ogc::draw::Font retrieved = context.GetDefaultFont();
    EXPECT_EQ(retrieved.GetFamily(), "Arial");
    EXPECT_EQ(retrieved.GetSize(), 14);
}

TEST(DrawContextTest, SetGetDefaultColor) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    ogc::draw::DrawContext context(device);
    
    context.SetDefaultColor(ogc::draw::Color::Red());
    ogc::draw::Color color = context.GetDefaultColor();
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 0);
    EXPECT_EQ(color.GetBlue(), 0);
}

TEST(DrawContextTest, StaticCreateWithDevice) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    auto context = ogc::draw::DrawContext::Create(device);
    EXPECT_NE(context, nullptr);
    EXPECT_TRUE(context->HasDevice());
}

TEST(DrawContextTest, StaticCreateWithDeviceAndEngine) {
    auto device = std::make_shared<ogc::draw::RasterImageDevice>(100, 100);
    auto engine = ogc::draw::DrawEngine::Create(ogc::draw::EngineType::kSimple2D);
    auto context = ogc::draw::DrawContext::Create(device, engine);
    EXPECT_NE(context, nullptr);
    EXPECT_TRUE(context->HasDevice());
    if (engine) {
        EXPECT_TRUE(context->HasEngine());
    }
}
