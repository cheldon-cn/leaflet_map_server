#ifndef OGC_ALERT_SPEED_ALERT_CHECKER_H
#define OGC_ALERT_SPEED_ALERT_CHECKER_H

#include "alert_checker.h"
#include "alert_engine.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class OGC_ALERT_API SpeedAlertChecker : public IAlertChecker {
public:
    SpeedAlertChecker();
    ~SpeedAlertChecker();
    
    std::string GetCheckerId() const override;
    AlertType GetAlertType() const override;
    int GetPriority() const override;
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    void SetThreshold(const AlertThreshold& threshold) override;
    AlertThreshold GetThreshold() const override;
    
    void SetSpeedLimitData(std::shared_ptr<void> speed_limit_data);
    
    double GetSpeedLimit(const Coordinate& position) const;
    AlertLevel DetermineSpeedLevel(double current_speed, double speed_limit) const;
    
    static std::unique_ptr<SpeedAlertChecker> Create();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
