#include "config.h"
#include "../utils/logger.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "../utils/config_encryptor.h"

namespace cycle {

using json = nlohmann::json;

bool RangeLimitConfig::ValidateBoundingBox(const BoundingBox& bbox) const {
    if (!bbox.IsValid()) {
        return false;
    }
    
    if (bbox.Width() > max_bbox_width || bbox.Height() > max_bbox_height) {
        return false;
    }
    
    if (bbox.Width() < min_bbox_width || bbox.Height() < min_bbox_height) {
        return false;
    }
    
    return true;
}

bool RangeLimitConfig::ValidateImageSize(int width, int height) const {
    if (width < min_image_width || width > max_image_width) {
        return false;
    }
    
    if (height < min_image_height || height > max_image_height) {
        return false;
    }
    
    return true;
}

bool RangeLimitConfig::ValidateDpi(int dpi) const {
    return dpi >= min_dpi && dpi <= max_dpi;
}

bool RangeLimitConfig::ValidateZoom(int zoom) const {
    return zoom >= 0 && zoom <= max_zoom;
}

Config& Config::Instance() {
    static Config instance;
    return instance;
}

bool Config::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: " + path);
        return false;
    }
    
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        json configJson = json::parse(content);
        
        if (configJson.contains("database")) {
            auto dbJson = configJson["database"];
            if (dbJson.contains("type")) database.type = dbJson["type"];
            if (dbJson.contains("sqlite_path")) database.sqlite_path = dbJson["sqlite_path"];
            if (dbJson.contains("pg_host")) database.pg_host = dbJson["pg_host"];
            if (dbJson.contains("pg_port")) database.pg_port = dbJson["pg_port"];
            if (dbJson.contains("pg_database")) database.pg_database = dbJson["pg_database"];
            if (dbJson.contains("pg_username")) database.pg_username = dbJson["pg_username"];
            if (dbJson.contains("pg_password")) database.pg_password = dbJson["pg_password"];
            if (dbJson.contains("max_connections")) database.max_connections = dbJson["max_connections"];
            if (dbJson.contains("connection_timeout")) database.connection_timeout = dbJson["connection_timeout"];
        }
        
        if (configJson.contains("encoder")) {
            auto encJson = configJson["encoder"];
            if (encJson.contains("format")) encoder.format = encJson["format"];
            if (encJson.contains("png_compression")) encoder.png_compression = encJson["png_compression"];
            if (encJson.contains("webp_quality")) encoder.webp_quality = encJson["webp_quality"];
            if (encJson.contains("webp_lossless")) encoder.webp_lossless = encJson["webp_lossless"];
            if (encJson.contains("default_dpi")) encoder.default_dpi = encJson["default_dpi"];
            if (encJson.contains("min_dpi")) encoder.min_dpi = encJson["min_dpi"];
            if (encJson.contains("max_dpi")) encoder.max_dpi = encJson["max_dpi"];
        }
        
        if (configJson.contains("range_limit")) {
            auto rangeJson = configJson["range_limit"];
            if (rangeJson.contains("enabled")) range_limit.enabled = rangeJson["enabled"];
            if (rangeJson.contains("max_bbox_width")) range_limit.max_bbox_width = rangeJson["max_bbox_width"];
            if (rangeJson.contains("max_bbox_height")) range_limit.max_bbox_height = rangeJson["max_bbox_height"];
            if (rangeJson.contains("min_bbox_width")) range_limit.min_bbox_width = rangeJson["min_bbox_width"];
            if (rangeJson.contains("min_bbox_height")) range_limit.min_bbox_height = rangeJson["min_bbox_height"];
            if (rangeJson.contains("max_image_width")) range_limit.max_image_width = rangeJson["max_image_width"];
            if (rangeJson.contains("max_image_height")) range_limit.max_image_height = rangeJson["max_image_height"];
            if (rangeJson.contains("min_image_width")) range_limit.min_image_width = rangeJson["min_image_width"];
            if (rangeJson.contains("min_image_height")) range_limit.min_image_height = rangeJson["min_image_height"];
            if (rangeJson.contains("min_dpi")) range_limit.min_dpi = rangeJson["min_dpi"];
            if (rangeJson.contains("max_dpi")) range_limit.max_dpi = rangeJson["max_dpi"];
            if (rangeJson.contains("max_zoom")) range_limit.max_zoom = rangeJson["max_zoom"];
            if (rangeJson.contains("reject_oversized")) range_limit.reject_oversized = rangeJson["reject_oversized"];
        }
        
        if (configJson.contains("server")) {
            auto srvJson = configJson["server"];
            if (srvJson.contains("host")) server.host = srvJson["host"];
            if (srvJson.contains("port")) server.port = srvJson["port"];
            if (srvJson.contains("thread_count")) server.thread_count = srvJson["thread_count"];
            if (srvJson.contains("read_timeout")) server.read_timeout = srvJson["read_timeout"];
            if (srvJson.contains("write_timeout")) server.write_timeout = srvJson["write_timeout"];
            if (srvJson.contains("enable_https")) server.enable_https = srvJson["enable_https"];
            if (srvJson.contains("https_port")) server.https_port = srvJson["https_port"];
            if (srvJson.contains("ssl_enabled")) server.enable_https = srvJson["ssl_enabled"];
            if (srvJson.contains("ssl_cert_file")) server.ssl_cert_file = srvJson["ssl_cert_file"];
            if (srvJson.contains("ssl_key_file")) server.ssl_key_file = srvJson["ssl_key_file"];
            if (srvJson.contains("ssl_ca_file")) server.ssl_ca_file = srvJson["ssl_ca_file"];
            if (srvJson.contains("jwt_secret")) server.jwt_secret = srvJson["jwt_secret"];
            if (srvJson.contains("token_expiry_hours")) server.token_expiry_hours = srvJson["token_expiry_hours"];
            if (srvJson.contains("enable_rate_limit")) server.enable_rate_limit = srvJson["enable_rate_limit"];
            if (srvJson.contains("requests_per_minute")) server.requests_per_minute = srvJson["requests_per_minute"];
            if (srvJson.contains("burst_size")) server.burst_size = srvJson["burst_size"];
        }
        
        if (configJson.contains("cache")) {
            auto cacheJson = configJson["cache"];
            if (cacheJson.contains("enabled")) cache.enabled = cacheJson["enabled"];
            if (cacheJson.contains("memory_cache_size")) cache.memory_cache_size = cacheJson["memory_cache_size"];
            if (cacheJson.contains("disk_cache_path")) cache.disk_cache_path = cacheJson["disk_cache_path"];
            if (cacheJson.contains("cache_ttl")) cache.cache_ttl = cacheJson["cache_ttl"];
        }
        
        if (configJson.contains("log")) {
            auto logJson = configJson["log"];
            if (logJson.contains("level")) log.level = logJson["level"];
            if (logJson.contains("file")) log.file = logJson["file"];
            if (logJson.contains("console_output")) log.console_output = logJson["console_output"];
            if (logJson.contains("rotate")) log.rotate = logJson["rotate"];
            if (logJson.contains("max_size")) log.max_size = logJson["max_size"];
            if (logJson.contains("max_files")) log.max_files = logJson["max_files"];
        }
        
        config_path_ = path;
        
        if (!Validate()) {
            LOG_ERROR("Config validation failed");
            return false;
        }
        
        LOG_INFO("Config loaded successfully from: " + path);
        return true;
        
    } catch (const json::exception& e) {
        LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::SaveToFile(const std::string& path) const {
    try {
        json configJson;
        
        configJson["database"]["type"] = database.type;
        configJson["database"]["sqlite_path"] = database.sqlite_path;
        configJson["database"]["pg_host"] = database.pg_host;
        configJson["database"]["pg_port"] = database.pg_port;
        configJson["database"]["pg_database"] = database.pg_database;
        configJson["database"]["pg_username"] = database.pg_username;
        configJson["database"]["pg_password"] = database.pg_password;
        configJson["database"]["max_connections"] = database.max_connections;
        configJson["database"]["connection_timeout"] = database.connection_timeout;
        
        configJson["encoder"]["format"] = encoder.format;
        configJson["encoder"]["png_compression"] = encoder.png_compression;
        configJson["encoder"]["webp_quality"] = encoder.webp_quality;
        configJson["encoder"]["webp_lossless"] = encoder.webp_lossless;
        configJson["encoder"]["default_dpi"] = encoder.default_dpi;
        configJson["encoder"]["min_dpi"] = encoder.min_dpi;
        configJson["encoder"]["max_dpi"] = encoder.max_dpi;
        
        configJson["range_limit"]["enabled"] = range_limit.enabled;
        configJson["range_limit"]["max_bbox_width"] = range_limit.max_bbox_width;
        configJson["range_limit"]["max_bbox_height"] = range_limit.max_bbox_height;
        configJson["range_limit"]["min_bbox_width"] = range_limit.min_bbox_width;
        configJson["range_limit"]["min_bbox_height"] = range_limit.min_bbox_height;
        configJson["range_limit"]["max_image_width"] = range_limit.max_image_width;
        configJson["range_limit"]["max_image_height"] = range_limit.max_image_height;
        configJson["range_limit"]["min_image_width"] = range_limit.min_image_width;
        configJson["range_limit"]["min_image_height"] = range_limit.min_image_height;
        configJson["range_limit"]["min_dpi"] = range_limit.min_dpi;
        configJson["range_limit"]["max_dpi"] = range_limit.max_dpi;
        configJson["range_limit"]["max_zoom"] = range_limit.max_zoom;
        configJson["range_limit"]["reject_oversized"] = range_limit.reject_oversized;
        
        configJson["server"]["host"] = server.host;
        configJson["server"]["port"] = server.port;
        configJson["server"]["thread_count"] = server.thread_count;
        configJson["server"]["read_timeout"] = server.read_timeout;
        configJson["server"]["write_timeout"] = server.write_timeout;
        configJson["server"]["enable_https"] = server.enable_https;
        configJson["server"]["https_port"] = server.https_port;
        configJson["server"]["ssl_cert_file"] = server.ssl_cert_file;
        configJson["server"]["ssl_key_file"] = server.ssl_key_file;
        configJson["server"]["ssl_ca_file"] = server.ssl_ca_file;
        configJson["server"]["jwt_secret"] = server.jwt_secret;
        configJson["server"]["token_expiry_hours"] = server.token_expiry_hours;
        configJson["server"]["enable_rate_limit"] = server.enable_rate_limit;
        configJson["server"]["requests_per_minute"] = server.requests_per_minute;
        configJson["server"]["burst_size"] = server.burst_size;
        
        configJson["cache"]["enabled"] = cache.enabled;
        configJson["cache"]["memory_cache_size"] = cache.memory_cache_size;
        configJson["cache"]["disk_cache_path"] = cache.disk_cache_path;
        configJson["cache"]["cache_ttl"] = cache.cache_ttl;
        
        configJson["log"]["level"] = log.level;
        configJson["log"]["file"] = log.file;
        configJson["log"]["console_output"] = log.console_output;
        configJson["log"]["rotate"] = log.rotate;
        configJson["log"]["max_size"] = log.max_size;
        configJson["log"]["max_files"] = log.max_files;
        
        std::ofstream file(path);
        if (!file.is_open()) {
            LOG_ERROR("Failed to create config file: " + path);
            return false;
        }
        
        file << configJson.dump(4);
        file.close();
        
        LOG_INFO("Config saved successfully to: " + path);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save config: " + std::string(e.what()));
        return false;
    }
}

bool Config::Validate() const {
    if (database.type != "sqlite3" && database.type != "postgresql") {
        LOG_ERROR("Invalid database type: " + database.type);
        return false;
    }
    
    if (database.type == "sqlite3" && database.sqlite_path.empty()) {
        LOG_ERROR("SQLite path is empty");
        return false;
    }
    
    if (encoder.format != "png8" && encoder.format != "png32" && encoder.format != "webp") {
        LOG_ERROR("Invalid encoder format: " + encoder.format);
        return false;
    }
    
    if (encoder.png_compression < 1 || encoder.png_compression > 9) {
        LOG_ERROR("Invalid PNG compression level: " + std::to_string(encoder.png_compression));
        return false;
    }
    
    if (encoder.webp_quality < 0 || encoder.webp_quality > 100) {
        LOG_ERROR("Invalid WebP quality: " + std::to_string(encoder.webp_quality));
        return false;
    }
    
    if (server.port < 1 || server.port > 65535) {
        LOG_ERROR("Invalid server port: " + std::to_string(server.port));
        return false;
    }
    
    if (server.thread_count < 1 || server.thread_count > 64) {
        LOG_ERROR("Invalid thread count: " + std::to_string(server.thread_count));
        return false;
    }
    
    if (log.level < 0 || log.level > 4) {
        LOG_ERROR("Invalid log level: " + std::to_string(log.level));
        return false;
    }
    
    return true;
}

bool Config::EnableSecureMode(const std::string& key_path) {
    if (secure_mode_) {
        LOG_WARN("Secure mode already enabled");
        return true;
    }
    
    secure_manager_ = std::make_unique<utils::SecureConfigManager>();
    
    if (!secure_manager_->Initialize(config_path_, key_path)) {
        LOG_ERROR("Failed to initialize secure config manager");
        secure_manager_.reset();
        return false;
    }
    
    // 加载敏感字段
    if (!LoadSensitiveFields()) {
        LOG_WARN("Failed to load sensitive fields, continuing in mixed mode");
    }
    
    secure_mode_ = true;
    LOG_INFO("Secure mode enabled");
    return true;
}

bool Config::ValidateSecurity() const {
    if (!secure_mode_) {
        LOG_WARN("Secure mode not enabled");
        return false;
    }
    
    if (!secure_manager_) {
        LOG_ERROR("Secure config manager not available");
        return false;
    }
    
    return secure_manager_->ValidateConfigSecurity();
}

std::string Config::GetSecureValue(const std::string& key) const {
    if (!secure_mode_ || !secure_manager_) {
        LOG_WARN("Secure mode not enabled, returning empty value");
        return "";
    }
    
    return secure_manager_->GetSecureConfigValue(key);
}

bool Config::SetSecureValue(const std::string& key, const std::string& value, bool encrypt) {
    if (!secure_mode_ || !secure_manager_) {
        LOG_ERROR("Secure mode not enabled");
        return false;
    }
    
    return secure_manager_->SetSecureConfigValue(key, value, encrypt);
}

bool Config::ParseSecureConfig(const void* json) {
    // 安全配置解析逻辑
    // 这里可以解析额外的安全相关配置
    return true;
}

bool Config::LoadSensitiveFields() {
    if (!secure_manager_) {
        LOG_ERROR("Secure manager not available");
        return false;
    }
    
    // 从环境变量加载敏感配置
    secure_manager_->LoadFromEnvironment();
    
    // 设置数据库密码（如果已加密）
    if (!database.pg_password.empty()) {
        SetSecureValue("database.password", database.pg_password, true);
    }
    
    LOG_INFO("Sensitive fields loaded");
    return true;
}

} // namespace cycle
