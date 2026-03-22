#include <gtest/gtest.h>
#include "ogc/draw/color.h"

TEST(ColorTest, DefaultConstructor) {
    ogc::draw::Color color;
    EXPECT_EQ(color.GetRed(), 0);
    EXPECT_EQ(color.GetGreen(), 0);
    EXPECT_EQ(color.GetBlue(), 0);
    EXPECT_EQ(color.GetAlpha(), 255);
}

TEST(ColorTest, RGBConstructor) {
    ogc::draw::Color color(255, 128, 64);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
    EXPECT_EQ(color.GetAlpha(), 255);
}

TEST(ColorTest, RGBAConstructor) {
    ogc::draw::Color color(255, 128, 64, 200);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
    EXPECT_EQ(color.GetAlpha(), 200);
}

TEST(ColorTest, SetRed) {
    ogc::draw::Color color;
    color.SetRed(100);
    EXPECT_EQ(color.GetRed(), 100);
}

TEST(ColorTest, SetGreen) {
    ogc::draw::Color color;
    color.SetGreen(150);
    EXPECT_EQ(color.GetGreen(), 150);
}

TEST(ColorTest, SetBlue) {
    ogc::draw::Color color;
    color.SetBlue(200);
    EXPECT_EQ(color.GetBlue(), 200);
}

TEST(ColorTest, SetAlpha) {
    ogc::draw::Color color;
    color.SetAlpha(128);
    EXPECT_EQ(color.GetAlpha(), 128);
}

TEST(ColorTest, GetRGB) {
    ogc::draw::Color color(255, 128, 64);
    uint32_t rgb = color.GetRGB();
    EXPECT_EQ(rgb, 0xFF8040);
}

TEST(ColorTest, GetRGBA) {
    ogc::draw::Color color(255, 128, 64, 200);
    uint32_t rgba = color.GetRGBA();
    EXPECT_EQ(rgba, 0xFF8040C8);
}

TEST(ColorTest, FromRGB) {
    ogc::draw::Color color = ogc::draw::Color::FromRGB(255, 128, 64);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
    EXPECT_EQ(color.GetAlpha(), 255);
}

TEST(ColorTest, FromRGBA) {
    ogc::draw::Color color = ogc::draw::Color::FromRGBA(255, 128, 64, 200);
    EXPECT_EQ(color.GetRed(), 255);
    EXPECT_EQ(color.GetGreen(), 128);
    EXPECT_EQ(color.GetBlue(), 64);
    EXPECT_EQ(color.GetAlpha(), 200);
}

TEST(ColorTest, PredefinedColors) {
    ogc::draw::Color red = ogc::draw::Color::Red();
    EXPECT_EQ(red.GetRed(), 255);
    EXPECT_EQ(red.GetGreen(), 0);
    EXPECT_EQ(red.GetBlue(), 0);
    
    ogc::draw::Color green = ogc::draw::Color::Green();
    EXPECT_EQ(green.GetRed(), 0);
    EXPECT_EQ(green.GetGreen(), 255);
    EXPECT_EQ(green.GetBlue(), 0);
    
    ogc::draw::Color blue = ogc::draw::Color::Blue();
    EXPECT_EQ(blue.GetRed(), 0);
    EXPECT_EQ(blue.GetGreen(), 0);
    EXPECT_EQ(blue.GetBlue(), 255);
    
    ogc::draw::Color white = ogc::draw::Color::White();
    EXPECT_EQ(white.GetRed(), 255);
    EXPECT_EQ(white.GetGreen(), 255);
    EXPECT_EQ(white.GetBlue(), 255);
}

TEST(ColorTest, Equality) {
    ogc::draw::Color c1(255, 128, 64, 200);
    ogc::draw::Color c2(255, 128, 64, 200);
    ogc::draw::Color c3(255, 128, 64, 100);
    
    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);
    EXPECT_TRUE(c1 != c3);
}

TEST(ColorTest, WithAlpha) {
    ogc::draw::Color c1(255, 128, 64);
    ogc::draw::Color c2 = c1.WithAlpha(128);
    
    EXPECT_EQ(c2.GetRed(), 255);
    EXPECT_EQ(c2.GetGreen(), 128);
    EXPECT_EQ(c2.GetBlue(), 64);
    EXPECT_EQ(c2.GetAlpha(), 128);
}

TEST(ColorTest, Lighter) {
    ogc::draw::Color c(100, 100, 100);
    ogc::draw::Color lighter = c.Lighter(1.5);
    
    EXPECT_GT(lighter.GetRed(), c.GetRed());
    EXPECT_GT(lighter.GetGreen(), c.GetGreen());
    EXPECT_GT(lighter.GetBlue(), c.GetBlue());
}

TEST(ColorTest, Darker) {
    ogc::draw::Color c(200, 200, 200);
    ogc::draw::Color darker = c.Darker(0.7);
    
    EXPECT_LT(darker.GetRed(), c.GetRed());
    EXPECT_LT(darker.GetGreen(), c.GetGreen());
    EXPECT_LT(darker.GetBlue(), c.GetBlue());
}

TEST(ColorTest, ToGrayscale) {
    ogc::draw::Color c(255, 0, 0);
    ogc::draw::Color gray = c.ToGrayscale();
    
    EXPECT_EQ(gray.GetRed(), gray.GetGreen());
    EXPECT_EQ(gray.GetGreen(), gray.GetBlue());
}

TEST(ColorTest, IsTransparent) {
    ogc::draw::Color c1(255, 128, 64, 0);
    ogc::draw::Color c2(255, 128, 64, 255);
    
    EXPECT_TRUE(c1.IsTransparent());
    EXPECT_FALSE(c2.IsTransparent());
}

TEST(ColorTest, IsOpaque) {
    ogc::draw::Color c1(255, 128, 64, 255);
    ogc::draw::Color c2(255, 128, 64, 128);
    
    EXPECT_TRUE(c1.IsOpaque());
    EXPECT_FALSE(c2.IsOpaque());
}
