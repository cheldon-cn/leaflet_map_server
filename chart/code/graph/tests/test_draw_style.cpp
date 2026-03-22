#include <gtest/gtest.h>
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"

TEST(DrawStyleTest, DefaultConstructor) {
    ogc::draw::DrawStyle style;
    EXPECT_TRUE(style.IsVisible());
    EXPECT_DOUBLE_EQ(style.opacity, 1.0);
    EXPECT_TRUE(style.HasStroke());
    EXPECT_TRUE(style.HasFill());
}

TEST(DrawStyleTest, SetOpacity) {
    ogc::draw::DrawStyle style;
    style.SetOpacity(0.5);
    EXPECT_DOUBLE_EQ(style.opacity, 0.5);
}

TEST(DrawStyleTest, SetStroke) {
    ogc::draw::DrawStyle style;
    ogc::draw::StrokeStyle stroke(0xFF0000FF, 2.5);
    style.SetStroke(stroke);
    EXPECT_TRUE(style.HasStroke());
    EXPECT_DOUBLE_EQ(style.stroke.width, 2.5);
    EXPECT_EQ(style.stroke.color, 0xFF0000FFu);
}

TEST(DrawStyleTest, SetFill) {
    ogc::draw::DrawStyle style;
    ogc::draw::FillStyle fill(0x00FF00FF);
    style.SetFill(fill);
    EXPECT_TRUE(style.HasFill());
    EXPECT_EQ(style.fill.color, 0x00FF00FFu);
}

TEST(DrawStyleTest, HasStroke) {
    ogc::draw::DrawStyle style;
    EXPECT_TRUE(style.HasStroke());
    style.stroke.visible = false;
    EXPECT_FALSE(style.HasStroke());
}

TEST(DrawStyleTest, HasFill) {
    ogc::draw::DrawStyle style;
    EXPECT_TRUE(style.HasFill());
    style.fill.visible = false;
    EXPECT_FALSE(style.HasFill());
}

TEST(DrawStyleTest, SetZOrder) {
    ogc::draw::DrawStyle style;
    style.SetZOrder(10);
    EXPECT_EQ(style.z_order, 10);
}

TEST(DrawStyleTest, SetName) {
    ogc::draw::DrawStyle style;
    style.SetName("test_style");
    EXPECT_EQ(style.name, "test_style");
}

TEST(DrawStyleTest, Reset) {
    ogc::draw::DrawStyle style;
    style.stroke.visible = false;
    style.fill.visible = false;
    style.opacity = 0.5;
    style.Reset();
    EXPECT_TRUE(style.HasStroke());
    EXPECT_TRUE(style.HasFill());
    EXPECT_DOUBLE_EQ(style.opacity, 1.0);
}
