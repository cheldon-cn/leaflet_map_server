#ifndef OGC_ALERT_TYPES_H
#define OGC_ALERT_TYPES_H

#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <chrono>
#include <ctime>

namespace ogc {
namespace alert {

enum class AlertType : uint8_t {
    kUnknown = 0,
    kDepth = 1,
    kWeather = 2,
    kCollision = 3,
    kChannel = 4,
    kDeviation = 5,
    kSpeed = 6,
    kRestrictedArea = 7
};

enum class AlertLevel : uint8_t {
    kNone = 0,
    kLevel1 = 1,
    kLevel2 = 2,
    kLevel3 = 3,
    kLevel4 = 4
};

enum class AlertStatus : uint8_t {
    kActive = 0,
    kAcknowledged = 1,
    kExpired = 2,
    kDismissed = 3
};

enum class PushMethod : uint8_t {
    kApp = 0,
    kSound = 1,
    kSms = 2,
    kEmail = 3
};

enum class WeatherType : uint8_t {
    kClear = 0,
    kCloudy = 1,
    kRain = 2,
    kHeavyRain = 3,
    kThunderstorm = 4,
    kFog = 5,
    kSnow = 6,
    kIce = 7,
    kHighWind = 8,
    kStorm = 9,
    kHurricane = 10,
    kUnknown = 255
};

struct OGC_ALERT_API DateTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
    
    DateTime();
    explicit DateTime(std::time_t timestamp);
    static DateTime Now();
    static DateTime FromString(const std::string& str);
    static DateTime FromTimestamp(std::time_t timestamp);
    std::string ToString() const;
    std::time_t ToTimestamp() const;
    bool IsValid() const;
    bool operator==(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator<=(const DateTime& other) const;
    bool operator>(const DateTime& other) const;
    bool operator>=(const DateTime& other) const;
};

struct OGC_ALERT_API Coordinate {
    double longitude;
    double latitude;
    double altitude;
    
    Coordinate();
    Coordinate(double lon, double lat, double alt = 0.0);
    bool IsValid() const;
    bool operator==(const Coordinate& other) const;
    double DistanceTo(const Coordinate& other) const;
};

struct OGC_ALERT_API AlertThreshold {
    double level1_threshold;
    double level2_threshold;
    double level3_threshold;
    double level4_threshold;
    
    AlertThreshold();
    AlertLevel DetermineLevel(double value) const;
};

struct OGC_ALERT_API DepthAlertThreshold : AlertThreshold {
    double ukc_safety_margin;
    bool use_dynamic_ukc;
};

struct OGC_ALERT_API CollisionAlertThreshold : AlertThreshold {
    double cpa_threshold_level1;
    double cpa_threshold_level2;
    double cpa_threshold_level3;
    double tcpa_threshold_level1;
    double tcpa_threshold_level2;
    double tcpa_threshold_level3;
};

struct OGC_ALERT_API DeviationThreshold : AlertThreshold {
};

struct OGC_ALERT_API SpeedThreshold {
    double level1_ratio;
    double level2_ratio;
    double level3_ratio;
};

struct OGC_ALERT_API AlertContent {
    std::string type;
    int level;
    std::string title;
    std::string message;
    Coordinate position;
    std::string action_required;
};

struct OGC_ALERT_API Alert {
    std::string alert_id;
    AlertType alert_type;
    AlertLevel alert_level;
    AlertStatus status;
    DateTime issue_time;
    DateTime expire_time;
    Coordinate position;
    AlertContent content;
    std::string user_id;
    bool acknowledge_required;
    
    Alert() 
        : alert_type(AlertType::kUnknown)
        , alert_level(AlertLevel::kNone)
        , status(AlertStatus::kActive)
        , acknowledge_required(true) {}
    
    virtual ~Alert() = default;
};

using AlertPtr = std::shared_ptr<Alert>;

struct OGC_ALERT_API DepthAlert : Alert {
    double current_depth;
    double ukc;
    double safety_depth;
    double tide_height;
    std::string chart_id;
};

struct OGC_ALERT_API CollisionAlert : Alert {
    std::string target_ship_id;
    std::string target_ship_name;
    double cpa;
    double tcpa;
    double relative_bearing;
    double relative_speed;
    Coordinate target_position;
};

struct OGC_ALERT_API WeatherAlert : Alert {
    std::string weather_type;
    double wind_speed;
    double wave_height;
    double visibility;
    std::string weather_description;
};

struct OGC_ALERT_API ChannelAlert : Alert {
    std::string channel_id;
    std::string channel_name;
    std::string restriction_type;
    DateTime start_time;
    DateTime end_time;
};

struct OGC_ALERT_API DeviationAlert : Alert {
    double deviation_distance;
    double max_deviation;
    Coordinate nearest_waypoint;
    std::string route_id;
};

struct OGC_ALERT_API SpeedAlert : Alert {
    double current_speed;
    double speed_limit;
    double over_speed_ratio;
    std::string zone_id;
};

struct OGC_ALERT_API RestrictedAreaAlert : Alert {
    std::string area_id;
    std::string area_name;
    double distance_to_boundary;
    bool is_inside;
};

struct OGC_ALERT_API AlertConfig {
    std::string user_id;
    bool depth_alert_enabled;
    bool collision_alert_enabled;
    bool weather_alert_enabled;
    bool channel_alert_enabled;
    bool deviation_alert_enabled;
    bool speed_alert_enabled;
    bool restricted_area_alert_enabled;
    DepthAlertThreshold depth_threshold;
    CollisionAlertThreshold collision_threshold;
    DeviationThreshold deviation_threshold;
    SpeedThreshold speed_threshold;
    std::vector<PushMethod> push_methods;
};

struct OGC_ALERT_API AlertQueryParams {
    std::string user_id;
    std::vector<AlertType> alert_types;
    std::vector<AlertLevel> alert_levels;
    DateTime start_time;
    DateTime end_time;
    int page;
    int page_size;
};

struct OGC_ALERT_API AlertListResult {
    std::vector<AlertPtr> alerts;
    int total_count;
    int page;
    int page_size;
};

struct OGC_ALERT_API AcknowledgeData {
    std::string alert_id;
    std::string user_id;
    DateTime acknowledge_time;
    std::string user_action;
};

struct OGC_ALERT_API FeedbackData {
    std::string alert_id;
    std::string user_id;
    std::string feedback_type;
    std::string feedback_content;
    DateTime feedback_time;
};

inline const char* GetAlertTypeName(AlertType type) noexcept {
    switch (type) {
        case AlertType::kDepth: return "Depth";
        case AlertType::kWeather: return "Weather";
        case AlertType::kCollision: return "Collision";
        case AlertType::kChannel: return "Channel";
        case AlertType::kDeviation: return "Deviation";
        case AlertType::kSpeed: return "Speed";
        case AlertType::kRestrictedArea: return "RestrictedArea";
        default: return "Unknown";
    }
}

inline const char* GetAlertLevelName(AlertLevel level) noexcept {
    switch (level) {
        case AlertLevel::kNone: return "None";
        case AlertLevel::kLevel1: return "Level1";
        case AlertLevel::kLevel2: return "Level2";
        case AlertLevel::kLevel3: return "Level3";
        case AlertLevel::kLevel4: return "Level4";
        default: return "Unknown";
    }
}

inline const char* GetAlertStatusName(AlertStatus status) noexcept {
    switch (status) {
        case AlertStatus::kActive: return "Active";
        case AlertStatus::kAcknowledged: return "Acknowledged";
        case AlertStatus::kExpired: return "Expired";
        case AlertStatus::kDismissed: return "Dismissed";
        default: return "Unknown";
    }
}

inline const char* GetPushMethodName(PushMethod method) noexcept {
    switch (method) {
        case PushMethod::kApp: return "App";
        case PushMethod::kSound: return "Sound";
        case PushMethod::kSms: return "SMS";
        case PushMethod::kEmail: return "Email";
        default: return "Unknown";
    }
}

}
}

#endif
