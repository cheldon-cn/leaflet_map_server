#ifndef OGC_ALERT_TEST_DATA_H
#define OGC_ALERT_TEST_DATA_H

#include "ogc/alert/types.h"
#include "ogc/alert/alert_engine.h"
#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/ukc_calculator.h"
#include <string>
#include <vector>

namespace ogc {
namespace alert {
namespace test {

struct TestCoordinates {
    static const Coordinate kShanghaiPort;
    static const Coordinate kBeijing;
    static const Coordinate kGuangzhou;
    static const Coordinate kShenzhen;
    static const Coordinate kTianjin;
    static const Coordinate kQingdao;
    static const Coordinate kDalian;
    static const Coordinate kNingbo;
    static const Coordinate kXiamen;
    static const Coordinate kOrigin;
    
    static const double kCoordinateTolerance;
    static const double kDistanceTolerance;
};

struct TestShipData {
    static const double kDefaultSpeed;
    static const double kHighSpeed;
    static const double kLowSpeed;
    static const double kDefaultHeading;
    static const double kNorthHeading;
    static const double kSouthHeading;
    static const double kEastHeading;
    static const double kWestHeading;
};

struct TestUKCData {
    static const double kShallowDepth;
    static const double kDeepDepth;
    static const double kDefaultDraft;
    static const double kLargeDraft;
    static const double kDefaultTide;
    static const double kHighTide;
    static const double kLowTide;
    static const double kDefaultSafetyMargin;
    static const double kDefaultSquat;
    static const double kDefaultHeelCorrection;
    static const double kDefaultWaveAllowance;
    static const double kDefaultWaterDensity;
};

struct TestCPAData {
    static const double kDangerousCPA;
    static const double kSafeCPA;
    static const double kShortTCPA;
    static const double kLongTCPA;
    static const double kCPAThreshold1;
    static const double kCPAThreshold2;
    static const double kTCPAThreshold1;
    static const double kTCPAThreshold2;
};

struct TestAlertIds {
    static const std::string kTestAlertId1;
    static const std::string kTestAlertId2;
    static const std::string kTestAlertId3;
    static const std::string kTestUserId1;
    static const std::string kTestUserId2;
    static const std::string kTestSourceId1;
    static const std::string kTestSourceId2;
};

struct TestThresholds {
    static const double kDepthWarningThreshold;
    static const double kDepthCriticalThreshold;
    static const double kSpeedWarningThreshold;
    static const double kSpeedCriticalThreshold;
    static const double kCPAWarningThreshold;
    static const double kCPACriticalThreshold;
    static const double kDeviationWarningThreshold;
    static const double kDeviationCriticalThreshold;
};

struct TestWeatherData {
    static const double kHighWindSpeed;
    static const double kLowWindSpeed;
    static const double kHighWaveHeight;
    static const double kLowWaveHeight;
    static const double kLowVisibility;
    static const double kGoodVisibility;
    static const double kHeavyRainfall;
    static const double kLightRainfall;
};

struct TestTimeData {
    static const int kTimeWindowSeconds;
    static const int kSuppressIntervalSeconds;
    static const int kShortTimeWindow;
    static const int kLongTimeWindow;
};

class TestDataFactory {
public:
    static AlertPtr CreateTestAlert(
        const std::string& id = TestAlertIds::kTestAlertId1,
        AlertType type = AlertType::kDepth,
        AlertLevel level = AlertLevel::kLevel2,
        const std::string& user_id = TestAlertIds::kTestUserId1);
    
    static AlertPtr CreateDepthAlert(
        double current_depth = TestUKCData::kShallowDepth,
        double ukc = 2.0,
        double safety_depth = 10.0);
    
    static AlertPtr CreateCollisionAlert(
        const std::string& target_id = "TARGET_001",
        double cpa = TestCPAData::kDangerousCPA,
        double tcpa = TestCPAData::kShortTCPA);
    
    static AlertPtr CreateWeatherAlert(
        const std::string& weather_type = "STORM",
        double wind_speed = TestWeatherData::kHighWindSpeed);
    
    static UKCInput CreateUKCInput(
        double chart_depth = TestUKCData::kShallowDepth,
        double ship_draft = TestUKCData::kDefaultDraft,
        double tide_height = TestUKCData::kDefaultTide);
    
    static ShipMotion CreateShipMotion(
        const Coordinate& position = TestCoordinates::kShanghaiPort,
        double speed = TestShipData::kDefaultSpeed,
        double heading = TestShipData::kDefaultHeading);
    
    static std::vector<AlertPtr> CreateTestAlertBatch(size_t count);
    
    static CheckContext CreateCheckContext(
        const Coordinate& position = TestCoordinates::kShanghaiPort,
        double speed = TestShipData::kDefaultSpeed,
        double heading = TestShipData::kDefaultHeading);
};

}
}
}

#endif
