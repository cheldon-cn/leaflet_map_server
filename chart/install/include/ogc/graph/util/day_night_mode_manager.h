#ifndef OGC_GRAPH_DAY_NIGHT_MODE_MANAGER_H
#define OGC_GRAPH_DAY_NIGHT_MODE_MANAGER_H

#include "ogc/graph/export.h"
#include <ogc/draw/color.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace ogc {
namespace graph {

enum class DisplayMode {
    kDay = 0,
    kNight = 1,
    kDusk = 2,
    kDawn = 3,
    kCustom = 4
};

struct ColorScheme {
    std::string name;
    ogc::draw::Color backgroundColor;
    ogc::draw::Color waterColor;
    ogc::draw::Color landColor;
    ogc::draw::Color textColor;
    ogc::draw::Color highlightColor;
    ogc::draw::Color selectionColor;
    ogc::draw::Color gridColor;
    ogc::draw::Color routeColor;
    ogc::draw::Color dangerColor;
    ogc::draw::Color safeWaterColor;
    double contrast = 1.0;
    double brightness = 1.0;
    double saturation = 1.0;
};

struct ModeTransition {
    DisplayMode fromMode;
    DisplayMode toMode;
    double duration = 1.0;
    double progress = 0.0;
    bool isActive = false;
};

class OGC_GRAPH_API DayNightModeManager {
public:
    using ModeChangedCallback = std::function<void(DisplayMode)>;
    using TransitionProgressCallback = std::function<void(double)>;
    using ColorSchemeChangedCallback = std::function<void(const ColorScheme&)>;
    
    static std::unique_ptr<DayNightModeManager> Create();
    
    ~DayNightModeManager();
    
    void SetMode(DisplayMode mode);
    DisplayMode GetMode() const { return m_currentMode; }
    
    void SetDayMode();
    void SetNightMode();
    void SetDuskMode();
    void SetDawnMode();
    void SetCustomMode(const std::string& schemeName);
    
    bool IsDayMode() const { return m_currentMode == DisplayMode::kDay; }
    bool IsNightMode() const { return m_currentMode == DisplayMode::kNight; }
    bool IsDuskMode() const { return m_currentMode == DisplayMode::kDusk; }
    bool IsDawnMode() const { return m_currentMode == DisplayMode::kDawn; }
    bool IsCustomMode() const { return m_currentMode == DisplayMode::kCustom; }
    
    const ColorScheme& GetCurrentScheme() const { return m_currentScheme; }
    const ColorScheme& GetDayScheme() const { return m_dayScheme; }
    const ColorScheme& GetNightScheme() const { return m_nightScheme; }
    
    void SetDayScheme(const ColorScheme& scheme);
    void SetNightScheme(const ColorScheme& scheme);
    void SetCustomScheme(const ColorScheme& scheme);
    
    void RegisterColorScheme(const std::string& name, const ColorScheme& scheme);
    bool HasColorScheme(const std::string& name) const;
    ColorScheme GetColorScheme(const std::string& name) const;
    std::vector<std::string> GetAvailableSchemes() const;
    
    void SetTransitionDuration(double seconds);
    double GetTransitionDuration() const { return m_transitionDuration; }
    
    void StartTransition(DisplayMode targetMode);
    void UpdateTransition(double deltaTime);
    bool IsTransitioning() const { return m_transition.isActive; }
    double GetTransitionProgress() const { return m_transition.progress; }
    
    void SetAutoModeSwitch(bool enabled);
    bool IsAutoModeSwitchEnabled() const { return m_autoModeSwitch; }
    
    void SetDayTimeRange(int startHour, int endHour);
    void SetNightTimeRange(int startHour, int endHour);
    void UpdateByTime(int hour);
    
    void SetModeChangedCallback(ModeChangedCallback callback);
    void SetTransitionProgressCallback(TransitionProgressCallback callback);
    void SetColorSchemeChangedCallback(ColorSchemeChangedCallback callback);
    
    ogc::draw::Color TransformColor(const ogc::draw::Color& color) const;
    ogc::draw::Color TransformColorForMode(const ogc::draw::Color& color, DisplayMode mode) const;
    
    double GetContrast() const { return m_currentScheme.contrast; }
    void SetContrast(double contrast);
    
    double GetBrightness() const { return m_currentScheme.brightness; }
    void SetBrightness(double brightness);
    
    double GetSaturation() const { return m_currentScheme.saturation; }
    void SetSaturation(double saturation);
    
    void ResetToDefaults();
    
    static ColorScheme CreateDefaultDayScheme();
    static ColorScheme CreateDefaultNightScheme();
    static ColorScheme CreateDefaultDuskScheme();
    static ColorScheme CreateDefaultDawnScheme();
    
private:
    DayNightModeManager();
    
    void ApplyCurrentMode();
    void InterpolateSchemes(const ColorScheme& from, const ColorScheme& to, double progress);
    void NotifyModeChanged();
    void NotifyTransitionProgress();
    void NotifyColorSchemeChanged();
    
    DisplayMode m_currentMode = DisplayMode::kDay;
    ColorScheme m_currentScheme;
    ColorScheme m_dayScheme;
    ColorScheme m_nightScheme;
    ColorScheme m_duskScheme;
    ColorScheme m_dawnScheme;
    
    ModeTransition m_transition;
    double m_transitionDuration = 1.0;
    
    bool m_autoModeSwitch = false;
    int m_dayStartHour = 6;
    int m_dayEndHour = 18;
    int m_nightStartHour = 20;
    int m_nightEndHour = 6;
    
    std::map<std::string, ColorScheme> m_customSchemes;
    std::string m_currentCustomScheme;
    
    ModeChangedCallback m_modeChangedCallback;
    TransitionProgressCallback m_transitionProgressCallback;
    ColorSchemeChangedCallback m_colorSchemeChangedCallback;
};

}  
}  

#endif
