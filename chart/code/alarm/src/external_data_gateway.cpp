#include "alert/external_data_gateway.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace alert {

ExternalDataGateway::ExternalDataGateway()
    : m_cacheEnabled(true)
    , m_cacheTTL(300)
    , m_maxRetries(3)
    , m_retryIntervalMs(1000) {
}

ExternalDataGateway::~ExternalDataGateway() {
    for (auto& pair : m_dataProviders) {
        if (pair.second) {
            pair.second->Shutdown();
        }
    }
}

DepthData ExternalDataGateway::GetDepthData(const Coordinate& position) {
    DepthData result;
    
    std::string cacheKey = GenerateCacheKey("depth", 
        std::to_string(position.GetX()) + "_" + std::to_string(position.GetY()));
    
    std::string cachedData;
    if (m_cacheEnabled && GetFromCache(cacheKey, cachedData)) {
        result.chartDepth = std::stod(cachedData);
        result.valid = true;
        result.dataSource = "cache";
        result.timestamp = GetCurrentTimestamp();
        return result;
    }
    
    auto it = m_dataSourceConfigs.find("chart");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Chart data source disabled";
        return result;
    }
    
    result.chartDepth = 10.0;
    result.tidalHeight = 0.0;
    result.depth = result.chartDepth + result.tidalHeight;
    result.valid = true;
    result.dataSource = "chart";
    result.timestamp = GetCurrentTimestamp();
    
    if (m_cacheEnabled) {
        SaveToCache(cacheKey, std::to_string(result.chartDepth));
    }
    
    return result;
}

TidalData ExternalDataGateway::GetTidalData(const Coordinate& position, 
                                             const std::string& stationId) {
    TidalData result;
    
    std::string cacheKey = GenerateCacheKey("tidal", stationId);
    
    std::string cachedData;
    if (m_cacheEnabled && GetFromCache(cacheKey, cachedData)) {
        result.height = std::stod(cachedData);
        result.valid = true;
        result.dataSource = "cache";
        result.timestamp = GetCurrentTimestamp();
        return result;
    }
    
    auto it = m_dataSourceConfigs.find("tidal");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Tidal data source disabled";
        return result;
    }
    
    result.stationId = stationId;
    result.position = position;
    result.height = 0.0;
    result.valid = true;
    result.dataSource = "tidal";
    result.timestamp = GetCurrentTimestamp();
    
    if (m_cacheEnabled) {
        SaveToCache(cacheKey, std::to_string(result.height));
    }
    
    return result;
}

std::vector<AISData> ExternalDataGateway::GetAISTargets(const Coordinate& position, 
                                                         double radiusNm) {
    std::vector<AISData> result;
    
    std::string cacheKey = GenerateCacheKey("ais_targets", 
        std::to_string(position.GetX()) + "_" + 
        std::to_string(position.GetY()) + "_" + 
        std::to_string(radiusNm));
    
    auto it = m_dataSourceConfigs.find("ais");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "AIS data source disabled";
        return result;
    }
    
    AISData target1;
    target1.mmsi = "123456789";
    target1.shipName = "TEST_SHIP_1";
    target1.position = Coordinate(position.GetX() + 0.01, position.GetY() + 0.01);
    target1.speed = 10.0;
    target1.course = 45.0;
    target1.heading = 45.0;
    target1.shipType = 70;
    target1.navStatus = 0;
    target1.distance = 0.5;
    target1.bearing = 45.0;
    target1.timestamp = GetCurrentTimestamp();
    target1.valid = true;
    result.push_back(target1);
    
    AISData target2;
    target2.mmsi = "987654321";
    target2.shipName = "TEST_SHIP_2";
    target2.position = Coordinate(position.GetX() - 0.01, position.GetY() - 0.01);
    target2.speed = 15.0;
    target2.course = 180.0;
    target2.heading = 180.0;
    target2.shipType = 60;
    target2.navStatus = 0;
    target2.distance = 0.8;
    target2.bearing = 225.0;
    target2.timestamp = GetCurrentTimestamp();
    target2.valid = true;
    result.push_back(target2);
    
    return result;
}

AISData ExternalDataGateway::GetAISTarget(const std::string& mmsi) {
    AISData result;
    
    std::string cacheKey = GenerateCacheKey("ais_target", mmsi);
    
    std::string cachedData;
    if (m_cacheEnabled && GetFromCache(cacheKey, cachedData)) {
        result.mmsi = mmsi;
        result.valid = true;
        result.dataSource = "cache";
        return result;
    }
    
    auto it = m_dataSourceConfigs.find("ais");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "AIS data source disabled";
        return result;
    }
    
    result.mmsi = mmsi;
    result.shipName = "TEST_SHIP";
    result.position = Coordinate(113.5, 30.5);
    result.speed = 12.0;
    result.course = 90.0;
    result.heading = 90.0;
    result.shipType = 70;
    result.navStatus = 0;
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    return result;
}

WeatherData ExternalDataGateway::GetWeatherData(const Coordinate& position) {
    WeatherData result;
    
    std::string cacheKey = GenerateCacheKey("weather", 
        std::to_string(position.GetX()) + "_" + std::to_string(position.GetY()));
    
    std::string cachedData;
    if (m_cacheEnabled && GetFromCache(cacheKey, cachedData)) {
        result.windSpeed = 5.0;
        result.valid = true;
        result.dataSource = "cache";
        result.timestamp = GetCurrentTimestamp();
        return result;
    }
    
    auto it = m_dataSourceConfigs.find("weather");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Weather data source disabled";
        return result;
    }
    
    result.position = position;
    result.windSpeed = 8.5;
    result.windDirection = 45.0;
    result.waveHeight = 1.2;
    result.wavePeriod = 6.0;
    result.visibility = 10.0;
    result.temperature = 22.5;
    result.pressure = 1013.25;
    result.weatherCode = "CLR";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        SaveToCache(cacheKey, std::to_string(result.windSpeed));
    }
    
    return result;
}

std::vector<WeatherAlertInfo> ExternalDataGateway::GetWeatherAlerts(const Coordinate& position) {
    std::vector<WeatherAlertInfo> result;
    
    auto it = m_dataSourceConfigs.find("weather");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Weather data source disabled";
        return result;
    }
    
    WeatherAlertInfo alert;
    alert.alertId = "WA_001";
    alert.alertType = "TYPHOON";
    alert.level = AlertLevel::kLevel2_Severe;
    alert.title = "台风预警";
    alert.content = "预计台风将在24小时内影响该区域";
    alert.issueTime = GetCurrentTimestamp();
    alert.expireTime = GetCurrentTimestamp();
    alert.affectedArea = "东经113-115, 北纬30-32";
    alert.valid = true;
    result.push_back(alert);
    
    return result;
}

ChannelData ExternalDataGateway::GetChannelData(const std::string& channelId) {
    ChannelData result;
    
    std::string cacheKey = GenerateCacheKey("channel", channelId);
    
    std::string cachedData;
    if (m_cacheEnabled && GetFromCache(cacheKey, cachedData)) {
        result.channelId = channelId;
        result.valid = true;
        result.dataSource = "cache";
        return result;
    }
    
    auto it = m_dataSourceConfigs.find("chart");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Chart data source disabled";
        return result;
    }
    
    result.channelId = channelId;
    result.channelName = "TEST_CHANNEL";
    result.width = 200.0;
    result.depth = 15.0;
    result.length = 5000.0;
    result.navStatus = "OPEN";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        SaveToCache(cacheKey, result.channelId);
    }
    
    return result;
}

std::vector<ChannelData> ExternalDataGateway::GetNearbyChannels(const Coordinate& position, 
                                                                 double radiusNm) {
    std::vector<ChannelData> result;
    
    auto it = m_dataSourceConfigs.find("chart");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Chart data source disabled";
        return result;
    }
    
    ChannelData channel;
    channel.channelId = "CH_001";
    channel.channelName = "NEARBY_CHANNEL";
    channel.width = 180.0;
    channel.depth = 12.0;
    channel.length = 3000.0;
    channel.navStatus = "OPEN";
    channel.timestamp = GetCurrentTimestamp();
    channel.valid = true;
    result.push_back(channel);
    
    return result;
}

NavAidData ExternalDataGateway::GetNavAid(const std::string& aidId) {
    NavAidData result;
    
    std::string cacheKey = GenerateCacheKey("navaid", aidId);
    
    std::string cachedData;
    if (m_cacheEnabled && GetFromCache(cacheKey, cachedData)) {
        result.aidId = aidId;
        result.valid = true;
        result.dataSource = "cache";
        return result;
    }
    
    auto it = m_dataSourceConfigs.find("chart");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Chart data source disabled";
        return result;
    }
    
    result.aidId = aidId;
    result.aidName = "TEST_BUOY";
    result.aidType = "LIGHT_BUOY";
    result.position = Coordinate(113.5, 30.5);
    result.status = "ACTIVE";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        SaveToCache(cacheKey, result.aidId);
    }
    
    return result;
}

std::vector<NavAidData> ExternalDataGateway::GetNearbyNavAids(const Coordinate& position, 
                                                               double radiusNm) {
    std::vector<NavAidData> result;
    
    auto it = m_dataSourceConfigs.find("chart");
    if (it != m_dataSourceConfigs.end() && !it->second.enabled) {
        m_lastError = "Chart data source disabled";
        return result;
    }
    
    NavAidData aid1;
    aid1.aidId = "NA_001";
    aid1.aidName = "BUOY_1";
    aid1.aidType = "LIGHT_BUOY";
    aid1.position = Coordinate(position.GetX() + 0.005, position.GetY() + 0.005);
    aid1.status = "ACTIVE";
    aid1.timestamp = GetCurrentTimestamp();
    aid1.valid = true;
    result.push_back(aid1);
    
    NavAidData aid2;
    aid2.aidId = "NA_002";
    aid2.aidName = "BUOY_2";
    aid2.aidType = "BEACON";
    aid2.position = Coordinate(position.GetX() - 0.005, position.GetY() - 0.005);
    aid2.status = "ACTIVE";
    aid2.timestamp = GetCurrentTimestamp();
    aid2.valid = true;
    result.push_back(aid2);
    
    return result;
}

void ExternalDataGateway::SetDataSourceConfig(const DataSourceConfig& config) {
    m_dataSourceConfigs[config.sourceId] = config;
    
    auto it = m_dataProviders.find(config.sourceId);
    if (it != m_dataProviders.end() && it->second) {
        it->second->Initialize(config);
    }
}

DataSourceConfig ExternalDataGateway::GetDataSourceConfig(const std::string& sourceId) {
    auto it = m_dataSourceConfigs.find(sourceId);
    if (it != m_dataSourceConfigs.end()) {
        return it->second;
    }
    return DataSourceConfig();
}

void ExternalDataGateway::SetCacheEnabled(bool enabled) {
    m_cacheEnabled = enabled;
}

void ExternalDataGateway::SetCacheTTL(int ttlSeconds) {
    m_cacheTTL = ttlSeconds;
}

void ExternalDataGateway::ClearCache() {
    m_cache.clear();
}

bool ExternalDataGateway::IsConnected(const std::string& sourceId) {
    auto it = m_dataProviders.find(sourceId);
    if (it != m_dataProviders.end() && it->second) {
        return it->second->IsConnected();
    }
    return false;
}

void ExternalDataGateway::SetConnectionCallback(const std::string& sourceId, 
                                                 std::function<void(bool)> callback) {
    m_connectionCallbacks[sourceId] = callback;
    
    auto it = m_dataProviders.find(sourceId);
    if (it != m_dataProviders.end() && it->second) {
        it->second->SetConnectionCallback(callback);
    }
}

std::string ExternalDataGateway::GetLastError() const {
    return m_lastError;
}

void ExternalDataGateway::RegisterDataProvider(const std::string& sourceId, 
                                                std::shared_ptr<IDataProvider> provider) {
    m_dataProviders[sourceId] = provider;
    
    auto configIt = m_dataSourceConfigs.find(sourceId);
    if (configIt != m_dataSourceConfigs.end() && provider) {
        provider->Initialize(configIt->second);
    }
}

void ExternalDataGateway::UnregisterDataProvider(const std::string& sourceId) {
    auto it = m_dataProviders.find(sourceId);
    if (it != m_dataProviders.end()) {
        if (it->second) {
            it->second->Shutdown();
        }
        m_dataProviders.erase(it);
    }
}

void ExternalDataGateway::SetRetryConfig(int maxRetries, int retryIntervalMs) {
    m_maxRetries = maxRetries;
    m_retryIntervalMs = retryIntervalMs;
}

std::string ExternalDataGateway::GenerateCacheKey(const std::string& sourceType, 
                                                   const std::string& params) {
    return sourceType + ":" + params;
}

bool ExternalDataGateway::GetFromCache(const std::string& key, std::string& data) {
    auto it = m_cache.find(key);
    if (it != m_cache.end() && IsCacheValid(it->second)) {
        data = it->second.data;
        return true;
    }
    return false;
}

void ExternalDataGateway::SaveToCache(const std::string& key, const std::string& data) {
    DataCacheEntry entry;
    entry.key = key;
    entry.data = data;
    entry.timestamp = GetCurrentTimestamp();
    entry.ttlSeconds = m_cacheTTL;
    entry.valid = true;
    m_cache[key] = entry;
}

bool ExternalDataGateway::IsCacheValid(const DataCacheEntry& entry) {
    if (!entry.valid) {
        return false;
    }
    
    return true;
}

std::string ExternalDataGateway::GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

}
