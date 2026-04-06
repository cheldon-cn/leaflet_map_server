#include "ogc/alert/push_service.h"
#include "ogc/draw/log.h"
#include <map>
#include <mutex>
#include <vector>

using ogc::draw::LogLevel;
using ogc::draw::LogHelper;

namespace ogc {
namespace alert {

class PushService::Impl {
public:
    void RegisterChannel(IPushChannelPtr channel) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (channel) {
            m_channels[channel->GetMethod()] = channel;
        }
    }
    
    void UnregisterChannel(PushMethod method) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_channels.erase(method);
    }
    
    IPushChannelPtr GetChannel(PushMethod method) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_channels.find(method);
        if (it != m_channels.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void SetPushStrategy(const std::vector<PushMethod>& level1,
                         const std::vector<PushMethod>& level2,
                         const std::vector<PushMethod>& level3,
                         const std::vector<PushMethod>& level4) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_level1Methods = level1;
        m_level2Methods = level2;
        m_level3Methods = level3;
        m_level4Methods = level4;
    }
    
    void SetConfig(const PushConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_config = config;
        m_retryCount = config.max_retry_count;
        m_retryIntervalMs = config.retry_interval_ms;
    }
    
    PushConfig GetConfig() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_config;
    }
    
    void SetPushCallback(PushCallback callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callback = callback;
    }
    
    std::vector<PushResult> Push(const AlertPtr& alert, const std::vector<std::string>& user_ids) {
        std::vector<PushResult> results;
        std::vector<PushMethod> methods;
        
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            switch (alert->alert_level) {
                case AlertLevel::kLevel1:
                    methods = m_level1Methods;
                    break;
                case AlertLevel::kLevel2:
                    methods = m_level2Methods;
                    break;
                case AlertLevel::kLevel3:
                    methods = m_level3Methods;
                    break;
                case AlertLevel::kLevel4:
                    methods = m_level4Methods;
                    break;
                default:
                    return results;
            }
        }
        
        for (PushMethod method : methods) {
            auto channelResults = PushByMethodInternal(alert, user_ids, method);
            results.insert(results.end(), channelResults.begin(), channelResults.end());
        }
        
        return results;
    }
    
    std::vector<PushResult> PushByMethod(const AlertPtr& alert,
                                         const std::vector<std::string>& user_ids,
                                         PushMethod method) {
        return PushByMethodInternal(alert, user_ids, method);
    }
    
    void SetRetryCount(int count) {
        m_retryCount = count;
    }
    
    void SetRetryInterval(int interval_ms) {
        m_retryIntervalMs = interval_ms;
    }
    
    std::vector<PushRecord> GetPushHistory(const std::string& alert_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<PushRecord> result;
        for (const auto& record : m_history) {
            if (record.alert_id == alert_id) {
                result.push_back(record);
            }
        }
        return result;
    }
    
private:
    std::vector<PushResult> PushByMethodInternal(const AlertPtr& alert,
                                                  const std::vector<std::string>& user_ids,
                                                  PushMethod method) {
        std::vector<PushResult> results;
        
        IPushChannelPtr channel;
        PushCallback callback;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_channels.find(method);
            if (it != m_channels.end()) {
                channel = it->second;
            }
            callback = m_callback;
        }
        
        if (!channel || !channel->IsAvailable()) {
            for (const auto& user_id : user_ids) {
                PushResult result;
                result.success = false;
                result.error_message = "Channel not available";
                results.push_back(result);
                if (callback) {
                    callback(*alert, result);
                }
            }
            return results;
        }
        
        for (const auto& user_id : user_ids) {
            PushResult result;
            for (int retry = 0; retry <= m_retryCount; ++retry) {
                result = channel->Send(alert, user_id);
                if (result.success) {
                    break;
                }
                if (retry < m_retryCount) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(m_retryIntervalMs));
                }
            }
            results.push_back(result);
            
            PushRecord record;
            record.push_id = result.push_id;
            record.alert_id = alert->alert_id;
            record.method = method;
            record.target_user = user_id;
            record.push_time = result.push_time;
            record.success = result.success;
            record.error_message = result.error_message;
            
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_history.push_back(record);
            }
            
            if (callback) {
                callback(*alert, result);
            }
        }
        
        return results;
    }
    
    std::map<PushMethod, IPushChannelPtr> m_channels;
    std::vector<PushMethod> m_level1Methods;
    std::vector<PushMethod> m_level2Methods;
    std::vector<PushMethod> m_level3Methods;
    std::vector<PushMethod> m_level4Methods;
    std::vector<PushRecord> m_history;
    mutable std::mutex m_mutex;
    int m_retryCount = 3;
    int m_retryIntervalMs = 1000;
    PushConfig m_config;
    PushCallback m_callback;
};

PushService::PushService() 
    : m_impl(std::make_unique<Impl>()) {
}

PushService::~PushService() {
}

void PushService::RegisterChannel(IPushChannelPtr channel) {
    m_impl->RegisterChannel(channel);
}

void PushService::UnregisterChannel(PushMethod method) {
    m_impl->UnregisterChannel(method);
}

IPushChannelPtr PushService::GetChannel(PushMethod method) const {
    return m_impl->GetChannel(method);
}

void PushService::SetPushStrategy(const std::vector<PushMethod>& level1_methods,
                                   const std::vector<PushMethod>& level2_methods,
                                   const std::vector<PushMethod>& level3_methods,
                                   const std::vector<PushMethod>& level4_methods) {
    m_impl->SetPushStrategy(level1_methods, level2_methods, level3_methods, level4_methods);
}

void PushService::SetConfig(const PushConfig& config) {
    m_impl->SetConfig(config);
}

PushConfig PushService::GetConfig() const {
    return m_impl->GetConfig();
}

void PushService::SetPushCallback(PushCallback callback) {
    m_impl->SetPushCallback(callback);
}

std::vector<PushResult> PushService::Push(const AlertPtr& alert, const std::vector<std::string>& user_ids) {
    return m_impl->Push(alert, user_ids);
}

std::vector<PushResult> PushService::PushByMethod(const AlertPtr& alert,
                                                   const std::vector<std::string>& user_ids,
                                                   PushMethod method) {
    return m_impl->PushByMethod(alert, user_ids, method);
}

void PushService::SetRetryCount(int count) {
    m_impl->SetRetryCount(count);
}

void PushService::SetRetryInterval(int interval_ms) {
    m_impl->SetRetryInterval(interval_ms);
}

std::vector<PushRecord> PushService::GetPushHistory(const std::string& alert_id) {
    return m_impl->GetPushHistory(alert_id);
}

std::unique_ptr<IPushService> IPushService::Create() {
    return std::unique_ptr<IPushService>(new PushService());
}

PushMethod AppPushChannel::GetMethod() const {
    return PushMethod::kApp;
}

bool AppPushChannel::IsAvailable() const {
    return !m_serverUrl.empty();
}

PushResult AppPushChannel::Send(const AlertPtr& alert, const std::string& user_id) {
    PushResult result;
    result.push_time = DateTime::Now();
    
    if (!IsAvailable()) {
        result.success = false;
        result.error_message = "App push server not configured";
        return result;
    }
    
    result.success = true;
    result.push_id = "APP_" + alert->alert_id + "_" + user_id;
    
    LOG_INFO() << "App push sent to user: " << user_id << " alert: " << alert->alert_id;
    return result;
}

void AppPushChannel::SetServerUrl(const std::string& url) {
    m_serverUrl = url;
}

PushMethod SoundPushChannel::GetMethod() const {
    return PushMethod::kSound;
}

bool SoundPushChannel::IsAvailable() const {
    return true;
}

PushResult SoundPushChannel::Send(const AlertPtr& alert, const std::string& user_id) {
    PushResult result;
    result.push_time = DateTime::Now();
    result.success = true;
    result.push_id = "SOUND_" + alert->alert_id + "_" + user_id;
    
    LOG_INFO() << "Sound alert played for user: " << user_id << " alert: " << alert->alert_id;
    return result;
}

void SoundPushChannel::SetSoundFile(const std::string& filepath) {
    m_soundFile = filepath;
}

PushMethod SmsPushChannel::GetMethod() const {
    return PushMethod::kSms;
}

bool SmsPushChannel::IsAvailable() const {
    return !m_smsServiceUrl.empty();
}

PushResult SmsPushChannel::Send(const AlertPtr& alert, const std::string& user_id) {
    PushResult result;
    result.push_time = DateTime::Now();
    
    if (!IsAvailable()) {
        result.success = false;
        result.error_message = "SMS service not configured";
        return result;
    }
    
    result.success = true;
    result.push_id = "SMS_" + alert->alert_id + "_" + user_id;
    
    LOG_INFO() << "SMS sent to user: " << user_id << " alert: " << alert->alert_id;
    return result;
}

void SmsPushChannel::SetSmsServiceUrl(const std::string& url) {
    m_smsServiceUrl = url;
}

PushMethod EmailPushChannel::GetMethod() const {
    return PushMethod::kEmail;
}

bool EmailPushChannel::IsAvailable() const {
    return !m_smtpServer.empty();
}

PushResult EmailPushChannel::Send(const AlertPtr& alert, const std::string& user_id) {
    PushResult result;
    result.push_time = DateTime::Now();
    
    if (!IsAvailable()) {
        result.success = false;
        result.error_message = "SMTP server not configured";
        return result;
    }
    
    result.success = true;
    result.push_id = "EMAIL_" + alert->alert_id + "_" + user_id;
    
    LOG_INFO() << "Email sent to user: " << user_id << " alert: " << alert->alert_id;
    return result;
}

void EmailPushChannel::SetSmtpServer(const std::string& server, int port) {
    m_smtpServer = server;
    m_smtpPort = port;
}

}
}
