#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>

namespace ogc {
namespace navi {

struct UkcResult {
    double charted_depth;
    double tide_height;
    double squat;
    double ukc;
    double safety_margin;
    bool is_safe;
    std::string warning_message;
    
    UkcResult()
        : charted_depth(0.0)
        , tide_height(0.0)
        , squat(0.0)
        , ukc(0.0)
        , safety_margin(0.0)
        , is_safe(true)
    {}
};

struct UkcParameters {
    double vessel_draft;
    double vessel_beam;
    double vessel_length;
    double vessel_speed;
    double safety_margin;
    double tide_height;
    bool apply_squat;
    double squat_coefficient;
    
    UkcParameters()
        : vessel_draft(0.0)
        , vessel_beam(0.0)
        , vessel_length(0.0)
        , vessel_speed(0.0)
        , safety_margin(1.0)
        , tide_height(0.0)
        , apply_squat(true)
        , squat_coefficient(1.0)
    {}
};

class OGC_NAVI_API UkcCalculator {
public:
    static UkcCalculator& Instance();
    
    void SetVesselDraft(double draft);
    void SetVesselBeam(double beam);
    void SetVesselLength(double length);
    void SetSafetyMargin(double margin);
    void SetSquatEnabled(bool enabled);
    
    double CalculateSquat(double speed, double depth) const;
    
    UkcResult CalculateUkc(
        double charted_depth,
        const UkcParameters& params) const;
    
    UkcResult CalculateUkcAtPosition(
        const GeoPoint& position,
        const UkcParameters& params) const;
    
    bool CheckRouteSafety(
        const std::vector<GeoPoint>& route_points,
        const UkcParameters& params,
        std::vector<int>& unsafe_segments);
    
    double GetMinimumSafeDepth(double draft, double safety_margin) const;
    
private:
    UkcCalculator();
    UkcCalculator(const UkcCalculator&) = delete;
    UkcCalculator& operator=(const UkcCalculator&) = delete;
    
    double vessel_draft_;
    double vessel_beam_;
    double vessel_length_;
    double safety_margin_;
    bool squat_enabled_;
};

}
}
