#include <gtest/gtest.h>
#include "ogc/portrayal/model/description.h"
#include "ogc/portrayal/model/file_reference.h"
#include "ogc/portrayal/model/symbol.h"
#include "ogc/portrayal/model/line_style.h"
#include "ogc/portrayal/model/area_fill.h"
#include "ogc/portrayal/model/style_sheet.h"
#include "ogc/portrayal/model/alert_catalog.h"
#include "ogc/portrayal/model/color_profile.h"
#include "ogc/portrayal/model/rule_file.h"
#include "ogc/portrayal/model/viewing_group.h"
#include "ogc/portrayal/model/portrayal_catalog.h"

using namespace ogc::portrayal::model;

class DescriptionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DescriptionTest, DefaultConstructor) {
    Description desc;
    EXPECT_EQ(desc.GetName(), "");
    EXPECT_EQ(desc.GetDescription(), "");
    EXPECT_EQ(desc.GetLanguage(), "");
}

TEST_F(DescriptionTest, ParameterizedConstructor) {
    Description desc("testName", "testDesc", "eng");
    EXPECT_EQ(desc.GetName(), "testName");
    EXPECT_EQ(desc.GetDescription(), "testDesc");
    EXPECT_EQ(desc.GetLanguage(), "eng");
}

TEST_F(DescriptionTest, Setters) {
    Description desc;
    desc.SetName("newName");
    desc.SetDescription("newDesc");
    desc.SetLanguage("chi");
    EXPECT_EQ(desc.GetName(), "newName");
    EXPECT_EQ(desc.GetDescription(), "newDesc");
    EXPECT_EQ(desc.GetLanguage(), "chi");
}

class FileReferenceTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FileReferenceTest, DefaultState) {
    Symbol symbol;
    EXPECT_EQ(symbol.GetId(), "");
    EXPECT_EQ(symbol.GetFileName(), "");
    EXPECT_EQ(symbol.GetFileType(), "");
    EXPECT_EQ(symbol.GetFileFormat(), "");
}

TEST_F(FileReferenceTest, Setters) {
    Symbol symbol;
    symbol.SetId("SYM001");
    symbol.SetFileName("symbol.svg");
    symbol.SetFileType("SVG");
    symbol.SetFileFormat("vector");
    EXPECT_EQ(symbol.GetId(), "SYM001");
    EXPECT_EQ(symbol.GetFileName(), "symbol.svg");
    EXPECT_EQ(symbol.GetFileType(), "SVG");
    EXPECT_EQ(symbol.GetFileFormat(), "vector");
}

TEST_F(FileReferenceTest, DescriptionInSymbol) {
    Symbol symbol;
    Description desc("buoy", "buoy symbol", "eng");
    symbol.SetDescription(desc);
    EXPECT_EQ(symbol.GetDescription().GetName(), "buoy");
    EXPECT_EQ(symbol.GetDescription().GetDescription(), "buoy symbol");
}

class LineStyleTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(LineStyleTest, DefaultState) {
    LineStyle style;
    EXPECT_EQ(style.GetId(), "");
}

TEST_F(LineStyleTest, Setters) {
    LineStyle style;
    style.SetId("LS001");
    style.SetFileName("line.svg");
    EXPECT_EQ(style.GetId(), "LS001");
    EXPECT_EQ(style.GetFileName(), "line.svg");
}

class AreaFillTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(AreaFillTest, DefaultState) {
    AreaFill fill;
    EXPECT_EQ(fill.GetId(), "");
}

TEST_F(AreaFillTest, Setters) {
    AreaFill fill;
    fill.SetId("AF001");
    fill.SetFileName("area.svg");
    EXPECT_EQ(fill.GetId(), "AF001");
    EXPECT_EQ(fill.GetFileName(), "area.svg");
}

class RuleFileTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(RuleFileTest, DefaultState) {
    RuleFile rule;
    EXPECT_EQ(rule.GetId(), "");
    EXPECT_EQ(rule.GetRuleType(), "");
}

TEST_F(RuleFileTest, Setters) {
    RuleFile rule;
    rule.SetId("RF001");
    rule.SetRuleType("conditional");
    rule.SetFileName("rules.xml");
    EXPECT_EQ(rule.GetId(), "RF001");
    EXPECT_EQ(rule.GetRuleType(), "conditional");
    EXPECT_EQ(rule.GetFileName(), "rules.xml");
}

class ViewingGroupTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(ViewingGroupTest, DefaultState) {
    ViewingGroup group;
    EXPECT_EQ(group.GetId(), "");
}

TEST_F(ViewingGroupTest, Setters) {
    ViewingGroup group;
    group.SetId("VG001");
    Description desc("base", "base display", "eng");
    group.SetDescription(desc);
    EXPECT_EQ(group.GetId(), "VG001");
    EXPECT_EQ(group.GetDescription().GetName(), "base");
}

class PortrayalCatalogTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(PortrayalCatalogTest, DefaultState) {
    PortrayalCatalog catalog;
    EXPECT_EQ(catalog.GetProductId(), "");
    EXPECT_EQ(catalog.GetVersion(), "");
    EXPECT_EQ(catalog.GetSymbols().size(), 0u);
    EXPECT_EQ(catalog.GetLineStyles().size(), 0u);
    EXPECT_EQ(catalog.GetAreaFills().size(), 0u);
    EXPECT_EQ(catalog.GetViewingGroups().size(), 0u);
    EXPECT_EQ(catalog.GetRuleFiles().size(), 0u);
    EXPECT_EQ(catalog.GetColorProfiles().size(), 0u);
    EXPECT_EQ(catalog.GetStyleSheets().size(), 0u);
}

TEST_F(PortrayalCatalogTest, SetProductIdAndVersion) {
    PortrayalCatalog catalog;
    catalog.SetProductId("S-401");
    catalog.SetVersion("1.0.0");
    EXPECT_EQ(catalog.GetProductId(), "S-401");
    EXPECT_EQ(catalog.GetVersion(), "1.0.0");
}

TEST_F(PortrayalCatalogTest, AddAndFindSymbol) {
    PortrayalCatalog catalog;
    Symbol symbol;
    symbol.SetId("SYM001");
    symbol.SetFileName("test.svg");
    catalog.AddSymbol(symbol);

    EXPECT_EQ(catalog.GetSymbols().size(), 1u);
    const Symbol* found = catalog.FindSymbol("SYM001");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->GetFileName(), "test.svg");

    EXPECT_EQ(catalog.FindSymbol("NONEXIST"), nullptr);
}

TEST_F(PortrayalCatalogTest, AddAndFindLineStyle) {
    PortrayalCatalog catalog;
    LineStyle style;
    style.SetId("LS001");
    catalog.AddLineStyle(style);

    EXPECT_EQ(catalog.GetLineStyles().size(), 1u);
    const LineStyle* found = catalog.FindLineStyle("LS001");
    ASSERT_NE(found, nullptr);
}

TEST_F(PortrayalCatalogTest, AddAndFindAreaFill) {
    PortrayalCatalog catalog;
    AreaFill fill;
    fill.SetId("AF001");
    catalog.AddAreaFill(fill);

    EXPECT_EQ(catalog.GetAreaFills().size(), 1u);
    const AreaFill* found = catalog.FindAreaFill("AF001");
    ASSERT_NE(found, nullptr);
}

TEST_F(PortrayalCatalogTest, AddAndFindRuleFile) {
    PortrayalCatalog catalog;
    RuleFile rule;
    rule.SetId("RF001");
    catalog.AddRuleFile(rule);

    EXPECT_EQ(catalog.GetRuleFiles().size(), 1u);
    const RuleFile* found = catalog.FindRuleFile("RF001");
    ASSERT_NE(found, nullptr);
}

TEST_F(PortrayalCatalogTest, AddAndFindViewingGroup) {
    PortrayalCatalog catalog;
    ViewingGroup group;
    group.SetId("VG001");
    catalog.AddViewingGroup(group);

    EXPECT_EQ(catalog.GetViewingGroups().size(), 1u);
    const ViewingGroup* found = catalog.FindViewingGroup("VG001");
    ASSERT_NE(found, nullptr);
}

TEST_F(PortrayalCatalogTest, SetAlertCatalog) {
    PortrayalCatalog catalog;
    AlertCatalog alertCatalog;
    alertCatalog.SetId("AC001");
    catalog.SetAlertCatalog(alertCatalog);

    EXPECT_EQ(catalog.GetAlertCatalog().GetId(), "AC001");
}

TEST_F(PortrayalCatalogTest, AddColorProfile) {
    PortrayalCatalog catalog;
    ColorProfile profile;
    profile.SetId("CP001");
    catalog.AddColorProfile(profile);

    EXPECT_EQ(catalog.GetColorProfiles().size(), 1u);
    EXPECT_EQ(catalog.GetColorProfiles()[0].GetId(), "CP001");
}

TEST_F(PortrayalCatalogTest, AddStyleSheet) {
    PortrayalCatalog catalog;
    StyleSheet sheet;
    sheet.SetId("SS001");
    catalog.AddStyleSheet(sheet);

    EXPECT_EQ(catalog.GetStyleSheets().size(), 1u);
    EXPECT_EQ(catalog.GetStyleSheets()[0].GetId(), "SS001");
}

TEST_F(PortrayalCatalogTest, MultipleSymbolsIndex) {
    PortrayalCatalog catalog;
    for (int i = 0; i < 10; ++i) {
        Symbol symbol;
        symbol.SetId("SYM" + std::to_string(i));
        catalog.AddSymbol(symbol);
    }
    EXPECT_EQ(catalog.GetSymbols().size(), 10u);
    EXPECT_NE(catalog.FindSymbol("SYM5"), nullptr);
    EXPECT_EQ(catalog.FindSymbol("SYM99"), nullptr);
}
