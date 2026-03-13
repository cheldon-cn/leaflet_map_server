#ifndef CYCLE_CONFIG_CONFIG_H
#define CYCLE_CONFIG_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "../utils/config_encryptor.h"
//class cycle::utils::SecureConfigManager;

namespace cycle {

struct BoundingBox {
    double minX;
    double minY;
    double maxX;
    double maxY;
    
    BoundingBox() : minX(0), minY(0), maxX(0), maxY(0) {}
    
    BoundingBox(double minx, double miny, double maxx, double maxy)
        : minX(minx), minY(miny), maxX(maxx), maxY(maxy) {}
    
    double Width() const { return maxX - minX; }
    double Height() const { return maxY - minY; }
    bool IsValid() const { return minX < maxX && minY < maxY; }
};

struct DatabaseConfig {
    std::string type = "sqlite3";
    std::string sqlite_path = "./data/map.db";
    
    std::string pg_host = "localhost";
    int pg_port = 5432;
    std::string pg_database = "mapdb";
    std::string pg_username = "postgres";
    std::string pg_password = "";
    
    int max_connections = 10;
    int connection_timeout = 30;
};

struct EncoderConfig {
    std::string format = "png32";
    int png_compression = 6;
    int webp_quality = 80;
    bool webp_lossless = false;
    
    int default_dpi = 96;
    int min_dpi = 72;
    int max_dpi = 600;
};

struct RangeLimitConfig {
    bool enabled = true;
    
    double max_bbox_width = 360.0;
    double max_bbox_height = 180.0;
    double min_bbox_width = 0.0001;
    double min_bbox_height = 0.0001;
    
    int max_image_width = 4096;
    int max_image_height = 4096;
    int min_image_width = 32;
    int min_image_height = 32;
    
    int min_dpi = 72;
    int max_dpi = 600;
    int max_zoom = 22;
    
    bool reject_oversized = true;
    
    bool ValidateBoundingBox(const BoundingBox& bbox) const;
    bool ValidateImageSize(int width, int height) const;
    bool ValidateDpi(int dpi) const;
    bool ValidateZoom(int zoom) const;
};

struct ServerConfig {
    std::string host = "0.0.0.0";
    int port = 8080;
    int thread_count = 4;
    int read_timeout = 30;
    int write_timeout = 30;
    
    bool enable_https = false;
    int https_port = 8443;
    std::string ssl_cert_file = "";
    std::string ssl_key_file = "";
    std::string ssl_ca_file = "";
    
    // 安全配置
    std::string jwt_secret = "";
    int token_expiry_hours = 24;
    bool enable_rate_limit = true;
    int requests_per_minute = 60;
    int burst_size = 10;
};

struct CacheConfig {
    bool enabled = true;
    size_t memory_cache_size = 512 * 1024 * 1024;
    std::string disk_cache_path = "./cache";
    int cache_ttl = 3600;
};

struct LogConfig {
    int level = 1;
    std::string file = "./logs/server.log";
    bool console_output = true;
    bool rotate = true;
    size_t max_size = 16 * 1024 * 1024;
    int max_files = 5;
};

class Config {
public:
    DatabaseConfig database;
    EncoderConfig encoder;
    RangeLimitConfig range_limit;
    ServerConfig server;
    CacheConfig cache;
    LogConfig log;
    
    bool LoadFromFile(const std::string& path);
    bool SaveToFile(const std::string& path) const;
    bool Validate() const;
    
    std::string GetConfigPath() const { return config_path_; }
    
    // 安全配置管理
    bool EnableSecureMode(const std::string& key_path = "");
    bool IsSecureMode() const { return secure_mode_; }
    bool ValidateSecurity() const;
    
    // 敏感字段访问
    std::string GetSecureValue(const std::string& key) const;
    bool SetSecureValue(const std::string& key, const std::string& value, bool encrypt = true);
    
    static Config& Instance();

    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    Config(Config&& other) noexcept 
        : database(std::move(other.database))
        , encoder(std::move(other.encoder))
        , range_limit(std::move(other.range_limit))
        , server(std::move(other.server))
        , cache(std::move(other.cache))
        , log(std::move(other.log))
        , config_path_(std::move(other.config_path_))
        , secure_mode_(other.secure_mode_)
        , secure_manager_(std::move(other.secure_manager_)) {}
    
    Config& operator=(Config&& other) noexcept {
        if (this != &other) {
            config_path_ = std::move(other.config_path_);
            secure_mode_ = other.secure_mode_;
            secure_manager_ = std::move(other.secure_manager_);
        }
        return *this;
    }
private:
    
    std::string config_path_;
    bool secure_mode_ = false;
    std::unique_ptr<utils::SecureConfigManager> secure_manager_;
    
    //bool ParseDatabaseConfig(const void* json);
    //bool ParseEncoderConfig(const void* json);
    //bool ParseRangeLimitConfig(const void* json);
    //bool ParseServerConfig(const void* json);
    //bool ParseCacheConfig(const void* json);
    //bool ParseLogConfig(const void* json);
    
    // 安全配置解析
    bool ParseSecureConfig(const void* json);
    bool LoadSensitiveFields();
};

} // namespace cycle


#endif // CYCLE_CONFIG_CONFIG_H
