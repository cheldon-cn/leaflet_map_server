#include <gtest/gtest.h>
#include "ogc/draw/font.h"

TEST(FontTest, DefaultConstructor) {
    ogc::draw::Font font;
    EXPECT_TRUE(font.IsValid());
    EXPECT_DOUBLE_EQ(font.GetSize(), 12.0);
    EXPECT_FALSE(font.IsBold());
    EXPECT_FALSE(font.IsItalic());
}

TEST(FontTest, ParameterizedConstructor) {
    ogc::draw::Font font("Times New Roman", 14.0);
    EXPECT_EQ(font.GetFamily(), "Times New Roman");
    EXPECT_DOUBLE_EQ(font.GetSize(), 14.0);
}

TEST(FontTest, ConstructorWithStyle) {
    ogc::draw::Font font("Arial", 16.0, ogc::draw::FontStyle::kBold);
    EXPECT_EQ(font.GetFamily(), "Arial");
    EXPECT_DOUBLE_EQ(font.GetSize(), 16.0);
    EXPECT_TRUE(font.IsBold());
}

TEST(FontTest, ConstructorWithWeight) {
    ogc::draw::Font font("Arial", 14.0, ogc::draw::FontWeight::kBold, true);
    EXPECT_EQ(font.GetFamily(), "Arial");
    EXPECT_DOUBLE_EQ(font.GetSize(), 14.0);
    EXPECT_TRUE(font.IsBold());
    EXPECT_TRUE(font.IsItalic());
}

TEST(FontTest, SetFamily) {
    ogc::draw::Font font;
    font.SetFamily("Courier New");
    EXPECT_EQ(font.GetFamily(), "Courier New");
}

TEST(FontTest, SetSize) {
    ogc::draw::Font font;
    font.SetSize(16.0);
    EXPECT_DOUBLE_EQ(font.GetSize(), 16.0);
}

TEST(FontTest, SetBold) {
    ogc::draw::Font font;
    font.SetBold(true);
    EXPECT_TRUE(font.IsBold());
}

TEST(FontTest, SetItalic) {
    ogc::draw::Font font;
    font.SetItalic(true);
    EXPECT_TRUE(font.IsItalic());
}

TEST(FontTest, SetWeight) {
    ogc::draw::Font font;
    font.SetWeight(ogc::draw::FontWeight::kBold);
    EXPECT_EQ(font.GetWeight(), ogc::draw::FontWeight::kBold);
}

TEST(FontTest, SetStyle) {
    ogc::draw::Font font;
    font.SetStyle(ogc::draw::FontStyle::kItalic);
    EXPECT_EQ(font.GetStyle(), ogc::draw::FontStyle::kItalic);
}

TEST(FontTest, GetStyleString) {
    ogc::draw::Font font("Arial", 12.0, ogc::draw::FontStyle::kBold);
    EXPECT_FALSE(font.GetStyleString().empty());
}

TEST(FontTest, GetWeightString) {
    ogc::draw::Font font("Arial", 12.0, ogc::draw::FontWeight::kBold);
    EXPECT_FALSE(font.GetWeightString().empty());
}

TEST(FontTest, ToString) {
    ogc::draw::Font font("Arial", 12.0);
    std::string str = font.ToString();
    EXPECT_FALSE(str.empty());
}

TEST(FontTest, DefaultFont) {
    ogc::draw::Font font = ogc::draw::Font::Default();
    EXPECT_TRUE(font.IsValid());
}

TEST(FontTest, Equality) {
    ogc::draw::Font font1("Arial", 12.0);
    ogc::draw::Font font2("Arial", 12.0);
    ogc::draw::Font font3("Arial", 14.0);
    
    EXPECT_TRUE(font1 == font2);
    EXPECT_FALSE(font1 == font3);
}

TEST(FontTest, GetLineHeight) {
    ogc::draw::Font font("Arial", 12.0);
    double lineHeight = font.GetLineHeight();
    EXPECT_GT(lineHeight, 0.0);
}
