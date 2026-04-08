#ifndef OGC_ALERT_CONFIG_SERVICE_H
#define OGC_ALERT_CONFIG_SERVICE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace ogc {
namespace alert {

struct QuietHoursConfig {
    bool enabled;
    std::string start_time;
    std::string end_time;
};

struct SystemDefaultConfig {
    DepthAlertThreshold depth_threshold;
    CollisionAlertThreshold collision_threshold;
    DeviationThreshold deviation_threshold;
    SpeedThreshold speed_threshold;
};

class OGC_ALERT_API IConfigService {
public:
    virtual ~IConfigService() = default;
    
    virtual AlertConfig GetConfig(const std::string& user_id) = 0;
    virtual bool SetConfig(const std::string& user_id, const AlertConfig& config) = 0;
    virtual bool DeleteConfig(const std::string& user_id) = 0;
    
    virtual bool EnableAlertType(const std::string& user_id, AlertType type, bool enabled) = 0;
    virtual bool IsAlertTypeEnabled(const std::string& user_id, AlertType type) = 0;
    
    virtual bool SetThreshold(const std::string& user_id, AlertType type, const AlertThreshold& threshold) = 0;
    virtual AlertThreshold GetThreshold(const std::string& user_id, AlertType type) = 0;
    
    virtual bool SetPushMethods(const std::string& user_id, const std::vector<PushMethod>& methods) = 0;
    virtual std::vector<PushMethod> GetPushMethods(const std::string& user_id) = 0;
    
    virtual bool SetQuietHours(const std::string& user_id, const QuietHoursConfig& config) = 0;
    virtual QuietHoursConfig GetQuietHours(const std::string& user_id) = 0;
    
    virtual SystemDefaultConfig GetSystemDefaults() = 0;
    virtual bool SetSystemDefaults(const SystemDefaultConfig& config) = 0;
    
    virtual bool ResetToDefaults(const std::string& user_id) = 0;
    
    static std::unique_ptr<IConfigService> Create();
};

class OGC_ALERT_API ConfigService : public IConfigService {
public:
    ConfigService();
    explicit ConfigService(const std::string& db_path);
    ~ConfigService();
    
    AlertConfig GetConfig(const std::string& user_id) override;
    bool SetConfig(const std::string& user_id, const AlertConfig& config) override;
    bool DeleteConfig(const std::string& user_id) override;
    
    bool EnableAlertType(const std::string& user_id, AlertType type, bool enabled) override;
    bool IsAlertTypeEnabled(const std::string& user_id, AlertType type) override;
    
    bool SetThreshold(const std::string& user_id, AlertType type, const AlertThreshold& threshold) override;
    AlertThreshold GetThreshold(const std::string& user_id, AlertType type) override;
    
    bool SetPushMethods(const std::string& user_id, const std::vector<PushMethod>& methods) override;
    std::vector<PushMethod> GetPushMethods(const std::string& user_id) override;
    
    bool SetQuietHours(const std::string& user_id, const QuietHoursConfig& config) override;
    QuietHoursConfig GetQuietHours(const std::string& user_id) override;
    
    SystemDefaultConfig GetSystemDefaults() override;
    bool SetSystemDefaults(const SystemDefaultConfig& config) override;
    
    bool ResetToDefaults(const std::string& user_id) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
