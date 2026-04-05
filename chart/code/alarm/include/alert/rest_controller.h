#ifndef ALERT_REST_CONTROLLER_H
#define ALERT_REST_CONTROLLER_H

#include "alert/alert.h"
#include "alert/alert_types.h"
#include "alert/coordinate.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace alert {

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::string contentType;
    std::string authorization;
    bool valid;
    
    HttpRequest()
        : valid(false) {}
};

struct HttpResponse {
    int statusCode;
    std::string statusMessage;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string contentType;
    bool valid;
    
    HttpResponse()
        : statusCode(200), statusMessage("OK"), valid(false) {}
};

struct ApiError {
    int code;
    std::string message;
    std::string details;
    
    ApiError()
        : code(0) {}
    
    ApiError(int c, const std::string& msg, const std::string& det = "")
        : code(c), message(msg), details(det) {}
};

struct SubscribeRequest {
    std::string userId;
    std::string shipId;
    std::vector<std::string> alertTypes;
    std::vector<std::string> pushMethods;
    bool valid;
    
    SubscribeRequest()
        : valid(false) {}
};

struct SubscribeResponse {
    std::string subscribeId;
    std::string userId;
    std::string createTime;
    bool valid;
    
    SubscribeResponse()
        : valid(false) {}
};

struct AlertListRequest {
    std::string userId;
    std::string status;
    std::string level;
    std::string type;
    std::string startTime;
    std::string endTime;
    int page;
    int pageSize;
    bool valid;
    
    AlertListRequest()
        : page(1), pageSize(20), valid(false) {}
};

struct AlertListResponse {
    int total;
    int page;
    int pageSize;
    std::vector<Alert> alerts;
    bool valid;
    
    AlertListResponse()
        : total(0), page(1), pageSize(20), valid(false) {}
};

struct AcknowledgeRequest {
    std::string userId;
    std::string actionTaken;
    bool valid;
    
    AcknowledgeRequest()
        : valid(false) {}
};

struct AcknowledgeResponse {
    std::string alertId;
    bool acknowledged;
    std::string acknowledgeTime;
    bool valid;
    
    AcknowledgeResponse()
        : acknowledged(false), valid(false) {}
};

typedef std::function<HttpResponse(const HttpRequest&)> RequestHandler;

class IAlertJudgeService;
class IAlertPushService;
class IAlertConfigService;

class IRestController {
public:
    virtual ~IRestController() {}
    
    virtual void SetAlertJudgeService(std::shared_ptr<IAlertJudgeService> service) = 0;
    virtual void SetAlertPushService(std::shared_ptr<IAlertPushService> service) = 0;
    virtual void SetAlertConfigService(std::shared_ptr<IAlertConfigService> service) = 0;
    
    virtual void RegisterHandler(const std::string& path, 
                                  const std::string& method,
                                  RequestHandler handler) = 0;
    virtual void UnregisterHandler(const std::string& path, 
                                    const std::string& method) = 0;
    
    virtual HttpResponse HandleRequest(const HttpRequest& request) = 0;
    
    virtual bool Start(int port) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    virtual void SetJwtSecret(const std::string& secret) = 0;
    virtual bool ValidateToken(const std::string& token, std::string& userId) = 0;
};

class RestController : public IRestController {
public:
    RestController();
    virtual ~RestController();
    
    void SetAlertJudgeService(std::shared_ptr<IAlertJudgeService> service) override;
    void SetAlertPushService(std::shared_ptr<IAlertPushService> service) override;
    void SetAlertConfigService(std::shared_ptr<IAlertConfigService> service) override;
    
    void RegisterHandler(const std::string& path, 
                          const std::string& method,
                          RequestHandler handler) override;
    void UnregisterHandler(const std::string& path, 
                            const std::string& method) override;
    
    HttpResponse HandleRequest(const HttpRequest& request) override;
    
    bool Start(int port) override;
    void Stop() override;
    bool IsRunning() const override;
    
    void SetJwtSecret(const std::string& secret) override;
    bool ValidateToken(const std::string& token, std::string& userId) override;
    
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    
    std::string GetLastError() const;
    
private:
    std::shared_ptr<IAlertJudgeService> m_judgeService;
    std::shared_ptr<IAlertPushService> m_pushService;
    std::shared_ptr<IAlertConfigService> m_configService;
    
    std::map<std::string, RequestHandler> m_handlers;
    std::string m_jwtSecret;
    bool m_running;
    bool m_enabled;
    int m_port;
    std::string m_lastError;
    
    void RegisterDefaultHandlers();
    
    HttpResponse HandleSubscribe(const HttpRequest& request);
    HttpResponse HandleAlertList(const HttpRequest& request);
    HttpResponse HandleAlertDetail(const HttpRequest& request);
    HttpResponse HandleAcknowledge(const HttpRequest& request);
    HttpResponse HandleConfigGet(const HttpRequest& request);
    HttpResponse HandleConfigSet(const HttpRequest& request);
    
    std::string GenerateSubscribeId();
    std::string GenerateAlertId();
    std::string GetCurrentTimestamp();
    
    bool ParseJson(const std::string& json, std::map<std::string, std::string>& result);
    std::string BuildJsonResponse(int code, const std::string& message, const std::string& data);
    std::string BuildErrorResponse(int code, const std::string& message, const std::string& details = "");
    
    std::string UrlDecode(const std::string& str);
    void ParseQueryParams(const std::string& query, std::map<std::string, std::string>& params);
    std::string ExtractPathParam(const std::string& path, const std::string& pattern, int paramIndex);
};

}

#endif
