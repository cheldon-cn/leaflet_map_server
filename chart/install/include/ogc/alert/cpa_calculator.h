#ifndef OGC_ALERT_CPA_CALCULATOR_H
#define OGC_ALERT_CPA_CALCULATOR_H

#include "export.h"
#include "types.h"
#include <vector>
#include <memory>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

struct Vector2D {
    double x;
    double y;
    
    Vector2D() : x(0.0), y(0.0) {}
    Vector2D(double x_, double y_) : x(x_), y(y_) {}
    
    double Magnitude() const { return std::sqrt(x * x + y * y); }
    double Dot(const Vector2D& other) const { return x * other.x + y * other.y; }
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
};

struct ShipMotion {
    Coordinate position;
    double speed;
    double heading;
    Vector2D velocity;
    
    ShipMotion() : speed(0.0), heading(0.0) {}
    
    void UpdateVelocity() {
        double heading_rad = heading * M_PI / 180.0;
        velocity.x = speed * std::sin(heading_rad);
        velocity.y = speed * std::cos(heading_rad);
    }
};

struct CPAResult {
    double cpa;
    double tcpa;
    double distance_at_cpa;
    Coordinate cpa_position_own;
    Coordinate cpa_position_target;
    double relative_bearing;
    double relative_speed;
    bool is_dangerous;
    AlertLevel alert_level;
    std::string warning_message;
};

struct CollisionRisk {
    double risk_probability;
    double time_to_collision;
    double distance_at_risk;
    std::vector<std::pair<double, double>> risk_zones;
};

class OGC_ALERT_API CPACalculator {
public:
    CPACalculator();
    ~CPACalculator();
    
    CPAResult Calculate(const ShipMotion& own_ship, const ShipMotion& target_ship) const;
    
    CPAResult Calculate(const Coordinate& own_pos, double own_speed, double own_heading,
                        const Coordinate& target_pos, double target_speed, double target_heading) const;
    
    CollisionRisk AssessCollisionRisk(const ShipMotion& own_ship, 
                                       const std::vector<ShipMotion>& target_ships) const;
    
    void SetCPAThresholds(double level1, double level2, double level3, double level4);
    void SetTCPAThresholds(double level1, double level2, double level3, double level4);
    
    double GetCPAThreshold(AlertLevel level) const;
    double GetTCPAThreshold(AlertLevel level) const;
    
    static double CalculateDistance(const Coordinate& a, const Coordinate& b);
    static double CalculateBearing(const Coordinate& from, const Coordinate& to);
    static double DegreesToRadians(double degrees);
    static double RadiansToDegrees(double radians);
    
private:
    double m_cpaLevel1;
    double m_cpaLevel2;
    double m_cpaLevel3;
    double m_cpaLevel4;
    
    double m_tcpaLevel1;
    double m_tcpaLevel2;
    double m_tcpaLevel3;
    double m_tcpaLevel4;
    
    AlertLevel DetermineAlertLevel(double cpa, double tcpa) const;
    Vector2D PositionToVector(const Coordinate& pos) const;
    Coordinate VectorToPosition(const Vector2D& vec, const Coordinate& reference) const;
};

class OGC_ALERT_API CollisionAvoidance {
public:
    struct Maneuver {
        enum Type {
            kNone,
            kAlterCourse,
            kReduceSpeed,
            kBoth
        };
        Type type;
        double course_change;
        double speed_change;
        double time_to_execute;
        double effectiveness;
    };
    
    CollisionAvoidance();
    ~CollisionAvoidance();
    
    Maneuver CalculateBestManeuver(const ShipMotion& own_ship,
                                    const ShipMotion& target_ship,
                                    const CPAResult& cpa_result);
    
    std::vector<Maneuver> GetAlternativeManeuvers(const ShipMotion& own_ship,
                                                   const ShipMotion& target_ship,
                                                   const CPAResult& cpa_result);
    
    void SetMinCPA(double min_cpa);
    void SetMinTCPA(double min_tcpa);
    
private:
    double m_minCPA;
    double m_minTCPA;
    
    Maneuver EvaluateCourseChange(const ShipMotion& own_ship,
                                   const ShipMotion& target_ship,
                                   double course_change);
    Maneuver EvaluateSpeedChange(const ShipMotion& own_ship,
                                  const ShipMotion& target_ship,
                                  double speed_change);
};

class OGC_ALERT_API MultiTargetTracker {
public:
    struct TrackedTarget {
        std::string target_id;
        ShipMotion motion;
        CPAResult cpa_result;
        DateTime last_update;
        int track_count;
    };
    
    MultiTargetTracker();
    ~MultiTargetTracker();
    
    void AddOrUpdateTarget(const std::string& target_id, const ShipMotion& motion);
    void RemoveTarget(const std::string& target_id);
    void ClearTargets();
    
    std::vector<TrackedTarget> GetDangerousTargets() const;
    std::vector<TrackedTarget> GetAllTargets() const;
    
    TrackedTarget GetMostDangerousTarget() const;
    
    void SetOwnShipMotion(const ShipMotion& motion);
    const ShipMotion& GetOwnShipMotion() const;
    
    void SetMaxTargets(size_t max_targets);
    size_t GetMaxTargets() const;
    
private:
    ShipMotion m_ownShip;
    std::vector<TrackedTarget> m_targets;
    size_t m_maxTargets;
    CPACalculator m_calculator;
    
    void SortByRisk();
};

}
}

#endif
