#include "ogc/navi/ais/collision_assessor.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace navi {

CollisionAssessor::CollisionAssessor()
    : cpa_threshold_(2.0)
    , tcpa_threshold_(30.0)
    , danger_cpa_threshold_(0.5)
    , danger_tcpa_threshold_(10.0)
{
}

CollisionAssessor& CollisionAssessor::Instance() {
    static CollisionAssessor instance;
    return instance;
}

void CollisionAssessor::SetCpaThreshold(double cpa_nm) {
    cpa_threshold_ = cpa_nm;
}

void CollisionAssessor::SetTcpaThreshold(double tcpa_minutes) {
    tcpa_threshold_ = tcpa_minutes;
}

void CollisionAssessor::SetDangerCpaThreshold(double cpa_nm) {
    danger_cpa_threshold_ = cpa_nm;
}

void CollisionAssessor::SetDangerTcpaThreshold(double tcpa_minutes) {
    danger_tcpa_threshold_ = tcpa_minutes;
}

double CollisionAssessor::GetCpaThreshold() const {
    return cpa_threshold_;
}

double CollisionAssessor::GetTcpaThreshold() const {
    return tcpa_threshold_;
}

CollisionAssessment CollisionAssessor::AssessTarget(
    const PositionData& own_position,
    double own_course,
    double own_speed,
    const AisTarget* target) {
    
    CollisionAssessment assessment;
    
    if (!target) {
        return assessment;
    }
    
    AisTargetData data = target->ToData();
    assessment.target_mmsi = data.mmsi;
    
    GeoPoint own_pos(own_position.longitude, own_position.latitude);
    GeoPoint target_pos(data.longitude, data.latitude);
    
    assessment.distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        own_pos, target_pos) / 1852.0;
    assessment.bearing = NavigationCalculator::Instance().CalculateBearingToWaypoint(
        own_pos, target_pos);
    
    CalculateCpaTcpa(
        own_position.latitude, own_position.longitude,
        own_course, own_speed,
        data.latitude, data.longitude,
        data.course_over_ground, data.speed_over_ground,
        assessment.cpa, assessment.tcpa);
    
    assessment.relative_speed = std::abs(own_speed - data.speed_over_ground);
    
    assessment.risk_level = DetermineRiskLevel(assessment.cpa, assessment.tcpa);
    
    assessment.collision_risk = (assessment.cpa < cpa_threshold_ && 
                                  assessment.tcpa > 0 && 
                                  assessment.tcpa < tcpa_threshold_);
    
    if (assessment.collision_risk) {
        if (assessment.risk_level == AlertLevel::Critical) {
            assessment.recommendation = "Immediate action required - collision imminent";
        } else if (assessment.risk_level == AlertLevel::Warning) {
            assessment.recommendation = "Monitor target closely - potential collision risk";
        } else {
            assessment.recommendation = "Be aware of target position";
        }
    } else {
        assessment.recommendation = "No collision risk";
    }
    
    if (assessment_callback_ && assessment.collision_risk) {
        assessment_callback_(assessment);
    }
    
    return assessment;
}

std::vector<CollisionAssessment> CollisionAssessor::AssessAllTargets(
    const PositionData& own_position,
    double own_course,
    double own_speed,
    const std::vector<AisTarget*>& targets) {
    
    std::vector<CollisionAssessment> assessments;
    
    for (const auto& target : targets) {
        if (target) {
            assessments.push_back(AssessTarget(own_position, own_course, own_speed, target));
        }
    }
    
    std::sort(assessments.begin(), assessments.end(),
        [](const CollisionAssessment& a, const CollisionAssessment& b) {
            if (a.collision_risk != b.collision_risk) {
                return a.collision_risk;
            }
            return a.tcpa < b.tcpa;
        });
    
    return assessments;
}

void CollisionAssessor::CalculateCpaTcpa(
    double own_lat, double own_lon,
    double own_course, double own_speed,
    double target_lat, double target_lon,
    double target_course, double target_speed,
    double& cpa, double& tcpa) {
    
    double own_speed_ms = own_speed * 0.514444;
    double target_speed_ms = target_speed * 0.514444;
    
    double own_course_rad = own_course * M_PI / 180.0;
    double target_course_rad = target_course * M_PI / 180.0;
    
    double own_vx = own_speed_ms * std::sin(own_course_rad);
    double own_vy = own_speed_ms * std::cos(own_course_rad);
    
    double target_vx = target_speed_ms * std::sin(target_course_rad);
    double target_vy = target_speed_ms * std::cos(target_course_rad);
    
    double rel_vx = target_vx - own_vx;
    double rel_vy = target_vy - own_vy;
    
    GeoPoint own_pos(own_lon, own_lat);
    GeoPoint target_pos(target_lon, target_lat);
    
    double distance_m = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        own_pos, target_pos);
    
    double bearing_rad = NavigationCalculator::Instance().CalculateBearingToWaypoint(
        own_pos, target_pos) * M_PI / 180.0;
    
    double rel_x = distance_m * std::sin(bearing_rad);
    double rel_y = distance_m * std::cos(bearing_rad);
    
    double rel_speed_sq = rel_vx * rel_vx + rel_vy * rel_vy;
    
    if (rel_speed_sq < 0.01) {
        cpa = distance_m / 1852.0;
        tcpa = 0.0;
        return;
    }
    
    double tcpa_seconds = -(rel_x * rel_vx + rel_y * rel_vy) / rel_speed_sq;
    
    if (tcpa_seconds < 0) {
        cpa = distance_m / 1852.0;
        tcpa = 0.0;
        return;
    }
    
    double cpa_x = rel_x + rel_vx * tcpa_seconds;
    double cpa_y = rel_y + rel_vy * tcpa_seconds;
    
    cpa = std::sqrt(cpa_x * cpa_x + cpa_y * cpa_y) / 1852.0;
    tcpa = tcpa_seconds / 60.0;
}

void CollisionAssessor::SetCollisionAssessmentCallback(CollisionAssessmentCallback callback) {
    assessment_callback_ = callback;
}

AlertLevel CollisionAssessor::DetermineRiskLevel(double cpa, double tcpa) const {
    if (cpa < danger_cpa_threshold_ && tcpa > 0 && tcpa < danger_tcpa_threshold_) {
        return AlertLevel::Critical;
    }
    
    if (cpa < cpa_threshold_ && tcpa > 0 && tcpa < tcpa_threshold_) {
        return AlertLevel::Warning;
    }
    
    return AlertLevel::Info;
}

}
}
