#include <gtest/gtest.h>
#include <ogc/symbology/style/s52_style_manager.h>

using namespace ogc::symbology;

class ColorSchemeManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(ColorSchemeManagerTest, Instance) {
    auto& instance = ColorSchemeManager::Instance();
    EXPECT_TRUE(true);
}

TEST_F(ColorSchemeManagerTest, SetDayNightMode) {
    auto& instance = ColorSchemeManager::Instance();
    
    instance.SetDayNightMode(DayNightMode::kDay);
    EXPECT_EQ(instance.GetDayNightMode(), DayNightMode::kDay);
    
    instance.SetDayNightMode(DayNightMode::kNight);
    EXPECT_EQ(instance.GetDayNightMode(), DayNightMode::kNight);
}

TEST_F(ColorSchemeManagerTest, GetDayColor) {
    auto& instance = ColorSchemeManager::Instance();
    
    Color dayColor = instance.GetDayColor("CHGRF");
    EXPECT_EQ(dayColor.GetRed(), 0);
    EXPECT_EQ(dayColor.GetGreen(), 0);
    EXPECT_EQ(dayColor.GetBlue(), 0);
    EXPECT_EQ(dayColor.GetAlpha(), 255);
}

TEST_F(ColorSchemeManagerTest, GetNightColor) {
    auto& instance = ColorSchemeManager::Instance();
    
    Color nightColor = instance.GetNightColor("CHGRF");
    EXPECT_EQ(nightColor.GetRed(), 139);
    EXPECT_EQ(nightColor.GetGreen(), 139);
    EXPECT_EQ(nightColor.GetBlue(), 0);
    EXPECT_EQ(nightColor.GetAlpha(), 255);
}

TEST_F(ColorSchemeManagerTest, GetColorDayMode) {
    auto& instance = ColorSchemeManager::Instance();
    instance.SetDayNightMode(DayNightMode::kDay);
    
    Color color = instance.GetColor("CHGRF");
    EXPECT_EQ(color.GetRed(), 0);
    EXPECT_EQ(color.GetGreen(), 0);
    EXPECT_EQ(color.GetBlue(), 0);
}

TEST_F(ColorSchemeManagerTest, GetColorNightMode) {
    auto& instance = ColorSchemeManager::Instance();
    instance.SetDayNightMode(DayNightMode::kNight);
    
    Color color = instance.GetColor("CHGRF");
    EXPECT_EQ(color.GetRed(), 139);
    EXPECT_EQ(color.GetGreen(), 139);
    EXPECT_EQ(color.GetBlue(), 0);
}

TEST_F(ColorSchemeManagerTest, RegisterColor) {
    auto& instance = ColorSchemeManager::Instance();
    
    Color dayColor(255, 0, 0, 255);
    Color nightColor(139, 0, 0, 255);
    
    instance.RegisterColor("TESTCOLOR", dayColor, nightColor);
    
    Color retrievedDay = instance.GetDayColor("TESTCOLOR");
    EXPECT_EQ(retrievedDay.GetRed(), 255);
    EXPECT_EQ(retrievedDay.GetGreen(), 0);
    EXPECT_EQ(retrievedDay.GetBlue(), 0);
    
    Color retrievedNight = instance.GetNightColor("TESTCOLOR");
    EXPECT_EQ(retrievedNight.GetRed(), 139);
    EXPECT_EQ(retrievedNight.GetGreen(), 0);
    EXPECT_EQ(retrievedNight.GetBlue(), 0);
}

TEST_F(ColorSchemeManagerTest, GetColorTokens) {
    auto& instance = ColorSchemeManager::Instance();
    auto tokens = instance.GetColorTokens();
    EXPECT_FALSE(tokens.empty());
}

TEST_F(ColorSchemeManagerTest, GetColorInvalidToken) {
    auto& instance = ColorSchemeManager::Instance();
    Color color = instance.GetColor("INVALID_TOKEN");
    EXPECT_EQ(color.GetRed(), 0);
    EXPECT_EQ(color.GetGreen(), 0);
    EXPECT_EQ(color.GetBlue(), 0);
    EXPECT_EQ(color.GetAlpha(), 255);
}

class SymbolLibraryTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SymbolLibraryTest, Instance) {
    auto& instance = SymbolLibrary::Instance();
    EXPECT_TRUE(true);
}

TEST_F(SymbolLibraryTest, GetSymbol) {
    auto& instance = SymbolLibrary::Instance();
    
    const SymbolDefinition* symbol = instance.GetSymbol("SNDMRK01");
    ASSERT_NE(symbol, nullptr);
    EXPECT_EQ(symbol->symbolId, "SNDMRK01");
    EXPECT_EQ(symbol->type, SymbolType::kVector);
}

TEST_F(SymbolLibraryTest, GetSymbolIds) {
    auto& instance = SymbolLibrary::Instance();
    auto ids = instance.GetSymbolIds();
    EXPECT_FALSE(ids.empty());
}

TEST_F(SymbolLibraryTest, RegisterSymbol) {
    auto& instance = SymbolLibrary::Instance();
    
    SymbolDefinition symbol;
    symbol.symbolId = "TESTSYMBOL";
    symbol.description = "Test symbol";
    symbol.type = SymbolType::kVector;
    symbol.displayPriority = 50;
    
    instance.RegisterSymbol(symbol);
    EXPECT_TRUE(instance.HasSymbol("TESTSYMBOL"));
    
    const SymbolDefinition* retrieved = instance.GetSymbol("TESTSYMBOL");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->symbolId, "TESTSYMBOL");
    EXPECT_EQ(retrieved->description, "Test symbol");
    
    instance.RemoveSymbol("TESTSYMBOL");
}

TEST_F(SymbolLibraryTest, RemoveSymbol) {
    auto& instance = SymbolLibrary::Instance();
    
    SymbolDefinition symbol;
    symbol.symbolId = "TEMPSYMBOL";
    symbol.description = "Temporary symbol";
    
    instance.RegisterSymbol(symbol);
    EXPECT_TRUE(instance.HasSymbol("TEMPSYMBOL"));
    
    bool removed = instance.RemoveSymbol("TEMPSYMBOL");
    EXPECT_TRUE(removed);
    EXPECT_FALSE(instance.HasSymbol("TEMPSYMBOL"));
}

TEST_F(SymbolLibraryTest, HasSymbol) {
    auto& instance = SymbolLibrary::Instance();
    EXPECT_TRUE(instance.HasSymbol("SNDMRK01"));
    EXPECT_FALSE(instance.HasSymbol("NONEXISTENT"));
}

TEST_F(SymbolLibraryTest, GetSymbolCount) {
    auto& instance = SymbolLibrary::Instance();
    size_t count = instance.GetSymbolCount();
    EXPECT_GT(count, 0);
}

TEST_F(SymbolLibraryTest, GetSymbolInvalid) {
    auto& instance = SymbolLibrary::Instance();
    const SymbolDefinition* symbol = instance.GetSymbol("INVALID_SYMBOL");
    EXPECT_EQ(symbol, nullptr);
}

class StyleRuleEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(StyleRuleEngineTest, Instance) {
    auto& instance = StyleRuleEngine::Instance();
    EXPECT_TRUE(true);
}

TEST_F(StyleRuleEngineTest, MatchRule) {
    auto& instance = StyleRuleEngine::Instance();
    
    std::map<std::string, std::string> attributes;
    const StyleRule* rule = instance.MatchRule("SOUNDG", attributes, 1.0);
    
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->featureType, "SOUNDG");
}

TEST_F(StyleRuleEngineTest, MatchRuleWithConditions) {
    auto& instance = StyleRuleEngine::Instance();
    
    StyleRule rule;
    rule.ruleId = "TEST_RULE_COND";
    rule.featureType = "TESTFEATURE";
    rule.displayPriority = 100;
    
    StyleCondition cond;
    cond.attributeName = "CAT";
    cond.op = ConditionOperator::kEqual;
    cond.value = "1";
    rule.conditions.push_back(cond);
    
    instance.RegisterRule(rule);
    
    std::map<std::string, std::string> matchingAttrs;
    matchingAttrs["CAT"] = "1";
    const StyleRule* matched = instance.MatchRule("TESTFEATURE", matchingAttrs, 1.0);
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(matched->ruleId, "TEST_RULE_COND");
    
    std::map<std::string, std::string> nonMatchingAttrs;
    nonMatchingAttrs["CAT"] = "2";
    const StyleRule* notMatched = instance.MatchRule("TESTFEATURE", nonMatchingAttrs, 1.0);
    EXPECT_EQ(notMatched, nullptr);
    
    instance.RemoveRule("TEST_RULE_COND");
}

TEST_F(StyleRuleEngineTest, GetRulesByFeatureType) {
    auto& instance = StyleRuleEngine::Instance();
    auto rules = instance.GetRulesByFeatureType("SOUNDG");
    EXPECT_FALSE(rules.empty());
}

TEST_F(StyleRuleEngineTest, GetAllRules) {
    auto& instance = StyleRuleEngine::Instance();
    auto rules = instance.GetAllRules();
    EXPECT_FALSE(rules.empty());
}

TEST_F(StyleRuleEngineTest, SetDisplayCategory) {
    auto& instance = StyleRuleEngine::Instance();
    
    instance.SetDisplayCategory(DisplayCategory::kBase);
    EXPECT_EQ(instance.GetDisplayCategory(), DisplayCategory::kBase);
    
    instance.SetDisplayCategory(DisplayCategory::kStandard);
    EXPECT_EQ(instance.GetDisplayCategory(), DisplayCategory::kStandard);
}

TEST_F(StyleRuleEngineTest, RegisterRule) {
    auto& instance = StyleRuleEngine::Instance();
    
    StyleRule rule;
    rule.ruleId = "TEST_RULE";
    rule.featureType = "TESTFEATURE";
    rule.displayPriority = 50;
    
    instance.RegisterRule(rule);
    EXPECT_TRUE(instance.HasRule("TEST_RULE"));
    
    instance.RemoveRule("TEST_RULE");
}

TEST_F(StyleRuleEngineTest, RemoveRule) {
    auto& instance = StyleRuleEngine::Instance();
    
    StyleRule rule;
    rule.ruleId = "TEMP_RULE";
    rule.featureType = "TEMPFEATURE";
    
    instance.RegisterRule(rule);
    EXPECT_TRUE(instance.HasRule("TEMP_RULE"));
    
    bool removed = instance.RemoveRule("TEMP_RULE");
    EXPECT_TRUE(removed);
    EXPECT_FALSE(instance.HasRule("TEMP_RULE"));
}

TEST_F(StyleRuleEngineTest, GetRuleCount) {
    auto& instance = StyleRuleEngine::Instance();
    size_t count = instance.GetRuleCount();
    EXPECT_GT(count, 0);
}

TEST_F(StyleRuleEngineTest, MatchRuleByScale) {
    auto& instance = StyleRuleEngine::Instance();
    
    StyleRule rule;
    rule.ruleId = "SCALE_RULE";
    rule.featureType = "SCALETEST";
    rule.minScale = 1000.0;
    rule.maxScale = 10000.0;
    rule.displayPriority = 100;
    
    instance.RegisterRule(rule);
    
    const StyleRule* matched = instance.MatchRule("SCALETEST", {}, 5000.0);
    ASSERT_NE(matched, nullptr);
    EXPECT_EQ(matched->ruleId, "SCALE_RULE");
    
    const StyleRule* notMatched = instance.MatchRule("SCALETEST", {}, 500.0);
    EXPECT_EQ(notMatched, nullptr);
    
    instance.RemoveRule("SCALE_RULE");
}

class DisplayPriorityCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(DisplayPriorityCalculatorTest, CalculatePriority) {
    std::map<std::string, std::string> attributes;
    int priority = DisplayPriorityCalculator::CalculatePriority("SOUNDG", nullptr, attributes);
    EXPECT_GT(priority, 0);
}

TEST_F(DisplayPriorityCalculatorTest, GetFeatureTypePriority) {
    int priority = DisplayPriorityCalculator::GetFeatureTypePriority("SOUNDG");
    EXPECT_EQ(priority, 100);
    
    priority = DisplayPriorityCalculator::GetFeatureTypePriority("WRECKS");
    EXPECT_EQ(priority, 95);
    
    priority = DisplayPriorityCalculator::GetFeatureTypePriority("UNKNOWN");
    EXPECT_EQ(priority, 0);
}

TEST_F(DisplayPriorityCalculatorTest, GetImportancePriority) {
    std::map<std::string, std::string> attributes;
    attributes["CAT"] = "1";
    
    int importance = DisplayPriorityCalculator::GetImportancePriority(attributes);
    EXPECT_GT(importance, 0);
}

TEST_F(DisplayPriorityCalculatorTest, GetImportancePriorityWithExpo) {
    std::map<std::string, std::string> attributes;
    attributes["CAT"] = "1";
    attributes["EXPO"] = "1";
    
    int importance = DisplayPriorityCalculator::GetImportancePriority(attributes);
    EXPECT_GT(importance, 0);
}

class S52StyleManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(S52StyleManagerTest, Instance) {
    auto& instance = S52StyleManager::Instance();
    EXPECT_TRUE(true);
}

TEST_F(S52StyleManagerTest, Initialize) {
    auto& instance = S52StyleManager::Instance();
    bool result = instance.Initialize();
    EXPECT_TRUE(result);
}

TEST_F(S52StyleManagerTest, SetDayNightMode) {
    auto& instance = S52StyleManager::Instance();
    
    instance.SetDayNightMode(DayNightMode::kDay);
    EXPECT_EQ(instance.GetDayNightMode(), DayNightMode::kDay);
    
    instance.SetDayNightMode(DayNightMode::kNight);
    EXPECT_EQ(instance.GetDayNightMode(), DayNightMode::kNight);
}

TEST_F(S52StyleManagerTest, SetDisplayCategory) {
    auto& instance = S52StyleManager::Instance();
    
    instance.SetDisplayCategory(DisplayCategory::kBase);
    EXPECT_EQ(instance.GetDisplayCategory(), DisplayCategory::kBase);
    
    instance.SetDisplayCategory(DisplayCategory::kStandard);
    EXPECT_EQ(instance.GetDisplayCategory(), DisplayCategory::kStandard);
}

TEST_F(S52StyleManagerTest, GetStyle) {
    auto& instance = S52StyleManager::Instance();
    instance.Initialize();
    
    std::map<std::string, std::string> attributes;
    const StyleRule* style = instance.GetStyle("SOUNDG", attributes, 1.0);
    
    ASSERT_NE(style, nullptr);
    EXPECT_EQ(style->featureType, "SOUNDG");
}

TEST_F(S52StyleManagerTest, GetSymbol) {
    auto& instance = S52StyleManager::Instance();
    instance.Initialize();
    
    const SymbolDefinition* symbol = instance.GetSymbol("SNDMRK01");
    ASSERT_NE(symbol, nullptr);
    EXPECT_EQ(symbol->symbolId, "SNDMRK01");
}

TEST_F(S52StyleManagerTest, GetColor) {
    auto& instance = S52StyleManager::Instance();
    instance.Initialize();
    
    instance.SetDayNightMode(DayNightMode::kDay);
    Color color = instance.GetColor("CHGRF");
    EXPECT_EQ(color.GetRed(), 0);
    EXPECT_EQ(color.GetGreen(), 0);
    EXPECT_EQ(color.GetBlue(), 0);
}

TEST_F(S52StyleManagerTest, CalculateDisplayPriority) {
    auto& instance = S52StyleManager::Instance();
    instance.Initialize();
    
    std::map<std::string, std::string> attributes;
    int priority = instance.CalculateDisplayPriority("SOUNDG", nullptr, attributes);
    EXPECT_GT(priority, 0);
}

TEST_F(S52StyleManagerTest, GetColorSchemeManager) {
    auto& instance = S52StyleManager::Instance();
    auto& colorManager = instance.GetColorSchemeManager();
    EXPECT_TRUE(true);
}

TEST_F(S52StyleManagerTest, GetSymbolLibrary) {
    auto& instance = S52StyleManager::Instance();
    auto& symbolLib = instance.GetSymbolLibrary();
    EXPECT_TRUE(true);
}

TEST_F(S52StyleManagerTest, GetRuleEngine) {
    auto& instance = S52StyleManager::Instance();
    auto& ruleEngine = instance.GetRuleEngine();
    EXPECT_TRUE(true);
}

TEST_F(S52StyleManagerTest, DayNightModeColorSwitch) {
    auto& instance = S52StyleManager::Instance();
    instance.Initialize();
    
    instance.SetDayNightMode(DayNightMode::kDay);
    Color dayColor = instance.GetColor("CHGRF");
    EXPECT_EQ(dayColor.GetRed(), 0);
    EXPECT_EQ(dayColor.GetGreen(), 0);
    EXPECT_EQ(dayColor.GetBlue(), 0);
    
    instance.SetDayNightMode(DayNightMode::kNight);
    Color nightColor = instance.GetColor("CHGRF");
    EXPECT_EQ(nightColor.GetRed(), 139);
    EXPECT_EQ(nightColor.GetGreen(), 139);
    EXPECT_EQ(nightColor.GetBlue(), 0);
}

TEST_F(S52StyleManagerTest, MultipleFeatureTypes) {
    auto& instance = S52StyleManager::Instance();
    instance.Initialize();
    
    std::map<std::string, std::string> attributes;
    
    const StyleRule* soundgStyle = instance.GetStyle("SOUNDG", attributes, 1.0);
    ASSERT_NE(soundgStyle, nullptr);
    EXPECT_EQ(soundgStyle->featureType, "SOUNDG");
    
    const StyleRule* wrecksStyle = instance.GetStyle("WRECKS", attributes, 1.0);
    ASSERT_NE(wrecksStyle, nullptr);
    EXPECT_EQ(wrecksStyle->featureType, "WRECKS");
    
    const StyleRule* lightsStyle = instance.GetStyle("LIGHTS", attributes, 1.0);
    ASSERT_NE(lightsStyle, nullptr);
    EXPECT_EQ(lightsStyle->featureType, "LIGHTS");
}
