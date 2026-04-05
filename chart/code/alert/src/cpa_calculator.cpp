#include "ogc/alert/cpa_calculator.h"
#include "ogc/alert/exception.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

CPACalculator::CPACalculator()
    : m_cpaLevel1(6.0)
    , m_cpaLevel2(4.0)
    , m_cpaLevel3(2.0)
    , m_cpaLevel4(1.0)
    , m_tcpaLevel1(30.0)
    , m_tcpaLevel2(20.0)
    , m_tcpaLevel3(10.0)
    , m_tcpaLevel4(5.0) {
}

CPACalculator::~CPACalculator() {
}

CPAResult CPACalculator::Calculate(const ShipMotion& own_ship, const ShipMotion& target_ship) {
    return Calculate(own_ship.position, own_ship.speed, own_ship.heading,
                     target_ship.position, target_ship.speed, target_ship.heading);
}

CPAResult CPACalculator::Calculate(const Coordinate& own_pos, double own_speed, double own_heading,
                                    const Coordinate& target_pos, double target_speed, double target_heading) {
    CPAResult result;
    
    double dx = target_pos.longitude - own_pos.longitude;
    double dy = target_pos.latitude - own_pos.latitude;
    double initial_distance = std::sqrt(dx * dx + dy * dy);
    
    double own_heading_rad = DegreesToRadians(own_heading);
    double target_heading_rad = DegreesToRadians(target_heading);
    
    double own_vx = own_speed * std::sin(own_heading_rad);
    double own_vy = own_speed * std::cos(own_heading_rad);
    
    double target_vx = target_speed * std::sin(target_heading_rad);
    double target_vy = target_speed * std::cos(target_heading_rad);
    
    double rel_vx = target_vx - own_vx;
    double rel_vy = target_vy - own_vy;
    
    double rel_speed_squared = rel_vx * rel_vx + rel_vy * rel_vy;
    double rel_speed = std::sqrt(rel_speed_squared);
    result.relative_speed = rel_speed;
    
    if (rel_speed < 0.001) {
        result.cpa = initial_distance;
        result.tcpa = -1.0;
        result.distance_at_cpa = initial_distance;
        result.cpa_position_own = own_pos;
        result.cpa_position_target = target_pos;
        result.is_dangerous = initial_distance < m_cpaLevel2;
        result.alert_level = result.is_dangerous ? AlertLevel::kLevel2 : AlertLevel::kNone;
        result.warning_message = "Vessels on parallel course";
        return result;
    }
    
    double tcpa = -(dx * rel_vx + dy * rel_vy) / rel_speed_squared;
    result.tcpa = tcpa;
    
    double cpa_x = dx + rel_vx * tcpa;
    double cpa_y = dy + rel_vy * tcpa;
    double cpa = std::sqrt(cpa_x * cpa_x + cpa_y * cpa_y);
    result.cpa = cpa;
    result.distance_at_cpa = cpa;
    
    double cpa_own_x = own_vx * tcpa;
    double cpa_own_y = own_vy * tcpa;
    result.cpa_position_own.longitude = own_pos.longitude + cpa_own_x;
    result.cpa_position_own.latitude = own_pos.latitude + cpa_own_y;
    
    double cpa_target_x = target_vx * tcpa;
    double cpa_target_y = target_vy * tcpa;
    result.cpa_position_target.longitude = target_pos.longitude + cpa_target_x;
    result.cpa_position_target.latitude = target_pos.latitude + cpa_target_y;
    
    result.relative_bearing = CalculateBearing(own_pos, target_pos);
    
    result.alert_level = DetermineAlertLevel(cpa, tcpa);
    result.is_dangerous = (tcpa > 0 && cpa < m_cpaLevel2);
    
    if (result.alert_level == AlertLevel::kLevel4) {
        result.warning_message = "CRITICAL: Immediate collision risk. Take evasive action now.";
    } else if (result.alert_level == AlertLevel::kLevel3) {
        result.warning_message = "WARNING: High collision risk. Consider course alteration.";
    } else if (result.alert_level == AlertLevel::kLevel2) {
        result.warning_message = "CAUTION: Moderate collision risk. Monitor closely.";
    } else if (result.alert_level == AlertLevel::kLevel1) {
        result.warning_message = "ADVISORY: Low collision risk. Maintain watch.";
    } else {
        result.warning_message = "No collision risk detected.";
    }
    
    return result;
}

CollisionRisk CPACalculator::AssessCollisionRisk(const ShipMotion& own_ship,
                                                   const std::vector<ShipMotion>& target_ships) {
    CollisionRisk risk;
    risk.risk_probability = 0.0;
    risk.time_to_collision = std::numeric_limits<double>::max();
    risk.distance_at_risk = std::numeric_limits<double>::max();
    
    if (target_ships.empty()) {
        return risk;
    }
    
    int dangerous_count = 0;
    double total_risk = 0.0;
    
    for (const auto& target : target_ships) {
        CPAResult cpa = Calculate(own_ship, target);
        
        if (cpa.tcpa > 0 && cpa.cpa < m_cpaLevel1) {
            dangerous_count++;
            
            double risk_factor = (m_cpaLevel1 - cpa.cpa) / m_cpaLevel1;
            double time_factor = 1.0 / (1.0 + cpa.tcpa / 60.0);
            double target_risk = risk_factor * time_factor;
            
            total_risk += target_risk;
            
            if (cpa.tcpa < risk.time_to_collision) {
                risk.time_to_collision = cpa.tcpa;
                risk.distance_at_risk = cpa.cpa;
            }
            
            risk.risk_zones.push_back({cpa.cpa, cpa.tcpa});
        }
    }
    
    risk.risk_probability = std::min(1.0, total_risk / static_cast<double>(target_ships.size()));
    
    return risk;
}

void CPACalculator::SetCPAThresholds(double level1, double level2, double level3, double level4) {
    m_cpaLevel1 = level1;
    m_cpaLevel2 = level2;
    m_cpaLevel3 = level3;
    m_cpaLevel4 = level4;
}

void CPACalculator::SetTCPAThresholds(double level1, double level2, double level3, double level4) {
    m_tcpaLevel1 = level1;
    m_tcpaLevel2 = level2;
    m_tcpaLevel3 = level3;
    m_tcpaLevel4 = level4;
}

double CPACalculator::GetCPAThreshold(AlertLevel level) const {
    switch (level) {
        case AlertLevel::kLevel1: return m_cpaLevel1;
        case AlertLevel::kLevel2: return m_cpaLevel2;
        case AlertLevel::kLevel3: return m_cpaLevel3;
        case AlertLevel::kLevel4: return m_cpaLevel4;
        default: return m_cpaLevel1;
    }
}

double CPACalculator::GetTCPAThreshold(AlertLevel level) const {
    switch (level) {
        case AlertLevel::kLevel1: return m_tcpaLevel1;
        case AlertLevel::kLevel2: return m_tcpaLevel2;
        case AlertLevel::kLevel3: return m_tcpaLevel3;
        case AlertLevel::kLevel4: return m_tcpaLevel4;
        default: return m_tcpaLevel1;
    }
}

AlertLevel CPACalculator::DetermineAlertLevel(double cpa, double tcpa) const {
    if (tcpa < 0) {
        return AlertLevel::kNone;
    }
    
    if (cpa <= m_cpaLevel4 && tcpa <= m_tcpaLevel4) {
        return AlertLevel::kLevel4;
    }
    if (cpa <= m_cpaLevel3 && tcpa <= m_tcpaLevel3) {
        return AlertLevel::kLevel3;
    }
    if (cpa <= m_cpaLevel2 && tcpa <= m_tcpaLevel2) {
        return AlertLevel::kLevel2;
    }
    if (cpa <= m_cpaLevel1 && tcpa <= m_tcpaLevel1) {
        return AlertLevel::kLevel1;
    }
    
    return AlertLevel::kNone;
}

Vector2D CPACalculator::PositionToVector(const Coordinate& pos) const {
    return Vector2D(pos.longitude, pos.latitude);
}

Coordinate CPACalculator::VectorToPosition(const Vector2D& vec, const Coordinate& reference) const {
    Coordinate pos;
    pos.longitude = reference.longitude + vec.x;
    pos.latitude = reference.latitude + vec.y;
    return pos;
}

double CPACalculator::CalculateDistance(const Coordinate& a, const Coordinate& b) {
    double dx = b.longitude - a.longitude;
    double dy = b.latitude - a.latitude;
    return std::sqrt(dx * dx + dy * dy);
}

double CPACalculator::CalculateBearing(const Coordinate& from, const Coordinate& to) {
    double dx = to.longitude - from.longitude;
    double dy = to.latitude - from.latitude;
    double bearing = std::atan2(dx, dy) * 180.0 / M_PI;
    if (bearing < 0) bearing += 360.0;
    return bearing;
}

double CPACalculator::DegreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double CPACalculator::RadiansToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

CollisionAvoidance::CollisionAvoidance()
    : m_minCPA(2.0)
    , m_minTCPA(10.0) {
}

CollisionAvoidance::~CollisionAvoidance() {
}

CollisionAvoidance::Maneuver CollisionAvoidance::CalculateBestManeuver(
    const ShipMotion& own_ship,
    const ShipMotion& target_ship,
    const CPAResult& cpa_result) {
    
    Maneuver best_maneuver;
    best_maneuver.type = Maneuver::kNone;
    best_maneuver.course_change = 0.0;
    best_maneuver.speed_change = 0.0;
    best_maneuver.time_to_execute = 0.0;
    best_maneuver.effectiveness = 0.0;
    
    if (cpa_result.cpa >= m_minCPA || cpa_result.tcpa < 0) {
        return best_maneuver;
    }
    
    Maneuver best_course = EvaluateCourseChange(own_ship, target_ship, 30.0);
    Maneuver best_speed = EvaluateSpeedChange(own_ship, target_ship, 0.5);
    
    if (best_course.effectiveness > best_speed.effectiveness) {
        best_maneuver = best_course;
    } else {
        best_maneuver = best_speed;
    }
    
    Maneuver combined;
    combined.type = Maneuver::kBoth;
    combined.course_change = best_maneuver.course_change;
    combined.speed_change = best_maneuver.speed_change;
    combined.time_to_execute = 0.0;
    combined.effectiveness = best_maneuver.effectiveness * 1.5;
    
    if (combined.effectiveness > best_maneuver.effectiveness) {
        best_maneuver = combined;
    }
    
    return best_maneuver;
}

std::vector<CollisionAvoidance::Maneuver> CollisionAvoidance::GetAlternativeManeuvers(
    const ShipMotion& own_ship,
    const ShipMotion& target_ship,
    const CPAResult& cpa_result) {
    
    std::vector<Maneuver> maneuvers;
    
    std::vector<double> course_changes = {15.0, 30.0, 45.0, 60.0, 90.0};
    for (double change : course_changes) {
        Maneuver m = EvaluateCourseChange(own_ship, target_ship, change);
        if (m.effectiveness > 0.5) {
            maneuvers.push_back(m);
        }
    }
    
    std::vector<double> speed_changes = {0.25, 0.5, 0.75, 1.0};
    for (double change : speed_changes) {
        Maneuver m = EvaluateSpeedChange(own_ship, target_ship, change);
        if (m.effectiveness > 0.5) {
            maneuvers.push_back(m);
        }
    }
    
    std::sort(maneuvers.begin(), maneuvers.end(), 
              [](const Maneuver& a, const Maneuver& b) {
                  return a.effectiveness > b.effectiveness;
              });
    
    return maneuvers;
}

void CollisionAvoidance::SetMinCPA(double min_cpa) {
    m_minCPA = min_cpa;
}

void CollisionAvoidance::SetMinTCPA(double min_tcpa) {
    m_minTCPA = min_tcpa;
}

CollisionAvoidance::Maneuver CollisionAvoidance::EvaluateCourseChange(
    const ShipMotion& own_ship,
    const ShipMotion& target_ship,
    double course_change) {
    
    Maneuver maneuver;
    maneuver.type = Maneuver::kAlterCourse;
    maneuver.course_change = course_change;
    maneuver.speed_change = 0.0;
    maneuver.time_to_execute = 0.0;
    
    ShipMotion modified_ship = own_ship;
    modified_ship.heading += course_change;
    modified_ship.UpdateVelocity();
    
    CPACalculator calc;
    CPAResult new_cpa = calc.Calculate(modified_ship, target_ship);
    
    if (new_cpa.cpa >= m_minCPA) {
        maneuver.effectiveness = 1.0;
    } else {
        maneuver.effectiveness = new_cpa.cpa / m_minCPA;
    }
    
    return maneuver;
}

CollisionAvoidance::Maneuver CollisionAvoidance::EvaluateSpeedChange(
    const ShipMotion& own_ship,
    const ShipMotion& target_ship,
    double speed_change) {
    
    Maneuver maneuver;
    maneuver.type = Maneuver::kReduceSpeed;
    maneuver.course_change = 0.0;
    maneuver.speed_change = speed_change;
    maneuver.time_to_execute = 0.0;
    
    ShipMotion modified_ship = own_ship;
    modified_ship.speed *= (1.0 - speed_change);
    modified_ship.UpdateVelocity();
    
    CPACalculator calc;
    CPAResult new_cpa = calc.Calculate(modified_ship, target_ship);
    
    if (new_cpa.cpa >= m_minCPA) {
        maneuver.effectiveness = 1.0;
    } else {
        maneuver.effectiveness = new_cpa.cpa / m_minCPA;
    }
    
    return maneuver;
}

MultiTargetTracker::MultiTargetTracker()
    : m_maxTargets(100) {
}

MultiTargetTracker::~MultiTargetTracker() {
}

void MultiTargetTracker::AddOrUpdateTarget(const std::string& target_id, const ShipMotion& motion) {
    for (auto& target : m_targets) {
        if (target.target_id == target_id) {
            target.motion = motion;
            target.cpa_result = m_calculator.Calculate(m_ownShip, motion);
            target.last_update = DateTime::Now();
            target.track_count++;
            SortByRisk();
            return;
        }
    }
    
    if (m_targets.size() >= m_maxTargets) {
        SortByRisk();
        m_targets.pop_back();
    }
    
    TrackedTarget new_target;
    new_target.target_id = target_id;
    new_target.motion = motion;
    new_target.cpa_result = m_calculator.Calculate(m_ownShip, motion);
    new_target.last_update = DateTime::Now();
    new_target.track_count = 1;
    m_targets.push_back(new_target);
    
    SortByRisk();
}

void MultiTargetTracker::RemoveTarget(const std::string& target_id) {
    m_targets.erase(
        std::remove_if(m_targets.begin(), m_targets.end(),
                       [&target_id](const TrackedTarget& t) {
                           return t.target_id == target_id;
                       }),
        m_targets.end());
}

void MultiTargetTracker::ClearTargets() {
    m_targets.clear();
}

std::vector<MultiTargetTracker::TrackedTarget> MultiTargetTracker::GetDangerousTargets() const {
    std::vector<TrackedTarget> dangerous;
    for (const auto& target : m_targets) {
        if (target.cpa_result.is_dangerous) {
            dangerous.push_back(target);
        }
    }
    return dangerous;
}

std::vector<MultiTargetTracker::TrackedTarget> MultiTargetTracker::GetAllTargets() const {
    return m_targets;
}

MultiTargetTracker::TrackedTarget MultiTargetTracker::GetMostDangerousTarget() const {
    if (m_targets.empty()) {
        return TrackedTarget();
    }
    
    TrackedTarget most_dangerous = m_targets[0];
    for (const auto& target : m_targets) {
        if (target.cpa_result.alert_level > most_dangerous.cpa_result.alert_level) {
            most_dangerous = target;
        } else if (target.cpa_result.alert_level == most_dangerous.cpa_result.alert_level) {
            if (target.cpa_result.tcpa < most_dangerous.cpa_result.tcpa && target.cpa_result.tcpa > 0) {
                most_dangerous = target;
            }
        }
    }
    
    return most_dangerous;
}

void MultiTargetTracker::SetOwnShipMotion(const ShipMotion& motion) {
    m_ownShip = motion;
    
    for (auto& target : m_targets) {
        target.cpa_result = m_calculator.Calculate(m_ownShip, target.motion);
    }
    
    SortByRisk();
}

const ShipMotion& MultiTargetTracker::GetOwnShipMotion() const {
    return m_ownShip;
}

void MultiTargetTracker::SetMaxTargets(size_t max_targets) {
    m_maxTargets = max_targets;
}

size_t MultiTargetTracker::GetMaxTargets() const {
    return m_maxTargets;
}

void MultiTargetTracker::SortByRisk() {
    std::sort(m_targets.begin(), m_targets.end(),
              [](const TrackedTarget& a, const TrackedTarget& b) {
                  if (a.cpa_result.alert_level != b.cpa_result.alert_level) {
                      return static_cast<int>(a.cpa_result.alert_level) > 
                             static_cast<int>(b.cpa_result.alert_level);
                  }
                  return a.cpa_result.tcpa < b.cpa_result.tcpa;
              });
}

}
}
