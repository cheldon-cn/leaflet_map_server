#include "ogc/alert/channel_alert_checker.h"
#include "ogc/base/log.h"

namespace ogc {
namespace alert {

class ChannelAlertChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(4) {
    }
    
    std::string GetCheckerId() const { return "channel_alert_checker"; }
    AlertType GetAlertType() const { return AlertType::kChannel; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = threshold;
    }
    
    AlertThreshold GetThreshold() const { return m_threshold; }
    
    void SetNoticeData(std::shared_ptr<void> notice_data) {
        m_noticeData = notice_data;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        return alerts;
    }
    
private:
    bool m_enabled;
    int m_priority;
    AlertThreshold m_threshold;
    std::shared_ptr<void> m_noticeData;
};

ChannelAlertChecker::ChannelAlertChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

ChannelAlertChecker::~ChannelAlertChecker() {
}

std::string ChannelAlertChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType ChannelAlertChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int ChannelAlertChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool ChannelAlertChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void ChannelAlertChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> ChannelAlertChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void ChannelAlertChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold ChannelAlertChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void ChannelAlertChecker::SetNoticeData(std::shared_ptr<void> notice_data) {
    m_impl->SetNoticeData(notice_data);
}

std::unique_ptr<ChannelAlertChecker> ChannelAlertChecker::Create() {
    return std::unique_ptr<ChannelAlertChecker>(new ChannelAlertChecker());
}

}
}
