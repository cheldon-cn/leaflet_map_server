#ifndef ALERT_SYSTEM_WEATHER_ALERT_ENGINE_H
#define ALERT_SYSTEM_WEATHER_ALERT_ENGINE_H

#include "i_alert_engine.h"
#include "coordinate.h"
#include <vector>
#include <string>
#include <map>

namespace alert {

struct WeatherAlertData {
    std::string alertId;
    std::string alertType;
    AlertLevel level;
    std::string issueTime;
    std::string expireTime;
    std::string content;
    std::string affectedArea;
    std::map<std::string, double> parameters;
    
    WeatherAlertData() : level(AlertLevel::kNone) {}
};

class WeatherAlertEngine : public IAlertEngine {
public:
    WeatherAlertEngine();
    virtual ~WeatherAlertEngine();
    
    Alert Evaluate(const AlertContext& context) override;
    
    AlertType GetType() const override;
    
    void SetThreshold(const ThresholdConfig& config) override;
    ThresholdConfig GetThreshold() const override;
    
    std::string GetName() const override;
    
    void SetWeatherProvider(std::vector<WeatherAlertData> (*provider)(const Coordinate&));
    
    void SetEnabledTypes(const std::vector<std::string>& types);
    
    Alert ConvertToAlert(const WeatherAlertData& data);
    
private:
    ThresholdConfig m_threshold;
    std::vector<WeatherAlertData> (*m_weatherProvider)(const Coordinate&);
    std::vector<std::string> m_enabledTypes;
    
    std::vector<WeatherAlertData> FetchWeatherAlerts(const Coordinate& position);
    bool IsInAffectedArea(const Coordinate& position, const std::string& area);
};

}

#endif
