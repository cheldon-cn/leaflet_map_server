#ifndef OGC_ALERT_USER_CONFIG_STORE_H
#define OGC_ALERT_USER_CONFIG_STORE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

struct UserAlertConfig {
    std::string user_id;
    bool depth_alert_enabled;
    double depth_threshold;
    bool collision_alert_enabled;
    double cpa_threshold;
    double tcpa_threshold;
    bool weather_alert_enabled;
    int weather_level_threshold;
    bool channel_alert_enabled;
    bool deviation_alert_enabled;
    double deviation_threshold;
    bool speed_alert_enabled;
    double speed_threshold;
    bool restricted_area_alert_enabled;
    std::vector<PushMethod> push_methods;
    bool quiet_hours_enabled;
    int quiet_start_hour;
    int quiet_start_minute;
    int quiet_end_hour;
    int quiet_end_minute;
    
    UserAlertConfig()
        : depth_alert_enabled(true)
        , depth_threshold(2.0)
        , collision_alert_enabled(true)
        , cpa_threshold(0.5)
        , tcpa_threshold(30.0)
        , weather_alert_enabled(true)
        , weather_level_threshold(3)
        , channel_alert_enabled(true)
        , deviation_alert_enabled(true)
        , deviation_threshold(0.5)
        , speed_alert_enabled(true)
        , speed_threshold(12.0)
        , restricted_area_alert_enabled(true)
        , quiet_hours_enabled(false)
        , quiet_start_hour(22)
        , quiet_start_minute(0)
        , quiet_end_hour(7)
        , quiet_end_minute(0) {}
};

struct UserSubscription {
    std::string user_id;
    std::string subscription_id;
    AlertType alert_type;
    AlertLevel min_level;
    bool enabled;
    DateTime created_at;
    DateTime updated_at;
};

class OGC_ALERT_API IUserConfigStore {
public:
    virtual ~IUserConfigStore() = default;
    
    virtual bool SaveConfig(const UserAlertConfig& config) = 0;
    virtual UserAlertConfig LoadConfig(const std::string& user_id) = 0;
    virtual bool DeleteConfig(const std::string& user_id) = 0;
    virtual bool HasConfig(const std::string& user_id) = 0;
    
    virtual bool SaveSubscription(const UserSubscription& subscription) = 0;
    virtual std::vector<UserSubscription> LoadSubscriptions(const std::string& user_id) = 0;
    virtual bool DeleteSubscription(const std::string& subscription_id) = 0;
    virtual bool UpdateSubscription(const std::string& subscription_id, bool enabled) = 0;
    
    virtual std::vector<std::string> GetSubscribedUsers(AlertType alert_type, AlertLevel level) = 0;
    
    static std::unique_ptr<IUserConfigStore> Create();
};

class OGC_ALERT_API UserConfigStore : public IUserConfigStore {
public:
    UserConfigStore();
    ~UserConfigStore();
    
    bool SaveConfig(const UserAlertConfig& config) override;
    UserAlertConfig LoadConfig(const std::string& user_id) override;
    bool DeleteConfig(const std::string& user_id) override;
    bool HasConfig(const std::string& user_id) override;
    
    bool SaveSubscription(const UserSubscription& subscription) override;
    std::vector<UserSubscription> LoadSubscriptions(const std::string& user_id) override;
    bool DeleteSubscription(const std::string& subscription_id) override;
    bool UpdateSubscription(const std::string& subscription_id, bool enabled) override;
    
    std::vector<std::string> GetSubscribedUsers(AlertType alert_type, AlertLevel level) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
