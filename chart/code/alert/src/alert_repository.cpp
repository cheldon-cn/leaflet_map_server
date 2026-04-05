#include "ogc/alert/alert_repository.h"
#include "ogc/draw/log.h"
#include <map>
#include <mutex>
#include <vector>

namespace ogc {
namespace alert {

class AlertRepository::Impl {
public:
    Impl() {}
    
    bool Save(const AlertPtr& alert) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_alerts[alert->alert_id] = alert;
        return true;
    }
    
    bool Update(const AlertPtr& alert) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_alerts[alert->alert_id] = alert;
        return true;
    }
    
    bool Delete(const std::string& alert_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_alerts.erase(alert_id) > 0;
    }
    
    AlertPtr FindById(const std::string& alert_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_alerts.find(alert_id);
        if (it != m_alerts.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<AlertPtr> Query(const AlertQueryParams& params) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertPtr> result;
        for (const auto& pair : m_alerts) {
            const AlertPtr& alert = pair.second;
            bool match = true;
            
            if (!params.user_id.empty() && alert->user_id != params.user_id) {
                match = false;
            }
            
            if (!params.alert_types.empty()) {
                bool typeMatch = false;
                for (AlertType type : params.alert_types) {
                    if (alert->alert_type == type) {
                        typeMatch = true;
                        break;
                    }
                }
                if (!typeMatch) match = false;
            }
            
            if (!params.alert_levels.empty()) {
                bool levelMatch = false;
                for (AlertLevel level : params.alert_levels) {
                    if (alert->alert_level == level) {
                        levelMatch = true;
                        break;
                    }
                }
                if (!levelMatch) match = false;
            }
            
            if (match) {
                result.push_back(alert);
            }
        }
        return result;
    }
    
    int Count(const AlertQueryParams& params) {
        return static_cast<int>(Query(params).size());
    }
    
    std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertPtr> result;
        for (const auto& pair : m_alerts) {
            const AlertPtr& alert = pair.second;
            if (alert->status == AlertStatus::kActive) {
                if (user_id.empty() || alert->user_id == user_id) {
                    result.push_back(alert);
                }
            }
        }
        return result;
    }
    
    std::vector<AlertPtr> GetAlertsByType(AlertType type, const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertPtr> result;
        for (const auto& pair : m_alerts) {
            const AlertPtr& alert = pair.second;
            if (alert->alert_type == type) {
                if (user_id.empty() || alert->user_id == user_id) {
                    result.push_back(alert);
                }
            }
        }
        return result;
    }
    
    std::vector<AlertPtr> GetAlertsByLevel(AlertLevel level, const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertPtr> result;
        for (const auto& pair : m_alerts) {
            const AlertPtr& alert = pair.second;
            if (alert->alert_level == level) {
                if (user_id.empty() || alert->user_id == user_id) {
                    result.push_back(alert);
                }
            }
        }
        return result;
    }
    
    bool UpdateStatus(const std::string& alert_id, AlertStatus status) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_alerts.find(alert_id);
        if (it != m_alerts.end()) {
            it->second->status = status;
            return true;
        }
        return false;
    }
    
    bool Acknowledge(const std::string& alert_id, const std::string& user_id, const std::string& action) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_alerts.find(alert_id);
        if (it != m_alerts.end()) {
            it->second->status = AlertStatus::kAcknowledged;
            AcknowledgeData data;
            data.alert_id = alert_id;
            data.user_id = user_id;
            data.acknowledge_time = DateTime::Now();
            data.user_action = action;
            m_acknowledges.push_back(data);
            return true;
        }
        return false;
    }
    
    bool SaveAcknowledge(const AcknowledgeData& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_acknowledges.push_back(data);
        return true;
    }
    
    bool SaveFeedback(const FeedbackData& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_feedbacks.push_back(data);
        return true;
    }
    
private:
    std::map<std::string, AlertPtr> m_alerts;
    std::vector<AcknowledgeData> m_acknowledges;
    std::vector<FeedbackData> m_feedbacks;
    mutable std::mutex m_mutex;
};

AlertRepository::AlertRepository() 
    : m_impl(std::make_unique<Impl>()) {
}

AlertRepository::AlertRepository(const std::string& db_path) 
    : m_impl(std::make_unique<Impl>()) {
}

AlertRepository::~AlertRepository() {
}

bool AlertRepository::Save(const AlertPtr& alert) {
    return m_impl->Save(alert);
}

bool AlertRepository::Update(const AlertPtr& alert) {
    return m_impl->Update(alert);
}

bool AlertRepository::Delete(const std::string& alert_id) {
    return m_impl->Delete(alert_id);
}

AlertPtr AlertRepository::FindById(const std::string& alert_id) {
    return m_impl->FindById(alert_id);
}

std::vector<AlertPtr> AlertRepository::Query(const AlertQueryParams& params) {
    return m_impl->Query(params);
}

int AlertRepository::Count(const AlertQueryParams& params) {
    return m_impl->Count(params);
}

std::vector<AlertPtr> AlertRepository::GetActiveAlerts(const std::string& user_id) {
    return m_impl->GetActiveAlerts(user_id);
}

std::vector<AlertPtr> AlertRepository::GetAlertsByType(AlertType type, const std::string& user_id) {
    return m_impl->GetAlertsByType(type, user_id);
}

std::vector<AlertPtr> AlertRepository::GetAlertsByLevel(AlertLevel level, const std::string& user_id) {
    return m_impl->GetAlertsByLevel(level, user_id);
}

bool AlertRepository::UpdateStatus(const std::string& alert_id, AlertStatus status) {
    return m_impl->UpdateStatus(alert_id, status);
}

bool AlertRepository::Acknowledge(const std::string& alert_id, const std::string& user_id, const std::string& action) {
    return m_impl->Acknowledge(alert_id, user_id, action);
}

bool AlertRepository::SaveAcknowledge(const AcknowledgeData& data) {
    return m_impl->SaveAcknowledge(data);
}

bool AlertRepository::SaveFeedback(const FeedbackData& data) {
    return m_impl->SaveFeedback(data);
}

std::unique_ptr<IAlertRepository> IAlertRepository::Create() {
    return std::unique_ptr<IAlertRepository>(new AlertRepository());
}

}
}
