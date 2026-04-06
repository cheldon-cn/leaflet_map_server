#include "ogc/alert/push_strategy.h"
#include "ogc/base/log.h"
#include <mutex>
#include <ctime>
#include <algorithm>

namespace ogc {
namespace alert {

using ogc::base::LogLevel;
using ogc::base::LogHelper;

PushStrategyManager::PushStrategyManager()
    : m_maxRetryCount(3)
    , m_retryIntervalMs(1000) {
    m_defaultStrategies[AlertLevel::kLevel1] = {AlertLevel::kLevel1, {PushMethod::kApp, PushMethod::kSound, PushMethod::kSms}, 3, 1000, true};
    m_defaultStrategies[AlertLevel::kLevel2] = {AlertLevel::kLevel2, {PushMethod::kApp, PushMethod::kSound}, 3, 1000, true};
    m_defaultStrategies[AlertLevel::kLevel3] = {AlertLevel::kLevel3, {PushMethod::kApp}, 2, 2000, true};
    m_defaultStrategies[AlertLevel::kLevel4] = {AlertLevel::kLevel4, {PushMethod::kApp}, 1, 3000, true};
}

PushStrategyManager::~PushStrategyManager() {
}

void PushStrategyManager::SetDefaultStrategy(AlertLevel level, const std::vector<PushMethod>& methods) {
    auto it = m_defaultStrategies.find(level);
    if (it != m_defaultStrategies.end()) {
        it->second.methods = methods;
    } else {
        PushStrategyConfig config;
        config.level = level;
        config.methods = methods;
        config.max_retry_count = m_maxRetryCount;
        config.retry_interval_ms = m_retryIntervalMs;
        config.enabled = true;
        m_defaultStrategies[level] = config;
    }
    
    LOG_INFO() << "Default push strategy set for level " << static_cast<int>(level);
}

std::vector<PushMethod> PushStrategyManager::GetDefaultStrategy(AlertLevel level) const {
    auto it = m_defaultStrategies.find(level);
    if (it != m_defaultStrategies.end()) {
        return it->second.methods;
    }
    return {};
}

void PushStrategyManager::SetUserPreference(const std::string& user_id, const UserPushPreference& preference) {
    m_userPreferences[user_id] = preference;
    LOG_INFO() << "User push preference set for user: " << user_id;
}

UserPushPreference PushStrategyManager::GetUserPreference(const std::string& user_id) const {
    auto it = m_userPreferences.find(user_id);
    if (it != m_userPreferences.end()) {
        return it->second;
    }
    return UserPushPreference();
}

bool PushStrategyManager::HasUserPreference(const std::string& user_id) const {
    return m_userPreferences.find(user_id) != m_userPreferences.end();
}

std::vector<PushMethod> PushStrategyManager::DeterminePushMethods(const std::string& user_id, AlertLevel level) {
    if (IsInQuietHours(user_id)) {
        auto it = m_userPreferences.find(user_id);
        if (it != m_userPreferences.end() && it->second.quiet_hours_enabled) {
            return {};
        }
    }
    
    auto userIt = m_userPreferences.find(user_id);
    if (userIt != m_userPreferences.end()) {
        auto levelIt = userIt->second.level_preferences.find(level);
        if (levelIt != userIt->second.level_preferences.end()) {
            return levelIt->second;
        }
    }
    
    auto defaultIt = m_defaultStrategies.find(level);
    if (defaultIt != m_defaultStrategies.end() && defaultIt->second.enabled) {
        return defaultIt->second.methods;
    }
    
    return {};
}

void PushStrategyManager::SetMaxRetryCount(int count) {
    m_maxRetryCount = count;
    for (auto& pair : m_defaultStrategies) {
        pair.second.max_retry_count = count;
    }
}

int PushStrategyManager::GetMaxRetryCount() const {
    return m_maxRetryCount;
}

void PushStrategyManager::SetRetryInterval(int interval_ms) {
    m_retryIntervalMs = interval_ms;
    for (auto& pair : m_defaultStrategies) {
        pair.second.retry_interval_ms = interval_ms;
    }
}

int PushStrategyManager::GetRetryInterval() const {
    return m_retryIntervalMs;
}

bool PushStrategyManager::IsInQuietHours(const std::string& user_id) {
    auto it = m_userPreferences.find(user_id);
    if (it == m_userPreferences.end() || !it->second.quiet_hours_enabled) {
        return false;
    }
    
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    int currentMinutes = localTime->tm_hour * 60 + localTime->tm_min;
    
    int startMinutes = it->second.quiet_start_hour * 60 + it->second.quiet_start_minute;
    int endMinutes = it->second.quiet_end_hour * 60 + it->second.quiet_end_minute;
    
    if (startMinutes <= endMinutes) {
        return currentMinutes >= startMinutes && currentMinutes < endMinutes;
    } else {
        return currentMinutes >= startMinutes || currentMinutes < endMinutes;
    }
}

void PushStrategyManager::EnableStrategy(AlertLevel level, bool enabled) {
    auto it = m_defaultStrategies.find(level);
    if (it != m_defaultStrategies.end()) {
        it->second.enabled = enabled;
    }
}

bool PushStrategyManager::IsStrategyEnabled(AlertLevel level) const {
    auto it = m_defaultStrategies.find(level);
    if (it != m_defaultStrategies.end()) {
        return it->second.enabled;
    }
    return false;
}

std::unique_ptr<IPushStrategyManager> IPushStrategyManager::Create() {
    return std::unique_ptr<IPushStrategyManager>(new PushStrategyManager());
}

}
}
