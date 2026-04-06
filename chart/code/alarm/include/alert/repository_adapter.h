#ifndef ALARM_REPOSITORY_ADAPTER_H
#define ALARM_REPOSITORY_ADAPTER_H

#include "ogc/alert/alert_repository.h"
#include "alert/alert_repository.h"
#include "alert/alert_type_adapter.h"
#include <memory>
#include <string>
#include <vector>

namespace alarm {

class RepositoryAdapter : public ogc::alert::IAlertRepository {
public:
    RepositoryAdapter();
    explicit RepositoryAdapter(std::shared_ptr<alert::IAlertRepository> alarmRepo);
    ~RepositoryAdapter() override;
    
    void SetAlarmRepository(std::shared_ptr<alert::IAlertRepository> repo);
    std::shared_ptr<alert::IAlertRepository> GetAlarmRepository() const;
    
    bool Save(const ogc::alert::AlertPtr& alert) override;
    bool Update(const ogc::alert::AlertPtr& alert) override;
    bool Delete(const std::string& alert_id) override;
    ogc::alert::AlertPtr FindById(const std::string& alert_id) override;
    
    std::vector<ogc::alert::AlertPtr> Query(const ogc::alert::AlertQueryParams& params) override;
    int Count(const ogc::alert::AlertQueryParams& params) override;
    
    std::vector<ogc::alert::AlertPtr> GetActiveAlerts(const std::string& user_id) override;
    std::vector<ogc::alert::AlertPtr> GetAlertsByType(ogc::alert::AlertType type, const std::string& user_id) override;
    std::vector<ogc::alert::AlertPtr> GetAlertsByLevel(ogc::alert::AlertLevel level, const std::string& user_id) override;
    
    bool UpdateStatus(const std::string& alert_id, ogc::alert::AlertStatus status) override;
    bool Acknowledge(const std::string& alert_id, const std::string& user_id, const std::string& action) override;
    
    bool SaveAcknowledge(const ogc::alert::AcknowledgeData& data) override;
    bool SaveFeedback(const ogc::alert::FeedbackData& data) override;
    
private:
    alert::Alert ConvertToAlarmAlert(const ogc::alert::AlertPtr& alert);
    ogc::alert::AlertPtr ConvertToAlertPtr(const alert::Alert& alert);
    std::vector<ogc::alert::AlertPtr> ConvertToAlertPtrList(const std::vector<alert::Alert>& alerts);
    
    std::shared_ptr<alert::IAlertRepository> m_alarmRepo;
};

class ConfigRepositoryAdapter : public ogc::alert::IConfigRepository {
public:
    ConfigRepositoryAdapter();
    ~ConfigRepositoryAdapter() override;
    
    void SetDefaultConfig(const ogc::alert::AlertConfig& config);
    
    bool SaveConfig(const ogc::alert::AlertConfig& config) override;
    ogc::alert::AlertConfig LoadConfig(const std::string& user_id) override;
    bool DeleteConfig(const std::string& user_id) override;
    
    bool UpdateThreshold(const std::string& user_id, ogc::alert::AlertType type, const ogc::alert::AlertThreshold& threshold) override;
    ogc::alert::AlertThreshold GetThreshold(const std::string& user_id, ogc::alert::AlertType type) override;
    
    bool SetPushMethods(const std::string& user_id, const std::vector<ogc::alert::PushMethod>& methods) override;
    std::vector<ogc::alert::PushMethod> GetPushMethods(const std::string& user_id) override;
    
private:
    ogc::alert::AlertConfig m_defaultConfig;
    std::map<std::string, ogc::alert::AlertConfig> m_configs;
};

ogc::alert::IAlertRepository* CreateRepositoryAdapter(std::shared_ptr<alert::IAlertRepository> alarmRepo);
ogc::alert::IConfigRepository* CreateConfigRepositoryAdapter();

}

#endif
