#ifndef OGC_ALERT_PUSH_SERVICE_H
#define OGC_ALERT_PUSH_SERVICE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

struct PushResult {
    std::string push_id;
    bool success;
    std::string error_message;
    DateTime push_time;
};

struct PushRecord {
    std::string push_id;
    std::string alert_id;
    PushMethod method;
    std::string target_user;
    DateTime push_time;
    bool success;
    std::string error_message;
};

struct PushConfig {
    int max_retry_count;
    int retry_interval_ms;
    bool enable_retry;
    bool enable_logging;
    int timeout_ms;
    std::string default_sender;
    
    PushConfig()
        : max_retry_count(3)
        , retry_interval_ms(1000)
        , enable_retry(true)
        , enable_logging(true)
        , timeout_ms(5000)
        , default_sender("alert_system") {}
};

using PushCallback = std::function<void(const Alert&, const PushResult&)>;

class OGC_ALERT_API IPushChannel {
public:
    virtual ~IPushChannel() = default;
    
    virtual PushMethod GetMethod() const = 0;
    virtual bool IsAvailable() const = 0;
    virtual PushResult Send(const AlertPtr& alert, const std::string& user_id) = 0;
};

using IPushChannelPtr = std::shared_ptr<IPushChannel>;

class OGC_ALERT_API IPushService {
public:
    virtual ~IPushService() = default;
    
    virtual void RegisterChannel(IPushChannelPtr channel) = 0;
    virtual void UnregisterChannel(PushMethod method) = 0;
    virtual IPushChannelPtr GetChannel(PushMethod method) const = 0;
    
    virtual void SetPushStrategy(const std::vector<PushMethod>& level1_methods,
                                  const std::vector<PushMethod>& level2_methods,
                                  const std::vector<PushMethod>& level3_methods,
                                  const std::vector<PushMethod>& level4_methods) = 0;
    
    virtual void SetConfig(const PushConfig& config) = 0;
    virtual PushConfig GetConfig() const = 0;
    
    virtual void SetPushCallback(PushCallback callback) = 0;
    
    virtual std::vector<PushResult> Push(const AlertPtr& alert, const std::vector<std::string>& user_ids) = 0;
    virtual std::vector<PushResult> PushByMethod(const AlertPtr& alert, 
                                                  const std::vector<std::string>& user_ids,
                                                  PushMethod method) = 0;
    
    virtual void SetRetryCount(int count) = 0;
    virtual void SetRetryInterval(int interval_ms) = 0;
    
    virtual std::vector<PushRecord> GetPushHistory(const std::string& alert_id) = 0;
    
    static std::unique_ptr<IPushService> Create();
};

class OGC_ALERT_API PushService : public IPushService {
public:
    PushService();
    ~PushService();
    
    void RegisterChannel(IPushChannelPtr channel) override;
    void UnregisterChannel(PushMethod method) override;
    IPushChannelPtr GetChannel(PushMethod method) const override;
    
    void SetPushStrategy(const std::vector<PushMethod>& level1_methods,
                         const std::vector<PushMethod>& level2_methods,
                         const std::vector<PushMethod>& level3_methods,
                         const std::vector<PushMethod>& level4_methods) override;
    
    void SetConfig(const PushConfig& config) override;
    PushConfig GetConfig() const override;
    
    void SetPushCallback(PushCallback callback) override;
    
    std::vector<PushResult> Push(const AlertPtr& alert, const std::vector<std::string>& user_ids) override;
    std::vector<PushResult> PushByMethod(const AlertPtr& alert, 
                                         const std::vector<std::string>& user_ids,
                                         PushMethod method) override;
    
    void SetRetryCount(int count) override;
    void SetRetryInterval(int interval_ms) override;
    
    std::vector<PushRecord> GetPushHistory(const std::string& alert_id) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

class OGC_ALERT_API AppPushChannel : public IPushChannel {
public:
    PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    PushResult Send(const AlertPtr& alert, const std::string& user_id) override;
    
    void SetServerUrl(const std::string& url);
    
private:
    std::string m_serverUrl;
};

class OGC_ALERT_API SoundPushChannel : public IPushChannel {
public:
    PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    PushResult Send(const AlertPtr& alert, const std::string& user_id) override;
    
    void SetSoundFile(const std::string& filepath);
    
private:
    std::string m_soundFile;
};

class OGC_ALERT_API SmsPushChannel : public IPushChannel {
public:
    PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    PushResult Send(const AlertPtr& alert, const std::string& user_id) override;
    
    void SetSmsServiceUrl(const std::string& url);
    
private:
    std::string m_smsServiceUrl;
};

class OGC_ALERT_API EmailPushChannel : public IPushChannel {
public:
    PushMethod GetMethod() const override;
    bool IsAvailable() const override;
    PushResult Send(const AlertPtr& alert, const std::string& user_id) override;
    
    void SetSmtpServer(const std::string& server, int port);
    
private:
    std::string m_smtpServer;
    int m_smtpPort;
};

}
}

#endif
