#include "ogc/alert/rest_controller.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/alert/config_service.h"
#include "ogc/alert/query_service.h"
#include "ogc/alert/acknowledge_service.h"
#include "ogc/alert/threshold_manager.h"
#include "ogc/alert/exception.h"
#include "ogc/draw/log.h"
#include <sstream>
#include <algorithm>
#include <cstring>
#include <vector>

namespace ogc {
namespace alert {

HttpResponse HttpResponse::Ok(const std::string& body) {
    HttpResponse resp;
    resp.status_code = 200;
    resp.body = body;
    resp.content_type = "application/json";
    return resp;
}

HttpResponse HttpResponse::Created(const std::string& body) {
    HttpResponse resp;
    resp.status_code = 201;
    resp.body = body;
    resp.content_type = "application/json";
    return resp;
}

HttpResponse HttpResponse::BadRequest(const std::string& message) {
    HttpResponse resp;
    resp.status_code = 400;
    resp.body = "{\"error\":\"" + message + "\"}";
    resp.content_type = "application/json";
    return resp;
}

HttpResponse HttpResponse::NotFound(const std::string& message) {
    HttpResponse resp;
    resp.status_code = 404;
    resp.body = "{\"error\":\"" + message + "\"}";
    resp.content_type = "application/json";
    return resp;
}

HttpResponse HttpResponse::InternalError(const std::string& message) {
    HttpResponse resp;
    resp.status_code = 500;
    resp.body = "{\"error\":\"" + message + "\"}";
    resp.content_type = "application/json";
    return resp;
}

HttpResponse HttpResponse::Json(int status, const std::string& json_body) {
    HttpResponse resp;
    resp.status_code = status;
    resp.body = json_body;
    resp.content_type = "application/json";
    return resp;
}

class RestControllerImpl : public IRestController {
public:
    void RegisterRoute(const std::string& method, const std::string& path, RouteHandler handler) override {
        Route route;
        route.method = method;
        route.path = path;
        route.handler = handler;
        m_routes.push_back(route);
    }
    
    HttpResponse HandleRequest(const HttpRequest& request) override {
        for (const auto& route : m_routes) {
            if (route.method == request.method && route.path == request.path) {
                try {
                    return route.handler(request);
                } catch (const AlertException& e) {
                    return HttpResponse::InternalError(e.what());
                } catch (const std::exception& e) {
                    return HttpResponse::InternalError(e.what());
                }
            }
        }
        return HttpResponse::NotFound("Route not found");
    }
    
    std::vector<Route> GetRoutes() const override {
        return m_routes;
    }
    
private:
    std::vector<Route> m_routes;
};

std::unique_ptr<IRestController> IRestController::Create() {
    return std::unique_ptr<IRestController>(new RestControllerImpl());
}

AlertController::AlertController()
    : m_repository(nullptr) {
}

AlertController::~AlertController() {
}

HttpResponse AlertController::GetAlerts(const HttpRequest& request) {
    if (!m_repository) {
        return HttpResponse::InternalError("Repository not configured");
    }
    
    auto it = request.query_params.find("limit");
    int limit = it != request.query_params.end() ? std::stoi(it->second) : 100;
    
    it = request.query_params.find("offset");
    int offset = it != request.query_params.end() ? std::stoi(it->second) : 0;
    
    std::vector<AlertPtr> alerts = m_repository->GetAlerts(limit, offset);
    
    std::string json = "{\"alerts\":" + AlertsToJson(alerts) + 
                       ",\"total\":" + std::to_string(alerts.size()) + "}";
    return HttpResponse::Ok(json);
}

HttpResponse AlertController::GetAlertById(const HttpRequest& request) {
    if (!m_repository) {
        return HttpResponse::InternalError("Repository not configured");
    }
    
    auto it = request.query_params.find("id");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert id");
    }
    
    AlertPtr alert = m_repository->GetAlertById(it->second);
    if (!alert) {
        return HttpResponse::NotFound("Alert not found");
    }
    
    return HttpResponse::Ok(AlertToJson(alert));
}

HttpResponse AlertController::GetActiveAlerts(const HttpRequest& request) {
    if (!m_repository) {
        return HttpResponse::InternalError("Repository not configured");
    }
    
    std::vector<AlertPtr> alerts = m_repository->GetActiveAlerts();
    return HttpResponse::Ok("{\"alerts\":" + AlertsToJson(alerts) + "}");
}

HttpResponse AlertController::AcknowledgeAlert(const HttpRequest& request) {
    if (!m_repository) {
        return HttpResponse::InternalError("Repository not configured");
    }
    
    auto it = request.query_params.find("id");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert id");
    }
    
    std::string user_id;
    auto userIt = request.query_params.find("user_id");
    if (userIt != request.query_params.end()) {
        user_id = userIt->second;
    }
    
    bool success = m_repository->AcknowledgeAlert(it->second, user_id);
    if (!success) {
        return HttpResponse::NotFound("Alert not found");
    }
    
    return HttpResponse::Ok("{\"success\":true}");
}

HttpResponse AlertController::GetAlertStatistics(const HttpRequest& request) {
    if (!m_repository) {
        return HttpResponse::InternalError("Repository not configured");
    }
    
    AlertStatistics stats = m_repository->GetStatistics();
    return HttpResponse::Ok(StatisticsToJson(stats));
}

void AlertController::SetAlertRepository(IAlertRepository* repository) {
    m_repository = repository;
}

std::string AlertController::AlertsToJson(const std::vector<AlertPtr>& alerts) const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < alerts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << AlertToJson(alerts[i]);
    }
    oss << "]";
    return oss.str();
}

std::string AlertController::AlertToJson(const AlertPtr& alert) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"id\":\"" << alert->alert_id << "\",";
    oss << "\"type\":" << static_cast<int>(alert->alert_type) << ",";
    oss << "\"level\":" << static_cast<int>(alert->alert_level) << ",";
    oss << "\"status\":" << static_cast<int>(alert->status) << ",";
    oss << "\"position\":{";
    oss << "\"lat\":" << alert->position.latitude << ",";
    oss << "\"lon\":" << alert->position.longitude;
    oss << "},";
    oss << "\"issue_time\":\"" << alert->issue_time.ToString() << "\",";
    oss << "\"content\":\"" << alert->content.message << "\"";
    oss << "}";
    return oss.str();
}

std::string AlertController::StatisticsToJson(const AlertStatistics& stats) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"total_alerts\":" << stats.total_alerts << ",";
    oss << "\"active_alerts\":" << stats.active_alerts << ",";
    oss << "\"acknowledged_alerts\":" << stats.acknowledged_alerts << ",";
    oss << "\"cleared_alerts\":" << stats.cleared_alerts << ",";
    oss << "\"by_level\":{";
    oss << "\"level1\":" << stats.by_level[static_cast<int>(AlertLevel::kLevel1)] << ",";
    oss << "\"level2\":" << stats.by_level[static_cast<int>(AlertLevel::kLevel2)] << ",";
    oss << "\"level3\":" << stats.by_level[static_cast<int>(AlertLevel::kLevel3)] << ",";
    oss << "\"level4\":" << stats.by_level[static_cast<int>(AlertLevel::kLevel4)];
    oss << "}";
    oss << "}";
    return oss.str();
}

ConfigController::ConfigController()
    : m_configService(nullptr)
    , m_thresholdManager(nullptr) {
}

ConfigController::~ConfigController() {
}

HttpResponse ConfigController::GetConfig(const HttpRequest& request) {
    if (!m_configService) {
        return HttpResponse::InternalError("Config service not configured");
    }
    
    EngineConfig config = m_configService->GetConfig();
    return HttpResponse::Ok(ConfigToJson(config));
}

HttpResponse ConfigController::UpdateConfig(const HttpRequest& request) {
    if (!m_configService) {
        return HttpResponse::InternalError("Config service not configured");
    }
    
    std::map<std::string, std::string> params = RestRouter::JsonDecode(request.body);
    
    EngineConfig config = m_configService->GetConfig();
    
    auto it = params.find("check_interval_ms");
    if (it != params.end()) {
        config.check_interval_ms = std::stoi(it->second);
    }
    
    it = params.find("max_alerts_per_check");
    if (it != params.end()) {
        config.max_alerts_per_check = std::stoi(it->second);
    }
    
    m_configService->UpdateConfig(config);
    
    return HttpResponse::Ok("{\"success\":true}");
}

HttpResponse ConfigController::GetThreshold(const HttpRequest& request) {
    if (!m_thresholdManager) {
        return HttpResponse::InternalError("Threshold manager not configured");
    }
    
    auto it = request.query_params.find("type");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert type");
    }
    
    int type_value = std::stoi(it->second);
    AlertType type = static_cast<AlertType>(type_value);
    
    ThresholdConfig config = m_thresholdManager->GetThreshold(type);
    return HttpResponse::Ok(ThresholdToJson(config));
}

HttpResponse ConfigController::UpdateThreshold(const HttpRequest& request) {
    if (!m_thresholdManager) {
        return HttpResponse::InternalError("Threshold manager not configured");
    }
    
    std::map<std::string, std::string> params = RestRouter::JsonDecode(request.body);
    
    auto it = params.find("type");
    if (it == params.end()) {
        return HttpResponse::BadRequest("Missing alert type");
    }
    
    int type_value = std::stoi(it->second);
    AlertType type = static_cast<AlertType>(type_value);
    
    ThresholdConfig config = m_thresholdManager->GetThreshold(type);
    
    it = params.find("level1");
    if (it != params.end()) {
        config.value.level1 = std::stod(it->second);
    }
    
    it = params.find("level2");
    if (it != params.end()) {
        config.value.level2 = std::stod(it->second);
    }
    
    it = params.find("level3");
    if (it != params.end()) {
        config.value.level3 = std::stod(it->second);
    }
    
    it = params.find("level4");
    if (it != params.end()) {
        config.value.level4 = std::stod(it->second);
    }
    
    m_thresholdManager->SetThreshold(config);
    
    return HttpResponse::Ok("{\"success\":true}");
}

HttpResponse ConfigController::GetUserConfig(const HttpRequest& request) {
    if (!m_thresholdManager) {
        return HttpResponse::InternalError("Threshold manager not configured");
    }
    
    auto it = request.query_params.find("user_id");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing user_id");
    }
    
    std::string user_id = it->second;
    
    it = request.query_params.find("type");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert type");
    }
    
    int type_value = std::stoi(it->second);
    AlertType type = static_cast<AlertType>(type_value);
    
    UserThresholdOverride override_config = m_thresholdManager->GetUserOverride(user_id, type);
    
    std::ostringstream oss;
    oss << "{";
    oss << "\"user_id\":\"" << override_config.user_id << "\",";
    oss << "\"type\":" << static_cast<int>(override_config.alert_type) << ",";
    oss << "\"enabled\":" << (override_config.enabled ? "true" : "false") << ",";
    oss << "\"value\":{";
    oss << "\"level1\":" << override_config.value.level1 << ",";
    oss << "\"level2\":" << override_config.value.level2 << ",";
    oss << "\"level3\":" << override_config.value.level3 << ",";
    oss << "\"level4\":" << override_config.value.level4;
    oss << "}}";
    
    return HttpResponse::Ok(oss.str());
}

HttpResponse ConfigController::UpdateUserConfig(const HttpRequest& request) {
    if (!m_thresholdManager) {
        return HttpResponse::InternalError("Threshold manager not configured");
    }
    
    std::map<std::string, std::string> params = RestRouter::JsonDecode(request.body);
    
    UserThresholdOverride override_config;
    
    auto it = params.find("user_id");
    if (it != params.end()) {
        override_config.user_id = it->second;
    }
    
    it = params.find("type");
    if (it != params.end()) {
        override_config.alert_type = static_cast<AlertType>(std::stoi(it->second));
    }
    
    it = params.find("enabled");
    if (it != params.end()) {
        override_config.enabled = (it->second == "true");
    }
    
    it = params.find("level1");
    if (it != params.end()) {
        override_config.value.level1 = std::stod(it->second);
    }
    
    it = params.find("level2");
    if (it != params.end()) {
        override_config.value.level2 = std::stod(it->second);
    }
    
    it = params.find("level3");
    if (it != params.end()) {
        override_config.value.level3 = std::stod(it->second);
    }
    
    it = params.find("level4");
    if (it != params.end()) {
        override_config.value.level4 = std::stod(it->second);
    }
    
    m_thresholdManager->SetUserOverride(override_config);
    
    return HttpResponse::Ok("{\"success\":true}");
}

void ConfigController::SetConfigService(IConfigService* service) {
    m_configService = service;
}

void ConfigController::SetThresholdManager(IThresholdManager* manager) {
    m_thresholdManager = manager;
}

std::string ConfigController::ConfigToJson(const EngineConfig& config) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"check_interval_ms\":" << config.check_interval_ms << ",";
    oss << "\"max_alerts_per_check\":" << config.max_alerts_per_check;
    oss << "}";
    return oss.str();
}

std::string ConfigController::ThresholdToJson(const ThresholdConfig& config) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"id\":\"" << config.config_id << "\",";
    oss << "\"name\":\"" << config.name << "\",";
    oss << "\"type\":" << static_cast<int>(config.alert_type) << ",";
    oss << "\"value\":{";
    oss << "\"level1\":" << config.value.level1 << ",";
    oss << "\"level2\":" << config.value.level2 << ",";
    oss << "\"level3\":" << config.value.level3 << ",";
    oss << "\"level4\":" << config.value.level4 << ",";
    oss << "\"safety_margin\":" << config.value.safety_margin << ",";
    oss << "\"unit\":\"" << config.value.unit << "\"";
    oss << "}}";
    return oss.str();
}

QueryController::QueryController()
    : m_queryService(nullptr) {
}

QueryController::~QueryController() {
}

HttpResponse QueryController::QueryAlerts(const HttpRequest& request) {
    if (!m_queryService) {
        return HttpResponse::InternalError("Query service not configured");
    }
    
    AlertQueryParams params;
    
    auto it = request.query_params.find("page");
    if (it != request.query_params.end()) {
        params.page = std::stoi(it->second);
    }
    
    it = request.query_params.find("page_size");
    if (it != request.query_params.end()) {
        params.page_size = std::stoi(it->second);
    }
    
    AlertListResult result = m_queryService->GetAlertList(params);
    
    std::ostringstream oss;
    oss << "{\"alerts\":[";
    for (size_t i = 0; i < result.alerts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << result.alerts[i]->alert_id << "\"}";
    }
    oss << "],\"total\":" << result.total_count << "}";
    
    return HttpResponse::Ok(oss.str());
}

HttpResponse QueryController::QueryAlertsByType(const HttpRequest& request) {
    if (!m_queryService) {
        return HttpResponse::InternalError("Query service not configured");
    }
    
    auto it = request.query_params.find("type");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert type");
    }
    
    int type_value = std::stoi(it->second);
    AlertType type = static_cast<AlertType>(type_value);
    
    it = request.query_params.find("user_id");
    std::string user_id = it != request.query_params.end() ? it->second : "";
    
    std::vector<AlertPtr> alerts = m_queryService->GetAlertsByType(user_id, type);
    
    std::ostringstream oss;
    oss << "{\"alerts\":[";
    for (size_t i = 0; i < alerts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << alerts[i]->alert_id << "\"}";
    }
    oss << "]}";
    
    return HttpResponse::Ok(oss.str());
}

HttpResponse QueryController::QueryAlertsByLevel(const HttpRequest& request) {
    if (!m_queryService) {
        return HttpResponse::InternalError("Query service not configured");
    }
    
    auto it = request.query_params.find("level");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert level");
    }
    
    int level_value = std::stoi(it->second);
    AlertLevel level = static_cast<AlertLevel>(level_value);
    
    it = request.query_params.find("user_id");
    std::string user_id = it != request.query_params.end() ? it->second : "";
    
    std::vector<AlertPtr> alerts = m_queryService->GetAlertsByLevel(user_id, level);
    
    std::ostringstream oss;
    oss << "{\"alerts\":[";
    for (size_t i = 0; i < alerts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << alerts[i]->alert_id << "\"}";
    }
    oss << "]}";
    
    return HttpResponse::Ok(oss.str());
}

HttpResponse QueryController::QueryAlertsByTimeRange(const HttpRequest& request) {
    if (!m_queryService) {
        return HttpResponse::InternalError("Query service not configured");
    }
    
    auto it = request.query_params.find("start_time");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing start_time");
    }
    int64_t start_ts = std::stoll(it->second);
    
    it = request.query_params.find("end_time");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing end_time");
    }
    int64_t end_ts = std::stoll(it->second);
    
    it = request.query_params.find("user_id");
    std::string user_id = it != request.query_params.end() ? it->second : "";
    
    it = request.query_params.find("page");
    int page = it != request.query_params.end() ? std::stoi(it->second) : 1;
    
    it = request.query_params.find("page_size");
    int page_size = it != request.query_params.end() ? std::stoi(it->second) : 20;
    
    AlertListResult result = m_queryService->GetAlertHistory(user_id, 
        DateTime::FromTimestamp(start_ts), DateTime::FromTimestamp(end_ts),
        page, page_size);
    
    std::ostringstream oss;
    oss << "{\"alerts\":[";
    for (size_t i = 0; i < result.alerts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << result.alerts[i]->alert_id << "\"}";
    }
    oss << "]}";
    
    return HttpResponse::Ok(oss.str());
}

HttpResponse QueryController::QueryAlertsByPosition(const HttpRequest& request) {
    if (!m_queryService) {
        return HttpResponse::InternalError("Query service not configured");
    }
    
    auto it = request.query_params.find("user_id");
    std::string user_id = it != request.query_params.end() ? it->second : "";
    
    std::vector<AlertPtr> alerts = m_queryService->GetActiveAlerts(user_id);
    
    std::ostringstream oss;
    oss << "{\"alerts\":[";
    for (size_t i = 0; i < alerts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << alerts[i]->alert_id << "\"}";
    }
    oss << "]}";
    
    return HttpResponse::Ok(oss.str());
}

void QueryController::SetQueryService(IQueryService* service) {
    m_queryService = service;
}

std::string QueryController::ParseJsonString(const std::map<std::string, std::string>& params,
                                              const std::string& key, const std::string& default_value) const {
    auto it = params.find(key);
    return it != params.end() ? it->second : default_value;
}

int QueryController::ParseJsonInt(const std::map<std::string, std::string>& params,
                                   const std::string& key, int default_value) const {
    auto it = params.find(key);
    return it != params.end() ? std::stoi(it->second) : default_value;
}

double QueryController::ParseJsonDouble(const std::map<std::string, std::string>& params,
                                         const std::string& key, double default_value) const {
    auto it = params.find(key);
    return it != params.end() ? std::stod(it->second) : default_value;
}

AcknowledgeController::AcknowledgeController()
    : m_acknowledgeService(nullptr) {
}

AcknowledgeController::~AcknowledgeController() {
}

HttpResponse AcknowledgeController::Acknowledge(const HttpRequest& request) {
    if (!m_acknowledgeService) {
        return HttpResponse::InternalError("Acknowledge service not configured");
    }
    
    std::map<std::string, std::string> params = RestRouter::JsonDecode(request.body);
    
    auto it = params.find("alert_id");
    if (it == params.end()) {
        return HttpResponse::BadRequest("Missing alert_id");
    }
    std::string alert_id = it->second;
    
    it = params.find("user_id");
    std::string user_id = it != params.end() ? it->second : "";
    
    it = params.find("note");
    std::string note = it != params.end() ? it->second : "";
    
    bool success = m_acknowledgeService->Acknowledge(alert_id, user_id, note);
    
    return HttpResponse::Ok("{\"success\":" + std::string(success ? "true" : "false") + "}");
}

HttpResponse AcknowledgeController::BatchAcknowledge(const HttpRequest& request) {
    if (!m_acknowledgeService) {
        return HttpResponse::InternalError("Acknowledge service not configured");
    }
    
    std::map<std::string, std::string> params = RestRouter::JsonDecode(request.body);
    
    auto it = params.find("user_id");
    std::string user_id = it != params.end() ? it->second : "";
    
    std::vector<std::string> alert_ids;
    
    int success_count = 0;
    for (const auto& alert_id : alert_ids) {
        if (m_acknowledgeService->Acknowledge(alert_id, user_id, "")) {
            success_count++;
        }
    }
    
    return HttpResponse::Ok("{\"success_count\":" + std::to_string(success_count) + "}");
}

HttpResponse AcknowledgeController::GetAcknowledgeHistory(const HttpRequest& request) {
    if (!m_acknowledgeService) {
        return HttpResponse::InternalError("Acknowledge service not configured");
    }
    
    auto it = request.query_params.find("alert_id");
    if (it == request.query_params.end()) {
        return HttpResponse::BadRequest("Missing alert_id");
    }
    
    std::vector<AcknowledgeRecord> records = m_acknowledgeService->GetHistory(it->second);
    
    std::ostringstream oss;
    oss << "{\"records\":[";
    for (size_t i = 0; i < records.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"alert_id\":\"" << records[i].alert_id << "\",";
        oss << "\"user_id\":\"" << records[i].user_id << "\",";
        oss << "\"time\":\"" << records[i].acknowledge_time.ToString() << "\"}";
    }
    oss << "]}";
    
    return HttpResponse::Ok(oss.str());
}

HttpResponse AcknowledgeController::AddFeedback(const HttpRequest& request) {
    if (!m_acknowledgeService) {
        return HttpResponse::InternalError("Acknowledge service not configured");
    }
    
    std::map<std::string, std::string> params = RestRouter::JsonDecode(request.body);
    
    auto it = params.find("alert_id");
    if (it == params.end()) {
        return HttpResponse::BadRequest("Missing alert_id");
    }
    std::string alert_id = it->second;
    
    it = params.find("feedback");
    std::string feedback = it != params.end() ? it->second : "";
    
    it = params.find("rating");
    int rating = it != params.end() ? std::stoi(it->second) : 0;
    
    bool success = m_acknowledgeService->AddFeedback(alert_id, feedback, rating);
    
    return HttpResponse::Ok("{\"success\":" + std::string(success ? "true" : "false") + "}");
}

void AcknowledgeController::SetAcknowledgeService(IAcknowledgeService* service) {
    m_acknowledgeService = service;
}

RestRouter::RestRouter() {
}

RestRouter::~RestRouter() {
}

void RestRouter::RegisterController(const std::string& base_path, IRestController* controller) {
    auto routes = controller->GetRoutes();
    for (const auto& route : routes) {
        Route r = route;
        r.path = base_path + route.path;
        m_routes.push_back(r);
    }
}

void RestRouter::RegisterRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    Route route;
    route.method = method;
    route.path = path;
    route.handler = handler;
    m_routes.push_back(route);
}

HttpResponse RestRouter::Route(const HttpRequest& request) {
    std::map<std::string, std::string> params;
    
    for (const auto& route : m_routes) {
        if (route.method == request.method && MatchPath(route.path, request.path, params)) {
            try {
                HttpRequest modified_request = request;
                for (const auto& p : params) {
                    modified_request.query_params[p.first] = p.second;
                }
                return route.handler(modified_request);
            } catch (const AlertException& e) {
                if (m_errorHandler) {
                    return m_errorHandler(e.GetCode(), e.what());
                }
                return HttpResponse::InternalError(e.what());
            } catch (const std::exception& e) {
                if (m_errorHandler) {
                    return m_errorHandler(500, e.what());
                }
                return HttpResponse::InternalError(e.what());
            }
        }
    }
    
    if (m_defaultHandler) {
        return m_defaultHandler(request);
    }
    
    return HttpResponse::NotFound("Route not found: " + request.path);
}

void RestRouter::SetDefaultHandler(RouteHandler handler) {
    m_defaultHandler = handler;
}

void RestRouter::SetErrorHandler(std::function<HttpResponse(int, const std::string&)> handler) {
    m_errorHandler = handler;
}

std::vector<Route> RestRouter::GetAllRoutes() const {
    return m_routes;
}

bool RestRouter::MatchPath(const std::string& pattern, const std::string& path,
                            std::map<std::string, std::string>& params) const {
    if (pattern == path) {
        return true;
    }
    
    if (pattern.find('{') == std::string::npos) {
        return pattern == path;
    }
    
    std::vector<std::string> pattern_parts;
    std::vector<std::string> path_parts;
    
    std::string p = pattern;
    std::string token;
    size_t pos = 0;
    while ((pos = p.find('/')) != std::string::npos) {
        token = p.substr(0, pos);
        if (!token.empty()) pattern_parts.push_back(token);
        p.erase(0, pos + 1);
    }
    if (!p.empty()) pattern_parts.push_back(p);
    
    p = path;
    while ((pos = p.find('/')) != std::string::npos) {
        token = p.substr(0, pos);
        if (!token.empty()) path_parts.push_back(token);
        p.erase(0, pos + 1);
    }
    if (!p.empty()) path_parts.push_back(p);
    
    if (pattern_parts.size() != path_parts.size()) {
        return false;
    }
    
    for (size_t i = 0; i < pattern_parts.size(); ++i) {
        const std::string& pp = pattern_parts[i];
        const std::string& hp = path_parts[i];
        
        if (pp.size() >= 2 && pp[0] == '{' && pp[pp.size()-1] == '}') {
            std::string param_name = pp.substr(1, pp.size() - 2);
            params[param_name] = hp;
        } else if (pp != hp) {
            return false;
        }
    }
    
    return true;
}

std::string RestRouter::JsonEncode(const std::map<std::string, std::string>& data) {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ",";
        oss << "\"" << pair.first << "\":\"" << pair.second << "\"";
        first = false;
    }
    oss << "}";
    return oss.str();
}

std::string RestRouter::JsonEncode(const std::vector<std::map<std::string, std::string>>& data) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) oss << ",";
        oss << JsonEncode(data[i]);
    }
    oss << "]";
    return oss.str();
}

std::map<std::string, std::string> RestRouter::JsonDecode(const std::string& json) {
    std::map<std::string, std::string> result;
    
    if (json.empty() || json[0] != '{') {
        return result;
    }
    
    std::string content = json;
    content.erase(std::remove(content.begin(), content.end(), ' '), content.end());
    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
    content.erase(std::remove(content.begin(), content.end(), '\t'), content.end());
    
    if (content.size() < 2) return result;
    content = content.substr(1, content.size() - 2);
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t key_start = content.find('"', pos);
        if (key_start == std::string::npos) break;
        
        size_t key_end = content.find('"', key_start + 1);
        if (key_end == std::string::npos) break;
        
        std::string key = content.substr(key_start + 1, key_end - key_start - 1);
        
        size_t colon = content.find(':', key_end);
        if (colon == std::string::npos) break;
        
        size_t value_start = content.find('"', colon);
        if (value_start == std::string::npos) break;
        
        size_t value_end = content.find('"', value_start + 1);
        if (value_end == std::string::npos) break;
        
        std::string value = content.substr(value_start + 1, value_end - value_start - 1);
        
        result[key] = value;
        pos = value_end + 1;
    }
    
    return result;
}

class RestServer::Impl {
public:
    ServerConfig config;
    RestRouter* router;
    bool running;
    
    Impl() : router(nullptr), running(false) {
        config.host = "0.0.0.0";
        config.port = 8080;
        config.thread_count = 4;
        config.max_connections = 100;
        config.timeout_seconds = 30;
        config.enable_ssl = false;
    }
};

RestServer::RestServer()
    : m_impl(std::make_unique<Impl>()) {
}

RestServer::RestServer(const ServerConfig& config)
    : m_impl(std::make_unique<Impl>()) {
    m_impl->config = config;
}

RestServer::~RestServer() {
    Stop();
}

void RestServer::SetRouter(RestRouter* router) {
    m_impl->router = router;
}

bool RestServer::Start() {
    if (m_impl->running) {
        return true;
    }
    
    LOG_INFO() << "REST server starting on port " << m_impl->config.port;
    
    m_impl->running = true;
    return true;
}

void RestServer::Stop() {
    if (!m_impl->running) {
        return;
    }
    
    LOG_INFO() << "REST server stopping";
    
    m_impl->running = false;
}

bool RestServer::IsRunning() const {
    return m_impl->running;
}

void RestServer::SetConfig(const ServerConfig& config) {
    m_impl->config = config;
}

RestServer::ServerConfig RestServer::GetConfig() const {
    return m_impl->config;
}

int RestServer::GetPort() const {
    return m_impl->config.port;
}

std::string RestServer::GetUrl() const {
    return "http://" + m_impl->config.host + ":" + std::to_string(m_impl->config.port);
}

}
}
