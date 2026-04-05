#include "alert/threshold_config.h"

namespace alert {

ThresholdConfig::ThresholdConfig()
    : m_enabled(true)
    , m_level1Threshold(0.3)
    , m_level2Threshold(0.5)
    , m_level3Threshold(1.0)
    , m_level4Threshold(1.5)
    , m_warningTime(600)
    , m_clearDelay(30) {}

ThresholdConfig::ThresholdConfig(const std::string& name)
    : m_name(name)
    , m_enabled(true)
    , m_level1Threshold(0.3)
    , m_level2Threshold(0.5)
    , m_level3Threshold(1.0)
    , m_level4Threshold(1.5)
    , m_warningTime(600)
    , m_clearDelay(30) {}

void ThresholdConfig::SetParameter(const std::string& key, double value) {
    m_parameters[key] = value;
}

double ThresholdConfig::GetParameter(const std::string& key, double defaultValue) const {
    auto it = m_parameters.find(key);
    if (it != m_parameters.end()) {
        return it->second;
    }
    return defaultValue;
}

bool ThresholdConfig::HasParameter(const std::string& key) const {
    return m_parameters.find(key) != m_parameters.end();
}

bool ThresholdConfig::IsValid() const {
    return m_level1Threshold < m_level2Threshold &&
           m_level2Threshold < m_level3Threshold &&
           m_level3Threshold < m_level4Threshold;
}

}
