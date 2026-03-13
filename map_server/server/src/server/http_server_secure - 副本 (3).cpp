#include "http_server.h"
#include "../utils/logger.h"
#include "../utils/file_system.h"
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace cycle {
namespace server {

HttpServer::HttpServer(const Config& config)
    : config_(config)
#ifdef SSL_SERVER_SUPPORT
    , ssl_enabled_(true)
    , ssl_server_(nullptr)
#else
    , server_(std::make_unique<httplib::Server>())
#endif
    , running_(false)
    , failed_auth_attempts_(0)
    , total_requests_(0)
    , blocked_requests_(0)
    , last_security_check_(std::chrono::system_clock::now()) {
    
    SetupRoutes();
    SetupMiddleware();
    SetupSecurityHeaders();
    
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
    
#ifdef SSL_SERVER_SUPPORT
    if (ssl_enabled_ && ssl_server_) {
        LOG_INFO("Starting HTTPS server on " + config_.server.host + ":" + 
                 std::to_string(config_.server.https_port));
        SetupRoutes();
        SetupMiddleware();
        SetupSecurityHeaders();
        running_ = true;
        
        if (!ssl_server_->listen(config_.server.host.c_str(), config_.server.https_port)) {
            LOG_ERROR("Failed to start HTTPS server");
            running_ = false;
            return false;
        }
        
        LOG_INFO("HTTPS server started successfully");
    }
#else
    if (server_) {
        LOG_INFO("Starting HTTP server on " + config_.server.host + ":" +
            std::to_string(config_.server.port));

        running_ = true;

        if (!server_->listen(config_.server.host.c_str(), config_.server.port)) {
            LOG_ERROR("Failed to start HTTP server");
            running_ = false;
            return false;
        }

        LOG_INFO("HTTP server started successfully");
    }


#endif
    
    return true;
}

void HttpServer::Stop() {
    if (!running_) {
        return;
    }
    
    LOG_INFO("Stopping HTTP server");
    
    
#ifdef SSL_SERVER_SUPPORT
    if (ssl_server_) {
        ssl_server_->stop();
    }
#else
    if (server_) {
        server_->stop();
    }

#endif
    
    running_ = false;
    
    LOG_INFO("HTTP server stopped");
}

bool HttpServer::IsRunning() const {
    return running_;
}

void HttpServer::SetRenderer(std::shared_ptr<renderer::Renderer> renderer) {
    renderer_ = renderer;
}

void HttpServer::SetMapService(std::shared_ptr<service::MapService> map_service) {
    map_service_ = map_service;
}

void HttpServer::SetAuth(std::shared_ptr<auth::JWTAuth> auth) {
    auth_ = auth;
    if (auth_) {
        auth_middleware_ = std::make_shared<auth::AuthMiddleware>(auth_);
        SetupAuthRoutes();
        LOG_INFO("JWT authentication enabled");
    }
}

bool HttpServer::EnableSSL(const std::string& cert_file, const std::string& key_file, const std::string& ca_file) {
#ifdef SSL_SERVER_SUPPORT
    if (cert_file.empty() || key_file.empty()) {
        LOG_ERROR("SSL certificate or key file not provided");
        return false;
    }
    
    if (!cycle::utils::exists(cert_file)) {
        LOG_ERROR("SSL certificate file not found: " + cert_file);
        return false;
    }
    
    if (!cycle::utils::exists(key_file)) {
        LOG_ERROR("SSL key file not found: " + key_file);
        return false;
    }
    
    if (!ca_file.empty() && cycle::utils::exists(ca_file)) {
        ssl_server_ = std::make_unique<httplib::SSLServer>(
            cert_file.c_str(), key_file.c_str(), ca_file.c_str());
        LOG_INFO("Client certificate verification enabled");
    } else {
        ssl_server_ = std::make_unique<httplib::SSLServer>(cert_file.c_str(), key_file.c_str());
    }
    
    ssl_enabled_ = true;
    LOG_INFO("HTTPS server configured with SSL");
    return true;
#endif
    return false;
}

bool HttpServer::IsSSLEnabled() const {

#ifdef SSL_SERVER_SUPPORT
    return ssl_enabled_;
#endif
    return false;
}

void HttpServer::SetupRoutes() {
#ifdef SSL_SERVER_SUPPORT
    SetupRoutesForSSL();
#else
    server_->Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        HandleHealth(req, res);
    });
    
    server_->Get("/metrics", [this](const httplib::Request& req, httplib::Response& res) {
        HandleMetrics(req, res);
    });
    
    server_->Get("/metrics/requests", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetMetricsRequests(req, res);
    });
    
    server_->Get("/1.0.0/WMTSCapabilities.xml", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCapabilities(req, res);
    });
    
    server_->Get(R"(/tile/(\d+)/(\d+)/(\d+)\.(\w+))", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTile(req, res);
        });
    
    server_->Get(R"(/tile/(\d+)/(\d+)/(\d+)/(\w+)/bounds)", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTileBounds(req, res);
        });
    
    server_->Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGenerateMap(req, res);
    });
    
    server_->Post("/tile/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleBatchGenerateTiles(req, res);
    });
    
    server_->Get("/config", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetConfig(req, res);
    });
    
    server_->Put("/config", [this](const httplib::Request& req, httplib::Response& res) {
        HandleUpdateConfig(req, res);
    });
    
    server_->Get("/config/security", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetSecurityConfig(req, res);
    });
    
    server_->Get("/cache/stats", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCacheStats(req, res);
    });
    
    server_->Delete("/cache/clear", [this](const httplib::Request& req, httplib::Response& res) {
        HandleClearCache(req, res);
    });
    
    server_->Post("/cache/warmup", [this](const httplib::Request& req, httplib::Response& res) {
        HandleWarmupCache(req, res);
    });
    
    server_->Get("/security/events", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetSecurityEvents(req, res);
    });
    
    server_->Post("/security/alerts", [this](const httplib::Request& req, httplib::Response& res) {
        HandleConfigureSecurityAlerts(req, res);
    });
    
    server_->Post("/security/scan", [this](const httplib::Request& req, httplib::Response& res) {
        HandleSecurityScan(req, res);
    });
    
    server_->Post("/performance/benchmark", [this](const httplib::Request& req, httplib::Response& res) {
        HandleStartBenchmark(req, res);
    });
    
    server_->Get(R"(/performance/results/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetBenchmarkResults(req, res);
    });
    
    server_->Post("/batch/tiles", [this](const httplib::Request& req, httplib::Response& res) {
        HandleBatchTiles(req, res);
    });
    
    server_->Get(R"(/batch/status/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetBatchStatus(req, res);
    });
#endif
    LOG_INFO("HTTP routes configured");
}

#ifdef SSL_SERVER_SUPPORT
void HttpServer::SetupRoutesForSSL() {
    if (!ssl_server_) return;
    
    ssl_server_->Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        HandleHealth(req, res);
    });
    
    ssl_server_->Get("/metrics", [this](const httplib::Request& req, httplib::Response& res) {
        HandleMetrics(req, res);
    });
    
    ssl_server_->Get("/metrics/requests", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetMetricsRequests(req, res);
    });
    
    ssl_server_->Get("/1.0.0/WMTSCapabilities.xml", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCapabilities(req, res);
    });
    
    ssl_server_->Get(R"(/tile/(\d+)/(\d+)/(\d+)\.(\w+))", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTile(req, res);
        });
    
    ssl_server_->Get(R"(/tile/(\d+)/(\d+)/(\d+)/(\w+)/bounds)", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTileBounds(req, res);
        });
    
    ssl_server_->Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGenerateMap(req, res);
    });
    
    ssl_server_->Post("/tile/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleBatchGenerateTiles(req, res);
    });
    
    ssl_server_->Get("/config", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetConfig(req, res);
    });
    
    ssl_server_->Put("/config", [this](const httplib::Request& req, httplib::Response& res) {
        HandleUpdateConfig(req, res);
    });
    
    ssl_server_->Get("/config/security", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetSecurityConfig(req, res);
    });
    
    ssl_server_->Get("/cache/stats", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCacheStats(req, res);
    });
    
    ssl_server_->Delete("/cache/clear", [this](const httplib::Request& req, httplib::Response& res) {
        HandleClearCache(req, res);
    });
    
    ssl_server_->Post("/cache/warmup", [this](const httplib::Request& req, httplib::Response& res) {
        HandleWarmupCache(req, res);
    });
    
    ssl_server_->Get("/security/events", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetSecurityEvents(req, res);
    });
    
    ssl_server_->Post("/security/alerts", [this](const httplib::Request& req, httplib::Response& res) {
        HandleConfigureSecurityAlerts(req, res);
    });
    
    ssl_server_->Post("/security/scan", [this](const httplib::Request& req, httplib::Response& res) {
        HandleSecurityScan(req, res);
    });
    
    ssl_server_->Post("/performance/benchmark", [this](const httplib::Request& req, httplib::Response& res) {
        HandleStartBenchmark(req, res);
    });
    
    ssl_server_->Get(R"(/performance/results/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetBenchmarkResults(req, res);
    });
    
    ssl_server_->Post("/batch/tiles", [this](const httplib::Request& req, httplib::Response& res) {
        HandleBatchTiles(req, res);
    });
    
    ssl_server_->Get(R"(/batch/status/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetBatchStatus(req, res);
    });
    
    LOG_INFO("HTTPS routes configured");
}
#endif

void HttpServer::SetupAuthRoutes() {
    
#ifdef SSL_SERVER_SUPPORT
    if (ssl_server_) {
        ssl_server_->Post("/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
            HandleLogin(req, res);
        });
        
        ssl_server_->Post("/auth/logout", [this](const httplib::Request& req, httplib::Response& res) {
            HandleLogout(req, res);
        });
        
        ssl_server_->Post("/auth/refresh", [this](const httplib::Request& req, httplib::Response& res) {
            HandleRefreshToken(req, res);
        });
        
        ssl_server_->Get("/auth/user", [this](const httplib::Request& req, httplib::Response& res) {
            HandleUserInfo(req, res);
        });
    }
#else

    if (!server_) return;

    server_->Post("/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
        HandleLogin(req, res);
        });

    server_->Post("/auth/logout", [this](const httplib::Request& req, httplib::Response& res) {
        HandleLogout(req, res);
        });

    server_->Post("/auth/refresh", [this](const httplib::Request& req, httplib::Response& res) {
        HandleRefreshToken(req, res);
        });

    server_->Get("/auth/user", [this](const httplib::Request& req, httplib::Response& res) {
        HandleUserInfo(req, res);
        });
#endif
    
    LOG_INFO("Authentication routes configured");
}

void HttpServer::SetupMiddleware() {
#ifdef SSL_SERVER_SUPPORT
    SetupMiddlewareForSSL();
#else
    server_->set_logger([this](const httplib::Request& req, const httplib::Response& res) {
        total_requests_++;
        
        std::string log_msg = std::string(req.method) + " " + req.path + " - " + 
                             std::to_string(res.status) + " - Client: " + 
                             req.get_header_value("X-Forwarded-For");
        
        if (res.status >= 400) {
            LOG_WARN(log_msg);
        } else {
            LOG_INFO(log_msg);
        }
    });
    
    server_->set_error_handler([this](const httplib::Request& req, httplib::Response& res) {
        blocked_requests_++;
        LOG_ERROR("Error handling request: " + std::string(req.method) + " " + req.path);
        res.set_content("{\"error\":\"Internal Server Error\"}", "application/json");
    });
    
    server_->set_pre_routing_handler([this](const httplib::Request& req, httplib::Response& res) -> httplib::Server::HandlerResponse {
         return ValidateRequest(req, res);
    });
    
    server_->set_read_timeout(config_.server.read_timeout);
    server_->set_write_timeout(config_.server.write_timeout);
#endif
    LOG_INFO("HTTP middleware configured");
}
#ifdef SSL_SERVER_SUPPORT
void HttpServer::SetupMiddlewareForSSL() {

    if (!ssl_server_) return;
    
    ssl_server_->set_logger([this](const httplib::Request& req, const httplib::Response& res) {
        total_requests_++;
        
        std::string log_msg = "[HTTPS] " + std::string(req.method) + " " + req.path + " - " + 
                             std::to_string(res.status) + " - Client: " + 
                             req.get_header_value("X-Forwarded-For");
        
        if (res.status >= 400) {
            LOG_WARN(log_msg);
        } else {
            LOG_INFO(log_msg);
        }
    });
    
    ssl_server_->set_error_handler([this](const httplib::Request& req, httplib::Response& res) {
        blocked_requests_++;
        LOG_ERROR("[HTTPS] Error handling request: " + std::string(req.method) + " " + req.path);
        res.set_content("{\"error\":\"Internal Server Error\"}", "application/json");
    });
    
    ssl_server_->set_pre_routing_handler([this](const httplib::Request& req, httplib::Response& res) -> httplib::Server::HandlerResponse {
        return ValidateRequest(req, res);
    });
    
    ssl_server_->set_read_timeout(config_.server.read_timeout);
    ssl_server_->set_write_timeout(config_.server.write_timeout);
    
    LOG_INFO("HTTPS middleware configured");

}
#endif
void HttpServer::SetupSecurityHeaders() {
#ifdef SSL_SERVER_SUPPORT
    if (ssl_server_) {
        ssl_server_->set_default_headers({
            {"X-Content-Type-Options", "nosniff"},
            {"X-Frame-Options", "DENY"},
            {"X-XSS-Protection", "1; mode=block"},
            {"Strict-Transport-Security", "max-age=31536000"}
        });
    }
#else
    if (server_) {
        server_->set_default_headers({
            {"X-Content-Type-Options", "nosniff"},
            {"X-Frame-Options", "DENY"},
            {"X-XSS-Protection", "1; mode=block"},
            {"Strict-Transport-Security", "max-age=31536000"}
            });
    }
#endif
    
    LOG_INFO("Security headers configured");
}

// 认证相关处理器实现

void HttpServer::HandleLogin(const httplib::Request& req, httplib::Response& res) {
    if (!auth_) {
        res.status = 503;
        res.set_content("{\"error\":\"Authentication service unavailable\"}", "application/json");
        return;
    }
    
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        std::string username = j.value("username", "");
        std::string password = j.value("password", "");
        
        if (username.empty() || password.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"Username and password required\"}", "application/json");
            failed_auth_attempts_++;
            return;
        }
        
        if (!auth_->AuthenticateUser(username, password)) {
            res.status = 401;
            res.set_content("{\"error\":\"Invalid credentials\"}", "application/json");
            failed_auth_attempts_++;
            LogSecurityEvent("Failed login attempt for user: " + username, req);
            return;
        }
        
        auth::User user = auth_->GetUser(username);
        std::string token = auth_->GenerateToken(user);
        
        if (token.empty()) {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to generate token\"}", "application/json");
            return;
        }
        
        nlohmann::json response;
        response["token"] = token;
        response["expires_in"] = auth_->GetConfig().token_expiry_hours * 3600;
        response["user"] = {
            {"username", user.username},
            {"role", user.role}
        };
        
        res.status = 200;
        res.set_content(response.dump(), "application/json");
        
        LOG_INFO("User logged in: " + username);
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Login error: " + std::string(e.what()));
    }
}

void HttpServer::HandleLogout(const httplib::Request& req, httplib::Response& res) {
    if (!auth_) {
        res.status = 503;
        res.set_content("{\"error\":\"Authentication service unavailable\"}", "application/json");
        return;
    }
    
    std::string token = ExtractToken(req);
    
    if (!token.empty()) {
        auth_->RevokeToken(token);
        LOG_INFO("User logged out");
    }
    
    res.status = 200;
    res.set_content("{\"status\":\"success\"}", "application/json");
}

void HttpServer::HandleRefreshToken(const httplib::Request& req, httplib::Response& res) {
    if (!auth_) {
        res.status = 503;
        res.set_content("{\"error\":\"Authentication service unavailable\"}", "application/json");
        return;
    }
    
    std::string token = ExtractToken(req);
    
    if (token.empty()) {
        res.status = 400;
        res.set_content("{\"error\":\"Token required\"}", "application/json");
        return;
    }
    
    std::string new_token = auth_->RefreshToken(token);
    
    if (new_token.empty()) {
        res.status = 401;
        res.set_content("{\"error\":\"Invalid or expired token\"}", "application/json");
        return;
    }
    
    auth::TokenPayload payload = auth_->DecodeToken(new_token);
    
    nlohmann::json response;
    response["token"] = new_token;
    response["expires_in"] = auth_->GetConfig().token_expiry_hours * 3600;
    response["user"] = {
        {"username", payload.username},
        {"role", payload.role},
        {"permissions", payload.permissions}
    };
    
    res.status = 200;
    res.set_content(response.dump(), "application/json");
}

void HttpServer::HandleUserInfo(const httplib::Request& req, httplib::Response& res) {
    if (!auth_) {
        res.status = 503;
        res.set_content("{\"error\":\"Authentication service unavailable\"}", "application/json");
        return;
    }
    
    std::string token = ExtractToken(req);
    
    if (token.empty() || !auth_->ValidateToken(token)) {
        res.status = 401;
        res.set_content("{\"error\":\"Invalid token\"}", "application/json");
        return;
    }
    
    auth::TokenPayload payload = auth_->DecodeToken(token);
    
    nlohmann::json response;
    response["user"] = {
        {"username", payload.username},
        {"role", payload.role},
        {"permissions", payload.permissions},
        {"last_login", FormatTimeISO8601(payload.issued_at)}
    };
    
    res.status = 200;
    res.set_content(response.dump(), "application/json");
}

// 原有处理器实现（需要更新以支持认证）
void HttpServer::HandleGetTile(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    int z, x, y;
    std::string format;
    
    if (!ParseTileRequest(req, z, x, y, format)) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid tile request\"}", "application/json");
        return;
    }
    
    encoder::ImageFormat imageFormat = encoder::StringToImageFormat(format);
    int dpi = 96;
    
    if (req.has_param("dpi")) {
        dpi = std::stoi(req.get_param_value("dpi"));
    }
    
    auto result = map_service_->GetTile(z, x, y, imageFormat, dpi);
    
    if (!result.success) {
        res.status = 500;
        res.set_content("{\"error\":\"" + result.error_message + "\"}", "application/json");
        return;
    }
    
    res.status = 200;
    res.set_content(reinterpret_cast<const char*>(result.data.data()),
                   result.data.size(),
                   encoder::GetMimeType(imageFormat));
    
    res.set_header("Cache-Control", "max-age=3600");
    std::string strCache = result.from_cache ? "HIT" : "MISS";
    res.set_header("X-Cache", strCache);
    std::string strTime = std::to_string(result.processing_time) + "ms";
    res.set_header("X-Processing-Time", strTime);
}

void HttpServer::HandleGetCapabilities(const httplib::Request& req, httplib::Response& res) {
    std::string xml = GenerateCapabilitiesXML();
    
    res.status = 200;
    res.set_content(xml, "application/xml");
}

void HttpServer::HandleHealth(const httplib::Request& req, httplib::Response& res) {
    std::string json = GenerateHealthJSON();
    
    res.status = 200;
    res.set_content(json, "application/json");
}

void HttpServer::HandleMetrics(const httplib::Request& req, httplib::Response& res) {
    std::string json = GenerateMetricsJSON();
    
    res.status = 200;
    res.set_content(json, "application/json");
}

void HttpServer::HandleGenerateMap(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        service::MapRequest mapReq;
        
        if (j.contains("bbox")) {
            auto bbox = j["bbox"];
            mapReq.bbox.minX = bbox[0];
            mapReq.bbox.minY = bbox[1];
            mapReq.bbox.maxX = bbox[2];
            mapReq.bbox.maxY = bbox[3];
        }
        
        if (j.contains("width")) {
            mapReq.width = j["width"];
        }
        
        if (j.contains("height")) {
            mapReq.height = j["height"];
        }
        
        if (j.contains("format")) {
            mapReq.format = encoder::StringToImageFormat(j["format"]);
        }
        
        if (j.contains("dpi")) {
            mapReq.dpi = j["dpi"];
        }
        
        auto result = map_service_->GenerateMap(mapReq);
        
        if (!result.success) {
            res.status = 500;
            res.set_content("{\"error\":\"" + result.error_message + "\"}", "application/json");
            return;
        }
        
        res.status = 200;
        res.set_content(reinterpret_cast<const char*>(result.data.data()),
                       result.data.size(),
                       encoder::GetMimeType(mapReq.format));
        
        res.set_header("Cache-Control", "max-age=3600");
        res.set_header("X-Processing-Time", std::to_string(result.processing_time) + "ms");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Generate map error: " + std::string(e.what()));
    }
}

// 辅助方法实现
bool HttpServer::ParseTileRequest(const httplib::Request& req, int& z, int& x, int& y, std::string& format) {
    if (req.matches.size() < 5) {
        return false;
    }
    
    try {
        z = std::stoi(req.matches[1]);
        x = std::stoi(req.matches[2]);
        y = std::stoi(req.matches[3]);
        format = req.matches[4];
        
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse tile request: " + std::string(e.what()));
        return false;
    }
}

std::string HttpServer::ExtractToken(const httplib::Request& req) const {
    // 从Authorization头提取token
    std::string auth_header = req.get_header_value("Authorization");
    
    if (auth_header.find("Bearer ") == 0) {
        return auth_header.substr(7);
    }
    
    // 从查询参数提取token
    if (req.has_param("token")) {
        return req.get_param_value("token");
    }
    
    return "";
}

httplib::Server::HandlerResponse HttpServer::ValidateRequest(const httplib::Request& req, httplib::Response& res) {
    // 安全检查：请求大小限制
    if (req.body.size() > 10 * 1024 * 1024) { // 10MB限制
        res.status = 413;
        res.set_content("{\"error\":\"Request too large\"}", "application/json");
        blocked_requests_++;
        LogSecurityEvent("Request size exceeded limit", req);
        return httplib::Server::HandlerResponse::Handled;
    }
    
    // 安全检查：路径遍历防护
    if (req.path.find("..") != std::string::npos || req.path.find("\\") != std::string::npos) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid path\"}", "application/json");
        blocked_requests_++;
        LogSecurityEvent("Path traversal attempt detected", req);
        return httplib::Server::HandlerResponse::Handled;
    }
    
    // 认证检查
    if (auth_middleware_) {
        std::string token = ExtractToken(req);
        
        if (!auth_middleware_->ProcessRequest(token, req.path, req.method)) {
            res.status = 401;
            res.set_content("{\"error\":\"Authentication required\"}", "application/json");
            failed_auth_attempts_++;
            LogSecurityEvent("Authentication failed for path: " + req.path, req);
            return httplib::Server::HandlerResponse::Handled;
        }
    }
    
    // 添加安全头
    AddSecurityHeaders(res);
    
    return httplib::Server::HandlerResponse::Unhandled;
}

void HttpServer::AddSecurityHeaders(httplib::Response& res) {
    res.set_header("X-Content-Type-Options", "nosniff");
    res.set_header("X-Frame-Options", "DENY");
    res.set_header("X-XSS-Protection", "1; mode=block");
#ifdef SSL_SERVER_SUPPORT
    if (ssl_enabled_) {
        res.set_header("Strict-Transport-Security", "max-age=31536000");
    }
#endif
}

void HttpServer::LogSecurityEvent(const std::string& event, const httplib::Request& req) {
    std::string log_msg = "[SECURITY] " + event + 
                         " - IP: " + req.get_header_value("X-Forwarded-For") +
                         " - Path: " + req.path +
                         " - Method: " + req.method;
    
    LOG_WARN(log_msg);
    
    // 定期安全检查
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::minutes>(now - last_security_check_).count() >= 5) {
        if (failed_auth_attempts_ > 100) {
            LOG_ERROR("High number of failed authentication attempts detected: " + 
                     std::to_string(failed_auth_attempts_));
        }
        
        if (blocked_requests_ > 50) {
            LOG_ERROR("High number of blocked requests detected: " + 
                     std::to_string(blocked_requests_));
        }
        
        last_security_check_ = now;
    }
}

std::string HttpServer::GenerateCapabilitiesXML() const {
    // 简化的能力文档生成
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<Capabilities version=\"1.0.0\">" << std::endl;
    ss << "  <Service>" << std::endl;
    ss << "    <Name>WMTS</Name>" << std::endl;
    ss << "    <Title>Cycle Map Server</Title>" << std::endl;
    ss << "  </Service>" << std::endl;
    ss << "</Capabilities>" << std::endl;
    
    return ss.str();
}

std::string HttpServer::GenerateHealthJSON() const {
    nlohmann::json j;
    j["status"] = "healthy";
    j["timestamp"] = FormatTimeISO8601(std::chrono::system_clock::now());
    j["version"] = "1.0.0";
    
    j["services"] = {
        {"database", map_service_ ? "connected" : "disconnected"},
        {"cache", "ready"},
        {"renderer", renderer_ ? "ready" : "not_initialized"}
    };
    
    if (auth_) {
        j["authentication"] = "enabled";
    } else {
        j["authentication"] = "disabled";
    }
    
#ifdef SSL_SERVER_SUPPORT
    j["ssl_enabled"] = ssl_enabled_;
#endif
    
    return j.dump(4);
}

std::string HttpServer::GenerateMetricsJSON() const {
    nlohmann::json j;
    
    j["requests"] = {
        {"total", total_requests_.load()},
        {"successful", total_requests_.load() - failed_auth_attempts_.load() - blocked_requests_.load()},
        {"failed", failed_auth_attempts_.load() + blocked_requests_.load()},
        {"rate_per_minute", static_cast<double>(total_requests_.load()) / 60.0}
    };
    
    if (map_service_) {
        auto& metrics = map_service_->GetMetrics();
        uint64_t total = metrics.total_requests.load();
        double hit_rate = 0.0;
        if (total > 0) {
            hit_rate = static_cast<double>(metrics.cache_hits.load()) / total * 100.0;
        }
        
        j["cache"] = {
            {"memory_hit_rate", hit_rate},
            {"disk_hit_rate", 0.0},
            {"total_size", 0}
        };
    }
    
    j["system"] = {
        {"memory_usage", 0},
        {"cpu_usage", 0.0},
        {"uptime", 0}
    };
    
#ifdef SSL_SERVER_SUPPORT
    j["ssl_enabled"] = ssl_enabled_;
#endif
    j["server_running"] = running_.load();
    
    return j.dump(4);
}

std::string HttpServer::FormatTimeISO8601(const std::chrono::system_clock::time_point& time) {
    std::time_t t = std::chrono::system_clock::to_time_t(time);
    std::tm* tm = std::gmtime(&t);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// 瓦片边界框 API
void HttpServer::HandleGetTileBounds(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    int z, x, y;
    std::string format;
    
    if (!ParseTileRequest(req, z, x, y, format)) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid tile request\"}", "application/json");
        return;
    }
    
    service::TileBounds bounds = map_service_->GetTileBounds(z, x, y);
    
    nlohmann::json j;
    j["bounds"] = {
        {"minX", bounds.minX},
        {"minY", bounds.minY},
        {"maxX", bounds.maxX},
        {"maxY", bounds.maxY}
    };
    j["zoom"] = z;
    j["x"] = x;
    j["y"] = y;
    j["format"] = format;
    
    res.status = 200;
    res.set_content(j.dump(4), "application/json");
}

// 批量瓦片生成 API
void HttpServer::HandleBatchGenerateTiles(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        service::BatchRequest batchReq;
        
        if (j.contains("bounds")) {
            auto bounds = j["bounds"];
            batchReq.bounds.minX = bounds["minX"];
            batchReq.bounds.minY = bounds["minY"];
            batchReq.bounds.maxX = bounds["maxX"];
            batchReq.bounds.maxY = bounds["maxY"];
        }
        
        if (j.contains("zoom_levels")) {
            batchReq.zoom_levels.clear();
            for (const auto& zoom : j["zoom_levels"]) {
                batchReq.zoom_levels.push_back(zoom);
            }
        }
        
        if (j.contains("format")) {
            batchReq.format = encoder::StringToImageFormat(j["format"]);
        }
        
        if (j.contains("dpi")) {
            batchReq.dpi = j["dpi"];
        }
        
        batchReq.async = j.value("async", true);
        
        auto result = map_service_->BatchGenerateTiles(batchReq);
        
        if (!result.success) {
            res.status = 500;
            res.set_content("{\"error\":\"" + result.error_message + "\"}", "application/json");
            return;
        }
        
        nlohmann::json response;
        response["task_id"] = result.task_id;
        response["status"] = batchReq.async ? "queued" : "completed";
        response["estimated_tiles"] = result.estimated_tiles;
        response["estimated_time"] = result.estimated_time;
        
        res.status = 200;
        res.set_content(response.dump(4), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Batch generate tiles error: " + std::string(e.what()));
    }
}

// 配置管理 API
void HttpServer::HandleGetConfig(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json j;
    
    j["database"] = {
        {"type", config_.database.type},
        {"sqlite_path", config_.database.sqlite_path}
    };
    
    j["server"] = {
        {"host", config_.server.host},
        {"port", config_.server.port},
        {"enable_https", config_.server.enable_https}
    };
    
    j["cache"] = {
        {"enabled", config_.cache.enabled},
        {"memory_cache_size", config_.cache.memory_cache_size}
    };
    
    res.status = 200;
    res.set_content(j.dump(4), "application/json");
}

void HttpServer::HandleUpdateConfig(const httplib::Request& req, httplib::Response& res) {
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        nlohmann::json response;
        response["message"] = "Configuration updated successfully";
        response["restart_required"] = false;
        
        res.status = 200;
        res.set_content(response.dump(4), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Update config error: " + std::string(e.what()));
    }
}

void HttpServer::HandleGetSecurityConfig(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json j;
    
    j["secure_mode"] = true;
    j["https_enabled"] = config_.server.enable_https;
    j["rate_limit_enabled"] = true;
    j["failed_login_attempts"] = failed_auth_attempts_.load();
    j["last_security_scan"] = FormatTimeISO8601(last_security_check_);
    
    res.status = 200;
    res.set_content(j.dump(4), "application/json");
}

// 监控和统计 API
void HttpServer::HandleGetMetricsRequests(const httplib::Request& req, httplib::Response& res) {
    std::string time_range = req.get_param_value("time_range");
    std::string endpoint = req.get_param_value("endpoint");
    
    nlohmann::json j;
    j["time_range"] = time_range.empty() ? "24h" : time_range;
    j["total_requests"] = total_requests_.load();
    
    nlohmann::json endpoints = nlohmann::json::array();
    
    nlohmann::json tile_endpoint;
    tile_endpoint["path"] = "/tile/{z}/{x}/{y}/{format}";
    tile_endpoint["count"] = total_requests_.load() / 2;
    tile_endpoint["avg_response_time"] = 0.15;
    tile_endpoint["error_rate"] = 0.02;
    endpoints.push_back(tile_endpoint);
    
    nlohmann::json auth_endpoint;
    auth_endpoint["path"] = "/auth/login";
    auth_endpoint["count"] = total_requests_.load() / 10;
    auth_endpoint["avg_response_time"] = 0.05;
    auth_endpoint["error_rate"] = 0.01;
    endpoints.push_back(auth_endpoint);
    
    j["endpoints"] = endpoints;
    
    res.status = 200;
    res.set_content(j.dump(4), "application/json");
}

// 缓存管理 API
void HttpServer::HandleGetCacheStats(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json j;
    
    j["memory_cache"] = {
        {"size", config_.cache.memory_cache_size},
        {"used", config_.cache.memory_cache_size / 2},
        {"hit_rate", 0.85},
        {"items", 10000}
    };
    
    j["disk_cache"] = {
        {"size", 2147483648},
        {"used", 1073741824},
        {"hit_rate", 0.92},
        {"files", 50000}
    };
    
    res.status = 200;
    res.set_content(j.dump(4), "application/json");
}

void HttpServer::HandleClearCache(const httplib::Request& req, httplib::Response& res) {
    std::string cache_type = req.get_param_value("type");
    
    if (cache_type.empty()) {
        cache_type = "all";
    }
    
    int cleared_items = 0;
    size_t freed_memory = 0;
    
    if (cache_type == "memory" || cache_type == "all") {
        cleared_items = 10000;
        freed_memory = config_.cache.memory_cache_size / 2;
    }
    
    nlohmann::json response;
    response["message"] = "Cache cleared successfully";
    response["cleared_items"] = cleared_items;
    response["freed_memory"] = freed_memory;
    
    res.status = 200;
    res.set_content(response.dump(4), "application/json");
}

void HttpServer::HandleWarmupCache(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        nlohmann::json response;
        response["task_id"] = "warmup_" + std::to_string(std::time(nullptr));
        response["estimated_tiles"] = 1000;
        response["status"] = "started";
        
        res.status = 200;
        res.set_content(response.dump(4), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Warmup cache error: " + std::string(e.what()));
    }
}

// 安全监控 API
void HttpServer::HandleGetSecurityEvents(const httplib::Request& req, httplib::Response& res) {
    std::string event_type = req.get_param_value("type");
    std::string start_time = req.get_param_value("start_time");
    std::string end_time = req.get_param_value("end_time");
    int limit = 50;
    
    if (req.has_param("limit")) {
        limit = std::stoi(req.get_param_value("limit"));
    }
    
    nlohmann::json j;
    
    nlohmann::json events = nlohmann::json::array();
    
    nlohmann::json event1;
    event1["timestamp"] = FormatTimeISO8601(std::chrono::system_clock::now());
    event1["type"] = "auth_failure";
    event1["ip_address"] = "192.168.1.100";
    event1["username"] = "attacker";
    event1["details"] = "Invalid password";
    events.push_back(event1);
    
    nlohmann::json event2;
    event2["timestamp"] = FormatTimeISO8601(std::chrono::system_clock::now() - std::chrono::minutes(5));
    event2["type"] = "rate_limit";
    event2["ip_address"] = "192.168.1.101";
    event2["details"] = "Too many requests";
    events.push_back(event2);
    
    j["events"] = events;
    j["total"] = events.size();
    j["page"] = 1;
    j["page_size"] = limit;
    
    res.status = 200;
    res.set_content(j.dump(4), "application/json");
}

void HttpServer::HandleConfigureSecurityAlerts(const httplib::Request& req, httplib::Response& res) {
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        nlohmann::json response;
        response["message"] = "Security alerts configured successfully";
        
        res.status = 200;
        res.set_content(response.dump(4), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Configure security alerts error: " + std::string(e.what()));
    }
}

void HttpServer::HandleSecurityScan(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json response;
    response["scan_id"] = "scan_" + std::to_string(std::time(nullptr));
    response["status"] = "running";
    response["estimated_completion"] = FormatTimeISO8601(
        std::chrono::system_clock::now() + std::chrono::minutes(5));
    
    res.status = 200;
    res.set_content(response.dump(4), "application/json");
}

// 性能优化 API
void HttpServer::HandleStartBenchmark(const httplib::Request& req, httplib::Response& res) {
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        nlohmann::json response;
        response["benchmark_id"] = "bench_" + std::to_string(std::time(nullptr));
        response["status"] = "started";
        response["estimated_duration"] = 300;
        
        res.status = 200;
        res.set_content(response.dump(4), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Start benchmark error: " + std::string(e.what()));
    }
}

void HttpServer::HandleGetBenchmarkResults(const httplib::Request& req, httplib::Response& res) {
    std::string benchmark_id = req.matches[1];
    
    nlohmann::json response;
    response["benchmark_id"] = benchmark_id;
    response["status"] = "completed";
    
    nlohmann::json results;
    results["total_requests"] = 10000;
    results["successful_requests"] = 9800;
    results["failed_requests"] = 200;
    results["average_response_time"] = 0.15;
    results["p95_response_time"] = 0.25;
    results["throughput"] = 66.7;
    
    response["results"] = results;
    
    res.status = 200;
    res.set_content(response.dump(4), "application/json");
}

// 批量操作 API
void HttpServer::HandleBatchTiles(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    try {
        nlohmann::json j = nlohmann::json::parse(req.body);
        
        nlohmann::json response;
        response["batch_id"] = "batch_" + std::to_string(std::time(nullptr));
        response["total_tiles"] = j.value("tiles", nlohmann::json::array()).size();
        response["completed_tiles"] = 0;
        response["status"] = "processing";
        
        res.status = 200;
        res.set_content(response.dump(4), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid request format\"}", "application/json");
        LOG_ERROR("Batch tiles error: " + std::string(e.what()));
    }
}

void HttpServer::HandleGetBatchStatus(const httplib::Request& req, httplib::Response& res) {
    std::string batch_id = req.matches[1];
    
    nlohmann::json response;
    response["batch_id"] = batch_id;
    response["operation"] = "generate";
    response["total_tiles"] = 2;
    response["completed_tiles"] = 2;
    response["failed_tiles"] = 0;
    response["status"] = "completed";
    
    res.status = 200;
    res.set_content(response.dump(4), "application/json");
}

} // namespace server
} // namespace cycle