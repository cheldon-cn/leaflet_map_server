#ifndef ALERT_SYSTEM_ALERT_H
#define ALERT_SYSTEM_ALERT_H

#include "alert_types.h"
#include "coordinate.h"
#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace alert {

struct AlertPosition {
    double longitude;
    double latitude;
    std::string description;
    
    AlertPosition() : longitude(0), latitude(0) {}
    AlertPosition(double lon, double lat) : longitude(lon), latitude(lat) {}
};

struct PushRecord {
    PushMethod method;
    std::string pushTime;
    std::string pushStatus;
    std::string errorMessage;
    
    PushRecord() : method(PushMethod::kApp) {}
};

class Alert {
public:
    Alert();
    Alert(const std::string& alertId, AlertType type, AlertLevel level);
    
    const std::string& GetAlertId() const { return m_alertId; }
    void SetAlertId(const std::string& id) { m_alertId = id; }
    
    AlertType GetAlertType() const { return m_type; }
    void SetAlertType(AlertType type) { m_type = type; }
    
    AlertLevel GetAlertLevel() const { return m_level; }
    void SetAlertLevel(AlertLevel level) { m_level = level; }
    
    AlertStatus GetStatus() const { return m_status; }
    void SetStatus(AlertStatus status) { m_status = status; }
    
    const std::string& GetTitle() const { return m_title; }
    void SetTitle(const std::string& title) { m_title = title; }
    
    const std::string& GetMessage() const { return m_message; }
    void SetMessage(const std::string& message) { m_message = message; }
    
    const std::string& GetRecommendation() const { return m_recommendation; }
    void SetRecommendation(const std::string& rec) { m_recommendation = rec; }
    
    const AlertPosition& GetPosition() const { return m_position; }
    void SetPosition(const AlertPosition& pos) { m_position = pos; }
    void SetPosition(double lon, double lat);
    
    const std::string& GetIssueTime() const { return m_issueTime; }
    void SetIssueTime(const std::string& time) { m_issueTime = time; }
    
    const std::string& GetExpireTime() const { return m_expireTime; }
    void SetExpireTime(const std::string& time) { m_expireTime = time; }
    
    const std::string& GetClearTime() const { return m_clearTime; }
    void SetClearTime(const std::string& time) { m_clearTime = time; }
    
    const std::string& GetClearReason() const { return m_clearReason; }
    void SetClearReason(const std::string& reason) { m_clearReason = reason; }
    
    bool IsAcknowledged() const { return m_acknowledged; }
    void SetAcknowledged(bool ack) { m_acknowledged = ack; }
    
    const std::string& GetAcknowledgeTime() const { return m_acknowledgeTime; }
    void SetAcknowledgeTime(const std::string& time) { m_acknowledgeTime = time; }
    
    const std::string& GetActionTaken() const { return m_actionTaken; }
    void SetActionTaken(const std::string& action) { m_actionTaken = action; }
    
    void SetDetail(const std::string& key, double value);
    double GetDetail(const std::string& key, double defaultValue = 0.0) const;
    const std::map<std::string, double>& GetDetails() const { return m_details; }
    
    void AddPushRecord(const PushRecord& record);
    const std::vector<PushRecord>& GetPushRecords() const { return m_pushRecords; }
    
    bool IsActive() const;
    bool IsExpired() const;
    
    std::string ToString() const;
    
    static std::string GenerateAlertId(AlertType type);
    
private:
    std::string m_alertId;
    AlertType m_type;
    AlertLevel m_level;
    AlertStatus m_status;
    std::string m_title;
    std::string m_message;
    std::string m_recommendation;
    AlertPosition m_position;
    std::string m_issueTime;
    std::string m_expireTime;
    std::string m_clearTime;
    std::string m_clearReason;
    bool m_acknowledged;
    std::string m_acknowledgeTime;
    std::string m_actionTaken;
    std::map<std::string, double> m_details;
    std::vector<PushRecord> m_pushRecords;
};

}
#endif
