#include "ogc/navi/route/ukc_calculator.h"
#include <cmath>

namespace ogc {
namespace navi {

UkcCalculator::UkcCalculator()
    : vessel_draft_(0.0)
    , vessel_beam_(0.0)
    , vessel_length_(0.0)
    , safety_margin_(1.0)
    , squat_enabled_(true)
{
}

UkcCalculator& UkcCalculator::Instance() {
    static UkcCalculator instance;
    return instance;
}

void UkcCalculator::SetVesselDraft(double draft) {
    vessel_draft_ = draft;
}

void UkcCalculator::SetVesselBeam(double beam) {
    vessel_beam_ = beam;
}

void UkcCalculator::SetVesselLength(double length) {
    vessel_length_ = length;
}

void UkcCalculator::SetSafetyMargin(double margin) {
    safety_margin_ = margin;
}

void UkcCalculator::SetSquatEnabled(bool enabled) {
    squat_enabled_ = enabled;
}

double UkcCalculator::CalculateSquat(double speed, double depth) const {
    if (speed <= 0.0 || depth <= 0.0) {
        return 0.0;
    }
    
    double speed_knots = speed;
    double speed_ms = speed_knots * 0.514444;
    double froude_number = speed_ms / std::sqrt(9.81 * depth);
    
    double squat = 0.0;
    if (froude_number < 0.7) {
        squat = 2.4 * (speed_ms * speed_ms) / (2.0 * 9.81);
    } else {
        squat = depth * 0.1 * froude_number;
    }
    
    return squat;
}

UkcResult UkcCalculator::CalculateUkc(
    double charted_depth,
    const UkcParameters& params) const {
    
    UkcResult result;
    result.charted_depth = charted_depth;
    result.tide_height = params.tide_height;
    
    if (params.apply_squat) {
        result.squat = CalculateSquat(params.vessel_speed, charted_depth);
    } else {
        result.squat = 0.0;
    }
    
    double available_depth = charted_depth + params.tide_height;
    double required_depth = params.vessel_draft + result.squat + params.safety_margin;
    
    result.ukc = available_depth - required_depth;
    result.safety_margin = params.safety_margin;
    
    if (result.ukc < 0.0) {
        result.is_safe = false;
        result.warning_message = "UKC is negative: " + std::to_string(result.ukc) + " meters";
    } else if (result.ukc < params.safety_margin * 0.5) {
        result.is_safe = true;
        result.warning_message = "UKC is marginal: " + std::to_string(result.ukc) + " meters";
    } else {
        result.is_safe = true;
        result.warning_message = "";
    }
    
    return result;
}

UkcResult UkcCalculator::CalculateUkcAtPosition(
    const GeoPoint& position,
    const UkcParameters& params) const {
    
    double charted_depth = 10.0;
    
    return CalculateUkc(charted_depth, params);
}

bool UkcCalculator::CheckRouteSafety(
    const std::vector<GeoPoint>& route_points,
    const UkcParameters& params,
    std::vector<int>& unsafe_segments) {
    
    unsafe_segments.clear();
    
    for (size_t i = 0; i < route_points.size(); ++i) {
        UkcResult result = CalculateUkcAtPosition(route_points[i], params);
        if (!result.is_safe) {
            unsafe_segments.push_back(static_cast<int>(i));
        }
    }
    
    return unsafe_segments.empty();
}

double UkcCalculator::GetMinimumSafeDepth(double draft, double safety_margin) const {
    return draft + safety_margin;
}

}
}
