#ifndef ALARM_PUSH_CHANNEL_ADAPTER_H
#define ALARM_PUSH_CHANNEL_ADAPTER_H

#include "alert_push_service.h"
#include "ogc/alert/push_service.h"
#include "alert_type_adapter.h"
#include <memory>
#include <map>

namespace alarm {

class PushChannelAdapter : public alert::IAlertPushService {
public:
    PushChannelAdapter();
    ~PushChannelAdapter() override;
    
    void SetPushService(std::shared_ptr<ogc::alert::IPushService> service);
    std::shared_ptr<ogc::alert::IPushService> GetPushService() const;
    
    alert::PushResult Push(const alert::Alert& alert, alert::PushChannel channel) override;
    std::vector<alert::PushResult> PushToAllChannels(const alert::Alert& alert) override;
    std::vector<alert::PushResult> PushToChannels(const alert::Alert& alert, const std::vector<alert::PushChannel>& channels) override;
    
    void SetConfig(const alert::PushConfig& config) override;
    alert::PushConfig GetConfig() const override;
    
    void EnableChannel(alert::PushChannel channel, bool enable) override;
    bool IsChannelEnabled(alert::PushChannel channel) const override;
    
    void SetPushCallback(std::function<void(const alert::Alert&, const alert::PushResult&)> callback) override;
    
private:
    ogc::alert::PushMethod ConvertChannel(alert::PushChannel channel);
    alert::PushChannel ConvertMethod(ogc::alert::PushMethod method);
    alert::PushResult ConvertResult(const ogc::alert::PushResult& result);
    
    std::shared_ptr<ogc::alert::IPushService> m_pushService;
    alert::PushConfig m_config;
    std::map<alert::PushChannel, bool> m_enabledChannels;
    std::function<void(const alert::Alert&, const alert::PushResult&)> m_callback;
};

ogc::alert::IPushChannelPtr CreateChannelAdapter(alert::PushChannel channel);

class WebSocketChannelAdapter : public ogc::alert::IPushChannel {
public:
    ogc::alert::PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    ogc::alert::PushResult Send(const ogc::alert::AlertPtr& alert, const std::string& user_id) override;
    
    void SetSender(std::function<bool(const std::string&)> sender);
    
private:
    std::function<bool(const std::string&)> m_sender;
};

class SmsChannelAdapter : public ogc::alert::IPushChannel {
public:
    ogc::alert::PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    ogc::alert::PushResult Send(const ogc::alert::AlertPtr& alert, const std::string& user_id) override;
    
    void SetSender(std::function<bool(const std::string&, const std::string&)> sender);
    
private:
    std::function<bool(const std::string&, const std::string&)> m_sender;
};

class EmailChannelAdapter : public ogc::alert::IPushChannel {
public:
    ogc::alert::PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    ogc::alert::PushResult Send(const ogc::alert::AlertPtr& alert, const std::string& user_id) override;
    
    void SetSender(std::function<bool(const std::string&, const std::string&, const std::string&)> sender);
    
private:
    std::function<bool(const std::string&, const std::string&, const std::string&)> m_sender;
};

class HttpChannelAdapter : public ogc::alert::IPushChannel {
public:
    ogc::alert::PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    ogc::alert::PushResult Send(const ogc::alert::AlertPtr& alert, const std::string& user_id) override;
    
    void SetSender(std::function<bool(const std::string&)> sender);
    
private:
    std::function<bool(const std::string&)> m_sender;
};

}

#endif
