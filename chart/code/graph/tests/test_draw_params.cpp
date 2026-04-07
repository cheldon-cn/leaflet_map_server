#include <gtest/gtest.h>
#include "ogc/graph/render/draw_params.h"
#include "ogc/envelope.h"

using ogc::graph::DrawParams;
using ogc::Envelope;

TEST(DrawParamsTest, DefaultConstructor) {
    DrawParams params;
    EXPECT_DOUBLE_EQ(params.dpi, 96.0);
    EXPECT_DOUBLE_EQ(params.scale, 1.0);
    EXPECT_TRUE(params.extent.IsEmpty());
}

TEST(DrawParamsTest, ParameterizedConstructor) {
    Envelope extent(0.0, 0.0, 100.0, 100.0);
    DrawParams params(extent, 800, 600, 150.0);
    EXPECT_DOUBLE_EQ(params.extent.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(params.extent.GetMaxX(), 100.0);
    EXPECT_EQ(params.pixel_width, 800);
    EXPECT_EQ(params.pixel_height, 600);
    EXPECT_DOUBLE_EQ(params.dpi, 150.0);
}

TEST(DrawParamsTest, SetDPI) {
    DrawParams params;
    params.SetDpi(150.0);
    EXPECT_DOUBLE_EQ(params.dpi, 150.0);
}

TEST(DrawParamsTest, SetExtent) {
    DrawParams params;
    Envelope extent(0.0, 0.0, 100.0, 100.0);
    params.SetExtent(extent);
    EXPECT_DOUBLE_EQ(params.extent.GetMinX(), 0.0);
    EXPECT_DOUBLE_EQ(params.extent.GetMaxX(), 100.0);
}

TEST(DrawParamsTest, SetSize) {
    DrawParams params;
    params.SetSize(800, 600);
    EXPECT_EQ(params.pixel_width, 800);
    EXPECT_EQ(params.pixel_height, 600);
}

TEST(DrawParamsTest, SetRotation) {
    DrawParams params;
    params.SetRotation(45.0);
    EXPECT_DOUBLE_EQ(params.rotation, 45.0);
}

TEST(DrawParamsTest, SetCenter) {
    DrawParams params;
    params.SetCenter(50.0, 60.0);
    EXPECT_DOUBLE_EQ(params.center_x, 50.0);
    EXPECT_DOUBLE_EQ(params.center_y, 60.0);
}

TEST(DrawParamsTest, SetBackgroundColor) {
    DrawParams params;
    params.SetBackgroundColor(0xFF0000FF);
    EXPECT_TRUE(params.has_background);
    EXPECT_EQ(params.background_color, 0xFF0000FFu);
}

TEST(DrawParamsTest, GetPixelSize) {
    Envelope extent(0.0, 0.0, 100.0, 100.0);
    DrawParams params(extent, 100, 100);
    double pixelSizeX = params.GetPixelSizeX();
    double pixelSizeY = params.GetPixelSizeY();
    EXPECT_DOUBLE_EQ(pixelSizeX, 1.0);
    EXPECT_DOUBLE_EQ(pixelSizeY, 1.0);
}

TEST(DrawParamsTest, IsValid) {
    DrawParams params;
    EXPECT_FALSE(params.IsValid());
    
    params.SetSize(800, 600);
    params.SetExtent(Envelope(0.0, 0.0, 100.0, 100.0));
    EXPECT_TRUE(params.IsValid());
}

TEST(DrawParamsTest, Reset) {
    DrawParams params;
    params.SetSize(800, 600);
    params.SetDpi(150.0);
    params.Reset();
    EXPECT_EQ(params.pixel_width, 0);
    EXPECT_EQ(params.pixel_height, 0);
    EXPECT_DOUBLE_EQ(params.dpi, 96.0);
}
