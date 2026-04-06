#include "alert/push_channel_adapter.h"
#include <algorithm>

namespace alarm {

PushChannelAdapter::PushChannelAdapter() {
    m_enabledChannels[alert::PushChannel::kWebSocket] = true;
    m_enabledChannels[alert::PushChannel::kSms] = false;
    m_enabledChannels[alert::PushChannel::kEmail] = false;
    m_enabledChannels[alert::PushChannel::kHttp] = false;
}

PushChannelAdapter::~PushChannelAdapter() {
}

void PushChannelAdapter::SetPushService(std::shared_ptr<ogc::alert::IPushService> service) {
    m_pushService = service;
}

std::shared_ptr<ogc::alert::IPushService> PushChannelAdapter::GetPushService() const {
    return m_pushService;
}

alert::PushResult PushChannelAdapter::Push(const alert::Alert& alert, alert::PushChannel channel) {
    if (!m_pushService) {
        alert::PushResult result;
        result.success = false;
        result.channel = channel;
        result.errorMessage = "Push service not initialized";
        return result;
    }
    
    ogc::alert::AlertPtr alert_ptr = AlertTypeAdapter::ToAlertPtr(alert);
    ogc::alert::PushMethod method = ConvertChannel(channel);
    
    std::vector<ogc::alert::PushResult> results = m_pushService->PushByMethod(
        alert_ptr, {alert.GetAlertId()}, method);
    
    if (!results.empty()) {
        return ConvertResult(results[0]);
    }
    
    alert::PushResult result;
    result.success = false;
    result.channel = channel;
    result.errorMessage = "No push result returned";
    return result;
}

std::vector<alert::PushResult> PushChannelAdapter::PushToAllChannels(const alert::Alert& alert) {
    std::vector<alert::PushResult> results;
    
    for (const auto& pair : m_enabledChannels) {
        if (pair.second) {
            results.push_back(Push(alert, pair.first));
        }
    }
    
    return results;
}

std::vector<alert::PushResult> PushChannelAdapter::PushToChannels(
    const alert::Alert& alert, const std::vector<alert::PushChannel>& channels) {
    
    std::vector<alert::PushResult> results;
    
    for (alert::PushChannel channel : channels) {
        results.push_back(Push(alert, channel));
    }
    
    return results;
}

void PushChannelAdapter::SetConfig(const alert::PushConfig& config) {
    m_config = config;
    
    if (m_pushService) {
        ogc::alert::PushConfig ogc_config;
        ogc_config.max_retry_count = config.maxRetries;
        ogc_config.retry_interval_ms = config.retryIntervalMs;
        ogc_config.enable_retry = true;
        ogc_config.enable_logging = true;
        m_pushService->SetConfig(ogc_config);
    }
    
    m_enabledChannels[alert::PushChannel::kWebSocket] = config.enableWebSocket;
    m_enabledChannels[alert::PushChannel::kSms] = config.enableSms;
    m_enabledChannels[alert::PushChannel::kEmail] = config.enableEmail;
    m_enabledChannels[alert::PushChannel::kHttp] = config.enableHttp;
}

alert::PushConfig PushChannelAdapter::GetConfig() const {
    return m_config;
}

void PushChannelAdapter::EnableChannel(alert::PushChannel channel, bool enable) {
    m_enabledChannels[channel] = enable;
}

bool PushChannelAdapter::IsChannelEnabled(alert::PushChannel channel) const {
    auto it = m_enabledChannels.find(channel);
    if (it != m_enabledChannels.end()) {
        return it->second;
    }
    return false;
}

void PushChannelAdapter::SetPushCallback(
    std::function<void(const alert::Alert&, const alert::PushResult&)> callback) {
    m_callback = callback;
    
    if (m_pushService) {
        m_pushService->SetPushCallback(
            [this](const ogc::alert::Alert& alert, const ogc::alert::PushResult& result) {
                if (m_callback) {
                    alert::Alert alarm_alert = AlertTypeAdapter::FromAlertPtr(
                        std::make_shared<ogc::alert::Alert>(alert));
                    m_callback(alarm_alert, ConvertResult(result));
                }
            });
    }
}

ogc::alert::PushMethod PushChannelAdapter::ConvertChannel(alert::PushChannel channel) {
    switch (channel) {
        case alert::PushChannel::kWebSocket:
            return ogc::alert::PushMethod::kApp;
        case alert::PushChannel::kSms:
            return ogc::alert::PushMethod::kSms;
        case alert::PushChannel::kEmail:
            return ogc::alert::PushMethod::kEmail;
        case alert::PushChannel::kHttp:
            return ogc::alert::PushMethod::kApp;
        default:
            return ogc::alert::PushMethod::kApp;
    }
}

alert::PushChannel PushChannelAdapter::ConvertMethod(ogc::alert::PushMethod method) {
    switch (method) {
        case ogc::alert::PushMethod::kApp:
            return alert::PushChannel::kWebSocket;
        case ogc::alert::PushMethod::kSms:
            return alert::PushChannel::kSms;
        case ogc::alert::PushMethod::kEmail:
            return alert::PushChannel::kEmail;
        case ogc::alert::PushMethod::kSound:
        default:
            return alert::PushChannel::kWebSocket;
    }
}

alert::PushResult PushChannelAdapter::ConvertResult(const ogc::alert::PushResult& result) {
    alert::PushResult converted;
    converted.success = result.success;
    converted.messageId = result.push_id;
    converted.errorMessage = result.error_message;
    return converted;
}

ogc::alert::IPushChannelPtr CreateChannelAdapter(alert::PushChannel channel) {
    switch (channel) {
        case alert::PushChannel::kWebSocket:
            return std::make_shared<WebSocketChannelAdapter>();
        case alert::PushChannel::kSms:
            return std::make_shared<SmsChannelAdapter>();
        case alert::PushChannel::kEmail:
            return std::make_shared<EmailChannelAdapter>();
        case alert::PushChannel::kHttp:
            return std::make_shared<HttpChannelAdapter>();
        default:
            return nullptr;
    }
}

ogc::alert::PushMethod WebSocketChannelAdapter::GetMethod() const {
    return ogc::alert::PushMethod::kApp;
}

bool WebSocketChannelAdapter::IsAvailable() const {
    return m_sender != nullptr;
}

ogc::alert::PushResult WebSocketChannelAdapter::Send(
    const ogc::alert::AlertPtr& alert, const std::string& user_id) {
    
    ogc::alert::PushResult result;
    result.push_id = alert->alert_id + "_ws";
    result.push_time = ogc::alert::DateTime::Now();
    
    if (!m_sender) {
        result.success = false;
        result.error_message = "WebSocket sender not configured";
        return result;
    }
    
    std::string message = alert->content.message;
    result.success = m_sender(message);
    
    if (!result.success) {
        result.error_message = "WebSocket push failed";
    }
    
    return result;
}

void WebSocketChannelAdapter::SetSender(std::function<bool(const std::string&)> sender) {
    m_sender = sender;
}

ogc::alert::PushMethod SmsChannelAdapter::GetMethod() const {
    return ogc::alert::PushMethod::kSms;
}

bool SmsChannelAdapter::IsAvailable() const {
    return m_sender != nullptr;
}

ogc::alert::PushResult SmsChannelAdapter::Send(
    const ogc::alert::AlertPtr& alert, const std::string& user_id) {
    
    ogc::alert::PushResult result;
    result.push_id = alert->alert_id + "_sms";
    result.push_time = ogc::alert::DateTime::Now();
    
    if (!m_sender) {
        result.success = false;
        result.error_message = "SMS sender not configured";
        return result;
    }
    
    std::string phone = user_id;
    std::string message = alert->content.message;
    result.success = m_sender(phone, message);
    
    if (!result.success) {
        result.error_message = "SMS push failed";
    }
    
    return result;
}

void SmsChannelAdapter::SetSender(std::function<bool(const std::string&, const std::string&)> sender) {
    m_sender = sender;
}

ogc::alert::PushMethod EmailChannelAdapter::GetMethod() const {
    return ogc::alert::PushMethod::kEmail;
}

bool EmailChannelAdapter::IsAvailable() const {
    return m_sender != nullptr;
}

ogc::alert::PushResult EmailChannelAdapter::Send(
    const ogc::alert::AlertPtr& alert, const std::string& user_id) {
    
    ogc::alert::PushResult result;
    result.push_id = alert->alert_id + "_email";
    result.push_time = ogc::alert::DateTime::Now();
    
    if (!m_sender) {
        result.success = false;
        result.error_message = "Email sender not configured";
        return result;
    }
    
    std::string email = user_id;
    std::string subject = alert->content.title;
    std::string body = alert->content.message;
    result.success = m_sender(email, subject, body);
    
    if (!result.success) {
        result.error_message = "Email push failed";
    }
    
    return result;
}

void EmailChannelAdapter::SetSender(
    std::function<bool(const std::string&, const std::string&, const std::string&)> sender) {
    m_sender = sender;
}

ogc::alert::PushMethod HttpChannelAdapter::GetMethod() const {
    return ogc::alert::PushMethod::kApp;
}

bool HttpChannelAdapter::IsAvailable() const {
    return m_sender != nullptr;
}

ogc::alert::PushResult HttpChannelAdapter::Send(
    const ogc::alert::AlertPtr& alert, const std::string& user_id) {
    
    ogc::alert::PushResult result;
    result.push_id = alert->alert_id + "_http";
    result.push_time = ogc::alert::DateTime::Now();
    
    if (!m_sender) {
        result.success = false;
        result.error_message = "HTTP sender not configured";
        return result;
    }
    
    std::string payload = alert->content.message;
    result.success = m_sender(payload);
    
    if (!result.success) {
        result.error_message = "HTTP push failed";
    }
    
    return result;
}

void HttpChannelAdapter::SetSender(std::function<bool(const std::string&)> sender) {
    m_sender = sender;
}

}
