#ifndef ALERT_SYSTEM_COLLISION_ALERT_ENGINE_H
#define ALERT_SYSTEM_COLLISION_ALERT_ENGINE_H

#include "i_alert_engine.h"
#include "coordinate.h"
#include <vector>
#include <string>

namespace alert {

struct AISTarget {
    std::string targetId;
    std::string targetName;
    Coordinate position;
    double speed;
    double course;
    
    AISTarget()
        : speed(0), course(0) {}
};

struct CollisionTarget {
    std::string targetId;
    std::string targetName;
    Coordinate position;
    double speed;
    double course;
    double cpa;
    double tcpa;
    AlertLevel riskLevel;
    
    CollisionTarget()
        : speed(0), course(0), cpa(0), tcpa(0), riskLevel(AlertLevel::kNone) {}
};

class CollisionAlertEngine : public IAlertEngine {
public:
    CollisionAlertEngine();
    virtual ~CollisionAlertEngine();
    
    Alert Evaluate(const AlertContext& context) override;
    
    AlertType GetType() const override;
    
    void SetThreshold(const ThresholdConfig& config) override;
    ThresholdConfig GetThreshold() const override;
    
    std::string GetName() const override;
    
    void SetAISProvider(std::vector<AISTarget> (*provider)(const Coordinate&, double));
    
    void CalculateCPA_TCPA(
        const Coordinate& myPos, double mySpeed, double myCourse,
        const Coordinate& targetPos, double targetSpeed, double targetCourse,
        double& cpa, double& tcpa);
    
    AlertLevel AssessRisk(double cpa, double tcpa);
    
    std::vector<CollisionTarget> GetRiskTargets(const AlertContext& context);
    
private:
    ThresholdConfig m_threshold;
    std::vector<AISTarget> (*m_aisProvider)(const Coordinate&, double);
};

}

#endif
