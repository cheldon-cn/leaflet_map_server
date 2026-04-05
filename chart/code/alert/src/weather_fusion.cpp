#include "ogc/alert/weather_fusion.h"
#include "ogc/draw/log.h"
#include "ogc/point.h"
#include <cmath>
#include <algorithm>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

namespace {
const double kEarthRadiusKm = 6371.0;
const double kMaxObservationDistanceKm = 100.0;
const int kMaxObservationAgeSeconds = 3600;

double HaversineDistance(const Coordinate& from, const Coordinate& to) {
    double lat1 = from.latitude * M_PI / 180.0;
    double lat2 = to.latitude * M_PI / 180.0;
    double dLat = (to.latitude - from.latitude) * M_PI / 180.0;
    double dLon = (to.longitude - from.longitude) * M_PI / 180.0;
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return kEarthRadiusKm * c;
}

double DegreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double RadiansToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

}

WeatherFusion::WeatherFusion() {
}

WeatherFusion::~WeatherFusion() {
}

void WeatherFusion::AddObservation(const WeatherObservation& observation) {
    m_observations.push_back(observation);
}

void WeatherFusion::AddForecast(const WeatherForecast& forecast) {
    m_forecasts.push_back(forecast);
}

WeatherFusionResult WeatherFusion::Fuse(const Coordinate& location, 
                                         const DateTime& time) {
    WeatherFusionResult result;
    result.fusion_time = time;
    result.location = location;
    result.fused_wind_speed = 0.0;
    result.fused_wind_direction = 0.0;
    result.fused_visibility = 0.0;
    result.fused_precipitation = 0.0;
    result.fused_wave_height = 0.0;
    result.overall_confidence = 0.0;
    result.max_severity = 0;
    
    std::vector<std::pair<double, double>> windSpeedValues;
    std::vector<std::pair<double, double>> windDirValues;
    std::vector<std::pair<double, double>> visibilityValues;
    std::vector<std::pair<double, double>> precipitationValues;
    std::vector<std::pair<double, double>> waveHeightValues;
    
    for (const auto& obs : m_observations) {
        double distanceWeight = CalculateDistanceWeight(obs.location, location, 
                                                         kMaxObservationDistanceKm);
        double timeWeight = CalculateTimeWeight(obs.observation_time, time, 
                                                 kMaxObservationAgeSeconds);
        
        auto configIt = m_sourceConfigs.find(obs.source_id);
        double sourceWeight = configIt != m_sourceConfigs.end() ? 
                              configIt->second.weight : 1.0;
        
        double totalWeight = distanceWeight * timeWeight * sourceWeight * obs.confidence;
        
        if (totalWeight > 0.01) {
            if (obs.wind_speed >= 0) {
                windSpeedValues.push_back({obs.wind_speed, totalWeight});
            }
            if (obs.wind_direction >= 0) {
                windDirValues.push_back({obs.wind_direction, totalWeight});
            }
            if (obs.visibility >= 0) {
                visibilityValues.push_back({obs.visibility, totalWeight});
            }
            if (obs.precipitation >= 0) {
                precipitationValues.push_back({obs.precipitation, totalWeight});
            }
            if (obs.wave_height >= 0) {
                waveHeightValues.push_back({obs.wave_height, totalWeight});
            }
        }
    }
    
    result.fused_wind_speed = FuseValue(windSpeedValues);
    result.fused_wind_direction = FuseValue(windDirValues);
    result.fused_visibility = FuseValue(visibilityValues);
    result.fused_precipitation = FuseValue(precipitationValues);
    result.fused_wave_height = FuseValue(waveHeightValues);
    
    double totalWeight = 0.0;
    for (const auto& v : windSpeedValues) {
        totalWeight += v.second;
    }
    result.overall_confidence = totalWeight > 0 ? std::min(totalWeight, 1.0) : 0.0;
    
    result.active_alerts = GetActiveAlerts(location, time);
    result.max_severity = WeatherAlertMatcher::DetermineMaxSeverity(result.active_alerts);
    result.summary = WeatherAlertMatcher::GenerateSummary(result.active_alerts);
    
    return result;
}

std::vector<WeatherForecast> WeatherFusion::GetActiveAlerts(
    const Coordinate& location, const DateTime& time) {
    return WeatherAlertMatcher::MatchAlerts(m_forecasts, location, time);
}

void WeatherFusion::SetSourceConfig(const WeatherSourceConfig& config) {
    m_sourceConfigs[config.source_id] = config;
}

WeatherSourceConfig WeatherFusion::GetSourceConfig(const std::string& source_id) const {
    auto it = m_sourceConfigs.find(source_id);
    if (it != m_sourceConfigs.end()) {
        return it->second;
    }
    return WeatherSourceConfig();
}

void WeatherFusion::RemoveExpiredData(const DateTime& cutoff_time) {
    m_observations.erase(
        std::remove_if(m_observations.begin(), m_observations.end(),
            [&cutoff_time](const WeatherObservation& obs) {
                return obs.observation_time < cutoff_time;
            }),
        m_observations.end());
    
    m_forecasts.erase(
        std::remove_if(m_forecasts.begin(), m_forecasts.end(),
            [&cutoff_time](const WeatherForecast& forecast) {
                return forecast.valid_to < cutoff_time;
            }),
        m_forecasts.end());
}

void WeatherFusion::Clear() {
    m_observations.clear();
    m_forecasts.clear();
}

double WeatherFusion::CalculateDistanceWeight(const Coordinate& obs_location,
                                               const Coordinate& target_location,
                                               double max_distance_km) const {
    double distance = HaversineDistance(obs_location, target_location);
    if (distance > max_distance_km) {
        return 0.0;
    }
    return 1.0 - (distance / max_distance_km);
}

double WeatherFusion::CalculateTimeWeight(const DateTime& obs_time,
                                           const DateTime& target_time,
                                           int max_age_seconds) const {
    int64_t age_seconds = target_time.ToTimestamp() - obs_time.ToTimestamp();
    if (age_seconds < 0 || age_seconds > max_age_seconds) {
        return 0.0;
    }
    return 1.0 - (static_cast<double>(age_seconds) / max_age_seconds);
}

double WeatherFusion::FuseValue(const std::vector<std::pair<double, double>>& value_weights) const {
    if (value_weights.empty()) {
        return 0.0;
    }
    
    double weighted_sum = 0.0;
    double total_weight = 0.0;
    
    for (const auto& vw : value_weights) {
        weighted_sum += vw.first * vw.second;
        total_weight += vw.second;
    }
    
    if (total_weight > 0) {
        return weighted_sum / total_weight;
    }
    return 0.0;
}

bool WeatherFusion::IsLocationInArea(const Coordinate& location, const ogc::GeometrySharedPtr& area) const {
    if (area && area->IsValid()) {
        auto point = ogc::Point::Create(location.longitude, location.latitude);
        return area->Contains(point.get());
    }
    return false;
}

bool WeatherFusion::IsForecastActive(const WeatherForecast& forecast, const DateTime& time) const {
    return time >= forecast.valid_from && time <= forecast.valid_to;
}

std::unique_ptr<IWeatherFusion> IWeatherFusion::Create() {
    return std::unique_ptr<IWeatherFusion>(new WeatherFusion());
}

double WeatherQualityAssessor::CalculateObservationQuality(const WeatherObservation& obs) {
    double quality = 1.0;
    
    if (obs.wind_speed < 0 || obs.wind_speed > 100) {
        quality *= 0.5;
    }
    if (obs.visibility < 0 || obs.visibility > 50000) {
        quality *= 0.5;
    }
    if (obs.confidence < 0 || obs.confidence > 1) {
        quality *= 0.5;
    }
    
    return quality;
}

double WeatherQualityAssessor::CalculateForecastReliability(const WeatherForecast& forecast) {
    double reliability = forecast.probability;
    
    int64_t duration = forecast.valid_to.ToTimestamp() - forecast.valid_from.ToTimestamp();
    if (duration > 86400) {
        reliability *= 0.8;
    }
    
    return reliability;
}

bool WeatherQualityAssessor::ValidateObservation(const WeatherObservation& obs) {
    if (obs.source_id.empty()) {
        return false;
    }
    if (obs.confidence < 0 || obs.confidence > 1) {
        return false;
    }
    return true;
}

bool WeatherQualityAssessor::ValidateForecast(const WeatherForecast& forecast) {
    if (forecast.forecast_id.empty()) {
        return false;
    }
    if (forecast.probability < 0 || forecast.probability > 1) {
        return false;
    }
    if (forecast.valid_from.ToTimestamp() > forecast.valid_to.ToTimestamp()) {
        return false;
    }
    return true;
}

double WeatherInterpolator::InterpolateWindSpeed(
    const std::vector<WeatherObservation>& observations,
    const Coordinate& location) {
    if (observations.empty()) {
        return 0.0;
    }
    
    double weighted_sum = 0.0;
    double total_weight = 0.0;
    
    for (const auto& obs : observations) {
        double distance = HaversineDistance(obs.location, location);
        if (distance < 0.001) {
            return obs.wind_speed;
        }
        double weight = 1.0 / (distance * distance);
        weighted_sum += obs.wind_speed * weight;
        total_weight += weight;
    }
    
    return total_weight > 0 ? weighted_sum / total_weight : 0.0;
}

double WeatherInterpolator::InterpolateVisibility(
    const std::vector<WeatherObservation>& observations,
    const Coordinate& location) {
    if (observations.empty()) {
        return 10000.0;
    }
    
    double weighted_sum = 0.0;
    double total_weight = 0.0;
    
    for (const auto& obs : observations) {
        double distance = HaversineDistance(obs.location, location);
        if (distance < 0.001) {
            return obs.visibility;
        }
        double weight = 1.0 / (distance * distance);
        weighted_sum += obs.visibility * weight;
        total_weight += weight;
    }
    
    return total_weight > 0 ? weighted_sum / total_weight : 10000.0;
}

WeatherObservation WeatherInterpolator::InterpolateObservation(
    const std::vector<WeatherObservation>& observations,
    const Coordinate& location) {
    WeatherObservation result;
    result.location = location;
    result.wind_speed = InterpolateWindSpeed(observations, location);
    result.visibility = InterpolateVisibility(observations, location);
    result.confidence = 0.8;
    return result;
}

std::vector<WeatherForecast> WeatherAlertMatcher::MatchAlerts(
    const std::vector<WeatherForecast>& forecasts,
    const Coordinate& location,
    const DateTime& time) {
    std::vector<WeatherForecast> matched;
    
    for (const auto& forecast : forecasts) {
        if (time >= forecast.valid_from && time <= forecast.valid_to) {
            if (forecast.affected_area && forecast.affected_area->IsValid()) {
                auto point = ogc::Point::Create(location.longitude, location.latitude);
                if (forecast.affected_area->Contains(point.get())) {
                    matched.push_back(forecast);
                }
            } else {
                matched.push_back(forecast);
            }
        }
    }
    
    std::sort(matched.begin(), matched.end(),
        [](const WeatherForecast& a, const WeatherForecast& b) {
            return a.severity > b.severity;
        });
    
    return matched;
}

int WeatherAlertMatcher::DetermineMaxSeverity(const std::vector<WeatherForecast>& alerts) {
    int max_severity = 0;
    for (const auto& alert : alerts) {
        if (alert.severity > max_severity) {
            max_severity = alert.severity;
        }
    }
    return max_severity;
}

std::string WeatherAlertMatcher::GenerateSummary(const std::vector<WeatherForecast>& alerts) {
    if (alerts.empty()) {
        return "No active weather alerts";
    }
    
    std::ostringstream oss;
    oss << alerts.size() << " active weather alert(s)";
    
    int max_severity = DetermineMaxSeverity(alerts);
    if (max_severity > 0) {
        oss << ", max severity: " << max_severity;
    }
    
    return oss.str();
}

}
}
