#ifndef ALERT_SYSTEM_DEPTH_ALERT_ENGINE_H
#define ALERT_SYSTEM_DEPTH_ALERT_ENGINE_H

#include "i_alert_engine.h"
#include "coordinate.h"
#include <string>

namespace alert {

class DepthAlertEngine : public IAlertEngine {
public:
    DepthAlertEngine();
    virtual ~DepthAlertEngine();
    
    Alert Evaluate(const AlertContext& context) override;
    
    AlertType GetType() const override;
    
    void SetThreshold(const ThresholdConfig& config) override;
    ThresholdConfig GetThreshold() const override;
    
    std::string GetName() const override;
    
    void SetDepthProvider(double (*provider)(const Coordinate&));
    void SetTidalProvider(double (*provider)(const Coordinate&));
    
    double CalculateSquat(double speed, double depth, double draft, double blockCoefficient);
    double CalculateUKC(double effectiveDepth, double draft, double squat);
    double CalculateEffectiveDepth(const Coordinate& position);
    
    AlertLevel DetermineAlertLevel(double ukc);
    
private:
    ThresholdConfig m_threshold;
    double (*m_depthProvider)(const Coordinate&);
    double (*m_tidalProvider)(const Coordinate&);
};

}

#endif
