#include "alert/channel_alert_engine.h"
#include "alert/alert_exception.h"

namespace alert {

ChannelAlertEngine::ChannelAlertEngine()
    : m_channelProvider(nullptr) {}

ChannelAlertEngine::~ChannelAlertEngine() {}

AlertType ChannelAlertEngine::GetType() const {
    return AlertType::kChannelAlert;
}

Alert ChannelAlertEngine::Evaluate(const AlertContext& context) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    return Alert();
}

void ChannelAlertEngine::SetThreshold(const ThresholdConfig& config) {
    m_threshold = config;
}

ThresholdConfig ChannelAlertEngine::GetThreshold() const {
    return m_threshold;
}

std::string ChannelAlertEngine::GetName() const {
    return "ChannelAlertEngine";
}

void ChannelAlertEngine::SetChannelProvider(ChannelData (*provider)(const Coordinate&)) {
    m_channelProvider = provider;
}

}
