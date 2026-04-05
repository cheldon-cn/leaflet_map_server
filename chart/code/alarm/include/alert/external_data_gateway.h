#ifndef ALERT_EXTERNAL_DATA_GATEWAY_H
#define ALERT_EXTERNAL_DATA_GATEWAY_H

#include "alert/coordinate.h"
#include "alert/alert_types.h"
#include "alert/alert_context.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace alert {

struct DepthData {
    double depth;
    double chartDepth;
    double tidalHeight;
    double waterLevel;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    DepthData()
        : depth(0), chartDepth(0), tidalHeight(0), waterLevel(0),
          valid(false) {}
};

struct TidalData {
    std::string stationId;
    std::string stationName;
    Coordinate position;
    double height;
    std::string time;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    TidalData()
        : height(0), valid(false) {}
};

struct AISData {
    std::string mmsi;
    std::string shipName;
    Coordinate position;
    double speed;
    double course;
    double heading;
    int shipType;
    int navStatus;
    double distance;
    double bearing;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    AISData()
        : speed(0), course(0), heading(0), shipType(0),
          navStatus(0), distance(0), bearing(0), valid(false) {}
};

struct WeatherData {
    std::string stationId;
    Coordinate position;
    double windSpeed;
    double windDirection;
    double waveHeight;
    double wavePeriod;
    double visibility;
    double temperature;
    double pressure;
    std::string weatherCode;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    WeatherData()
        : windSpeed(0), windDirection(0), waveHeight(0), wavePeriod(0),
          visibility(10), temperature(20), pressure(1013.25), valid(false) {}
};

struct WeatherAlertInfo {
    std::string alertId;
    std::string alertType;
    AlertLevel level;
    std::string title;
    std::string content;
    std::string issueTime;
    std::string expireTime;
    std::string affectedArea;
    bool valid;
    
    WeatherAlertInfo()
        : level(AlertLevel::kNone), valid(false) {}
};

struct ChannelData {
    std::string channelId;
    std::string channelName;
    double width;
    double depth;
    double length;
    std::vector<Coordinate> centerLine;
    std::vector<Coordinate> boundaries;
    std::string navStatus;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    ChannelData()
        : width(0), depth(0), length(0), valid(false) {}
};

struct NavAidData {
    std::string aidId;
    std::string aidName;
    std::string aidType;
    Coordinate position;
    std::string status;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    NavAidData()
        : valid(false) {}
};

struct DataCacheEntry {
    std::string key;
    std::string data;
    std::string timestamp;
    int ttlSeconds;
    bool valid;
    
    DataCacheEntry()
        : ttlSeconds(300), valid(false) {}
};

struct DataSourceConfig {
    std::string sourceId;
    std::string sourceType;
    std::string endpoint;
    std::string apiKey;
    int timeoutMs;
    int retryCount;
    int retryIntervalMs;
    bool enabled;
    
    DataSourceConfig()
        : timeoutMs(5000), retryCount(3), retryIntervalMs(1000), enabled(true) {}
};

class IExternalDataGateway {
public:
    virtual ~IExternalDataGateway() {}
    
    virtual DepthData GetDepthData(const Coordinate& position) = 0;
    virtual TidalData GetTidalData(const Coordinate& position, const std::string& stationId) = 0;
    
    virtual std::vector<AISData> GetAISTargets(const Coordinate& position, double radiusNm) = 0;
    virtual AISData GetAISTarget(const std::string& mmsi) = 0;
    
    virtual WeatherData GetWeatherData(const Coordinate& position) = 0;
    virtual std::vector<WeatherAlertInfo> GetWeatherAlerts(const Coordinate& position) = 0;
    
    virtual ChannelData GetChannelData(const std::string& channelId) = 0;
    virtual std::vector<ChannelData> GetNearbyChannels(const Coordinate& position, double radiusNm) = 0;
    
    virtual NavAidData GetNavAid(const std::string& aidId) = 0;
    virtual std::vector<NavAidData> GetNearbyNavAids(const Coordinate& position, double radiusNm) = 0;
    
    virtual void SetDataSourceConfig(const DataSourceConfig& config) = 0;
    virtual DataSourceConfig GetDataSourceConfig(const std::string& sourceId) = 0;
    
    virtual void SetCacheEnabled(bool enabled) = 0;
    virtual void SetCacheTTL(int ttlSeconds) = 0;
    virtual void ClearCache() = 0;
    
    virtual bool IsConnected(const std::string& sourceId) = 0;
    virtual void SetConnectionCallback(const std::string& sourceId, 
                                       std::function<void(bool)> callback) = 0;
    
    virtual std::string GetLastError() const = 0;
};

class IDataProvider {
public:
    virtual ~IDataProvider() {}
    
    virtual std::string GetProviderId() const = 0;
    virtual std::string GetProviderType() const = 0;
    virtual bool Initialize(const DataSourceConfig& config) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsConnected() const = 0;
    
    virtual void SetConnectionCallback(std::function<void(bool)> callback) = 0;
};

class ExternalDataGateway : public IExternalDataGateway {
public:
    ExternalDataGateway();
    virtual ~ExternalDataGateway();
    
    DepthData GetDepthData(const Coordinate& position) override;
    TidalData GetTidalData(const Coordinate& position, const std::string& stationId) override;
    
    std::vector<AISData> GetAISTargets(const Coordinate& position, double radiusNm) override;
    AISData GetAISTarget(const std::string& mmsi) override;
    
    WeatherData GetWeatherData(const Coordinate& position) override;
    std::vector<WeatherAlertInfo> GetWeatherAlerts(const Coordinate& position) override;
    
    ChannelData GetChannelData(const std::string& channelId) override;
    std::vector<ChannelData> GetNearbyChannels(const Coordinate& position, double radiusNm) override;
    
    NavAidData GetNavAid(const std::string& aidId) override;
    std::vector<NavAidData> GetNearbyNavAids(const Coordinate& position, double radiusNm) override;
    
    void SetDataSourceConfig(const DataSourceConfig& config) override;
    DataSourceConfig GetDataSourceConfig(const std::string& sourceId) override;
    
    void SetCacheEnabled(bool enabled) override;
    void SetCacheTTL(int ttlSeconds) override;
    void ClearCache() override;
    
    bool IsConnected(const std::string& sourceId) override;
    void SetConnectionCallback(const std::string& sourceId, 
                               std::function<void(bool)> callback) override;
    
    std::string GetLastError() const override;
    
    void RegisterDataProvider(const std::string& sourceId, 
                              std::shared_ptr<IDataProvider> provider);
    void UnregisterDataProvider(const std::string& sourceId);
    
    void SetRetryConfig(int maxRetries, int retryIntervalMs);
    
private:
    std::map<std::string, DataSourceConfig> m_dataSourceConfigs;
    std::map<std::string, std::shared_ptr<IDataProvider>> m_dataProviders;
    std::map<std::string, DataCacheEntry> m_cache;
    std::map<std::string, std::function<void(bool)>> m_connectionCallbacks;
    
    bool m_cacheEnabled;
    int m_cacheTTL;
    int m_maxRetries;
    int m_retryIntervalMs;
    std::string m_lastError;
    
    std::string GenerateCacheKey(const std::string& sourceType, const std::string& params);
    bool GetFromCache(const std::string& key, std::string& data);
    void SaveToCache(const std::string& key, const std::string& data);
    bool IsCacheValid(const DataCacheEntry& entry);
    std::string GetCurrentTimestamp();
    
    template<typename T>
    T RetryOperation(std::function<T()> operation, const std::string& sourceId);
};

}

#endif
