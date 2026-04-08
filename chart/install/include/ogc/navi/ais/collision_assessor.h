#pragma once

#include "ogc/navi/ais/ais_target.h"
#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <vector>
#include <functional>

namespace ogc {
namespace navi {

struct CollisionAssessment {
    uint32_t target_mmsi;
    double cpa;
    double tcpa;
    double distance;
    double bearing;
    double relative_speed;
    bool collision_risk;
    AlertLevel risk_level;
    std::string recommendation;
    
    CollisionAssessment()
        : target_mmsi(0)
        , cpa(0.0)
        , tcpa(0.0)
        , distance(0.0)
        , bearing(0.0)
        , relative_speed(0.0)
        , collision_risk(false)
        , risk_level(AlertLevel::Info)
    {}
};

using CollisionAssessmentCallback = std::function<void(const CollisionAssessment&)>;

class OGC_NAVI_API CollisionAssessor {
public:
    static CollisionAssessor& Instance();
    
    void SetCpaThreshold(double cpa_nm);
    void SetTcpaThreshold(double tcpa_minutes);
    void SetDangerCpaThreshold(double cpa_nm);
    void SetDangerTcpaThreshold(double tcpa_minutes);
    
    double GetCpaThreshold() const;
    double GetTcpaThreshold() const;
    
    CollisionAssessment AssessTarget(
        const PositionData& own_position,
        double own_course,
        double own_speed,
        const AisTarget* target);
    
    std::vector<CollisionAssessment> AssessAllTargets(
        const PositionData& own_position,
        double own_course,
        double own_speed,
        const std::vector<AisTarget*>& targets);
    
    void CalculateCpaTcpa(
        double own_lat, double own_lon,
        double own_course, double own_speed,
        double target_lat, double target_lon,
        double target_course, double target_speed,
        double& cpa, double& tcpa);
    
    void SetCollisionAssessmentCallback(CollisionAssessmentCallback callback);
    
private:
    CollisionAssessor();
    CollisionAssessor(const CollisionAssessor&) = delete;
    CollisionAssessor& operator=(const CollisionAssessor&) = delete;
    
    AlertLevel DetermineRiskLevel(double cpa, double tcpa) const;
    
    double cpa_threshold_;
    double tcpa_threshold_;
    double danger_cpa_threshold_;
    double danger_tcpa_threshold_;
    CollisionAssessmentCallback assessment_callback_;
};

}
}
