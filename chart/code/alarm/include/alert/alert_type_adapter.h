#ifndef ALARM_ALERT_TYPE_ADAPTER_H
#define ALARM_ALERT_TYPE_ADAPTER_H

#include "alert_types.h"
#include "alert.h"
#include "coordinate.h"
#include "ogc/alert/types.h"
#include <string>

namespace alarm {

class AlertTypeAdapter {
public:
    static ogc::alert::AlertType ToAlertType(alert::AlertType type) {
        switch (type) {
            case alert::AlertType::kDepthAlert:
                return ogc::alert::AlertType::kDepth;
            case alert::AlertType::kCollisionAlert:
                return ogc::alert::AlertType::kCollision;
            case alert::AlertType::kWeatherAlert:
                return ogc::alert::AlertType::kWeather;
            case alert::AlertType::kChannelAlert:
                return ogc::alert::AlertType::kChannel;
            case alert::AlertType::kOtherAlert:
            default:
                return ogc::alert::AlertType::kUnknown;
        }
    }
    
    static alert::AlertType FromAlertType(ogc::alert::AlertType type) {
        switch (type) {
            case ogc::alert::AlertType::kDepth:
                return alert::AlertType::kDepthAlert;
            case ogc::alert::AlertType::kCollision:
                return alert::AlertType::kCollisionAlert;
            case ogc::alert::AlertType::kWeather:
                return alert::AlertType::kWeatherAlert;
            case ogc::alert::AlertType::kChannel:
                return alert::AlertType::kChannelAlert;
            case ogc::alert::AlertType::kDeviation:
            case ogc::alert::AlertType::kSpeed:
            case ogc::alert::AlertType::kRestrictedArea:
            case ogc::alert::AlertType::kUnknown:
            default:
                return alert::AlertType::kOtherAlert;
        }
    }
    
    static ogc::alert::AlertLevel ToAlertLevel(alert::AlertLevel level) {
        switch (level) {
            case alert::AlertLevel::kLevel1_Critical:
                return ogc::alert::AlertLevel::kLevel1;
            case alert::AlertLevel::kLevel2_Severe:
                return ogc::alert::AlertLevel::kLevel2;
            case alert::AlertLevel::kLevel3_Moderate:
                return ogc::alert::AlertLevel::kLevel3;
            case alert::AlertLevel::kLevel4_Minor:
                return ogc::alert::AlertLevel::kLevel4;
            case alert::AlertLevel::kNone:
            default:
                return ogc::alert::AlertLevel::kNone;
        }
    }
    
    static alert::AlertLevel FromAlertLevel(ogc::alert::AlertLevel level) {
        switch (level) {
            case ogc::alert::AlertLevel::kLevel1:
                return alert::AlertLevel::kLevel1_Critical;
            case ogc::alert::AlertLevel::kLevel2:
                return alert::AlertLevel::kLevel2_Severe;
            case ogc::alert::AlertLevel::kLevel3:
                return alert::AlertLevel::kLevel3_Moderate;
            case ogc::alert::AlertLevel::kLevel4:
                return alert::AlertLevel::kLevel4_Minor;
            case ogc::alert::AlertLevel::kNone:
            default:
                return alert::AlertLevel::kNone;
        }
    }
    
    static ogc::alert::AlertStatus ToAlertStatus(alert::AlertStatus status) {
        switch (status) {
            case alert::AlertStatus::kPending:
                return ogc::alert::AlertStatus::kPending;
            case alert::AlertStatus::kActive:
                return ogc::alert::AlertStatus::kActive;
            case alert::AlertStatus::kPushed:
                return ogc::alert::AlertStatus::kPushed;
            case alert::AlertStatus::kAcknowledged:
                return ogc::alert::AlertStatus::kAcknowledged;
            case alert::AlertStatus::kCleared:
                return ogc::alert::AlertStatus::kCleared;
            case alert::AlertStatus::kExpired:
                return ogc::alert::AlertStatus::kExpired;
            default:
                return ogc::alert::AlertStatus::kActive;
        }
    }
    
    static alert::AlertStatus FromAlertStatus(ogc::alert::AlertStatus status) {
        switch (status) {
            case ogc::alert::AlertStatus::kPending:
                return alert::AlertStatus::kPending;
            case ogc::alert::AlertStatus::kActive:
                return alert::AlertStatus::kActive;
            case ogc::alert::AlertStatus::kPushed:
                return alert::AlertStatus::kPushed;
            case ogc::alert::AlertStatus::kAcknowledged:
                return alert::AlertStatus::kAcknowledged;
            case ogc::alert::AlertStatus::kCleared:
                return alert::AlertStatus::kCleared;
            case ogc::alert::AlertStatus::kExpired:
                return alert::AlertStatus::kExpired;
            case ogc::alert::AlertStatus::kDismissed:
                return alert::AlertStatus::kCleared;
            default:
                return alert::AlertStatus::kActive;
        }
    }
    
    static ogc::alert::AlertPtr ToAlertPtr(const alert::Alert& alarm_alert) {
        ogc::alert::AlertPtr result;
        
        auto alert_type = ToAlertType(alarm_alert.GetAlertType());
        
        switch (alert_type) {
            case ogc::alert::AlertType::kDepth: {
                auto depth_alert = std::make_shared<ogc::alert::DepthAlert>();
                depth_alert->current_depth = alarm_alert.GetDetail("depth", 0.0);
                depth_alert->ukc = alarm_alert.GetDetail("ukc", 0.0);
                depth_alert->safety_depth = alarm_alert.GetDetail("safety_depth", 0.0);
                depth_alert->tide_height = alarm_alert.GetDetail("tide_height", 0.0);
                result = depth_alert;
                break;
            }
            case ogc::alert::AlertType::kCollision: {
                auto collision_alert = std::make_shared<ogc::alert::CollisionAlert>();
                collision_alert->cpa = alarm_alert.GetDetail("cpa", 0.0);
                collision_alert->tcpa = alarm_alert.GetDetail("tcpa", 0.0);
                collision_alert->relative_bearing = alarm_alert.GetDetail("relative_bearing", 0.0);
                collision_alert->relative_speed = alarm_alert.GetDetail("relative_speed", 0.0);
                result = collision_alert;
                break;
            }
            case ogc::alert::AlertType::kWeather: {
                auto weather_alert = std::make_shared<ogc::alert::WeatherAlert>();
                weather_alert->wind_speed = alarm_alert.GetDetail("wind_speed", 0.0);
                weather_alert->wave_height = alarm_alert.GetDetail("wave_height", 0.0);
                weather_alert->visibility = alarm_alert.GetDetail("visibility", 0.0);
                result = weather_alert;
                break;
            }
            default: {
                result = std::make_shared<ogc::alert::Alert>();
                break;
            }
        }
        
        result->alert_id = alarm_alert.GetAlertId();
        result->alert_type = alert_type;
        result->alert_level = ToAlertLevel(alarm_alert.GetAlertLevel());
        result->status = ToAlertStatus(alarm_alert.GetStatus());
        result->content.title = alarm_alert.GetTitle();
        result->content.message = alarm_alert.GetMessage();
        result->content.action_required = alarm_alert.GetRecommendation();
        result->position.longitude = alarm_alert.GetPosition().longitude;
        result->position.latitude = alarm_alert.GetPosition().latitude;
        result->issue_time = ogc::alert::DateTime::FromString(alarm_alert.GetIssueTime());
        result->expire_time = ogc::alert::DateTime::FromString(alarm_alert.GetExpireTime());
        result->acknowledge_required = true;
        
        return result;
    }
    
    static alert::Alert FromAlertPtr(const ogc::alert::AlertPtr& alert_ptr) {
        alert::Alert result;
        result.SetAlertId(alert_ptr->alert_id);
        result.SetAlertType(FromAlertType(alert_ptr->alert_type));
        result.SetAlertLevel(FromAlertLevel(alert_ptr->alert_level));
        result.SetStatus(FromAlertStatus(alert_ptr->status));
        result.SetTitle(alert_ptr->content.title);
        result.SetMessage(alert_ptr->content.message);
        result.SetRecommendation(alert_ptr->content.action_required);
        result.SetPosition(alert_ptr->position.longitude, alert_ptr->position.latitude);
        result.SetIssueTime(alert_ptr->issue_time.ToString());
        result.SetExpireTime(alert_ptr->expire_time.ToString());
        
        return result;
    }
    
    static ogc::alert::Coordinate ToCoordinate(const alert::Coordinate& coord) {
        return ogc::alert::Coordinate(coord.GetLongitude(), coord.GetLatitude());
    }
    
    static alert::Coordinate FromCoordinate(const ogc::alert::Coordinate& coord) {
        alert::Coordinate result;
        result.SetLongitude(coord.longitude);
        result.SetLatitude(coord.latitude);
        return result;
    }
};

}

#endif
