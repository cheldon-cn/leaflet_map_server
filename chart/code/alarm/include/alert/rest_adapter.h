#ifndef ALARM_REST_ADAPTER_H
#define ALARM_REST_ADAPTER_H

#include "alert/rest_controller.h"
#include "alert/alert_push_service.h"
#include "alert/alert_config_service.h"
#include "ogc/alert/rest_controller.h"
#include "alert_type_adapter.h"
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace alarm {

class RestAdapter : public ogc::alert::IRestController {
public:
    RestAdapter();
    ~RestAdapter() override;
    
    void SetAlarmRestController(std::shared_ptr<alert::IRestController> controller);
    std::shared_ptr<alert::IRestController> GetAlarmRestController() const;
    
    void RegisterRoute(const std::string& method, const std::string& path, 
                       ogc::alert::RouteHandler handler) override;
    ogc::alert::HttpResponse HandleRequest(const ogc::alert::HttpRequest& request) override;
    std::vector<ogc::alert::Route> GetRoutes() const override;
    
    void RegisterDefaultRoutes();
    
private:
    ogc::alert::HttpRequest ConvertRequest(const alert::HttpRequest& request);
    alert::HttpRequest ConvertRequest(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse ConvertResponse(const alert::HttpResponse& response);
    alert::HttpResponse ConvertResponse(const ogc::alert::HttpResponse& response);
    
    std::shared_ptr<alert::IRestController> m_alarmController;
    std::vector<ogc::alert::Route> m_routes;
};

class AlertControllerAdapter {
public:
    AlertControllerAdapter();
    ~AlertControllerAdapter();
    
    void SetAlertRepository(ogc::alert::IAlertRepository* repository);
    
    ogc::alert::HttpResponse GetAlerts(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse GetAlertById(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse GetActiveAlerts(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse AcknowledgeAlert(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse GetAlertStatistics(const ogc::alert::HttpRequest& request);
    
private:
    ogc::alert::IAlertRepository* m_repository;
    
    std::string BuildAlertListResponse(const std::vector<alert::Alert>& alerts, int total, int page, int size);
    std::string BuildAlertResponse(const alert::Alert& alert);
    std::string BuildErrorResponse(int code, const std::string& message);
    std::string BuildSuccessResponse(const std::string& data);
    std::string BuildBoolResponse(bool success);
    
    std::map<std::string, std::string> ParseQueryParams(const std::string& query);
    std::map<std::string, std::string> ParseJsonBody(const std::string& body);
};

class ConfigControllerAdapter {
public:
    ConfigControllerAdapter();
    ~ConfigControllerAdapter();
    
    void SetConfigService(alert::IAlertConfigService* service);
    
    ogc::alert::HttpResponse GetConfig(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse UpdateConfig(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse GetThreshold(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse UpdateThreshold(const ogc::alert::HttpRequest& request);
    
private:
    alert::IAlertConfigService* m_configService;
    
    std::string BuildConfigResponse(const alert::ThresholdConfig& threshold, 
                                     const alert::PushConfig& pushConfig);
    std::string BuildThresholdResponse(const alert::ThresholdConfig& threshold);
    std::string BuildSuccessResponse(bool success);
    std::string BuildErrorResponse(int code, const std::string& message);
    
    std::map<std::string, std::string> ParseJsonBody(const std::string& body);
};

class PushControllerAdapter {
public:
    PushControllerAdapter();
    ~PushControllerAdapter();
    
    void SetPushService(alert::IAlertPushService* service);
    
    ogc::alert::HttpResponse GetPushStatus(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse PushAlert(const ogc::alert::HttpRequest& request);
    ogc::alert::HttpResponse EnableChannel(const ogc::alert::HttpRequest& request);
    
private:
    alert::IAlertPushService* m_pushService;
    
    std::string BuildPushStatusResponse(const std::map<alert::PushChannel, bool>& channels);
    std::string BuildPushResultResponse(const std::vector<alert::PushResult>& results);
    std::string BuildSuccessResponse(bool success);
    std::string BuildErrorResponse(int code, const std::string& message);
    
    std::map<std::string, std::string> ParseJsonBody(const std::string& body);
};

ogc::alert::IRestController* CreateRestAdapter(std::shared_ptr<alert::IRestController> controller);

}

#endif
