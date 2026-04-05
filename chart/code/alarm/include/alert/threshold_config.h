#ifndef ALERT_SYSTEM_THRESHOLD_CONFIG_H
#define ALERT_SYSTEM_THRESHOLD_CONFIG_H

#include <vector>
#include <map>
#include <string>

namespace alert {

class ThresholdConfig {
public:
    ThresholdConfig();
    explicit ThresholdConfig(const std::string& name);
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    double GetLevel1Threshold() const { return m_level1Threshold; }
    void SetLevel1Threshold(double value) { m_level1Threshold = value; }
    
    double GetLevel2Threshold() const { return m_level2Threshold; }
    void SetLevel2Threshold(double value) { m_level2Threshold = value; }
    
    double GetLevel3Threshold() const { return m_level3Threshold; }
    void SetLevel3Threshold(double value) { m_level3Threshold = value; }
    
    double GetLevel4Threshold() const { return m_level4Threshold; }
    void SetLevel4Threshold(double value) { m_level4Threshold = value; }
    
    double GetWarningTime() const { return m_warningTime; }
    void SetWarningTime(double time) { m_warningTime = time; }
    
    double GetClearDelay() const { return m_clearDelay; }
    void SetClearDelay(double delay) { m_clearDelay = delay; }
    
    void SetParameter(const std::string& key, double value);
    double GetParameter(const std::string& key, double defaultValue = 0.0) const;
    bool HasParameter(const std::string& key) const;
    
    bool IsValid() const;
    
private:
    std::string m_name;
    bool m_enabled;
    double m_level1Threshold;
    double m_level2Threshold;
    double m_level3Threshold;
    double m_level4Threshold;
    double m_warningTime;
    double m_clearDelay;
    std::map<std::string, double> m_parameters;
};

}
#endif
