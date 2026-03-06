#include "Config.h"
#include <fstream>
#include <sstream>


// 尝试使用nlohmann/json，如果可用

#ifdef HAS_NLOHMANN_JSON

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#endif

namespace cycle {

bool LoadConfigFromFile(const std::string& strFilePath, ServerConfig& config) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(strFilePath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        if (j.contains("server")) {
            auto& server = j["server"];
            if (server.contains("host")) config.m_strHost = server["host"];
            if (server.contains("port")) config.m_nPort = server["port"];
            if (server.contains("worker_threads")) config.m_nWorkerThreads = server["worker_threads"];
            if (server.contains("max_request_size")) config.m_nMaxRequestSize = server["max_request_size"];
            if (server.contains("timeout_seconds")) config.m_nTimeoutSeconds = server["timeout_seconds"];
        }
        
        if (j.contains("database")) {
            auto& db = j["database"];
            if (db.contains("path")) config.m_strDatabasePath = db["path"];
            if (db.contains("connection_pool_size")) config.m_nConnectionPoolSize = db["connection_pool_size"];
        }
        
        if (j.contains("cache")) {
            auto& cache = j["cache"];
            if (cache.contains("memory_max_items")) config.m_nMemoryCacheMaxItems = cache["memory_max_items"];
            if (cache.contains("disk_cache_dir")) config.m_strDiskCacheDir = cache["disk_cache_dir"];
            if (cache.contains("ttl_seconds")) config.m_nCacheTTLSeconds = cache["ttl_seconds"];
        }
        
        if (j.contains("limits")) {
            auto& limits = j["limits"];
            if (limits.contains("max_image_width")) config.m_nMaxImageWidth = limits["max_image_width"];
            if (limits.contains("max_image_height")) config.m_nMaxImageHeight = limits["max_image_height"];
            if (limits.contains("max_features_per_request")) config.m_nMaxFeaturesPerRequest = limits["max_features_per_request"];
        }
        
        if (j.contains("output_dir")) {
            config.m_strOutputDir = j["output_dir"];
        }
        
        return true;
    } catch (...) {
        return false;
    }
#else
    // 简单实现：只读取键值对格式
    std::ifstream file(strFilePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "host") config.m_strHost = value;
            else if (key == "port") config.m_nPort = std::stoi(value);
            else if (key == "worker_threads") config.m_nWorkerThreads = std::stoul(value);
            else if (key == "max_request_size") config.m_nMaxRequestSize = std::stoul(value);
            else if (key == "timeout_seconds") config.m_nTimeoutSeconds = std::stoi(value);
            else if (key == "database_path") config.m_strDatabasePath = value;
            else if (key == "connection_pool_size") config.m_nConnectionPoolSize = std::stoul(value);
            else if (key == "memory_cache_max_items") config.m_nMemoryCacheMaxItems = std::stoul(value);
            else if (key == "disk_cache_dir") config.m_strDiskCacheDir = value;
            else if (key == "cache_ttl_seconds") config.m_nCacheTTLSeconds = std::stoi(value);
            else if (key == "max_image_width") config.m_nMaxImageWidth = std::stoi(value);
            else if (key == "max_image_height") config.m_nMaxImageHeight = std::stoi(value);
            else if (key == "max_features_per_request") config.m_nMaxFeaturesPerRequest = std::stoi(value);
            else if (key == "output_dir") config.m_strOutputDir = value;
            else if (key == "log_file") config.m_strLogFile = value;
            else if (key == "log_level") config.m_nLogLevel = std::stoi(value);
        }
    }
    
    return true;
#endif
}

bool SaveConfigToFile(const std::string& strFilePath, const ServerConfig& config) {
#ifdef HAS_NLOHMANN_JSON
    try {
        json j;
        
        // 服务器配置
        j["server"]["host"] = config.m_strHost;
        j["server"]["port"] = config.m_nPort;
        j["server"]["worker_threads"] = config.m_nWorkerThreads;
        j["server"]["max_request_size"] = config.m_nMaxRequestSize;
        j["server"]["timeout_seconds"] = config.m_nTimeoutSeconds;
        
        // 数据库配置
        j["database"]["path"] = config.m_strDatabasePath;
        j["database"]["connection_pool_size"] = config.m_nConnectionPoolSize;
        
        // 缓存配置
        j["cache"]["memory_max_items"] = config.m_nMemoryCacheMaxItems;
        j["cache"]["disk_cache_dir"] = config.m_strDiskCacheDir;
        j["cache"]["ttl_seconds"] = config.m_nCacheTTLSeconds;
        
        // 限制配置
        j["limits"]["max_image_width"] = config.m_nMaxImageWidth;
        j["limits"]["max_image_height"] = config.m_nMaxImageHeight;
        j["limits"]["max_features_per_request"] = config.m_nMaxFeaturesPerRequest;
        
        // 输出目录
        j["output_dir"] = config.m_strOutputDir;
        
        // 日志配置
        j["log"]["file"] = config.m_strLogFile;
        j["log"]["level"] = config.m_nLogLevel;
        
        std::ofstream file(strFilePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2); // 漂亮打印，缩进2空格
        return true;
    } catch (...) {
        return false;
    }
#else
    // 简单实现：键值对格式
    std::ofstream file(strFilePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "host=" << config.m_strHost << "\n";
    file << "port=" << config.m_nPort << "\n";
    file << "worker_threads=" << config.m_nWorkerThreads << "\n";
    file << "max_request_size=" << config.m_nMaxRequestSize << "\n";
    file << "timeout_seconds=" << config.m_nTimeoutSeconds << "\n";
    file << "database_path=" << config.m_strDatabasePath << "\n";
    file << "connection_pool_size=" << config.m_nConnectionPoolSize << "\n";
    file << "memory_cache_max_items=" << config.m_nMemoryCacheMaxItems << "\n";
    file << "disk_cache_dir=" << config.m_strDiskCacheDir << "\n";
    file << "cache_ttl_seconds=" << config.m_nCacheTTLSeconds << "\n";
    file << "max_image_width=" << config.m_nMaxImageWidth << "\n";
    file << "max_image_height=" << config.m_nMaxImageHeight << "\n";
    file << "max_features_per_request=" << config.m_nMaxFeaturesPerRequest << "\n";
    file << "output_dir=" << config.m_strOutputDir << "\n";
    file << "log_file=" << config.m_strLogFile << "\n";
    file << "log_level=" << config.m_nLogLevel << "\n";
    
    return true;
#endif
}

ServerConfig GetDefaultConfig() {
    return ServerConfig();
}

} // namespace cycle