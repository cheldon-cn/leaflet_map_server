#include "alert/repository_adapter.h"
#include "alert/alert.h"
#include <map>

namespace alarm {

RepositoryAdapter::RepositoryAdapter()
    : m_alarmRepo(nullptr) {
}

RepositoryAdapter::RepositoryAdapter(std::shared_ptr<alert::IAlertRepository> alarmRepo)
    : m_alarmRepo(alarmRepo) {
}

RepositoryAdapter::~RepositoryAdapter() {
}

void RepositoryAdapter::SetAlarmRepository(std::shared_ptr<alert::IAlertRepository> repo) {
    m_alarmRepo = repo;
}

std::shared_ptr<alert::IAlertRepository> RepositoryAdapter::GetAlarmRepository() const {
    return m_alarmRepo;
}

bool RepositoryAdapter::Save(const ogc::alert::AlertPtr& alert) {
    if (!m_alarmRepo || !alert) {
        return false;
    }
    
    alert::Alert alarmAlert = ConvertToAlarmAlert(alert);
    return m_alarmRepo->Create(alarmAlert);
}

bool RepositoryAdapter::Update(const ogc::alert::AlertPtr& alert) {
    if (!m_alarmRepo || !alert) {
        return false;
    }
    
    alert::Alert alarmAlert = ConvertToAlarmAlert(alert);
    return m_alarmRepo->Update(alarmAlert);
}

bool RepositoryAdapter::Delete(const std::string& alert_id) {
    if (!m_alarmRepo) {
        return false;
    }
    
    return m_alarmRepo->Remove(alert_id);
}

ogc::alert::AlertPtr RepositoryAdapter::FindById(const std::string& alert_id) {
    if (!m_alarmRepo) {
        return nullptr;
    }
    
    alert::Alert alarmAlert = m_alarmRepo->GetById(alert_id);
    return ConvertToAlertPtr(alarmAlert);
}

std::vector<ogc::alert::AlertPtr> RepositoryAdapter::Query(const ogc::alert::AlertQueryParams& params) {
    if (!m_alarmRepo) {
        return {};
    }
    
    std::vector<ogc::alert::AlertPtr> results;
    
    if (!params.alert_types.empty()) {
        alert::AlertType alarmType = AlertTypeAdapter::FromAlertType(params.alert_types[0]);
        std::vector<alert::Alert> alerts = m_alarmRepo->FindByType(alarmType);
        results = ConvertToAlertPtrList(alerts);
    } else if (!params.alert_levels.empty()) {
        alert::AlertLevel alarmLevel = AlertTypeAdapter::FromAlertLevel(params.alert_levels[0]);
        std::vector<alert::Alert> alerts = m_alarmRepo->FindByLevel(alarmLevel);
        results = ConvertToAlertPtrList(alerts);
    } else {
        int limit = params.page_size > 0 ? params.page_size : 100;
        int offset = params.page > 0 ? (params.page - 1) * params.page_size : 0;
        std::vector<alert::Alert> alerts = m_alarmRepo->FindAll(limit, offset);
        results = ConvertToAlertPtrList(alerts);
    }
    
    return results;
}

int RepositoryAdapter::Count(const ogc::alert::AlertQueryParams& params) {
    if (!m_alarmRepo) {
        return 0;
    }
    
    return m_alarmRepo->GetActiveCount();
}

std::vector<ogc::alert::AlertPtr> RepositoryAdapter::GetActiveAlerts(const std::string& user_id) {
    if (!m_alarmRepo) {
        return {};
    }
    
    std::vector<alert::Alert> alerts = m_alarmRepo->FindActive();
    return ConvertToAlertPtrList(alerts);
}

std::vector<ogc::alert::AlertPtr> RepositoryAdapter::GetAlertsByType(ogc::alert::AlertType type, const std::string& user_id) {
    if (!m_alarmRepo) {
        return {};
    }
    
    alert::AlertType alarmType = AlertTypeAdapter::FromAlertType(type);
    std::vector<alert::Alert> alerts = m_alarmRepo->FindByType(alarmType);
    return ConvertToAlertPtrList(alerts);
}

std::vector<ogc::alert::AlertPtr> RepositoryAdapter::GetAlertsByLevel(ogc::alert::AlertLevel level, const std::string& user_id) {
    if (!m_alarmRepo) {
        return {};
    }
    
    alert::AlertLevel alarmLevel = AlertTypeAdapter::FromAlertLevel(level);
    std::vector<alert::Alert> alerts = m_alarmRepo->FindByLevel(alarmLevel);
    return ConvertToAlertPtrList(alerts);
}

bool RepositoryAdapter::UpdateStatus(const std::string& alert_id, ogc::alert::AlertStatus status) {
    if (!m_alarmRepo) {
        return false;
    }
    
    alert::Alert alert = m_alarmRepo->GetById(alert_id);
    if (alert.GetAlertId().empty()) {
        return false;
    }
    
    alert::AlertStatus alarmStatus = AlertTypeAdapter::FromAlertStatus(status);
    alert.SetStatus(alarmStatus);
    
    return m_alarmRepo->Update(alert);
}

bool RepositoryAdapter::Acknowledge(const std::string& alert_id, const std::string& user_id, const std::string& action) {
    if (!m_alarmRepo) {
        return false;
    }
    
    return m_alarmRepo->Acknowledge(alert_id, user_id);
}

bool RepositoryAdapter::SaveAcknowledge(const ogc::alert::AcknowledgeData& data) {
    return false;
}

bool RepositoryAdapter::SaveFeedback(const ogc::alert::FeedbackData& data) {
    return false;
}

alert::Alert RepositoryAdapter::ConvertToAlarmAlert(const ogc::alert::AlertPtr& alert) {
    alert::Alert result;
    
    result.SetAlertId(alert->alert_id);
    result.SetAlertType(AlertTypeAdapter::FromAlertType(alert->alert_type));
    result.SetAlertLevel(AlertTypeAdapter::FromAlertLevel(alert->alert_level));
    result.SetStatus(AlertTypeAdapter::FromAlertStatus(alert->status));
    result.SetTitle(alert->content.title);
    result.SetMessage(alert->content.message);
    
    alert::AlertPosition pos(alert->position.longitude, alert->position.latitude);
    result.SetPosition(pos);
    
    result.SetIssueTime(alert->issue_time.ToString());
    
    return result;
}

ogc::alert::AlertPtr RepositoryAdapter::ConvertToAlertPtr(const alert::Alert& alert) {
    ogc::alert::AlertPtr result = std::make_shared<ogc::alert::Alert>();
    
    result->alert_id = alert.GetAlertId();
    result->alert_type = AlertTypeAdapter::ToAlertType(alert.GetAlertType());
    result->alert_level = AlertTypeAdapter::ToAlertLevel(alert.GetAlertLevel());
    result->status = AlertTypeAdapter::ToAlertStatus(alert.GetStatus());
    result->content.title = alert.GetTitle();
    result->content.message = alert.GetMessage();
    
    ogc::alert::Coordinate coord;
    coord.longitude = alert.GetPosition().longitude;
    coord.latitude = alert.GetPosition().latitude;
    result->position = coord;
    
    result->issue_time = ogc::alert::DateTime::FromString(alert.GetIssueTime());
    
    return result;
}

std::vector<ogc::alert::AlertPtr> RepositoryAdapter::ConvertToAlertPtrList(const std::vector<alert::Alert>& alerts) {
    std::vector<ogc::alert::AlertPtr> results;
    results.reserve(alerts.size());
    
    for (const auto& alert : alerts) {
        results.push_back(ConvertToAlertPtr(alert));
    }
    
    return results;
}

ConfigRepositoryAdapter::ConfigRepositoryAdapter() {
    m_defaultConfig.user_id = "default";
    m_defaultConfig.depth_threshold.level1_threshold = 0.5;
    m_defaultConfig.depth_threshold.level2_threshold = 1.0;
    m_defaultConfig.depth_threshold.level3_threshold = 2.0;
    m_defaultConfig.depth_threshold.level4_threshold = 3.0;
    m_defaultConfig.collision_threshold.level1_threshold = 0.5;
    m_defaultConfig.collision_threshold.level2_threshold = 1.0;
    m_defaultConfig.collision_threshold.level3_threshold = 2.0;
    m_defaultConfig.collision_threshold.level4_threshold = 3.0;
}

ConfigRepositoryAdapter::~ConfigRepositoryAdapter() {
}

void ConfigRepositoryAdapter::SetDefaultConfig(const ogc::alert::AlertConfig& config) {
    m_defaultConfig = config;
}

bool ConfigRepositoryAdapter::SaveConfig(const ogc::alert::AlertConfig& config) {
    m_configs[config.user_id] = config;
    return true;
}

ogc::alert::AlertConfig ConfigRepositoryAdapter::LoadConfig(const std::string& user_id) {
    auto it = m_configs.find(user_id);
    if (it != m_configs.end()) {
        return it->second;
    }
    
    ogc::alert::AlertConfig config = m_defaultConfig;
    config.user_id = user_id;
    return config;
}

bool ConfigRepositoryAdapter::DeleteConfig(const std::string& user_id) {
    auto it = m_configs.find(user_id);
    if (it != m_configs.end()) {
        m_configs.erase(it);
        return true;
    }
    return false;
}

bool ConfigRepositoryAdapter::UpdateThreshold(const std::string& user_id, ogc::alert::AlertType type, const ogc::alert::AlertThreshold& threshold) {
    ogc::alert::AlertConfig config = LoadConfig(user_id);
    
    switch (type) {
        case ogc::alert::AlertType::kDepth:
            config.depth_threshold.level1_threshold = threshold.level1_threshold;
            config.depth_threshold.level2_threshold = threshold.level2_threshold;
            config.depth_threshold.level3_threshold = threshold.level3_threshold;
            config.depth_threshold.level4_threshold = threshold.level4_threshold;
            break;
        case ogc::alert::AlertType::kCollision:
            config.collision_threshold.level1_threshold = threshold.level1_threshold;
            config.collision_threshold.level2_threshold = threshold.level2_threshold;
            config.collision_threshold.level3_threshold = threshold.level3_threshold;
            config.collision_threshold.level4_threshold = threshold.level4_threshold;
            break;
        default:
            break;
    }
    
    m_configs[user_id] = config;
    return true;
}

ogc::alert::AlertThreshold ConfigRepositoryAdapter::GetThreshold(const std::string& user_id, ogc::alert::AlertType type) {
    ogc::alert::AlertConfig config = LoadConfig(user_id);
    
    switch (type) {
        case ogc::alert::AlertType::kDepth:
            return config.depth_threshold;
        case ogc::alert::AlertType::kCollision:
            return config.collision_threshold;
        default:
            return ogc::alert::AlertThreshold();
    }
}

bool ConfigRepositoryAdapter::SetPushMethods(const std::string& user_id, const std::vector<ogc::alert::PushMethod>& methods) {
    ogc::alert::AlertConfig config = LoadConfig(user_id);
    config.push_methods = methods;
    m_configs[user_id] = config;
    return true;
}

std::vector<ogc::alert::PushMethod> ConfigRepositoryAdapter::GetPushMethods(const std::string& user_id) {
    ogc::alert::AlertConfig config = LoadConfig(user_id);
    return config.push_methods;
}

ogc::alert::IAlertRepository* CreateRepositoryAdapter(std::shared_ptr<alert::IAlertRepository> alarmRepo) {
    return new RepositoryAdapter(alarmRepo);
}

ogc::alert::IConfigRepository* CreateConfigRepositoryAdapter() {
    return new ConfigRepositoryAdapter();
}

}
