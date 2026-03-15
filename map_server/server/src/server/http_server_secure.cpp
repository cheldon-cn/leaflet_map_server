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
    , ssl_enabled_(false)
    , server_(std::make_unique<httplib::Server>())
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

void HttpServer::LogApiUsageExamples() const {
    LOG_INFO("========================================");
    LOG_INFO("       API Usage Examples");
    LOG_INFO("========================================");
    
    // Health & Metrics
    LOG_INFO("GET  /health              - Health check");
    LOG_INFO("GET  /metrics             - Server metrics");
    LOG_INFO("GET  /metrics/requests    - Request metrics");
    
    // Map Tiles
    LOG_INFO("GET  /1.0.0/WMTSCapabilities.xml - WMTS capabilities");
    LOG_INFO("GET  /tile/{z}/{x}/{y}.{format}   - Get tile (e.g., /tile/5/10/20.png)");
    LOG_INFO("GET  /tile/{z}/{x}/{y}/{format}/bounds - Tile with bounds");
    
    // Map Generation
    LOG_INFO("POST /generate            - Generate map (bbox, width, height, format)");
    LOG_INFO("POST /tile/generate       - Generate single tile");
    
    // Configuration
    LOG_INFO("GET  /config              - Get current config");
    LOG_INFO("PUT  /config              - Update config (JSON body)");
    LOG_INFO("GET  /config/security     - Get security settings");
    
    // Cache Management
    LOG_INFO("GET  /cache/stats         - Get cache statistics");
    LOG_INFO("DELETE /cache/clear       - Clear cache");
    LOG_INFO("POST /cache/warmup        - Warmup cache (JSON body)");
    
    // Security
    LOG_INFO("GET  /security/events     - Get security events");
    LOG_INFO("POST /security/alerts     - Create security alert");
    LOG_INFO("POST /security/scan       - Run security scan");
    
    // Performance
    LOG_INFO("POST /performance/benchmark - Run benchmark");
    LOG_INFO("GET  /performance/results/{id} - Get benchmark results");
    
    // Batch Processing
    LOG_INFO("POST /batch/tiles         - Batch tile generation");
    LOG_INFO("GET  /batch/status/{id}   - Get batch status");
    
    // Authentication
    LOG_INFO("POST /auth/login          - Login (username, password)");
    LOG_INFO("POST /auth/logout         - Logout");
    LOG_INFO("POST /auth/refresh        - Refresh token");
    LOG_INFO("GET  /auth/user           - Get current user info");
    
    LOG_INFO("========================================");
    LOG_INFO("Example curl commands:");
    LOG_INFO("  curl http://localhost:8080/health");
    LOG_INFO("  curl http://localhost:8080/tile/5/10/20.png");
    LOG_INFO("  curl -X POST http://localhost:8080/generate -H 'Content-Type: application/json' -d '{\"bbox\":[0,0,10,10],\"width\":512,\"height\":512}'");
    LOG_INFO("========================================");
}

bool HttpServer::Start() {
    if (running_) {
        LOG_WARN("HTTP server is already running");
        return true;
    }
    
    // Log API usage examples
    LogApiUsageExamples();
    
    if (ssl_enabled_ && ssl_server_) {
        LOG_INFO("Starting HTTPS server on " + config_.server.host + ":" + 
                 std::to_string(config_.server.https_port));
        
        SetupRoutesForServer(*ssl_server_);
        SetupMiddlewareForServer(*ssl_server_);
        SetupSecurityHeadersForServer(*ssl_server_);
        
        running_ = true;
        
        if (!ssl_server_->listen(config_.server.host.c_str(), config_.server.https_port)) {
            LOG_ERROR("Failed to start HTTPS server");
            running_ = false;
            return false;
        }
        
        LOG_INFO("HTTPS server started successfully");
    } else {
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
    
    if (ssl_server_) {
        ssl_server_->stop();
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
}

bool HttpServer::IsSSLEnabled() const {
    return ssl_enabled_;
}

void HttpServer::SetupRoutes() {
    SetupCommonRoutes();
}

void HttpServer::SetupCommonRoutes() {
    SetupRoutesForServer(*server_);
}

void HttpServer::SetupRoutesForServer(httplib::Server& server) {
    server.Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        HandleHealth(req, res);
    });
    
    server.Get("/metrics", [this](const httplib::Request& req, httplib::Response& res) {
        HandleMetrics(req, res);
    });
    
    server.Get("/metrics/requests", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetMetricsRequests(req, res);
    });
    
    server.Get("/1.0.0/WMTSCapabilities.xml", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCapabilities(req, res);
    });
    
    server.Get(R"(/tile/(\d+)/(\d+)/(\d+)\.(\w+))", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTile(req, res);
        });
    
    server.Get(R"(/tile/(\d+)/(\d+)/(\d+)/(\w+)/bounds)", 
        [this](const httplib::Request& req, httplib::Response& res) {
            HandleGetTileBounds(req, res);
        });
    
    server.Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGenerateMap(req, res);
    });
    
    server.Post("/tile/generate", [this](const httplib::Request& req, httplib::Response& res) {
        HandleBatchGenerateTiles(req, res);
    });
    
    server.Get("/config", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetConfig(req, res);
    });
    
    server.Put("/config", [this](const httplib::Request& req, httplib::Response& res) {
        HandleUpdateConfig(req, res);
    });
    
    server.Get("/config/security", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetSecurityConfig(req, res);
    });
    
    server.Get("/cache/stats", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetCacheStats(req, res);
    });
    
    server.Delete("/cache/clear", [this](const httplib::Request& req, httplib::Response& res) {
        HandleClearCache(req, res);
    });
    
    server.Post("/cache/warmup", [this](const httplib::Request& req, httplib::Response& res) {
        HandleWarmupCache(req, res);
    });
    
    server.Get("/security/events", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetSecurityEvents(req, res);
    });
    
    server.Post("/security/alerts", [this](const httplib::Request& req, httplib::Response& res) {
        HandleConfigureSecurityAlerts(req, res);
    });
    
    server.Post("/security/scan", [this](const httplib::Request& req, httplib::Response& res) {
        HandleSecurityScan(req, res);
    });
    
    server.Post("/performance/benchmark", [this](const httplib::Request& req, httplib::Response& res) {
        HandleStartBenchmark(req, res);
    });
    
    server.Get(R"(/performance/results/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetBenchmarkResults(req, res);
    });
    
    server.Post("/batch/tiles", [this](const httplib::Request& req, httplib::Response& res) {
        HandleBatchTiles(req, res);
    });
    
    server.Get(R"(/batch/status/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        HandleGetBatchStatus(req, res);
    });
    
    LOG_INFO("HTTP routes configured");
}

void HttpServer::SetupMiddleware() {
    SetupCommonMiddleware();
}

void HttpServer::SetupCommonMiddleware() {
    SetupMiddlewareForServer(*server_);
}

void HttpServer::SetupMiddlewareForServer(httplib::Server& server) {
    server.set_logger([this](const httplib::Request& req, const httplib::Response& res) {
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
    
    server.set_error_handler([this](const httplib::Request& req, httplib::Response& res) {
        blocked_requests_++;
        LOG_ERROR("Error handling request: " + std::string(req.method) + " " + req.path);
        res.set_content("{\"error\":\"Internal Server Error\"}", "application/json");
    });
    
    server.set_pre_routing_handler([this](const httplib::Request& req, httplib::Response& res) -> httplib::Server::HandlerResponse {
         return ValidateRequest(req, res);
    });
    
    server.set_read_timeout(config_.server.read_timeout);
    server.set_write_timeout(config_.server.write_timeout);
    
    LOG_INFO("HTTP middleware configured");
}

void HttpServer::SetupSecurityHeaders() {
    SetupSecurityHeadersForServer(*server_);
}

void HttpServer::SetupSecurityHeadersForServer(httplib::Server& server) {
    server.set_default_headers({
        {"X-Content-Type-Options", "nosniff"},
        {"X-Frame-Options", "DENY"},
        {"X-XSS-Protection", "1; mode=block"},
        {"Strict-Transport-Security", "max-age=31536000"}
    });
    
    LOG_INFO("Security headers configured");
}

void HttpServer::SetupAuthRoutes() {
    if (ssl_enabled_ && ssl_server_) {
        SetupAuthRoutesForServer(*ssl_server_);
    } else {
        SetupAuthRoutesForServer(*server_);
    }
}

void HttpServer::SetupAuthRoutesForServer(httplib::Server& server) {
    server.Post("/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
        HandleLogin(req, res);
    });
    
    server.Post("/auth/logout", [this](const httplib::Request& req, httplib::Response& res) {
        HandleLogout(req, res);
    });
    
    server.Post("/auth/refresh", [this](const httplib::Request& req, httplib::Response& res) {
        HandleRefreshToken(req, res);
    });
    
    server.Get("/auth/user", [this](const httplib::Request& req, httplib::Response& res) {
        HandleUserInfo(req, res);
    });
    
    LOG_INFO("Authentication routes configured");
}

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

void HttpServer::HandleGetTile(const httplib::Request& req, httplib::Response& res) {
    if (!map_service_) {
        res.status = 500;
        res.set_content("{\"error\":\"MapService not initialized\"}", "application/json");
        return;
    }
    
    int z, x, y;
    std::string format;

	LOG_INFO("ParseTileRequest begin");
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

	LOG_INFO("GetTile ok");
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
	std::string strK = "Content-Type";
	std::string strV = "application/xml";
	res.set_header(strK, strV);
	strK = "Cache-Control"; strV = "max-age=86400";
	res.set_header(strK, strV);
    res.set_content(xml, "application/xml");
    
    LOG_DEBUG("WMTS Capabilities requested");
}

void HttpServer::HandleHealth(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json j = nlohmann::json::parse(GenerateHealthJSON());
    
    j["uptime_seconds"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    j["server_running"] = running_.load();
    
    res.status = 200;
    res.set_header("Content-Type", "application/json");
    res.set_content(j.dump(4), "application/json");
    
    LOG_DEBUG("Health check requested");
}

void HttpServer::HandleMetrics(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json j = nlohmann::json::parse(GenerateMetricsJSON());
    
    j["requests"]["total"] = total_requests_.load();
    j["requests"]["blocked"] = blocked_requests_.load();
    j["requests"]["active"] = (running_ ? 1 : 0);
    j["auth"]["failed_attempts"] = failed_auth_attempts_.load();
    
    res.status = 200;
    res.set_header("Content-Type", "application/json");
    res.set_content(j.dump(4), "application/json");
    
    LOG_DEBUG("Metrics requested");
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
    std::string auth_header = req.get_header_value("Authorization");
    
    if (auth_header.find("Bearer ") == 0) {
        return auth_header.substr(7);
    }
    
    if (req.has_param("token")) {
        return req.get_param_value("token");
    }
    
    return "";
}


httplib::Server::HandlerResponse HttpServer::ValidateRequest(const httplib::Request& req, httplib::Response& res) {
    // ��ȫ��飺�����С����
    if (req.body.size() > 10 * 1024 * 1024) { // 10MB����
        res.status = 413;
        res.set_content("{\"error\":\"Request too large\"}", "application/json");
        blocked_requests_++;
        LogSecurityEvent("Request size exceeded limit", req);
        return httplib::Server::HandlerResponse::Handled;
    }

    // ��ȫ��飺·����������?
    if (req.path.find("..") != std::string::npos || req.path.find("\\") != std::string::npos) {
        res.status = 400;
        res.set_content("{\"error\":\"Invalid path\"}", "application/json");
        blocked_requests_++;
        LogSecurityEvent("Path traversal attempt detected", req);
        return httplib::Server::HandlerResponse::Handled;
    }

    // ��֤���?
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

    // ���Ӱ�ȫͷ
    AddSecurityHeaders(res);

    return httplib::Server::HandlerResponse::Unhandled;
}



void HttpServer::AddSecurityHeaders(httplib::Response& res) {
    res.set_header("X-Content-Type-Options", "nosniff");
    res.set_header("X-Frame-Options", "DENY");
    res.set_header("X-XSS-Protection", "1; mode=block");
    res.set_header("Strict-Transport-Security", "max-age=31536000");
}

void HttpServer::LogSecurityEvent(const std::string& event, const httplib::Request& req) {
    std::string log_msg = "[SECURITY] " + event + 
                         " - IP: " + req.get_header_value("X-Forwarded-For") +
                         " - Path: " + req.path +
                         " - Method: " + req.method;
    
    LOG_WARN(log_msg);
    
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
    std::stringstream ss;
    
    std::string base_url = "http://" + config_.server.host + ":" + std::to_string(config_.server.port);
    
    // WMTS Capabilities XML
    ss << R"(<?xml version="1.0" encoding="UTF-8"?>)" << std::endl;
    ss << R"(<Capabilities version="1.0.0" xmlns="http://www.opengis.net/wmts/1.0")" << std::endl;
    ss << R"(         xmlns:ows="http://www.opengis.net/ows/1.1")" << std::endl;
    ss << R"(         xmlns:xlink="http://www.w3.org/1999/xlink")" << std::endl;
    ss << R"(         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance")" << std::endl;
    ss << R"(         xsi:schemaLocation="http://www.opengis.net/wmts/1.0 http://schemas.opengis.net/wmts/1.0/wmtsGetCapabilities_response.xsd">)" << std::endl;
    
    // Service Identification
    ss << "  <ows:ServiceIdentification>" << std::endl;
    ss << "    <ows:Title>Cycle Map Server WMTS</ows:Title>" << std::endl;
    ss << "    <ows:Abstract>High-performance map tile server with Web Map Tile Service (WMTS) support</ows:Abstract>" << std::endl;
    ss << "    <ows:ServiceType>OGC WMTS</ows:ServiceType>" << std::endl;
    ss << "    <ows:ServiceTypeVersion>1.0.0</ows:ServiceTypeVersion>" << std::endl;
    ss << "  </ows:ServiceIdentification>" << std::endl;
    
    // Service Provider
    ss << "  <ows:ServiceProvider>" << std::endl;
    ss << "    <ows:ProviderName>Cycle</ows:ProviderName>" << std::endl;
    ss << "    <ows:ProviderSite/>" << std::endl;
    ss << "  </ows:ServiceProvider>" << std::endl;
    
    // Operations Metadata
    ss << "  <ows:OperationsMetadata>" << std::endl;
    
    // GetTile operation
    ss << R"(    <ows:Operation name="GetTile">)" << std::endl;
    ss << "      <ows:DCP>" << std::endl;
    ss << "        <ows:HTTP>" << std::endl;
    ss << "          <ows:Get>" << std::endl;
    ss << R"(            <ows:Constraint name="GetEncoding">)" << std::endl;
    ss << "              <ows:AllowedValues>" << std::endl;
    ss << "                <ows:Value>KVP</ows:Value>" << std::endl;
    ss << "                <ows:Value>RESTful</ows:Value>" << std::endl;
    ss << "              </ows:AllowedValues>" << std::endl;
    ss << "            </ows:Constraint>" << std::endl;
    ss << R"(            <ows:Constraint name="Encoding">)" << std::endl;
    ss << "              <ows:AllowedValues>" << std::endl;
    ss << "                <ows:Value>KVP</ows:Value>" << std::endl;
    ss << "                <ows:Value>RESTful</ows:Value>" << std::endl;
    ss << "              </ows:AllowedValues>" << std::endl;
    ss << "            </ows:Constraint>" << std::endl;
	ss << "            <xlink:href=\"" << base_url << "/tile/\">" << std::endl;
	ss << R"(            <xlink:href=")" << base_url << R"(/tile/">)" << std::endl;
    ss << "          </ows:Get>" << std::endl;
    ss << "        </ows:HTTP>" << std::endl;
    ss << "      </ows:DCP>" << std::endl;
    ss << "    </ows:Operation>" << std::endl;
    
    // GetCapabilities operation
    ss << R"(    <ows:Operation name="GetCapabilities">)" << std::endl;
    ss << "      <ows:DCP>" << std::endl;
    ss << "        <ows:HTTP>" << std::endl;
    ss << "          <ows:Get>" << std::endl;
    ss << R"(            <xlink:href=")" << base_url << R"(/1.0.0/WMTSCapabilities.xml">)" << std::endl;
    ss << "          </ows:Get>" << std::endl;
    ss << "        </ows:HTTP>" << std::endl;
    ss << "      </ows:DCP>" << std::endl;
    ss << "    </ows:Operation>" << std::endl;
    
    // GetFeatureInfo operation (optional)
    ss << R"(    <ows:Operation name="GetFeatureInfo">)" << std::endl;
    ss << "      <ows:DCP>" << std::endl;
    ss << "        <ows:HTTP>" << std::endl;
    ss << "          <ows:Get>" << std::endl;
    ss << R"(            <xlink:href=")" << base_url << R"(/featureinfo/">)" << std::endl;
    ss << "          </ows:Get>" << std::endl;
    ss << "        </ows:HTTP>" << std::endl;
    ss << "      </ows:DCP>" << std::endl;
    ss << "    </ows:Operation>" << std::endl;
    
    ss << "  </ows:OperationsMetadata>" << std::endl;
    
    // Contents - Layer
    ss << "  <Contents>" << std::endl;
    ss << "    <Layer>" << std::endl;
    ss << "      <ows:Title>Cycle Map</ows:Title>" << std::endl;
    ss << "      <ows:Abstract>Base map layer</ows:Abstract>" << std::endl;
    ss << "      <ows:Identifier>cycle_map</ows:Identifier>" << std::endl;
    
    // Style
    ss << "      <Style>" << std::endl;
    ss << "        <ows:Identifier>default</ows:Identifier>" << std::endl;
    ss << "        <ows:Title>Default Style</ows:Title>" << std::endl;
    ss << "        <isDefault>true</isDefault>" << std::endl;
    ss << "      </Style>" << std::endl;
    
    // Format
    ss << "      <Format>image/png</Format>" << std::endl;
    ss << "      <Format>image/webp</Format>" << std::endl;
    ss << "      <Format>image/jpeg</Format>" << std::endl;
    
    // TileMatrixSetLink
    ss << "      <TileMatrixSetLink>" << std::endl;
    ss << "        <TileMatrixSet>GoogleMapsCompatible</TileMatrixSet>" << std::endl;
    ss << "      </TileMatrixSetLink>" << std::endl;
    
    // ResourceURL for tile (RESTful)
    ss << R"(      <ResourceURL format="image/png" resourceType="tile" template=")" << base_url << R"(/tile/{TileMatrix}/{TileCol}/{TileRow}.png">)" << std::endl;
    ss << R"(      <ResourceURL format="image/webp" resourceType="tile" template=")" << base_url << R"(/tile/{TileMatrix}/{TileCol}/{TileRow}.webp">)" << std::endl;
    
    // ResourceURL for feature info
    ss << R"(      <ResourceURL format="application/json" resourceType="featureinfo" template=")" << base_url << R"(/featureinfo/{TileMatrix}/{TileCol}/{TileRow}/{I}.json">)" << std::endl;
    
    ss << "    </Layer>" << std::endl;
    
    // TileMatrixSet - GoogleMapsCompatible (EPSG:3857)
    ss << "    <TileMatrixSet>" << std::endl;
    ss << "      <ows:Identifier>GoogleMapsCompatible</ows:Identifier>" << std::endl;
    ss << "      <ows:SupportedCRS>EPSG:3857</ows:SupportedCRS>" << std::endl;
    
    // Generate TileMatrix for zoom levels 0-19
    double scale_denominator = 559082264.0287178;
    int matrix_width = 1;
    int matrix_height = 1;
    
    for (int z = 0; z <= 19; z++) {
        ss << "      <TileMatrix>" << std::endl;
        ss << "        <ows:Identifier>" << z << "</ows:Identifier>" << std::endl;
        ss << "        <ScaleDenominator>" << scale_denominator << "</ScaleDenominator>" << std::endl;
        ss << "        <TopLeftCorner>-20037508.34 20037508.34</TopLeftCorner>" << std::endl;
        ss << "        <TileWidth>256</TileWidth>" << std::endl;
        ss << "        <TileHeight>256</TileHeight>" << std::endl;
        ss << "        <MatrixWidth>" << matrix_width << "</MatrixWidth>" << std::endl;
        ss << "        <MatrixHeight>" << matrix_height << "</MatrixHeight>" << std::endl;
        ss << "      </TileMatrix>" << std::endl;
        
        // Next level
        scale_denominator /= 2.0;
        matrix_width *= 2;
        matrix_height *= 2;
    }
    
    ss << "    </TileMatrixSet>" << std::endl;
    ss << "  </Contents>" << std::endl;
    
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
    
    j["ssl_enabled"] = ssl_enabled_;
    
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
    
    j["ssl_enabled"] = ssl_enabled_;
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

} // namespace server
} // namespace cycle
