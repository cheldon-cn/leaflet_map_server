#include <gtest/gtest.h>
#include "ogc/alert/types.h"
#include "ogc/alert/weather_alert_checker.h"
#include "ogc/alert/channel_alert_checker.h"
#include "ogc/alert/deviation_alert_checker.h"
#include "ogc/alert/speed_alert_checker.h"
#include "ogc/alert/restricted_area_checker.h"
#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/deviation_calculator.h"
#include "ogc/alert/speed_zone_matcher.h"
#include <memory>
#include <limits>

namespace ogc {
namespace alert {
namespace test {

class WeatherAlertCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = WeatherAlertChecker::Create();
    }
    
    std::unique_ptr<WeatherAlertChecker> m_checker;
};

TEST_F(WeatherAlertCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(WeatherAlertCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kWeather);
    EXPECT_FALSE(m_checker->GetCheckerId().empty());
}

TEST_F(WeatherAlertCheckerTest, EnableDisable) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
    
    m_checker->SetEnabled(false);
    EXPECT_FALSE(m_checker->IsEnabled());
}

TEST_F(WeatherAlertCheckerTest, SetGetThreshold) {
    AlertThreshold threshold;
    threshold.level1_threshold = 10.0;
    threshold.level2_threshold = 20.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 10.0);
}

TEST_F(WeatherAlertCheckerTest, DetermineWeatherLevel) {
    AlertLevel level = m_checker->DetermineWeatherLevel(5.0, 1.0, 1000.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(WeatherAlertCheckerTest, BoundaryValue_ZeroWindSpeed) {
    AlertLevel level = m_checker->DetermineWeatherLevel(0.0, 0.0, 10000.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(WeatherAlertCheckerTest, BoundaryValue_HighWindSpeed) {
    AlertLevel level = m_checker->DetermineWeatherLevel(50.0, 5.0, 100.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(WeatherAlertCheckerTest, InvalidInput_NegativeWindSpeed) {
    AlertLevel level = m_checker->DetermineWeatherLevel(-5.0, 1.0, 1000.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

class ChannelAlertCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = ChannelAlertChecker::Create();
    }
    
    std::unique_ptr<ChannelAlertChecker> m_checker;
};

TEST_F(ChannelAlertCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(ChannelAlertCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kChannel);
    EXPECT_FALSE(m_checker->GetCheckerId().empty());
}

TEST_F(ChannelAlertCheckerTest, EnableDisable) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
    
    m_checker->SetEnabled(false);
    EXPECT_FALSE(m_checker->IsEnabled());
}

TEST_F(ChannelAlertCheckerTest, SetGetThreshold) {
    AlertThreshold threshold;
    threshold.level1_threshold = 100.0;
    threshold.level2_threshold = 80.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 100.0);
}

class DeviationAlertCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = DeviationAlertChecker::Create();
    }
    
    std::unique_ptr<DeviationAlertChecker> m_checker;
};

TEST_F(DeviationAlertCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(DeviationAlertCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kDeviation);
    EXPECT_FALSE(m_checker->GetCheckerId().empty());
}

TEST_F(DeviationAlertCheckerTest, EnableDisable) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
    
    m_checker->SetEnabled(false);
    EXPECT_FALSE(m_checker->IsEnabled());
}

TEST_F(DeviationAlertCheckerTest, SetGetThreshold) {
    AlertThreshold threshold;
    threshold.level1_threshold = 50.0;
    threshold.level2_threshold = 100.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 50.0);
}

TEST_F(DeviationAlertCheckerTest, DetermineDeviationLevel) {
    AlertLevel level = m_checker->DetermineDeviationLevel(30.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(DeviationAlertCheckerTest, BoundaryValue_ZeroDeviation) {
    AlertLevel level = m_checker->DetermineDeviationLevel(0.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(DeviationAlertCheckerTest, BoundaryValue_LargeDeviation) {
    AlertLevel level = m_checker->DetermineDeviationLevel(1000.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

class SpeedAlertCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = SpeedAlertChecker::Create();
    }
    
    std::unique_ptr<SpeedAlertChecker> m_checker;
};

TEST_F(SpeedAlertCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(SpeedAlertCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kSpeed);
    EXPECT_FALSE(m_checker->GetCheckerId().empty());
}

TEST_F(SpeedAlertCheckerTest, EnableDisable) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
    
    m_checker->SetEnabled(false);
    EXPECT_FALSE(m_checker->IsEnabled());
}

TEST_F(SpeedAlertCheckerTest, SetGetThreshold) {
    AlertThreshold threshold;
    threshold.level1_threshold = 12.0;
    threshold.level2_threshold = 15.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 12.0);
}

TEST_F(SpeedAlertCheckerTest, DetermineSpeedLevel) {
    AlertLevel level = m_checker->DetermineSpeedLevel(10.0, 15.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(SpeedAlertCheckerTest, BoundaryValue_ZeroSpeed) {
    AlertLevel level = m_checker->DetermineSpeedLevel(0.0, 15.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(SpeedAlertCheckerTest, BoundaryValue_ExactlyLimit) {
    AlertLevel level = m_checker->DetermineSpeedLevel(15.0, 15.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(SpeedAlertCheckerTest, InvalidInput_NegativeSpeed) {
    AlertLevel level = m_checker->DetermineSpeedLevel(-5.0, 15.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

class RestrictedAreaCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_checker = RestrictedAreaChecker::Create();
    }
    
    std::unique_ptr<RestrictedAreaChecker> m_checker;
};

TEST_F(RestrictedAreaCheckerTest, CreateChecker) {
    ASSERT_NE(m_checker, nullptr);
}

TEST_F(RestrictedAreaCheckerTest, GetCheckerInfo) {
    EXPECT_EQ(m_checker->GetAlertType(), AlertType::kRestrictedArea);
    EXPECT_FALSE(m_checker->GetCheckerId().empty());
}

TEST_F(RestrictedAreaCheckerTest, EnableDisable) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
    
    m_checker->SetEnabled(false);
    EXPECT_FALSE(m_checker->IsEnabled());
}

class UKCCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_calculator.reset(new UKCCalculator());
    }
    
    std::unique_ptr<UKCCalculator> m_calculator;
};

TEST_F(UKCCalculatorTest, CreateCalculator) {
    ASSERT_NE(m_calculator, nullptr);
}

TEST_F(UKCCalculatorTest, CalculateUKC) {
    UKCInput input;
    input.chart_depth = 10.0;
    input.tide_height = 2.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_DOUBLE_EQ(result.ukc, 6.0);
    EXPECT_TRUE(result.is_safe);
}

TEST_F(UKCCalculatorTest, CalculateUKC_Unsafe) {
    UKCInput input;
    input.chart_depth = 5.0;
    input.tide_height = 1.0;
    input.ship_draft = 6.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_DOUBLE_EQ(result.ukc, -1.0);
    EXPECT_FALSE(result.is_safe);
}

TEST_F(UKCCalculatorTest, BoundaryValue_ZeroDepth) {
    UKCInput input;
    input.chart_depth = 0.0;
    input.tide_height = 0.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_DOUBLE_EQ(result.ukc, -6.0);
    EXPECT_FALSE(result.is_safe);
}

TEST_F(UKCCalculatorTest, BoundaryValue_ZeroDraft) {
    UKCInput input;
    input.chart_depth = 10.0;
    input.tide_height = 2.0;
    input.ship_draft = 0.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_DOUBLE_EQ(result.ukc, 11.0);
    EXPECT_TRUE(result.is_safe);
}

TEST_F(UKCCalculatorTest, BoundaryValue_NegativeTide) {
    UKCInput input;
    input.chart_depth = 10.0;
    input.tide_height = -2.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_DOUBLE_EQ(result.ukc, 2.0);
    EXPECT_FALSE(result.is_safe);
}

TEST_F(UKCCalculatorTest, BoundaryValue_MaxValues) {
    UKCInput input;
    input.chart_depth = std::numeric_limits<double>::max();
    input.tide_height = 0.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_GT(result.ukc, 0.0);
    EXPECT_TRUE(result.is_safe);
}

TEST_F(UKCCalculatorTest, InvalidInput_NegativeDepth) {
    UKCInput input;
    input.chart_depth = -10.0;
    input.tide_height = 2.0;
    input.ship_draft = 5.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_LT(result.ukc, 0.0);
}

TEST_F(UKCCalculatorTest, InvalidInput_NegativeDraft) {
    UKCInput input;
    input.chart_depth = 10.0;
    input.tide_height = 2.0;
    input.ship_draft = -5.0;
    input.safety_margin = 0.5;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_GT(result.ukc, 10.0);
}

class DeviationCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_calculator.reset(new DeviationCalculator());
    }
    
    std::unique_ptr<DeviationCalculator> m_calculator;
};

TEST_F(DeviationCalculatorTest, CreateCalculator) {
    ASSERT_NE(m_calculator, nullptr);
}

TEST_F(DeviationCalculatorTest, CalculateCrossTrack) {
    Coordinate shipPos(120.5, 31.0);
    Coordinate routeStart(120.0, 31.0);
    Coordinate routeEnd(121.0, 31.0);
    
    double deviation = m_calculator->CalculateCrossTrack(shipPos, routeStart, routeEnd);
    EXPECT_GE(deviation, 0.0);
}

TEST_F(DeviationCalculatorTest, BoundaryValue_OnRoute) {
    Coordinate shipPos(120.5, 31.0);
    Coordinate routeStart(120.0, 31.0);
    Coordinate routeEnd(121.0, 31.0);
    
    double deviation = m_calculator->CalculateCrossTrack(shipPos, routeStart, routeEnd);
    EXPECT_GE(deviation, 0.0);
}

class SpeedZoneMatcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_matcher.reset(new SpeedZoneMatcher());
    }
    
    std::unique_ptr<SpeedZoneMatcher> m_matcher;
};

TEST_F(SpeedZoneMatcherTest, CreateMatcher) {
    ASSERT_NE(m_matcher, nullptr);
}

TEST_F(SpeedZoneMatcherTest, AddZone) {
    SpeedZone zone;
    zone.zone_id = "ZONE_001";
    zone.max_speed = 10.0;
    
    m_matcher->AddZone(zone);
    auto retrieved = m_matcher->GetZone("ZONE_001");
    EXPECT_EQ(retrieved.zone_id, "ZONE_001");
}

TEST_F(SpeedZoneMatcherTest, BoundaryValue_EmptyZones) {
    Coordinate pos(120.0, 31.0);
    auto zones = m_matcher->GetZonesAtPosition(pos);
    EXPECT_TRUE(zones.empty());
}

TEST_F(SpeedZoneMatcherTest, BoundaryValue_MultipleZones) {
    for (int i = 0; i < 50; i++) {
        SpeedZone zone;
        zone.zone_id = "ZONE_" + std::to_string(i);
        zone.max_speed = 10.0 + i;
        m_matcher->AddZone(zone);
    }
    
    Coordinate pos(120.0, 31.0);
    auto zones = m_matcher->GetZonesAtPosition(pos);
    EXPECT_EQ(zones.size(), 0);
}

TEST_F(SpeedZoneMatcherTest, InvalidInput_NegativeSpeedLimit) {
    SpeedZone zone;
    zone.zone_id = "ZONE_NEG";
    zone.max_speed = -10.0;
    
    m_matcher->AddZone(zone);
    auto retrieved = m_matcher->GetZone("ZONE_NEG");
    EXPECT_DOUBLE_EQ(retrieved.max_speed, -10.0);
}

TEST_F(WeatherAlertCheckerTest, ExceptionPath_EmptyThreshold) {
    AlertThreshold threshold;
    threshold.level1_threshold = 0.0;
    threshold.level2_threshold = 0.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 0.0);
}

TEST_F(WeatherAlertCheckerTest, ExceptionPath_InvalidThresholdOrder) {
    AlertThreshold threshold;
    threshold.level1_threshold = 30.0;
    threshold.level2_threshold = 10.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 30.0);
}

TEST_F(ChannelAlertCheckerTest, ExceptionPath_EmptyChannelId) {
    AlertThreshold threshold;
    threshold.level1_threshold = 5.0;
    
    m_checker->SetThreshold(threshold);
    auto retrieved = m_checker->GetThreshold();
    EXPECT_DOUBLE_EQ(retrieved.level1_threshold, 5.0);
}

TEST_F(DeviationAlertCheckerTest, ExceptionPath_NegativeDeviation) {
    AlertLevel level = m_checker->DetermineDeviationLevel(-100.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(DeviationAlertCheckerTest, ExceptionPath_ZeroMaxDeviation) {
    AlertLevel level = m_checker->DetermineDeviationLevel(0.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(SpeedAlertCheckerTest, ExceptionPath_ZeroSpeedLimit) {
    AlertLevel level = m_checker->DetermineSpeedLevel(10.0, 0.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(SpeedAlertCheckerTest, ExceptionPath_NegativeSpeedLimit) {
    AlertLevel level = m_checker->DetermineSpeedLevel(10.0, -15.0);
    EXPECT_GE(static_cast<int>(level), 0);
}

TEST_F(RestrictedAreaCheckerTest, ExceptionPath_NullGeometry) {
    m_checker->SetEnabled(true);
    EXPECT_TRUE(m_checker->IsEnabled());
}

TEST_F(UKCCalculatorTest, ExceptionPath_AllZeros) {
    UKCInput input;
    input.chart_depth = 0.0;
    input.tide_height = 0.0;
    input.ship_draft = 0.0;
    input.safety_margin = 0.0;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = 1.025;
    input.speed_knots = 0.0;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_DOUBLE_EQ(result.ukc, -0.5);
}

TEST_F(UKCCalculatorTest, ExceptionPath_AllNegative) {
    UKCInput input;
    input.chart_depth = -10.0;
    input.tide_height = -2.0;
    input.ship_draft = -5.0;
    input.safety_margin = -0.5;
    input.squat = 0.0;
    input.heel_correction = 0.0;
    input.wave_allowance = 0.0;
    input.water_density = 1.025;
    input.speed_knots = 0.0;
    
    UKCResult result = m_calculator->Calculate(input);
    EXPECT_LT(result.ukc, 0.0);
}

TEST_F(DeviationCalculatorTest, ExceptionPath_SamePoints) {
    Coordinate pos(120.0, 31.0);
    double deviation = m_calculator->CalculateCrossTrack(pos, pos, pos);
    EXPECT_DOUBLE_EQ(deviation, 0.0);
}

TEST_F(DeviationCalculatorTest, ExceptionPath_Antimeridian) {
    Coordinate shipPos(179.9, 31.0);
    Coordinate routeStart(-179.9, 31.0);
    Coordinate routeEnd(179.9, 31.0);
    
    double deviation = m_calculator->CalculateCrossTrack(shipPos, routeStart, routeEnd);
    EXPECT_GE(deviation, 0.0);
}

TEST_F(SpeedZoneMatcherTest, ExceptionPath_DuplicateZoneId) {
    SpeedZone zone1;
    zone1.zone_id = "ZONE_DUP";
    zone1.max_speed = 10.0;
    
    SpeedZone zone2;
    zone2.zone_id = "ZONE_DUP";
    zone2.max_speed = 20.0;
    
    m_matcher->AddZone(zone1);
    m_matcher->AddZone(zone2);
    
    auto retrieved = m_matcher->GetZone("ZONE_DUP");
    EXPECT_DOUBLE_EQ(retrieved.max_speed, 20.0);
}

TEST_F(SpeedZoneMatcherTest, ExceptionPath_EmptyZoneId) {
    SpeedZone zone;
    zone.zone_id = "";
    zone.max_speed = 10.0;
    
    m_matcher->AddZone(zone);
    auto retrieved = m_matcher->GetZone("");
    EXPECT_TRUE(retrieved.zone_id.empty());
}

}
}
}
