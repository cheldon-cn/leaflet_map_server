#include "ogc/graph/util/day_night_mode_manager.h"
#include <ogc/draw/color.h>
#include <algorithm>
#include <cmath>

namespace ogc {
namespace graph {

using ogc::draw::Color;

std::unique_ptr<DayNightModeManager> DayNightModeManager::Create() {
    return std::unique_ptr<DayNightModeManager>(new DayNightModeManager());
}

DayNightModeManager::DayNightModeManager() {
    m_dayScheme = CreateDefaultDayScheme();
    m_nightScheme = CreateDefaultNightScheme();
    m_duskScheme = CreateDefaultDuskScheme();
    m_dawnScheme = CreateDefaultDawnScheme();
    m_currentScheme = m_dayScheme;
}

DayNightModeManager::~DayNightModeManager() {
}

void DayNightModeManager::SetMode(DisplayMode mode) {
    if (m_currentMode == mode) {
        return;
    }
    
    m_currentMode = mode;
    ApplyCurrentMode();
    NotifyModeChanged();
}

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
        m_currentCustomScheme = schemeName;
        SetMode(DisplayMode::kCustom);
    }
}

void DayNightModeManager::SetDayScheme(const ColorScheme& scheme) {
    m_dayScheme = scheme;
    if (m_currentMode == DisplayMode::kDay) {
        m_currentScheme = scheme;
        NotifyColorSchemeChanged();
    }
}

void DayNightModeManager::SetNightScheme(const ColorScheme& scheme) {
    m_nightScheme = scheme;
    if (m_currentMode == DisplayMode::kNight) {
        m_currentScheme = scheme;
        NotifyColorSchemeChanged();
    }
}

void DayNightModeManager::SetCustomScheme(const ColorScheme& scheme) {
    m_currentScheme = scheme;
    if (m_currentMode == DisplayMode::kCustom) {
        NotifyColorSchemeChanged();
    }
}

void DayNightModeManager::RegisterColorScheme(const std::string& name, const ColorScheme& scheme) {
    m_customSchemes[name] = scheme;
}

bool DayNightModeManager::HasColorScheme(const std::string& name) const {
    return m_customSchemes.find(name) != m_customSchemes.end();
}

ColorScheme DayNightModeManager::GetColorScheme(const std::string& name) const {
    auto it = m_customSchemes.find(name);
    if (it != m_customSchemes.end()) {
        return it->second;
    }
    return ColorScheme();
}

std::vector<std::string> DayNightModeManager::GetAvailableSchemes() const {
    std::vector<std::string> names;
    for (const auto& pair : m_customSchemes) {
        names.push_back(pair.first);
    }
    return names;
}

void DayNightModeManager::SetTransitionDuration(double seconds) {
    m_transitionDuration = std::max(0.0, std::min(10.0, seconds));
}

void DayNightModeManager::StartTransition(DisplayMode targetMode) {
    if (m_currentMode == targetMode) {
        return;
    }
    
    m_transition.fromMode = m_currentMode;
    m_transition.toMode = targetMode;
    m_transition.progress = 0.0;
    m_transition.duration = m_transitionDuration;
    m_transition.isActive = true;
}

void DayNightModeManager::UpdateTransition(double deltaTime) {
    if (!m_transition.isActive) {
        return;
    }
    
    m_transition.progress += deltaTime / m_transition.duration;
    
    if (m_transition.progress >= 1.0) {
        m_transition.progress = 1.0;
        m_transition.isActive = false;
        m_currentMode = m_transition.toMode;
        ApplyCurrentMode();
        NotifyModeChanged();
    } else {
        ColorScheme fromScheme;
        ColorScheme toScheme;
        
        switch (m_transition.fromMode) {
            case DisplayMode::kDay: fromScheme = m_dayScheme; break;
            case DisplayMode::kNight: fromScheme = m_nightScheme; break;
            case DisplayMode::kDusk: fromScheme = m_duskScheme; break;
            case DisplayMode::kDawn: fromScheme = m_dawnScheme; break;
            case DisplayMode::kCustom: fromScheme = m_currentScheme; break;
        }
        
        switch (m_transition.toMode) {
            case DisplayMode::kDay: toScheme = m_dayScheme; break;
            case DisplayMode::kNight: toScheme = m_nightScheme; break;
            case DisplayMode::kDusk: toScheme = m_duskScheme; break;
            case DisplayMode::kDawn: toScheme = m_dawnScheme; break;
            case DisplayMode::kCustom: toScheme = GetColorScheme(m_currentCustomScheme); break;
        }
        
        InterpolateSchemes(fromScheme, toScheme, m_transition.progress);
        NotifyTransitionProgress();
    }
}

void DayNightModeManager::SetAutoModeSwitch(bool enabled) {
    m_autoModeSwitch = enabled;
}

void DayNightModeManager::SetDayTimeRange(int startHour, int endHour) {
    m_dayStartHour = startHour;
    m_dayEndHour = endHour;
}

void DayNightModeManager::SetNightTimeRange(int startHour, int endHour) {
    m_nightStartHour = startHour;
    m_nightEndHour = endHour;
}

void DayNightModeManager::UpdateByTime(int hour) {
    if (!m_autoModeSwitch) {
        return;
    }
    
    if (hour >= m_dayStartHour && hour < m_dayEndHour) {
        if (m_currentMode != DisplayMode::kDay) {
            StartTransition(DisplayMode::kDay);
        }
    } else if (hour >= m_nightStartHour || hour < m_nightEndHour) {
        if (m_currentMode != DisplayMode::kNight) {
            StartTransition(DisplayMode::kNight);
        }
    } else if (hour >= m_dayEndHour && hour < m_nightStartHour) {
        if (m_currentMode != DisplayMode::kDusk) {
            StartTransition(DisplayMode::kDusk);
        }
    } else if (hour >= m_nightEndHour && hour < m_dayStartHour) {
        if (m_currentMode != DisplayMode::kDawn) {
            StartTransition(DisplayMode::kDawn);
        }
    }
}

void DayNightModeManager::SetModeChangedCallback(ModeChangedCallback callback) {
    m_modeChangedCallback = callback;
}

void DayNightModeManager::SetTransitionProgressCallback(TransitionProgressCallback callback) {
    m_transitionProgressCallback = callback;
}

void DayNightModeManager::SetColorSchemeChangedCallback(ColorSchemeChangedCallback callback) {
    m_colorSchemeChangedCallback = callback;
}

ogc::draw::Color DayNightModeManager::TransformColor(const ogc::draw::Color& color) const {
    return TransformColorForMode(color, m_currentMode);
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

void DayNightModeManager::SetContrast(double contrast) {
    m_currentScheme.contrast = std::max(0.0, std::min(2.0, contrast));
    NotifyColorSchemeChanged();
}

void DayNightModeManager::SetBrightness(double brightness) {
    m_currentScheme.brightness = std::max(0.0, std::min(2.0, brightness));
    NotifyColorSchemeChanged();
}

void DayNightModeManager::SetSaturation(double saturation) {
    m_currentScheme.saturation = std::max(0.0, std::min(2.0, saturation));
    NotifyColorSchemeChanged();
}

void DayNightModeManager::ResetToDefaults() {
    m_dayScheme = CreateDefaultDayScheme();
    m_nightScheme = CreateDefaultNightScheme();
    m_duskScheme = CreateDefaultDuskScheme();
    m_dawnScheme = CreateDefaultDawnScheme();
    m_currentMode = DisplayMode::kDay;
    m_currentScheme = m_dayScheme;
    m_transitionDuration = 1.0;
    m_autoModeSwitch = false;
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
    switch (m_currentMode) {
        case DisplayMode::kDay:
            m_currentScheme = m_dayScheme;
            break;
        case DisplayMode::kNight:
            m_currentScheme = m_nightScheme;
            break;
        case DisplayMode::kDusk:
            m_currentScheme = m_duskScheme;
            break;
        case DisplayMode::kDawn:
            m_currentScheme = m_dawnScheme;
            break;
        case DisplayMode::kCustom:
            if (HasColorScheme(m_currentCustomScheme)) {
                m_currentScheme = GetColorScheme(m_currentCustomScheme);
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
    
    m_currentScheme.backgroundColor = lerpColor(from.backgroundColor, to.backgroundColor, progress);
    m_currentScheme.waterColor = lerpColor(from.waterColor, to.waterColor, progress);
    m_currentScheme.landColor = lerpColor(from.landColor, to.landColor, progress);
    m_currentScheme.textColor = lerpColor(from.textColor, to.textColor, progress);
    m_currentScheme.highlightColor = lerpColor(from.highlightColor, to.highlightColor, progress);
    m_currentScheme.selectionColor = lerpColor(from.selectionColor, to.selectionColor, progress);
    m_currentScheme.gridColor = lerpColor(from.gridColor, to.gridColor, progress);
    m_currentScheme.routeColor = lerpColor(from.routeColor, to.routeColor, progress);
    m_currentScheme.dangerColor = lerpColor(from.dangerColor, to.dangerColor, progress);
    m_currentScheme.safeWaterColor = lerpColor(from.safeWaterColor, to.safeWaterColor, progress);
    
    m_currentScheme.contrast = from.contrast + (to.contrast - from.contrast) * progress;
    m_currentScheme.brightness = from.brightness + (to.brightness - from.brightness) * progress;
    m_currentScheme.saturation = from.saturation + (to.saturation - from.saturation) * progress;
}

void DayNightModeManager::NotifyModeChanged() {
    if (m_modeChangedCallback) {
        m_modeChangedCallback(m_currentMode);
    }
}

void DayNightModeManager::NotifyTransitionProgress() {
    if (m_transitionProgressCallback) {
        m_transitionProgressCallback(m_transition.progress);
    }
}

void DayNightModeManager::NotifyColorSchemeChanged() {
    if (m_colorSchemeChangedCallback) {
        m_colorSchemeChangedCallback(m_currentScheme);
    }
}

}  
}  
