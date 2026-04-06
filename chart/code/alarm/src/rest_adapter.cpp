#include "alert/rest_adapter.h"
#include "alert/alert_config_service.h"
#include "alert/alert_push_service.h"
#include <sstream>
#include <algorithm>

namespace alarm {

RestAdapter::RestAdapter() {
}

RestAdapter::~RestAdapter() {
}

void RestAdapter::SetAlarmRestController(std::shared_ptr<alert::IRestController> controller) {
    m_alarmController = controller;
}

std::shared_ptr<alert::IRestController> RestAdapter::GetAlarmRestController() const {
    return m_alarmController;
}

void RestAdapter::RegisterRoute(const std::string& method, const std::string& path, 
                                 ogc::alert::RouteHandler handler) {
    ogc::alert::Route route;
    route.method = method;
    route.path = path;
    route.handler = handler;
    m_routes.push_back(route);
}

ogc::alert::HttpResponse RestAdapter::HandleRequest(const ogc::alert::HttpRequest& request) {
    if (m_alarmController) {
        alert::HttpRequest alarm_request = ConvertRequest(request);
        alert::HttpResponse alarm_response = m_alarmController->HandleRequest(alarm_request);
        return ConvertResponse(alarm_response);
    }
    
    for (const auto& route : m_routes) {
        if (route.method == request.method && route.path == request.path) {
            return route.handler(request);
        }
    }
    
    return ogc::alert::HttpResponse::NotFound("Route not found: " + request.path);
}

std::vector<ogc::alert::Route> RestAdapter::GetRoutes() const {
    return m_routes;
}

void RestAdapter::RegisterDefaultRoutes() {
    RegisterRoute("GET", "/api/v1/alerts", 
        [this](const ogc::alert::HttpRequest& req) {
            AlertControllerAdapter adapter;
            return adapter.GetAlerts(req);
        });
    
    RegisterRoute("GET", "/api/v1/alerts/active",
        [this](const ogc::alert::HttpRequest& req) {
            AlertControllerAdapter adapter;
            return adapter.GetActiveAlerts(req);
        });
    
    RegisterRoute("GET", "/api/v1/alerts/config",
        [this](const ogc::alert::HttpRequest& req) {
            ConfigControllerAdapter adapter;
            return adapter.GetConfig(req);
        });
    
    RegisterRoute("PUT", "/api/v1/alerts/config",
        [this](const ogc::alert::HttpRequest& req) {
            ConfigControllerAdapter adapter;
            return adapter.UpdateConfig(req);
        });
    
    RegisterRoute("GET", "/api/v1/alerts/push/status",
        [this](const ogc::alert::HttpRequest& req) {
            PushControllerAdapter adapter;
            return adapter.GetPushStatus(req);
        });
}

ogc::alert::HttpRequest RestAdapter::ConvertRequest(const alert::HttpRequest& request) {
    ogc::alert::HttpRequest result;
    result.method = request.method;
    result.path = request.path;
    result.body = request.body;
    result.headers = request.headers;
    result.query_params = request.queryParams;
    result.content_type = request.contentType;
    return result;
}

alert::HttpRequest RestAdapter::ConvertRequest(const ogc::alert::HttpRequest& request) {
    alert::HttpRequest result;
    result.method = request.method;
    result.path = request.path;
    result.body = request.body;
    result.headers = request.headers;
    result.queryParams = request.query_params;
    result.contentType = request.content_type;
    result.valid = true;
    return result;
}

ogc::alert::HttpResponse RestAdapter::ConvertResponse(const alert::HttpResponse& response) {
    ogc::alert::HttpResponse result;
    result.status_code = response.statusCode;
    result.body = response.body;
    result.headers = response.headers;
    result.content_type = response.contentType;
    return result;
}

alert::HttpResponse RestAdapter::ConvertResponse(const ogc::alert::HttpResponse& response) {
    alert::HttpResponse result;
    result.statusCode = response.status_code;
    result.body = response.body;
    result.headers = response.headers;
    result.contentType = response.content_type;
    result.valid = true;
    return result;
}

AlertControllerAdapter::AlertControllerAdapter()
    : m_repository(nullptr) {
}

AlertControllerAdapter::~AlertControllerAdapter() {
}

void AlertControllerAdapter::SetAlertRepository(ogc::alert::IAlertRepository* repository) {
    m_repository = repository;
}

ogc::alert::HttpResponse AlertControllerAdapter::GetAlerts(const ogc::alert::HttpRequest& request) {
    auto params = ParseQueryParams(request.query_params.empty() ? "" : request.path);
    
    int page = 1;
    int size = 20;
    std::string status;
    std::string type;
    
    auto it = params.find("page");
    if (it != params.end()) {
        page = std::stoi(it->second);
    }
    it = params.find("size");
    if (it != params.end()) {
        size = std::stoi(it->second);
    }
    it = params.find("status");
    if (it != params.end()) {
        status = it->second;
    }
    it = params.find("type");
    if (it != params.end()) {
        type = it->second;
    }
    
    std::vector<alert::Alert> alerts;
    int total = 0;
    
    std::string body = BuildAlertListResponse(alerts, total, page, size);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse AlertControllerAdapter::GetAlertById(const ogc::alert::HttpRequest& request) {
    std::string path = request.path;
    size_t pos = path.rfind('/');
    if (pos == std::string::npos) {
        return ogc::alert::HttpResponse::BadRequest("Invalid alert ID");
    }
    
    std::string alertId = path.substr(pos + 1);
    
    alert::Alert alert;
    alert.SetAlertId(alertId);
    
    std::string body = BuildAlertResponse(alert);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse AlertControllerAdapter::GetActiveAlerts(const ogc::alert::HttpRequest& request) {
    std::vector<alert::Alert> alerts;
    
    std::string body = BuildAlertListResponse(alerts, 0, 1, 20);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse AlertControllerAdapter::AcknowledgeAlert(const ogc::alert::HttpRequest& request) {
    std::string path = request.path;
    size_t pos = path.find("/acknowledge");
    if (pos == std::string::npos) {
        return ogc::alert::HttpResponse::BadRequest("Invalid acknowledge path");
    }
    
    std::string alertId = path.substr(0, pos);
    pos = alertId.rfind('/');
    if (pos != std::string::npos) {
        alertId = alertId.substr(pos + 1);
    }
    
    auto body_params = ParseJsonBody(request.body);
    std::string actionTaken;
    auto it = body_params.find("action_taken");
    if (it != body_params.end()) {
        actionTaken = it->second;
    }
    
    std::string response_body = BuildBoolResponse(true);
    return ogc::alert::HttpResponse::Ok(response_body);
}

ogc::alert::HttpResponse AlertControllerAdapter::GetAlertStatistics(const ogc::alert::HttpRequest& request) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"total_alerts\":0,";
    oss << "\"active_alerts\":0,";
    oss << "\"acknowledged_alerts\":0,";
    oss << "\"cleared_alerts\":0";
    oss << "}}";
    
    return ogc::alert::HttpResponse::Ok(oss.str());
}

std::string AlertControllerAdapter::BuildAlertListResponse(const std::vector<alert::Alert>& alerts, 
                                                            int total, int page, int size) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"alerts\":[],";
    oss << "\"total\":" << total << ",";
    oss << "\"page\":" << page << ",";
    oss << "\"size\":" << size;
    oss << "}}";
    return oss.str();
}

std::string AlertControllerAdapter::BuildAlertResponse(const alert::Alert& alert) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"alert\":{";
    oss << "\"alert_id\":\"" << alert.GetAlertId() << "\",";
    oss << "\"alert_type\":\"" << alert::AlertTypeToString(alert.GetAlertType()) << "\",";
    oss << "\"alert_level\":" << static_cast<int>(alert.GetAlertLevel()) << ",";
    oss << "\"status\":\"" << alert::AlertStatusToString(alert.GetStatus()) << "\",";
    oss << "\"title\":\"" << alert.GetTitle() << "\",";
    oss << "\"message\":\"" << alert.GetMessage() << "\"";
    oss << "}}}";
    return oss.str();
}

std::string AlertControllerAdapter::BuildErrorResponse(int code, const std::string& message) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":" << code << ",";
    oss << "\"message\":\"" << message << "\"";
    oss << "}";
    return oss.str();
}

std::string AlertControllerAdapter::BuildSuccessResponse(const std::string& data) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":" << data;
    oss << "}";
    return oss.str();
}

std::string AlertControllerAdapter::BuildBoolResponse(bool success) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"success\":" << (success ? "true" : "false");
    oss << "}}";
    return oss.str();
}

std::map<std::string, std::string> AlertControllerAdapter::ParseQueryParams(const std::string& query) {
    std::map<std::string, std::string> params;
    
    size_t pos = query.find('?');
    std::string queryStr = (pos != std::string::npos) ? query.substr(pos + 1) : query;
    
    std::istringstream iss(queryStr);
    std::string pair;
    while (std::getline(iss, pair, '&')) {
        size_t eqPos = pair.find('=');
        if (eqPos != std::string::npos) {
            std::string key = pair.substr(0, eqPos);
            std::string value = pair.substr(eqPos + 1);
            params[key] = value;
        }
    }
    
    return params;
}

std::map<std::string, std::string> AlertControllerAdapter::ParseJsonBody(const std::string& body) {
    std::map<std::string, std::string> params;
    
    if (body.empty()) {
        return params;
    }
    
    size_t pos = 0;
    while ((pos = body.find('"', pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        size_t keyEnd = body.find('"', keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = body.substr(keyStart, keyEnd - keyStart);
        
        size_t colonPos = body.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = body.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string value;
        if (body[valueStart] == '"') {
            size_t valueEnd = body.find('"', valueStart + 1);
            if (valueEnd != std::string::npos) {
                value = body.substr(valueStart + 1, valueEnd - valueStart - 1);
            }
        } else {
            size_t valueEnd = body.find_first_of(",}]", valueStart);
            if (valueEnd != std::string::npos) {
                value = body.substr(valueStart, valueEnd - valueStart);
                while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
                    value.pop_back();
                }
            }
        }
        
        params[key] = value;
        pos = body.find(',', valueStart);
        if (pos == std::string::npos) break;
    }
    
    return params;
}

ConfigControllerAdapter::ConfigControllerAdapter()
    : m_configService(nullptr) {
}

ConfigControllerAdapter::~ConfigControllerAdapter() {
}

void ConfigControllerAdapter::SetConfigService(alert::IAlertConfigService* service) {
    m_configService = service;
}

ogc::alert::HttpResponse ConfigControllerAdapter::GetConfig(const ogc::alert::HttpRequest& request) {
    alert::ThresholdConfig threshold;
    alert::PushConfig pushConfig;
    
    std::string body = BuildConfigResponse(threshold, pushConfig);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse ConfigControllerAdapter::UpdateConfig(const ogc::alert::HttpRequest& request) {
    auto params = ParseJsonBody(request.body);
    
    std::string body = BuildSuccessResponse(true);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse ConfigControllerAdapter::GetThreshold(const ogc::alert::HttpRequest& request) {
    alert::ThresholdConfig threshold;
    
    std::string body = BuildThresholdResponse(threshold);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse ConfigControllerAdapter::UpdateThreshold(const ogc::alert::HttpRequest& request) {
    auto params = ParseJsonBody(request.body);
    
    std::string body = BuildSuccessResponse(true);
    return ogc::alert::HttpResponse::Ok(body);
}

std::string ConfigControllerAdapter::BuildConfigResponse(const alert::ThresholdConfig& threshold,
                                                          const alert::PushConfig& pushConfig) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"thresholds\":{";
    oss << "\"depth\":{";
    oss << "\"level1\":" << threshold.GetLevel1Threshold() << ",";
    oss << "\"level2\":" << threshold.GetLevel2Threshold() << ",";
    oss << "\"level3\":" << threshold.GetLevel3Threshold() << ",";
    oss << "\"level4\":" << threshold.GetLevel4Threshold();
    oss << "}},";
    oss << "\"push_methods\":[\"app\",\"sound\",\"sms\",\"email\"],";
    oss << "\"enabled_channels\":{";
    oss << "\"app\":" << (pushConfig.enableWebSocket ? "true" : "false") << ",";
    oss << "\"sound\":true,";
    oss << "\"sms\":" << (pushConfig.enableSms ? "true" : "false") << ",";
    oss << "\"email\":" << (pushConfig.enableEmail ? "true" : "false");
    oss << "}}}";
    return oss.str();
}

std::string ConfigControllerAdapter::BuildThresholdResponse(const alert::ThresholdConfig& threshold) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"level1\":" << threshold.GetLevel1Threshold() << ",";
    oss << "\"level2\":" << threshold.GetLevel2Threshold() << ",";
    oss << "\"level3\":" << threshold.GetLevel3Threshold() << ",";
    oss << "\"level4\":" << threshold.GetLevel4Threshold();
    oss << "}}";
    return oss.str();
}

std::string ConfigControllerAdapter::BuildSuccessResponse(bool success) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"updated\":" << (success ? "true" : "false");
    oss << "}}";
    return oss.str();
}

std::string ConfigControllerAdapter::BuildErrorResponse(int code, const std::string& message) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":" << code << ",";
    oss << "\"message\":\"" << message << "\"";
    oss << "}";
    return oss.str();
}

std::map<std::string, std::string> ConfigControllerAdapter::ParseJsonBody(const std::string& body) {
    std::map<std::string, std::string> params;
    
    if (body.empty()) {
        return params;
    }
    
    size_t pos = 0;
    while ((pos = body.find('"', pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        size_t keyEnd = body.find('"', keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = body.substr(keyStart, keyEnd - keyStart);
        
        size_t colonPos = body.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = body.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string value;
        if (body[valueStart] == '"') {
            size_t valueEnd = body.find('"', valueStart + 1);
            if (valueEnd != std::string::npos) {
                value = body.substr(valueStart + 1, valueEnd - valueStart - 1);
            }
        } else {
            size_t valueEnd = body.find_first_of(",}]", valueStart);
            if (valueEnd != std::string::npos) {
                value = body.substr(valueStart, valueEnd - valueStart);
                while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
                    value.pop_back();
                }
            }
        }
        
        params[key] = value;
        pos = body.find(',', valueStart);
        if (pos == std::string::npos) break;
    }
    
    return params;
}

PushControllerAdapter::PushControllerAdapter()
    : m_pushService(nullptr) {
}

PushControllerAdapter::~PushControllerAdapter() {
}

void PushControllerAdapter::SetPushService(alert::IAlertPushService* service) {
    m_pushService = service;
}

ogc::alert::HttpResponse PushControllerAdapter::GetPushStatus(const ogc::alert::HttpRequest& request) {
    std::map<alert::PushChannel, bool> channels;
    channels[alert::PushChannel::kWebSocket] = true;
    channels[alert::PushChannel::kSms] = false;
    channels[alert::PushChannel::kEmail] = false;
    channels[alert::PushChannel::kHttp] = false;
    
    std::string body = BuildPushStatusResponse(channels);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse PushControllerAdapter::PushAlert(const ogc::alert::HttpRequest& request) {
    auto params = ParseJsonBody(request.body);
    
    std::vector<alert::PushResult> results;
    
    std::string body = BuildPushResultResponse(results);
    return ogc::alert::HttpResponse::Ok(body);
}

ogc::alert::HttpResponse PushControllerAdapter::EnableChannel(const ogc::alert::HttpRequest& request) {
    auto params = ParseJsonBody(request.body);
    
    std::string body = BuildSuccessResponse(true);
    return ogc::alert::HttpResponse::Ok(body);
}

std::string PushControllerAdapter::BuildPushStatusResponse(const std::map<alert::PushChannel, bool>& channels) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"channels\":{";
    oss << "\"app\":{\"enabled\":" << (channels.at(alert::PushChannel::kWebSocket) ? "true" : "false") << ",\"available\":true},";
    oss << "\"sound\":{\"enabled\":true,\"available\":true},";
    oss << "\"sms\":{\"enabled\":" << (channels.at(alert::PushChannel::kSms) ? "true" : "false") << ",\"available\":true},";
    oss << "\"email\":{\"enabled\":" << (channels.at(alert::PushChannel::kEmail) ? "true" : "false") << ",\"available\":false}";
    oss << "}}}";
    return oss.str();
}

std::string PushControllerAdapter::BuildPushResultResponse(const std::vector<alert::PushResult>& results) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"push_results\":[]";
    oss << "}}";
    return oss.str();
}

std::string PushControllerAdapter::BuildSuccessResponse(bool success) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":0,";
    oss << "\"message\":\"success\",";
    oss << "\"data\":{";
    oss << "\"success\":" << (success ? "true" : "false");
    oss << "}}";
    return oss.str();
}

std::string PushControllerAdapter::BuildErrorResponse(int code, const std::string& message) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"code\":" << code << ",";
    oss << "\"message\":\"" << message << "\"";
    oss << "}";
    return oss.str();
}

std::map<std::string, std::string> PushControllerAdapter::ParseJsonBody(const std::string& body) {
    std::map<std::string, std::string> params;
    
    if (body.empty()) {
        return params;
    }
    
    size_t pos = 0;
    while ((pos = body.find('"', pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        size_t keyEnd = body.find('"', keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = body.substr(keyStart, keyEnd - keyStart);
        
        size_t colonPos = body.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = body.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        std::string value;
        if (body[valueStart] == '"') {
            size_t valueEnd = body.find('"', valueStart + 1);
            if (valueEnd != std::string::npos) {
                value = body.substr(valueStart + 1, valueEnd - valueStart - 1);
            }
        } else {
            size_t valueEnd = body.find_first_of(",}]", valueStart);
            if (valueEnd != std::string::npos) {
                value = body.substr(valueStart, valueEnd - valueStart);
                while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
                    value.pop_back();
                }
            }
        }
        
        params[key] = value;
        pos = body.find(',', valueStart);
        if (pos == std::string::npos) break;
    }
    
    return params;
}

ogc::alert::IRestController* CreateRestAdapter(std::shared_ptr<alert::IRestController> controller) {
    RestAdapter* adapter = new RestAdapter();
    adapter->SetAlarmRestController(controller);
    adapter->RegisterDefaultRoutes();
    return adapter;
}

}
