#include "ogc/alert/user_config_store.h"
#include "ogc/draw/log.h"
#include <map>
#include <mutex>
#include <algorithm>

using ogc::draw::LogLevel;
using ogc::draw::LogHelper;

namespace ogc {
namespace alert {

class UserConfigStore::Impl {
public:
    bool SaveConfig(const UserAlertConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_configs[config.user_id] = config;
        LOG_INFO() << "User config saved for user: " << config.user_id;
        return true;
    }
    
    UserAlertConfig LoadConfig(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_configs.find(user_id);
        if (it != m_configs.end()) {
            return it->second;
        }
        UserAlertConfig defaultConfig;
        defaultConfig.user_id = user_id;
        return defaultConfig;
    }
    
    bool DeleteConfig(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_configs.find(user_id);
        if (it != m_configs.end()) {
            m_configs.erase(it);
            LOG_INFO() << "User config deleted for user: " << user_id;
            return true;
        }
        return false;
    }
    
    bool HasConfig(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_configs.find(user_id) != m_configs.end();
    }
    
    bool SaveSubscription(const UserSubscription& subscription) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_subscriptions[subscription.subscription_id] = subscription;
        m_userSubscriptions[subscription.user_id].push_back(subscription.subscription_id);
        LOG_INFO() << "Subscription saved: " << subscription.subscription_id;
        return true;
    }
    
    std::vector<UserSubscription> LoadSubscriptions(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<UserSubscription> result;
        auto it = m_userSubscriptions.find(user_id);
        if (it != m_userSubscriptions.end()) {
            for (const auto& subId : it->second) {
                auto subIt = m_subscriptions.find(subId);
                if (subIt != m_subscriptions.end()) {
                    result.push_back(subIt->second);
                }
            }
        }
        return result;
    }
    
    bool DeleteSubscription(const std::string& subscription_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_subscriptions.find(subscription_id);
        if (it != m_subscriptions.end()) {
            std::string userId = it->second.user_id;
            m_subscriptions.erase(it);
            
            auto userIt = m_userSubscriptions.find(userId);
            if (userIt != m_userSubscriptions.end()) {
                auto& subs = userIt->second;
                subs.erase(std::remove(subs.begin(), subs.end(), subscription_id), subs.end());
            }
            
            LOG_INFO() << "Subscription deleted: " << subscription_id;
            return true;
        }
        return false;
    }
    
    bool UpdateSubscription(const std::string& subscription_id, bool enabled) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_subscriptions.find(subscription_id);
        if (it != m_subscriptions.end()) {
            it->second.enabled = enabled;
            it->second.updated_at = DateTime::Now();
            LOG_INFO() << "Subscription updated: " << subscription_id;
            return true;
        }
        return false;
    }
    
    std::vector<std::string> GetSubscribedUsers(AlertType alert_type, AlertLevel level) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> result;
        
        for (const auto& pair : m_subscriptions) {
            const auto& sub = pair.second;
            if (sub.enabled && sub.alert_type == alert_type && 
                static_cast<int>(sub.min_level) <= static_cast<int>(level)) {
                result.push_back(sub.user_id);
            }
        }
        
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        
        return result;
    }
    
private:
    std::map<std::string, UserAlertConfig> m_configs;
    std::map<std::string, UserSubscription> m_subscriptions;
    std::map<std::string, std::vector<std::string>> m_userSubscriptions;
    mutable std::mutex m_mutex;
};

UserConfigStore::UserConfigStore()
    : m_impl(std::make_unique<Impl>()) {
}

UserConfigStore::~UserConfigStore() {
}

bool UserConfigStore::SaveConfig(const UserAlertConfig& config) {
    return m_impl->SaveConfig(config);
}

UserAlertConfig UserConfigStore::LoadConfig(const std::string& user_id) {
    return m_impl->LoadConfig(user_id);
}

bool UserConfigStore::DeleteConfig(const std::string& user_id) {
    return m_impl->DeleteConfig(user_id);
}

bool UserConfigStore::HasConfig(const std::string& user_id) {
    return m_impl->HasConfig(user_id);
}

bool UserConfigStore::SaveSubscription(const UserSubscription& subscription) {
    return m_impl->SaveSubscription(subscription);
}

std::vector<UserSubscription> UserConfigStore::LoadSubscriptions(const std::string& user_id) {
    return m_impl->LoadSubscriptions(user_id);
}

bool UserConfigStore::DeleteSubscription(const std::string& subscription_id) {
    return m_impl->DeleteSubscription(subscription_id);
}

bool UserConfigStore::UpdateSubscription(const std::string& subscription_id, bool enabled) {
    return m_impl->UpdateSubscription(subscription_id, enabled);
}

std::vector<std::string> UserConfigStore::GetSubscribedUsers(AlertType alert_type, AlertLevel level) {
    return m_impl->GetSubscribedUsers(alert_type, level);
}

std::unique_ptr<IUserConfigStore> IUserConfigStore::Create() {
    return std::unique_ptr<IUserConfigStore>(new UserConfigStore());
}

}
}
