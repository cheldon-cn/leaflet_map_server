#include <gtest/gtest.h>
#include "ogc/portrayal/parser/parser_registry.h"
#include "ogc/portrayal/parser/symbol_parser.h"
#include "ogc/portrayal/parser/line_style_parser.h"
#include "ogc/portrayal/parser/area_fill_parser.h"
#include "ogc/portrayal/parser/rule_file_parser.h"
#include "ogc/portrayal/parser/viewing_group_parser.h"
#include "ogc/portrayal/parser/alert_catalog_parser.h"
#include "ogc/portrayal/parser/color_profile_parser.h"
#include "ogc/portrayal/parser/style_sheet_parser.h"
#include "ogc/portrayal/parser/portrayal_catalog_parser.h"

using namespace ogc::portrayal::parser;

class ParserRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ParserRegistryTest, InstanceIsSingleton) {
    ParserRegistry& reg1 = ParserRegistry::Instance();
    ParserRegistry& reg2 = ParserRegistry::Instance();
    EXPECT_EQ(&reg1, &reg2);
}

TEST_F(ParserRegistryTest, HasAllDefaultParsers) {
    ParserRegistry& reg = ParserRegistry::Instance();
    EXPECT_TRUE(reg.HasParser("symbol"));
    EXPECT_TRUE(reg.HasParser("lineStyle"));
    EXPECT_TRUE(reg.HasParser("areaFill"));
    EXPECT_TRUE(reg.HasParser("ruleFile"));
    EXPECT_TRUE(reg.HasParser("viewingGroup"));
    EXPECT_TRUE(reg.HasParser("alertCatalog"));
    EXPECT_TRUE(reg.HasParser("colorProfile"));
    EXPECT_TRUE(reg.HasParser("styleSheet"));
}

TEST_F(ParserRegistryTest, GetParserReturnsCorrectType) {
    ParserRegistry& reg = ParserRegistry::Instance();
    ElementParser* parser = reg.GetParser("symbol");
    ASSERT_NE(parser, nullptr);
    EXPECT_EQ(parser->GetElementName(), "symbol");
}

TEST_F(ParserRegistryTest, GetParserReturnsNullForUnknown) {
    ParserRegistry& reg = ParserRegistry::Instance();
    ElementParser* parser = reg.GetParser("unknownElement");
    EXPECT_EQ(parser, nullptr);
}

TEST_F(ParserRegistryTest, GetRegisteredElementNames) {
    ParserRegistry& reg = ParserRegistry::Instance();
    auto names = reg.GetRegisteredElementNames();
    EXPECT_EQ(names.size(), 8u);
}

class SymbolParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(SymbolParserTest, GetElementName) {
    SymbolParser parser;
    EXPECT_EQ(parser.GetElementName(), "symbol");
}

TEST_F(SymbolParserTest, CanParse) {
    SymbolParser parser;
    EXPECT_TRUE(parser.CanParse("symbol"));
    EXPECT_FALSE(parser.CanParse("lineStyle"));
}

class LineStyleParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(LineStyleParserTest, GetElementName) {
    LineStyleParser parser;
    EXPECT_EQ(parser.GetElementName(), "lineStyle");
}

class AreaFillParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(AreaFillParserTest, GetElementName) {
    AreaFillParser parser;
    EXPECT_EQ(parser.GetElementName(), "areaFill");
}

class RuleFileParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(RuleFileParserTest, GetElementName) {
    RuleFileParser parser;
    EXPECT_EQ(parser.GetElementName(), "ruleFile");
}

class ViewingGroupParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(ViewingGroupParserTest, GetElementName) {
    ViewingGroupParser parser;
    EXPECT_EQ(parser.GetElementName(), "viewingGroup");
}

class AlertCatalogParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(AlertCatalogParserTest, GetElementName) {
    AlertCatalogParser parser;
    EXPECT_EQ(parser.GetElementName(), "alertCatalog");
}

class ColorProfileParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(ColorProfileParserTest, GetElementName) {
    ColorProfileParser parser;
    EXPECT_EQ(parser.GetElementName(), "colorProfile");
}

class StyleSheetParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(StyleSheetParserTest, GetElementName) {
    StyleSheetParser parser;
    EXPECT_EQ(parser.GetElementName(), "styleSheet");
}

class PortrayalCatalogParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(PortrayalCatalogParserTest, ParseNonExistentFile) {
    PortrayalCatalogParser parser;
    ogc::portrayal::model::PortrayalCatalog catalog;
    bool result = parser.Parse("/nonexistent/file.xml", catalog);
    EXPECT_FALSE(result);
    EXPECT_FALSE(parser.GetLastError().empty());
}
