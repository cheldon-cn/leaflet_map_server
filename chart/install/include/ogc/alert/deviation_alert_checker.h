#ifndef OGC_ALERT_DEVIATION_ALERT_CHECKER_H
#define OGC_ALERT_DEVIATION_ALERT_CHECKER_H

#include "alert_checker.h"
#include "alert_engine.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class OGC_ALERT_API DeviationAlertChecker : public IAlertChecker {
public:
    DeviationAlertChecker();
    ~DeviationAlertChecker();
    
    std::string GetCheckerId() const override;
    AlertType GetAlertType() const override;
    int GetPriority() const override;
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    void SetThreshold(const AlertThreshold& threshold) override;
    AlertThreshold GetThreshold() const override;
    
    void SetRouteData(std::shared_ptr<void> route_data);
    
    double CalculateDeviation(const Coordinate& position, const std::vector<Coordinate>& waypoints) const;
    Coordinate FindNearestWaypoint(const Coordinate& position, const std::vector<Coordinate>& waypoints) const;
    
    AlertLevel DetermineDeviationLevel(double deviation) const;
    
    static std::unique_ptr<DeviationAlertChecker> Create();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
