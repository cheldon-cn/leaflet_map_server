#include <gtest/gtest.h>
#include "ogc/draw/color.h"

using namespace ogc::draw;

class ColorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ColorTest, DefaultConstructor) {
    Color c;
    EXPECT_EQ(c.GetRed(), 0);
    EXPECT_EQ(c.GetGreen(), 0);
    EXPECT_EQ(c.GetBlue(), 0);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST_F(ColorTest, RGBConstructor) {
    Color c(100, 150, 200);
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST_F(ColorTest, RGBAConstructor) {
    Color c(100, 150, 200, 128);
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 128);
}

TEST_F(ColorTest, UInt32Constructor) {
    Color c(0x6496C880);
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 128);
}

TEST_F(ColorTest, SettersAndGetters) {
    Color c;
    c.SetRed(50);
    c.SetGreen(100);
    c.SetBlue(150);
    c.SetAlpha(200);
    
    EXPECT_EQ(c.GetRed(), 50);
    EXPECT_EQ(c.GetGreen(), 100);
    EXPECT_EQ(c.GetBlue(), 150);
    EXPECT_EQ(c.GetAlpha(), 200);
}

TEST_F(ColorTest, FloatSettersAndGetters) {
    Color c;
    c.SetRedF(0.5);
    c.SetGreenF(0.25);
    c.SetBlueF(0.75);
    c.SetAlphaF(0.5);
    
    EXPECT_NEAR(c.GetRedF(), 0.5, 0.01);
    EXPECT_NEAR(c.GetGreenF(), 0.25, 0.01);
    EXPECT_NEAR(c.GetBlueF(), 0.75, 0.01);
    EXPECT_NEAR(c.GetAlphaF(), 0.5, 0.01);
}

TEST_F(ColorTest, GetSetARGB) {
    Color c;
    c.SetARGB(0x806496C8);
    
    EXPECT_EQ(c.GetAlpha(), 128);
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    
    EXPECT_EQ(c.GetARGB(), 0x806496C8);
}

TEST_F(ColorTest, GetSetRGBA) {
    Color c;
    c.SetRGBA(0x6496C880);
    
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 128);
    
    EXPECT_EQ(c.GetRGBA(), 0x6496C880);
}

TEST_F(ColorTest, GetSetRGB) {
    Color c;
    c.SetRGB(0x6496C8);
    
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 255);
    
    EXPECT_EQ(c.GetRGB(), 0x6496C8);
}

TEST_F(ColorTest, HSVConversion) {
    Color c = Color::FromHSV(120.0, 1.0, 1.0);
    
    double h, s, v;
    c.GetHSV(h, s, v);
    
    EXPECT_NEAR(h, 120.0, 1.0);
    EXPECT_NEAR(s, 1.0, 0.01);
    EXPECT_NEAR(v, 1.0, 0.01);
}

TEST_F(ColorTest, HSLConversion) {
    Color c = Color::FromHSL(240.0, 1.0, 0.5);
    
    double h, s, l;
    c.GetHSL(h, s, l);
    
    EXPECT_NEAR(h, 240.0, 1.0);
    EXPECT_NEAR(s, 1.0, 0.01);
    EXPECT_NEAR(l, 0.5, 0.01);
}

TEST_F(ColorTest, WithAlpha) {
    Color c(100, 150, 200, 255);
    Color c2 = c.WithAlpha(128);
    
    EXPECT_EQ(c2.GetRed(), 100);
    EXPECT_EQ(c2.GetGreen(), 150);
    EXPECT_EQ(c2.GetBlue(), 200);
    EXPECT_EQ(c2.GetAlpha(), 128);
}

TEST_F(ColorTest, WithAlphaF) {
    Color c(100, 150, 200, 255);
    Color c2 = c.WithAlphaF(0.5);
    
    EXPECT_EQ(c2.GetRed(), 100);
    EXPECT_EQ(c2.GetGreen(), 150);
    EXPECT_EQ(c2.GetBlue(), 200);
    EXPECT_NEAR(c2.GetAlphaF(), 0.5, 0.01);
}

TEST_F(ColorTest, Lighter) {
    Color c(100, 100, 100, 255);
    Color lighter = c.Lighter(1.5);
    
    EXPECT_GT(lighter.GetRed(), c.GetRed());
    EXPECT_GT(lighter.GetGreen(), c.GetGreen());
    EXPECT_GT(lighter.GetBlue(), c.GetBlue());
}

TEST_F(ColorTest, Darker) {
    Color c(200, 200, 200, 255);
    Color darker = c.Darker(0.5);
    
    EXPECT_LT(darker.GetRed(), c.GetRed());
    EXPECT_LT(darker.GetGreen(), c.GetGreen());
    EXPECT_LT(darker.GetBlue(), c.GetBlue());
}

TEST_F(ColorTest, ToGrayscale) {
    Color c(100, 150, 200, 255);
    Color gray = c.ToGrayscale();
    
    EXPECT_EQ(gray.GetRed(), gray.GetGreen());
    EXPECT_EQ(gray.GetGreen(), gray.GetBlue());
    EXPECT_EQ(gray.GetAlpha(), 255);
}

TEST_F(ColorTest, ToInverted) {
    Color c(100, 150, 200, 128);
    Color inverted = c.ToInverted();
    
    EXPECT_EQ(inverted.GetRed(), 155);
    EXPECT_EQ(inverted.GetGreen(), 105);
    EXPECT_EQ(inverted.GetBlue(), 55);
    EXPECT_EQ(inverted.GetAlpha(), 128);
}

TEST_F(ColorTest, Blend) {
    Color c1(0, 0, 0, 255);
    Color c2(255, 255, 255, 255);
    Color blended = c1.Blend(c2, 0.5);
    
    EXPECT_NEAR(blended.GetRed(), 128, 1);
    EXPECT_NEAR(blended.GetGreen(), 128, 1);
    EXPECT_NEAR(blended.GetBlue(), 128, 1);
}

TEST_F(ColorTest, IsTransparent) {
    Color c1(0, 0, 0, 0);
    Color c2(0, 0, 0, 255);
    
    EXPECT_TRUE(c1.IsTransparent());
    EXPECT_FALSE(c2.IsTransparent());
}

TEST_F(ColorTest, IsOpaque) {
    Color c1(0, 0, 0, 255);
    Color c2(0, 0, 0, 128);
    
    EXPECT_TRUE(c1.IsOpaque());
    EXPECT_FALSE(c2.IsOpaque());
}

TEST_F(ColorTest, IsValid) {
    Color c;
    EXPECT_TRUE(c.IsValid());
}

TEST_F(ColorTest, ToString) {
    Color c(100, 150, 200, 255);
    std::string str = c.ToString();
    
    EXPECT_TRUE(str.find("100") != std::string::npos);
    EXPECT_TRUE(str.find("150") != std::string::npos);
    EXPECT_TRUE(str.find("200") != std::string::npos);
    EXPECT_TRUE(str.find("255") != std::string::npos);
}

TEST_F(ColorTest, ToHexString) {
    Color c(100, 150, 200, 128);
    
    EXPECT_EQ(c.ToHexString(false), "#6496c8");
    EXPECT_EQ(c.ToHexString(true), "#806496c8");
}

TEST_F(ColorTest, ToRGBString) {
    Color c(100, 150, 200, 128);
    std::string str = c.ToRGBString();
    
    EXPECT_TRUE(str.find("100") != std::string::npos);
    EXPECT_TRUE(str.find("150") != std::string::npos);
    EXPECT_TRUE(str.find("200") != std::string::npos);
}

TEST_F(ColorTest, ToRGBAString) {
    Color c(100, 150, 200, 128);
    std::string str = c.ToRGBAString();
    
    EXPECT_TRUE(str.find("100") != std::string::npos);
    EXPECT_TRUE(str.find("150") != std::string::npos);
    EXPECT_TRUE(str.find("200") != std::string::npos);
    EXPECT_TRUE(str.find("0.5") != std::string::npos);
}

TEST_F(ColorTest, FromHexString) {
    Color c1 = Color::FromHexString("#6496c8");
    EXPECT_EQ(c1.GetRed(), 100);
    EXPECT_EQ(c1.GetGreen(), 150);
    EXPECT_EQ(c1.GetBlue(), 200);
    
    Color c2 = Color::FromHexString("6496c8");
    EXPECT_EQ(c2.GetRed(), 100);
    EXPECT_EQ(c2.GetGreen(), 150);
    EXPECT_EQ(c2.GetBlue(), 200);
    
    Color c3 = Color::FromHexString("#806496c8");
    EXPECT_EQ(c3.GetRed(), 100);
    EXPECT_EQ(c3.GetGreen(), 150);
    EXPECT_EQ(c3.GetBlue(), 200);
    EXPECT_EQ(c3.GetAlpha(), 128);
}

TEST_F(ColorTest, FromHex) {
    Color c = Color::FromHex(0x6496C8);
    
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST_F(ColorTest, FromHexWithAlpha) {
    Color c = Color::FromHexWithAlpha(0x6496C880);
    
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 128);
}

TEST_F(ColorTest, ToHex) {
    Color c(100, 150, 200, 128);
    EXPECT_EQ(c.ToHex(), 0x6496C8);
}

TEST_F(ColorTest, ToHexWithAlpha) {
    Color c(100, 150, 200, 128);
    EXPECT_EQ(c.ToHexWithAlpha(), 0x6496C880);
}

TEST_F(ColorTest, PredefinedColors) {
    EXPECT_EQ(Color::Transparent(), Color(0, 0, 0, 0));
    EXPECT_EQ(Color::Black(), Color(0, 0, 0, 255));
    EXPECT_EQ(Color::White(), Color(255, 255, 255, 255));
    EXPECT_EQ(Color::Red(), Color(255, 0, 0, 255));
    EXPECT_EQ(Color::Green(), Color(0, 255, 0, 255));
    EXPECT_EQ(Color::Blue(), Color(0, 0, 255, 255));
    EXPECT_EQ(Color::Yellow(), Color(255, 255, 0, 255));
    EXPECT_EQ(Color::Cyan(), Color(0, 255, 255, 255));
    EXPECT_EQ(Color::Magenta(), Color(255, 0, 255, 255));
    EXPECT_EQ(Color::Gray(), Color(128, 128, 128, 255));
    EXPECT_EQ(Color::LightGray(), Color(192, 192, 192, 255));
    EXPECT_EQ(Color::DarkGray(), Color(64, 64, 64, 255));
}

TEST_F(ColorTest, Equality) {
    Color c1(100, 150, 200, 255);
    Color c2(100, 150, 200, 255);
    Color c3(100, 150, 200, 128);
    
    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);
    EXPECT_TRUE(c1 != c3);
    EXPECT_FALSE(c1 != c2);
}

TEST_F(ColorTest, FromRGBStatic) {
    Color c = Color::FromRGB(100, 150, 200);
    
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 255);
}

TEST_F(ColorTest, FromRGBAStatic) {
    Color c = Color::FromRGBA(100, 150, 200, 128);
    
    EXPECT_EQ(c.GetRed(), 100);
    EXPECT_EQ(c.GetGreen(), 150);
    EXPECT_EQ(c.GetBlue(), 200);
    EXPECT_EQ(c.GetAlpha(), 128);
}
