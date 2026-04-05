#include <gtest/gtest.h>
#include "alert/alert_types.h"

class AlertTypesTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AlertTypesTest, AlertTypeToString) {
    EXPECT_EQ(alert::AlertTypeToString(alert::AlertType::kDepthAlert), "depth");
    EXPECT_EQ(alert::AlertTypeToString(alert::AlertType::kCollisionAlert), "collision");
    EXPECT_EQ(alert::AlertTypeToString(alert::AlertType::kWeatherAlert), "weather");
    EXPECT_EQ(alert::AlertTypeToString(alert::AlertType::kChannelAlert), "channel");
    EXPECT_EQ(alert::AlertTypeToString(alert::AlertType::kOtherAlert), "other");
}

TEST_F(AlertTypesTest, AlertLevelToString) {
    EXPECT_EQ(alert::AlertLevelToString(alert::AlertLevel::kLevel1_Critical), "critical");
    EXPECT_EQ(alert::AlertLevelToString(alert::AlertLevel::kLevel2_Severe), "severe");
    EXPECT_EQ(alert::AlertLevelToString(alert::AlertLevel::kLevel3_Moderate), "moderate");
    EXPECT_EQ(alert::AlertLevelToString(alert::AlertLevel::kLevel4_Minor), "minor");
    EXPECT_EQ(alert::AlertLevelToString(alert::AlertLevel::kNone), "none");
}

TEST_F(AlertTypesTest, AlertStatusToString) {
    EXPECT_EQ(alert::AlertStatusToString(alert::AlertStatus::kPending), "pending");
    EXPECT_EQ(alert::AlertStatusToString(alert::AlertStatus::kActive), "active");
    EXPECT_EQ(alert::AlertStatusToString(alert::AlertStatus::kPushed), "pushed");
    EXPECT_EQ(alert::AlertStatusToString(alert::AlertStatus::kAcknowledged), "acknowledged");
    EXPECT_EQ(alert::AlertStatusToString(alert::AlertStatus::kCleared), "cleared");
    EXPECT_EQ(alert::AlertStatusToString(alert::AlertStatus::kExpired), "expired");
}

TEST_F(AlertTypesTest, ShipTypeToString) {
    EXPECT_EQ(alert::ShipTypeToString(alert::ShipType::kUnknown), "unknown");
    EXPECT_EQ(alert::ShipTypeToString(alert::ShipType::kCargo), "cargo");
    EXPECT_EQ(alert::ShipTypeToString(alert::ShipType::kTanker), "tanker");
    EXPECT_EQ(alert::ShipTypeToString(alert::ShipType::kPassenger), "passenger");
    EXPECT_EQ(alert::ShipTypeToString(alert::ShipType::kFishing), "fishing");
}

TEST_F(AlertTypesTest, StringToAlertType) {
    EXPECT_EQ(alert::StringToAlertType("depth"), alert::AlertType::kDepthAlert);
    EXPECT_EQ(alert::StringToAlertType("collision"), alert::AlertType::kCollisionAlert);
    EXPECT_EQ(alert::StringToAlertType("weather"), alert::AlertType::kWeatherAlert);
    EXPECT_EQ(alert::StringToAlertType("channel"), alert::AlertType::kChannelAlert);
    EXPECT_EQ(alert::StringToAlertType("other"), alert::AlertType::kOtherAlert);
}

TEST_F(AlertTypesTest, StringToAlertLevel) {
    EXPECT_EQ(alert::StringToAlertLevel("critical"), alert::AlertLevel::kLevel1_Critical);
    EXPECT_EQ(alert::StringToAlertLevel("severe"), alert::AlertLevel::kLevel2_Severe);
    EXPECT_EQ(alert::StringToAlertLevel("moderate"), alert::AlertLevel::kLevel3_Moderate);
    EXPECT_EQ(alert::StringToAlertLevel("minor"), alert::AlertLevel::kLevel4_Minor);
    EXPECT_EQ(alert::StringToAlertLevel("none"), alert::AlertLevel::kNone);
}

TEST_F(AlertTypesTest, StringToAlertStatus) {
    EXPECT_EQ(alert::StringToAlertStatus("pending"), alert::AlertStatus::kPending);
    EXPECT_EQ(alert::StringToAlertStatus("active"), alert::AlertStatus::kActive);
    EXPECT_EQ(alert::StringToAlertStatus("pushed"), alert::AlertStatus::kPushed);
    EXPECT_EQ(alert::StringToAlertStatus("acknowledged"), alert::AlertStatus::kAcknowledged);
    EXPECT_EQ(alert::StringToAlertStatus("cleared"), alert::AlertStatus::kCleared);
    EXPECT_EQ(alert::StringToAlertStatus("expired"), alert::AlertStatus::kExpired);
}

TEST_F(AlertTypesTest, StringToShipType) {
    EXPECT_EQ(alert::StringToShipType("cargo"), alert::ShipType::kCargo);
    EXPECT_EQ(alert::StringToShipType("tanker"), alert::ShipType::kTanker);
    EXPECT_EQ(alert::StringToShipType("passenger"), alert::ShipType::kPassenger);
    EXPECT_EQ(alert::StringToShipType("unknown"), alert::ShipType::kUnknown);
}

TEST_F(AlertTypesTest, AlertLevelToPriority) {
    EXPECT_EQ(alert::AlertLevelToPriority(alert::AlertLevel::kLevel1_Critical), 1);
    EXPECT_EQ(alert::AlertLevelToPriority(alert::AlertLevel::kLevel2_Severe), 2);
    EXPECT_EQ(alert::AlertLevelToPriority(alert::AlertLevel::kLevel3_Moderate), 3);
    EXPECT_EQ(alert::AlertLevelToPriority(alert::AlertLevel::kLevel4_Minor), 4);
    EXPECT_EQ(alert::AlertLevelToPriority(alert::AlertLevel::kNone), 5);
}

TEST_F(AlertTypesTest, AlertConstants) {
    EXPECT_DOUBLE_EQ(alert::AlertConstants::kMinUKC, 0.3);
    EXPECT_DOUBLE_EQ(alert::AlertConstants::kDefaultCPAThreshold, 0.5);
    EXPECT_DOUBLE_EQ(alert::AlertConstants::kDefaultTCPAThreshold, 6.0);
    EXPECT_DOUBLE_EQ(alert::AlertConstants::kNauticalMileToMeters, 1852.0);
    EXPECT_DOUBLE_EQ(alert::AlertConstants::kKnotsToMs, 0.514444);
    EXPECT_DOUBLE_EQ(alert::AlertConstants::kEarthRadiusMeters, 6371000.0);
    EXPECT_EQ(alert::AlertConstants::kMaxRetryCount, 3);
    EXPECT_EQ(alert::AlertConstants::kDefaultTimeoutMs, 5000);
}
