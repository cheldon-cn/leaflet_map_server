#ifndef ALERT_SYSTEM_ALERT_PUSH_SERVICE_H
#define ALERT_SYSTEM_ALERT_PUSH_SERVICE_H

#include "alert/alert.h"
#include "alert/alert_types.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace alert {

enum class PushChannel {
    kWebSocket,
    kSms,
    kEmail,
    kHttp
};

struct PushResult {
    bool success;
    PushChannel channel;
    std::string messageId;
    std::string errorMessage;
    
    PushResult() : success(false), channel(PushChannel::kWebSocket) {}
};

struct PushConfig {
    bool enableWebSocket;
    bool enableSms;
    bool enableEmail;
    bool enableHttp;
    
    std::string smsGateway;
    std::string smsApiKey;
    
    std::string smtpServer;
    int smtpPort;
    std::string smtpUser;
    std::string smtpPassword;
    
    std::string httpEndpoint;
    std::string httpApiKey;
    
    int maxRetries;
    int retryIntervalMs;
    
    PushConfig()
        : enableWebSocket(true)
        , enableSms(false)
        , enableEmail(false)
        , enableHttp(false)
        , smtpPort(25)
        , maxRetries(3)
        , retryIntervalMs(1000) {}
};

class IAlertPushService {
public:
    virtual ~IAlertPushService() {}
    
    virtual PushResult Push(const Alert& alert, PushChannel channel) = 0;
    virtual std::vector<PushResult> PushToAllChannels(const Alert& alert) = 0;
    virtual std::vector<PushResult> PushToChannels(const Alert& alert, const std::vector<PushChannel>& channels) = 0;
    
    virtual void SetConfig(const PushConfig& config) = 0;
    virtual PushConfig GetConfig() const = 0;
    
    virtual void EnableChannel(PushChannel channel, bool enable) = 0;
    virtual bool IsChannelEnabled(PushChannel channel) const = 0;
    
    virtual void SetPushCallback(std::function<void(const Alert&, const PushResult&)> callback) = 0;
};

class AlertPushService : public IAlertPushService {
public:
    AlertPushService();
    virtual ~AlertPushService();
    
    PushResult Push(const Alert& alert, PushChannel channel) override;
    std::vector<PushResult> PushToAllChannels(const Alert& alert) override;
    std::vector<PushResult> PushToChannels(const Alert& alert, const std::vector<PushChannel>& channels) override;
    
    void SetConfig(const PushConfig& config) override;
    PushConfig GetConfig() const override;
    
    void EnableChannel(PushChannel channel, bool enable) override;
    bool IsChannelEnabled(PushChannel channel) const override;
    
    void SetPushCallback(std::function<void(const Alert&, const PushResult&)> callback) override;
    
    void SetWebSocketSender(std::function<bool(const std::string&)> sender);
    void SetSmsSender(std::function<bool(const std::string&, const std::string&)> sender);
    void SetEmailSender(std::function<bool(const std::string&, const std::string&, const std::string&)> sender);
    void SetHttpSender(std::function<bool(const std::string&)> sender);
    
private:
    PushConfig m_config;
    std::map<PushChannel, bool> m_enabledChannels;
    std::function<void(const Alert&, const PushResult&)> m_callback;
    
    std::function<bool(const std::string&)> m_webSocketSender;
    std::function<bool(const std::string&, const std::string&)> m_smsSender;
    std::function<bool(const std::string&, const std::string&, const std::string&)> m_emailSender;
    std::function<bool(const std::string&)> m_httpSender;
    
    PushResult PushViaWebSocket(const Alert& alert);
    PushResult PushViaSms(const Alert& alert);
    PushResult PushViaEmail(const Alert& alert);
    PushResult PushViaHttp(const Alert& alert);
    
    std::string AlertToJson(const Alert& alert) const;
    std::string FormatSmsMessage(const Alert& alert) const;
    std::string FormatEmailSubject(const Alert& alert) const;
    std::string FormatEmailBody(const Alert& alert) const;
    
    PushResult RetryPush(std::function<PushResult()> pushFunc, int maxRetries);
};

}

#endif
