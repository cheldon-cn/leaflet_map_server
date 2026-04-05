#ifndef ALERT_WEBSOCKET_SERVICE_H
#define ALERT_WEBSOCKET_SERVICE_H

#include "alert/alert.h"
#include "alert/alert_types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

namespace alert {

struct WebSocketMessage {
    std::string type;
    std::string data;
    std::string timestamp;
    bool valid;
    
    WebSocketMessage()
        : valid(false) {}
};

struct WebSocketClient {
    std::string clientId;
    std::string userId;
    std::string connectionId;
    std::vector<std::string> subscribedTypes;
    std::string connectTime;
    std::string lastActiveTime;
    bool authenticated;
    bool valid;
    
    WebSocketClient()
        : authenticated(false), valid(false) {}
};

struct WebSocketConfig {
    int port;
    int maxConnections;
    int pingInterval;
    int pongTimeout;
    int maxMessageSize;
    bool enableSSL;
    std::string certPath;
    std::string keyPath;
    bool valid;
    
    WebSocketConfig()
        : port(9002)
        , maxConnections(1000)
        , pingInterval(30)
        , pongTimeout(10)
        , maxMessageSize(65536)
        , enableSSL(false)
        , valid(false) {}
};

class IAlertPushService;

typedef std::function<void(const std::string& clientId, const WebSocketMessage& message)> MessageCallback;
typedef std::function<void(const std::string& clientId, const WebSocketClient& client)> ConnectionCallback;

class IWebSocketService {
public:
    virtual ~IWebSocketService() {}
    
    virtual void SetAlertPushService(std::shared_ptr<IAlertPushService> service) = 0;
    
    virtual bool Start(const WebSocketConfig& config) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    virtual bool SendToClient(const std::string& clientId, const WebSocketMessage& message) = 0;
    virtual bool SendToUser(const std::string& userId, const WebSocketMessage& message) = 0;
    virtual void Broadcast(const WebSocketMessage& message) = 0;
    virtual void BroadcastToType(const std::string& alertType, const WebSocketMessage& message) = 0;
    
    virtual void SetMessageCallback(MessageCallback callback) = 0;
    virtual void SetConnectionCallback(ConnectionCallback onConnect, ConnectionCallback onDisconnect) = 0;
    
    virtual WebSocketClient GetClient(const std::string& clientId) = 0;
    virtual std::vector<WebSocketClient> GetAllClients() = 0;
    virtual int GetClientCount() const = 0;
    
    virtual bool CloseClient(const std::string& clientId) = 0;
    virtual void CloseAllClients() = 0;
    
    virtual void SetJwtSecret(const std::string& secret) = 0;
    virtual bool ValidateToken(const std::string& token, std::string& userId) = 0;
};

class WebSocketService : public IWebSocketService {
public:
    WebSocketService();
    virtual ~WebSocketService();
    
    void SetAlertPushService(std::shared_ptr<IAlertPushService> service) override;
    
    bool Start(const WebSocketConfig& config) override;
    void Stop() override;
    bool IsRunning() const override;
    
    bool SendToClient(const std::string& clientId, const WebSocketMessage& message) override;
    bool SendToUser(const std::string& userId, const WebSocketMessage& message) override;
    void Broadcast(const WebSocketMessage& message) override;
    void BroadcastToType(const std::string& alertType, const WebSocketMessage& message) override;
    
    void SetMessageCallback(MessageCallback callback) override;
    void SetConnectionCallback(ConnectionCallback onConnect, ConnectionCallback onDisconnect) override;
    
    WebSocketClient GetClient(const std::string& clientId) override;
    std::vector<WebSocketClient> GetAllClients() override;
    int GetClientCount() const override;
    
    bool CloseClient(const std::string& clientId) override;
    void CloseAllClients() override;
    
    void SetJwtSecret(const std::string& secret) override;
    bool ValidateToken(const std::string& token, std::string& userId) override;
    
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    
    std::string GetLastError() const;
    
    WebSocketMessage CreateAlertPushMessage(const Alert& alert);
    WebSocketMessage CreateAlertClearMessage(const std::string& alertId, const std::string& reason);
    
private:
    std::shared_ptr<IAlertPushService> m_pushService;
    WebSocketConfig m_config;
    std::string m_jwtSecret;
    bool m_running;
    bool m_enabled;
    std::string m_lastError;
    
    std::map<std::string, WebSocketClient> m_clients;
    std::map<std::string, std::string> m_userClientMap;
    mutable std::mutex m_clientsMutex;
    
    MessageCallback m_messageCallback;
    ConnectionCallback m_onConnect;
    ConnectionCallback m_onDisconnect;
    
    std::string GenerateClientId();
    std::string GenerateConnectionId();
    std::string GetCurrentTimestamp();
    std::string BuildMessageJson(const WebSocketMessage& message);
    bool ParseMessageJson(const std::string& json, WebSocketMessage& message);
    
    void OnClientConnect(const std::string& clientId, const std::string& userId);
    void OnClientDisconnect(const std::string& clientId);
    void OnClientMessage(const std::string& clientId, const std::string& message);
};

}

#endif
