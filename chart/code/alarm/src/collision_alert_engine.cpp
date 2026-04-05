#include "alert/collision_alert_engine.h"
#include "alert/alert_exception.h"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace alert {

CollisionAlertEngine::CollisionAlertEngine()
    : m_aisProvider(nullptr) {}

CollisionAlertEngine::~CollisionAlertEngine() {}

AlertType CollisionAlertEngine::GetType() const {
    return AlertType::kCollisionAlert;
}

Alert CollisionAlertEngine::Evaluate(const AlertContext& context) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    std::vector<CollisionTarget> riskTargets = GetRiskTargets(context);
    
    if (riskTargets.empty()) {
        return Alert();
    }
    
    std::sort(riskTargets.begin(), riskTargets.end(), 
        [](const CollisionTarget& a, const CollisionTarget& b) {
            return AlertLevelToPriority(a.riskLevel) < AlertLevelToPriority(b.riskLevel);
        });
    
    const CollisionTarget& topTarget = riskTargets.front();
    
    Alert alert(Alert::GenerateAlertId(AlertType::kCollisionAlert), 
                AlertType::kCollisionAlert, topTarget.riskLevel);
    alert.SetTitle("Collision Alert");
    alert.SetMessage("Collision risk detected with target: " + topTarget.targetName);
    alert.SetPosition(topTarget.position.GetLongitude(), topTarget.position.GetLatitude());
    
    alert.SetDetail("target_id", std::stod(topTarget.targetId.empty() ? "0" : topTarget.targetId));
    alert.SetDetail("cpa_nm", topTarget.cpa);
    alert.SetDetail("tcpa_min", topTarget.tcpa);
    alert.SetDetail("target_speed", topTarget.speed);
    alert.SetDetail("target_course", topTarget.course);
    
    return alert;
}

void CollisionAlertEngine::SetThreshold(const ThresholdConfig& config) {
    m_threshold = config;
}

ThresholdConfig CollisionAlertEngine::GetThreshold() const {
    return m_threshold;
}

std::string CollisionAlertEngine::GetName() const {
    return "CollisionAlertEngine";
}

void CollisionAlertEngine::CalculateCPA_TCPA(
    const Coordinate& myPos, double mySpeed, double myCourse,
    const Coordinate& targetPos, double targetSpeed, double targetCourse,
    double& cpa, double& tcpa) {
    
    double latAvg = (myPos.GetLatitude() + targetPos.GetLatitude()) / 2.0 * M_PI / 180.0;
    double metersPerDegLon = AlertConstants::kEarthRadiusMeters * std::cos(latAvg) * M_PI / 180.0;
    double metersPerDegLat = AlertConstants::kEarthRadiusMeters * M_PI / 180.0;
    
    double dx = (targetPos.GetLongitude() - myPos.GetLongitude()) * metersPerDegLon;
    double dy = (targetPos.GetLatitude() - myPos.GetLatitude()) * metersPerDegLat;
    double distance = std::sqrt(dx * dx + dy * dy);
    
    double mySpeedMs = mySpeed * AlertConstants::kKnotsToMs;
    double targetSpeedMs = targetSpeed * AlertConstants::kKnotsToMs;
    
    double myCourseRad = myCourse * M_PI / 180.0;
    double targetCourseRad = targetCourse * M_PI / 180.0;
    
    double myVx = mySpeedMs * std::sin(myCourseRad);
    double myVy = mySpeedMs * std::cos(myCourseRad);
    double targetVx = targetSpeedMs * std::sin(targetCourseRad);
    double targetVy = targetSpeedMs * std::cos(targetCourseRad);
    
    double relVx = targetVx - myVx;
    double relVy = targetVy - myVy;
    double relSpeed = std::sqrt(relVx * relVx + relVy * relVy);
    
    if (relSpeed < 0.01) {
        cpa = distance / AlertConstants::kNauticalMileToMeters;
        tcpa = std::numeric_limits<double>::max();
        return;
    }
    
    double tcpaSeconds = -(dx * relVx + dy * relVy) / (relSpeed * relSpeed);
    
    double cpaX = dx + relVx * tcpaSeconds;
    double cpaY = dy + relVy * tcpaSeconds;
    double cpaMeters = std::sqrt(cpaX * cpaX + cpaY * cpaY);
    
    cpa = cpaMeters / AlertConstants::kNauticalMileToMeters;
    tcpa = tcpaSeconds / 60.0;
    
    if (tcpa < 0) {
        tcpa = -tcpa;
    }
}

AlertLevel CollisionAlertEngine::AssessRisk(double cpa, double tcpa) {
    double cpaLevel1 = m_threshold.GetLevel1Threshold();
    double cpaLevel2 = m_threshold.GetLevel2Threshold();
    double cpaLevel3 = m_threshold.GetLevel3Threshold();
    double cpaLevel4 = m_threshold.GetLevel4Threshold();
    
    double tcpaLevel1 = m_threshold.GetParameter("tcpa_level1", 6.0);
    double tcpaLevel2 = m_threshold.GetParameter("tcpa_level2", 15.0);
    double tcpaLevel3 = m_threshold.GetParameter("tcpa_level3", 30.0);
    
    if (cpa < cpaLevel1 && tcpa < tcpaLevel1) return AlertLevel::kLevel1_Critical;
    if (cpa < cpaLevel2 && tcpa < tcpaLevel2) return AlertLevel::kLevel2_Severe;
    if (cpa < cpaLevel3 && tcpa < tcpaLevel3) return AlertLevel::kLevel3_Moderate;
    if (cpa < cpaLevel4) return AlertLevel::kLevel4_Minor;
    
    return AlertLevel::kNone;
}

std::vector<CollisionTarget> CollisionAlertEngine::GetRiskTargets(const AlertContext& context) {
    std::vector<CollisionTarget> riskTargets;
    
    if (!m_aisProvider) {
        return riskTargets;
    }
    
    std::vector<AISTarget> targets = m_aisProvider(context.GetPosition(), 10.0);
    
    for (const auto& target : targets) {
        double cpa, tcpa;
        CalculateCPA_TCPA(
            context.GetPosition(), context.GetSpeed(), context.GetCourse(),
            target.position, target.speed, target.course,
            cpa, tcpa);
        
        AlertLevel riskLevel = AssessRisk(cpa, tcpa);
        
        if (riskLevel != AlertLevel::kNone) {
            CollisionTarget riskTarget;
            riskTarget.targetId = target.targetId;
            riskTarget.targetName = target.targetName;
            riskTarget.position = target.position;
            riskTarget.speed = target.speed;
            riskTarget.course = target.course;
            riskTarget.cpa = cpa;
            riskTarget.tcpa = tcpa;
            riskTarget.riskLevel = riskLevel;
            
            riskTargets.push_back(riskTarget);
        }
    }
    
    return riskTargets;
}

void CollisionAlertEngine::SetAISProvider(std::vector<AISTarget> (*provider)(const Coordinate&, double)) {
    m_aisProvider = provider;
}

}
