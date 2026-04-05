#ifndef OGC_ALERT_WEBSOCKET_SERVICE_H
#define OGC_ALERT_WEBSOCKET_SERVICE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

struct WebSocketConfig {
    std::string host;
    int port;
    int max_connections;
    int ping_interval_ms;
    int ping_timeout_ms;
    bool enable_ssl;
    std::string cert_file;
    std::string key_file;
    
    WebSocketConfig()
        : host("0.0.0.0")
        , port(8080)
        , max_connections(1000)
        , ping_interval_ms(30000)
        , ping_timeout_ms(5000)
        , enable_ssl(false) {}
};

struct WebSocketSession {
    std::string session_id;
    std::string user_id;
    std::string remote_address;
    DateTime connected_at;
    bool authenticated;
};

struct WebSocketMessage {
    std::string session_id;
    std::string type;
    std::string payload;
    DateTime timestamp;
};

using WebSocketMessageHandler = std::function<void(const WebSocketMessage&)>;
using WebSocketConnectHandler = std::function<void(const WebSocketSession&)>;
using WebSocketDisconnectHandler = std::function<void(const std::string& session_id)>;

class OGC_ALERT_API IWebSocketService {
public:
    virtual ~IWebSocketService() = default;
    
    virtual bool Start(const WebSocketConfig& config) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    virtual void SetMessageHandler(WebSocketMessageHandler handler) = 0;
    virtual void SetConnectHandler(WebSocketConnectHandler handler) = 0;
    virtual void SetDisconnectHandler(WebSocketDisconnectHandler handler) = 0;
    
    virtual bool SendToSession(const std::string& session_id, const std::string& message) = 0;
    virtual bool SendToUser(const std::string& user_id, const std::string& message) = 0;
    virtual void Broadcast(const std::string& message) = 0;
    virtual void BroadcastExcept(const std::string& message, const std::vector<std::string>& exclude_sessions) = 0;
    
    virtual std::vector<WebSocketSession> GetActiveSessions() = 0;
    virtual WebSocketSession GetSession(const std::string& session_id) = 0;
    virtual int GetSessionCount() = 0;
    virtual bool CloseSession(const std::string& session_id) = 0;
    
    virtual bool AuthenticateSession(const std::string& session_id, const std::string& user_id, const std::string& token) = 0;
    virtual bool IsSessionAuthenticated(const std::string& session_id) = 0;
    
    virtual void SubscribeToAlert(const std::string& session_id, AlertType alert_type) = 0;
    virtual void UnsubscribeFromAlert(const std::string& session_id, AlertType alert_type) = 0;
    virtual std::vector<AlertType> GetSubscriptions(const std::string& session_id) = 0;
    
    static std::unique_ptr<IWebSocketService> Create();
};

class OGC_ALERT_API WebSocketService : public IWebSocketService {
public:
    WebSocketService();
    ~WebSocketService();
    
    bool Start(const WebSocketConfig& config) override;
    void Stop() override;
    bool IsRunning() const override;
    
    void SetMessageHandler(WebSocketMessageHandler handler) override;
    void SetConnectHandler(WebSocketConnectHandler handler) override;
    void SetDisconnectHandler(WebSocketDisconnectHandler handler) override;
    
    bool SendToSession(const std::string& session_id, const std::string& message) override;
    bool SendToUser(const std::string& user_id, const std::string& message) override;
    void Broadcast(const std::string& message) override;
    void BroadcastExcept(const std::string& message, const std::vector<std::string>& exclude_sessions) override;
    
    std::vector<WebSocketSession> GetActiveSessions() override;
    WebSocketSession GetSession(const std::string& session_id) override;
    int GetSessionCount() override;
    bool CloseSession(const std::string& session_id) override;
    
    bool AuthenticateSession(const std::string& session_id, const std::string& user_id, const std::string& token) override;
    bool IsSessionAuthenticated(const std::string& session_id) override;
    
    void SubscribeToAlert(const std::string& session_id, AlertType alert_type) override;
    void UnsubscribeFromAlert(const std::string& session_id, AlertType alert_type) override;
    std::vector<AlertType> GetSubscriptions(const std::string& session_id) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
