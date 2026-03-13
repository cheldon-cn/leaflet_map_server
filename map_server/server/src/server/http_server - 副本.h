#ifndef CYCLE_SERVER_HTTP_SERVER_H
#define CYCLE_SERVER_HTTP_SERVER_H

#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include <chrono>

#include "../config/config.h"
#include "../renderer/renderer.h"
#include "../service/map_service.h"
#include "../auth/jwt_auth.h"

#include <httplib.h>

namespace cycle {
namespace server {

class HttpServer {
public:
    explicit HttpServer(const Config& config);
    ~HttpServer();
    
    bool Start();
    void Stop();
    bool IsRunning() const;
    
    void SetRenderer(std::shared_ptr<renderer::Renderer> renderer);
    void SetMapService(std::shared_ptr<service::MapService> map_service);
    void SetAuth(std::shared_ptr<auth::JWTAuth> auth);
    
    bool EnableSSL(const std::string& cert_file, const std::string& key_file, const std::string& ca_file = "");
    bool IsSSLEnabled() const;
    
private:
    void SetupRoutes();
    void SetupMiddleware();
    void SetupAuthRoutes();
    void SetupSecurityHeaders();
    
#ifdef SSL_SERVER_SUPPORT
    void SetupRoutesForSSL();
    void SetupMiddlewareForSSL();
#endif
    
    // 认证相关处理器
    void HandleLogin(const httplib::Request& req, httplib::Response& res);
    void HandleLogout(const httplib::Request& req, httplib::Response& res);
    void HandleRefreshToken(const httplib::Request& req, httplib::Response& res);
    void HandleUserInfo(const httplib::Request& req, httplib::Response& res);
    
    // 原有处理器
    void HandleGetTile(const httplib::Request& req, httplib::Response& res);
    void HandleGetCapabilities(const httplib::Request& req, httplib::Response& res);
    void HandleHealth(const httplib::Request& req, httplib::Response& res);
    void HandleMetrics(const httplib::Request& req, httplib::Response& res);
    void HandleGenerateMap(const httplib::Request& req, httplib::Response& res);
    
    // 瓦片边界框 API
    void HandleGetTileBounds(const httplib::Request& req, httplib::Response& res);
    
    // 批量瓦片生成 API
    void HandleBatchGenerateTiles(const httplib::Request& req, httplib::Response& res);
    
    // 配置管理 API
    void HandleGetConfig(const httplib::Request& req, httplib::Response& res);
    void HandleUpdateConfig(const httplib::Request& req, httplib::Response& res);
    void HandleGetSecurityConfig(const httplib::Request& req, httplib::Response& res);
    
    // 监控和统计 API
    void HandleGetMetricsRequests(const httplib::Request& req, httplib::Response& res);
    
    // 缓存管理 API
    void HandleGetCacheStats(const httplib::Request& req, httplib::Response& res);
    void HandleClearCache(const httplib::Request& req, httplib::Response& res);
    void HandleWarmupCache(const httplib::Request& req, httplib::Response& res);
    
    // 安全监控 API
    void HandleGetSecurityEvents(const httplib::Request& req, httplib::Response& res);
    void HandleConfigureSecurityAlerts(const httplib::Request& req, httplib::Response& res);
    void HandleSecurityScan(const httplib::Request& req, httplib::Response& res);
    
    // 性能优化 API
    void HandleStartBenchmark(const httplib::Request& req, httplib::Response& res);
    void HandleGetBenchmarkResults(const httplib::Request& req, httplib::Response& res);
    
    // 批量操作 API
    void HandleBatchTiles(const httplib::Request& req, httplib::Response& res);
    void HandleGetBatchStatus(const httplib::Request& req, httplib::Response& res);
    
    // 辅助方法
    bool ParseTileRequest(const httplib::Request& req, 
                         int& z, int& x, int& y,
                         std::string& format);
    std::string ExtractToken(const httplib::Request& req) const;
    httplib::Server::HandlerResponse ValidateRequest(const httplib::Request& req, httplib::Response& res);
    void AddSecurityHeaders(httplib::Response& res);
    void LogSecurityEvent(const std::string& event, const httplib::Request& req);
    
    std::string GenerateCapabilitiesXML() const;
    std::string GenerateHealthJSON() const;
    std::string GenerateMetricsJSON() const;
    
    static std::string FormatTimeISO8601(const std::chrono::system_clock::time_point& time);
    
    const Config& config_;
#ifdef SSL_SERVER_SUPPORT
    bool ssl_enabled_;
    std::unique_ptr<httplib::SSLServer> ssl_server_;
#else
    std::unique_ptr<httplib::Server> server_;
#endif
    std::shared_ptr<renderer::Renderer> renderer_;
    std::shared_ptr<service::MapService> map_service_;
    std::shared_ptr<auth::JWTAuth> auth_;
    std::shared_ptr<auth::AuthMiddleware> auth_middleware_;
    std::atomic<bool> running_;
    
    // 安全监控
    std::atomic<int> failed_auth_attempts_;
    std::atomic<int> total_requests_;
    std::atomic<int> blocked_requests_;
    std::chrono::system_clock::time_point last_security_check_;
};

} // namespace server
} // namespace cycle

#endif // CYCLE_SERVER_HTTP_SERVER_H
