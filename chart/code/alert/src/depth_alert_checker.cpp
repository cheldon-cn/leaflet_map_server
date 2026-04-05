#include "ogc/alert/depth_alert_checker.h"
#include "ogc/draw/log.h"
#include <cmath>

namespace ogc {
namespace alert {

class DepthAlertChecker::Impl {
public:
    Impl() : m_enabled(true), m_priority(1), m_shipDraft(0.0) {
        m_threshold.level1_threshold = 2.0;
        m_threshold.level2_threshold = 1.5;
        m_threshold.level3_threshold = 1.0;
        m_threshold.level4_threshold = 0.5;
        m_threshold.ukc_safety_margin = 0.5;
        m_threshold.use_dynamic_ukc = true;
    }
    
    std::string GetCheckerId() const { return "depth_alert_checker"; }
    AlertType GetAlertType() const { return AlertType::kDepth; }
    int GetPriority() const { return m_priority; }
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) {
        m_threshold = static_cast<const DepthAlertThreshold&>(threshold);
    }
    
    DepthAlertThreshold GetThreshold() const { return m_threshold; }
    
    void SetDepthData(std::shared_ptr<void> depth_data) {
        m_depthData = depth_data;
    }
    
    void SetTideData(std::shared_ptr<void> tide_data) {
        m_tideData = tide_data;
    }
    
    void SetShipDraft(double draft) {
        m_shipDraft = draft;
    }
    
    double CalculateUKC(double depth, double draft, double tide) const {
        return depth + tide - draft;
    }
    
    AlertLevel DetermineUKCLevel(double ukc) const {
        if (ukc <= m_threshold.level4_threshold) return AlertLevel::kLevel4;
        if (ukc <= m_threshold.level3_threshold) return AlertLevel::kLevel3;
        if (ukc <= m_threshold.level2_threshold) return AlertLevel::kLevel2;
        if (ukc <= m_threshold.level1_threshold) return AlertLevel::kLevel1;
        return AlertLevel::kNone;
    }
    
    std::vector<AlertPtr> Check(const CheckContext& context) {
        std::vector<AlertPtr> alerts;
        
        double depth = 10.0;
        double tide = 0.0;
        double ukc = CalculateUKC(depth, m_shipDraft, tide);
        
        AlertLevel level = DetermineUKCLevel(ukc);
        if (level != AlertLevel::kNone) {
            auto alert = std::make_shared<DepthAlert>();
            alert->alert_id = "DEPTH_" + std::to_string(std::time(nullptr));
            alert->alert_type = AlertType::kDepth;
            alert->alert_level = level;
            alert->status = AlertStatus::kActive;
            alert->issue_time = DateTime::Now();
            alert->position = context.ship_position;
            alert->user_id = context.user_id;
            alert->acknowledge_required = true;
            alert->current_depth = depth;
            alert->ukc = ukc;
            alert->safety_depth = m_shipDraft + m_threshold.ukc_safety_margin;
            alert->tide_height = tide;
            
            alert->content.type = "Depth";
            alert->content.level = static_cast<int>(level);
            alert->content.title = "Depth Alert";
            alert->content.message = "UKC is " + std::to_string(ukc) + " meters";
            alert->content.position = context.ship_position;
            alert->content.action_required = "Reduce speed or change course";
            
            alerts.push_back(alert);
        }
        
        return alerts;
    }
    
private:
    bool m_enabled;
    int m_priority;
    DepthAlertThreshold m_threshold;
    std::shared_ptr<void> m_depthData;
    std::shared_ptr<void> m_tideData;
    double m_shipDraft;
};

DepthAlertChecker::DepthAlertChecker() 
    : m_impl(std::make_unique<Impl>()) {
}

DepthAlertChecker::~DepthAlertChecker() {
}

std::string DepthAlertChecker::GetCheckerId() const {
    return m_impl->GetCheckerId();
}

AlertType DepthAlertChecker::GetAlertType() const {
    return m_impl->GetAlertType();
}

int DepthAlertChecker::GetPriority() const {
    return m_impl->GetPriority();
}

bool DepthAlertChecker::IsEnabled() const {
    return m_impl->IsEnabled();
}

void DepthAlertChecker::SetEnabled(bool enabled) {
    m_impl->SetEnabled(enabled);
}

std::vector<AlertPtr> DepthAlertChecker::Check(const CheckContext& context) {
    return m_impl->Check(context);
}

void DepthAlertChecker::SetThreshold(const AlertThreshold& threshold) {
    m_impl->SetThreshold(threshold);
}

AlertThreshold DepthAlertChecker::GetThreshold() const {
    return m_impl->GetThreshold();
}

void DepthAlertChecker::SetDepthData(std::shared_ptr<void> depth_data) {
    m_impl->SetDepthData(depth_data);
}

void DepthAlertChecker::SetTideData(std::shared_ptr<void> tide_data) {
    m_impl->SetTideData(tide_data);
}

void DepthAlertChecker::SetShipDraft(double draft) {
    m_impl->SetShipDraft(draft);
}

double DepthAlertChecker::CalculateUKC(double depth, double draft, double tide) const {
    return m_impl->CalculateUKC(depth, draft, tide);
}

AlertLevel DepthAlertChecker::DetermineUKCLevel(double ukc) const {
    return m_impl->DetermineUKCLevel(ukc);
}

std::unique_ptr<DepthAlertChecker> DepthAlertChecker::Create() {
    return std::unique_ptr<DepthAlertChecker>(new DepthAlertChecker());
}

}
}
