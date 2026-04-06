#include "ogc/alert/acknowledge_service.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/base/log.h"
#include <vector>

namespace ogc {
namespace alert {

class AcknowledgeService::Impl {
public:
    Impl() {}
    explicit Impl(std::shared_ptr<IAlertRepository> repository) 
        : m_repository(repository) {}
    
    bool AcknowledgeAlert(const std::string& alert_id,
                          const std::string& user_id,
                          const std::string& action) {
        if (m_repository) {
            return m_repository->Acknowledge(alert_id, user_id, action);
        }
        return false;
    }
    
    bool DismissAlert(const std::string& alert_id, const std::string& user_id) {
        if (m_repository) {
            return m_repository->UpdateStatus(alert_id, AlertStatus::kDismissed);
        }
        return false;
    }
    
    bool SubmitFeedback(const FeedbackData& feedback) {
        if (m_repository) {
            return m_repository->SaveFeedback(feedback);
        }
        return false;
    }
    
    bool SubmitFeedback(const std::string& alert_id,
                        const std::string& user_id,
                        const std::string& feedback_type,
                        const std::string& feedback_content) {
        FeedbackData feedback;
        feedback.alert_id = alert_id;
        feedback.user_id = user_id;
        feedback.feedback_type = feedback_type;
        feedback.feedback_content = feedback_content;
        feedback.feedback_time = DateTime::Now();
        return SubmitFeedback(feedback);
    }
    
    std::vector<AcknowledgeData> GetAcknowledgeHistory(const std::string& alert_id) {
        return {};
    }
    
    std::vector<FeedbackData> GetFeedbackHistory(const std::string& alert_id) {
        return {};
    }
    
    bool IsAcknowledged(const std::string& alert_id, const std::string& user_id) {
        if (m_repository) {
            auto alert = m_repository->FindById(alert_id);
            if (alert) {
                return alert->status == AlertStatus::kAcknowledged;
            }
        }
        return false;
    }
    
private:
    std::shared_ptr<IAlertRepository> m_repository;
};

AcknowledgeService::AcknowledgeService() 
    : m_impl(std::make_unique<Impl>()) {
}

AcknowledgeService::AcknowledgeService(std::shared_ptr<IAlertRepository> repository) 
    : m_impl(std::make_unique<Impl>(repository)) {
}

AcknowledgeService::~AcknowledgeService() {
}

bool AcknowledgeService::AcknowledgeAlert(const std::string& alert_id,
                                          const std::string& user_id,
                                          const std::string& action) {
    return m_impl->AcknowledgeAlert(alert_id, user_id, action);
}

bool AcknowledgeService::DismissAlert(const std::string& alert_id, const std::string& user_id) {
    return m_impl->DismissAlert(alert_id, user_id);
}

bool AcknowledgeService::SubmitFeedback(const FeedbackData& feedback) {
    return m_impl->SubmitFeedback(feedback);
}

bool AcknowledgeService::SubmitFeedback(const std::string& alert_id,
                                        const std::string& user_id,
                                        const std::string& feedback_type,
                                        const std::string& feedback_content) {
    return m_impl->SubmitFeedback(alert_id, user_id, feedback_type, feedback_content);
}

std::vector<AcknowledgeData> AcknowledgeService::GetAcknowledgeHistory(const std::string& alert_id) {
    return m_impl->GetAcknowledgeHistory(alert_id);
}

std::vector<FeedbackData> AcknowledgeService::GetFeedbackHistory(const std::string& alert_id) {
    return m_impl->GetFeedbackHistory(alert_id);
}

bool AcknowledgeService::IsAcknowledged(const std::string& alert_id, const std::string& user_id) {
    return m_impl->IsAcknowledged(alert_id, user_id);
}

std::unique_ptr<IAcknowledgeService> IAcknowledgeService::Create() {
    return std::unique_ptr<IAcknowledgeService>(new AcknowledgeService());
}

}
}
