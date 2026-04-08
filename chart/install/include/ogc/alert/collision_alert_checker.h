#ifndef OGC_ALERT_COLLISION_ALERT_CHECKER_H
#define OGC_ALERT_COLLISION_ALERT_CHECKER_H

#include "alert_checker.h"
#include "alert_engine.h"
#include "cpa_calculator.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class OGC_ALERT_API CollisionAlertChecker : public IAlertChecker {
public:
    CollisionAlertChecker();
    ~CollisionAlertChecker();
    
    std::string GetCheckerId() const override;
    AlertType GetAlertType() const override;
    int GetPriority() const override;
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    void SetThreshold(const AlertThreshold& threshold) override;
    AlertThreshold GetThreshold() const override;
    
    void SetAisData(std::shared_ptr<void> ais_data);
    
    void CalculateCPA_TCPA(const Coordinate& own_pos, double own_speed, double own_heading,
                           const Coordinate& target_pos, double target_speed, double target_heading,
                           double& cpa, double& tcpa) const;
    
    CPAResult CalculateCPAWithDetails(const Coordinate& own_pos, double own_speed, double own_heading,
                                       const Coordinate& target_pos, double target_speed, double target_heading) const;
    
    AlertLevel DetermineCollisionLevel(double cpa, double tcpa) const;
    
    static std::unique_ptr<CollisionAlertChecker> Create();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
