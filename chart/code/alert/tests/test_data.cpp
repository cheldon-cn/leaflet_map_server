#include "test_data.h"
#include <memory>

namespace ogc {
namespace alert {
namespace test {

const Coordinate TestCoordinates::kShanghaiPort(121.4737, 31.2304);
const Coordinate TestCoordinates::kBeijing(116.4074, 39.9042);
const Coordinate TestCoordinates::kGuangzhou(113.2644, 23.1291);
const Coordinate TestCoordinates::kShenzhen(114.0579, 22.5431);
const Coordinate TestCoordinates::kTianjin(117.1901, 39.1255);
const Coordinate TestCoordinates::kQingdao(120.3826, 36.0671);
const Coordinate TestCoordinates::kDalian(121.6147, 38.9140);
const Coordinate TestCoordinates::kNingbo(121.5440, 29.8683);
const Coordinate TestCoordinates::kXiamen(118.0894, 24.4798);
const Coordinate TestCoordinates::kOrigin(0.0, 0.0);

const double TestCoordinates::kCoordinateTolerance = 0.0001;
const double TestCoordinates::kDistanceTolerance = 0.1;

const double TestShipData::kDefaultSpeed = 10.0;
const double TestShipData::kHighSpeed = 25.0;
const double TestShipData::kLowSpeed = 5.0;
const double TestShipData::kDefaultHeading = 0.0;
const double TestShipData::kNorthHeading = 0.0;
const double TestShipData::kSouthHeading = 180.0;
const double TestShipData::kEastHeading = 90.0;
const double TestShipData::kWestHeading = 270.0;

const double TestUKCData::kShallowDepth = 10.0;
const double TestUKCData::kDeepDepth = 50.0;
const double TestUKCData::kDefaultDraft = 5.0;
const double TestUKCData::kLargeDraft = 10.0;
const double TestUKCData::kDefaultTide = 2.0;
const double TestUKCData::kHighTide = 4.0;
const double TestUKCData::kLowTide = 0.5;
const double TestUKCData::kDefaultSafetyMargin = 0.5;
const double TestUKCData::kDefaultSquat = 0.3;
const double TestUKCData::kDefaultHeelCorrection = 0.1;
const double TestUKCData::kDefaultWaveAllowance = 0.2;
const double TestUKCData::kDefaultWaterDensity = 1.025;

const double TestCPAData::kDangerousCPA = 0.5;
const double TestCPAData::kSafeCPA = 5.0;
const double TestCPAData::kShortTCPA = 5.0;
const double TestCPAData::kLongTCPA = 30.0;
const double TestCPAData::kCPAThreshold1 = 0.5;
const double TestCPAData::kCPAThreshold2 = 1.0;
const double TestCPAData::kTCPAThreshold1 = 10.0;
const double TestCPAData::kTCPAThreshold2 = 20.0;

const std::string TestAlertIds::kTestAlertId1 = "TEST_ALERT_001";
const std::string TestAlertIds::kTestAlertId2 = "TEST_ALERT_002";
const std::string TestAlertIds::kTestAlertId3 = "TEST_ALERT_003";
const std::string TestAlertIds::kTestUserId1 = "TEST_USER_001";
const std::string TestAlertIds::kTestUserId2 = "TEST_USER_002";
const std::string TestAlertIds::kTestSourceId1 = "TEST_SOURCE_001";
const std::string TestAlertIds::kTestSourceId2 = "TEST_SOURCE_002";

const double TestThresholds::kDepthWarningThreshold = 2.0;
const double TestThresholds::kDepthCriticalThreshold = 1.0;
const double TestThresholds::kSpeedWarningThreshold = 12.0;
const double TestThresholds::kSpeedCriticalThreshold = 15.0;
const double TestThresholds::kCPAWarningThreshold = 1.0;
const double TestThresholds::kCPACriticalThreshold = 0.5;
const double TestThresholds::kDeviationWarningThreshold = 0.5;
const double TestThresholds::kDeviationCriticalThreshold = 1.0;

const double TestWeatherData::kHighWindSpeed = 25.0;
const double TestWeatherData::kLowWindSpeed = 5.0;
const double TestWeatherData::kHighWaveHeight = 4.0;
const double TestWeatherData::kLowWaveHeight = 0.5;
const double TestWeatherData::kLowVisibility = 0.5;
const double TestWeatherData::kGoodVisibility = 10.0;
const double TestWeatherData::kHeavyRainfall = 50.0;
const double TestWeatherData::kLightRainfall = 5.0;

const int TestTimeData::kTimeWindowSeconds = 300;
const int TestTimeData::kSuppressIntervalSeconds = 300;
const int TestTimeData::kShortTimeWindow = 60;
const int TestTimeData::kLongTimeWindow = 600;

AlertPtr TestDataFactory::CreateTestAlert(
    const std::string& id,
    AlertType type,
    AlertLevel level,
    const std::string& user_id) {
    
    auto alert = std::make_shared<Alert>();
    alert->alert_id = id;
    alert->alert_type = type;
    alert->alert_level = level;
    alert->user_id = user_id;
    alert->status = AlertStatus::kActive;
    alert->issue_time = DateTime::Now();
    alert->position = TestCoordinates::kShanghaiPort;
    alert->acknowledge_required = true;
    
    return alert;
}

AlertPtr TestDataFactory::CreateDepthAlert(
    double current_depth,
    double ukc,
    double safety_depth) {
    
    auto alert = std::make_shared<DepthAlert>();
    alert->alert_id = TestAlertIds::kTestAlertId1;
    alert->alert_type = AlertType::kDepth;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = TestAlertIds::kTestUserId1;
    alert->status = AlertStatus::kActive;
    alert->issue_time = DateTime::Now();
    alert->position = TestCoordinates::kShanghaiPort;
    
    DepthAlert* depth_alert = static_cast<DepthAlert*>(alert.get());
    depth_alert->current_depth = current_depth;
    depth_alert->ukc = ukc;
    depth_alert->safety_depth = safety_depth;
    depth_alert->tide_height = TestUKCData::kDefaultTide;
    
    return alert;
}

AlertPtr TestDataFactory::CreateCollisionAlert(
    const std::string& target_id,
    double cpa,
    double tcpa) {
    
    auto alert = std::make_shared<CollisionAlert>();
    alert->alert_id = TestAlertIds::kTestAlertId1;
    alert->alert_type = AlertType::kCollision;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = TestAlertIds::kTestUserId1;
    alert->status = AlertStatus::kActive;
    alert->issue_time = DateTime::Now();
    alert->position = TestCoordinates::kShanghaiPort;
    
    CollisionAlert* collision_alert = static_cast<CollisionAlert*>(alert.get());
    collision_alert->target_ship_id = target_id;
    collision_alert->cpa = cpa;
    collision_alert->tcpa = tcpa;
    collision_alert->relative_bearing = 45.0;
    collision_alert->relative_speed = 10.0;
    collision_alert->target_position = TestCoordinates::kNingbo;
    
    return alert;
}

AlertPtr TestDataFactory::CreateWeatherAlert(
    const std::string& weather_type,
    double wind_speed) {
    
    auto alert = std::make_shared<WeatherAlert>();
    alert->alert_id = TestAlertIds::kTestAlertId1;
    alert->alert_type = AlertType::kWeather;
    alert->alert_level = AlertLevel::kLevel2;
    alert->user_id = TestAlertIds::kTestUserId1;
    alert->status = AlertStatus::kActive;
    alert->issue_time = DateTime::Now();
    alert->position = TestCoordinates::kShanghaiPort;
    
    WeatherAlert* weather_alert = static_cast<WeatherAlert*>(alert.get());
    weather_alert->weather_type = weather_type;
    weather_alert->wind_speed = wind_speed;
    
    return alert;
}

UKCInput TestDataFactory::CreateUKCInput(
    double chart_depth,
    double ship_draft,
    double tide_height) {
    
    UKCInput input;
    input.chart_depth = chart_depth;
    input.ship_draft = ship_draft;
    input.tide_height = tide_height;
    input.safety_margin = TestUKCData::kDefaultSafetyMargin;
    input.squat = TestUKCData::kDefaultSquat;
    input.heel_correction = TestUKCData::kDefaultHeelCorrection;
    input.wave_allowance = TestUKCData::kDefaultWaveAllowance;
    input.water_density = TestUKCData::kDefaultWaterDensity;
    input.speed_knots = TestShipData::kDefaultSpeed;
    
    return input;
}

ShipMotion TestDataFactory::CreateShipMotion(
    const Coordinate& position,
    double speed,
    double heading) {
    
    ShipMotion motion;
    motion.position = position;
    motion.speed = speed;
    motion.heading = heading;
    motion.UpdateVelocity();
    
    return motion;
}

std::vector<AlertPtr> TestDataFactory::CreateTestAlertBatch(size_t count) {
    std::vector<AlertPtr> alerts;
    alerts.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        std::string id = "BATCH_ALERT_" + std::to_string(i);
        alerts.push_back(CreateTestAlert(id));
    }
    
    return alerts;
}

CheckContext TestDataFactory::CreateCheckContext(
    const Coordinate& position,
    double speed,
    double heading) {
    
    CheckContext context;
    context.ship_position = position;
    context.ship_speed = speed;
    context.ship_heading = heading;
    context.check_time = DateTime::Now();
    
    return context;
}

}
}
}
