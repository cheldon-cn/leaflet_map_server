#include <gtest/gtest.h>
#include "ogc/draw/draw_types.h"

using namespace ogc::draw;

TEST(ColorTest, DefaultConstructor) {
    Color c;
    EXPECT_EQ(c.GetRed(), 0);
    EXPECT_EQ(c.GetGreen(), 0);
    EXPECT_EQ(c.GetBlue(), 0);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST(ColorTest, ParameterizedConstructor) {
    Color c(100, 150, 200, 128);
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 128);
}

TEST(ColorTest, FromHex) {
    Color c = Color::FromHex(0xFF8000);
    EXPECT_EQ(c.GetRed(), 255);
    EXPECT_EQ(c.GetGreen(), 128);
    EXPECT_EQ(c.GetBlue(), 0);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST(ColorTest, FromHexWithAlpha) {
    Color c = Color::FromHexWithAlpha(0xFF800080);
    EXPECT_EQ(c.GetRed(), 255);
    EXPECT_EQ(c.GetGreen(), 128);
    EXPECT_EQ(c.GetBlue(), 0);
    EXPECT_EQ(c.GetAlpha(), 128);
}

TEST(ColorTest, ToHex) {
    Color c(255, 128, 0, 255);
    EXPECT_EQ(c.ToHex(), 0xFF8000);
}

TEST(ColorTest, ToHexWithAlpha) {
    Color c(255, 128, 0, 128);
    EXPECT_EQ(c.ToHexWithAlpha(), 0xFF800080);
}

TEST(ColorTest, FromHSV_Red) {
    Color c = Color::FromHSV(0, 1.0, 1.0);
    EXPECT_EQ(c.GetRed(), 255);
    EXPECT_EQ(c.GetGreen(), 0);
    EXPECT_EQ(c.GetBlue(), 0);
}

TEST(ColorTest, FromHSV_Green) {
    Color c = Color::FromHSV(120, 1.0, 1.0);
    EXPECT_EQ(c.GetRed(), 0);
    EXPECT_EQ(c.GetGreen(), 255);
    EXPECT_EQ(c.GetBlue(), 0);
}

TEST(ColorTest, FromHSV_Blue) {
    Color c = Color::FromHSV(240, 1.0, 1.0);
    EXPECT_EQ(c.GetRed(), 0);
    EXPECT_EQ(c.GetGreen(), 0);
    EXPECT_EQ(c.GetBlue(), 255);
}

TEST(ColorTest, ToHSV) {
    Color c(255, 0, 0);
    double h, s, v;
    c.GetHSV(h, s, v);
    EXPECT_NEAR(h, 0.0, 0.1);
    EXPECT_NEAR(s, 1.0, 0.01);
    EXPECT_NEAR(v, 1.0, 0.01);
}

TEST(ColorTest, Equality) {
    Color c1(100, 150, 200, 128);
    Color c2(100, 150, 200, 128);
    Color c3(100, 150, 200, 255);
    
    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);
    EXPECT_TRUE(c1 != c3);
}

TEST(ColorTest, WithAlpha) {
    Color c(100, 150, 200, 255);
    Color c2 = c.WithAlpha(128);
    EXPECT_EQ(c2.GetRed(), 100);
    EXPECT_EQ(c2.GetGreen(), 150);
    EXPECT_EQ(c2.GetBlue(), 200);
    EXPECT_EQ(c2.GetAlpha(), 128);
}

TEST(ColorTest, Blend) {
    Color c1(0, 0, 0, 255);
    Color c2(255, 255, 255, 255);
    Color blended = c1.Blend(c2, 0.5);
    EXPECT_NEAR(blended.GetRed(), 127, 1);
    EXPECT_NEAR(blended.GetGreen(), 127, 1);
    EXPECT_NEAR(blended.GetBlue(), 127, 1);
}

TEST(ColorTest, PredefinedColors) {
    EXPECT_EQ(Color::Black(), Color(0, 0, 0, 255));
    EXPECT_EQ(Color::White(), Color(255, 255, 255, 255));
    EXPECT_EQ(Color::Red(), Color(255, 0, 0, 255));
    EXPECT_EQ(Color::Green(), Color(0, 255, 0, 255));
    EXPECT_EQ(Color::Blue(), Color(0, 0, 255, 255));
}

TEST(PointTest, DefaultConstructor) {
    Point p;
    EXPECT_DOUBLE_EQ(p.x, 0.0);
    EXPECT_DOUBLE_EQ(p.y, 0.0);
}

TEST(PointTest, ParameterizedConstructor) {
    Point p(10.5, 20.5);
    EXPECT_DOUBLE_EQ(p.x, 10.5);
    EXPECT_DOUBLE_EQ(p.y, 20.5);
}

TEST(PointTest, Equality) {
    Point p1(10.0, 20.0);
    Point p2(10.0, 20.0);
    Point p3(10.1, 20.0);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
}

TEST(PointTest, Addition) {
    Point p1(10.0, 20.0);
    Point p2(5.0, 15.0);
    Point result = p1 + p2;
    
    EXPECT_DOUBLE_EQ(result.x, 15.0);
    EXPECT_DOUBLE_EQ(result.y, 35.0);
}

TEST(PointTest, Subtraction) {
    Point p1(10.0, 20.0);
    Point p2(5.0, 15.0);
    Point result = p1 - p2;
    
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 5.0);
}

TEST(PointTest, Scale) {
    Point p(10.0, 20.0);
    Point result = p * 2.0;
    
    EXPECT_DOUBLE_EQ(result.x, 20.0);
    EXPECT_DOUBLE_EQ(result.y, 40.0);
}

TEST(PointTest, DistanceTo) {
    Point p1(0.0, 0.0);
    Point p2(3.0, 4.0);
    
    EXPECT_DOUBLE_EQ(p1.DistanceTo(p2), 5.0);
}

TEST(SizeTest, DefaultConstructor) {
    Size s;
    EXPECT_DOUBLE_EQ(s.w, 0.0);
    EXPECT_DOUBLE_EQ(s.h, 0.0);
}

TEST(SizeTest, ParameterizedConstructor) {
    Size s(100.0, 200.0);
    EXPECT_DOUBLE_EQ(s.w, 100.0);
    EXPECT_DOUBLE_EQ(s.h, 200.0);
}

TEST(SizeTest, IsEmpty) {
    Size s1(100.0, 200.0);
    Size s2(0.0, 200.0);
    Size s3(100.0, 0.0);
    
    EXPECT_FALSE(s1.IsEmpty());
    EXPECT_TRUE(s2.IsEmpty());
    EXPECT_TRUE(s3.IsEmpty());
}

TEST(SizeTest, GetArea) {
    Size s(10.0, 20.0);
    EXPECT_DOUBLE_EQ(s.GetArea(), 200.0);
}

TEST(RectTest, DefaultConstructor) {
    Rect r;
    EXPECT_DOUBLE_EQ(r.x, 0.0);
    EXPECT_DOUBLE_EQ(r.y, 0.0);
    EXPECT_DOUBLE_EQ(r.w, 0.0);
    EXPECT_DOUBLE_EQ(r.h, 0.0);
}

TEST(RectTest, ParameterizedConstructor) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
    EXPECT_DOUBLE_EQ(r.w, 100.0);
    EXPECT_DOUBLE_EQ(r.h, 200.0);
}

TEST(RectTest, FromLTRB) {
    Rect r = Rect::FromLTRB(10.0, 20.0, 110.0, 220.0);
    EXPECT_DOUBLE_EQ(r.x, 10.0);
    EXPECT_DOUBLE_EQ(r.y, 20.0);
    EXPECT_DOUBLE_EQ(r.w, 100.0);
    EXPECT_DOUBLE_EQ(r.h, 200.0);
}

TEST(RectTest, Getters) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    
    EXPECT_DOUBLE_EQ(r.GetLeft(), 10.0);
    EXPECT_DOUBLE_EQ(r.GetTop(), 20.0);
    EXPECT_DOUBLE_EQ(r.GetRight(), 110.0);
    EXPECT_DOUBLE_EQ(r.GetBottom(), 220.0);
}

TEST(RectTest, ContainsPoint) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    
    EXPECT_TRUE(r.Contains(Point(50.0, 100.0)));
    EXPECT_TRUE(r.Contains(Point(10.0, 20.0)));
    EXPECT_FALSE(r.Contains(Point(5.0, 100.0)));
    EXPECT_FALSE(r.Contains(Point(150.0, 100.0)));
}

TEST(RectTest, ContainsRect) {
    Rect r1(0.0, 0.0, 200.0, 200.0);
    Rect r2(10.0, 10.0, 50.0, 50.0);
    Rect r3(150.0, 150.0, 100.0, 100.0);
    
    EXPECT_TRUE(r1.Contains(r2));
    EXPECT_FALSE(r1.Contains(r3));
}

TEST(RectTest, Intersects) {
    Rect r1(0.0, 0.0, 100.0, 100.0);
    Rect r2(50.0, 50.0, 100.0, 100.0);
    Rect r3(200.0, 200.0, 100.0, 100.0);
    
    EXPECT_TRUE(r1.Intersects(r2));
    EXPECT_FALSE(r1.Intersects(r3));
}

TEST(RectTest, Intersect) {
    Rect r1(0.0, 0.0, 100.0, 100.0);
    Rect r2(50.0, 50.0, 100.0, 100.0);
    Rect result = r1.Intersect(r2);
    
    EXPECT_DOUBLE_EQ(result.x, 50.0);
    EXPECT_DOUBLE_EQ(result.y, 50.0);
    EXPECT_DOUBLE_EQ(result.w, 50.0);
    EXPECT_DOUBLE_EQ(result.h, 50.0);
}

TEST(RectTest, Union) {
    Rect r1(0.0, 0.0, 100.0, 100.0);
    Rect r2(50.0, 50.0, 100.0, 100.0);
    Rect result = r1.Union(r2);
    
    EXPECT_DOUBLE_EQ(result.x, 0.0);
    EXPECT_DOUBLE_EQ(result.y, 0.0);
    EXPECT_DOUBLE_EQ(result.w, 150.0);
    EXPECT_DOUBLE_EQ(result.h, 150.0);
}

TEST(RectTest, Inflate) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    Rect result = r.Inflate(5.0, 10.0);
    
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 10.0);
    EXPECT_DOUBLE_EQ(result.w, 110.0);
    EXPECT_DOUBLE_EQ(result.h, 220.0);
}

TEST(RectTest, Offset) {
    Rect r(10.0, 20.0, 100.0, 200.0);
    Rect result = r.Offset(5.0, 10.0);
    
    EXPECT_DOUBLE_EQ(result.x, 15.0);
    EXPECT_DOUBLE_EQ(result.y, 30.0);
    EXPECT_DOUBLE_EQ(result.w, 100.0);
    EXPECT_DOUBLE_EQ(result.h, 200.0);
}
