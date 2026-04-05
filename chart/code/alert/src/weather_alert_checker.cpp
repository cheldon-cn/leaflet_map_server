#include "ogc/alert/weather_alert_checker.h"
#include "ogc/draw/log.h"
#include <cmath>

namespace ogc {
namespace alert {

class WeatherAlertChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(3) {
        m_threshold.level1_threshold = 15.0;
        m_threshold.level2_threshold = 20.0;
        m_threshold.level3_threshold = 25.0;
        m_threshold.level4_threshold = 30.0;
    }
    
    std::string GetCheckerId() const { return "weather_alert_checker"; }
    AlertType GetAlertType() const { return AlertType::kWeather; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = threshold;
    }
    
    AlertThreshold GetThreshold() const { return m_threshold; }
    
    void SetWeatherData(std::shared_ptr<void> weather_data) {
        m_weatherData = weather_data;
    }
    
    AlertLevel DetermineWeatherLevel(double wind_speed, double wave_height, double visibility) const {
        if (wind_speed >= m_threshold.level4_threshold) return AlertLevel::kLevel4;
        if (wind_speed >= m_threshold.level3_threshold) return AlertLevel::kLevel3;
        if (wind_speed >= m_threshold.level2_threshold) return AlertLevel::kLevel2;
        if (wind_speed >= m_threshold.level1_threshold) return AlertLevel::kLevel1;
        return AlertLevel::kNone;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        
        double wind_speed = 18.0;
        double wave_height = 2.5;
        double visibility = 5.0;
        
        AlertLevel level = DetermineWeatherLevel(wind_speed, wave_height, visibility);
        if (level != AlertLevel::kNone) {
            auto alert = std::make_shared<WeatherAlert>();
            alert->alert_id = "WEATHER_" + std::to_string(std::time(nullptr));
            alert->alert_type = AlertType::kWeather;
            alert->alert_level = level;
            alert->status = AlertStatus::kActive;
            alert->issue_time = DateTime::Now();
            alert->position = context.ship_position;
            alert->user_id = context.user_id;
            alert->acknowledge_required = true;
            alert->wind_speed = wind_speed;
            alert->wave_height = wave_height;
            alert->visibility = visibility;
            alert->weather_type = "Wind";
            alert->weather_description = "Strong wind warning";
            
            alert->content.type = "Weather";
            alert->content.level = static_cast<int>(level);
            alert->content.title = "Weather Alert";
            alert->content.message = "Wind speed: " + std::to_string(wind_speed) + " m/s";
            alert->content.position = context.ship_position;
            alert->content.action_required = "Reduce speed or seek shelter";
            
            alerts.push_back(alert);
        }
        
        return alerts;
    }
    
private:
    bool m_enabled;
    int m_priority;
    AlertThreshold m_threshold;
    std::shared_ptr<void> m_weatherData;
};

WeatherAlertChecker::WeatherAlertChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

WeatherAlertChecker::~WeatherAlertChecker() {
}

std::string WeatherAlertChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType WeatherAlertChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int WeatherAlertChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool WeatherAlertChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void WeatherAlertChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> WeatherAlertChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void WeatherAlertChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold WeatherAlertChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void WeatherAlertChecker::SetWeatherData(std::shared_ptr<void> weather_data) {
    m_impl->SetWeatherData(weather_data);
}

AlertLevel WeatherAlertChecker::DetermineWeatherLevel(double wind_speed, double wave_height, double visibility) const {
    return m_impl->DetermineWeatherLevel(wind_speed, wave_height, visibility);
}

std::unique_ptr<WeatherAlertChecker> WeatherAlertChecker::Create() {
    return std::unique_ptr<WeatherAlertChecker>(new WeatherAlertChecker());
}

}
}
