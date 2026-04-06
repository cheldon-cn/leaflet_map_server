#include "ogc/alert/websocket_service.h"
#include "ogc/base/log.h"
#include <map>
#include <set>
#include <mutex>
#include <algorithm>
#include <random>

using ogc::base::LogLevel;
using ogc::base::LogHelper;

namespace ogc {
namespace alert {

class WebSocketService::Impl {
public:
    Impl() : m_running(false) {}
    
    ~Impl() {
        Stop();
    }
    
    bool Start(const WebSocketConfig& config) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_running) {
            LOG_WARNING() << "WebSocket service already running";
            return false;
        }
        
        m_config = config;
        m_running = true;
        
        LOG_INFO() << "WebSocket service started on " << config.host << ":" << config.port;
        return true;
    }
    
    void Stop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_running) {
            return;
        }
        
        m_sessions.clear();
        m_userSessions.clear();
        m_sessionSubscriptions.clear();
        m_running = false;
        
        LOG_INFO() << "WebSocket service stopped";
    }
    
    bool IsRunning() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_running;
    }
    
    void SetMessageHandler(WebSocketMessageHandler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messageHandler = handler;
    }
    
    void SetConnectHandler(WebSocketConnectHandler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connectHandler = handler;
    }
    
    void SetDisconnectHandler(WebSocketDisconnectHandler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_disconnectHandler = handler;
    }
    
    bool SendToSession(const std::string& session_id, const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sessions.find(session_id);
        if (it == m_sessions.end()) {
            LOG_WARNING() << "Session not found: " << session_id;
            return false;
        }
        
        LOG_DEBUG() << "Message sent to session " << session_id << ": " << message.substr(0, 100);
        return true;
    }
    
    bool SendToUser(const std::string& user_id, const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_userSessions.find(user_id);
        if (it == m_userSessions.end() || it->second.empty()) {
            LOG_WARNING() << "No sessions for user: " << user_id;
            return false;
        }
        
        for (const auto& sessionId : it->second) {
            LOG_DEBUG() << "Message sent to user " << user_id << " session " << sessionId;
        }
        return true;
    }
    
    void Broadcast(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        LOG_DEBUG() << "Broadcasting message to " << static_cast<int>(m_sessions.size()) << " sessions";
    }
    
    void BroadcastExcept(const std::string& message, const std::vector<std::string>& exclude_sessions) {
        std::lock_guard<std::mutex> lock(m_mutex);
        int count = 0;
        for (const auto& pair : m_sessions) {
            if (std::find(exclude_sessions.begin(), exclude_sessions.end(), pair.first) == exclude_sessions.end()) {
                count++;
            }
        }
        LOG_DEBUG() << "Broadcasting message to " << count << " sessions (excluding " << static_cast<int>(exclude_sessions.size()) << ")";
    }
    
    std::vector<WebSocketSession> GetActiveSessions() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<WebSocketSession> result;
        for (const auto& pair : m_sessions) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    WebSocketSession GetSession(const std::string& session_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sessions.find(session_id);
        if (it != m_sessions.end()) {
            return it->second;
        }
        return WebSocketSession();
    }
    
    int GetSessionCount() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return static_cast<int>(m_sessions.size());
    }
    
    bool CloseSession(const std::string& session_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sessions.find(session_id);
        if (it == m_sessions.end()) {
            return false;
        }
        
        std::string userId = it->second.user_id;
        m_sessions.erase(it);
        
        if (!userId.empty()) {
            auto userIt = m_userSessions.find(userId);
            if (userIt != m_userSessions.end()) {
                userIt->second.erase(session_id);
                if (userIt->second.empty()) {
                    m_userSessions.erase(userIt);
                }
            }
        }
        
        m_sessionSubscriptions.erase(session_id);
        
        if (m_disconnectHandler) {
            m_disconnectHandler(session_id);
        }
        
        LOG_INFO() << "Session closed: " << session_id;
        return true;
    }
    
    bool AuthenticateSession(const std::string& session_id, const std::string& user_id, const std::string& token) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sessions.find(session_id);
        if (it == m_sessions.end()) {
            return false;
        }
        
        it->second.user_id = user_id;
        it->second.authenticated = true;
        
        m_userSessions[user_id].insert(session_id);
        
        LOG_INFO() << "Session authenticated: " << session_id << " for user " << user_id;
        return true;
    }
    
    bool IsSessionAuthenticated(const std::string& session_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sessions.find(session_id);
        return it != m_sessions.end() && it->second.authenticated;
    }
    
    void SubscribeToAlert(const std::string& session_id, AlertType alert_type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sessionSubscriptions[session_id].insert(alert_type);
        LOG_INFO() << "Session " << session_id << " subscribed to alert type " << static_cast<int>(alert_type);
    }
    
    void UnsubscribeFromAlert(const std::string& session_id, AlertType alert_type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_sessionSubscriptions.find(session_id);
        if (it != m_sessionSubscriptions.end()) {
            it->second.erase(alert_type);
        }
        LOG_INFO() << "Session " << session_id << " unsubscribed from alert type " << static_cast<int>(alert_type);
    }
    
    std::vector<AlertType> GetSubscriptions(const std::string& session_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertType> result;
        auto it = m_sessionSubscriptions.find(session_id);
        if (it != m_sessionSubscriptions.end()) {
            for (const auto& type : it->second) {
                result.push_back(type);
            }
        }
        return result;
    }
    
    std::string GenerateSessionId() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
        static const char* hex = "0123456789abcdef";
        
        std::string id;
        id.reserve(32);
        for (int i = 0; i < 32; ++i) {
            id += hex[dis(gen)];
        }
        return id;
    }
    
    void SimulateConnect(const std::string& remote_address) {
        std::lock_guard<std::mutex> lock(m_mutex);
        WebSocketSession session;
        session.session_id = GenerateSessionId();
        session.remote_address = remote_address;
        session.connected_at = DateTime::Now();
        session.authenticated = false;
        
        m_sessions[session.session_id] = session;
        
        if (m_connectHandler) {
            m_connectHandler(session);
        }
        
        LOG_INFO() << "New session connected: " << session.session_id << " from " << remote_address;
    }
    
private:
    WebSocketConfig m_config;
    std::map<std::string, WebSocketSession> m_sessions;
    std::map<std::string, std::set<std::string>> m_userSessions;
    std::map<std::string, std::set<AlertType>> m_sessionSubscriptions;
    WebSocketMessageHandler m_messageHandler;
    WebSocketConnectHandler m_connectHandler;
    WebSocketDisconnectHandler m_disconnectHandler;
    bool m_running;
    mutable std::mutex m_mutex;
};

WebSocketService::WebSocketService()
    : m_impl(std::make_unique<Impl>()) {
}

WebSocketService::~WebSocketService() {
}

bool WebSocketService::Start(const WebSocketConfig& config) {
    return m_impl->Start(config);
}

void WebSocketService::Stop() {
    m_impl->Stop();
}

bool WebSocketService::IsRunning() const {
    return m_impl->IsRunning();
}

void WebSocketService::SetMessageHandler(WebSocketMessageHandler handler) {
    m_impl->SetMessageHandler(handler);
}

void WebSocketService::SetConnectHandler(WebSocketConnectHandler handler) {
    m_impl->SetConnectHandler(handler);
}

void WebSocketService::SetDisconnectHandler(WebSocketDisconnectHandler handler) {
    m_impl->SetDisconnectHandler(handler);
}

bool WebSocketService::SendToSession(const std::string& session_id, const std::string& message) {
    return m_impl->SendToSession(session_id, message);
}

bool WebSocketService::SendToUser(const std::string& user_id, const std::string& message) {
    return m_impl->SendToUser(user_id, message);
}

void WebSocketService::Broadcast(const std::string& message) {
    m_impl->Broadcast(message);
}

void WebSocketService::BroadcastExcept(const std::string& message, const std::vector<std::string>& exclude_sessions) {
    m_impl->BroadcastExcept(message, exclude_sessions);
}

std::vector<WebSocketSession> WebSocketService::GetActiveSessions() {
    return m_impl->GetActiveSessions();
}

WebSocketSession WebSocketService::GetSession(const std::string& session_id) {
    return m_impl->GetSession(session_id);
}

int WebSocketService::GetSessionCount() {
    return m_impl->GetSessionCount();
}

bool WebSocketService::CloseSession(const std::string& session_id) {
    return m_impl->CloseSession(session_id);
}

bool WebSocketService::AuthenticateSession(const std::string& session_id, const std::string& user_id, const std::string& token) {
    return m_impl->AuthenticateSession(session_id, user_id, token);
}

bool WebSocketService::IsSessionAuthenticated(const std::string& session_id) {
    return m_impl->IsSessionAuthenticated(session_id);
}

void WebSocketService::SubscribeToAlert(const std::string& session_id, AlertType alert_type) {
    m_impl->SubscribeToAlert(session_id, alert_type);
}

void WebSocketService::UnsubscribeFromAlert(const std::string& session_id, AlertType alert_type) {
    m_impl->UnsubscribeFromAlert(session_id, alert_type);
}

std::vector<AlertType> WebSocketService::GetSubscriptions(const std::string& session_id) {
    return m_impl->GetSubscriptions(session_id);
}

std::unique_ptr<IWebSocketService> IWebSocketService::Create() {
    return std::unique_ptr<IWebSocketService>(new WebSocketService());
}

}
}
