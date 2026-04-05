#ifndef ALERT_SYSTEM_I_ALERT_ENGINE_H
#define ALERT_SYSTEM_I_ALERT_ENGINE_H

#include "alert.h"
#include "alert_context.h"
#include "threshold_config.h"
#include <string>

namespace alert {

class IAlertEngine {
public:
    virtual ~IAlertEngine() {}
    
    virtual Alert Evaluate(const AlertContext& context) = 0;
    
    virtual AlertType GetType() const = 0;
    
    virtual void SetThreshold(const ThresholdConfig& config) = 0;
    
    virtual ThresholdConfig GetThreshold() const = 0;
    
    virtual std::string GetName() const = 0;
};

}

#endif
