#ifndef OGC_ALERT_CHANNEL_ALERT_CHECKER_H
#define OGC_ALERT_CHANNEL_ALERT_CHECKER_H

#include "alert_checker.h"
#include "alert_engine.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class OGC_ALERT_API ChannelAlertChecker : public IAlertChecker {
public:
    ChannelAlertChecker();
    ~ChannelAlertChecker();
    
    std::string GetCheckerId() const override;
    AlertType GetAlertType() const override;
    int GetPriority() const override;
    bool IsEnabled() const override;
    void SetEnabled(bool enabled) override;
    std::vector<AlertPtr> Check(const CheckContext& context) override;
    void SetThreshold(const AlertThreshold& threshold) override;
    AlertThreshold GetThreshold() const override;
    
    void SetNoticeData(std::shared_ptr<void> notice_data);
    
    static std::unique_ptr<ChannelAlertChecker> Create();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
