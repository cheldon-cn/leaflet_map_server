#include "ogc/alert/threshold_manager.h"
#include "ogc/alert/exception.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ogc {
namespace alert {

ThresholdManager::ThresholdManager() {
    InitializeDefaults();
}

ThresholdManager::~ThresholdManager() {
}

void ThresholdManager::Initialize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    InitializeDefaults();
}

ThresholdConfig ThresholdManager::GetThreshold(AlertType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_thresholds.find(type);
    if (it != m_thresholds.end()) {
        return it->second;
    }
    
    return CreateDefaultThreshold(type);
}

ThresholdConfig ThresholdManager::GetThreshold(const std::string& config_id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_thresholdsById.find(config_id);
    if (it != m_thresholdsById.end()) {
        return it->second;
    }
    
    throw ConfigurationException("Threshold config not found: " + config_id);
}

void ThresholdManager::SetThreshold(const ThresholdConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!ValidateThreshold(config.value)) {
        throw ValidationException("Invalid threshold values");
    }
    
    ThresholdConfig new_config = config;
    if (new_config.config_id.empty()) {
        new_config.config_id = GenerateConfigId();
    }
    new_config.updated_at = DateTime::Now();
    
    m_thresholds[config.alert_type] = new_config;
    m_thresholdsById[new_config.config_id] = new_config;
    
    NotifyChange(config.alert_type, config.value);
}

void ThresholdManager::UpdateThreshold(const std::string& config_id, const ThresholdValue& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_thresholdsById.find(config_id);
    if (it == m_thresholdsById.end()) {
        throw ConfigurationException("Threshold config not found: " + config_id);
    }
    
    if (!ValidateThreshold(value)) {
        throw ValidationException("Invalid threshold values");
    }
    
    it->second.value = value;
    it->second.updated_at = DateTime::Now();
    
    m_thresholds[it->second.alert_type] = it->second;
    
    NotifyChange(it->second.alert_type, value);
}

void ThresholdManager::DeleteThreshold(const std::string& config_id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_thresholdsById.find(config_id);
    if (it == m_thresholdsById.end()) {
        return;
    }
    
    AlertType type = it->second.alert_type;
    m_thresholdsById.erase(it);
    
    auto typeIt = m_thresholds.find(type);
    if (typeIt != m_thresholds.end() && typeIt->second.config_id == config_id) {
        m_thresholds.erase(typeIt);
    }
}

std::vector<ThresholdConfig> ThresholdManager::GetAllThresholds() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ThresholdConfig> result;
    result.reserve(m_thresholds.size());
    
    for (const auto& pair : m_thresholds) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<ThresholdConfig> ThresholdManager::GetThresholdsByType(AlertType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ThresholdConfig> result;
    
    auto it = m_thresholds.find(type);
    if (it != m_thresholds.end()) {
        result.push_back(it->second);
    }
    
    for (const auto& pair : m_thresholdsById) {
        if (pair.second.alert_type == type && 
            (result.empty() || result[0].config_id != pair.first)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

void ThresholdManager::SetUserOverride(const UserThresholdOverride& override_config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!ValidateThreshold(override_config.value)) {
        throw ValidationException("Invalid threshold values");
    }
    
    m_userOverrides[override_config.user_id][override_config.alert_type] = override_config;
}

void ThresholdManager::RemoveUserOverride(const std::string& user_id, AlertType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto userIt = m_userOverrides.find(user_id);
    if (userIt != m_userOverrides.end()) {
        userIt->second.erase(type);
        if (userIt->second.empty()) {
            m_userOverrides.erase(userIt);
        }
    }
}

UserThresholdOverride ThresholdManager::GetUserOverride(const std::string& user_id, AlertType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto userIt = m_userOverrides.find(user_id);
    if (userIt == m_userOverrides.end()) {
        return UserThresholdOverride();
    }
    
    auto typeIt = userIt->second.find(type);
    if (typeIt == userIt->second.end()) {
        return UserThresholdOverride();
    }
    
    return typeIt->second;
}

std::vector<UserThresholdOverride> ThresholdManager::GetUserOverrides(const std::string& user_id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<UserThresholdOverride> result;
    
    auto userIt = m_userOverrides.find(user_id);
    if (userIt == m_userOverrides.end()) {
        return result;
    }
    
    for (const auto& pair : userIt->second) {
        result.push_back(pair.second);
    }
    
    return result;
}

ThresholdValue ThresholdManager::GetEffectiveThreshold(const std::string& user_id, AlertType type) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    DateTime now = DateTime::Now();
    
    auto userIt = m_userOverrides.find(user_id);
    if (userIt != m_userOverrides.end()) {
        auto typeIt = userIt->second.find(type);
        if (typeIt != userIt->second.end()) {
            const auto& override_config = typeIt->second;
            if (override_config.enabled) {
                int64_t now_ts = now.ToTimestamp();
                int64_t from_ts = override_config.effective_from.ToTimestamp();
                int64_t to_ts = override_config.effective_to.ToTimestamp();
                
                if (now_ts >= from_ts && now_ts <= to_ts) {
                    return override_config.value;
                }
            }
        }
    }
    
    auto it = m_thresholds.find(type);
    if (it != m_thresholds.end()) {
        return it->second.value;
    }
    
    return CreateDefaultThreshold(type).value;
}

void ThresholdManager::ResetToDefault(AlertType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    ThresholdConfig default_config = CreateDefaultThreshold(type);
    m_thresholds[type] = default_config;
    m_thresholdsById[default_config.config_id] = default_config;
    
    NotifyChange(type, default_config.value);
}

void ThresholdManager::ResetAllToDefault() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_thresholds.clear();
    m_thresholdsById.clear();
    InitializeDefaults();
}

bool ThresholdManager::ValidateThreshold(const ThresholdValue& value) const {
    return ThresholdValidator::ValidateGeneral(value);
}

void ThresholdManager::SetChangeCallback(ThresholdChangeCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_changeCallback = callback;
}

void ThresholdManager::InitializeDefaults() {
    ThresholdConfig depth_config = CreateDefaultThreshold(AlertType::kDepth);
    m_thresholds[AlertType::kDepth] = depth_config;
    m_thresholdsById[depth_config.config_id] = depth_config;
    
    ThresholdConfig collision_config = CreateDefaultThreshold(AlertType::kCollision);
    m_thresholds[AlertType::kCollision] = collision_config;
    m_thresholdsById[collision_config.config_id] = collision_config;
    
    ThresholdConfig weather_config = CreateDefaultThreshold(AlertType::kWeather);
    m_thresholds[AlertType::kWeather] = weather_config;
    m_thresholdsById[weather_config.config_id] = weather_config;
    
    ThresholdConfig channel_config = CreateDefaultThreshold(AlertType::kChannel);
    m_thresholds[AlertType::kChannel] = channel_config;
    m_thresholdsById[channel_config.config_id] = channel_config;
    
    ThresholdConfig deviation_config = CreateDefaultThreshold(AlertType::kDeviation);
    m_thresholds[AlertType::kDeviation] = deviation_config;
    m_thresholdsById[deviation_config.config_id] = deviation_config;
    
    ThresholdConfig speed_config = CreateDefaultThreshold(AlertType::kSpeed);
    m_thresholds[AlertType::kSpeed] = speed_config;
    m_thresholdsById[speed_config.config_id] = speed_config;
    
    ThresholdConfig restricted_config = CreateDefaultThreshold(AlertType::kRestrictedArea);
    m_thresholds[AlertType::kRestrictedArea] = restricted_config;
    m_thresholdsById[restricted_config.config_id] = restricted_config;
}

ThresholdConfig ThresholdManager::CreateDefaultThreshold(AlertType type) const {
    ThresholdConfig config;
    config.config_id = GenerateConfigId();
    config.alert_type = type;
    config.is_default = true;
    config.is_active = true;
    config.created_at = DateTime::Now();
    config.updated_at = DateTime::Now();
    
    switch (type) {
        case AlertType::kDepth:
            config.name = "Depth Alert Threshold";
            config.value = ThresholdPresets::GetDepthPreset("default");
            break;
            
        case AlertType::kCollision:
            config.name = "Collision Alert Threshold";
            config.value = ThresholdPresets::GetCollisionPreset("default");
            break;
            
        case AlertType::kWeather:
            config.name = "Weather Alert Threshold";
            config.value = ThresholdPresets::GetWeatherPreset("default");
            break;
            
        case AlertType::kChannel:
            config.name = "Channel Alert Threshold";
            config.value.level1 = 500.0;
            config.value.level2 = 300.0;
            config.value.level3 = 100.0;
            config.value.level4 = 50.0;
            config.value.unit = "meters";
            config.value.description = "Distance to channel boundary";
            break;
            
        case AlertType::kDeviation:
            config.name = "Deviation Alert Threshold";
            config.value.level1 = 200.0;
            config.value.level2 = 150.0;
            config.value.level3 = 100.0;
            config.value.level4 = 50.0;
            config.value.unit = "meters";
            config.value.description = "Distance from planned route";
            break;
            
        case AlertType::kSpeed:
            config.name = "Speed Alert Threshold";
            config.value.level1 = 1.0;
            config.value.level2 = 2.0;
            config.value.level3 = 5.0;
            config.value.level4 = 10.0;
            config.value.unit = "knots";
            config.value.description = "Speed over limit";
            break;
            
        case AlertType::kRestrictedArea:
            config.name = "Restricted Area Alert Threshold";
            config.value.level1 = 1000.0;
            config.value.level2 = 500.0;
            config.value.level3 = 200.0;
            config.value.level4 = 100.0;
            config.value.unit = "meters";
            config.value.description = "Distance to restricted area";
            break;
            
        default:
            config.name = "Unknown Alert Threshold";
            config.value.level1 = 1.0;
            config.value.level2 = 0.5;
            config.value.level3 = 0.2;
            config.value.level4 = 0.1;
            config.value.unit = "unknown";
            break;
    }
    
    return config;
}

std::string ThresholdManager::GenerateConfigId() const {
    std::ostringstream oss;
    oss << "TH_" << std::time(nullptr) << "_" << std::rand();
    return oss.str();
}

void ThresholdManager::NotifyChange(AlertType type, const ThresholdValue& value) {
    if (m_changeCallback) {
        m_changeCallback(type, value);
    }
}

bool ThresholdValidator::ValidateDepthThreshold(const ThresholdValue& value) {
    return value.level1 >= 0 && value.level2 >= 0 && 
           value.level3 >= 0 && value.level4 >= 0 &&
           value.level1 > value.level2 && value.level2 > value.level3 && 
           value.level3 > value.level4;
}

bool ThresholdValidator::ValidateCollisionThreshold(const ThresholdValue& value) {
    return value.level1 >= 0 && value.level2 >= 0 && 
           value.level3 >= 0 && value.level4 >= 0 &&
           value.level1 > value.level2 && value.level2 > value.level3 && 
           value.level3 > value.level4;
}

bool ThresholdValidator::ValidateWeatherThreshold(const ThresholdValue& value) {
    return value.level1 >= 0 && value.level2 >= 0 && 
           value.level3 >= 0 && value.level4 >= 0;
}

bool ThresholdValidator::ValidateSpeedThreshold(const ThresholdValue& value) {
    return value.level1 >= 0 && value.level2 >= 0 && 
           value.level3 >= 0 && value.level4 >= 0;
}

bool ThresholdValidator::ValidateDistanceThreshold(const ThresholdValue& value) {
    return value.level1 >= 0 && value.level2 >= 0 && 
           value.level3 >= 0 && value.level4 >= 0 &&
           value.level1 > value.level2 && value.level2 > value.level3 && 
           value.level3 > value.level4;
}

bool ThresholdValidator::ValidateGeneral(const ThresholdValue& value) {
    if (value.level1 < 0 || value.level2 < 0 || 
        value.level3 < 0 || value.level4 < 0) {
        return false;
    }
    
    return true;
}

std::string ThresholdValidator::GetValidationMessage(const ThresholdValue& value) {
    if (value.level1 < 0) return "Level 1 threshold cannot be negative";
    if (value.level2 < 0) return "Level 2 threshold cannot be negative";
    if (value.level3 < 0) return "Level 3 threshold cannot be negative";
    if (value.level4 < 0) return "Level 4 threshold cannot be negative";
    
    return "Valid threshold values";
}

ThresholdValue ThresholdPresets::GetDepthPreset(const std::string& preset_name) {
    ThresholdValue value;
    value.unit = "meters";
    value.description = "Under Keel Clearance";
    
    if (preset_name == "conservative") {
        value = ConservativePreset();
        value.unit = "meters";
        value.description = "Under Keel Clearance (Conservative)";
    } else if (preset_name == "aggressive") {
        value = AggressivePreset();
        value.unit = "meters";
        value.description = "Under Keel Clearance (Aggressive)";
    } else {
        value.level1 = 2.0;
        value.level2 = 1.5;
        value.level3 = 1.0;
        value.level4 = 0.5;
        value.safety_margin = 0.5;
    }
    
    return value;
}

ThresholdValue ThresholdPresets::GetCollisionPreset(const std::string& preset_name) {
    ThresholdValue value;
    value.unit = "nm";
    value.description = "CPA distance";
    
    if (preset_name == "conservative") {
        value = ConservativePreset();
        value.unit = "nm";
        value.description = "CPA distance (Conservative)";
    } else if (preset_name == "aggressive") {
        value = AggressivePreset();
        value.unit = "nm";
        value.description = "CPA distance (Aggressive)";
    } else {
        value.level1 = 6.0;
        value.level2 = 4.0;
        value.level3 = 2.0;
        value.level4 = 1.0;
        value.safety_margin = 0.5;
    }
    
    return value;
}

ThresholdValue ThresholdPresets::GetWeatherPreset(const std::string& preset_name) {
    ThresholdValue value;
    value.unit = "m/s";
    value.description = "Wind speed";
    
    if (preset_name == "conservative") {
        value = ConservativePreset();
        value.unit = "m/s";
        value.description = "Wind speed (Conservative)";
    } else if (preset_name == "aggressive") {
        value = AggressivePreset();
        value.unit = "m/s";
        value.description = "Wind speed (Aggressive)";
    } else {
        value.level1 = 10.0;
        value.level2 = 15.0;
        value.level3 = 20.0;
        value.level4 = 25.0;
        value.safety_margin = 5.0;
    }
    
    return value;
}

std::vector<std::string> ThresholdPresets::GetAvailablePresets(AlertType type) {
    return {"default", "conservative", "moderate", "aggressive"};
}

ThresholdValue ThresholdPresets::ConservativePreset() {
    ThresholdValue value;
    value.level1 = 3.0;
    value.level2 = 2.5;
    value.level3 = 2.0;
    value.level4 = 1.5;
    value.safety_margin = 1.0;
    return value;
}

ThresholdValue ThresholdPresets::ModeratePreset() {
    ThresholdValue value;
    value.level1 = 2.0;
    value.level2 = 1.5;
    value.level3 = 1.0;
    value.level4 = 0.5;
    value.safety_margin = 0.5;
    return value;
}

ThresholdValue ThresholdPresets::AggressivePreset() {
    ThresholdValue value;
    value.level1 = 1.0;
    value.level2 = 0.7;
    value.level3 = 0.5;
    value.level4 = 0.3;
    value.safety_margin = 0.2;
    return value;
}

std::unique_ptr<IThresholdManager> IThresholdManager::Create() {
    return std::unique_ptr<IThresholdManager>(new ThresholdManager());
}

}
}
