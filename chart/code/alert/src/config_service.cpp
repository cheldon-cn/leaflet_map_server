#include "ogc/alert/config_service.h"
#include "ogc/base/log.h"
#include <map>
#include <mutex>

namespace ogc {
namespace alert {

class ConfigService::Impl {
public:
    AlertConfig GetConfig(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_configs.find(user_id);
        if (it != m_configs.end()) {
            return it->second;
        }
        return CreateDefaultConfig(user_id);
    }
    
    bool SetConfig(const std::string& user_id, const AlertConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_configs[user_id] = config;
        return true;
    }
    
    bool DeleteConfig(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_configs.erase(user_id) > 0;
    }
    
    bool EnableAlertType(const std::string& user_id, AlertType type, bool enabled) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& config = GetOrCreateConfig(user_id);
        switch (type) {
            case AlertType::kDepth:
                config.depth_alert_enabled = enabled;
                break;
            case AlertType::kCollision:
                config.collision_alert_enabled = enabled;
                break;
            case AlertType::kWeather:
                config.weather_alert_enabled = enabled;
                break;
            case AlertType::kChannel:
                config.channel_alert_enabled = enabled;
                break;
            case AlertType::kDeviation:
                config.deviation_alert_enabled = enabled;
                break;
            case AlertType::kSpeed:
                config.speed_alert_enabled = enabled;
                break;
            case AlertType::kRestrictedArea:
                config.restricted_area_alert_enabled = enabled;
                break;
            default:
                return false;
        }
        return true;
    }
    
    bool IsAlertTypeEnabled(const std::string& user_id, AlertType type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        const auto& config = GetOrCreateConfig(user_id);
        switch (type) {
            case AlertType::kDepth:
                return config.depth_alert_enabled;
            case AlertType::kCollision:
                return config.collision_alert_enabled;
            case AlertType::kWeather:
                return config.weather_alert_enabled;
            case AlertType::kChannel:
                return config.channel_alert_enabled;
            case AlertType::kDeviation:
                return config.deviation_alert_enabled;
            case AlertType::kSpeed:
                return config.speed_alert_enabled;
            case AlertType::kRestrictedArea:
                return config.restricted_area_alert_enabled;
            default:
                return false;
        }
    }
    
    bool SetThreshold(const std::string& user_id, AlertType type, const AlertThreshold& threshold) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& config = GetOrCreateConfig(user_id);
        switch (type) {
            case AlertType::kDepth:
                config.depth_threshold = static_cast<const DepthAlertThreshold&>(threshold);
                break;
            case AlertType::kCollision:
                config.collision_threshold = static_cast<const CollisionAlertThreshold&>(threshold);
                break;
            case AlertType::kDeviation:
                config.deviation_threshold = static_cast<const DeviationThreshold&>(threshold);
                break;
            default:
                return false;
        }
        return true;
    }
    
    AlertThreshold GetThreshold(const std::string& user_id, AlertType type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        const auto& config = GetOrCreateConfig(user_id);
        switch (type) {
            case AlertType::kDepth:
                return config.depth_threshold;
            case AlertType::kCollision:
                return config.collision_threshold;
            case AlertType::kDeviation:
                return config.deviation_threshold;
            default:
                return AlertThreshold();
        }
    }
    
    bool SetPushMethods(const std::string& user_id, const std::vector<PushMethod>& methods) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& config = GetOrCreateConfig(user_id);
        config.push_methods = methods;
        return true;
    }
    
    std::vector<PushMethod> GetPushMethods(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return GetOrCreateConfig(user_id).push_methods;
    }
    
    bool SetQuietHours(const std::string& user_id, const QuietHoursConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_quietHours[user_id] = config;
        return true;
    }
    
    QuietHoursConfig GetQuietHours(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_quietHours.find(user_id);
        if (it != m_quietHours.end()) {
            return it->second;
        }
        return QuietHoursConfig();
    }
    
    SystemDefaultConfig GetSystemDefaults() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_defaults;
    }
    
    bool SetSystemDefaults(const SystemDefaultConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_defaults = config;
        return true;
    }
    
    bool ResetToDefaults(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_configs[user_id] = CreateDefaultConfig(user_id);
        return true;
    }
    
private:
    AlertConfig& GetOrCreateConfig(const std::string& user_id) {
        auto it = m_configs.find(user_id);
        if (it == m_configs.end()) {
            m_configs[user_id] = CreateDefaultConfig(user_id);
        }
        return m_configs[user_id];
    }
    
    AlertConfig CreateDefaultConfig(const std::string& user_id) {
        AlertConfig config;
        config.user_id = user_id;
        config.depth_alert_enabled = true;
        config.collision_alert_enabled = true;
        config.weather_alert_enabled = true;
        config.channel_alert_enabled = true;
        config.deviation_alert_enabled = true;
        config.speed_alert_enabled = true;
        config.restricted_area_alert_enabled = true;
        config.push_methods = {PushMethod::kApp, PushMethod::kSound};
        return config;
    }
    
    std::map<std::string, AlertConfig> m_configs;
    std::map<std::string, QuietHoursConfig> m_quietHours;
    SystemDefaultConfig m_defaults;
    mutable std::mutex m_mutex;
};

ConfigService::ConfigService() 
    : m_impl(std::make_unique<Impl>()) {
}

ConfigService::ConfigService(const std::string& db_path) 
    : m_impl(std::make_unique<Impl>()) {
}

ConfigService::~ConfigService() {
}

AlertConfig ConfigService::GetConfig(const std::string& user_id) {
    return m_impl->GetConfig(user_id);
}

bool ConfigService::SetConfig(const std::string& user_id, const AlertConfig& config) {
    return m_impl->SetConfig(user_id, config);
}

bool ConfigService::DeleteConfig(const std::string& user_id) {
    return m_impl->DeleteConfig(user_id);
}

bool ConfigService::EnableAlertType(const std::string& user_id, AlertType type, bool enabled) {
    return m_impl->EnableAlertType(user_id, type, enabled);
}

bool ConfigService::IsAlertTypeEnabled(const std::string& user_id, AlertType type) {
    return m_impl->IsAlertTypeEnabled(user_id, type);
}

bool ConfigService::SetThreshold(const std::string& user_id, AlertType type, const AlertThreshold& threshold) {
    return m_impl->SetThreshold(user_id, type, threshold);
}

AlertThreshold ConfigService::GetThreshold(const std::string& user_id, AlertType type) {
    return m_impl->GetThreshold(user_id, type);
}

bool ConfigService::SetPushMethods(const std::string& user_id, const std::vector<PushMethod>& methods) {
    return m_impl->SetPushMethods(user_id, methods);
}

std::vector<PushMethod> ConfigService::GetPushMethods(const std::string& user_id) {
    return m_impl->GetPushMethods(user_id);
}

bool ConfigService::SetQuietHours(const std::string& user_id, const QuietHoursConfig& config) {
    return m_impl->SetQuietHours(user_id, config);
}

QuietHoursConfig ConfigService::GetQuietHours(const std::string& user_id) {
    return m_impl->GetQuietHours(user_id);
}

SystemDefaultConfig ConfigService::GetSystemDefaults() {
    return m_impl->GetSystemDefaults();
}

bool ConfigService::SetSystemDefaults(const SystemDefaultConfig& config) {
    return m_impl->SetSystemDefaults(config);
}

bool ConfigService::ResetToDefaults(const std::string& user_id) {
    return m_impl->ResetToDefaults(user_id);
}

std::unique_ptr<IConfigService> IConfigService::Create() {
    return std::unique_ptr<IConfigService>(new ConfigService());
}

}
}
