#ifndef OGC_ALERT_PUSH_STRATEGY_H
#define OGC_ALERT_PUSH_STRATEGY_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ogc {
namespace alert {

struct PushStrategyConfig {
    AlertLevel level;
    std::vector<PushMethod> methods;
    int max_retry_count;
    int retry_interval_ms;
    bool enabled;
};

struct TimeWindow {
    int start_hour;
    int start_minute;
    int end_hour;
    int end_minute;
    bool enabled;
};

struct UserPushPreference {
    std::string user_id;
    std::map<AlertLevel, std::vector<PushMethod>> level_preferences;
    std::map<PushMethod, TimeWindow> method_time_windows;
    bool quiet_hours_enabled;
    int quiet_start_hour;
    int quiet_start_minute;
    int quiet_end_hour;
    int quiet_end_minute;
    
    UserPushPreference()
        : quiet_hours_enabled(false)
        , quiet_start_hour(22)
        , quiet_start_minute(0)
        , quiet_end_hour(7)
        , quiet_end_minute(0) {}
};

class OGC_ALERT_API IPushStrategyManager {
public:
    virtual ~IPushStrategyManager() = default;
    
    virtual void SetDefaultStrategy(AlertLevel level, const std::vector<PushMethod>& methods) = 0;
    virtual std::vector<PushMethod> GetDefaultStrategy(AlertLevel level) const = 0;
    
    virtual void SetUserPreference(const std::string& user_id, const UserPushPreference& preference) = 0;
    virtual UserPushPreference GetUserPreference(const std::string& user_id) const = 0;
    virtual bool HasUserPreference(const std::string& user_id) const = 0;
    
    virtual std::vector<PushMethod> DeterminePushMethods(const std::string& user_id, AlertLevel level) = 0;
    
    virtual void SetMaxRetryCount(int count) = 0;
    virtual int GetMaxRetryCount() const = 0;
    
    virtual void SetRetryInterval(int interval_ms) = 0;
    virtual int GetRetryInterval() const = 0;
    
    virtual bool IsInQuietHours(const std::string& user_id) = 0;
    
    virtual void EnableStrategy(AlertLevel level, bool enabled) = 0;
    virtual bool IsStrategyEnabled(AlertLevel level) const = 0;
    
    static std::unique_ptr<IPushStrategyManager> Create();
};

class OGC_ALERT_API PushStrategyManager : public IPushStrategyManager {
public:
    PushStrategyManager();
    ~PushStrategyManager();
    
    void SetDefaultStrategy(AlertLevel level, const std::vector<PushMethod>& methods) override;
    std::vector<PushMethod> GetDefaultStrategy(AlertLevel level) const override;
    
    void SetUserPreference(const std::string& user_id, const UserPushPreference& preference) override;
    UserPushPreference GetUserPreference(const std::string& user_id) const override;
    bool HasUserPreference(const std::string& user_id) const override;
    
    std::vector<PushMethod> DeterminePushMethods(const std::string& user_id, AlertLevel level) override;
    
    void SetMaxRetryCount(int count) override;
    int GetMaxRetryCount() const override;
    
    void SetRetryInterval(int interval_ms) override;
    int GetRetryInterval() const override;
    
    bool IsInQuietHours(const std::string& user_id) override;
    
    void EnableStrategy(AlertLevel level, bool enabled) override;
    bool IsStrategyEnabled(AlertLevel level) const override;
    
private:
    std::map<AlertLevel, PushStrategyConfig> m_defaultStrategies;
    std::map<std::string, UserPushPreference> m_userPreferences;
    int m_maxRetryCount;
    int m_retryIntervalMs;
};

}
}

#endif
