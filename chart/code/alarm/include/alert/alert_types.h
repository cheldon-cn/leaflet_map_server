#ifndef ALERT_SYSTEM_ALERT_TYPES_H
#define ALERT_SYSTEM_ALERT_TYPES_H

#include <string>
#include <cstdint>

namespace alert {

enum class AlertType {
    kDepthAlert = 0,
    kCollisionAlert = 1,
    kWeatherAlert = 2,
    kChannelAlert = 3,
    kOtherAlert = 4
};

enum class AlertLevel {
    kLevel1_Critical = 1,
    kLevel2_Severe = 2,
    kLevel3_Moderate = 3,
    kLevel4_Minor = 4,
    kNone = 0
};

enum class AlertStatus {
    kPending = 0,
    kActive = 1,
    kPushed = 2,
    kAcknowledged = 3,
    kCleared = 4,
    kExpired = 5
};

enum class ShipType {
    kUnknown = 0,
    kCargo = 1,
    kTanker = 2,
    kPassenger = 3,
    kFishing = 4,
    kTug = 5,
    kPilot = 6,
    kSearchRescue = 7,
    kPleasure = 8,
    kHighSpeed = 9,
    kOther = 10
};

enum class PushMethod {
    kApp = 0,
    kSms = 1,
    kEmail = 2,
    kSound = 3,
    kScreen = 4
};

struct AlertConstants {
    static constexpr double kMinUKC = 0.3;
    static constexpr double kDefaultCPAThreshold = 0.5;
    static constexpr double kDefaultTCPAThreshold = 6.0;
    static constexpr double kNauticalMileToMeters = 1852.0;
    static constexpr double kKnotsToMs = 0.514444;
    static constexpr double kMetersPerDegreeLat = 110574.0;
    static constexpr double kEarthRadiusMeters = 6371000.0;
    static constexpr int kMaxRetryCount = 3;
    static constexpr int kDefaultTimeoutMs = 5000;
};

std::string AlertTypeToString(AlertType type);
std::string AlertLevelToString(AlertLevel level);
std::string AlertStatusToString(AlertStatus status);
std::string ShipTypeToString(ShipType type);

AlertType StringToAlertType(const std::string& str);
AlertLevel StringToAlertLevel(const std::string& str);
AlertStatus StringToAlertStatus(const std::string& str);
ShipType StringToShipType(const std::string& str);

int AlertLevelToPriority(AlertLevel level);

}

#endif
