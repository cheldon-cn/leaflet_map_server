#include "ogc/graph/util/day_night_mode_manager.h"
#include <ogc/draw/color.h>
#include <algorithm>
#include <cmath>
#include <map>

namespace ogc {
namespace graph {

using ogc::draw::Color;

struct DayNightModeManager::Impl {
    DisplayMode currentMode = DisplayMode::kDay;
    ColorScheme currentScheme;
    ColorScheme dayScheme;
    ColorScheme nightScheme;
    ColorScheme duskScheme;
    ColorScheme dawnScheme;
    
    ModeTransition transition;
    double transitionDuration = 1.0;
    
    bool autoModeSwitch = false;
    int dayStartHour = 6;
    int dayEndHour = 18;
    int nightStartHour = 20;
    int nightEndHour = 6;
    
    std::map<std::string, ColorScheme> customSchemes;
    std::string currentCustomScheme;
    
    ModeChangedCallback modeChangedCallback;
    TransitionProgressCallback transitionProgressCallback;
    ColorSchemeChangedCallback colorSchemeChangedCallback;
};

std::unique_ptr<DayNightModeManager> DayNightModeManager::Create() {
    return std::unique_ptr<DayNightModeManager>(new DayNightModeManager());
}

DayNightModeManager::DayNightModeManager()
    : impl_(std::make_unique<Impl>())
{
    impl_->dayScheme = CreateDefaultDayScheme();
    impl_->nightScheme = CreateDefaultNightScheme();
    impl_->duskScheme = CreateDefaultDuskScheme();
    impl_->dawnScheme = CreateDefaultDawnScheme();
    impl_->currentScheme = impl_->dayScheme;
}

DayNightModeManager::~DayNightModeManager() {
}

void DayNightModeManager::SetMode(DisplayMode mode) {
    if (impl_->currentMode == mode) {
        return;
    }
    
    impl_->currentMode = mode;
    ApplyCurrentMode();
    NotifyModeChanged();
}

DisplayMode DayNightModeManager::GetMode() const { return impl_->currentMode; }

void DayNightModeManager::SetDayMode() {
    SetMode(DisplayMode::kDay);
}

void DayNightModeManager::SetNightMode() {
    SetMode(DisplayMode::kNight);
}

void DayNightModeManager::SetDuskMode() {
    SetMode(DisplayMode::kDusk);
}

void DayNightModeManager::SetDawnMode() {
    SetMode(DisplayMode::kDawn);
}

void DayNightModeManager::SetCustomMode(const std::string& schemeName) {
    if (HasColorScheme(schemeName)) {
        impl_->currentCustomScheme = schemeName;
        SetMode(DisplayMode::kCustom);
    }
}

bool DayNightModeManager::IsDayMode() const { return impl_->currentMode == DisplayMode::kDay; }
bool DayNightModeManager::IsNightMode() const { return impl_->currentMode == DisplayMode::kNight; }
bool DayNightModeManager::IsDuskMode() const { return impl_->currentMode == DisplayMode::kDusk; }
bool DayNightModeManager::IsDawnMode() const { return impl_->currentMode == DisplayMode::kDawn; }
bool DayNightModeManager::IsCustomMode() const { return impl_->currentMode == DisplayMode::kCustom; }

const ColorScheme& DayNightModeManager::GetCurrentScheme() const { return impl_->currentScheme; }
const ColorScheme& DayNightModeManager::GetDayScheme() const { return impl_->dayScheme; }
const ColorScheme& DayNightModeManager::GetNightScheme() const { return impl_->nightScheme; }

void DayNightModeManager::SetDayScheme(const ColorScheme& scheme) {
    impl_->dayScheme = scheme;
    if (impl_->currentMode == DisplayMode::kDay) {
        impl_->currentScheme = scheme;
        NotifyColorSchemeChanged();
    }
}

void DayNightModeManager::SetNightScheme(const ColorScheme& scheme) {
    impl_->nightScheme = scheme;
    if (impl_->currentMode == DisplayMode::kNight) {
        impl_->currentScheme = scheme;
        NotifyColorSchemeChanged();
    }
}

void DayNightModeManager::SetCustomScheme(const ColorScheme& scheme) {
    impl_->currentScheme = scheme;
    if (impl_->currentMode == DisplayMode::kCustom) {
        NotifyColorSchemeChanged();
    }
}

void DayNightModeManager::RegisterColorScheme(const std::string& name, const ColorScheme& scheme) {
    impl_->customSchemes[name] = scheme;
}

bool DayNightModeManager::HasColorScheme(const std::string& name) const {
    return impl_->customSchemes.find(name) != impl_->customSchemes.end();
}

ColorScheme DayNightModeManager::GetColorScheme(const std::string& name) const {
    auto it = impl_->customSchemes.find(name);
    if (it != impl_->customSchemes.end()) {
        return it->second;
    }
    return ColorScheme();
}

std::vector<std::string> DayNightModeManager::GetAvailableSchemes() const {
    std::vector<std::string> names;
    for (const auto& pair : impl_->customSchemes) {
        names.push_back(pair.first);
    }
    return names;
}

void DayNightModeManager::SetTransitionDuration(double seconds) {
    impl_->transitionDuration = std::max(0.0, std::min(10.0, seconds));
}

double DayNightModeManager::GetTransitionDuration() const { return impl_->transitionDuration; }

void DayNightModeManager::StartTransition(DisplayMode targetMode) {
    if (impl_->currentMode == targetMode) {
        return;
    }
    
    impl_->transition.fromMode = impl_->currentMode;
    impl_->transition.toMode = targetMode;
    impl_->transition.progress = 0.0;
    impl_->transition.duration = impl_->transitionDuration;
    impl_->transition.isActive = true;
}

void DayNightModeManager::UpdateTransition(double deltaTime) {
    if (!impl_->transition.isActive) {
        return;
    }
    
    impl_->transition.progress += deltaTime / impl_->transition.duration;
    
    if (impl_->transition.progress >= 1.0) {
        impl_->transition.progress = 1.0;
        impl_->transition.isActive = false;
        impl_->currentMode = impl_->transition.toMode;
        ApplyCurrentMode();
        NotifyModeChanged();
    } else {
        ColorScheme fromScheme;
        ColorScheme toScheme;
        
        switch (impl_->transition.fromMode) {
            case DisplayMode::kDay: fromScheme = impl_->dayScheme; break;
            case DisplayMode::kNight: fromScheme = impl_->nightScheme; break;
            case DisplayMode::kDusk: fromScheme = impl_->duskScheme; break;
            case DisplayMode::kDawn: fromScheme = impl_->dawnScheme; break;
            case DisplayMode::kCustom: fromScheme = impl_->currentScheme; break;
        }
        
        switch (impl_->transition.toMode) {
            case DisplayMode::kDay: toScheme = impl_->dayScheme; break;
            case DisplayMode::kNight: toScheme = impl_->nightScheme; break;
            case DisplayMode::kDusk: toScheme = impl_->duskScheme; break;
            case DisplayMode::kDawn: toScheme = impl_->dawnScheme; break;
            case DisplayMode::kCustom: toScheme = GetColorScheme(impl_->currentCustomScheme); break;
        }
        
        InterpolateSchemes(fromScheme, toScheme, impl_->transition.progress);
        NotifyTransitionProgress();
    }
}

bool DayNightModeManager::IsTransitioning() const { return impl_->transition.isActive; }
double DayNightModeManager::GetTransitionProgress() const { return impl_->transition.progress; }

void DayNightModeManager::SetAutoModeSwitch(bool enabled) {
    impl_->autoModeSwitch = enabled;
}

bool DayNightModeManager::IsAutoModeSwitchEnabled() const { return impl_->autoModeSwitch; }

void DayNightModeManager::SetDayTimeRange(int startHour, int endHour) {
    impl_->dayStartHour = startHour;
    impl_->dayEndHour = endHour;
}

void DayNightModeManager::SetNightTimeRange(int startHour, int endHour) {
    impl_->nightStartHour = startHour;
    impl_->nightEndHour = endHour;
}

void DayNightModeManager::UpdateByTime(int hour) {
    if (!impl_->autoModeSwitch) {
        return;
    }
    
    if (hour >= impl_->dayStartHour && hour < impl_->dayEndHour) {
        if (impl_->currentMode != DisplayMode::kDay) {
            StartTransition(DisplayMode::kDay);
        }
    } else if (hour >= impl_->nightStartHour || hour < impl_->nightEndHour) {
        if (impl_->currentMode != DisplayMode::kNight) {
            StartTransition(DisplayMode::kNight);
        }
    } else if (hour >= impl_->dayEndHour && hour < impl_->nightStartHour) {
        if (impl_->currentMode != DisplayMode::kDusk) {
            StartTransition(DisplayMode::kDusk);
        }
    } else if (hour >= impl_->nightEndHour && hour < impl_->dayStartHour) {
        if (impl_->currentMode != DisplayMode::kDawn) {
            StartTransition(DisplayMode::kDawn);
        }
    }
}

void DayNightModeManager::SetModeChangedCallback(ModeChangedCallback callback) {
    impl_->modeChangedCallback = callback;
}

void DayNightModeManager::SetTransitionProgressCallback(TransitionProgressCallback callback) {
    impl_->transitionProgressCallback = callback;
}

void DayNightModeManager::SetColorSchemeChangedCallback(ColorSchemeChangedCallback callback) {
    impl_->colorSchemeChangedCallback = callback;
}

ogc::draw::Color DayNightModeManager::TransformColor(const ogc::draw::Color& color) const {
    return TransformColorForMode(color, impl_->currentMode);
}

ogc::draw::Color DayNightModeManager::TransformColorForMode(const ogc::draw::Color& color, DisplayMode mode) const {
    uint8_t r = color.GetRed();
    uint8_t g = color.GetGreen();
    uint8_t b = color.GetBlue();
    uint8_t a = color.GetAlpha();
    
    switch (mode) {
        case DisplayMode::kNight:
            r = static_cast<uint8_t>(std::min(255.0, r * 0.6));
            g = static_cast<uint8_t>(std::min(255.0, g * 0.6));
            b = static_cast<uint8_t>(std::min(255.0, b * 0.8));
            break;
        case DisplayMode::kDusk:
            r = static_cast<uint8_t>(std::min(255.0, r * 0.9));
            g = static_cast<uint8_t>(std::min(255.0, g * 0.8));
            b = static_cast<uint8_t>(std::min(255.0, b * 0.7));
            break;
        case DisplayMode::kDawn:
            r = static_cast<uint8_t>(std::min(255.0, r * 0.85));
            g = static_cast<uint8_t>(std::min(255.0, g * 0.85));
            b = static_cast<uint8_t>(std::min(255.0, b * 0.9));
            break;
        default:
            break;
    }
    
    return ogc::draw::Color(r, g, b, a);
}

double DayNightModeManager::GetContrast() const { return impl_->currentScheme.contrast; }

void DayNightModeManager::SetContrast(double contrast) {
    impl_->currentScheme.contrast = std::max(0.0, std::min(2.0, contrast));
    NotifyColorSchemeChanged();
}

double DayNightModeManager::GetBrightness() const { return impl_->currentScheme.brightness; }

void DayNightModeManager::SetBrightness(double brightness) {
    impl_->currentScheme.brightness = std::max(0.0, std::min(2.0, brightness));
    NotifyColorSchemeChanged();
}

double DayNightModeManager::GetSaturation() const { return impl_->currentScheme.saturation; }

void DayNightModeManager::SetSaturation(double saturation) {
    impl_->currentScheme.saturation = std::max(0.0, std::min(2.0, saturation));
    NotifyColorSchemeChanged();
}

void DayNightModeManager::ResetToDefaults() {
    impl_->dayScheme = CreateDefaultDayScheme();
    impl_->nightScheme = CreateDefaultNightScheme();
    impl_->duskScheme = CreateDefaultDuskScheme();
    impl_->dawnScheme = CreateDefaultDawnScheme();
    impl_->currentMode = DisplayMode::kDay;
    impl_->currentScheme = impl_->dayScheme;
    impl_->transitionDuration = 1.0;
    impl_->autoModeSwitch = false;
    NotifyModeChanged();
    NotifyColorSchemeChanged();
}

ColorScheme DayNightModeManager::CreateDefaultDayScheme() {
    ColorScheme scheme;
    scheme.name = "Day";
    scheme.backgroundColor = ogc::draw::Color(255, 255, 255);
    scheme.waterColor = ogc::draw::Color(173, 216, 230);
    scheme.landColor = ogc::draw::Color(210, 180, 140);
    scheme.textColor = ogc::draw::Color(0, 0, 0);
    scheme.highlightColor = ogc::draw::Color(255, 255, 0);
    scheme.selectionColor = ogc::draw::Color(0, 255, 0);
    scheme.gridColor = ogc::draw::Color(200, 200, 200);
    scheme.routeColor = ogc::draw::Color(255, 0, 0);
    scheme.dangerColor = ogc::draw::Color(255, 0, 0);
    scheme.safeWaterColor = ogc::draw::Color(0, 128, 0);
    scheme.contrast = 1.0;
    scheme.brightness = 1.0;
    scheme.saturation = 1.0;
    return scheme;
}

ColorScheme DayNightModeManager::CreateDefaultNightScheme() {
    ColorScheme scheme;
    scheme.name = "Night";
    scheme.backgroundColor = ogc::draw::Color(20, 20, 40);
    scheme.waterColor = ogc::draw::Color(30, 40, 80);
    scheme.landColor = ogc::draw::Color(60, 50, 40);
    scheme.textColor = ogc::draw::Color(200, 200, 200);
    scheme.highlightColor = ogc::draw::Color(255, 255, 100);
    scheme.selectionColor = ogc::draw::Color(100, 255, 100);
    scheme.gridColor = ogc::draw::Color(80, 80, 100);
    scheme.routeColor = ogc::draw::Color(255, 100, 100);
    scheme.dangerColor = ogc::draw::Color(255, 50, 50);
    scheme.safeWaterColor = ogc::draw::Color(50, 200, 50);
    scheme.contrast = 1.2;
    scheme.brightness = 0.6;
    scheme.saturation = 0.8;
    return scheme;
}

ColorScheme DayNightModeManager::CreateDefaultDuskScheme() {
    ColorScheme scheme;
    scheme.name = "Dusk";
    scheme.backgroundColor = ogc::draw::Color(255, 200, 150);
    scheme.waterColor = ogc::draw::Color(100, 120, 160);
    scheme.landColor = ogc::draw::Color(180, 140, 100);
    scheme.textColor = ogc::draw::Color(50, 30, 20);
    scheme.highlightColor = ogc::draw::Color(255, 255, 150);
    scheme.selectionColor = ogc::draw::Color(100, 255, 100);
    scheme.gridColor = ogc::draw::Color(180, 160, 140);
    scheme.routeColor = ogc::draw::Color(255, 80, 80);
    scheme.dangerColor = ogc::draw::Color(255, 50, 50);
    scheme.safeWaterColor = ogc::draw::Color(50, 150, 50);
    scheme.contrast = 1.1;
    scheme.brightness = 0.9;
    scheme.saturation = 1.1;
    return scheme;
}

ColorScheme DayNightModeManager::CreateDefaultDawnScheme() {
    ColorScheme scheme;
    scheme.name = "Dawn";
    scheme.backgroundColor = ogc::draw::Color(255, 220, 200);
    scheme.waterColor = ogc::draw::Color(120, 140, 180);
    scheme.landColor = ogc::draw::Color(190, 160, 130);
    scheme.textColor = ogc::draw::Color(40, 30, 20);
    scheme.highlightColor = ogc::draw::Color(255, 255, 180);
    scheme.selectionColor = ogc::draw::Color(100, 255, 100);
    scheme.gridColor = ogc::draw::Color(190, 180, 170);
    scheme.routeColor = ogc::draw::Color(255, 100, 80);
    scheme.dangerColor = ogc::draw::Color(255, 60, 50);
    scheme.safeWaterColor = ogc::draw::Color(60, 160, 60);
    scheme.contrast = 1.05;
    scheme.brightness = 0.95;
    scheme.saturation = 1.05;
    return scheme;
}

void DayNightModeManager::ApplyCurrentMode() {
    switch (impl_->currentMode) {
        case DisplayMode::kDay:
            impl_->currentScheme = impl_->dayScheme;
            break;
        case DisplayMode::kNight:
            impl_->currentScheme = impl_->nightScheme;
            break;
        case DisplayMode::kDusk:
            impl_->currentScheme = impl_->duskScheme;
            break;
        case DisplayMode::kDawn:
            impl_->currentScheme = impl_->dawnScheme;
            break;
        case DisplayMode::kCustom:
            if (HasColorScheme(impl_->currentCustomScheme)) {
                impl_->currentScheme = GetColorScheme(impl_->currentCustomScheme);
            }
            break;
    }
    NotifyColorSchemeChanged();
}

void DayNightModeManager::InterpolateSchemes(const ColorScheme& from, const ColorScheme& to, double progress) {
    auto lerpColor = [](const Color& a, const Color& b, double t) -> Color {
        return Color(
            static_cast<uint8_t>(a.GetRed() + (b.GetRed() - a.GetRed()) * t),
            static_cast<uint8_t>(a.GetGreen() + (b.GetGreen() - a.GetGreen()) * t),
            static_cast<uint8_t>(a.GetBlue() + (b.GetBlue() - a.GetBlue()) * t),
            static_cast<uint8_t>(a.GetAlpha() + (b.GetAlpha() - a.GetAlpha()) * t)
        );
    };
    
    impl_->currentScheme.backgroundColor = lerpColor(from.backgroundColor, to.backgroundColor, progress);
    impl_->currentScheme.waterColor = lerpColor(from.waterColor, to.waterColor, progress);
    impl_->currentScheme.landColor = lerpColor(from.landColor, to.landColor, progress);
    impl_->currentScheme.textColor = lerpColor(from.textColor, to.textColor, progress);
    impl_->currentScheme.highlightColor = lerpColor(from.highlightColor, to.highlightColor, progress);
    impl_->currentScheme.selectionColor = lerpColor(from.selectionColor, to.selectionColor, progress);
    impl_->currentScheme.gridColor = lerpColor(from.gridColor, to.gridColor, progress);
    impl_->currentScheme.routeColor = lerpColor(from.routeColor, to.routeColor, progress);
    impl_->currentScheme.dangerColor = lerpColor(from.dangerColor, to.dangerColor, progress);
    impl_->currentScheme.safeWaterColor = lerpColor(from.safeWaterColor, to.safeWaterColor, progress);
    
    impl_->currentScheme.contrast = from.contrast + (to.contrast - from.contrast) * progress;
    impl_->currentScheme.brightness = from.brightness + (to.brightness - from.brightness) * progress;
    impl_->currentScheme.saturation = from.saturation + (to.saturation - from.saturation) * progress;
}

void DayNightModeManager::NotifyModeChanged() {
    if (impl_->modeChangedCallback) {
        impl_->modeChangedCallback(impl_->currentMode);
    }
}

void DayNightModeManager::NotifyTransitionProgress() {
    if (impl_->transitionProgressCallback) {
        impl_->transitionProgressCallback(impl_->transition.progress);
    }
}

void DayNightModeManager::NotifyColorSchemeChanged() {
    if (impl_->colorSchemeChangedCallback) {
        impl_->colorSchemeChangedCallback(impl_->currentScheme);
    }
}

}  
}  
