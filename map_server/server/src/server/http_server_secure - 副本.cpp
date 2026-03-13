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
    , server_(std::make_unique<httplib::Server>())
#ifdef SSL_SERVER_SUPPORT
    , ssl_enabled_(false)
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
        
        running_ = true;
        
        if (!ssl_server_->listen(config_.server.host.c_str(), config_.server.https_port)) {
            LOG_ERROR("Failed to start HTTPS server");
            running_ = false;
            return false;
        }
        
        LOG_INFO("HTTPS server started successfully");
    }
#endif
    
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
    
    return true;
}

void HttpServer::Stop() {
    if (!running_) {
        return;
    }
    
    LOG_INFO("Stopping HTTP server");
    
    if (server_) {
        server_->stop();
    }
    
#ifdef SSL_SERVER_SUPPORT
    if (ssl_server_) {
        ssl_server_->stop();
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
    
    ssl_server_ = std::make_unique<httplib::SSLServer>(cert_file.c_str(), key_file.c_str());
    
    if (!ca_file.empty() && cycle::utils::exists(ca_file)) {
        ssl_server_->set_ca_cert_path(ca_file.c_str());
        ssl_server_->enable_client_certificate_verification();
        LOG_INFO("Client certificate verification enabled");
    }
    
    ssl_enabled_ = true;
    LOG_INFO("HTTPS server configured with SSL");
    return true;
#endif
}

bool HttpServer::IsSSLEnabled() const {

#ifdef SSL_SERVER_SUPPORT
    return ssl_enabled_;
#endif
}

void HttpServer::SetupRoutes() {
    server_->Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        HandleHealth(req, res);
    });
    
    server_->Get("/metrics", [this](const httplib::Request& req, httplib::Response& res) {
        HandleMetrics(req, res);
    });
    
    server_->Get("/1.0.0/WMTSCapabilities.xml", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCapabilities(req, res);
    });
    
    server_->Get(R"(/tile/(\d+)/(\d+)/(\d+)\.(\w+))", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTile(req, res);
        });
    
    server_->Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGenerateMap(req, res);
    });
    
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
    
    ssl_server_->Get("/1.0.0/WMTSCapabilities.xml", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCapabilities(req, res);
    });
    
    ssl_server_->Get(R"(/tile/(\d+)/(\d+)/(\d+)\.(\w+))", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTile(req, res);
        });
    
    ssl_server_->Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGenerateMap(req, res);
    });
    
    LOG_INFO("HTTPS routes configured");
}
#endif

void HttpServer::SetupAuthRoutes() {
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
#endif
    
    LOG_INFO("Authentication routes configured");
}

void HttpServer::SetupMiddleware() {
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
    
    server_->set_pre_routing_handler([this](const httplib::Request& req, httplib::Response& res) {
        return ValidateRequest(req, res);
    });
    
    server_->set_read_timeout(config_.server.read_timeout);
    server_->set_write_timeout(config_.server.write_timeout);
    
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
    
    ssl_server_->set_pre_routing_handler([this](const httplib::Request& req, httplib::Response& res) {
        return ValidateRequest(req, res);
    });
    
    ssl_server_->set_read_timeout(config_.server.read_timeout);
    ssl_server_->set_write_timeout(config_.server.write_timeout);
    
    LOG_INFO("HTTPS middleware configured");

}
#endif
void HttpServer::SetupSecurityHeaders() {
    if (server_) {
        server_->set_default_headers([](const httplib::Request& req, httplib::Response& res) {
            res.set_header("X-Content-Type-Options", "nosniff");
            res.set_header("X-Frame-Options", "DENY");
            res.set_header("X-XSS-Protection", "1; mode=block");
            res.set_header("Strict-Transport-Security", "max-age=31536000");
        });
    }
    
#ifdef SSL_SERVER_SUPPORT
    if (ssl_server_) {
        ssl_server_->set_default_headers([](const httplib::Request& req, httplib::Response& res) {
            res.set_header("X-Content-Type-Options", "nosniff");
            res.set_header("X-Frame-Options", "DENY");
            res.set_header("X-XSS-Protection", "1; mode=block");
            res.set_header("Strict-Transport-Security", "max-age=31536000");
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
        {"last_login", std::chrono::format("%Y-%m-%dT%H:%M:%SZ", payload.issued_at)}
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
    res.set_header("X-Cache", result.from_cache ? "HIT" : "MISS");
    res.set_header("X-Processing-Time", std::to_string(result.processing_time) + "ms");
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

bool HttpServer::ValidateRequest(const httplib::Request& req, httplib::Response& res) {
    // 安全检查：请求大小限制
    if (req.body.size() > 10 * 1024 * 1024) { // 10MB限制
        res.status = 413;
        res.set_content("{\"error\":\"Request too large\"}", "application/json");
        blocked_requests_++;
        LogSecurityEvent("Request size exceeded limit", req);
        return false;
    }
    
    // 安全检查：路径遍历防护
    if (req.path.find("..") != std::string::npos || req.path.find("\\") != std::string::npos) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid path\"}", "application/json");
        blocked_requests_++;
        LogSecurityEvent("Path traversal attempt detected", req);
        return false;
    }
    
    // 认证检查
    if (auth_middleware_) {
        std::string token = ExtractToken(req);
        
        if (!auth_middleware_->ProcessRequest(token, req.path, req.method)) {
            res.status = 401;
            res.set_content("{\"error\":\"Authentication required\"}", "application/json");
            failed_auth_attempts_++;
            LogSecurityEvent("Authentication failed for path: " + req.path, req);
            return false;
        }
    }
    
    // 添加安全头
    AddSecurityHeaders(res);
    
    return true;
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
    j["timestamp"] = std::chrono::format("%Y-%m-%dT%H:%M:%SZ", 
        std::chrono::system_clock::now());
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

} // namespace server
} // namespace cycle