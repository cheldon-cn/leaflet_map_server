#include "alert/alert_types.h"

namespace alert {

std::string AlertTypeToString(AlertType type) {
    switch (type) {
        case AlertType::kDepthAlert: return "depth";
        case AlertType::kCollisionAlert: return "collision";
        case AlertType::kWeatherAlert: return "weather";
        case AlertType::kChannelAlert: return "channel";
        case AlertType::kOtherAlert: return "other";
        default: return "unknown";
    }
}

std::string AlertLevelToString(AlertLevel level) {
    switch (level) {
        case AlertLevel::kLevel1_Critical: return "critical";
        case AlertLevel::kLevel2_Severe: return "severe";
        case AlertLevel::kLevel3_Moderate: return "moderate";
        case AlertLevel::kLevel4_Minor: return "minor";
        case AlertLevel::kNone: return "none";
        default: return "unknown";
    }
}

std::string AlertStatusToString(AlertStatus status) {
    switch (status) {
        case AlertStatus::kPending: return "pending";
        case AlertStatus::kActive: return "active";
        case AlertStatus::kPushed: return "pushed";
        case AlertStatus::kAcknowledged: return "acknowledged";
        case AlertStatus::kCleared: return "cleared";
        case AlertStatus::kExpired: return "expired";
        default: return "unknown";
    }
}

std::string ShipTypeToString(ShipType type) {
    switch (type) {
        case ShipType::kUnknown: return "unknown";
        case ShipType::kCargo: return "cargo";
        case ShipType::kTanker: return "tanker";
        case ShipType::kPassenger: return "passenger";
        case ShipType::kFishing: return "fishing";
        case ShipType::kTug: return "tug";
        case ShipType::kPilot: return "pilot";
        case ShipType::kSearchRescue: return "sar";
        case ShipType::kPleasure: return "pleasure";
        case ShipType::kHighSpeed: return "hsc";
        case ShipType::kOther: return "other";
        default: return "unknown";
    }
}

AlertType StringToAlertType(const std::string& str) {
    if (str == "depth") return AlertType::kDepthAlert;
    if (str == "collision") return AlertType::kCollisionAlert;
    if (str == "weather") return AlertType::kWeatherAlert;
    if (str == "channel") return AlertType::kChannelAlert;
    if (str == "other") return AlertType::kOtherAlert;
    return AlertType::kOtherAlert;
}

AlertLevel StringToAlertLevel(const std::string& str) {
    if (str == "critical") return AlertLevel::kLevel1_Critical;
    if (str == "severe") return AlertLevel::kLevel2_Severe;
    if (str == "moderate") return AlertLevel::kLevel3_Moderate;
    if (str == "minor") return AlertLevel::kLevel4_Minor;
    return AlertLevel::kNone;
}

AlertStatus StringToAlertStatus(const std::string& str) {
    if (str == "pending") return AlertStatus::kPending;
    if (str == "active") return AlertStatus::kActive;
    if (str == "pushed") return AlertStatus::kPushed;
    if (str == "acknowledged") return AlertStatus::kAcknowledged;
    if (str == "cleared") return AlertStatus::kCleared;
    if (str == "expired") return AlertStatus::kExpired;
    return AlertStatus::kPending;
}

ShipType StringToShipType(const std::string& str) {
    if (str == "cargo") return ShipType::kCargo;
    if (str == "tanker") return ShipType::kTanker;
    if (str == "passenger") return ShipType::kPassenger;
    if (str == "fishing") return ShipType::kFishing;
    if (str == "tug") return ShipType::kTug;
    if (str == "pilot") return ShipType::kPilot;
    if (str == "sar") return ShipType::kSearchRescue;
    if (str == "pleasure") return ShipType::kPleasure;
    if (str == "hsc") return ShipType::kHighSpeed;
    if (str == "other") return ShipType::kOther;
    return ShipType::kUnknown;
}

int AlertLevelToPriority(AlertLevel level) {
    switch (level) {
        case AlertLevel::kLevel1_Critical: return 1;
        case AlertLevel::kLevel2_Severe: return 2;
        case AlertLevel::kLevel3_Moderate: return 3;
        case AlertLevel::kLevel4_Minor: return 4;
        default: return 5;
    }
}

}
