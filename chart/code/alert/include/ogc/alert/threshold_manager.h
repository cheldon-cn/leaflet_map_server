#ifndef OGC_ALERT_THRESHOLD_MANAGER_H
#define OGC_ALERT_THRESHOLD_MANAGER_H

#include "export.h"
#include "types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <functional>

namespace ogc {
namespace alert {

struct ThresholdValue {
    double level1;
    double level2;
    double level3;
    double level4;
    double safety_margin;
    std::string unit;
    std::string description;
};

struct ThresholdConfig {
    std::string config_id;
    std::string name;
    AlertType alert_type;
    ThresholdValue value;
    bool is_default;
    bool is_active;
    DateTime created_at;
    DateTime updated_at;
    std::string created_by;
};

struct UserThresholdOverride {
    std::string user_id;
    AlertType alert_type;
    ThresholdValue value;
    bool enabled;
    DateTime effective_from;
    DateTime effective_to;
};

class OGC_ALERT_API IThresholdManager {
public:
    virtual ~IThresholdManager() = default;
    
    virtual void Initialize() = 0;
    
    virtual ThresholdConfig GetThreshold(AlertType type) const = 0;
    virtual ThresholdConfig GetThreshold(const std::string& config_id) const = 0;
    
    virtual void SetThreshold(const ThresholdConfig& config) = 0;
    virtual void UpdateThreshold(const std::string& config_id, const ThresholdValue& value) = 0;
    virtual void DeleteThreshold(const std::string& config_id) = 0;
    
    virtual std::vector<ThresholdConfig> GetAllThresholds() const = 0;
    virtual std::vector<ThresholdConfig> GetThresholdsByType(AlertType type) const = 0;
    
    virtual void SetUserOverride(const UserThresholdOverride& override_config) = 0;
    virtual void RemoveUserOverride(const std::string& user_id, AlertType type) = 0;
    virtual UserThresholdOverride GetUserOverride(const std::string& user_id, AlertType type) const = 0;
    virtual std::vector<UserThresholdOverride> GetUserOverrides(const std::string& user_id) const = 0;
    
    virtual ThresholdValue GetEffectiveThreshold(const std::string& user_id, AlertType type) const = 0;
    
    virtual void ResetToDefault(AlertType type) = 0;
    virtual void ResetAllToDefault() = 0;
    
    virtual bool ValidateThreshold(const ThresholdValue& value) const = 0;
    
    using ThresholdChangeCallback = std::function<void(AlertType, const ThresholdValue&)>;
    virtual void SetChangeCallback(ThresholdChangeCallback callback) = 0;
    
    static std::unique_ptr<IThresholdManager> Create();
};

class OGC_ALERT_API ThresholdManager : public IThresholdManager {
public:
    ThresholdManager();
    ~ThresholdManager() override;
    
    void Initialize() override;
    
    ThresholdConfig GetThreshold(AlertType type) const override;
    ThresholdConfig GetThreshold(const std::string& config_id) const override;
    
    void SetThreshold(const ThresholdConfig& config) override;
    void UpdateThreshold(const std::string& config_id, const ThresholdValue& value) override;
    void DeleteThreshold(const std::string& config_id) override;
    
    std::vector<ThresholdConfig> GetAllThresholds() const override;
    std::vector<ThresholdConfig> GetThresholdsByType(AlertType type) const override;
    
    void SetUserOverride(const UserThresholdOverride& override_config) override;
    void RemoveUserOverride(const std::string& user_id, AlertType type) override;
    UserThresholdOverride GetUserOverride(const std::string& user_id, AlertType type) const override;
    std::vector<UserThresholdOverride> GetUserOverrides(const std::string& user_id) const override;
    
    ThresholdValue GetEffectiveThreshold(const std::string& user_id, AlertType type) const override;
    
    void ResetToDefault(AlertType type) override;
    void ResetAllToDefault() override;
    
    bool ValidateThreshold(const ThresholdValue& value) const override;
    
    void SetChangeCallback(ThresholdChangeCallback callback) override;

private:
    mutable std::mutex m_mutex;
    std::map<AlertType, ThresholdConfig> m_thresholds;
    std::map<std::string, ThresholdConfig> m_thresholdsById;
    std::map<std::string, std::map<AlertType, UserThresholdOverride>> m_userOverrides;
    ThresholdChangeCallback m_changeCallback;
    
    void InitializeDefaults();
    ThresholdConfig CreateDefaultThreshold(AlertType type) const;
    std::string GenerateConfigId() const;
    void NotifyChange(AlertType type, const ThresholdValue& value);
};

class OGC_ALERT_API ThresholdValidator {
public:
    static bool ValidateDepthThreshold(const ThresholdValue& value);
    static bool ValidateCollisionThreshold(const ThresholdValue& value);
    static bool ValidateWeatherThreshold(const ThresholdValue& value);
    static bool ValidateSpeedThreshold(const ThresholdValue& value);
    static bool ValidateDistanceThreshold(const ThresholdValue& value);
    
    static bool ValidateGeneral(const ThresholdValue& value);
    static std::string GetValidationMessage(const ThresholdValue& value);
};

class OGC_ALERT_API ThresholdPresets {
public:
    static ThresholdValue GetDepthPreset(const std::string& preset_name);
    static ThresholdValue GetCollisionPreset(const std::string& preset_name);
    static ThresholdValue GetWeatherPreset(const std::string& preset_name);
    
    static std::vector<std::string> GetAvailablePresets(AlertType type);
    
    static ThresholdValue ConservativePreset();
    static ThresholdValue ModeratePreset();
    static ThresholdValue AggressivePreset();
};

}
}

#endif
