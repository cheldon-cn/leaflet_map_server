#include <gtest/gtest.h>
#include "ogc/graph/util/day_night_mode_manager.h"

using namespace ogc::graph;
using ogc::draw::Color;

class DayNightModeManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = DayNightModeManager::Create();
    }
    
    void TearDown() override {
        manager.reset();
    }
    
    std::unique_ptr<DayNightModeManager> manager;
};

TEST_F(DayNightModeManagerTest, CreateManager) {
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDay);
}

TEST_F(DayNightModeManagerTest, SetDayMode) {
    manager->SetNightMode();
    EXPECT_TRUE(manager->IsNightMode());
    
    manager->SetDayMode();
    EXPECT_TRUE(manager->IsDayMode());
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDay);
}

TEST_F(DayNightModeManagerTest, SetNightMode) {
    manager->SetNightMode();
    EXPECT_TRUE(manager->IsNightMode());
    EXPECT_EQ(manager->GetMode(), DisplayMode::kNight);
}

TEST_F(DayNightModeManagerTest, SetDuskMode) {
    manager->SetDuskMode();
    EXPECT_TRUE(manager->IsDuskMode());
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDusk);
}

TEST_F(DayNightModeManagerTest, SetDawnMode) {
    manager->SetDawnMode();
    EXPECT_TRUE(manager->IsDawnMode());
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDawn);
}

TEST_F(DayNightModeManagerTest, SetMode) {
    manager->SetMode(DisplayMode::kNight);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kNight);
    
    manager->SetMode(DisplayMode::kDusk);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDusk);
    
    manager->SetMode(DisplayMode::kDawn);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDawn);
    
    manager->SetMode(DisplayMode::kDay);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDay);
}

TEST_F(DayNightModeManagerTest, GetCurrentScheme) {
    const ColorScheme& scheme = manager->GetCurrentScheme();
    EXPECT_EQ(scheme.name, "Day");
}

TEST_F(DayNightModeManagerTest, GetDayScheme) {
    const ColorScheme& scheme = manager->GetDayScheme();
    EXPECT_EQ(scheme.name, "Day");
    EXPECT_EQ(scheme.backgroundColor.GetRed(), 255);
    EXPECT_EQ(scheme.backgroundColor.GetGreen(), 255);
    EXPECT_EQ(scheme.backgroundColor.GetBlue(), 255);
}

TEST_F(DayNightModeManagerTest, GetNightScheme) {
    const ColorScheme& scheme = manager->GetNightScheme();
    EXPECT_EQ(scheme.name, "Night");
    EXPECT_LT(scheme.backgroundColor.GetRed(), 100);
    EXPECT_LT(scheme.backgroundColor.GetGreen(), 100);
    EXPECT_LT(scheme.backgroundColor.GetBlue(), 100);
}

TEST_F(DayNightModeManagerTest, SetDayScheme) {
    ColorScheme customScheme;
    customScheme.name = "CustomDay";
    customScheme.backgroundColor = Color(240, 240, 240);
    
    manager->SetDayScheme(customScheme);
    const ColorScheme& scheme = manager->GetDayScheme();
    EXPECT_EQ(scheme.name, "CustomDay");
    EXPECT_EQ(scheme.backgroundColor.GetRed(), 240);
}

TEST_F(DayNightModeManagerTest, SetNightScheme) {
    ColorScheme customScheme;
    customScheme.name = "CustomNight";
    customScheme.backgroundColor = Color(10, 10, 20);
    
    manager->SetNightScheme(customScheme);
    const ColorScheme& scheme = manager->GetNightScheme();
    EXPECT_EQ(scheme.name, "CustomNight");
    EXPECT_EQ(scheme.backgroundColor.GetRed(), 10);
}

TEST_F(DayNightModeManagerTest, RegisterColorScheme) {
    ColorScheme customScheme;
    customScheme.name = "HighContrast";
    customScheme.backgroundColor = Color(0, 0, 0);
    customScheme.textColor = Color(255, 255, 255);
    
    manager->RegisterColorScheme("HighContrast", customScheme);
    EXPECT_TRUE(manager->HasColorScheme("HighContrast"));
    
    ColorScheme retrieved = manager->GetColorScheme("HighContrast");
    EXPECT_EQ(retrieved.name, "HighContrast");
}

TEST_F(DayNightModeManagerTest, GetAvailableSchemes) {
    ColorScheme scheme1;
    scheme1.name = "Scheme1";
    ColorScheme scheme2;
    scheme2.name = "Scheme2";
    
    manager->RegisterColorScheme("Scheme1", scheme1);
    manager->RegisterColorScheme("Scheme2", scheme2);
    
    auto schemes = manager->GetAvailableSchemes();
    EXPECT_EQ(schemes.size(), 2u);
}

TEST_F(DayNightModeManagerTest, SetCustomMode) {
    ColorScheme customScheme;
    customScheme.name = "Custom";
    manager->RegisterColorScheme("Custom", customScheme);
    
    manager->SetCustomMode("Custom");
    EXPECT_TRUE(manager->IsCustomMode());
    EXPECT_EQ(manager->GetMode(), DisplayMode::kCustom);
}

TEST_F(DayNightModeManagerTest, SetCustomModeNonExistent) {
    manager->SetCustomMode("NonExistent");
    EXPECT_FALSE(manager->IsCustomMode());
}

TEST_F(DayNightModeManagerTest, SetTransitionDuration) {
    manager->SetTransitionDuration(2.5);
    EXPECT_DOUBLE_EQ(manager->GetTransitionDuration(), 2.5);
    
    manager->SetTransitionDuration(-1.0);
    EXPECT_DOUBLE_EQ(manager->GetTransitionDuration(), 0.0);
    
    manager->SetTransitionDuration(15.0);
    EXPECT_DOUBLE_EQ(manager->GetTransitionDuration(), 10.0);
}

TEST_F(DayNightModeManagerTest, StartTransition) {
    manager->StartTransition(DisplayMode::kNight);
    EXPECT_TRUE(manager->IsTransitioning());
    EXPECT_EQ(manager->GetTransitionProgress(), 0.0);
}

TEST_F(DayNightModeManagerTest, UpdateTransition) {
    manager->SetTransitionDuration(1.0);
    manager->StartTransition(DisplayMode::kNight);
    
    manager->UpdateTransition(0.5);
    EXPECT_TRUE(manager->IsTransitioning());
    EXPECT_DOUBLE_EQ(manager->GetTransitionProgress(), 0.5);
    
    manager->UpdateTransition(0.5);
    EXPECT_FALSE(manager->IsTransitioning());
    EXPECT_DOUBLE_EQ(manager->GetTransitionProgress(), 1.0);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kNight);
}

TEST_F(DayNightModeManagerTest, TransitionComplete) {
    manager->SetTransitionDuration(1.0);
    manager->StartTransition(DisplayMode::kNight);
    
    manager->UpdateTransition(1.0);
    EXPECT_FALSE(manager->IsTransitioning());
    EXPECT_EQ(manager->GetMode(), DisplayMode::kNight);
}

TEST_F(DayNightModeManagerTest, SetAutoModeSwitch) {
    manager->SetAutoModeSwitch(true);
    EXPECT_TRUE(manager->IsAutoModeSwitchEnabled());
    
    manager->SetAutoModeSwitch(false);
    EXPECT_FALSE(manager->IsAutoModeSwitchEnabled());
}

TEST_F(DayNightModeManagerTest, UpdateByTimeDay) {
    manager->SetAutoModeSwitch(true);
    manager->SetDayTimeRange(6, 18);
    
    manager->UpdateByTime(10);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDay);
}

TEST_F(DayNightModeManagerTest, UpdateByTimeNight) {
    manager->SetAutoModeSwitch(true);
    manager->SetNightTimeRange(20, 6);
    
    manager->UpdateByTime(22);
    EXPECT_TRUE(manager->IsTransitioning() || manager->GetMode() == DisplayMode::kNight);
}

TEST_F(DayNightModeManagerTest, TransformColorDay) {
    Color original(100, 150, 200);
    Color transformed = manager->TransformColor(original);
    EXPECT_EQ(transformed.GetRed(), original.GetRed());
    EXPECT_EQ(transformed.GetGreen(), original.GetGreen());
    EXPECT_EQ(transformed.GetBlue(), original.GetBlue());
}

TEST_F(DayNightModeManagerTest, TransformColorNight) {
    manager->SetNightMode();
    Color original(100, 150, 200);
    Color transformed = manager->TransformColor(original);
    EXPECT_LT(transformed.GetRed(), original.GetRed());
    EXPECT_LT(transformed.GetGreen(), original.GetGreen());
}

TEST_F(DayNightModeManagerTest, TransformColorForMode) {
    Color original(100, 100, 100);
    
    Color dayColor = manager->TransformColorForMode(original, DisplayMode::kDay);
    EXPECT_EQ(dayColor.GetRed(), original.GetRed());
    
    Color nightColor = manager->TransformColorForMode(original, DisplayMode::kNight);
    EXPECT_LT(nightColor.GetRed(), original.GetRed());
}

TEST_F(DayNightModeManagerTest, SetContrast) {
    manager->SetContrast(1.5);
    EXPECT_DOUBLE_EQ(manager->GetContrast(), 1.5);
    
    manager->SetContrast(-0.5);
    EXPECT_DOUBLE_EQ(manager->GetContrast(), 0.0);
    
    manager->SetContrast(3.0);
    EXPECT_DOUBLE_EQ(manager->GetContrast(), 2.0);
}

TEST_F(DayNightModeManagerTest, SetBrightness) {
    manager->SetBrightness(0.8);
    EXPECT_DOUBLE_EQ(manager->GetBrightness(), 0.8);
    
    manager->SetBrightness(-0.5);
    EXPECT_DOUBLE_EQ(manager->GetBrightness(), 0.0);
    
    manager->SetBrightness(3.0);
    EXPECT_DOUBLE_EQ(manager->GetBrightness(), 2.0);
}

TEST_F(DayNightModeManagerTest, SetSaturation) {
    manager->SetSaturation(1.2);
    EXPECT_DOUBLE_EQ(manager->GetSaturation(), 1.2);
    
    manager->SetSaturation(-0.5);
    EXPECT_DOUBLE_EQ(manager->GetSaturation(), 0.0);
    
    manager->SetSaturation(3.0);
    EXPECT_DOUBLE_EQ(manager->GetSaturation(), 2.0);
}

TEST_F(DayNightModeManagerTest, ResetToDefaults) {
    manager->SetNightMode();
    manager->SetContrast(1.5);
    manager->SetBrightness(0.5);
    manager->SetAutoModeSwitch(true);
    
    manager->ResetToDefaults();
    
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDay);
    EXPECT_DOUBLE_EQ(manager->GetContrast(), 1.0);
    EXPECT_DOUBLE_EQ(manager->GetBrightness(), 1.0);
    EXPECT_FALSE(manager->IsAutoModeSwitchEnabled());
}

TEST_F(DayNightModeManagerTest, ModeChangedCallback) {
    DisplayMode changedMode = DisplayMode::kDay;
    manager->SetModeChangedCallback([&changedMode](DisplayMode mode) {
        changedMode = mode;
    });
    
    manager->SetNightMode();
    EXPECT_EQ(changedMode, DisplayMode::kNight);
}

TEST_F(DayNightModeManagerTest, TransitionProgressCallback) {
    double progress = 0.0;
    manager->SetTransitionProgressCallback([&progress](double p) {
        progress = p;
    });
    
    manager->SetTransitionDuration(1.0);
    manager->StartTransition(DisplayMode::kNight);
    manager->UpdateTransition(0.5);
    
    EXPECT_DOUBLE_EQ(progress, 0.5);
}

TEST_F(DayNightModeManagerTest, ColorSchemeChangedCallback) {
    std::string schemeName;
    manager->SetColorSchemeChangedCallback([&schemeName](const ColorScheme& scheme) {
        schemeName = scheme.name;
    });
    
    manager->SetNightMode();
    EXPECT_EQ(schemeName, "Night");
}

TEST_F(DayNightModeManagerTest, CreateDefaultDayScheme) {
    ColorScheme scheme = DayNightModeManager::CreateDefaultDayScheme();
    EXPECT_EQ(scheme.name, "Day");
    EXPECT_DOUBLE_EQ(scheme.contrast, 1.0);
    EXPECT_DOUBLE_EQ(scheme.brightness, 1.0);
}

TEST_F(DayNightModeManagerTest, CreateDefaultNightScheme) {
    ColorScheme scheme = DayNightModeManager::CreateDefaultNightScheme();
    EXPECT_EQ(scheme.name, "Night");
    EXPECT_DOUBLE_EQ(scheme.brightness, 0.6);
}

TEST_F(DayNightModeManagerTest, CreateDefaultDuskScheme) {
    ColorScheme scheme = DayNightModeManager::CreateDefaultDuskScheme();
    EXPECT_EQ(scheme.name, "Dusk");
}

TEST_F(DayNightModeManagerTest, CreateDefaultDawnScheme) {
    ColorScheme scheme = DayNightModeManager::CreateDefaultDawnScheme();
    EXPECT_EQ(scheme.name, "Dawn");
}

TEST_F(DayNightModeManagerTest, SchemeSwitchingUpdatesCurrentScheme) {
    manager->SetDayMode();
    EXPECT_EQ(manager->GetCurrentScheme().name, "Day");
    
    manager->SetNightMode();
    EXPECT_EQ(manager->GetCurrentScheme().name, "Night");
    
    manager->SetDuskMode();
    EXPECT_EQ(manager->GetCurrentScheme().name, "Dusk");
    
    manager->SetDawnMode();
    EXPECT_EQ(manager->GetCurrentScheme().name, "Dawn");
}

TEST_F(DayNightModeManagerTest, SameModeNoChange) {
    manager->SetDayMode();
    DisplayMode mode = manager->GetMode();
    
    manager->SetMode(DisplayMode::kDay);
    EXPECT_EQ(manager->GetMode(), mode);
}

TEST_F(DayNightModeManagerTest, TransitionToSameMode) {
    manager->SetDayMode();
    manager->StartTransition(DisplayMode::kDay);
    EXPECT_FALSE(manager->IsTransitioning());
}

TEST_F(DayNightModeManagerTest, InterpolationDuringTransition) {
    manager->SetTransitionDuration(1.0);
    manager->StartTransition(DisplayMode::kNight);
    
    manager->UpdateTransition(0.5);
    
    const ColorScheme& scheme = manager->GetCurrentScheme();
    EXPECT_GT(scheme.backgroundColor.GetRed(), manager->GetNightScheme().backgroundColor.GetRed());
    EXPECT_LT(scheme.backgroundColor.GetRed(), manager->GetDayScheme().backgroundColor.GetRed());
}

TEST_F(DayNightModeManagerTest, MultipleTransitions) {
    manager->SetTransitionDuration(0.5);
    
    manager->StartTransition(DisplayMode::kNight);
    manager->UpdateTransition(0.5);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kNight);
    
    manager->StartTransition(DisplayMode::kDay);
    manager->UpdateTransition(0.5);
    EXPECT_EQ(manager->GetMode(), DisplayMode::kDay);
}

TEST_F(DayNightModeManagerTest, CustomSchemeNotAffectBuiltIn) {
    ColorScheme customScheme;
    customScheme.name = "Custom";
    customScheme.backgroundColor = Color(128, 128, 128);
    
    manager->RegisterColorScheme("Custom", customScheme);
    manager->SetCustomMode("Custom");
    
    const ColorScheme& dayScheme = manager->GetDayScheme();
    EXPECT_NE(dayScheme.backgroundColor.GetRed(), 128);
}
