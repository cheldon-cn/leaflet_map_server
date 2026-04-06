#include "ogc/alert/alert_checker.h"
#include "ogc/alert/depth_alert_checker.h"
#include "ogc/alert/collision_alert_checker.h"
#include "ogc/alert/weather_alert_checker.h"
#include "ogc/alert/channel_alert_checker.h"
#include "ogc/alert/deviation_alert_checker.h"
#include "ogc/alert/speed_alert_checker.h"
#include "ogc/alert/restricted_area_checker.h"
#include "ogc/base/log.h"
#include <mutex>

namespace ogc {
namespace alert {

class AlertCheckerBase : public IAlertChecker {
public:
    AlertCheckerBase(AlertType type) 
        : m_type(type), m_enabled(true), m_priority(0) {}
    
    std::string GetCheckerId() const override { return m_id; }
    AlertType GetAlertType() const override { return m_type; }
    int GetPriority() const override { return m_priority; }
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    
    void SetThreshold(const AlertThreshold& threshold) override {
        m_threshold = threshold;
    }
    
    AlertThreshold GetThreshold() const override {
        return m_threshold;
    }
    
protected:
    std::string m_id;
    AlertType m_type;
    bool m_enabled;
    int m_priority;
    AlertThreshold m_threshold;
};

std::unique_ptr<IAlertChecker> IAlertChecker::Create(AlertType type) {
    switch (type) {
        case AlertType::kDepth:
            return DepthAlertChecker::Create();
        case AlertType::kCollision:
            return CollisionAlertChecker::Create();
        case AlertType::kWeather:
            return WeatherAlertChecker::Create();
        case AlertType::kChannel:
            return ChannelAlertChecker::Create();
        case AlertType::kDeviation:
            return DeviationAlertChecker::Create();
        case AlertType::kSpeed:
            return SpeedAlertChecker::Create();
        case AlertType::kRestrictedArea:
            return RestrictedAreaChecker::Create();
        default:
            return nullptr;
    }
}

}
}
