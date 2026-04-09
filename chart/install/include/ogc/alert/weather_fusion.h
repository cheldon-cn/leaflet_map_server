#ifndef OGC_ALERT_WEATHER_FUSION_H
#define OGC_ALERT_WEATHER_FUSION_H

#include "export.h"
#include "types.h"
#include "ogc/geom/geometry.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

struct WeatherObservation {
    std::string source_id;
    std::string source_type;
    DateTime observation_time;
    Coordinate location;
    double wind_speed;
    double wind_direction;
    double visibility;
    double precipitation;
    double wave_height;
    double temperature;
    double humidity;
    double pressure;
    std::string weather_condition;
    double confidence;
};

struct WeatherForecast {
    std::string forecast_id;
    std::string source_id;
    DateTime issue_time;
    DateTime valid_from;
    DateTime valid_to;
    ogc::GeometrySharedPtr affected_area;
    WeatherType weather_type;
    int severity;
    double probability;
    std::string description;
    std::string recommendation;
};

struct WeatherFusionResult {
    DateTime fusion_time;
    Coordinate location;
    double fused_wind_speed;
    double fused_wind_direction;
    double fused_visibility;
    double fused_precipitation;
    double fused_wave_height;
    double overall_confidence;
    std::vector<WeatherForecast> active_alerts;
    int max_severity;
    std::string summary;
};

struct WeatherSourceConfig {
    std::string source_id;
    std::string source_name;
    std::string source_type;
    double weight;
    double reliability_score;
    int priority;
    bool enabled;
    int update_interval_seconds;
};

class OGC_ALERT_API IWeatherFusion {
public:
    virtual ~IWeatherFusion() = default;
    
    virtual void AddObservation(const WeatherObservation& observation) = 0;
    virtual void AddForecast(const WeatherForecast& forecast) = 0;
    
    virtual WeatherFusionResult Fuse(const Coordinate& location, 
                                      const DateTime& time) = 0;
    
    virtual std::vector<WeatherForecast> GetActiveAlerts(
        const Coordinate& location, const DateTime& time) = 0;
    
    virtual void SetSourceConfig(const WeatherSourceConfig& config) = 0;
    virtual WeatherSourceConfig GetSourceConfig(const std::string& source_id) const = 0;
    
    virtual void RemoveExpiredData(const DateTime& cutoff_time) = 0;
    
    virtual void Clear() = 0;
    
    static std::unique_ptr<IWeatherFusion> Create();
};

class OGC_ALERT_API WeatherFusion : public IWeatherFusion {
public:
    WeatherFusion();
    ~WeatherFusion() override;
    
    void AddObservation(const WeatherObservation& observation) override;
    void AddForecast(const WeatherForecast& forecast) override;
    
    WeatherFusionResult Fuse(const Coordinate& location, 
                              const DateTime& time) override;
    
    std::vector<WeatherForecast> GetActiveAlerts(
        const Coordinate& location, const DateTime& time) override;
    
    void SetSourceConfig(const WeatherSourceConfig& config) override;
    WeatherSourceConfig GetSourceConfig(const std::string& source_id) const override;
    
    void RemoveExpiredData(const DateTime& cutoff_time) override;
    
    void Clear() override;

private:
    std::map<std::string, WeatherSourceConfig> m_sourceConfigs;
    std::vector<WeatherObservation> m_observations;
    std::vector<WeatherForecast> m_forecasts;
    
    double CalculateDistanceWeight(const Coordinate& obs_location,
                                    const Coordinate& target_location,
                                    double max_distance_km) const;
    double CalculateTimeWeight(const DateTime& obs_time,
                                const DateTime& target_time,
                                int max_age_seconds) const;
    double FuseValue(const std::vector<std::pair<double, double>>& value_weights) const;
    bool IsLocationInArea(const Coordinate& location, const ogc::GeometrySharedPtr& area) const;
    bool IsForecastActive(const WeatherForecast& forecast, const DateTime& time) const;
};

class OGC_ALERT_API WeatherQualityAssessor {
public:
    static double CalculateObservationQuality(const WeatherObservation& obs);
    static double CalculateForecastReliability(const WeatherForecast& forecast);
    static bool ValidateObservation(const WeatherObservation& obs);
    static bool ValidateForecast(const WeatherForecast& forecast);
};

class OGC_ALERT_API WeatherInterpolator {
public:
    static double InterpolateWindSpeed(const std::vector<WeatherObservation>& observations,
                                        const Coordinate& location);
    static double InterpolateVisibility(const std::vector<WeatherObservation>& observations,
                                         const Coordinate& location);
    static WeatherObservation InterpolateObservation(
        const std::vector<WeatherObservation>& observations,
        const Coordinate& location);
};

class OGC_ALERT_API WeatherAlertMatcher {
public:
    static std::vector<WeatherForecast> MatchAlerts(
        const std::vector<WeatherForecast>& forecasts,
        const Coordinate& location,
        const DateTime& time);
    
    static int DetermineMaxSeverity(const std::vector<WeatherForecast>& alerts);
    static std::string GenerateSummary(const std::vector<WeatherForecast>& alerts);
};

}
}

#endif
