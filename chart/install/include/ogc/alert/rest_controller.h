#ifndef OGC_ALERT_REST_CONTROLLER_H
#define OGC_ALERT_REST_CONTROLLER_H

#include "export.h"
#include "types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;
    std::string content_type;
};

struct OGC_ALERT_API HttpResponse {
    int status_code;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string content_type;
    
    static HttpResponse Ok(const std::string& body);
    static HttpResponse Created(const std::string& body);
    static HttpResponse BadRequest(const std::string& message);
    static HttpResponse NotFound(const std::string& message);
    static HttpResponse InternalError(const std::string& message);
    static HttpResponse Json(int status, const std::string& json_body);
};

using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

struct Route {
    std::string method;
    std::string path;
    RouteHandler handler;
};

class OGC_ALERT_API IRestController {
public:
    virtual ~IRestController() = default;
    
    virtual void RegisterRoute(const std::string& method, const std::string& path, RouteHandler handler) = 0;
    virtual HttpResponse HandleRequest(const HttpRequest& request) = 0;
    virtual std::vector<Route> GetRoutes() const = 0;
    
    static std::unique_ptr<IRestController> Create();
};

class OGC_ALERT_API AlertController {
public:
    AlertController();
    ~AlertController();
    
    HttpResponse GetAlerts(const HttpRequest& request);
    HttpResponse GetAlertById(const HttpRequest& request);
    HttpResponse GetActiveAlerts(const HttpRequest& request);
    HttpResponse AcknowledgeAlert(const HttpRequest& request);
    HttpResponse GetAlertStatistics(const HttpRequest& request);
    
    void SetAlertRepository(class IAlertRepository* repository);
    
private:
    class IAlertRepository* m_repository;
    
    std::string AlertsToJson(const std::vector<AlertPtr>& alerts) const;
    std::string AlertToJson(const AlertPtr& alert) const;
};

class OGC_ALERT_API ConfigController {
public:
    ConfigController();
    ~ConfigController();
    
    HttpResponse GetConfig(const HttpRequest& request);
    HttpResponse UpdateConfig(const HttpRequest& request);
    HttpResponse GetThreshold(const HttpRequest& request);
    HttpResponse UpdateThreshold(const HttpRequest& request);
    HttpResponse GetUserConfig(const HttpRequest& request);
    HttpResponse UpdateUserConfig(const HttpRequest& request);
    
    void SetConfigService(class IConfigService* service);
    void SetThresholdManager(class ThresholdManager* manager);
    
private:
    class IConfigService* m_configService;
    class ThresholdManager* m_thresholdManager;
};

class OGC_ALERT_API QueryController {
public:
    QueryController();
    ~QueryController();
    
    HttpResponse QueryAlerts(const HttpRequest& request);
    HttpResponse QueryAlertsByType(const HttpRequest& request);
    HttpResponse QueryAlertsByLevel(const HttpRequest& request);
    HttpResponse QueryAlertsByTimeRange(const HttpRequest& request);
    HttpResponse QueryAlertsByPosition(const HttpRequest& request);
    
    void SetQueryService(class IQueryService* service);
    
private:
    class IQueryService* m_queryService;
    
    std::string ParseJsonString(const std::map<std::string, std::string>& params, 
                                 const std::string& key, const std::string& default_value) const;
    int ParseJsonInt(const std::map<std::string, std::string>& params,
                     const std::string& key, int default_value) const;
    double ParseJsonDouble(const std::map<std::string, std::string>& params,
                           const std::string& key, double default_value) const;
};

class OGC_ALERT_API AcknowledgeController {
public:
    AcknowledgeController();
    ~AcknowledgeController();
    
    HttpResponse Acknowledge(const HttpRequest& request);
    HttpResponse BatchAcknowledge(const HttpRequest& request);
    HttpResponse GetAcknowledgeHistory(const HttpRequest& request);
    HttpResponse AddFeedback(const HttpRequest& request);
    
    void SetAcknowledgeService(class IAcknowledgeService* service);
    
private:
    class IAcknowledgeService* m_acknowledgeService;
};

class OGC_ALERT_API RestRouter {
public:
    RestRouter();
    ~RestRouter();
    
    void RegisterController(const std::string& base_path, IRestController* controller);
    void RegisterRoute(const std::string& method, const std::string& path, RouteHandler handler);
    
    HttpResponse HandleRoute(const HttpRequest& request);
    
    void SetDefaultHandler(RouteHandler handler);
    void SetErrorHandler(std::function<HttpResponse(int, const std::string&)> handler);
    
    std::vector<Route> GetAllRoutes() const;
    
    static std::string JsonEncode(const std::map<std::string, std::string>& data);
    static std::string JsonEncode(const std::vector<std::map<std::string, std::string>>& data);
    static std::map<std::string, std::string> JsonDecode(const std::string& json);
    
private:
    std::vector<Route> m_routes;
    RouteHandler m_defaultHandler;
    std::function<HttpResponse(int, const std::string&)> m_errorHandler;
    
    bool MatchPath(const std::string& pattern, const std::string& path, 
                   std::map<std::string, std::string>& params) const;
};

class OGC_ALERT_API RestServer {
public:
    struct ServerConfig {
        std::string host;
        int port;
        int thread_count;
        int max_connections;
        int timeout_seconds;
        bool enable_ssl;
        std::string ssl_cert_path;
        std::string ssl_key_path;
    };
    
    RestServer();
    explicit RestServer(const ServerConfig& config);
    ~RestServer();
    
    void SetRouter(RestRouter* router);
    
    bool Start();
    void Stop();
    bool IsRunning() const;
    
    void SetConfig(const ServerConfig& config);
    ServerConfig GetConfig() const;
    
    int GetPort() const;
    std::string GetUrl() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
