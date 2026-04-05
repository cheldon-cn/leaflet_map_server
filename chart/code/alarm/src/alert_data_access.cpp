#include "alert/alert_data_access.h"
#include "alert/alert_exception.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace alert {

AlertDataAccess::AlertDataAccess()
    : m_connected(true)
    , m_storagePath("./alert_data") {}

AlertDataAccess::~AlertDataAccess() {}

bool AlertDataAccess::SaveAlert(const Alert& alert) {
    std::string alertId = alert.GetAlertId();
    if (alertId.empty()) {
        return false;
    }
    
    m_alerts[alertId] = alert;
    PersistToFile();
    return true;
}

bool AlertDataAccess::UpdateAlert(const Alert& alert) {
    std::string alertId = alert.GetAlertId();
    if (alertId.empty()) {
        return false;
    }
    
    auto it = m_alerts.find(alertId);
    if (it == m_alerts.end()) {
        return false;
    }
    
    m_alerts[alertId] = alert;
    PersistToFile();
    return true;
}

bool AlertDataAccess::DeleteAlert(const std::string& alertId) {
    auto it = m_alerts.find(alertId);
    if (it == m_alerts.end()) {
        return false;
    }
    
    m_alerts.erase(it);
    m_pushRecords.erase(alertId);
    m_historyRecords.erase(alertId);
    PersistToFile();
    return true;
}

Alert AlertDataAccess::GetAlertById(const std::string& alertId) {
    auto it = m_alerts.find(alertId);
    if (it != m_alerts.end()) {
        return it->second;
    }
    return Alert();
}

std::vector<Alert> AlertDataAccess::QueryAlerts(const AlertQueryCriteria& criteria) {
    std::vector<Alert> results;
    
    for (const auto& pair : m_alerts) {
        const Alert& alert = pair.second;
        
        if (criteria.alertType != AlertType::kOtherAlert && 
            alert.GetAlertType() != criteria.alertType) {
            continue;
        }
        
        if (static_cast<int>(alert.GetAlertLevel()) < static_cast<int>(criteria.minLevel)) {
            continue;
        }
        
        if (criteria.status != AlertStatus::kActive && 
            alert.GetStatus() != criteria.status) {
            continue;
        }
        
        results.push_back(alert);
    }
    
    if (static_cast<size_t>(criteria.offset) < results.size()) {
        if (criteria.limit > 0 && results.size() > static_cast<size_t>(criteria.offset + criteria.limit)) {
            results.resize(criteria.offset + criteria.limit);
        }
    }
    
    return results;
}

bool AlertDataAccess::SavePushRecord(const DataAccessPushRecord& record) {
    if (record.alertId.empty()) {
        return false;
    }
    
    m_pushRecords[record.alertId].push_back(record);
    PersistToFile();
    return true;
}

std::vector<DataAccessPushRecord> AlertDataAccess::GetPushRecords(const std::string& alertId) {
    auto it = m_pushRecords.find(alertId);
    if (it != m_pushRecords.end()) {
        return it->second;
    }
    return std::vector<DataAccessPushRecord>();
}

bool AlertDataAccess::SaveHistoryRecord(const AlertHistoryRecord& record) {
    if (record.alertId.empty()) {
        return false;
    }
    
    m_historyRecords[record.alertId].push_back(record);
    PersistToFile();
    return true;
}

std::vector<AlertHistoryRecord> AlertDataAccess::GetHistoryRecords(const std::string& alertId) {
    auto it = m_historyRecords.find(alertId);
    if (it != m_historyRecords.end()) {
        return it->second;
    }
    return std::vector<AlertHistoryRecord>();
}

AlertStatistics AlertDataAccess::GetStatistics(const std::string& startTime, const std::string& endTime) {
    AlertStatistics stats;
    
    for (const auto& pair : m_alerts) {
        const Alert& alert = pair.second;
        
        stats.totalCount++;
        
        if (alert.GetStatus() == AlertStatus::kActive) {
            stats.activeCount++;
        } else if (alert.GetStatus() == AlertStatus::kAcknowledged) {
            stats.acknowledgedCount++;
        } else if (alert.GetStatus() == AlertStatus::kExpired) {
            stats.expiredCount++;
        }
        
        stats.countByType[alert.GetAlertType()]++;
        stats.countByLevel[alert.GetAlertLevel()]++;
    }
    
    return stats;
}

bool AlertDataAccess::AcknowledgeAlert(const std::string& alertId, const std::string& operatorId) {
    auto it = m_alerts.find(alertId);
    if (it == m_alerts.end()) {
        return false;
    }
    
    it->second.SetStatus(AlertStatus::kAcknowledged);
    
    AlertHistoryRecord history;
    history.historyId = GenerateId("HIST");
    history.alertId = alertId;
    history.action = "acknowledge";
    history.actionTime = "";
    history.operatorId = operatorId;
    SaveHistoryRecord(history);
    
    PersistToFile();
    return true;
}

bool AlertDataAccess::ExpireAlert(const std::string& alertId) {
    auto it = m_alerts.find(alertId);
    if (it == m_alerts.end()) {
        return false;
    }
    
    it->second.SetStatus(AlertStatus::kExpired);
    
    AlertHistoryRecord history;
    history.historyId = GenerateId("HIST");
    history.alertId = alertId;
    history.action = "expire";
    history.actionTime = "";
    history.operatorId = "system";
    SaveHistoryRecord(history);
    
    PersistToFile();
    return true;
}

int AlertDataAccess::GetActiveAlertCount() {
    int count = 0;
    for (const auto& pair : m_alerts) {
        if (pair.second.GetStatus() == AlertStatus::kActive) {
            count++;
        }
    }
    return count;
}

std::vector<Alert> AlertDataAccess::GetActiveAlerts() {
    std::vector<Alert> results;
    for (const auto& pair : m_alerts) {
        if (pair.second.GetStatus() == AlertStatus::kActive) {
            results.push_back(pair.second);
        }
    }
    return results;
}

void AlertDataAccess::SetConnectionCallback(std::function<bool()> callback) {
    m_connectionCallback = callback;
}

bool AlertDataAccess::IsConnected() {
    if (m_connectionCallback) {
        m_connected = m_connectionCallback();
    }
    return m_connected;
}

void AlertDataAccess::SetStoragePath(const std::string& path) {
    m_storagePath = path;
}

std::string AlertDataAccess::GetStoragePath() const {
    return m_storagePath;
}

std::string AlertDataAccess::GenerateId(const std::string& prefix) {
    std::ostringstream oss;
    oss << prefix << "_" << std::time(nullptr) << "_" << (m_alerts.size() + 1);
    return oss.str();
}

bool AlertDataAccess::PersistToFile() {
    return true;
}

bool AlertDataAccess::LoadFromFile() {
    return true;
}

std::string AlertDataAccess::AlertToJson(const Alert& alert) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"id\":\"" << alert.GetAlertId() << "\",";
    oss << "\"type\":\"" << AlertTypeToString(alert.GetAlertType()) << "\",";
    oss << "\"level\":\"" << AlertLevelToString(alert.GetAlertLevel()) << "\"";
    oss << "}";
    return oss.str();
}

Alert AlertDataAccess::JsonToAlert(const std::string& json) const {
    return Alert();
}

}
