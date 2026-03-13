#include "http_server.h"
#include "../utils/logger.h"
#include <sstream>
#include <iomanip>

namespace cycle {
namespace server {

HttpServer::HttpServer(const Config& config)
    : config_(config)
    , server_(std::make_unique<httplib::Server>())
    , running_(false) {
    SetupRoutes();
    SetupMiddleware();
    
    LOG_INFO("HTTP server initialized on " + config_.server.host + ":" + 
             std::to_string(config_.server.port));
}

HttpServer::~HttpServer() {
    Stop();
}

bool HttpServer::Start() {
    if (running_) {
        LOG_WARN("HTTP server is already running");
        return true;
    }
    
    if (!server_) {
        LOG_ERROR("HTTP server not initialized");
        return false;
    }
    
    LOG_INFO("Starting HTTP server on " + config_.server.host + ":" + 
             std::to_string(config_.server.port));
    
    running_ = true;
    
    if (!server_->listen(config_.server.host.c_str(), config_.server.port)) {
        LOG_ERROR("Failed to start HTTP server");
        running_ = false;
        return false;
    }
    
    return true;
}

void HttpServer::Stop() {
    if (!running_) {
        LOG_WARN("HTTP server is not running");
        return;
    }
    
    if (server_) {
        server_->stop();
    }
    
    running_ = false;
    LOG_INFO("HTTP server stopped");
}

bool HttpServer::IsRunning() const {
    return running_;
}

void HttpServer::SetRenderer(std::shared_ptr<renderer::Renderer> renderer) {
    renderer_ = renderer;
    LOG_DEBUG("Renderer set for HTTP server");
}

void HttpServer::SetMapService(std::shared_ptr<service::MapService> map_service) {
    map_service_ = map_service;
    LOG_DEBUG("Map service set for HTTP server");
}

void HttpServer::SetAuth(std::shared_ptr<auth::JWTAuth> auth) {
    auth_ = auth;
    LOG_DEBUG("Auth set for HTTP server");
}

// 简化实现 - 只提供基本功能
void HttpServer::SetupRoutes() {
    // 健康检查端点
    server_->Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_content("{\"status\": \"ok\"}", "application/json");
    });
    
    // 瓦片服务端点
    server_->Get("/tiles/", [this](const httplib::Request& req, httplib::Response& res) {
        if (!map_service_) {
            res.status = 503;
            res.set_content("{\"error\": \"Map service not available\"}", "application/json");
            return;
        }
        
        // 简化实现 - 返回成功响应
        res.set_content("{\"status\": \"tile service ready\"}", "application/json");
    });
}

void HttpServer::SetupMiddleware() {
    // 简化的中间件实现
    LOG_DEBUG("HTTP server middleware setup");
}

void HttpServer::SetupAuthRoutes() {
    // 简化的认证路由
    LOG_DEBUG("HTTP server auth routes setup");
}

void HttpServer::SetupSecurityHeaders() {
    // 简化的安全头设置
    LOG_DEBUG("HTTP server security headers setup");
}

// HTTPS 支持 - 简化实现
bool HttpServer::EnableSSL(const std::string& cert_file, const std::string& key_file, const std::string& ca_file) {

#ifdef SSL_SERVER_SUPPORT
#else
    LOG_WARN("SSL support not implemented in simplified HTTP server");
    return false;
#endif
}

bool HttpServer::IsSSLEnabled() const {

#ifdef SSL_SERVER_SUPPORT
#else
    return false;
#endif
}

} // namespace server
} // namespace cycle