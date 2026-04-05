#include "alert/alert_push_service.h"
#include "alert/alert_exception.h"
#include <sstream>

namespace alert {

AlertPushService::AlertPushService() {
    m_enabledChannels[PushChannel::kWebSocket] = true;
    m_enabledChannels[PushChannel::kSms] = false;
    m_enabledChannels[PushChannel::kEmail] = false;
    m_enabledChannels[PushChannel::kHttp] = false;
    
    m_webSocketSender = nullptr;
    m_smsSender = nullptr;
    m_emailSender = nullptr;
    m_httpSender = nullptr;
    m_callback = nullptr;
}

AlertPushService::~AlertPushService() {}

PushResult AlertPushService::Push(const Alert& alert, PushChannel channel) {
    if (!IsChannelEnabled(channel)) {
        PushResult result;
        result.success = false;
        result.channel = channel;
        result.errorMessage = "Channel is disabled";
        return result;
    }
    
    PushResult result;
    result.channel = channel;
    
    auto pushFunc = [this, &alert, channel]() -> PushResult {
        switch (channel) {
            case PushChannel::kWebSocket:
                return PushViaWebSocket(alert);
            case PushChannel::kSms:
                return PushViaSms(alert);
            case PushChannel::kEmail:
                return PushViaEmail(alert);
            case PushChannel::kHttp:
                return PushViaHttp(alert);
            default:
                PushResult r;
                r.success = false;
                r.channel = channel;
                r.errorMessage = "Unknown channel";
                return r;
        }
    };
    
    result = RetryPush(pushFunc, m_config.maxRetries);
    
    if (m_callback) {
        m_callback(alert, result);
    }
    
    return result;
}

std::vector<PushResult> AlertPushService::PushToAllChannels(const Alert& alert) {
    std::vector<PushChannel> channels;
    
    if (m_config.enableWebSocket) channels.push_back(PushChannel::kWebSocket);
    if (m_config.enableSms) channels.push_back(PushChannel::kSms);
    if (m_config.enableEmail) channels.push_back(PushChannel::kEmail);
    if (m_config.enableHttp) channels.push_back(PushChannel::kHttp);
    
    return PushToChannels(alert, channels);
}

std::vector<PushResult> AlertPushService::PushToChannels(const Alert& alert, const std::vector<PushChannel>& channels) {
    std::vector<PushResult> results;
    
    for (PushChannel channel : channels) {
        if (IsChannelEnabled(channel)) {
            results.push_back(Push(alert, channel));
        }
    }
    
    return results;
}

void AlertPushService::SetConfig(const PushConfig& config) {
    m_config = config;
    
    m_enabledChannels[PushChannel::kWebSocket] = config.enableWebSocket;
    m_enabledChannels[PushChannel::kSms] = config.enableSms;
    m_enabledChannels[PushChannel::kEmail] = config.enableEmail;
    m_enabledChannels[PushChannel::kHttp] = config.enableHttp;
}

PushConfig AlertPushService::GetConfig() const {
    return m_config;
}

void AlertPushService::EnableChannel(PushChannel channel, bool enable) {
    m_enabledChannels[channel] = enable;
}

bool AlertPushService::IsChannelEnabled(PushChannel channel) const {
    auto it = m_enabledChannels.find(channel);
    if (it != m_enabledChannels.end()) {
        return it->second;
    }
    return false;
}

void AlertPushService::SetPushCallback(std::function<void(const Alert&, const PushResult&)> callback) {
    m_callback = callback;
}

void AlertPushService::SetWebSocketSender(std::function<bool(const std::string&)> sender) {
    m_webSocketSender = sender;
}

void AlertPushService::SetSmsSender(std::function<bool(const std::string&, const std::string&)> sender) {
    m_smsSender = sender;
}

void AlertPushService::SetEmailSender(std::function<bool(const std::string&, const std::string&, const std::string&)> sender) {
    m_emailSender = sender;
}

void AlertPushService::SetHttpSender(std::function<bool(const std::string&)> sender) {
    m_httpSender = sender;
}

PushResult AlertPushService::PushViaWebSocket(const Alert& alert) {
    PushResult result;
    result.channel = PushChannel::kWebSocket;
    
    if (!m_webSocketSender) {
        result.success = false;
        result.errorMessage = "WebSocket sender not configured";
        return result;
    }
    
    std::string json = AlertToJson(alert);
    result.success = m_webSocketSender(json);
    
    if (result.success) {
        result.messageId = alert.GetAlertId();
    } else {
        result.errorMessage = "WebSocket send failed";
    }
    
    return result;
}

PushResult AlertPushService::PushViaSms(const Alert& alert) {
    PushResult result;
    result.channel = PushChannel::kSms;
    
    if (!m_smsSender) {
        result.success = false;
        result.errorMessage = "SMS sender not configured";
        return result;
    }
    
    std::string message = FormatSmsMessage(alert);
    std::string recipient = "";
    
    result.success = m_smsSender(recipient, message);
    
    if (result.success) {
        result.messageId = alert.GetAlertId();
    } else {
        result.errorMessage = "SMS send failed";
    }
    
    return result;
}

PushResult AlertPushService::PushViaEmail(const Alert& alert) {
    PushResult result;
    result.channel = PushChannel::kEmail;
    
    if (!m_emailSender) {
        result.success = false;
        result.errorMessage = "Email sender not configured";
        return result;
    }
    
    std::string subject = FormatEmailSubject(alert);
    std::string body = FormatEmailBody(alert);
    std::string recipient = "";
    
    result.success = m_emailSender(recipient, subject, body);
    
    if (result.success) {
        result.messageId = alert.GetAlertId();
    } else {
        result.errorMessage = "Email send failed";
    }
    
    return result;
}

PushResult AlertPushService::PushViaHttp(const Alert& alert) {
    PushResult result;
    result.channel = PushChannel::kHttp;
    
    if (!m_httpSender) {
        result.success = false;
        result.errorMessage = "HTTP sender not configured";
        return result;
    }
    
    std::string json = AlertToJson(alert);
    result.success = m_httpSender(json);
    
    if (result.success) {
        result.messageId = alert.GetAlertId();
    } else {
        result.errorMessage = "HTTP send failed";
    }
    
    return result;
}

std::string AlertPushService::AlertToJson(const Alert& alert) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"alertId\":\"" << alert.GetAlertId() << "\",";
    oss << "\"alertType\":\"" << AlertTypeToString(alert.GetAlertType()) << "\",";
    oss << "\"alertLevel\":\"" << AlertLevelToString(alert.GetAlertLevel()) << "\",";
    oss << "\"title\":\"" << alert.GetTitle() << "\",";
    oss << "\"message\":\"" << alert.GetMessage() << "\",";
    oss << "\"status\":\"" << AlertStatusToString(alert.GetStatus()) << "\"";
    oss << "}";
    return oss.str();
}

std::string AlertPushService::FormatSmsMessage(const Alert& alert) const {
    std::ostringstream oss;
    oss << "[" << AlertLevelToString(alert.GetAlertLevel()) << "] ";
    oss << alert.GetTitle() << ": " << alert.GetMessage();
    return oss.str();
}

std::string AlertPushService::FormatEmailSubject(const Alert& alert) const {
    std::ostringstream oss;
    oss << "[" << AlertLevelToString(alert.GetAlertLevel()) << "] ";
    oss << alert.GetTitle();
    return oss.str();
}

std::string AlertPushService::FormatEmailBody(const Alert& alert) const {
    std::ostringstream oss;
    oss << "Alert ID: " << alert.GetAlertId() << "\n";
    oss << "Type: " << AlertTypeToString(alert.GetAlertType()) << "\n";
    oss << "Level: " << AlertLevelToString(alert.GetAlertLevel()) << "\n";
    oss << "Title: " << alert.GetTitle() << "\n";
    oss << "Message: " << alert.GetMessage() << "\n";
    oss << "Recommendation: " << alert.GetRecommendation() << "\n";
    return oss.str();
}

PushResult AlertPushService::RetryPush(std::function<PushResult()> pushFunc, int maxRetries) {
    PushResult result;
    
    for (int i = 0; i <= maxRetries; ++i) {
        result = pushFunc();
        if (result.success) {
            return result;
        }
        
        if (i < maxRetries) {
        }
    }
    
    return result;
}

}
