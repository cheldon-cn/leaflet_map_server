#ifndef ALERT_WEATHER_DATA_ADAPTER_H
#define ALERT_WEATHER_DATA_ADAPTER_H

#include "alert/external_data_gateway.h"
#include "alert/coordinate.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace alert {

struct TyphoonData {
    std::string typhoonId;
    std::string typhoonName;
    std::string typhoonNameCn;
    int grade;
    double centerPressure;
    double maxWindSpeed;
    double moveSpeed;
    double moveDirection;
    Coordinate currentPosition;
    std::string forecastTime;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    TyphoonData()
        : grade(0), centerPressure(0), maxWindSpeed(0), 
          moveSpeed(0), moveDirection(0), valid(false) {}
};

struct TyphoonForecastPoint {
    Coordinate position;
    std::string forecastTime;
    double probability;
    int grade;
    double centerPressure;
    double maxWindSpeed;
    
    TyphoonForecastPoint()
        : probability(0), grade(0), centerPressure(0), maxWindSpeed(0) {}
};

struct TyphoonTrack {
    std::string typhoonId;
    std::vector<TyphoonForecastPoint> forecastPoints;
    std::vector<Coordinate> warningCircle;
    std::vector<Coordinate> stormCircle;
    bool valid;
    
    TyphoonTrack()
        : valid(false) {}
};

struct WeatherForecast {
    std::string stationId;
    Coordinate position;
    std::string forecastTime;
    std::string issueTime;
    
    struct ForecastPeriod {
        std::string period;
        std::string weatherCode;
        std::string weatherDesc;
        double windSpeed;
        double windDirection;
        double waveHeight;
        double visibility;
        double temperatureHigh;
        double temperatureLow;
    };
    
    std::vector<ForecastPeriod> periods;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    WeatherForecast()
        : valid(false) {}
};

struct WeatherStation {
    std::string stationId;
    std::string stationName;
    Coordinate position;
    double altitude;
    std::string stationType;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    WeatherStation()
        : altitude(0), valid(false) {}
};

struct MarineWeather {
    std::string areaId;
    std::string areaName;
    std::vector<Coordinate> boundary;
    std::string weatherCode;
    double windSpeed;
    double windDirection;
    double waveHeight;
    double wavePeriod;
    double visibility;
    std::string issueTime;
    std::string validTime;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    MarineWeather()
        : windSpeed(0), windDirection(0), waveHeight(0), 
          wavePeriod(0), visibility(10), valid(false) {}
};

class IWeatherDataAdapter : public IDataProvider {
public:
    virtual ~IWeatherDataAdapter() {}
    
    virtual WeatherData GetCurrentWeather(const Coordinate& position) = 0;
    virtual std::vector<WeatherAlertInfo> GetWeatherAlerts(const Coordinate& position, double radiusNm) = 0;
    virtual WeatherAlertInfo GetWeatherAlertDetail(const std::string& alertId) = 0;
    
    virtual TyphoonData GetTyphoonInfo(const std::string& typhoonId) = 0;
    virtual std::vector<TyphoonData> GetActiveTyphoons() = 0;
    virtual TyphoonTrack GetTyphoonTrack(const std::string& typhoonId) = 0;
    virtual bool IsInTyphoonWarningArea(const Coordinate& position, const std::string& typhoonId) = 0;
    
    virtual WeatherForecast GetWeatherForecast(const Coordinate& position, int hours) = 0;
    virtual std::vector<WeatherForecast> GetMultiDayForecast(const Coordinate& position, int days) = 0;
    
    virtual MarineWeather GetMarineWeather(const Coordinate& position) = 0;
    virtual std::vector<MarineWeather> GetMarineWeatherByArea(const std::vector<std::string>& areaIds) = 0;
    
    virtual WeatherStation GetNearestStation(const Coordinate& position) = 0;
    virtual std::vector<WeatherStation> GetNearbyStations(const Coordinate& position, double radiusNm) = 0;
    
    virtual void SetWeatherApiConfig(const DataSourceConfig& config) = 0;
    virtual DataSourceConfig GetWeatherApiConfig() const = 0;
    
    virtual void SetAlertApiConfig(const DataSourceConfig& config) = 0;
    virtual DataSourceConfig GetAlertApiConfig() const = 0;
    
    virtual void SetCacheEnabled(bool enabled) = 0;
    virtual void SetCacheTTL(int ttlSeconds) = 0;
    virtual void ClearCache() = 0;
};

class WeatherDataAdapter : public IWeatherDataAdapter {
public:
    WeatherDataAdapter();
    virtual ~WeatherDataAdapter();
    
    std::string GetProviderId() const override { return "weather"; }
    std::string GetProviderType() const override { return "weather"; }
    
    bool Initialize(const DataSourceConfig& config) override;
    void Shutdown() override;
    bool IsConnected() const override;
    void SetConnectionCallback(std::function<void(bool)> callback) override;
    
    WeatherData GetCurrentWeather(const Coordinate& position) override;
    std::vector<WeatherAlertInfo> GetWeatherAlerts(const Coordinate& position, double radiusNm) override;
    WeatherAlertInfo GetWeatherAlertDetail(const std::string& alertId) override;
    
    TyphoonData GetTyphoonInfo(const std::string& typhoonId) override;
    std::vector<TyphoonData> GetActiveTyphoons() override;
    TyphoonTrack GetTyphoonTrack(const std::string& typhoonId) override;
    bool IsInTyphoonWarningArea(const Coordinate& position, const std::string& typhoonId) override;
    
    WeatherForecast GetWeatherForecast(const Coordinate& position, int hours) override;
    std::vector<WeatherForecast> GetMultiDayForecast(const Coordinate& position, int days) override;
    
    MarineWeather GetMarineWeather(const Coordinate& position) override;
    std::vector<MarineWeather> GetMarineWeatherByArea(const std::vector<std::string>& areaIds) override;
    
    WeatherStation GetNearestStation(const Coordinate& position) override;
    std::vector<WeatherStation> GetNearbyStations(const Coordinate& position, double radiusNm) override;
    
    void SetWeatherApiConfig(const DataSourceConfig& config) override;
    DataSourceConfig GetWeatherApiConfig() const override;
    
    void SetAlertApiConfig(const DataSourceConfig& config) override;
    DataSourceConfig GetAlertApiConfig() const override;
    
    void SetCacheEnabled(bool enabled) override;
    void SetCacheTTL(int ttlSeconds) override;
    void ClearCache() override;
    
    std::string GetLastError() const;
    
private:
    DataSourceConfig m_weatherApiConfig;
    DataSourceConfig m_alertApiConfig;
    std::function<void(bool)> m_connectionCallback;
    bool m_connected;
    bool m_cacheEnabled;
    int m_cacheTTL;
    std::string m_lastError;
    
    std::map<std::string, WeatherData> m_weatherCache;
    std::map<std::string, WeatherAlertInfo> m_alertCache;
    std::map<std::string, TyphoonData> m_typhoonCache;
    std::map<std::string, WeatherForecast> m_forecastCache;
    
    std::string GenerateCacheKey(const Coordinate& position, const std::string& type);
    std::string GenerateCacheKey(const std::string& id, const std::string& type);
    std::string GetCurrentTimestamp();
    bool IsCacheValid(const std::string& timestamp);
    double CalculateDistanceNm(const Coordinate& p1, const Coordinate& p2);
    bool IsPointInPolygon(const Coordinate& point, const std::vector<Coordinate>& polygon);
};

}

#endif
