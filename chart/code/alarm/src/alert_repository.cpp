#include "alert/alert_repository.h"
#include "alert/alert_exception.h"

namespace alert {

AlertRepository::AlertRepository()
    : m_dataAccess(std::make_shared<AlertDataAccess>())
    , m_ownsDataAccess(true) {}

AlertRepository::AlertRepository(std::shared_ptr<IAlertDataAccess> dataAccess)
    : m_dataAccess(dataAccess)
    , m_ownsDataAccess(false) {}

AlertRepository::~AlertRepository() {}

bool AlertRepository::Create(const Alert& alert) {
    if (!m_dataAccess) {
        return false;
    }
    
    if (alert.GetAlertId().empty()) {
        return false;
    }
    
    return m_dataAccess->SaveAlert(alert);
}

bool AlertRepository::Update(const Alert& alert) {
    if (!m_dataAccess) {
        return false;
    }
    
    return m_dataAccess->UpdateAlert(alert);
}

bool AlertRepository::Remove(const std::string& alertId) {
    if (!m_dataAccess) {
        return false;
    }
    
    return m_dataAccess->DeleteAlert(alertId);
}

Alert AlertRepository::GetById(const std::string& alertId) {
    if (!m_dataAccess) {
        return Alert();
    }
    
    return m_dataAccess->GetAlertById(alertId);
}

std::vector<Alert> AlertRepository::FindAll(int limit, int offset) {
    if (!m_dataAccess) {
        return std::vector<Alert>();
    }
    
    AlertQueryCriteria criteria;
    criteria.limit = limit;
    criteria.offset = offset;
    criteria.status = AlertStatus::kActive;
    
    return m_dataAccess->QueryAlerts(criteria);
}

std::vector<Alert> AlertRepository::FindByShipId(const std::string& shipId) {
    if (!m_dataAccess) {
        return std::vector<Alert>();
    }
    
    AlertQueryCriteria criteria;
    criteria.shipId = shipId;
    
    return m_dataAccess->QueryAlerts(criteria);
}

std::vector<Alert> AlertRepository::FindByType(AlertType type) {
    if (!m_dataAccess) {
        return std::vector<Alert>();
    }
    
    AlertQueryCriteria criteria;
    criteria.alertType = type;
    
    return m_dataAccess->QueryAlerts(criteria);
}

std::vector<Alert> AlertRepository::FindByLevel(AlertLevel level) {
    if (!m_dataAccess) {
        return std::vector<Alert>();
    }
    
    AlertQueryCriteria criteria;
    criteria.minLevel = level;
    
    return m_dataAccess->QueryAlerts(criteria);
}

std::vector<Alert> AlertRepository::FindByStatus(AlertStatus status) {
    if (!m_dataAccess) {
        return std::vector<Alert>();
    }
    
    AlertQueryCriteria criteria;
    criteria.status = status;
    
    return m_dataAccess->QueryAlerts(criteria);
}

std::vector<Alert> AlertRepository::FindActive() {
    if (!m_dataAccess) {
        return std::vector<Alert>();
    }
    
    return m_dataAccess->GetActiveAlerts();
}

bool AlertRepository::Acknowledge(const std::string& alertId, const std::string& operatorId) {
    if (!m_dataAccess) {
        return false;
    }
    
    return m_dataAccess->AcknowledgeAlert(alertId, operatorId);
}

bool AlertRepository::Expire(const std::string& alertId) {
    if (!m_dataAccess) {
        return false;
    }
    
    return m_dataAccess->ExpireAlert(alertId);
}

bool AlertRepository::Dismiss(const std::string& alertId, const std::string& reason) {
    if (!m_dataAccess) {
        return false;
    }
    
    Alert alert = m_dataAccess->GetAlertById(alertId);
    if (alert.GetAlertId().empty()) {
        return false;
    }
    
    alert.SetStatus(AlertStatus::kCleared);
    alert.SetClearReason(reason);
    m_dataAccess->UpdateAlert(alert);
    
    AlertHistoryRecord record;
    record.historyId = alertId + "_dismiss";
    record.alertId = alertId;
    record.action = "dismiss";
    record.operatorId = "system";
    record.details["reason"] = reason;
    
    return m_dataAccess->SaveHistoryRecord(record);
}

AlertStatistics AlertRepository::GetStatistics(const std::string& startTime, const std::string& endTime) {
    if (!m_dataAccess) {
        return AlertStatistics();
    }
    
    return m_dataAccess->GetStatistics(startTime, endTime);
}

int AlertRepository::GetActiveCount() {
    if (!m_dataAccess) {
        return 0;
    }
    
    return m_dataAccess->GetActiveAlertCount();
}

void AlertRepository::SetDataAccess(std::shared_ptr<IAlertDataAccess> dataAccess) {
    m_dataAccess = dataAccess;
}

}
