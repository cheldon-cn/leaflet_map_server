#include "alert/websocket_service.h"
#include "alert/alert_push_service.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <algorithm>

namespace alert {

WebSocketService::WebSocketService()
    : m_running(false)
    , m_enabled(true) {
}

WebSocketService::~WebSocketService() {
    Stop();
}

void WebSocketService::SetAlertPushService(std::shared_ptr<IAlertPushService> service) {
    m_pushService = service;
}

bool WebSocketService::Start(const WebSocketConfig& config) {
    if (m_running) {
        return true;
    }
    
    m_config = config;
    m_running = true;
    return true;
}

void WebSocketService::Stop() {
    if (!m_running) {
        return;
    }
    
    CloseAllClients();
    m_running = false;
}

bool WebSocketService::IsRunning() const {
    return m_running;
}

bool WebSocketService::SendToClient(const std::string& clientId, const WebSocketMessage& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    auto it = m_clients.find(clientId);
    if (it == m_clients.end()) {
        m_lastError = "Client not found: " + clientId;
        return false;
    }
    
    it->second.lastActiveTime = GetCurrentTimestamp();
    return true;
}

bool WebSocketService::SendToUser(const std::string& userId, const WebSocketMessage& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    auto it = m_userClientMap.find(userId);
    if (it == m_userClientMap.end()) {
        m_lastError = "User not connected: " + userId;
        return false;
    }
    
    return SendToClient(it->second, message);
}

void WebSocketService::Broadcast(const WebSocketMessage& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    for (auto& pair : m_clients) {
        pair.second.lastActiveTime = GetCurrentTimestamp();
    }
}

void WebSocketService::BroadcastToType(const std::string& alertType, const WebSocketMessage& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    for (auto& pair : m_clients) {
        WebSocketClient& client = pair.second;
        auto it = std::find(client.subscribedTypes.begin(), client.subscribedTypes.end(), alertType);
        if (it != client.subscribedTypes.end()) {
            client.lastActiveTime = GetCurrentTimestamp();
        }
    }
}

void WebSocketService::SetMessageCallback(MessageCallback callback) {
    m_messageCallback = callback;
}

void WebSocketService::SetConnectionCallback(ConnectionCallback onConnect, ConnectionCallback onDisconnect) {
    m_onConnect = onConnect;
    m_onDisconnect = onDisconnect;
}

WebSocketClient WebSocketService::GetClient(const std::string& clientId) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    auto it = m_clients.find(clientId);
    if (it != m_clients.end()) {
        return it->second;
    }
    
    return WebSocketClient();
}

std::vector<WebSocketClient> WebSocketService::GetAllClients() {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    std::vector<WebSocketClient> result;
    for (const auto& pair : m_clients) {
        result.push_back(pair.second);
    }
    return result;
}

int WebSocketService::GetClientCount() const {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    return static_cast<int>(m_clients.size());
}

bool WebSocketService::CloseClient(const std::string& clientId) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    auto it = m_clients.find(clientId);
    if (it == m_clients.end()) {
        return false;
    }
    
    OnClientDisconnect(clientId);
    return true;
}

void WebSocketService::CloseAllClients() {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    for (const auto& pair : m_clients) {
        OnClientDisconnect(pair.first);
    }
}

void WebSocketService::SetJwtSecret(const std::string& secret) {
    m_jwtSecret = secret;
}

bool WebSocketService::ValidateToken(const std::string& token, std::string& userId) {
    if (token.empty()) {
        return false;
    }
    
    if (token.substr(0, 7) == "Bearer ") {
        std::string actualToken = token.substr(7);
        
        if (actualToken.find("user_") == 0) {
            userId = actualToken;
            return true;
        }
    }
    
    return false;
}

void WebSocketService::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

bool WebSocketService::IsEnabled() const {
    return m_enabled;
}

std::string WebSocketService::GetLastError() const {
    return m_lastError;
}

WebSocketMessage WebSocketService::CreateAlertPushMessage(const Alert& alert) {
    WebSocketMessage message;
    message.type = "alert_push";
    message.timestamp = GetCurrentTimestamp();
    message.valid = true;
    
    std::ostringstream oss;
    oss << "{"
        << "\"alert_id\":\"" << alert.GetAlertId() << "\","
        << "\"alert_type\":\"" << AlertTypeToString(alert.GetAlertType()) << "\","
        << "\"alert_level\":" << static_cast<int>(alert.GetAlertLevel()) << ","
        << "\"title\":\"" << alert.GetTitle() << "\","
        << "\"message\":\"" << alert.GetMessage() << "\","
        << "\"position\":{"
        << "\"longitude\":" << alert.GetPosition().longitude << ","
        << "\"latitude\":" << alert.GetPosition().latitude
        << "},"
        << "\"action_required\":\"" << alert.GetRecommendation() << "\""
        << "}";
    message.data = oss.str();
    
    return message;
}

WebSocketMessage WebSocketService::CreateAlertClearMessage(const std::string& alertId, const std::string& reason) {
    WebSocketMessage message;
    message.type = "alert_clear";
    message.timestamp = GetCurrentTimestamp();
    message.valid = true;
    
    std::ostringstream oss;
    oss << "{"
        << "\"alert_id\":\"" << alertId << "\","
        << "\"clear_reason\":\"" << reason << "\""
        << "}";
    message.data = oss.str();
    
    return message;
}

std::string WebSocketService::GenerateClientId() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << "WS_" << std::put_time(tm_info, "%Y%m%d%H%M%S") << "_" 
        << std::setfill('0') << std::setw(6) << (std::rand() % 1000000);
    return oss.str();
}

std::string WebSocketService::GenerateConnectionId() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << "CONN_" << std::put_time(tm_info, "%Y%m%d%H%M%S") << "_" 
        << std::setfill('0') << std::setw(6) << (std::rand() % 1000000);
    return oss.str();
}

std::string WebSocketService::GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string WebSocketService::BuildMessageJson(const WebSocketMessage& message) {
    std::ostringstream oss;
    oss << "{"
        << "\"type\":\"" << message.type << "\","
        << "\"data\":" << message.data << ","
        << "\"timestamp\":\"" << message.timestamp << "\""
        << "}";
    return oss.str();
}

bool WebSocketService::ParseMessageJson(const std::string& json, WebSocketMessage& message) {
    if (json.empty()) {
        return false;
    }
    
    size_t typePos = json.find("\"type\"");
    if (typePos == std::string::npos) {
        return false;
    }
    
    size_t typeStart = json.find('"', typePos + 7);
    if (typeStart == std::string::npos) return false;
    size_t typeEnd = json.find('"', typeStart + 1);
    if (typeEnd == std::string::npos) return false;
    message.type = json.substr(typeStart + 1, typeEnd - typeStart - 1);
    
    size_t dataPos = json.find("\"data\"");
    if (dataPos != std::string::npos) {
        size_t dataStart = json.find('{', dataPos + 6);
        if (dataStart != std::string::npos) {
            int braceCount = 1;
            size_t dataEnd = dataStart + 1;
            while (dataEnd < json.length() && braceCount > 0) {
                if (json[dataEnd] == '{') braceCount++;
                else if (json[dataEnd] == '}') braceCount--;
                dataEnd++;
            }
            message.data = json.substr(dataStart, dataEnd - dataStart);
        }
    }
    
    message.valid = true;
    return true;
}

void WebSocketService::OnClientConnect(const std::string& clientId, const std::string& userId) {
    WebSocketClient client;
    client.clientId = clientId;
    client.userId = userId;
    client.connectionId = GenerateConnectionId();
    client.connectTime = GetCurrentTimestamp();
    client.lastActiveTime = client.connectTime;
    client.authenticated = true;
    client.valid = true;
    
    client.subscribedTypes.push_back("depth");
    client.subscribedTypes.push_back("collision");
    client.subscribedTypes.push_back("weather");
    client.subscribedTypes.push_back("channel");
    
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients[clientId] = client;
        m_userClientMap[userId] = clientId;
    }
    
    if (m_onConnect) {
        m_onConnect(clientId, client);
    }
}

void WebSocketService::OnClientDisconnect(const std::string& clientId) {
    WebSocketClient client;
    {
        auto it = m_clients.find(clientId);
        if (it != m_clients.end()) {
            client = it->second;
            m_userClientMap.erase(client.userId);
            m_clients.erase(it);
        }
    }
    
    if (m_onDisconnect && client.valid) {
        m_onDisconnect(clientId, client);
    }
}

void WebSocketService::OnClientMessage(const std::string& clientId, const std::string& message) {
    WebSocketMessage wsMessage;
    if (!ParseMessageJson(message, wsMessage)) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        auto it = m_clients.find(clientId);
        if (it != m_clients.end()) {
            it->second.lastActiveTime = GetCurrentTimestamp();
        }
    }
    
    if (m_messageCallback) {
        m_messageCallback(clientId, wsMessage);
    }
}

}
