#include "alert/weather_alert_engine.h"
#include "alert/alert_exception.h"

namespace alert {

WeatherAlertEngine::WeatherAlertEngine()
    : m_weatherProvider(nullptr) {}

WeatherAlertEngine::~WeatherAlertEngine() {}

AlertType WeatherAlertEngine::GetType() const {
    return AlertType::kWeatherAlert;
}

Alert WeatherAlertEngine::Evaluate(const AlertContext& context) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    std::vector<WeatherAlertData> alerts = FetchWeatherAlerts(context.GetPosition());
    
    for (const auto& alertData : alerts) {
        if (IsInAffectedArea(context.GetPosition(), alertData.affectedArea)) {
            return ConvertToAlert(alertData);
        }
    }
    
    return Alert();
}

void WeatherAlertEngine::SetThreshold(const ThresholdConfig& config) {
    m_threshold = config;
}

ThresholdConfig WeatherAlertEngine::GetThreshold() const {
    return m_threshold;
}

std::string WeatherAlertEngine::GetName() const {
    return "WeatherAlertEngine";
}

std::vector<WeatherAlertData> WeatherAlertEngine::FetchWeatherAlerts(const Coordinate& position) {
    if (m_weatherProvider) {
        return m_weatherProvider(position);
    }
    return std::vector<WeatherAlertData>();
}

bool WeatherAlertEngine::IsInAffectedArea(const Coordinate& position, const std::string& area) {
    return true;
}

Alert WeatherAlertEngine::ConvertToAlert(const WeatherAlertData& data) {
    Alert alert(Alert::GenerateAlertId(AlertType::kWeatherAlert), 
                AlertType::kWeatherAlert, data.level);
    alert.SetTitle("Weather Alert");
    alert.SetMessage(data.content);
    
    return alert;
}

void WeatherAlertEngine::SetWeatherProvider(std::vector<WeatherAlertData> (*provider)(const Coordinate&)) {
    m_weatherProvider = provider;
}

void WeatherAlertEngine::SetEnabledTypes(const std::vector<std::string>& types) {
    m_enabledTypes = types;
}

}
