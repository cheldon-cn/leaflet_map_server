#include "ogc/alert/ukc_calculator.h"
#include "ogc/alert/exception.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

UKCCalculator::UKCCalculator()
    : m_safetyMargin(0.5)
    , m_level1Threshold(2.0)
    , m_level2Threshold(1.5)
    , m_level3Threshold(1.0)
    , m_level4Threshold(0.5) {
}

UKCCalculator::~UKCCalculator() {
}

UKCResult UKCCalculator::Calculate(const UKCInput& input) {
    UKCResult result;
    
    double effective_depth = input.chart_depth + input.tide_height;
    result.effective_depth = effective_depth;
    
    double squat = input.squat;
    if (squat <= 0 && input.speed_knots > 0) {
        SquatParams default_params;
        default_params.block_coefficient = 0.7;
        default_params.ship_length = 100.0;
        default_params.ship_beam = 20.0;
        default_params.channel_depth = input.chart_depth;
        default_params.is_confined_water = false;
        squat = CalculateSquat(input.speed_knots, default_params);
    }
    result.squat_estimate = squat;
    
    double static_ukc = effective_depth - input.ship_draft;
    result.static_ukc = static_ukc;
    
    double dynamic_ukc = static_ukc - squat - input.heel_correction - input.wave_allowance;
    result.dynamic_ukc = dynamic_ukc;
    
    double ukc = dynamic_ukc - input.safety_margin - m_safetyMargin;
    result.ukc = ukc;
    
    result.alert_level = DetermineAlertLevel(ukc);
    result.is_safe = ukc > m_level1Threshold;
    
    if (result.alert_level == AlertLevel::kLevel4) {
        result.warning_message = "CRITICAL: UKC is critically low. Immediate action required.";
    } else if (result.alert_level == AlertLevel::kLevel3) {
        result.warning_message = "WARNING: UKC is below safe threshold. Reduce speed or change course.";
    } else if (result.alert_level == AlertLevel::kLevel2) {
        result.warning_message = "CAUTION: UKC is approaching safe threshold. Monitor closely.";
    } else if (result.alert_level == AlertLevel::kLevel1) {
        result.warning_message = "ADVISORY: UKC is within caution zone.";
    } else {
        result.warning_message = "UKC is within safe limits.";
    }
    
    return result;
}

double UKCCalculator::CalculateSquat(double speed_knots, const SquatParams& params) const {
    if (speed_knots <= 0) return 0.0;
    
    double speed_ms = KnotsToMetersPerSecond(speed_knots);
    double fn = speed_ms / std::sqrt(9.81 * params.channel_depth);
    
    double squat = 0.0;
    
    if (params.is_confined_water) {
        squat = CalculateICorellSquat(speed_knots, params.block_coefficient,
                                       params.ship_length, params.channel_depth, params.ship_beam);
    } else {
        squat = CalculateBarrassSquat(speed_knots, params.block_coefficient,
                                       params.ship_length, params.channel_depth);
    }
    
    return std::max(0.0, squat);
}

double UKCCalculator::CalculateBarrassSquat(double speed_knots, double block_coeff,
                                             double ship_length, double depth) const {
    double speed_ms = KnotsToMetersPerSecond(speed_knots);
    double fn = speed_ms / std::sqrt(9.81 * depth);
    
    double cb_factor = block_coeff > 0.7 ? 1.0 : block_coeff / 0.7;
    
    double squat = 2.4 * (ship_length / 100.0) * fn * fn * cb_factor;
    
    return squat;
}

double UKCCalculator::CalculateICorellSquat(double speed_knots, double block_coeff,
                                             double ship_length, double depth, double beam) const {
    double speed_ms = KnotsToMetersPerSecond(speed_knots);
    double fn = speed_ms / std::sqrt(9.81 * depth);
    
    double s_factor = ship_length * beam * block_coeff;
    double a_factor = depth * beam;
    
    double squat = (s_factor / a_factor) * fn * fn * 2.0;
    
    return squat;
}

double UKCCalculator::CalculateHeelCorrection(double speed_knots, double turn_radius, double ship_draft) const {
    if (turn_radius <= 0 || speed_knots <= 0) return 0.0;
    
    double speed_ms = KnotsToMetersPerSecond(speed_knots);
    double heel_angle = std::atan(speed_ms * speed_ms / (9.81 * turn_radius));
    
    double correction = ship_draft * (1.0 - std::cos(heel_angle));
    
    return correction;
}

double UKCCalculator::CalculateWaveAllowance(double wave_height) const {
    return wave_height * 0.5;
}

double UKCCalculator::CalculateDepthCorrection(double water_density) const {
    const double standard_density = 1.025;
    return (standard_density - water_density) * 0.1;
}

void UKCCalculator::SetSafetyMargin(double margin) {
    m_safetyMargin = margin;
}

double UKCCalculator::GetSafetyMargin() const {
    return m_safetyMargin;
}

void UKCCalculator::SetUKCThresholds(double level1, double level2, double level3, double level4) {
    m_level1Threshold = level1;
    m_level2Threshold = level2;
    m_level3Threshold = level3;
    m_level4Threshold = level4;
}

AlertLevel UKCCalculator::DetermineAlertLevel(double ukc) const {
    if (ukc <= m_level4Threshold) return AlertLevel::kLevel4;
    if (ukc <= m_level3Threshold) return AlertLevel::kLevel3;
    if (ukc <= m_level2Threshold) return AlertLevel::kLevel2;
    if (ukc <= m_level1Threshold) return AlertLevel::kLevel1;
    return AlertLevel::kNone;
}

double UKCCalculator::NauticalMilesToMeters(double nm) {
    return nm * 1852.0;
}

double UKCCalculator::MetersToNauticalMiles(double m) {
    return m / 1852.0;
}

double UKCCalculator::KnotsToMetersPerSecond(double knots) {
    return knots * 0.514444;
}

DepthInterpolator::DepthInterpolator() {
}

DepthInterpolator::~DepthInterpolator() {
}

void DepthInterpolator::AddDepthPoint(const Coordinate& position, double depth) {
    m_points.push_back({position, depth});
}

void DepthInterpolator::Clear() {
    m_points.clear();
}

double DepthInterpolator::InterpolateDepth(const Coordinate& position) const {
    if (m_points.empty()) {
        return 0.0;
    }
    
    if (m_points.size() == 1) {
        return m_points[0].depth;
    }
    
    std::vector<std::pair<double, double>> distances;
    const double max_distance = 0.1;
    
    for (const auto& point : m_points) {
        double dist = CalculateDistance(position, point.position);
        if (dist < max_distance) {
            distances.push_back({dist, point.depth});
        }
    }
    
    if (distances.empty()) {
        return GetNearestDepth(position);
    }
    
    return InverseDistanceWeight(distances);
}

double DepthInterpolator::GetNearestDepth(const Coordinate& position) const {
    if (m_points.empty()) {
        return 0.0;
    }
    
    double min_distance = std::numeric_limits<double>::max();
    double nearest_depth = 0.0;
    
    for (const auto& point : m_points) {
        double dist = CalculateDistance(position, point.position);
        if (dist < min_distance) {
            min_distance = dist;
            nearest_depth = point.depth;
        }
    }
    
    return nearest_depth;
}

bool DepthInterpolator::HasData() const {
    return !m_points.empty();
}

size_t DepthInterpolator::GetPointCount() const {
    return m_points.size();
}

double DepthInterpolator::CalculateDistance(const Coordinate& a, const Coordinate& b) const {
    double dx = b.longitude - a.longitude;
    double dy = b.latitude - a.latitude;
    return std::sqrt(dx * dx + dy * dy);
}

double DepthInterpolator::InverseDistanceWeight(const std::vector<std::pair<double, double>>& distances) const {
    double weight_sum = 0.0;
    double value_sum = 0.0;
    const double power = 2.0;
    const double epsilon = 1e-10;
    
    for (const auto& d : distances) {
        double weight = 1.0 / std::pow(d.first + epsilon, power);
        weight_sum += weight;
        value_sum += weight * d.second;
    }
    
    return value_sum / weight_sum;
}

TidePredictor::TidePredictor() {
}

TidePredictor::~TidePredictor() {
}

void TidePredictor::SetTideStation(const std::string& station_id, const Coordinate& position) {
    TideStation station;
    station.station_id = station_id;
    station.position = position;
    station.current_height = 0.0;
    m_stations.push_back(station);
}

void TidePredictor::SetTideData(const std::string& station_id, double current_height,
                                 const DateTime& next_high, const DateTime& next_low) {
    for (auto& station : m_stations) {
        if (station.station_id == station_id) {
            station.current_height = current_height;
            station.next_high_tide = next_high;
            station.next_low_tide = next_low;
            return;
        }
    }
    
    TideStation station;
    station.station_id = station_id;
    station.position = Coordinate{0.0, 0.0};
    station.current_height = current_height;
    station.next_high_tide = next_high;
    station.next_low_tide = next_low;
    m_stations.push_back(station);
}

double TidePredictor::PredictTide(const Coordinate& position, const DateTime& time) const {
    const TideStation* station = FindNearestStation(position);
    if (!station) {
        return 0.0;
    }
    
    return HarmonicPrediction(*station, time);
}

double TidePredictor::GetCurrentTide(const Coordinate& position) const {
    const TideStation* station = FindNearestStation(position);
    if (!station) {
        return 0.0;
    }
    return station->current_height;
}

bool TidePredictor::HasStation(const std::string& station_id) const {
    for (const auto& station : m_stations) {
        if (station.station_id == station_id) {
            return true;
        }
    }
    return false;
}

const TidePredictor::TideStation* TidePredictor::FindNearestStation(const Coordinate& position) const {
    if (m_stations.empty()) {
        return nullptr;
    }
    
    double min_distance = std::numeric_limits<double>::max();
    const TideStation* nearest = nullptr;
    
    for (const auto& station : m_stations) {
        double dist = CalculateDistance(position, station.position);
        if (dist < min_distance) {
            min_distance = dist;
            nearest = &station;
        }
    }
    
    return nearest;
}

double TidePredictor::HarmonicPrediction(const TideStation& station, const DateTime& time) const {
    double current_tide = station.current_height;
    
    int64_t current_ts = station.next_high_tide.ToTimestamp();
    int64_t target_ts = time.ToTimestamp();
    int64_t diff = target_ts - current_ts;
    
    double tidal_period = 12.42 * 3600;
    double phase = (diff % static_cast<int64_t>(tidal_period)) / tidal_period * 2.0 * M_PI;
    
    double amplitude = 1.5;
    double predicted = current_tide + amplitude * std::sin(phase);
    
    return predicted;
}

double TidePredictor::CalculateDistance(const Coordinate& a, const Coordinate& b) const {
    double dx = b.longitude - a.longitude;
    double dy = b.latitude - a.latitude;
    return std::sqrt(dx * dx + dy * dy);
}

}
}
