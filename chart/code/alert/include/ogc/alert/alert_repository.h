#ifndef OGC_ALERT_REPOSITORY_H
#define OGC_ALERT_REPOSITORY_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class OGC_ALERT_API IAlertRepository {
public:
    virtual ~IAlertRepository() = default;
    
    virtual bool Save(const AlertPtr& alert) = 0;
    virtual bool Update(const AlertPtr& alert) = 0;
    virtual bool Delete(const std::string& alert_id) = 0;
    virtual AlertPtr FindById(const std::string& alert_id) = 0;
    
    virtual std::vector<AlertPtr> Query(const AlertQueryParams& params) = 0;
    virtual int Count(const AlertQueryParams& params) = 0;
    
    virtual std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) = 0;
    virtual std::vector<AlertPtr> GetAlertsByType(AlertType type, const std::string& user_id) = 0;
    virtual std::vector<AlertPtr> GetAlertsByLevel(AlertLevel level, const std::string& user_id) = 0;
    
    virtual bool UpdateStatus(const std::string& alert_id, AlertStatus status) = 0;
    virtual bool Acknowledge(const std::string& alert_id, const std::string& user_id, const std::string& action) = 0;
    
    virtual bool SaveAcknowledge(const AcknowledgeData& data) = 0;
    virtual bool SaveFeedback(const FeedbackData& data) = 0;
    
    static std::unique_ptr<IAlertRepository> Create();
};

class OGC_ALERT_API IConfigRepository {
public:
    virtual ~IConfigRepository() = default;
    
    virtual bool SaveConfig(const AlertConfig& config) = 0;
    virtual AlertConfig LoadConfig(const std::string& user_id) = 0;
    virtual bool DeleteConfig(const std::string& user_id) = 0;
    
    virtual bool UpdateThreshold(const std::string& user_id, AlertType type, const AlertThreshold& threshold) = 0;
    virtual AlertThreshold GetThreshold(const std::string& user_id, AlertType type) = 0;
    
    virtual bool SetPushMethods(const std::string& user_id, const std::vector<PushMethod>& methods) = 0;
    virtual std::vector<PushMethod> GetPushMethods(const std::string& user_id) = 0;
    
    static std::unique_ptr<IConfigRepository> Create();
};

class OGC_ALERT_API AlertRepository : public IAlertRepository {
public:
    AlertRepository();
    explicit AlertRepository(const std::string& db_path);
    ~AlertRepository();
    
    bool Save(const AlertPtr& alert) override;
    bool Update(const AlertPtr& alert) override;
    bool Delete(const std::string& alert_id) override;
    AlertPtr FindById(const std::string& alert_id) override;
    
    std::vector<AlertPtr> Query(const AlertQueryParams& params) override;
    int Count(const AlertQueryParams& params) override;
    
    std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) override;
    std::vector<AlertPtr> GetAlertsByType(AlertType type, const std::string& user_id) override;
    std::vector<AlertPtr> GetAlertsByLevel(AlertLevel level, const std::string& user_id) override;
    
    bool UpdateStatus(const std::string& alert_id, AlertStatus status) override;
    bool Acknowledge(const std::string& alert_id, const std::string& user_id, const std::string& action) override;
    
    bool SaveAcknowledge(const AcknowledgeData& data) override;
    bool SaveFeedback(const FeedbackData& data) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
