#pragma once

#include "Config.h"
#include "RenderEngine.h"
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <vector>

// 前向声明cpp-httplib类
namespace httplib {
class Server;
struct Response;
}

namespace cycle {

// 前向声明DatabaseManager
class DatabaseManager;

class HttpServer {
public:
    HttpServer(const ServerConfig& config);
    ~HttpServer();
    
    // 禁止拷贝
    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    
    // 启动服务器
    bool Start();
    
    // 停止服务器
    void Stop();
    
    // 检查服务器是否正在运行
    bool IsRunning() const { return m_bRunning; }
    
    // 获取服务器端口
    uint16_t GetPort() const { return m_config.m_nPort; }
    
    // 获取错误信息
    const std::string& GetError() const { return m_strError; }

private:
    // 初始化路由
    void SetupRoutes();
    
    // 请求处理函数
    void HandleGenerateRequest(const std::string& strBody, 
                               httplib::Response& res,
                               std::string& strResponse,
                               std::string& strContentType);
    
    void HandleTileRequest(int z, int x, int y,
                           httplib::Response& res,
                           std::vector<uint8_t>& vecResponse,
                           std::string& strContentType);
    
    void HandleLayersRequest(std::string& strResponse,
                             std::string& strContentType);
    
    void HandleHealthRequest(std::string& strResponse,
                             std::string& strContentType);
    
    void HandleLayerInfoRequest(const std::string& layerId,
                                std::string& strResponse,
                                std::string& strContentType);
    
    void HandleCapabilitiesRequest(std::string& strResponse,
                                   std::string& strContentType);
    
    void HandleMetricsRequest(const std::string& strBody,
                              std::string& strResponse,
                              std::string& strContentType);
    
    // 验证函数
    bool ValidateTileParameters(int z, int x, int y, std::string& strError);
    bool ValidateMapRequestParameters(const MapRequest& request, std::string& strError);
    
    // 解析JSON请求
    bool ParseMapRequest(const std::string& strJson, MapRequest& request);
    
    // 生成错误响应
    std::string GenerateErrorResponse(const std::string& strCode,
                                      const std::string& strMessage,
                                      const std::string& strDetails = "");
    
    // 错误处理
    void SetError(const std::string& strError);

private:
    ServerConfig m_config;
    std::unique_ptr<httplib::Server> m_pServer;
    std::unique_ptr<RenderEngine> m_pRenderEngine;
    std::unique_ptr<DatabaseManager> m_pDbManager;
    std::atomic<bool> m_bRunning{false};
    std::vector<std::thread> m_vecWorkerThreads;
    std::string m_strError;
    
    // 输出目录
    std::string m_strOutputDir;
};

} // namespace cycle