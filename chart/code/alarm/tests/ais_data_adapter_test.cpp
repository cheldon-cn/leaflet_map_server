#include <gtest/gtest.h>
#include "alert/ais_data_adapter.h"

class AISDataAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        adapter = new alert::AISDataAdapter();
        
        alert::DataSourceConfig config;
        config.sourceId = "ais_api";
        config.sourceType = "rest";
        config.endpoint = "http://api.ais.example.com";
        config.enabled = true;
        
        adapter->Initialize(config);
    }
    
    void TearDown() override {
        adapter->Shutdown();
        delete adapter;
    }
    
    alert::AISDataAdapter* adapter;
};

TEST_F(AISDataAdapterTest, Initialize) {
    EXPECT_TRUE(adapter->IsConnected());
    EXPECT_EQ(adapter->GetProviderId(), "ais");
    EXPECT_EQ(adapter->GetProviderType(), "ais");
}

TEST_F(AISDataAdapterTest, GetTargetsInArea) {
    alert::Coordinate center(113.5, 30.5);
    double radiusNm = 10.0;
    
    std::vector<alert::AISData> targets = adapter->GetTargetsInArea(center, radiusNm);
    
    EXPECT_GE(targets.size(), 0);
    
    for (const auto& target : targets) {
        EXPECT_TRUE(target.valid);
        EXPECT_FALSE(target.mmsi.empty());
        EXPECT_FALSE(target.shipName.empty());
        EXPECT_GE(target.speed, 0);
        EXPECT_GE(target.course, 0);
        EXPECT_LT(target.course, 360);
        EXPECT_FALSE(target.dataSource.empty());
        EXPECT_FALSE(target.timestamp.empty());
    }
}

TEST_F(AISDataAdapterTest, GetTargetByMMSI) {
    std::string mmsi = "123456789";
    
    alert::AISData target = adapter->GetTargetByMMSI(mmsi);
    
    EXPECT_TRUE(target.valid);
    EXPECT_EQ(target.mmsi, mmsi);
    EXPECT_FALSE(target.shipName.empty());
    EXPECT_GE(target.speed, 0);
    EXPECT_FALSE(target.dataSource.empty());
    EXPECT_FALSE(target.timestamp.empty());
}

TEST_F(AISDataAdapterTest, GetTargetsByType) {
    alert::Coordinate center(113.5, 30.5);
    double radiusNm = 10.0;
    int shipType = 70;
    
    std::vector<alert::AISData> targets = adapter->GetTargetsByType(shipType, center, radiusNm);
    
    for (const auto& target : targets) {
        EXPECT_TRUE(target.valid);
        EXPECT_EQ(target.shipType, shipType);
    }
}

TEST_F(AISDataAdapterTest, GetTargetTrack) {
    std::string mmsi = "123456789";
    int hours = 2;
    
    alert::AISTargetTrack track = adapter->GetTargetTrack(mmsi, hours);
    
    EXPECT_TRUE(track.valid);
    EXPECT_EQ(track.mmsi, mmsi);
    EXPECT_GE(track.positions.size(), 0);
    EXPECT_EQ(track.positions.size(), track.timestamps.size());
    EXPECT_EQ(track.positions.size(), track.speeds.size());
    EXPECT_EQ(track.positions.size(), track.courses.size());
    EXPECT_FALSE(track.dataSource.empty());
    EXPECT_FALSE(track.timestamp.empty());
}

TEST_F(AISDataAdapterTest, GetTargetTrackByTime) {
    std::string mmsi = "123456789";
    std::string startTime = "2024-01-01T00:00:00Z";
    std::string endTime = "2024-01-01T02:00:00Z";
    
    alert::AISTargetTrack track = adapter->GetTargetTrackByTime(mmsi, startTime, endTime);
    
    EXPECT_TRUE(track.valid);
    EXPECT_EQ(track.mmsi, mmsi);
    EXPECT_GE(track.positions.size(), 0);
    EXPECT_FALSE(track.dataSource.empty());
}

TEST_F(AISDataAdapterTest, GetAreaTracks) {
    alert::Coordinate center(113.5, 30.5);
    double radiusNm = 10.0;
    int hours = 1;
    
    std::vector<alert::AISTargetTrack> tracks = adapter->GetAreaTracks(center, radiusNm, hours);
    
    EXPECT_GE(tracks.size(), 0);
    
    for (const auto& track : tracks) {
        EXPECT_TRUE(track.valid);
        EXPECT_FALSE(track.mmsi.empty());
    }
}

TEST_F(AISDataAdapterTest, GetStaticData) {
    std::string mmsi = "123456789";
    
    alert::AISStaticData staticData = adapter->GetStaticData(mmsi);
    
    EXPECT_TRUE(staticData.valid);
    EXPECT_EQ(staticData.mmsi, mmsi);
    EXPECT_FALSE(staticData.shipName.empty());
    EXPECT_FALSE(staticData.callSign.empty());
    EXPECT_GE(staticData.shipType, 0);
    EXPECT_GE(staticData.length, 0);
    EXPECT_GE(staticData.beam, 0);
    EXPECT_GE(staticData.draft, 0);
    EXPECT_FALSE(staticData.dataSource.empty());
}

TEST_F(AISDataAdapterTest, GetDynamicData) {
    std::string mmsi = "123456789";
    
    alert::AISDynamicData dynamicData = adapter->GetDynamicData(mmsi);
    
    EXPECT_TRUE(dynamicData.valid);
    EXPECT_EQ(dynamicData.mmsi, mmsi);
    EXPECT_GE(dynamicData.speed, 0);
    EXPECT_GE(dynamicData.course, 0);
    EXPECT_LT(dynamicData.course, 360);
    EXPECT_FALSE(dynamicData.dataSource.empty());
}

TEST_F(AISDataAdapterTest, GetVoyageData) {
    std::string mmsi = "123456789";
    
    alert::AISVoyageData voyageData = adapter->GetVoyageData(mmsi);
    
    EXPECT_TRUE(voyageData.valid);
    EXPECT_EQ(voyageData.mmsi, mmsi);
    EXPECT_FALSE(voyageData.destination.empty());
    EXPECT_GE(voyageData.draft, 0);
    EXPECT_FALSE(voyageData.dataSource.empty());
}

TEST_F(AISDataAdapterTest, GetTargetSummary) {
    std::string mmsi = "123456789";
    alert::Coordinate referencePosition(113.5, 30.5);
    
    alert::AISTargetSummary summary = adapter->GetTargetSummary(mmsi, referencePosition);
    
    EXPECT_TRUE(summary.valid);
    EXPECT_EQ(summary.mmsi, mmsi);
    EXPECT_FALSE(summary.shipName.empty());
    EXPECT_GE(summary.distance, 0);
    EXPECT_GE(summary.bearing, 0);
    EXPECT_LT(summary.bearing, 360);
    EXPECT_GE(summary.cpa, 0);
    EXPECT_GE(summary.tcpa, 0);
    EXPECT_FALSE(summary.dataSource.empty());
}

TEST_F(AISDataAdapterTest, GetTargetSummaries) {
    alert::Coordinate referencePosition(113.5, 30.5);
    double radiusNm = 10.0;
    
    std::vector<alert::AISTargetSummary> summaries = adapter->GetTargetSummaries(referencePosition, radiusNm);
    
    EXPECT_GE(summaries.size(), 0);
    
    for (const auto& summary : summaries) {
        EXPECT_TRUE(summary.valid);
        EXPECT_FALSE(summary.mmsi.empty());
        EXPECT_FALSE(summary.shipName.empty());
        EXPECT_GE(summary.distance, 0);
        EXPECT_GE(summary.cpa, 0);
    }
}

TEST_F(AISDataAdapterTest, GetTargetCount) {
    alert::Coordinate center(113.5, 30.5);
    double radiusNm = 10.0;
    
    int count = adapter->GetTargetCount(center, radiusNm);
    
    EXPECT_GE(count, 0);
}

TEST_F(AISDataAdapterTest, GetTargetCountByType) {
    alert::Coordinate center(113.5, 30.5);
    double radiusNm = 10.0;
    
    std::map<int, int> countByType = adapter->GetTargetCountByType(center, radiusNm);
    
    int totalCount = 0;
    for (const auto& pair : countByType) {
        EXPECT_GE(pair.first, 0);
        EXPECT_GE(pair.second, 0);
        totalCount += pair.second;
    }
    
    int directCount = adapter->GetTargetCount(center, radiusNm);
    EXPECT_EQ(totalCount, directCount);
}

TEST_F(AISDataAdapterTest, CacheEnabled) {
    adapter->SetCacheEnabled(true);
    adapter->SetCacheTTL(60);
    
    std::string mmsi = "123456789";
    
    alert::AISData target1 = adapter->GetTargetByMMSI(mmsi);
    alert::AISData target2 = adapter->GetTargetByMMSI(mmsi);
    
    EXPECT_EQ(target1.mmsi, target2.mmsi);
    EXPECT_EQ(target1.shipName, target2.shipName);
}

TEST_F(AISDataAdapterTest, ClearCache) {
    adapter->SetCacheEnabled(true);
    
    std::string mmsi = "123456789";
    adapter->GetTargetByMMSI(mmsi);
    
    adapter->ClearCache();
    
    alert::AISData target = adapter->GetTargetByMMSI(mmsi);
    EXPECT_TRUE(target.valid);
}

TEST_F(AISDataAdapterTest, SetAISApiConfig) {
    alert::DataSourceConfig config;
    config.sourceId = "custom_ais_api";
    config.sourceType = "rest";
    config.endpoint = "http://custom.ais.api";
    config.apiKey = "test_key";
    config.enabled = true;
    
    adapter->SetAISApiConfig(config);
    
    alert::DataSourceConfig retrieved = adapter->GetAISApiConfig();
    EXPECT_EQ(retrieved.sourceId, config.sourceId);
    EXPECT_EQ(retrieved.endpoint, config.endpoint);
    EXPECT_EQ(retrieved.apiKey, config.apiKey);
}

TEST_F(AISDataAdapterTest, ConnectionCallback) {
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
    config.sourceId = "ais_api";
    config.enabled = true;
    adapter->Initialize(config);
    EXPECT_TRUE(connectedStatus);
}
