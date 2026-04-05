#ifndef ALERT_DATA_ACCESS_H
#define ALERT_DATA_ACCESS_H

#include "alert/alert.h"
#include "alert/alert_types.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace alert {

struct AlertQueryCriteria {
    std::string shipId;
    AlertType alertType;
    AlertLevel minLevel;
    AlertStatus status;
    std::string startTime;
    std::string endTime;
    int limit;
    int offset;
    
    AlertQueryCriteria()
        : alertType(AlertType::kOtherAlert)
        , minLevel(AlertLevel::kNone)
        , status(AlertStatus::kActive)
        , limit(100)
        , offset(0) {}
};

struct AlertStatistics {
    int totalCount;
    int activeCount;
    int acknowledgedCount;
    int expiredCount;
    std::map<AlertType, int> countByType;
    std::map<AlertLevel, int> countByLevel;
    
    AlertStatistics()
        : totalCount(0)
        , activeCount(0)
        , acknowledgedCount(0)
        , expiredCount(0) {}
};

struct DataAccessPushRecord {
    std::string pushId;
    std::string alertId;
    std::string userId;
    std::string pushMethod;
    std::string pushTime;
    std::string pushStatus;
    std::string errorMessage;
};

struct AlertHistoryRecord {
    std::string historyId;
    std::string alertId;
    std::string action;
    std::string actionTime;
    std::string operatorId;
    std::map<std::string, std::string> details;
};

class IAlertDataAccess {
public:
    virtual ~IAlertDataAccess() {}
    
    virtual bool SaveAlert(const Alert& alert) = 0;
    virtual bool UpdateAlert(const Alert& alert) = 0;
    virtual bool DeleteAlert(const std::string& alertId) = 0;
    virtual Alert GetAlertById(const std::string& alertId) = 0;
    virtual std::vector<Alert> QueryAlerts(const AlertQueryCriteria& criteria) = 0;
    
    virtual bool SavePushRecord(const DataAccessPushRecord& record) = 0;
    virtual std::vector<DataAccessPushRecord> GetPushRecords(const std::string& alertId) = 0;
    
    virtual bool SaveHistoryRecord(const AlertHistoryRecord& record) = 0;
    virtual std::vector<AlertHistoryRecord> GetHistoryRecords(const std::string& alertId) = 0;
    
    virtual AlertStatistics GetStatistics(const std::string& startTime, const std::string& endTime) = 0;
    
    virtual bool AcknowledgeAlert(const std::string& alertId, const std::string& operatorId) = 0;
    virtual bool ExpireAlert(const std::string& alertId) = 0;
    
    virtual int GetActiveAlertCount() = 0;
    virtual std::vector<Alert> GetActiveAlerts() = 0;
    
    virtual void SetConnectionCallback(std::function<bool()> callback) = 0;
    virtual bool IsConnected() = 0;
};

class AlertDataAccess : public IAlertDataAccess {
public:
    AlertDataAccess();
    virtual ~AlertDataAccess();
    
    bool SaveAlert(const Alert& alert) override;
    bool UpdateAlert(const Alert& alert) override;
    bool DeleteAlert(const std::string& alertId) override;
    Alert GetAlertById(const std::string& alertId) override;
    std::vector<Alert> QueryAlerts(const AlertQueryCriteria& criteria) override;
    
    bool SavePushRecord(const DataAccessPushRecord& record) override;
    std::vector<DataAccessPushRecord> GetPushRecords(const std::string& alertId) override;
    
    bool SaveHistoryRecord(const AlertHistoryRecord& record) override;
    std::vector<AlertHistoryRecord> GetHistoryRecords(const std::string& alertId) override;
    
    AlertStatistics GetStatistics(const std::string& startTime, const std::string& endTime) override;
    
    bool AcknowledgeAlert(const std::string& alertId, const std::string& operatorId) override;
    bool ExpireAlert(const std::string& alertId) override;
    
    int GetActiveAlertCount() override;
    std::vector<Alert> GetActiveAlerts() override;
    
    void SetConnectionCallback(std::function<bool()> callback) override;
    bool IsConnected() override;
    
    void SetStoragePath(const std::string& path);
    std::string GetStoragePath() const;
    
private:
    std::map<std::string, Alert> m_alerts;
    std::map<std::string, std::vector<DataAccessPushRecord>> m_pushRecords;
    std::map<std::string, std::vector<AlertHistoryRecord>> m_historyRecords;
    std::function<bool()> m_connectionCallback;
    std::string m_storagePath;
    bool m_connected;
    
    std::string GenerateId(const std::string& prefix);
    bool PersistToFile();
    bool LoadFromFile();
    std::string AlertToJson(const Alert& alert) const;
    Alert JsonToAlert(const std::string& json) const;
};

}

#endif
