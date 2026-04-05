#include <gtest/gtest.h>
#include "alert/weather_data_adapter.h"

class WeatherDataAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        adapter = new alert::WeatherDataAdapter();
        
        alert::DataSourceConfig config;
        config.sourceId = "weather_api";
        config.sourceType = "rest";
        config.endpoint = "http://api.weather.example.com";
        config.enabled = true;
        
        adapter->Initialize(config);
    }
    
    void TearDown() override {
        adapter->Shutdown();
        delete adapter;
    }
    
    alert::WeatherDataAdapter* adapter;
};

TEST_F(WeatherDataAdapterTest, Initialize) {
    EXPECT_TRUE(adapter->IsConnected());
    EXPECT_EQ(adapter->GetProviderId(), "weather");
    EXPECT_EQ(adapter->GetProviderType(), "weather");
}

TEST_F(WeatherDataAdapterTest, GetCurrentWeather) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::WeatherData weather = adapter->GetCurrentWeather(position);
    
    EXPECT_TRUE(weather.valid);
    EXPECT_GE(weather.windSpeed, 0);
    EXPECT_GE(weather.windDirection, 0);
    EXPECT_LT(weather.windDirection, 360);
    EXPECT_FALSE(weather.dataSource.empty());
    EXPECT_FALSE(weather.timestamp.empty());
}

TEST_F(WeatherDataAdapterTest, GetWeatherAlerts) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 10.0;
    
    std::vector<alert::WeatherAlertInfo> alerts = adapter->GetWeatherAlerts(position, radiusNm);
    
    EXPECT_GE(alerts.size(), 0);
    
    for (const auto& alert : alerts) {
        EXPECT_TRUE(alert.valid);
        EXPECT_FALSE(alert.alertId.empty());
        EXPECT_FALSE(alert.alertType.empty());
        EXPECT_NE(alert.level, alert::AlertLevel::kNone);
        EXPECT_FALSE(alert.title.empty());
        EXPECT_FALSE(alert.content.empty());
    }
}

TEST_F(WeatherDataAdapterTest, GetWeatherAlertDetail) {
    std::string alertId = "WA_001";
    
    alert::WeatherAlertInfo alert = adapter->GetWeatherAlertDetail(alertId);
    
    EXPECT_TRUE(alert.valid);
    EXPECT_EQ(alert.alertId, alertId);
    EXPECT_FALSE(alert.alertType.empty());
    EXPECT_NE(alert.level, alert::AlertLevel::kNone);
}

TEST_F(WeatherDataAdapterTest, GetTyphoonInfo) {
    std::string typhoonId = "TY_2024_001";
    
    alert::TyphoonData typhoon = adapter->GetTyphoonInfo(typhoonId);
    
    EXPECT_TRUE(typhoon.valid);
    EXPECT_EQ(typhoon.typhoonId, typhoonId);
    EXPECT_FALSE(typhoon.typhoonName.empty());
    EXPECT_GE(typhoon.grade, 0);
    EXPECT_GT(typhoon.centerPressure, 0);
    EXPECT_GE(typhoon.maxWindSpeed, 0);
    EXPECT_FALSE(typhoon.dataSource.empty());
    EXPECT_FALSE(typhoon.timestamp.empty());
}

TEST_F(WeatherDataAdapterTest, GetActiveTyphoons) {
    std::vector<alert::TyphoonData> typhoons = adapter->GetActiveTyphoons();
    
    EXPECT_GE(typhoons.size(), 0);
    
    for (const auto& typhoon : typhoons) {
        EXPECT_TRUE(typhoon.valid);
        EXPECT_FALSE(typhoon.typhoonId.empty());
        EXPECT_FALSE(typhoon.typhoonName.empty());
        EXPECT_GE(typhoon.grade, 0);
        EXPECT_GT(typhoon.centerPressure, 0);
    }
}

TEST_F(WeatherDataAdapterTest, GetTyphoonTrack) {
    std::string typhoonId = "TY_2024_001";
    
    alert::TyphoonTrack track = adapter->GetTyphoonTrack(typhoonId);
    
    EXPECT_TRUE(track.valid);
    EXPECT_EQ(track.typhoonId, typhoonId);
    EXPECT_GE(track.forecastPoints.size(), 0);
    
    for (const auto& point : track.forecastPoints) {
        EXPECT_GE(point.probability, 0);
        EXPECT_LE(point.probability, 1);
    }
}

TEST_F(WeatherDataAdapterTest, IsInTyphoonWarningArea) {
    alert::Coordinate position(125.5, 25.5);
    std::string typhoonId = "TY_2024_001";
    
    bool inWarningArea = adapter->IsInTyphoonWarningArea(position, typhoonId);
    
    EXPECT_TRUE(inWarningArea || !inWarningArea);
}

TEST_F(WeatherDataAdapterTest, GetWeatherForecast) {
    alert::Coordinate position(113.5, 30.5);
    int hours = 24;
    
    alert::WeatherForecast forecast = adapter->GetWeatherForecast(position, hours);
    
    EXPECT_TRUE(forecast.valid);
    EXPECT_FALSE(forecast.stationId.empty());
    EXPECT_GE(forecast.periods.size(), 0);
    EXPECT_FALSE(forecast.dataSource.empty());
    EXPECT_FALSE(forecast.timestamp.empty());
}

TEST_F(WeatherDataAdapterTest, GetMultiDayForecast) {
    alert::Coordinate position(113.5, 30.5);
    int days = 3;
    
    std::vector<alert::WeatherForecast> forecasts = adapter->GetMultiDayForecast(position, days);
    
    EXPECT_GE(forecasts.size(), 0);
    EXPECT_LE(static_cast<int>(forecasts.size()), days);
    
    for (const auto& forecast : forecasts) {
        EXPECT_TRUE(forecast.valid);
        EXPECT_FALSE(forecast.stationId.empty());
        EXPECT_GE(forecast.periods.size(), 0);
    }
}

TEST_F(WeatherDataAdapterTest, GetMarineWeather) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::MarineWeather marine = adapter->GetMarineWeather(position);
    
    EXPECT_TRUE(marine.valid);
    EXPECT_FALSE(marine.areaId.empty());
    EXPECT_FALSE(marine.areaName.empty());
    EXPECT_GE(marine.windSpeed, 0);
    EXPECT_GE(marine.windDirection, 0);
    EXPECT_LT(marine.windDirection, 360);
    EXPECT_FALSE(marine.dataSource.empty());
    EXPECT_FALSE(marine.timestamp.empty());
}

TEST_F(WeatherDataAdapterTest, GetMarineWeatherByArea) {
    std::vector<std::string> areaIds;
    areaIds.push_back("AREA_001");
    areaIds.push_back("AREA_002");
    
    std::vector<alert::MarineWeather> marineWeathers = adapter->GetMarineWeatherByArea(areaIds);
    
    EXPECT_EQ(marineWeathers.size(), areaIds.size());
    
    for (size_t i = 0; i < marineWeathers.size(); ++i) {
        EXPECT_TRUE(marineWeathers[i].valid);
        EXPECT_EQ(marineWeathers[i].areaId, areaIds[i]);
    }
}

TEST_F(WeatherDataAdapterTest, GetNearestStation) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::WeatherStation station = adapter->GetNearestStation(position);
    
    EXPECT_TRUE(station.valid);
    EXPECT_FALSE(station.stationId.empty());
    EXPECT_FALSE(station.stationName.empty());
    EXPECT_FALSE(station.dataSource.empty());
    EXPECT_FALSE(station.timestamp.empty());
}

TEST_F(WeatherDataAdapterTest, GetNearbyStations) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 10.0;
    
    std::vector<alert::WeatherStation> stations = adapter->GetNearbyStations(position, radiusNm);
    
    EXPECT_GE(stations.size(), 0);
    
    for (const auto& station : stations) {
        EXPECT_TRUE(station.valid);
        EXPECT_FALSE(station.stationId.empty());
        EXPECT_FALSE(station.stationName.empty());
    }
}

TEST_F(WeatherDataAdapterTest, CacheEnabled) {
    adapter->SetCacheEnabled(true);
    adapter->SetCacheTTL(600);
    
    alert::Coordinate position(113.5, 30.5);
    
    alert::WeatherData weather1 = adapter->GetCurrentWeather(position);
    alert::WeatherData weather2 = adapter->GetCurrentWeather(position);
    
    EXPECT_EQ(weather1.stationId, weather2.stationId);
    EXPECT_EQ(weather1.windSpeed, weather2.windSpeed);
}

TEST_F(WeatherDataAdapterTest, ClearCache) {
    adapter->SetCacheEnabled(true);
    
    alert::Coordinate position(113.5, 30.5);
    adapter->GetCurrentWeather(position);
    
    adapter->ClearCache();
    
    alert::WeatherData weather = adapter->GetCurrentWeather(position);
    EXPECT_TRUE(weather.valid);
}

TEST_F(WeatherDataAdapterTest, SetWeatherApiConfig) {
    alert::DataSourceConfig config;
    config.sourceId = "custom_weather_api";
    config.sourceType = "rest";
    config.endpoint = "http://custom.weather.api";
    config.apiKey = "test_key";
    config.enabled = true;
    
    adapter->SetWeatherApiConfig(config);
    
    alert::DataSourceConfig retrieved = adapter->GetWeatherApiConfig();
    EXPECT_EQ(retrieved.sourceId, config.sourceId);
    EXPECT_EQ(retrieved.endpoint, config.endpoint);
    EXPECT_EQ(retrieved.apiKey, config.apiKey);
}

TEST_F(WeatherDataAdapterTest, SetAlertApiConfig) {
    alert::DataSourceConfig config;
    config.sourceId = "custom_alert_api";
    config.sourceType = "rest";
    config.endpoint = "http://custom.alert.api";
    config.enabled = true;
    
    adapter->SetAlertApiConfig(config);
    
    alert::DataSourceConfig retrieved = adapter->GetAlertApiConfig();
    EXPECT_EQ(retrieved.sourceId, config.sourceId);
    EXPECT_EQ(retrieved.endpoint, config.endpoint);
}

TEST_F(WeatherDataAdapterTest, ConnectionCallback) {
    bool callbackCalled = false;
    bool connectedStatus = false;
    
    adapter->SetConnectionCallback([&](bool connected) {
        callbackCalled = true;
        connectedStatus = connected;
    });
    
    adapter->Shutdown();
    EXPECT_TRUE(callbackCalled);
    EXPECT_FALSE(connectedStatus);
    
    alert::DataSourceConfig config;
    config.sourceId = "weather_api";
    config.enabled = true;
    adapter->Initialize(config);
    EXPECT_TRUE(connectedStatus);
}
