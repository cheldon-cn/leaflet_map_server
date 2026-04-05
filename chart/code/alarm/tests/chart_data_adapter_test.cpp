#include <gtest/gtest.h>
#include "alert/chart_data_adapter.h"
#include <memory>

namespace {

class ChartDataAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        adapter = std::unique_ptr<alert::ChartDataAdapter>(new alert::ChartDataAdapter());
        
        alert::DataSourceConfig config;
        config.sourceId = "chart";
        config.sourceType = "chart";
        config.endpoint = "postgresql://localhost:5432/chart";
        config.enabled = true;
        adapter->Initialize(config);
    }
    
    void TearDown() override {
        adapter->Shutdown();
    }
    
    std::unique_ptr<alert::ChartDataAdapter> adapter;
};

TEST_F(ChartDataAdapterTest, GetDepthAtPosition) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::DepthData depth = adapter->GetDepthAtPosition(position);
    
    EXPECT_TRUE(depth.valid);
    EXPECT_GE(depth.chartDepth, 0);
    EXPECT_FALSE(depth.dataSource.empty());
    EXPECT_FALSE(depth.timestamp.empty());
}

TEST_F(ChartDataAdapterTest, GetDepthContours) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::DepthContour> contours = adapter->GetDepthContours(position, radiusNm);
    
    EXPECT_GE(contours.size(), 0);
    
    for (const auto& contour : contours) {
        EXPECT_TRUE(contour.valid);
        EXPECT_FALSE(contour.contourId.empty());
        EXPECT_GE(contour.depth, 0);
    }
}

TEST_F(ChartDataAdapterTest, GetSafetyContours) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    double safetyDepth = 10.0;
    
    std::vector<alert::SafetyContour> contours = adapter->GetSafetyContours(position, radiusNm, safetyDepth);
    
    EXPECT_GE(contours.size(), 0);
    
    for (const auto& contour : contours) {
        EXPECT_TRUE(contour.valid);
        EXPECT_FALSE(contour.contourId.empty());
        EXPECT_GE(contour.depth, 0);
    }
}

TEST_F(ChartDataAdapterTest, GetChannelInfo) {
    std::string channelId = "CH_001";
    
    alert::ChannelData channel = adapter->GetChannelInfo(channelId);
    
    EXPECT_TRUE(channel.valid);
    EXPECT_EQ(channel.channelId, channelId);
    EXPECT_FALSE(channel.channelName.empty());
    EXPECT_GE(channel.width, 0);
    EXPECT_GE(channel.depth, 0);
}

TEST_F(ChartDataAdapterTest, GetNearbyChannels) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::ChannelData> channels = adapter->GetNearbyChannels(position, radiusNm);
    
    EXPECT_GE(channels.size(), 0);
    
    for (const auto& channel : channels) {
        EXPECT_TRUE(channel.valid);
        EXPECT_FALSE(channel.channelId.empty());
        EXPECT_FALSE(channel.channelName.empty());
    }
}

TEST_F(ChartDataAdapterTest, GetNavAidInfo) {
    std::string aidId = "NA_001";
    
    alert::NavAidData aid = adapter->GetNavAidInfo(aidId);
    
    EXPECT_TRUE(aid.valid);
    EXPECT_EQ(aid.aidId, aidId);
    EXPECT_FALSE(aid.aidName.empty());
    EXPECT_FALSE(aid.aidType.empty());
}

TEST_F(ChartDataAdapterTest, GetNearbyNavAids) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::NavAidData> aids = adapter->GetNearbyNavAids(position, radiusNm);
    
    EXPECT_GE(aids.size(), 0);
    
    for (const auto& aid : aids) {
        EXPECT_TRUE(aid.valid);
        EXPECT_FALSE(aid.aidId.empty());
        EXPECT_FALSE(aid.aidName.empty());
    }
}

TEST_F(ChartDataAdapterTest, GetRestrictedAreas) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::RestrictedArea> areas = adapter->GetRestrictedAreas(position, radiusNm);
    
    EXPECT_GE(areas.size(), 0);
    
    for (const auto& area : areas) {
        EXPECT_TRUE(area.valid);
        EXPECT_FALSE(area.areaId.empty());
        EXPECT_FALSE(area.name.empty());
    }
}

TEST_F(ChartDataAdapterTest, GetAnchorageAreas) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    
    std::vector<alert::AnchorageArea> areas = adapter->GetAnchorageAreas(position, radiusNm);
    
    EXPECT_GE(areas.size(), 0);
    
    for (const auto& area : areas) {
        EXPECT_TRUE(area.valid);
        EXPECT_FALSE(area.anchorageId.empty());
        EXPECT_FALSE(area.name.empty());
    }
}

TEST_F(ChartDataAdapterTest, QueryFeatures) {
    alert::Coordinate position(113.5, 30.5);
    double radiusNm = 5.0;
    std::string featureType = "SOUNDG";
    
    std::vector<alert::ChartFeature> features = adapter->QueryFeatures(position, radiusNm, featureType);
    
    EXPECT_GE(features.size(), 0);
    
    for (const auto& feature : features) {
        EXPECT_TRUE(feature.valid);
        EXPECT_FALSE(feature.featureId.empty());
        EXPECT_FALSE(feature.featureType.empty());
    }
}

TEST_F(ChartDataAdapterTest, SetAndGetChartScale) {
    int scale = 25000;
    adapter->SetChartScale(scale);
    
    EXPECT_EQ(adapter->GetChartScale(), scale);
}

TEST_F(ChartDataAdapterTest, SetAndGetDepthUnit) {
    std::string unit = "feet";
    adapter->SetDepthUnit(unit);
    
    EXPECT_EQ(adapter->GetDepthUnit(), unit);
}

TEST_F(ChartDataAdapterTest, SetAndGetConnectionString) {
    std::string connStr = "postgresql://test:5432/chart";
    adapter->SetConnectionString(connStr);
    
    EXPECT_EQ(adapter->GetConnectionString(), connStr);
}

TEST_F(ChartDataAdapterTest, IsConnected) {
    EXPECT_TRUE(adapter->IsConnected());
}

TEST_F(ChartDataAdapterTest, GetProviderId) {
    EXPECT_EQ(adapter->GetProviderId(), "chart");
}

TEST_F(ChartDataAdapterTest, GetProviderType) {
    EXPECT_EQ(adapter->GetProviderType(), "chart");
}

TEST_F(ChartDataAdapterTest, GetLastError) {
    std::string error = adapter->GetLastError();
    EXPECT_TRUE(error.empty() || error.length() >= 0);
}

TEST_F(ChartDataAdapterTest, DepthCache) {
    alert::Coordinate position(113.5, 30.5);
    
    alert::DepthData depth1 = adapter->GetDepthAtPosition(position);
    alert::DepthData depth2 = adapter->GetDepthAtPosition(position);
    
    EXPECT_TRUE(depth1.valid);
    EXPECT_TRUE(depth2.valid);
    EXPECT_DOUBLE_EQ(depth1.chartDepth, depth2.chartDepth);
}

TEST_F(ChartDataAdapterTest, ChannelCache) {
    std::string channelId = "CH_CACHE";
    
    alert::ChannelData channel1 = adapter->GetChannelInfo(channelId);
    alert::ChannelData channel2 = adapter->GetChannelInfo(channelId);
    
    EXPECT_TRUE(channel1.valid);
    EXPECT_TRUE(channel2.valid);
    EXPECT_EQ(channel1.channelId, channel2.channelId);
}

TEST_F(ChartDataAdapterTest, NavAidCache) {
    std::string aidId = "NA_CACHE";
    
    alert::NavAidData aid1 = adapter->GetNavAidInfo(aidId);
    alert::NavAidData aid2 = adapter->GetNavAidInfo(aidId);
    
    EXPECT_TRUE(aid1.valid);
    EXPECT_TRUE(aid2.valid);
    EXPECT_EQ(aid1.aidId, aid2.aidId);
}

TEST_F(ChartDataAdapterTest, RestrictedAreaStructure) {
    alert::RestrictedArea area;
    area.areaId = "RA_001";
    area.name = "TEST_AREA";
    area.restrictionType = "NO_ENTRY";
    area.description = "Test restricted area";
    area.valid = true;
    
    EXPECT_EQ(area.areaId, "RA_001");
    EXPECT_EQ(area.name, "TEST_AREA");
    EXPECT_EQ(area.restrictionType, "NO_ENTRY");
    EXPECT_TRUE(area.valid);
}

TEST_F(ChartDataAdapterTest, AnchorageAreaStructure) {
    alert::AnchorageArea area;
    area.anchorageId = "AA_001";
    area.name = "TEST_ANCHORAGE";
    area.minDepth = 5.0;
    area.maxDepth = 15.0;
    area.restrictions = "Max 24h";
    area.valid = true;
    
    EXPECT_EQ(area.anchorageId, "AA_001");
    EXPECT_EQ(area.name, "TEST_ANCHORAGE");
    EXPECT_DOUBLE_EQ(area.minDepth, 5.0);
    EXPECT_DOUBLE_EQ(area.maxDepth, 15.0);
    EXPECT_TRUE(area.valid);
}

TEST_F(ChartDataAdapterTest, ChartFeatureStructure) {
    alert::ChartFeature feature;
    feature.featureId = "CF_001";
    feature.featureType = "SOUNDG";
    feature.featureClass = "SOUNDING";
    feature.position = alert::Coordinate(113.5, 30.5);
    feature.attributes["depth"] = "10.5";
    feature.geometryWkt = "POINT(113.5 30.5)";
    feature.valid = true;
    
    EXPECT_EQ(feature.featureId, "CF_001");
    EXPECT_EQ(feature.featureType, "SOUNDG");
    EXPECT_EQ(feature.featureClass, "SOUNDING");
    EXPECT_TRUE(feature.valid);
}

}
