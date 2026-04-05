#ifndef OGC_ALERT_DEPTH_ALERT_CHECKER_H
#define OGC_ALERT_DEPTH_ALERT_CHECKER_H

#include "alert_checker.h"
#include "alert_engine.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class OGC_ALERT_API DepthAlertChecker : public IAlertChecker {
public:
    DepthAlertChecker();
    ~DepthAlertChecker();
    
    std::string GetCheckerId() const override;
    AlertType GetAlertType() const override;
    int GetPriority() const override;
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    void SetThreshold(const AlertThreshold& threshold) override;
    AlertThreshold GetThreshold() const override;
    
    void SetDepthData(std::shared_ptr<void> depth_data);
    void SetTideData(std::shared_ptr<void> tide_data);
    void SetShipDraft(double draft);
    
    double CalculateUKC(double depth, double draft, double tide) const;
    AlertLevel DetermineUKCLevel(double ukc) const;
    
    static std::unique_ptr<DepthAlertChecker> Create();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
