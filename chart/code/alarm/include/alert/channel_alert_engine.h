#ifndef ALERT_SYSTEM_CHANNEL_ALERT_ENGINE_H
#define ALERT_SYSTEM_CHANNEL_ALERT_ENGINE_H

#include "i_alert_engine.h"
#include "coordinate.h"
#include <vector>
#include <string>

namespace alert {

struct ChannelStatus {
    std::string channelId;
    std::string channelName;
    bool isNavigable;
    double minDepth;
    double maxWidth;
    std::string status;
    std::string restriction;
    
    ChannelStatus()
        : isNavigable(true), minDepth(0), maxWidth(0) {}
};

struct NavAidStatus {
    std::string navAidId;
    std::string navAidName;
    std::string type;
    bool isOperational;
    std::string status;
    Coordinate position;
    
    NavAidStatus() : isOperational(true) {}
};

struct ChannelData {
    std::vector<ChannelStatus> channels;
    std::vector<NavAidStatus> navAids;
    std::vector<std::string> notices;
};

class ChannelAlertEngine : public IAlertEngine {
public:
    ChannelAlertEngine();
    virtual ~ChannelAlertEngine();
    
    Alert Evaluate(const AlertContext& context) override;
    
    AlertType GetType() const override;
    
    void SetThreshold(const ThresholdConfig& config) override;
    ThresholdConfig GetThreshold() const override;
    
    std::string GetName() const override;
    
    void SetChannelProvider(ChannelData (*provider)(const Coordinate&));
    
private:
    ThresholdConfig m_threshold;
    ChannelData (*m_channelProvider)(const Coordinate&);
};

}

#endif
