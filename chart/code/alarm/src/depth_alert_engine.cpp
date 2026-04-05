#include "alert/depth_alert_engine.h"
#include "alert/alert_exception.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace alert {

DepthAlertEngine::DepthAlertEngine()
    : m_depthProvider(nullptr)
    , m_tidalProvider(nullptr) {}

DepthAlertEngine::~DepthAlertEngine() {}

AlertType DepthAlertEngine::GetType() const {
    return AlertType::kDepthAlert;
}

Alert DepthAlertEngine::Evaluate(const AlertContext& context) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    const ShipInfo& ship = context.GetShipInfo();
    const Coordinate& position = context.GetPosition();
    double speed = context.GetSpeed();
    
    double chartDepth = CalculateEffectiveDepth(position);
    double tidalHeight = 0.0;
    if (m_tidalProvider) {
        tidalHeight = m_tidalProvider(position);
    }
    
    double effectiveDepth = chartDepth + tidalHeight;
    double squat = CalculateSquat(speed, effectiveDepth, ship.GetDraft(), ship.GetBlockCoefficient());
    double ukc = CalculateUKC(effectiveDepth, ship.GetDraft(), squat);
    
    AlertLevel level = DetermineAlertLevel(ukc);
    
    if (level == AlertLevel::kNone) {
        return Alert();
    }
    
    Alert alert(Alert::GenerateAlertId(AlertType::kDepthAlert), AlertType::kDepthAlert, level);
    alert.SetTitle("Depth Alert");
    alert.SetMessage("Under Keel Clearance warning");
    alert.SetPosition(position.GetLongitude(), position.GetLatitude());
    
    alert.SetDetail("chart_depth", chartDepth);
    alert.SetDetail("tidal_height", tidalHeight);
    alert.SetDetail("effective_depth", effectiveDepth);
    alert.SetDetail("ship_draft", ship.GetDraft());
    alert.SetDetail("squat", squat);
    alert.SetDetail("ukc", ukc);
    
    return alert;
}

void DepthAlertEngine::SetThreshold(const ThresholdConfig& config) {
    m_threshold = config;
}

ThresholdConfig DepthAlertEngine::GetThreshold() const {
    return m_threshold;
}

std::string DepthAlertEngine::GetName() const {
    return "DepthAlertEngine";
}

double DepthAlertEngine::CalculateEffectiveDepth(const Coordinate& position) {
    if (m_depthProvider) {
        return m_depthProvider(position);
    }
    return 10.0;
}

double DepthAlertEngine::CalculateSquat(double speed, double depth, double draft, double cb) {
    double speedMs = speed * AlertConstants::kKnotsToMs;
    double squat = cb * speed * speed / 100.0;
    
    if (depth > 0 && draft > 0) {
        double shallowFactor = 1.0 + 1.0 / (15.0 * depth / draft - 1.0);
        squat *= shallowFactor;
    }
    
    return squat;
}

double DepthAlertEngine::CalculateUKC(double effectiveDepth, double draft, double squat) {
    return effectiveDepth - draft - squat;
}

AlertLevel DepthAlertEngine::DetermineAlertLevel(double ukc) {
    double level1 = m_threshold.GetLevel1Threshold();
    double level2 = m_threshold.GetLevel2Threshold();
    double level3 = m_threshold.GetLevel3Threshold();
    double level4 = m_threshold.GetLevel4Threshold();
    
    if (ukc < level1) return AlertLevel::kLevel1_Critical;
    if (ukc < level2) return AlertLevel::kLevel2_Severe;
    if (ukc < level3) return AlertLevel::kLevel3_Moderate;
    if (ukc < level4) return AlertLevel::kLevel4_Minor;
    
    return AlertLevel::kNone;
}

void DepthAlertEngine::SetDepthProvider(double (*provider)(const Coordinate&)) {
    m_depthProvider = provider;
}

void DepthAlertEngine::SetTidalProvider(double (*provider)(const Coordinate&)) {
    m_tidalProvider = provider;
}

}
