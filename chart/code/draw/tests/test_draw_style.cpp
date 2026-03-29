#include <gtest/gtest.h>
#include "ogc/draw/draw_style.h"

using namespace ogc::draw;

TEST(PenTest, DefaultConstructor) {
    Pen pen;
    EXPECT_EQ(pen.color, Color(0, 0, 0, 255));
    EXPECT_DOUBLE_EQ(pen.width, 1.0);
    EXPECT_EQ(pen.style, PenStyle::kSolid);
    EXPECT_EQ(pen.cap, LineCap::kFlat);
    EXPECT_EQ(pen.join, LineJoin::kMiter);
}

TEST(PenTest, ParameterizedConstructor) {
    Pen pen(Color::Red(), 2.5);
    EXPECT_EQ(pen.color, Color::Red());
    EXPECT_DOUBLE_EQ(pen.width, 2.5);
}

TEST(PenTest, Solid) {
    Pen pen = Pen::Solid(Color::Blue(), 3.0);
    EXPECT_EQ(pen.color, Color::Blue());
    EXPECT_DOUBLE_EQ(pen.width, 3.0);
    EXPECT_EQ(pen.style, PenStyle::kSolid);
}

TEST(PenTest, Dash) {
    Pen pen = Pen::Dash(Color::Red());
    EXPECT_EQ(pen.style, PenStyle::kDash);
    EXPECT_FALSE(pen.dashPattern.empty());
}

TEST(PenTest, Dot) {
    Pen pen = Pen::Dot(Color::Green());
    EXPECT_EQ(pen.style, PenStyle::kDot);
    EXPECT_FALSE(pen.dashPattern.empty());
}

TEST(PenTest, NoPen) {
    Pen pen = Pen::NoPen();
    EXPECT_EQ(pen.style, PenStyle::kNone);
    EXPECT_FALSE(pen.IsVisible());
}

TEST(PenTest, WithWidth) {
    Pen pen = Pen::Solid(Color::Red(), 1.0);
    Pen pen2 = pen.WithWidth(5.0);
    EXPECT_DOUBLE_EQ(pen2.width, 5.0);
}

TEST(PenTest, WithColor) {
    Pen pen = Pen::Solid(Color::Red(), 1.0);
    Pen pen2 = pen.WithColor(Color::Blue());
    EXPECT_EQ(pen2.color, Color::Blue());
}

TEST(PenTest, IsVisible) {
    Pen pen1 = Pen::Solid(Color::Red(), 1.0);
    EXPECT_TRUE(pen1.IsVisible());
    
    Pen pen2 = Pen::NoPen();
    EXPECT_FALSE(pen2.IsVisible());
    
    Pen pen3 = Pen::Solid(Color(255, 0, 0, 0), 1.0);
    EXPECT_FALSE(pen3.IsVisible());
}

TEST(BrushTest, DefaultConstructor) {
    Brush brush;
    EXPECT_EQ(brush.color, Color(255, 255, 255, 255));
    EXPECT_EQ(brush.style, BrushStyle::kSolid);
}

TEST(BrushTest, ParameterizedConstructor) {
    Brush brush(Color::Red());
    EXPECT_EQ(brush.color, Color::Red());
}

TEST(BrushTest, Solid) {
    Brush brush = Brush::Solid(Color::Blue());
    EXPECT_EQ(brush.color, Color::Blue());
    EXPECT_EQ(brush.style, BrushStyle::kSolid);
}

TEST(BrushTest, NoBrush) {
    Brush brush = Brush::NoBrush();
    EXPECT_EQ(brush.style, BrushStyle::kNone);
    EXPECT_FALSE(brush.IsVisible());
}

TEST(BrushTest, Horizontal) {
    Brush brush = Brush::Horizontal(Color::Red());
    EXPECT_EQ(brush.style, BrushStyle::kHorizontal);
}

TEST(BrushTest, Vertical) {
    Brush brush = Brush::Vertical(Color::Green());
    EXPECT_EQ(brush.style, BrushStyle::kVertical);
}

TEST(BrushTest, Cross) {
    Brush brush = Brush::Cross(Color::Blue());
    EXPECT_EQ(brush.style, BrushStyle::kCross);
}

TEST(BrushTest, Texture) {
    Brush brush = Brush::Texture("path/to/texture.png");
    EXPECT_EQ(brush.style, BrushStyle::kTexture);
    EXPECT_EQ(brush.texturePath, "path/to/texture.png");
}

TEST(BrushTest, WithColor) {
    Brush brush = Brush::Solid(Color::Red());
    Brush brush2 = brush.WithColor(Color::Blue());
    EXPECT_EQ(brush2.color, Color::Blue());
}

TEST(BrushTest, WithOpacity) {
    Brush brush = Brush::Solid(Color::Red());
    Brush brush2 = brush.WithOpacity(0.5);
    EXPECT_DOUBLE_EQ(brush2.opacity, 0.5);
}

TEST(BrushTest, IsVisible) {
    Brush brush1 = Brush::Solid(Color::Red());
    EXPECT_TRUE(brush1.IsVisible());
    
    Brush brush2 = Brush::NoBrush();
    EXPECT_FALSE(brush2.IsVisible());
}

TEST(FontTest, DefaultConstructor) {
    Font font;
    EXPECT_EQ(font.family, "Arial");
    EXPECT_DOUBLE_EQ(font.size, 12.0);
    EXPECT_EQ(font.weight, FontWeight::kNormal);
    EXPECT_EQ(font.style, FontStyle::kNormal);
    EXPECT_FALSE(font.underline);
    EXPECT_FALSE(font.strikethrough);
}

TEST(FontTest, ParameterizedConstructor) {
    Font font("Times New Roman", 16.0);
    EXPECT_EQ(font.family, "Times New Roman");
    EXPECT_DOUBLE_EQ(font.size, 16.0);
}

TEST(FontTest, DefaultFont) {
    Font font = Font::Default();
    EXPECT_EQ(font.family, "Arial");
    EXPECT_DOUBLE_EQ(font.size, 12.0);
}

TEST(FontTest, WithFamily) {
    Font font = Font::Default();
    Font font2 = font.WithFamily("Courier New");
    EXPECT_EQ(font2.family, "Courier New");
}

TEST(FontTest, WithSize) {
    Font font = Font::Default();
    Font font2 = font.WithSize(24.0);
    EXPECT_DOUBLE_EQ(font2.size, 24.0);
}

TEST(FontTest, WithWeight) {
    Font font = Font::Default();
    Font font2 = font.WithWeight(FontWeight::kBold);
    EXPECT_EQ(font2.weight, FontWeight::kBold);
}

TEST(FontTest, Bold) {
    Font font = Font::Default();
    Font font2 = font.Bold();
    EXPECT_TRUE(font2.IsBold());
}

TEST(FontTest, Italic) {
    Font font = Font::Default();
    Font font2 = font.Italic();
    EXPECT_TRUE(font2.IsItalic());
}

TEST(DrawStyleTest, DefaultConstructor) {
    DrawStyle style;
    EXPECT_DOUBLE_EQ(style.opacity, 1.0);
    EXPECT_EQ(style.fillRule, FillRule::kEvenOdd);
    EXPECT_TRUE(style.antialias);
}

TEST(DrawStyleTest, Stroke) {
    DrawStyle style = DrawStyle::Stroke(Color::Red(), 2.0);
    EXPECT_TRUE(style.HasStroke());
    EXPECT_FALSE(style.HasFill());
    EXPECT_EQ(style.pen.color, Color::Red());
    EXPECT_DOUBLE_EQ(style.pen.width, 2.0);
}

TEST(DrawStyleTest, Fill) {
    DrawStyle style = DrawStyle::Fill(Color::Blue());
    EXPECT_FALSE(style.HasStroke());
    EXPECT_TRUE(style.HasFill());
    EXPECT_EQ(style.brush.color, Color::Blue());
}

TEST(DrawStyleTest, StrokeAndFill) {
    DrawStyle style = DrawStyle::StrokeAndFill(Color::Red(), 2.0, Color::Blue());
    EXPECT_TRUE(style.HasStroke());
    EXPECT_TRUE(style.HasFill());
}

TEST(DrawStyleTest, WithPen) {
    DrawStyle style = DrawStyle::Default();
    Pen pen = Pen::Solid(Color::Red(), 3.0);
    DrawStyle style2 = style.WithPen(pen);
    EXPECT_EQ(style2.pen.color, Color::Red());
    EXPECT_DOUBLE_EQ(style2.pen.width, 3.0);
}

TEST(DrawStyleTest, WithBrush) {
    DrawStyle style = DrawStyle::Default();
    Brush brush = Brush::Solid(Color::Green());
    DrawStyle style2 = style.WithBrush(brush);
    EXPECT_EQ(style2.brush.color, Color::Green());
}

TEST(DrawStyleTest, WithOpacity) {
    DrawStyle style = DrawStyle::Default();
    DrawStyle style2 = style.WithOpacity(0.5);
    EXPECT_DOUBLE_EQ(style2.opacity, 0.5);
}

TEST(DrawStyleTest, WithFillRule) {
    DrawStyle style = DrawStyle::Default();
    DrawStyle style2 = style.WithFillRule(FillRule::kNonZero);
    EXPECT_EQ(style2.fillRule, FillRule::kNonZero);
}

TEST(DrawStyleTest, WithAntialias) {
    DrawStyle style = DrawStyle::Default();
    DrawStyle style2 = style.WithAntialias(false);
    EXPECT_FALSE(style2.antialias);
}
