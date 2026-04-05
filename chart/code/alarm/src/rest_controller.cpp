#include "alert/rest_controller.h"
#include "alert/i_alert_engine.h"
#include "alert/alert_judge_service.h"
#include "alert/alert_push_service.h"
#include "alert/alert_config_service.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <algorithm>

namespace alert {

RestController::RestController()
    : m_running(false)
    , m_enabled(true)
    , m_port(8080) {
    RegisterDefaultHandlers();
}

RestController::~RestController() {
    Stop();
}

void RestController::SetAlertJudgeService(std::shared_ptr<IAlertJudgeService> service) {
    m_judgeService = service;
}

void RestController::SetAlertPushService(std::shared_ptr<IAlertPushService> service) {
    m_pushService = service;
}

void RestController::SetAlertConfigService(std::shared_ptr<IAlertConfigService> service) {
    m_configService = service;
}

void RestController::RegisterHandler(const std::string& path, 
                                      const std::string& method,
                                      RequestHandler handler) {
    std::string key = method + ":" + path;
    m_handlers[key] = handler;
}

void RestController::UnregisterHandler(const std::string& path, 
                                        const std::string& method) {
    std::string key = method + ":" + path;
    m_handlers.erase(key);
}

HttpResponse RestController::HandleRequest(const HttpRequest& request) {
    HttpResponse response;
    
    if (!m_enabled) {
        response.statusCode = 503;
        response.statusMessage = "Service Unavailable";
        response.body = BuildErrorResponse(503, "Service unavailable", "REST API is disabled");
        response.contentType = "application/json";
        response.valid = true;
        return response;
    }
    
    if (!request.valid) {
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.body = BuildErrorResponse(400, "Invalid request");
        response.contentType = "application/json";
        response.valid = true;
        return response;
    }
    
    std::string path = request.path;
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        path = path.substr(0, queryPos);
    }
    
    std::string key = request.method + ":" + path;
    auto it = m_handlers.find(key);
    
    if (it != m_handlers.end()) {
        return it->second(request);
    }
    
    for (const auto& pair : m_handlers) {
        size_t wildcardPos = pair.first.find("*");
        if (wildcardPos != std::string::npos) {
            std::string prefix = pair.first.substr(0, wildcardPos);
            if (key.find(prefix) == 0) {
                return pair.second(request);
            }
        }
    }
    
    response.statusCode = 404;
    response.statusMessage = "Not Found";
    response.body = BuildErrorResponse(404, "Endpoint not found", request.path);
    response.contentType = "application/json";
    response.valid = true;
    return response;
}

bool RestController::Start(int port) {
    if (m_running) {
        return true;
    }
    
    m_port = port;
    m_running = true;
    return true;
}

void RestController::Stop() {
    m_running = false;
}

bool RestController::IsRunning() const {
    return m_running;
}

void RestController::SetJwtSecret(const std::string& secret) {
    m_jwtSecret = secret;
}

bool RestController::ValidateToken(const std::string& token, std::string& userId) {
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

void RestController::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

bool RestController::IsEnabled() const {
    return m_enabled;
}

std::string RestController::GetLastError() const {
    return m_lastError;
}

void RestController::RegisterDefaultHandlers() {
    RegisterHandler("/api/v1/alert/subscribe", "POST",
        [this](const HttpRequest& req) { return HandleSubscribe(req); });
    
    RegisterHandler("/api/v1/alert/list", "GET",
        [this](const HttpRequest& req) { return HandleAlertList(req); });
    
    RegisterHandler("/api/v1/alert/detail/*", "GET",
        [this](const HttpRequest& req) { return HandleAlertDetail(req); });
    
    RegisterHandler("/api/v1/alert/acknowledge/*", "POST",
        [this](const HttpRequest& req) { return HandleAcknowledge(req); });
    
    RegisterHandler("/api/v1/alert/config", "GET",
        [this](const HttpRequest& req) { return HandleConfigGet(req); });
    
    RegisterHandler("/api/v1/alert/config", "POST",
        [this](const HttpRequest& req) { return HandleConfigSet(req); });
}

HttpResponse RestController::HandleSubscribe(const HttpRequest& request) {
    HttpResponse response;
    response.contentType = "application/json";
    
    std::string userId;
    if (!ValidateToken(request.authorization, userId)) {
        response.statusCode = 401;
        response.statusMessage = "Unauthorized";
        response.body = BuildErrorResponse(401, "Unauthorized", "Invalid or missing token");
        response.valid = true;
        return response;
    }
    
    std::map<std::string, std::string> params;
    if (!ParseJson(request.body, params)) {
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.body = BuildErrorResponse(400, "Invalid JSON body");
        response.valid = true;
        return response;
    }
    
    SubscribeResponse subResp;
    subResp.subscribeId = GenerateSubscribeId();
    subResp.userId = userId;
    subResp.createTime = GetCurrentTimestamp();
    subResp.valid = true;
    
    std::ostringstream dataJson;
    dataJson << "{"
             << "\"subscribe_id\":\"" << subResp.subscribeId << "\","
             << "\"user_id\":\"" << subResp.userId << "\","
             << "\"create_time\":\"" << subResp.createTime << "\""
             << "}";
    
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.body = BuildJsonResponse(0, "success", dataJson.str());
    response.valid = true;
    return response;
}

HttpResponse RestController::HandleAlertList(const HttpRequest& request) {
    HttpResponse response;
    response.contentType = "application/json";
    
    std::string userId;
    if (!ValidateToken(request.authorization, userId)) {
        response.statusCode = 401;
        response.statusMessage = "Unauthorized";
        response.body = BuildErrorResponse(401, "Unauthorized", "Invalid or missing token");
        response.valid = true;
        return response;
    }
    
    AlertListRequest listReq;
    listReq.userId = userId;
    
    auto it = request.queryParams.find("status");
    if (it != request.queryParams.end()) {
        listReq.status = it->second;
    }
    
    it = request.queryParams.find("page");
    if (it != request.queryParams.end()) {
        listReq.page = std::atoi(it->second.c_str());
        if (listReq.page < 1) listReq.page = 1;
    }
    
    it = request.queryParams.find("page_size");
    if (it != request.queryParams.end()) {
        listReq.pageSize = std::atoi(it->second.c_str());
        if (listReq.pageSize < 1) listReq.pageSize = 20;
        if (listReq.pageSize > 100) listReq.pageSize = 100;
    }
    
    std::vector<Alert> alerts;
    
    Alert alert1;
    alert1.SetAlertId("ALERT_20240520_001");
    alert1.SetAlertType(AlertType::kDepthAlert);
    alert1.SetAlertLevel(AlertLevel::kLevel2_Severe);
    alert1.SetTitle("Depth Warning - Orange");
    alert1.SetMessage("Depth insufficient 2nm ahead, current UKC is 0.4m");
    alert1.SetRecommendation("Reduce speed or alter course");
    alert1.SetStatus(AlertStatus::kPushed);
    alert1.SetIssueTime(GetCurrentTimestamp());
    alerts.push_back(alert1);
    
    Alert alert2;
    alert2.SetAlertId("ALERT_20240520_002");
    alert2.SetAlertType(AlertType::kCollisionAlert);
    alert2.SetAlertLevel(AlertLevel::kLevel1_Critical);
    alert2.SetTitle("Collision Warning - Red");
    alert2.SetMessage("Vessel approaching on collision course, CPA 0.5nm, TCPA 10min");
    alert2.SetRecommendation("Take immediate evasive action");
    alert2.SetStatus(AlertStatus::kPushed);
    alert2.SetIssueTime(GetCurrentTimestamp());
    alerts.push_back(alert2);
    
    std::ostringstream alertsJson;
    alertsJson << "[";
    for (size_t i = 0; i < alerts.size(); ++i) {
        const Alert& alert = alerts[i];
        if (i > 0) alertsJson << ",";
        
        alertsJson << "{"
                   << "\"alert_id\":\"" << alert.GetAlertId() << "\","
                   << "\"alert_type\":\"" << AlertTypeToString(alert.GetAlertType()) << "\","
                   << "\"alert_level\":" << static_cast<int>(alert.GetAlertLevel()) << ","
                   << "\"title\":\"" << alert.GetTitle() << "\","
                   << "\"message\":\"" << alert.GetMessage() << "\","
                   << "\"status\":\"" << AlertStatusToString(alert.GetStatus()) << "\","
                   << "\"issue_time\":\"" << alert.GetIssueTime() << "\""
                   << "}";
    }
    alertsJson << "]";
    
    std::ostringstream dataJson;
    dataJson << "{"
             << "\"total\":" << alerts.size() << ","
             << "\"page\":" << listReq.page << ","
             << "\"page_size\":" << listReq.pageSize << ","
             << "\"alerts\":" << alertsJson.str()
             << "}";
    
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.body = BuildJsonResponse(0, "success", dataJson.str());
    response.valid = true;
    return response;
}

HttpResponse RestController::HandleAlertDetail(const HttpRequest& request) {
    HttpResponse response;
    response.contentType = "application/json";
    
    std::string userId;
    if (!ValidateToken(request.authorization, userId)) {
        response.statusCode = 401;
        response.statusMessage = "Unauthorized";
        response.body = BuildErrorResponse(401, "Unauthorized", "Invalid or missing token");
        response.valid = true;
        return response;
    }
    
    std::string alertId = ExtractPathParam(request.path, "/api/v1/alert/detail/", 0);
    if (alertId.empty()) {
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.body = BuildErrorResponse(400, "Missing alert_id");
        response.valid = true;
        return response;
    }
    
    Alert alert;
    alert.SetAlertId(alertId);
    alert.SetAlertType(AlertType::kDepthAlert);
    alert.SetAlertLevel(AlertLevel::kLevel2_Severe);
    alert.SetTitle("Depth Warning - Orange");
    alert.SetMessage("Depth insufficient 2nm ahead, current UKC is 0.4m");
    alert.SetRecommendation("Reduce speed or alter course");
    alert.SetStatus(AlertStatus::kPushed);
    alert.SetIssueTime(GetCurrentTimestamp());
    
    std::ostringstream dataJson;
    dataJson << "{"
             << "\"alert_id\":\"" << alert.GetAlertId() << "\","
             << "\"alert_type\":\"" << AlertTypeToString(alert.GetAlertType()) << "\","
             << "\"alert_level\":" << static_cast<int>(alert.GetAlertLevel()) << ","
             << "\"title\":\"" << alert.GetTitle() << "\","
             << "\"message\":\"" << alert.GetMessage() << "\","
             << "\"recommendation\":\"" << alert.GetRecommendation() << "\","
             << "\"status\":\"" << AlertStatusToString(alert.GetStatus()) << "\","
             << "\"issue_time\":\"" << alert.GetIssueTime() << "\","
             << "\"acknowledged\":false,"
             << "\"details\":{"
             << "\"chart_depth\":5.0,"
             << "\"tidal_height\":1.2,"
             << "\"effective_depth\":6.2,"
             << "\"ship_draft\":5.5,"
             << "\"squat\":0.3,"
             << "\"ukc\":0.4"
             << "}"
             << "}";
    
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.body = BuildJsonResponse(0, "success", dataJson.str());
    response.valid = true;
    return response;
}

HttpResponse RestController::HandleAcknowledge(const HttpRequest& request) {
    HttpResponse response;
    response.contentType = "application/json";
    
    std::string userId;
    if (!ValidateToken(request.authorization, userId)) {
        response.statusCode = 401;
        response.statusMessage = "Unauthorized";
        response.body = BuildErrorResponse(401, "Unauthorized", "Invalid or missing token");
        response.valid = true;
        return response;
    }
    
    std::string alertId = ExtractPathParam(request.path, "/api/v1/alert/acknowledge/", 0);
    if (alertId.empty()) {
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.body = BuildErrorResponse(400, "Missing alert_id");
        response.valid = true;
        return response;
    }
    
    std::map<std::string, std::string> params;
    ParseJson(request.body, params);
    
    AcknowledgeResponse ackResp;
    ackResp.alertId = alertId;
    ackResp.acknowledged = true;
    ackResp.acknowledgeTime = GetCurrentTimestamp();
    ackResp.valid = true;
    
    std::ostringstream dataJson;
    dataJson << "{"
             << "\"alert_id\":\"" << ackResp.alertId << "\","
             << "\"acknowledged\":" << (ackResp.acknowledged ? "true" : "false") << ","
             << "\"acknowledge_time\":\"" << ackResp.acknowledgeTime << "\""
             << "}";
    
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.body = BuildJsonResponse(0, "success", dataJson.str());
    response.valid = true;
    return response;
}

HttpResponse RestController::HandleConfigGet(const HttpRequest& request) {
    HttpResponse response;
    response.contentType = "application/json";
    
    std::string userId;
    if (!ValidateToken(request.authorization, userId)) {
        response.statusCode = 401;
        response.statusMessage = "Unauthorized";
        response.body = BuildErrorResponse(401, "Unauthorized", "Invalid or missing token");
        response.valid = true;
        return response;
    }
    
    std::ostringstream dataJson;
    dataJson << "{"
             << "\"depth_alert\":{"
             << "\"enabled\":true,"
             << "\"ukc_threshold_critical\":0.3,"
             << "\"ukc_threshold_severe\":0.5,"
             << "\"ukc_threshold_moderate\":0.8"
             << "},"
             << "\"collision_alert\":{"
             << "\"enabled\":true,"
             << "\"cpa_threshold_nm\":0.5,"
             << "\"tcpa_threshold_min\":10"
             << "},"
             << "\"weather_alert\":{"
             << "\"enabled\":true,"
             << "\"wind_speed_threshold_kn\":25,"
             << "\"wave_height_threshold_m\":2.0"
             << "}"
             << "}";
    
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.body = BuildJsonResponse(0, "success", dataJson.str());
    response.valid = true;
    return response;
}

HttpResponse RestController::HandleConfigSet(const HttpRequest& request) {
    HttpResponse response;
    response.contentType = "application/json";
    
    std::string userId;
    if (!ValidateToken(request.authorization, userId)) {
        response.statusCode = 401;
        response.statusMessage = "Unauthorized";
        response.body = BuildErrorResponse(401, "Unauthorized", "Invalid or missing token");
        response.valid = true;
        return response;
    }
    
    std::map<std::string, std::string> params;
    if (!ParseJson(request.body, params)) {
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.body = BuildErrorResponse(400, "Invalid JSON body");
        response.valid = true;
        return response;
    }
    
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.body = BuildJsonResponse(0, "success", "{}");
    response.valid = true;
    return response;
}

std::string RestController::GenerateSubscribeId() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << "SUB_" << std::put_time(tm_info, "%Y%m%d") << "_" 
        << std::setfill('0') << std::setw(6) << (std::rand() % 1000000);
    return oss.str();
}

std::string RestController::GenerateAlertId() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << "ALERT_" << std::put_time(tm_info, "%Y%m%d") << "_" 
        << std::setfill('0') << std::setw(6) << (std::rand() % 1000000);
    return oss.str();
}

std::string RestController::GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

bool RestController::ParseJson(const std::string& json, std::map<std::string, std::string>& result) {
    if (json.empty()) {
        return false;
    }
    
    size_t start = json.find('{');
    size_t end = json.rfind('}');
    
    if (start == std::string::npos || end == std::string::npos) {
        return false;
    }
    
    std::string content = json.substr(start + 1, end - start - 1);
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = content.find_first_of("\"0123456789-truefalsenull", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string value;
        if (content[valueStart] == '"') {
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            pos = valueEnd + 1;
        } else {
            size_t valueEnd = content.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.length();
            value = content.substr(valueStart, valueEnd - valueStart);
            pos = valueEnd;
        }
        
        result[key] = value;
    }
    
    return !result.empty();
}

std::string RestController::BuildJsonResponse(int code, const std::string& message, const std::string& data) {
    std::ostringstream oss;
    oss << "{"
        << "\"code\":" << code << ","
        << "\"message\":\"" << message << "\","
        << "\"data\":" << data
        << "}";
    return oss.str();
}

std::string RestController::BuildErrorResponse(int code, const std::string& message, const std::string& details) {
    std::ostringstream oss;
    oss << "{"
        << "\"code\":" << code << ","
        << "\"message\":\"" << message << "\"";
    if (!details.empty()) {
        oss << ",\"details\":\"" << details << "\"";
    }
    oss << "}";
    return oss.str();
}

std::string RestController::UrlDecode(const std::string& str) {
    std::ostringstream result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            int value;
            std::istringstream iss(hex);
            iss >> std::hex >> value;
            result << static_cast<char>(value);
            i += 2;
        } else if (str[i] == '+') {
            result << ' ';
        } else {
            result << str[i];
        }
    }
    return result.str();
}

void RestController::ParseQueryParams(const std::string& query, std::map<std::string, std::string>& params) {
    if (query.empty()) return;
    
    size_t start = 0;
    while (start < query.length()) {
        size_t ampPos = query.find('&', start);
        if (ampPos == std::string::npos) ampPos = query.length();
        
        std::string param = query.substr(start, ampPos - start);
        size_t eqPos = param.find('=');
        
        if (eqPos != std::string::npos) {
            std::string key = UrlDecode(param.substr(0, eqPos));
            std::string value = UrlDecode(param.substr(eqPos + 1));
            params[key] = value;
        }
        
        start = ampPos + 1;
    }
}

std::string RestController::ExtractPathParam(const std::string& path, const std::string& pattern, int paramIndex) {
    if (path.length() <= pattern.length()) {
        return "";
    }
    
    return path.substr(pattern.length());
}

}
