#ifndef OGC_GRAPH_DAY_NIGHT_MODE_MANAGER_H
#define OGC_GRAPH_DAY_NIGHT_MODE_MANAGER_H

#include "ogc/graph/export.h"
#include <ogc/draw/color.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>

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
    DisplayMode GetMode() const;
    
    void SetDayMode();
    void SetNightMode();
    void SetDuskMode();
    void SetDawnMode();
    void SetCustomMode(const std::string& schemeName);
    
    bool IsDayMode() const;
    bool IsNightMode() const;
    bool IsDuskMode() const;
    bool IsDawnMode() const;
    bool IsCustomMode() const;
    
    const ColorScheme& GetCurrentScheme() const;
    const ColorScheme& GetDayScheme() const;
    const ColorScheme& GetNightScheme() const;
    
    void SetDayScheme(const ColorScheme& scheme);
    void SetNightScheme(const ColorScheme& scheme);
    void SetCustomScheme(const ColorScheme& scheme);
    
    void RegisterColorScheme(const std::string& name, const ColorScheme& scheme);
    bool HasColorScheme(const std::string& name) const;
    ColorScheme GetColorScheme(const std::string& name) const;
    std::vector<std::string> GetAvailableSchemes() const;
    
    void SetTransitionDuration(double seconds);
    double GetTransitionDuration() const;
    
    void StartTransition(DisplayMode targetMode);
    void UpdateTransition(double deltaTime);
    bool IsTransitioning() const;
    double GetTransitionProgress() const;
    
    void SetAutoModeSwitch(bool enabled);
    bool IsAutoModeSwitchEnabled() const;
    
    void SetDayTimeRange(int startHour, int endHour);
    void SetNightTimeRange(int startHour, int endHour);
    void UpdateByTime(int hour);
    
    void SetModeChangedCallback(ModeChangedCallback callback);
    void SetTransitionProgressCallback(TransitionProgressCallback callback);
    void SetColorSchemeChangedCallback(ColorSchemeChangedCallback callback);
    
    ogc::draw::Color TransformColor(const ogc::draw::Color& color) const;
    ogc::draw::Color TransformColorForMode(const ogc::draw::Color& color, DisplayMode mode) const;
    
    double GetContrast() const;
    void SetContrast(double contrast);
    
    double GetBrightness() const;
    void SetBrightness(double brightness);
    
    double GetSaturation() const;
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
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  
}  

#endif
