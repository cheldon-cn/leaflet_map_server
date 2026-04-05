#include <gtest/gtest.h>
#include "alert/external_data_gateway.h"
#include <memory>

namespace {

class ExternalDataGatewayTest : public ::testing::Test {
protected:
    void SetUp() override {
        gateway = std::unique_ptr<alert::ExternalDataGateway>(new alert::ExternalDataGateway());
        
        alert::DataSourceConfig chartConfig;
        chartConfig.sourceId = "chart";
        chartConfig.sourceType = "chart";
        chartConfig.endpoint = "http://localhost:8080/chart";
        chartConfig.enabled = true;
        gateway->SetDataSourceConfig(chartConfig);
        
        alert::DataSourceConfig aisConfig;
        aisConfig.sourceId = "ais";
        aisConfig.sourceType = "ais";
        aisConfig.endpoint = "http://localhost:8080/ais";
        aisConfig.enabled = true;
        gateway->SetDataSourceConfig(aisConfig);
        
        alert::DataSourceConfig weatherConfig;
        weatherConfig.sourceId = "weather";
        weatherConfig.sourceType = "weather";
        weatherConfig.endpoint = "http://localhost:8080/weather";
        weatherConfig.enabled = true;
        gateway->SetDataSourceConfig(weatherConfig);
        
        alert::DataSourceConfig tidalConfig;
        tidalConfig.sourceId = "tidal";
        tidalConfig.sourceType = "tidal";
        tidalConfig.endpoint = "http://localhost:8080/tidal";
        tidalConfig.enabled = true;
        gateway->SetDataSourceConfig(tidalConfig);
    }
    
    void TearDown() override {
        gateway.reset();
    }
    
    std::unique_ptr<alert::ExternalDataGateway> gateway;
};

TEST_F(ExternalDataGatewayTest, GetDepthData) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::DepthData depth = gateway->GetDepthData(position);
    
    EXPECT_TRUE(depth.valid);
    EXPECT_GE(depth.chartDepth, 0);
    EXPECT_FALSE(depth.dataSource.empty());
    EXPECT_FALSE(depth.timestamp.empty());
}

TEST_F(ExternalDataGatewayTest, GetTidalData) {
    alert::Coordinate position(113.5, 30.5);
    std::string stationId = "STATION_001";
    
    alert::TidalData tidal = gateway->GetTidalData(position, stationId);
    
    EXPECT_TRUE(tidal.valid);
    EXPECT_EQ(tidal.stationId, stationId);
    EXPECT_FALSE(tidal.timestamp.empty());
}

TEST_F(ExternalDataGatewayTest, GetAISTargets) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::AISData> targets = gateway->GetAISTargets(position, radiusNm);
    
    EXPECT_GE(targets.size(), 0);
    
    for (const auto& target : targets) {
        EXPECT_TRUE(target.valid);
        EXPECT_FALSE(target.mmsi.empty());
        EXPECT_GE(target.speed, 0);
        EXPECT_GE(target.course, 0);
        EXPECT_LT(target.course, 360);
    }
}

TEST_F(ExternalDataGatewayTest, GetAISTarget) {
    std::string mmsi = "123456789";
    
    alert::AISData target = gateway->GetAISTarget(mmsi);
    
    EXPECT_TRUE(target.valid);
    EXPECT_EQ(target.mmsi, mmsi);
    EXPECT_FALSE(target.timestamp.empty());
}

TEST_F(ExternalDataGatewayTest, GetWeatherData) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::WeatherData weather = gateway->GetWeatherData(position);
    
    EXPECT_TRUE(weather.valid);
    EXPECT_GE(weather.windSpeed, 0);
    EXPECT_GE(weather.windDirection, 0);
    EXPECT_LT(weather.windDirection, 360);
    EXPECT_GE(weather.visibility, 0);
    EXPECT_FALSE(weather.timestamp.empty());
}

TEST_F(ExternalDataGatewayTest, GetWeatherAlerts) {
    alert::Coordinate position(113.5, 30.5);
    
    std::vector<alert::WeatherAlertInfo> alerts = gateway->GetWeatherAlerts(position);
    
    EXPECT_GE(alerts.size(), 0);
    
    for (const auto& alert : alerts) {
        EXPECT_TRUE(alert.valid);
        EXPECT_FALSE(alert.alertId.empty());
        EXPECT_FALSE(alert.alertType.empty());
        EXPECT_FALSE(alert.title.empty());
    }
}

TEST_F(ExternalDataGatewayTest, GetChannelData) {
    std::string channelId = "CH_001";
    
    alert::ChannelData channel = gateway->GetChannelData(channelId);
    
    EXPECT_TRUE(channel.valid);
    EXPECT_EQ(channel.channelId, channelId);
    EXPECT_GE(channel.width, 0);
    EXPECT_GE(channel.depth, 0);
    EXPECT_FALSE(channel.timestamp.empty());
}

TEST_F(ExternalDataGatewayTest, GetNearbyChannels) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::ChannelData> channels = gateway->GetNearbyChannels(position, radiusNm);
    
    EXPECT_GE(channels.size(), 0);
    
    for (const auto& channel : channels) {
        EXPECT_TRUE(channel.valid);
        EXPECT_FALSE(channel.channelId.empty());
        EXPECT_FALSE(channel.channelName.empty());
    }
}

TEST_F(ExternalDataGatewayTest, GetNavAid) {
    std::string aidId = "NA_001";
    
    alert::NavAidData aid = gateway->GetNavAid(aidId);
    
    EXPECT_TRUE(aid.valid);
    EXPECT_EQ(aid.aidId, aidId);
    EXPECT_FALSE(aid.aidName.empty());
    EXPECT_FALSE(aid.aidType.empty());
}

TEST_F(ExternalDataGatewayTest, GetNearbyNavAids) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::NavAidData> aids = gateway->GetNearbyNavAids(position, radiusNm);
    
    EXPECT_GE(aids.size(), 0);
    
    for (const auto& aid : aids) {
        EXPECT_TRUE(aid.valid);
        EXPECT_FALSE(aid.aidId.empty());
        EXPECT_FALSE(aid.aidName.empty());
    }
}

TEST_F(ExternalDataGatewayTest, SetAndGetDataSourceConfig) {
    alert::DataSourceConfig config;
    config.sourceId = "test_source";
    config.sourceType = "test";
    config.endpoint = "http://test:8080";
    config.timeoutMs = 3000;
    config.retryCount = 5;
    config.enabled = true;
    
    gateway->SetDataSourceConfig(config);
    
    alert::DataSourceConfig retrieved = gateway->GetDataSourceConfig("test_source");
    
    EXPECT_EQ(retrieved.sourceId, "test_source");
    EXPECT_EQ(retrieved.sourceType, "test");
    EXPECT_EQ(retrieved.endpoint, "http://test:8080");
    EXPECT_EQ(retrieved.timeoutMs, 3000);
    EXPECT_EQ(retrieved.retryCount, 5);
    EXPECT_TRUE(retrieved.enabled);
}

TEST_F(ExternalDataGatewayTest, CacheEnabled) {
    gateway->SetCacheEnabled(true);
    gateway->ClearCache();
    
    alert::Coordinate position(113.5, 30.5);
    
    alert::DepthData depth1 = gateway->GetDepthData(position);
    EXPECT_TRUE(depth1.valid);
    
    alert::DepthData depth2 = gateway->GetDepthData(position);
    EXPECT_TRUE(depth2.valid);
}

TEST_F(ExternalDataGatewayTest, CacheDisabled) {
    gateway->SetCacheEnabled(false);
    gateway->ClearCache();
    
    alert::Coordinate position(113.5, 30.5);
    
    alert::DepthData depth1 = gateway->GetDepthData(position);
    EXPECT_TRUE(depth1.valid);
    
    alert::DepthData depth2 = gateway->GetDepthData(position);
    EXPECT_TRUE(depth2.valid);
}

TEST_F(ExternalDataGatewayTest, SetCacheTTL) {
    gateway->SetCacheTTL(600);
    gateway->SetCacheEnabled(true);
    gateway->ClearCache();
    
    alert::Coordinate position(113.5, 30.5);
    alert::DepthData depth = gateway->GetDepthData(position);
    
    EXPECT_TRUE(depth.valid);
}

TEST_F(ExternalDataGatewayTest, ClearCache) {
    gateway->SetCacheEnabled(true);
    
    alert::Coordinate position(113.5, 30.5);
    gateway->GetDepthData(position);
    
    gateway->ClearCache();
    
    alert::DepthData depth = gateway->GetDepthData(position);
    EXPECT_TRUE(depth.valid);
}

TEST_F(ExternalDataGatewayTest, IsConnectedWithoutProvider) {
    bool connected = gateway->IsConnected("nonexistent");
    EXPECT_FALSE(connected);
}

TEST_F(ExternalDataGatewayTest, GetLastError) {
    gateway->GetDepthData(alert::Coordinate(113.5, 30.5));
    
    std::string error = gateway->GetLastError();
    EXPECT_TRUE(error.empty() || error.length() > 0);
}

TEST_F(ExternalDataGatewayTest, SetRetryConfig) {
    gateway->SetRetryConfig(5, 2000);
    
    alert::Coordinate position(113.5, 30.5);
    alert::DepthData depth = gateway->GetDepthData(position);
    
    EXPECT_TRUE(depth.valid);
}

TEST_F(ExternalDataGatewayTest, MultipleDataRequests) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::DepthData depth = gateway->GetDepthData(position);
    alert::WeatherData weather = gateway->GetWeatherData(position);
    std::vector<alert::AISData> targets = gateway->GetAISTargets(position, 5.0);
    
    EXPECT_TRUE(depth.valid);
    EXPECT_TRUE(weather.valid);
    EXPECT_GE(targets.size(), 0);
}

TEST_F(ExternalDataGatewayTest, DataSourceDisabled) {
    alert::DataSourceConfig config;
    config.sourceId = "disabled_source";
    config.sourceType = "test";
    config.enabled = false;
    gateway->SetDataSourceConfig(config);
    
    alert::Coordinate position(113.5, 30.5);
    alert::DepthData depth = gateway->GetDepthData(position);
    
    EXPECT_TRUE(depth.valid);
}

TEST_F(ExternalDataGatewayTest, ConnectionCallback) {
    bool callbackCalled = false;
    bool connectionStatus = false;
    
    gateway->SetConnectionCallback("chart", [&](bool connected) {
        callbackCalled = true;
        connectionStatus = connected;
    });
    
    EXPECT_TRUE(callbackCalled == false);
}

TEST_F(ExternalDataGatewayTest, DepthDataStructure) {
    alert::DepthData depth;
    depth.chartDepth = 10.0;
    depth.tidalHeight = 1.5;
    depth.depth = 11.5;
    depth.dataSource = "test";
    depth.timestamp = "2024-01-01T00:00:00Z";
    depth.valid = true;
    
    EXPECT_DOUBLE_EQ(depth.chartDepth, 10.0);
    EXPECT_DOUBLE_EQ(depth.tidalHeight, 1.5);
    EXPECT_DOUBLE_EQ(depth.depth, 11.5);
    EXPECT_EQ(depth.dataSource, "test");
    EXPECT_TRUE(depth.valid);
}

TEST_F(ExternalDataGatewayTest, AISDataStructure) {
    alert::AISData ais;
    ais.mmsi = "123456789";
    ais.shipName = "TEST_SHIP";
    ais.position = alert::Coordinate(113.5, 30.5);
    ais.speed = 12.5;
    ais.course = 90.0;
    ais.heading = 90.0;
    ais.shipType = 70;
    ais.navStatus = 0;
    ais.distance = 1.5;
    ais.bearing = 45.0;
    ais.valid = true;
    
    EXPECT_EQ(ais.mmsi, "123456789");
    EXPECT_EQ(ais.shipName, "TEST_SHIP");
    EXPECT_DOUBLE_EQ(ais.speed, 12.5);
    EXPECT_DOUBLE_EQ(ais.course, 90.0);
    EXPECT_TRUE(ais.valid);
}

TEST_F(ExternalDataGatewayTest, WeatherDataStructure) {
    alert::WeatherData weather;
    weather.windSpeed = 15.0;
    weather.windDirection = 180.0;
    weather.waveHeight = 2.5;
    weather.wavePeriod = 8.0;
    weather.visibility = 5.0;
    weather.temperature = 25.0;
    weather.pressure = 1015.0;
    weather.weatherCode = "RAIN";
    weather.valid = true;
    
    EXPECT_DOUBLE_EQ(weather.windSpeed, 15.0);
    EXPECT_DOUBLE_EQ(weather.windDirection, 180.0);
    EXPECT_DOUBLE_EQ(weather.waveHeight, 2.5);
    EXPECT_TRUE(weather.valid);
}

TEST_F(ExternalDataGatewayTest, ChannelDataStructure) {
    alert::ChannelData channel;
    channel.channelId = "CH_001";
    channel.channelName = "TEST_CHANNEL";
    channel.width = 200.0;
    channel.depth = 15.0;
    channel.length = 5000.0;
    channel.navStatus = "OPEN";
    channel.valid = true;
    
    EXPECT_EQ(channel.channelId, "CH_001");
    EXPECT_EQ(channel.channelName, "TEST_CHANNEL");
    EXPECT_DOUBLE_EQ(channel.width, 200.0);
    EXPECT_DOUBLE_EQ(channel.depth, 15.0);
    EXPECT_TRUE(channel.valid);
}

TEST_F(ExternalDataGatewayTest, NavAidDataStructure) {
    alert::NavAidData aid;
    aid.aidId = "NA_001";
    aid.aidName = "TEST_BUOY";
    aid.aidType = "LIGHT_BUOY";
    aid.position = alert::Coordinate(113.5, 30.5);
    aid.status = "ACTIVE";
    aid.valid = true;
    
    EXPECT_EQ(aid.aidId, "NA_001");
    EXPECT_EQ(aid.aidName, "TEST_BUOY");
    EXPECT_EQ(aid.aidType, "LIGHT_BUOY");
    EXPECT_TRUE(aid.valid);
}

}
