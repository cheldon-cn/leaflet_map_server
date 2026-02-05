#pragma once

#include <string>
#include <cstdint>

namespace cycle {

struct ServerConfig {
    // HTTP server配置
    std::string m_strHost = "0.0.0.0";
    uint16_t m_nPort = 8080;
    size_t m_nWorkerThreads = 4;
    size_t m_nMaxRequestSize = 10 * 1024 * 1024; // 10MB
    int m_nTimeoutSeconds = 30;
    
    // 数据库配置
    std::string m_strDatabasePath = "./spatial_data.db";
    size_t m_nConnectionPoolSize = 5;
    
    // 缓存配置
    size_t m_nMemoryCacheMaxItems = 100;
    std::string m_strDiskCacheDir = "./tile_cache";
    int m_nCacheTTLSeconds = 30;
    
    // 限制配置
    int m_nMaxImageWidth = 4096;
    int m_nMaxImageHeight = 4096;
    int m_nMaxFeaturesPerRequest = 10000;
    
    // 输出目录（leaf文件夹）
    std::string m_strOutputDir = "./leaf/output";
    
    // 日志配置
    std::string m_strLogFile = "./map_server.log";
    int m_nLogLevel = 1; // 0=error,1=info,2=debug
};

// 加载配置从JSON文件
bool LoadConfigFromFile(const std::string& strFilePath, ServerConfig& config);

// 保存配置到JSON文件
bool SaveConfigToFile(const std::string& strFilePath, const ServerConfig& config);

// 获取默认配置
ServerConfig GetDefaultConfig();

} // namespace cycle