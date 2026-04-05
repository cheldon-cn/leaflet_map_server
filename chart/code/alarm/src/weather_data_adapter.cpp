#include "alert/weather_data_adapter.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace alert {

WeatherDataAdapter::WeatherDataAdapter()
    : m_connected(false)
    , m_cacheEnabled(true)
    , m_cacheTTL(300) {
}

WeatherDataAdapter::~WeatherDataAdapter() {
    Shutdown();
}

bool WeatherDataAdapter::Initialize(const DataSourceConfig& config) {
    m_weatherApiConfig = config;
    m_connected = true;
    
    if (m_connectionCallback) {
        m_connectionCallback(true);
    }
    
    return true;
}

void WeatherDataAdapter::Shutdown() {
    m_connected = false;
    ClearCache();
    
    if (m_connectionCallback) {
        m_connectionCallback(false);
    }
}

bool WeatherDataAdapter::IsConnected() const {
    return m_connected;
}

void WeatherDataAdapter::SetConnectionCallback(std::function<void(bool)> callback) {
    m_connectionCallback = callback;
}

WeatherData WeatherDataAdapter::GetCurrentWeather(const Coordinate& position) {
    WeatherData result;
    
    std::string cacheKey = GenerateCacheKey(position, "current");
    auto it = m_weatherCache.find(cacheKey);
    if (it != m_weatherCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    result.stationId = "WS_" + std::to_string(static_cast<int>(position.GetX() * 100)) + 
                       "_" + std::to_string(static_cast<int>(position.GetY() * 100));
    result.position = position;
    result.windSpeed = 5.0 + std::fmod(position.GetX() * 10.0, 15.0);
    result.windDirection = std::fmod(position.GetY() * 100.0, 360.0);
    result.waveHeight = 0.5 + std::fmod(position.GetX() * 5.0, 2.5);
    result.wavePeriod = 4.0 + std::fmod(position.GetY() * 3.0, 6.0);
    result.visibility = 8.0 + std::fmod(position.GetX() * 2.0, 7.0);
    result.temperature = 15.0 + std::fmod(position.GetY() * 5.0, 20.0);
    result.pressure = 1010.0 + std::fmod(position.GetX() * 2.0, 10.0);
    result.weatherCode = "01";
    result.dataSource = "weather_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        m_weatherCache[cacheKey] = result;
    }
    
    return result;
}

std::vector<WeatherAlertInfo> WeatherDataAdapter::GetWeatherAlerts(const Coordinate& position, double radiusNm) {
    std::vector<WeatherAlertInfo> result;
    
    WeatherAlertInfo alert1;
    alert1.alertId = "WA_001";
    alert1.alertType = "wind";
    alert1.level = AlertLevel::kLevel2_Severe;
    alert1.title = "Strong Wind Warning";
    alert1.content = "Wind speed over 8 grade expected in next 6 hours";
    alert1.issueTime = GetCurrentTimestamp();
    alert1.expireTime = GetCurrentTimestamp();
    alert1.affectedArea = "Yangtze River Estuary";
    alert1.valid = true;
    result.push_back(alert1);
    
    WeatherAlertInfo alert2;
    alert2.alertId = "WA_002";
    alert2.alertType = "fog";
    alert2.level = AlertLevel::kLevel3_Moderate;
    alert2.title = "Dense Fog Warning";
    alert2.content = "Visibility less than 500m expected in next 12 hours";
    alert2.issueTime = GetCurrentTimestamp();
    alert2.expireTime = GetCurrentTimestamp();
    alert2.affectedArea = "Zhoushan Islands";
    alert2.valid = true;
    result.push_back(alert2);
    
    return result;
}

WeatherAlertInfo WeatherDataAdapter::GetWeatherAlertDetail(const std::string& alertId) {
    WeatherAlertInfo result;
    
    std::string cacheKey = GenerateCacheKey(alertId, "alert");
    auto it = m_alertCache.find(cacheKey);
    if (it != m_alertCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    if (alertId == "WA_001") {
        result.alertId = alertId;
        result.alertType = "wind";
        result.level = AlertLevel::kLevel2_Severe;
        result.title = "Strong Wind Warning - Orange";
        result.content = "Wind speed over 8 grade expected in next 6 hours, gusts up to 10 grade";
        result.issueTime = GetCurrentTimestamp();
        result.expireTime = GetCurrentTimestamp();
        result.affectedArea = "Yangtze River Estuary, Zhoushan Islands";
        result.valid = true;
    } else if (alertId == "WA_002") {
        result.alertId = alertId;
        result.alertType = "fog";
        result.level = AlertLevel::kLevel3_Moderate;
        result.title = "Dense Fog Warning - Yellow";
        result.content = "Visibility less than 500m expected in next 12 hours";
        result.issueTime = GetCurrentTimestamp();
        result.expireTime = GetCurrentTimestamp();
        result.affectedArea = "Zhoushan Islands";
        result.valid = true;
    }
    
    if (m_cacheEnabled && result.valid) {
        m_alertCache[cacheKey] = result;
    }
    
    return result;
}

TyphoonData WeatherDataAdapter::GetTyphoonInfo(const std::string& typhoonId) {
    TyphoonData result;
    
    std::string cacheKey = GenerateCacheKey(typhoonId, "typhoon");
    auto it = m_typhoonCache.find(cacheKey);
    if (it != m_typhoonCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    result.typhoonId = typhoonId;
    result.typhoonName = "TYPHOON_" + typhoonId;
    result.typhoonNameCn = "Typhoon_" + typhoonId;
    result.grade = 2;
    result.centerPressure = 980.0;
    result.maxWindSpeed = 35.0;
    result.moveSpeed = 20.0;
    result.moveDirection = 315.0;
    result.currentPosition = Coordinate(125.0, 25.0);
    result.forecastTime = GetCurrentTimestamp();
    result.dataSource = "weather_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        m_typhoonCache[cacheKey] = result;
    }
    
    return result;
}

std::vector<TyphoonData> WeatherDataAdapter::GetActiveTyphoons() {
    std::vector<TyphoonData> result;
    
    TyphoonData typhoon1;
    typhoon1.typhoonId = "TY_2024_001";
    typhoon1.typhoonName = "TYPHOON_001";
    typhoon1.typhoonNameCn = "Typhoon_001";
    typhoon1.grade = 3;
    typhoon1.centerPressure = 960.0;
    typhoon1.maxWindSpeed = 45.0;
    typhoon1.moveSpeed = 25.0;
    typhoon1.moveDirection = 300.0;
    typhoon1.currentPosition = Coordinate(130.0, 20.0);
    typhoon1.forecastTime = GetCurrentTimestamp();
    typhoon1.dataSource = "weather_api";
    typhoon1.timestamp = GetCurrentTimestamp();
    typhoon1.valid = true;
    result.push_back(typhoon1);
    
    TyphoonData typhoon2;
    typhoon2.typhoonId = "TY_2024_002";
    typhoon2.typhoonName = "TYPHOON_002";
    typhoon2.typhoonNameCn = "Typhoon_002";
    typhoon2.grade = 2;
    typhoon2.centerPressure = 980.0;
    typhoon2.maxWindSpeed = 35.0;
    typhoon2.moveSpeed = 20.0;
    typhoon2.moveDirection = 315.0;
    typhoon2.currentPosition = Coordinate(125.0, 25.0);
    typhoon2.forecastTime = GetCurrentTimestamp();
    typhoon2.dataSource = "weather_api";
    typhoon2.timestamp = GetCurrentTimestamp();
    typhoon2.valid = true;
    result.push_back(typhoon2);
    
    return result;
}

TyphoonTrack WeatherDataAdapter::GetTyphoonTrack(const std::string& typhoonId) {
    TyphoonTrack result;
    result.typhoonId = typhoonId;
    
    TyphoonForecastPoint point1;
    point1.position = Coordinate(125.0, 25.0);
    point1.forecastTime = GetCurrentTimestamp();
    point1.probability = 0.7;
    point1.grade = 2;
    point1.centerPressure = 980.0;
    point1.maxWindSpeed = 35.0;
    result.forecastPoints.push_back(point1);
    
    TyphoonForecastPoint point2;
    point2.position = Coordinate(124.0, 26.0);
    point2.forecastTime = GetCurrentTimestamp();
    point2.probability = 0.5;
    point2.grade = 2;
    point2.centerPressure = 985.0;
    point2.maxWindSpeed = 32.0;
    result.forecastPoints.push_back(point2);
    
    TyphoonForecastPoint point3;
    point3.position = Coordinate(123.0, 27.0);
    point3.forecastTime = GetCurrentTimestamp();
    point3.probability = 0.3;
    point3.grade = 1;
    point3.centerPressure = 990.0;
    point3.maxWindSpeed = 28.0;
    result.forecastPoints.push_back(point3);
    
    result.warningCircle.push_back(Coordinate(124.0, 24.0));
    result.warningCircle.push_back(Coordinate(126.0, 24.0));
    result.warningCircle.push_back(Coordinate(126.0, 26.0));
    result.warningCircle.push_back(Coordinate(124.0, 26.0));
    
    result.stormCircle.push_back(Coordinate(124.5, 24.5));
    result.stormCircle.push_back(Coordinate(125.5, 24.5));
    result.stormCircle.push_back(Coordinate(125.5, 25.5));
    result.stormCircle.push_back(Coordinate(124.5, 25.5));
    
    result.valid = true;
    
    return result;
}

bool WeatherDataAdapter::IsInTyphoonWarningArea(const Coordinate& position, const std::string& typhoonId) {
    TyphoonTrack track = GetTyphoonTrack(typhoonId);
    if (!track.valid) {
        return false;
    }
    
    return IsPointInPolygon(position, track.warningCircle);
}

WeatherForecast WeatherDataAdapter::GetWeatherForecast(const Coordinate& position, int hours) {
    WeatherForecast result;
    
    std::string cacheKey = GenerateCacheKey(position, "forecast_" + std::to_string(hours));
    auto it = m_forecastCache.find(cacheKey);
    if (it != m_forecastCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    result.stationId = "WF_" + std::to_string(static_cast<int>(position.GetX() * 100));
    result.position = position;
    result.forecastTime = GetCurrentTimestamp();
    result.issueTime = GetCurrentTimestamp();
    
    for (int i = 0; i < hours / 3 && i < 8; ++i) {
        WeatherForecast::ForecastPeriod period;
        period.period = "+" + std::to_string((i + 1) * 3) + "h";
        period.weatherCode = "01";
        period.weatherDesc = "Clear";
        period.windSpeed = 5.0 + std::fmod(position.GetX() * 10.0 + i, 15.0);
        period.windDirection = std::fmod(position.GetY() * 100.0 + i * 30.0, 360.0);
        period.waveHeight = 0.5 + std::fmod(position.GetX() * 5.0 + i, 2.5);
        period.visibility = 8.0 + std::fmod(position.GetX() * 2.0 + i, 7.0);
        period.temperatureHigh = 20.0 + std::fmod(position.GetY() * 5.0 + i, 10.0);
        period.temperatureLow = 15.0 + std::fmod(position.GetY() * 5.0 + i, 8.0);
        result.periods.push_back(period);
    }
    
    result.dataSource = "weather_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        m_forecastCache[cacheKey] = result;
    }
    
    return result;
}

std::vector<WeatherForecast> WeatherDataAdapter::GetMultiDayForecast(const Coordinate& position, int days) {
    std::vector<WeatherForecast> result;
    
    for (int d = 0; d < days && d < 7; ++d) {
        WeatherForecast forecast;
        forecast.stationId = "WF_" + std::to_string(static_cast<int>(position.GetX() * 100)) + "_D" + std::to_string(d);
        forecast.position = position;
        forecast.forecastTime = GetCurrentTimestamp();
        forecast.issueTime = GetCurrentTimestamp();
        
        WeatherForecast::ForecastPeriod period;
        period.period = "Day " + std::to_string(d + 1);
        period.weatherCode = "01";
        period.weatherDesc = (d % 2 == 0) ? "Clear" : "Cloudy";
        period.windSpeed = 5.0 + std::fmod(position.GetX() * 10.0 + d, 15.0);
        period.windDirection = std::fmod(position.GetY() * 100.0 + d * 45.0, 360.0);
        period.waveHeight = 0.5 + std::fmod(position.GetX() * 5.0 + d, 2.5);
        period.visibility = 8.0 + std::fmod(position.GetX() * 2.0 + d, 7.0);
        period.temperatureHigh = 20.0 + std::fmod(position.GetY() * 5.0 + d, 10.0);
        period.temperatureLow = 15.0 + std::fmod(position.GetY() * 5.0 + d, 8.0);
        forecast.periods.push_back(period);
        
        forecast.dataSource = "weather_api";
        forecast.timestamp = GetCurrentTimestamp();
        forecast.valid = true;
        
        result.push_back(forecast);
    }
    
    return result;
}

MarineWeather WeatherDataAdapter::GetMarineWeather(const Coordinate& position) {
    MarineWeather result;
    
    result.areaId = "MW_" + std::to_string(static_cast<int>(position.GetX() * 100));
    result.areaName = "Yangtze River Estuary";
    result.boundary.push_back(Coordinate(position.GetX() - 0.5, position.GetY() - 0.5));
    result.boundary.push_back(Coordinate(position.GetX() + 0.5, position.GetY() - 0.5));
    result.boundary.push_back(Coordinate(position.GetX() + 0.5, position.GetY() + 0.5));
    result.boundary.push_back(Coordinate(position.GetX() - 0.5, position.GetY() + 0.5));
    result.weatherCode = "01";
    result.windSpeed = 5.0 + std::fmod(position.GetX() * 10.0, 15.0);
    result.windDirection = std::fmod(position.GetY() * 100.0, 360.0);
    result.waveHeight = 0.5 + std::fmod(position.GetX() * 5.0, 2.5);
    result.wavePeriod = 4.0 + std::fmod(position.GetY() * 3.0, 6.0);
    result.visibility = 8.0 + std::fmod(position.GetX() * 2.0, 7.0);
    result.issueTime = GetCurrentTimestamp();
    result.validTime = GetCurrentTimestamp();
    result.dataSource = "weather_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    return result;
}

std::vector<MarineWeather> WeatherDataAdapter::GetMarineWeatherByArea(const std::vector<std::string>& areaIds) {
    std::vector<MarineWeather> result;
    
    for (const auto& areaId : areaIds) {
        MarineWeather weather;
        weather.areaId = areaId;
        weather.areaName = "Marine_Area_" + areaId;
        weather.weatherCode = "01";
        weather.windSpeed = 5.0 + std::fmod(static_cast<double>(areaId.size() * 10), 15.0);
        weather.windDirection = std::fmod(static_cast<double>(areaId.size() * 100), 360.0);
        weather.waveHeight = 0.5 + std::fmod(static_cast<double>(areaId.size() * 5), 2.5);
        weather.wavePeriod = 4.0 + std::fmod(static_cast<double>(areaId.size() * 3), 6.0);
        weather.visibility = 8.0 + std::fmod(static_cast<double>(areaId.size() * 2), 7.0);
        weather.issueTime = GetCurrentTimestamp();
        weather.validTime = GetCurrentTimestamp();
        weather.dataSource = "weather_api";
        weather.timestamp = GetCurrentTimestamp();
        weather.valid = true;
        result.push_back(weather);
    }
    
    return result;
}

WeatherStation WeatherDataAdapter::GetNearestStation(const Coordinate& position) {
    WeatherStation result;
    
    result.stationId = "WS_" + std::to_string(static_cast<int>(position.GetX() * 100)) + 
                       "_" + std::to_string(static_cast<int>(position.GetY() * 100));
    result.stationName = "Weather_Station_" + result.stationId;
    result.position = Coordinate(position.GetX() + 0.01, position.GetY() + 0.01);
    result.altitude = 10.0;
    result.stationType = "automatic";
    result.dataSource = "weather_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    return result;
}

std::vector<WeatherStation> WeatherDataAdapter::GetNearbyStations(const Coordinate& position, double radiusNm) {
    std::vector<WeatherStation> result;
    
    WeatherStation station1;
    station1.stationId = "WS_001";
    station1.stationName = "Weather_Station_001";
    station1.position = Coordinate(position.GetX() + 0.01, position.GetY() + 0.01);
    station1.altitude = 10.0;
    station1.stationType = "automatic";
    station1.dataSource = "weather_api";
    station1.timestamp = GetCurrentTimestamp();
    station1.valid = true;
    result.push_back(station1);
    
    WeatherStation station2;
    station2.stationId = "WS_002";
    station2.stationName = "Weather_Station_002";
    station2.position = Coordinate(position.GetX() - 0.01, position.GetY() - 0.01);
    station2.altitude = 15.0;
    station2.stationType = "manual";
    station2.dataSource = "weather_api";
    station2.timestamp = GetCurrentTimestamp();
    station2.valid = true;
    result.push_back(station2);
    
    return result;
}

void WeatherDataAdapter::SetWeatherApiConfig(const DataSourceConfig& config) {
    m_weatherApiConfig = config;
}

DataSourceConfig WeatherDataAdapter::GetWeatherApiConfig() const {
    return m_weatherApiConfig;
}

void WeatherDataAdapter::SetAlertApiConfig(const DataSourceConfig& config) {
    m_alertApiConfig = config;
}

DataSourceConfig WeatherDataAdapter::GetAlertApiConfig() const {
    return m_alertApiConfig;
}

void WeatherDataAdapter::SetCacheEnabled(bool enabled) {
    m_cacheEnabled = enabled;
}

void WeatherDataAdapter::SetCacheTTL(int ttlSeconds) {
    m_cacheTTL = ttlSeconds;
}

void WeatherDataAdapter::ClearCache() {
    m_weatherCache.clear();
    m_alertCache.clear();
    m_typhoonCache.clear();
    m_forecastCache.clear();
}

std::string WeatherDataAdapter::GetLastError() const {
    return m_lastError;
}

std::string WeatherDataAdapter::GenerateCacheKey(const Coordinate& position, const std::string& type) {
    std::ostringstream oss;
    oss << type << "_" << std::fixed << std::setprecision(4) 
        << position.GetX() << "_" << position.GetY();
    return oss.str();
}

std::string WeatherDataAdapter::GenerateCacheKey(const std::string& id, const std::string& type) {
    return type + "_" + id;
}

std::string WeatherDataAdapter::GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

bool WeatherDataAdapter::IsCacheValid(const std::string& timestamp) {
    return true;
}

double WeatherDataAdapter::CalculateDistanceNm(const Coordinate& p1, const Coordinate& p2) {
    double lat1 = p1.GetY() * M_PI / 180.0;
    double lat2 = p2.GetY() * M_PI / 180.0;
    double deltaLat = (p2.GetY() - p1.GetY()) * M_PI / 180.0;
    double deltaLon = (p2.GetX() - p1.GetX()) * M_PI / 180.0;
    
    double a = std::sin(deltaLat / 2) * std::sin(deltaLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(deltaLon / 2) * std::sin(deltaLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return c * 3440.065;
}

bool WeatherDataAdapter::IsPointInPolygon(const Coordinate& point, const std::vector<Coordinate>& polygon) {
    if (polygon.size() < 3) {
        return false;
    }
    
    int n = static_cast<int>(polygon.size());
    bool inside = false;
    
    for (int i = 0, j = n - 1; i < n; j = i++) {
        double xi = polygon[i].GetX(), yi = polygon[i].GetY();
        double xj = polygon[j].GetX(), yj = polygon[j].GetY();
        
        double x = point.GetX(), y = point.GetY();
        
        if (((yi > y) != (yj > y)) &&
            (x < (xj - xi) * (y - yi) / (yj - yi) + xi)) {
            inside = !inside;
        }
    }
    
    return inside;
}

}
