#include <gtest/gtest.h>
#include "alert/channel_alert_engine.h"
#include "alert/alert_context.h"
#include "alert/alert_exception.h"

class ChannelAlertEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = new alert::ChannelAlertEngine();
    }
    
    void TearDown() override {
        delete engine;
    }
    
    alert::ChannelAlertEngine* engine;
    
    static alert::ChannelData MockChannelProvider(const alert::Coordinate& pos) {
        alert::ChannelData data;
        
        alert::ChannelStatus channel;
        channel.channelId = "CH001";
        channel.channelName = "Test Channel";
        channel.isNavigable = true;
        channel.minDepth = 15.0;
        channel.maxWidth = 200.0;
        channel.status = "Open";
        data.channels.push_back(channel);
        
        alert::NavAidStatus navAid;
        navAid.navAidId = "NA001";
        navAid.navAidName = "Test Buoy";
        navAid.type = "Buoy";
        navAid.isOperational = true;
        navAid.position = alert::Coordinate(120.5, 31.2);
        data.navAids.push_back(navAid);
        
        data.notices.push_back("Notice 1");
        
        return data;
    }
};

TEST_F(ChannelAlertEngineTest, GetType) {
    EXPECT_EQ(engine->GetType(), alert::AlertType::kChannelAlert);
}

TEST_F(ChannelAlertEngineTest, GetName) {
    EXPECT_EQ(engine->GetName(), "ChannelAlertEngine");
}

TEST_F(ChannelAlertEngineTest, SetAndGetThreshold) {
    alert::ThresholdConfig config("ChannelThreshold");
    config.SetLevel1Threshold(1.0);
    
    engine->SetThreshold(config);
    
    alert::ThresholdConfig retrieved = engine->GetThreshold();
    EXPECT_EQ(retrieved.GetName(), "ChannelThreshold");
}

TEST_F(ChannelAlertEngineTest, EvaluateWithInvalidContext) {
    alert::AlertContext context;
    
    EXPECT_THROW(engine->Evaluate(context), alert::InvalidParameterException);
}

TEST_F(ChannelAlertEngineTest, EvaluateNoAlert) {
    alert::ShipInfo ship("SHIP001", "Test Ship");
    ship.SetDraft(10.0);
    alert::Coordinate position(120.5, 31.2);
    
    alert::AlertContext context(ship, position);
    
    alert::Alert alert = engine->Evaluate(context);
    
    EXPECT_EQ(alert.GetAlertLevel(), alert::AlertLevel::kNone);
}

TEST_F(ChannelAlertEngineTest, SetChannelProvider) {
    engine->SetChannelProvider(MockChannelProvider);
}

TEST_F(ChannelAlertEngineTest, ChannelStatusDefaults) {
    alert::ChannelStatus status;
    
    EXPECT_TRUE(status.channelId.empty());
    EXPECT_TRUE(status.channelName.empty());
    EXPECT_TRUE(status.isNavigable);
    EXPECT_DOUBLE_EQ(status.minDepth, 0.0);
    EXPECT_DOUBLE_EQ(status.maxWidth, 0.0);
}

TEST_F(ChannelAlertEngineTest, NavAidStatusDefaults) {
    alert::NavAidStatus status;
    
    EXPECT_TRUE(status.navAidId.empty());
    EXPECT_TRUE(status.navAidName.empty());
    EXPECT_TRUE(status.type.empty());
    EXPECT_TRUE(status.isOperational);
}

TEST_F(ChannelAlertEngineTest, ChannelDataStructure) {
    alert::ChannelData data = MockChannelProvider(alert::Coordinate(120.5, 31.2));
    
    EXPECT_EQ(data.channels.size(), 1);
    EXPECT_EQ(data.navAids.size(), 1);
    EXPECT_EQ(data.notices.size(), 1);
    
    EXPECT_EQ(data.channels[0].channelId, "CH001");
    EXPECT_EQ(data.navAids[0].navAidId, "NA001");
}
