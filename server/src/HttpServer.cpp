#include "HttpServer.h"
#include "DatabaseManager.h"

// 检查是否包含cpp-httplib
#ifdef HAS_CPP_HTTPLIB
#include "httplib.h"
#else
// 简单模拟httplib::Server的前向声明，避免编译错误
namespace httplib {
class Server {
public:
    bool listen(const char* host, int port) { return false; }
    void stop() {}
};
}
#endif

// 尝试使用nlohmann/json，如果可用
#ifdef HAS_NLOHMANN_JSON
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#endif

#include "PngEncoder.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <cmath>

// 简单JSON解析辅助函数
namespace {
    std::string GetJsonString(const std::string& strJson, const std::string& strKey) {
        // 简单实现：查找"key":"value"
        size_t pos = strJson.find("\"" + strKey + "\":");
        if (pos == std::string::npos) return "";
        
        pos += strKey.length() + 3; // 跳过"key":
        size_t end = strJson.find_first_of(",\"}", pos);
        if (end == std::string::npos) end = strJson.length();
        
        std::string value = strJson.substr(pos, end - pos);
        // 去除引号
        if (value.length() >= 2 && value[0] == '"' && value[value.length()-1] == '"') {
            value = value.substr(1, value.length() - 2);
        }
        return value;
    }
    
    double GetJsonDouble(const std::string& strJson, const std::string& strKey) {
        std::string strValue = GetJsonString(strJson, strKey);
        if (strValue.empty()) return 0.0;
        try {
            return std::stod(strValue);
        } catch (...) {
            return 0.0;
        }
    }
    
    int GetJsonInt(const std::string& strJson, const std::string& strKey) {
        std::string strValue = GetJsonString(strJson, strKey);
        if (strValue.empty()) return 0;
        try {
            return std::stoi(strValue);
        } catch (...) {
            return 0;
        }
    }
}

namespace cycle {

HttpServer::HttpServer(const ServerConfig& config)
    : m_config(config) {
    m_strOutputDir = m_config.m_strOutputDir;
    
    // 确保输出目录存在
    std::string cmd = "mkdir -p \"" + m_strOutputDir + "\"";
    system(cmd.c_str());
    
    // 创建渲染引擎
    m_pRenderEngine = std::make_unique<RenderEngine>(m_config);
    m_pRenderEngine->SetOutputDir(m_strOutputDir);
    
    // 创建数据库管理器
    m_pDbManager = std::make_unique<DatabaseManager>(m_config.m_strDatabasePath, 
                                                      m_config.m_nConnectionPoolSize);
    
    std::cout << "Database manager initialized with path: " 
              << m_config.m_strDatabasePath << std::endl;
}

HttpServer::~HttpServer() {
    Stop();
}

bool HttpServer::Start() {
    if (m_bRunning) {
        return true;
    }
    
#ifdef HAS_CPP_HTTPLIB
    try {
        m_pServer = std::make_unique<httplib::Server>();
        
        // 设置路由
        SetupRoutes();
        
        // 启动服务器线程
        m_bRunning = true;
        m_vecWorkerThreads.emplace_back([this]() {
            std::string host = m_config.m_strHost;
            int port = m_config.m_nPort;
            
            std::cout << "Starting Map Server on " << host << ":" << port << std::endl;
            std::cout << "Output directory: " << m_strOutputDir << std::endl;
            
            if (!m_pServer->listen(host.c_str(), port)) {
                SetError("Failed to start server on " + host + ":" + std::to_string(port));
                m_bRunning = false;
            }
        });
        
        // 等待服务器启动
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        return m_bRunning;
    } catch (const std::exception& e) {
        SetError(std::string("Exception in Start: ") + e.what());
        return false;
    } catch (...) {
        SetError("Unknown exception in Start");
        return false;
    }
#else
    SetError("cpp-httplib not available");
    return false;
#endif
}

void HttpServer::Stop() {
    if (!m_bRunning) {
        return;
    }
    
#ifdef HAS_CPP_HTTPLIB
    if (m_pServer) {
        m_pServer->stop();
    }
#endif
    
    m_bRunning = false;
    
    // 等待工作线程结束
    for (auto& thread : m_vecWorkerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    m_vecWorkerThreads.clear();
    m_pServer.reset();
}

void HttpServer::SetupRoutes() {
#ifdef HAS_CPP_HTTPLIB
    if (!m_pServer) return;
    
    // 健康检查端点
    m_pServer->Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        std::string response, contentType;
        HandleHealthRequest(response, contentType);
        res.set_content(response, contentType);
    });
    
    // 服务能力端点
    m_pServer->Get("/capabilities", [this](const httplib::Request& req, httplib::Response& res) {
        std::string response, contentType;
        HandleCapabilitiesRequest(response, contentType);
        res.set_content(response, contentType.c_str());
    });
    
    // 图层列表端点
    m_pServer->Get("/layers", [this](const httplib::Request& req, httplib::Response& res) {
        std::string response, contentType;
        HandleLayersRequest(response, contentType);
        res.set_content(response, contentType.c_str());
    });
    
    // 图层详情端点
    m_pServer->Get(R"(/layers/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
        std::string layerId = req.matches[1];
        std::string response, contentType;
        HandleLayerInfoRequest(layerId, response, contentType);
        res.set_content(response, contentType.c_str());
    });
    
    // 地图生成端点 (POST)
    m_pServer->Post("/generate", [this](const httplib::Request& req, httplib::Response& res) {
        std::string response, contentType;
        HandleGenerateRequest(req.body, res, response, contentType);
        res.set_content(response, contentType.c_str());
    });
    
    // 指标收集端点
    m_pServer->Post("/metrics", [this](const httplib::Request& req, httplib::Response& res) {
        std::string response, contentType;
        HandleMetricsRequest(req.body, response, contentType);
        res.set_content(response, contentType.c_str());
    });
    
    // 瓦片服务端点 (GET)
    m_pServer->Get(R"(/tile/(\d+)/(\d+)/(\d+)\.png)", [this](const httplib::Request& req, httplib::Response& res) {
        int z = std::stoi(req.matches[1]);
        int x = std::stoi(req.matches[2]);
        int y = std::stoi(req.matches[3]);
        
        std::vector<uint8_t> response;
        std::string contentType;
        HandleTileRequest(z, x, y, res, response, contentType);
        
        if (!response.empty()) {
            res.set_content(reinterpret_cast<const char*>(response.data()), 
                           response.size(), 
                           contentType.c_str());
        } else {
            res.status = 404;
            res.set_content("Tile not found", "text/plain");
        }
    });
    
    // 静态文件服务（用于测试）
    m_pServer->set_mount_point("/", m_strOutputDir.c_str());
    
    // 设置CORS头
    m_pServer->set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        return httplib::Server::HandlerResponse::Unhandled;
    });
    
    // 处理OPTIONS请求（CORS预检）
    m_pServer->Options(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
    });
#endif
}

void HttpServer::HandleGenerateRequest(const std::string& strBody, 
                                       httplib::Response& res,
                                       std::string& strResponse,
                                       std::string& strContentType) {
    MapRequest request;
    
    if (!ParseMapRequest(strBody, request)) {
        strResponse = GenerateErrorResponse("INVALID_REQUEST", 
                                           "Failed to parse map request");
        strContentType = "application/json";
        return;
    }
    
    // 验证请求参数
    std::string validationError;
    if (!ValidateMapRequestParameters(request, validationError)) {
        strResponse = GenerateErrorResponse("INVALID_REQUEST", validationError);
        strContentType = "application/json";
        return;
    }
    
    // 渲染地图
    std::vector<uint8_t> vecPngData;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!m_pRenderEngine->RenderMap(request, vecPngData)) {
        strResponse = GenerateErrorResponse("RENDER_FAILED",
                                           m_pRenderEngine->GetError());
        strContentType = "application/json";
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // 生成文件名
    std::ostringstream oss;
    oss << "map_" << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()
        << ".png";
    
    std::string filename = oss.str();
    std::string filepath = m_strOutputDir + "/" + filename;
    
    // 保存到文件
    std::ofstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(vecPngData.data()), vecPngData.size());
        file.close();
    }
    
    // 返回PNG数据
    strResponse.assign(reinterpret_cast<const char*>(vecPngData.data()), vecPngData.size());
    strContentType = "image/png";
    
    // 设置响应头元数据
    res.set_header("x-render-time", std::to_string(duration.count()));
    res.set_header("x-cache-hit", "false");
    res.set_header("x-features-count", "0");
    res.set_header("x-timestamp", std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    // 格式化边界框
    std::string bbox = "[" + std::to_string(request.m_bbox.m_dMinX) + "," +
                       std::to_string(request.m_bbox.m_dMinY) + "," +
                       std::to_string(request.m_bbox.m_dMaxX) + "," +
                       std::to_string(request.m_bbox.m_dMaxY) + "]";
    res.set_header("x-bbox", bbox);
    
    // 添加响应头信息
    std::cout << "Rendered map: " << filename 
              << " (" << request.m_nWidth << "x" << request.m_nHeight 
              << ") in " << duration.count() << "ms" << std::endl;
}
#define M_PI 3.1415926535893756
void HttpServer::HandleTileRequest(int z, int x, int y,
                                   httplib::Response& res,
                                   std::vector<uint8_t>& vecResponse,
                                   std::string& strContentType) {
    // 验证瓦片参数
    std::string validationError;
    if (!ValidateTileParameters(z, x, y, validationError)) {
        res.status = 400;
        std::string strResponse = GenerateErrorResponse("INVALID_TILE_PARAMS", validationError);
        strContentType = "application/json";
        vecResponse.clear();
        return;
    }
    
    // 简单瓦片实现：生成一个256x256的测试瓦片
    auto startTime = std::chrono::high_resolution_clock::now();
    const int tileSize = 256;
    
    // 创建测试图像（棋盘格）
    std::vector<uint8_t> vecImageData(tileSize * tileSize * 4);
    bool dark = ((x + y) % 2 == 0);
    
    for (int py = 0; py < tileSize; ++py) {
        for (int px = 0; px < tileSize; ++px) {
            size_t index = (py * tileSize + px) * 4;
            if (dark) {
                vecImageData[index] = 100;     // R
                vecImageData[index + 1] = 100; // G
                vecImageData[index + 2] = 100; // B
                vecImageData[index + 3] = 255; // A
            } else {
                vecImageData[index] = 200;     // R
                vecImageData[index + 1] = 200; // G
                vecImageData[index + 2] = 200; // B
                vecImageData[index + 3] = 255; // A
            }
        }
    }
    
    // 添加瓦片坐标文本（简单实现）
    std::string text = "Z:" + std::to_string(z) + " X:" + std::to_string(x) + " Y:" + std::to_string(y);
    for (size_t i = 0; i < text.length() && i < 20; ++i) {
        int px = 10 + i * 8;
        int py = 20;
        for (int dy = 0; dy < 8; ++dy) {
            for (int dx = 0; dx < 8; ++dx) {
                int xpos = px + dx;
                int ypos = py + dy;
                if (xpos < tileSize && ypos < tileSize) {
                    size_t index = (ypos * tileSize + xpos) * 4;
                    vecImageData[index] = 255;     // R
                    vecImageData[index + 1] = 0;   // G
                    vecImageData[index + 2] = 0;   // B
                    vecImageData[index + 3] = 255; // A
                }
            }
        }
    }
    
    // 编码为PNG
    if (PngEncoder::EncodeToMemory(vecImageData.data(), tileSize, tileSize, vecResponse)) {
        strContentType = "image/png";
        
        // 保存到文件供缓存
        std::string filename = "tile_" + std::to_string(z) + "_" + 
                               std::to_string(x) + "_" + std::to_string(y) + ".png";
        std::string filepath = m_strOutputDir + "/tiles/" + filename;
        
        std::string dir = m_strOutputDir + "/tiles";
        system(("mkdir -p \"" + dir + "\"").c_str());
        
        std::ofstream file(filepath, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(vecResponse.data()), vecResponse.size());
            file.close();
        }
        
        // 设置响应头元数据
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        res.set_header("x-render-time", std::to_string(duration.count()));
        res.set_header("x-cache-hit", "false");
        res.set_header("x-features-count", "0");
        res.set_header("x-timestamp", std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        // 计算瓦片的边界框（Web Mercator）
        double n = pow(2, z);
        double tileX = x;
        double tileY = y;
        double lonLeft = tileX / n * 360.0 - 180.0;
        double lonRight = (tileX + 1) / n * 360.0 - 180.0;
        double latTop = atan(sinh(M_PI * (1 - 2 * tileY / n))) * 180.0 / M_PI;
        double latBottom = atan(sinh(M_PI * (1 - 2 * (tileY + 1) / n))) * 180.0 / M_PI;
        std::string bbox = "[" + std::to_string(lonLeft) + "," + std::to_string(latBottom) + "," +
                           std::to_string(lonRight) + "," + std::to_string(latTop) + "]";
        res.set_header("x-bbox", bbox);
    } else {
        vecResponse.clear();
    }
}

void HttpServer::HandleLayersRequest(std::string& strResponse,
                                     std::string& strContentType) {
    // 从数据库获取图层列表
    if (!m_pDbManager) {
        strResponse = GenerateErrorResponse("DATABASE_ERROR", 
                                           "Database manager not initialized");
        strContentType = "application/json";
        return;
    }
    
    try {
        auto layers = m_pDbManager->GetAllLayers();
        
#ifdef HAS_NLOHMANN_JSON
        // 使用nlohmann/json构建响应
        json response;
        json layersArray = json::array();
        
        for (const auto& layer : layers) {
            json layerObj = {
                {"id", layer.id},
                {"name", layer.name},
                {"type", layer.type},
                {"description", layer.description}
            };
            
            // 添加边界框（如果有效）
            if (layer.bounds.IsValid()) {
                json bboxArray = {
                    layer.bounds.m_dMinX,
                    layer.bounds.m_dMinY,
                    layer.bounds.m_dMaxX,
                    layer.bounds.m_dMaxY
                };
                layerObj["bounds"] = bboxArray;
            }
            
            // 添加属性列表
            if (!layer.attributes.empty()) {
                layerObj["attributes"] = layer.attributes;
            }
            
            layersArray.push_back(layerObj);
        }
        
        response["layers"] = layersArray;
        strResponse = response.dump(2); // 带缩进的JSON
#else
        // 回退到手动构建JSON
        std::ostringstream oss;
        oss << "{\n  \"layers\": [\n";
        
        for (size_t i = 0; i < layers.size(); ++i) {
            const auto& layer = layers[i];
            oss << "    {\n";
            oss << "      \"id\": \"" << layer.id << "\",\n";
            oss << "      \"name\": \"" << layer.name << "\",\n";
            oss << "      \"type\": \"" << layer.type << "\",\n";
            oss << "      \"description\": \"" << layer.description << "\"";
            
            // 添加边界框（如果有效）
            if (layer.bounds.IsValid()) {
                oss << ",\n      \"bounds\": [" 
                    << layer.bounds.m_dMinX << ", "
                    << layer.bounds.m_dMinY << ", "
                    << layer.bounds.m_dMaxX << ", "
                    << layer.bounds.m_dMaxY << "]";
            }
            
            // 添加属性列表
            if (!layer.attributes.empty()) {
                oss << ",\n      \"attributes\": [";
                for (size_t j = 0; j < layer.attributes.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << "\"" << layer.attributes[j] << "\"";
                }
                oss << "]";
            }
            
            oss << "\n    }";
            if (i < layers.size() - 1) oss << ",";
            oss << "\n";
        }
        
        oss << "  ]\n}";
        strResponse = oss.str();
#endif
        
        strContentType = "application/json";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in HandleLayersRequest: " << e.what() << std::endl;
        strResponse = GenerateErrorResponse("INTERNAL_ERROR", 
                                           "Failed to retrieve layers");
        strContentType = "application/json";
    }
}

void HttpServer::HandleHealthRequest(std::string& strResponse,
                                     std::string& strContentType) {
   
    strResponse = R"({
  "status": "healthy",
  "version": "1.0.0",
  "service": "map-server",
  "timestamp": ")" + []() -> std::string {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        tm tim;
        localtime_s(&tim, &in_time_t);
        std::stringstream ss;
        ss << std::put_time(&tim, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }() + R"(",
  "endpoints": [
    "/health",
    "/layers",
    "/generate",
    "/tile/{z}/{x}/{y}.png"
  ]
})";
    strContentType = "application/json";
}

bool HttpServer::ParseMapRequest(const std::string& strJson, MapRequest& request) {
    if (strJson.empty()) {
        // 使用默认请求
        request.m_bbox = BoundingBox(-180, -90, 180, 90);
        request.m_nWidth = 1024;
        request.m_nHeight = 768;
        request.m_strBackgroundColor = "#ffffff";
        return true;
    }
    #ifdef HAS_NLOHMANN_JSON
    try {
        auto json = nlohmann::json::parse(strJson);
        
        // 解析 bbox (支持数组格式或单独字段)
        if (json.contains("bbox") && json["bbox"].is_array() && json["bbox"].size() == 4) {
            request.m_bbox.m_dMinX = json["bbox"][0];
            request.m_bbox.m_dMinY = json["bbox"][1];
            request.m_bbox.m_dMaxX = json["bbox"][2];
            request.m_bbox.m_dMaxY = json["bbox"][3];
        } else if (json.contains("minx") && json.contains("miny") && 
                   json.contains("maxx") && json.contains("maxy")) {
            request.m_bbox.m_dMinX = json["minx"];
            request.m_bbox.m_dMinY = json["miny"];
            request.m_bbox.m_dMaxX = json["maxx"];
            request.m_bbox.m_dMaxY = json["maxy"];
        } else {
            return false; // 无效的 bbox
        }
        
        // 验证 bbox 有效性
        if (!request.m_bbox.IsValid() || 
            request.m_bbox.m_dMinX >= request.m_bbox.m_dMaxX ||
            request.m_bbox.m_dMinY >= request.m_bbox.m_dMaxY) {
            return false;
        }
        
        // 解析并验证其他参数...
        request.m_nWidth = json.value("width", 1024);
        request.m_nHeight = json.value("height", 768);
        
        // 尺寸限制检查
        if (request.m_nWidth <= 0 || request.m_nHeight <= 0 ||
            request.m_nWidth > m_config.m_nMaxImageWidth ||
            request.m_nHeight > m_config.m_nMaxImageHeight) {
            return false;
        }
        
        // 解析 layers 数组
        if (json.contains("layers") && json["layers"].is_array()) {
            for (const auto& layer : json["layers"]) {
                request.m_vecLayers.push_back(layer.get<std::string>());
            }
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        SetError(std::string("JSON parsing error: ") + e.what());
        return false;
    }
    #else
    try {
        // 解析边界框
        request.m_bbox.m_dMinX = GetJsonDouble(strJson, "minx");
        request.m_bbox.m_dMinY = GetJsonDouble(strJson, "miny");
        request.m_bbox.m_dMaxX = GetJsonDouble(strJson, "maxx");
        request.m_bbox.m_dMaxY = GetJsonDouble(strJson, "maxy");
        
        // 如果使用bbox数组格式
        std::string bboxStr = GetJsonString(strJson, "bbox");
        if (!bboxStr.empty()) {
            std::istringstream iss(bboxStr);
            char comma;
            if (iss >> request.m_bbox.m_dMinX >> comma 
                    >> request.m_bbox.m_dMinY >> comma
                    >> request.m_bbox.m_dMaxX >> comma
                    >> request.m_bbox.m_dMaxY) {
                // 成功解析
            }
        }
        
        // 解析尺寸
        request.m_nWidth = GetJsonInt(strJson, "width");
        request.m_nHeight = GetJsonInt(strJson, "height");
        
        // 解析背景颜色
        request.m_strBackgroundColor = GetJsonString(strJson, "background_color");
        if (request.m_strBackgroundColor.empty()) {
            request.m_strBackgroundColor = "#ffffff";
        }
        
        // 解析图层（简单实现）
        std::string layersStr = GetJsonString(strJson, "layers");
        if (!layersStr.empty()) {
            // 假设格式为"layer1,layer2,layer3"
            std::istringstream iss(layersStr);
            std::string layer;
            while (std::getline(iss, layer, ',')) {
                request.m_vecLayers.push_back(layer);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
    #endif
}

std::string HttpServer::GenerateErrorResponse(const std::string& strCode,
                                              const std::string& strMessage,
                                              const std::string& strDetails) {
    std::ostringstream oss;
    oss << R"({
  "error": {
    "code": ")" << strCode << R"(",
    "message": ")" << strMessage << R"(")";
    
    if (!strDetails.empty()) {
        oss << R"(",
    "details": ")" << strDetails << R"(")";
    }
    
    oss << R"(
  }
})";
    return oss.str();
}

void HttpServer::SetError(const std::string& strError) {
    m_strError = strError;
    std::cerr << "HttpServer Error: " << strError << std::endl;
}

void HttpServer::HandleLayerInfoRequest(const std::string& layerId,
                                        std::string& strResponse,
                                        std::string& strContentType) {
    // 从数据库获取图层信息
    if (!m_pDbManager) {
        strResponse = GenerateErrorResponse("DATABASE_ERROR", 
                                           "Database manager not initialized");
        strContentType = "application/json";
        return;
    }
    
    try {
        auto layer = m_pDbManager->GetLayerById(layerId);
        
        if (layer.id.empty()) {
            strResponse = GenerateErrorResponse("LAYER_NOT_FOUND",
                                               "Layer '" + layerId + "' not found");
            strContentType = "application/json";
            return;
        }
        
#ifdef HAS_NLOHMANN_JSON
        // 使用nlohmann/json构建响应
        json response = {
            {"id", layer.id},
            {"name", layer.name},
            {"type", layer.type},
            {"description", layer.description}
        };
        
        // 添加边界框（如果有效）
        if (layer.bounds.IsValid()) {
            response["bounds"] = {
                layer.bounds.m_dMinX,
                layer.bounds.m_dMinY,
                layer.bounds.m_dMaxX,
                layer.bounds.m_dMaxY
            };
        }
        
        // 添加属性列表
        if (!layer.attributes.empty()) {
            response["attributes"] = layer.attributes;
        }
        
        strResponse = response.dump(2); // 带缩进的JSON
#else
        // 回退到手动构建JSON
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"id\": \"" << layer.id << "\",\n";
        oss << "  \"name\": \"" << layer.name << "\",\n";
        oss << "  \"type\": \"" << layer.type << "\",\n";
        oss << "  \"description\": \"" << layer.description << "\"";
        
        // 添加边界框（如果有效）
        if (layer.bounds.IsValid()) {
            oss << ",\n  \"bounds\": [" 
                << layer.bounds.m_dMinX << ", "
                << layer.bounds.m_dMinY << ", "
                << layer.bounds.m_dMaxX << ", "
                << layer.bounds.m_dMaxY << "]";
        }
        
        // 添加属性列表
        if (!layer.attributes.empty()) {
            oss << ",\n  \"attributes\": [";
            for (size_t i = 0; i < layer.attributes.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << layer.attributes[i] << "\"";
            }
            oss << "]";
        }
        
        oss << "\n}";
        strResponse = oss.str();
#endif
        
        strContentType = "application/json";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in HandleLayerInfoRequest: " << e.what() << std::endl;
        strResponse = GenerateErrorResponse("INTERNAL_ERROR", 
                                           "Failed to retrieve layer info");
        strContentType = "application/json";
    }
}

void HttpServer::HandleCapabilitiesRequest(std::string& strResponse,
                                           std::string& strContentType) {
    // 构建服务能力信息
    try {
#ifdef HAS_NLOHMANN_JSON
        // 使用nlohmann/json构建响应
        json serviceInfo = {
            {"name", "map-server"},
            {"type", "WMS"},  // WMS-like service
            {"version", "1.0.0"},
            {"title", "Leaf Map Rendering Service"},
            {"abstract", "High-performance map rendering service for spatial data visualization"},
            {"keywords", {"map", "rendering", "GIS", "tile", "vector", "raster"}},
            {"onlineResource", "http://localhost:8080/"},
            {"contactInformation", {
                {"contactPerson", "System Administrator"},
                {"contactPosition", ""},
                {"contactAddress", {
                    {"addressType", ""},
                    {"address", ""},
                    {"city", ""},
                    {"stateOrProvince", ""},
                    {"postCode", ""},
                    {"country", ""}
                }},
                {"contactVoiceTelephone", ""},
                {"contactFacsimileTelephone", ""},
                {"contactElectronicMailAddress", "admin@example.com"}
            }},
            {"fees", "none"},
            {"accessConstraints", "none"}
        };
        
        json capabilities = {
            {"maxWidth", m_config.m_nMaxImageWidth},
            {"maxHeight", m_config.m_nMaxImageHeight},
            {"formats", {"png", "webp", "jpeg"}},
            {"projections", {"EPSG:4326", "EPSG:3857"}},
            {"supportedOperations", {
                {"GetCapabilities", {"GET", "POST"}},
                {"GetMap", {"GET", "POST"}},
                {"GetTile", {"GET"}},
                {"GetFeatureInfo", {"GET", "POST"}}
            }},
            {"supportedSRS", {
                "EPSG:4326",
                "EPSG:3857",
                "EPSG:900913"
            }},
            {"supportedStyles", {
                {"default", "Default style"},
                {"grayscale", "Grayscale style"},
                {"dark", "Dark mode style"}
            }}
        };
        
        // 从数据库获取图层列表（如果数据库管理器可用）
        if (m_pDbManager) {
            try {
                auto layers = m_pDbManager->GetAllLayers();
                json layersArray = json::array();
                
                for (const auto& layer : layers) {
                    json layerObj = {
                        {"id", layer.id},
                        {"name", layer.name},
                        {"type", layer.type},
                        {"description", layer.description}
                    };
                    
                    layersArray.push_back(layerObj);
                }
                
                capabilities["layers"] = layersArray;
            } catch (const std::exception& dbError) {
                std::cerr << "Failed to retrieve layers from database: " << dbError.what() << std::endl;
                // 使用模拟图层数据
                capabilities["layers"] = {
                    {{"id", "roads"}, {"name", "Roads"}, {"type", "vector"}, {"description", "Road network"}},
                    {{"id", "buildings"}, {"name", "Buildings"}, {"type", "vector"}, {"description", "Building footprints"}},
                    {{"id", "points"}, {"name", "Points of Interest"}, {"type", "vector"}, {"description", "Points of interest"}}
                };
            }
        } else {
            // 数据库管理器不可用，使用模拟数据
            capabilities["layers"] = {
                {{"id", "roads"}, {"name", "Roads"}, {"type", "vector"}, {"description", "Road network"}},
                {{"id", "buildings"}, {"name", "Buildings"}, {"type", "vector"}, {"description", "Building footprints"}},
                {{"id", "points"}, {"name", "Points of Interest"}, {"type", "vector"}, {"description", "Points of interest"}}
            };
        }
        
        json response = {
            {"service", serviceInfo},
            {"capabilities", capabilities}
        };
        
        strResponse = response.dump(2); // 带缩进的JSON
#else
        // 回退到手动构建JSON
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"service\": {\n";
        oss << "    \"name\": \"map-server\",\n";
        oss << "    \"type\": \"WMS\",\n";
        oss << "    \"version\": \"1.0.0\",\n";
        oss << "    \"title\": \"Leaf Map Rendering Service\",\n";
        oss << "    \"abstract\": \"High-performance map rendering service for spatial data visualization\",\n";
        oss << "    \"keywords\": [\"map\", \"rendering\", \"GIS\", \"tile\", \"vector\", \"raster\"],\n";
        oss << "    \"onlineResource\": \"http://localhost:8080/\",\n";
        oss << "    \"contactInformation\": {\n";
        oss << "      \"contactPerson\": \"System Administrator\",\n";
        oss << "      \"contactPosition\": \"\",\n";
        oss << "      \"contactAddress\": {\n";
        oss << "        \"addressType\": \"\",\n";
        oss << "        \"address\": \"\",\n";
        oss << "        \"city\": \"\",\n";
        oss << "        \"stateOrProvince\": \"\",\n";
        oss << "        \"postCode\": \"\",\n";
        oss << "        \"country\": \"\"\n";
        oss << "      },\n";
        oss << "      \"contactVoiceTelephone\": \"\",\n";
        oss << "      \"contactFacsimileTelephone\": \"\",\n";
        oss << "      \"contactElectronicMailAddress\": \"admin@example.com\"\n";
        oss << "    },\n";
        oss << "    \"fees\": \"none\",\n";
        oss << "    \"accessConstraints\": \"none\"\n";
        oss << "  },\n";
        oss << "  \"capabilities\": {\n";
        oss << "    \"maxWidth\": " << m_config.m_nMaxImageWidth << ",\n";
        oss << "    \"maxHeight\": " << m_config.m_nMaxImageHeight << ",\n";
        oss << "    \"formats\": [\"png\", \"webp\", \"jpeg\"],\n";
        oss << "    \"projections\": [\"EPSG:4326\", \"EPSG:3857\"],\n";
        oss << "    \"supportedOperations\": {\n";
        oss << "      \"GetCapabilities\": [\"GET\", \"POST\"],\n";
        oss << "      \"GetMap\": [\"GET\", \"POST\"],\n";
        oss << "      \"GetTile\": [\"GET\"],\n";
        oss << "      \"GetFeatureInfo\": [\"GET\", \"POST\"]\n";
        oss << "    },\n";
        oss << "    \"supportedSRS\": [\n";
        oss << "      \"EPSG:4326\",\n";
        oss << "      \"EPSG:3857\",\n";
        oss << "      \"EPSG:900913\"\n";
        oss << "    ],\n";
        oss << "    \"supportedStyles\": {\n";
        oss << "      \"default\": \"Default style\",\n";
        oss << "      \"grayscale\": \"Grayscale style\",\n";
        oss << "      \"dark\": \"Dark mode style\"\n";
        oss << "    }";
        
        // 从数据库获取图层列表（如果数据库管理器可用）
        if (m_pDbManager) {
            try {
                auto layers = m_pDbManager->GetAllLayers();
                oss << ",\n    \"layers\": [\n";
                
                for (size_t i = 0; i < layers.size(); ++i) {
                    const auto& layer = layers[i];
                    oss << "      {\n";
                    oss << "        \"id\": \"" << layer.id << "\",\n";
                    oss << "        \"name\": \"" << layer.name << "\",\n";
                    oss << "        \"type\": \"" << layer.type << "\",\n";
                    oss << "        \"description\": \"" << layer.description << "\"";
                    
                    // 添加边界框（如果有效）
                    if (layer.bounds.IsValid()) {
                        oss << ",\n        \"bounds\": [" 
                            << layer.bounds.m_dMinX << ", "
                            << layer.bounds.m_dMinY << ", "
                            << layer.bounds.m_dMaxX << ", "
                            << layer.bounds.m_dMaxY << "]";
                    }
                    
                    // 添加属性列表
                    if (!layer.attributes.empty()) {
                        oss << ",\n        \"attributes\": [";
                        for (size_t j = 0; j < layer.attributes.size(); ++j) {
                            if (j > 0) oss << ", ";
                            oss << "\"" << layer.attributes[j] << "\"";
                        }
                        oss << "]";
                    }
                    
                    oss << "\n      }";
                    if (i < layers.size() - 1) oss << ",";
                    oss << "\n";
                }
                
                oss << "    ]";
            } catch (const std::exception& dbError) {
                std::cerr << "Failed to retrieve layers from database: " << dbError.what() << std::endl;
                // 使用模拟图层数据
                oss << ",\n    \"layers\": [\n";
                oss << "      {\"id\": \"roads\", \"name\": \"Roads\", \"type\": \"vector\", \"description\": \"Road network\"},\n";
                oss << "      {\"id\": \"buildings\", \"name\": \"Buildings\", \"type\": \"vector\", \"description\": \"Building footprints\"},\n";
                oss << "      {\"id\": \"points\", \"name\": \"Points of Interest\", \"type\": \"vector\", \"description\": \"Points of interest\"}\n";
                oss << "    ]";
            }
        } else {
            // 数据库管理器不可用，使用模拟数据
            oss << ",\n    \"layers\": [\n";
            oss << "      {\"id\": \"roads\", \"name\": \"Roads\", \"type\": \"vector\", \"description\": \"Road network\"},\n";
            oss << "      {\"id\": \"buildings\", \"name\": \"Buildings\", \"type\": \"vector\", \"description\": \"Building footprints\"},\n";
            oss << "      {\"id\": \"points\", \"name\": \"Points of Interest\", \"type\": \"vector\", \"description\": \"Points of interest\"}\n";
            oss << "    ]";
        }
        
        oss << "\n  }\n}";
        strResponse = oss.str();
#endif
        
        strContentType = "application/json";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in HandleCapabilitiesRequest: " << e.what() << std::endl;
        strResponse = GenerateErrorResponse("INTERNAL_ERROR", 
                                           "Failed to retrieve capabilities");
        strContentType = "application/json";
    }
}

void HttpServer::HandleMetricsRequest(const std::string& strBody,
                                      std::string& strResponse,
                                      std::string& strContentType) {
#ifdef HAS_NLOHMANN_JSON
    // 使用nlohmann/json解析指标数据
    try {
        auto jsonData = nlohmann::json::parse(strBody);
        
        if (!jsonData.contains("metrics") || !jsonData["metrics"].is_array()) {
            strResponse = GenerateErrorResponse("INVALID_REQUEST", 
                                               "Missing or invalid 'metrics' array");
            strContentType = "application/json";
            return;
        }
        
        const auto& metricsArray = jsonData["metrics"];
        std::cout << "Received " << metricsArray.size() << " metrics" << std::endl;
        
        // 记录每个指标
        for (const auto& metric : metricsArray) {
            if (!metric.contains("name") || !metric.contains("value")) {
                std::cerr << "Warning: Skipping invalid metric entry" << std::endl;
                continue;
            }
            
            std::string name = metric["name"];
            double value = metric["value"];
            std::string timestamp = metric.value("timestamp", "");
            std::string tagsStr = "{}";
            
            if (metric.contains("tags")) {
                tagsStr = metric["tags"].dump();
            }
            
            // 格式化日志输出
            std::string logEntry = "Metric: " + name + " = " + std::to_string(value);
            if (!timestamp.empty()) {
                logEntry += " @ " + timestamp;
            }
            if (metric.contains("tags") && !metric["tags"].empty()) {
                logEntry += " tags: " + tagsStr;
            }
            
            std::cout << logEntry << std::endl;
            
            // 这里可以扩展：写入文件、数据库或监控系统
            // 例如：WriteMetricToFile(name, value, timestamp, tagsStr);
        }
        
        // 返回成功响应
        strResponse = R"({
  "status": "ok",
  "message": "Metrics received successfully",
  "count": )" + std::to_string(metricsArray.size()) + R"(
})";
        strContentType = "application/json";
        
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error in HandleMetricsRequest: " << e.what() << std::endl;
        strResponse = GenerateErrorResponse("INVALID_JSON", 
                                           "Failed to parse JSON request");
        strContentType = "application/json";
    } catch (const std::exception& e) {
        std::cerr << "Error in HandleMetricsRequest: " << e.what() << std::endl;
        strResponse = GenerateErrorResponse("INTERNAL_ERROR", 
                                           "Failed to process metrics");
        strContentType = "application/json";
    }
#else
    // 回退到简单日志记录（保持向后兼容）
    try {
        std::cout << "Received metrics data: " << strBody << std::endl;
        
        // 尝试使用简单的JSON解析辅助函数
        std::string metricsStr = GetJsonString(strBody, "metrics");
        if (!metricsStr.empty()) {
            // 简单计数 - 假设格式为 [ {...}, {...} ]
            size_t count = 0;
            size_t pos = metricsStr.find('{');
            while (pos != std::string::npos) {
                count++;
                pos = metricsStr.find('{', pos + 1);
            }
            std::cout << "Approximately " << count << " metrics detected" << std::endl;
        }
        
        // 返回成功响应
        strResponse = R"({
  "status": "ok",
  "message": "Metrics received successfully"
})";
        strContentType = "application/json";
        
    } catch (const std::exception& e) {
        std::cerr << "Error in HandleMetricsRequest: " << e.what() << std::endl;
        strResponse = GenerateErrorResponse("INTERNAL_ERROR", 
                                           "Failed to process metrics");
        strContentType = "application/json";
    }
#endif
}

bool HttpServer::ValidateTileParameters(int z, int x, int y, std::string& strError) {
    // 验证缩放级别
    if (z < 0 || z > 20) { // 通常瓦片服务的最大缩放级别是20
        strError = "Zoom level must be between 0 and 20";
        return false;
    }
    
    // 计算该缩放级别下的最大瓦片坐标
    int maxTile = (1 << z) - 1; // 2^z - 1
    
    // 验证X坐标
    if (x < 0 || x > maxTile) {
        strError = "Tile X coordinate must be between 0 and " + std::to_string(maxTile);
        return false;
    }
    
    // 验证Y坐标
    if (y < 0 || y > maxTile) {
        strError = "Tile Y coordinate must be between 0 and " + std::to_string(maxTile);
        return false;
    }
    
    return true;
}

bool HttpServer::ValidateMapRequestParameters(const MapRequest& request, std::string& strError) {
    // 验证边界框
    if (!request.m_bbox.IsValid()) {
        strError = "Invalid bounding box. minX must be less than maxX and minY less than maxY";
        return false;
    }
    
    // 验证边界框坐标范围
    const double& minX = request.m_bbox.m_dMinX;
    const double& minY = request.m_bbox.m_dMinY;
    const double& maxX = request.m_bbox.m_dMaxX;
    const double& maxY = request.m_bbox.m_dMaxY;
    
    if (minX < -180.0 || maxX > 180.0) {
        strError = "Longitude values must be between -180 and 180 degrees";
        return false;
    }
    
    if (minY < -90.0 || maxY > 90.0) {
        strError = "Latitude values must be between -90 and 90 degrees";
        return false;
    }
    
    // 验证图像尺寸
    if (request.m_nWidth <= 0 || request.m_nHeight <= 0) {
        strError = "Image width and height must be positive";
        return false;
    }
    
    // 验证尺寸限制（使用配置中的最大值）
    if (request.m_nWidth > m_config.m_nMaxImageWidth) {
        strError = "Image width exceeds maximum allowed value of " + 
                   std::to_string(m_config.m_nMaxImageWidth);
        return false;
    }
    
    if (request.m_nHeight > m_config.m_nMaxImageHeight) {
        strError = "Image height exceeds maximum allowed value of " + 
                   std::to_string(m_config.m_nMaxImageHeight);
        return false;
    }
    
    // 验证背景颜色格式（如果提供）
    if (!request.m_strBackgroundColor.empty()) {
        // 简单验证：检查是否以#开头且长度为7（#RRGGBB）或4（#RGB）
        const std::string& color = request.m_strBackgroundColor;
        if (color[0] != '#' || (color.length() != 7 && color.length() != 4)) {
            strError = "Background color must be in format #RRGGBB or #RGB";
            return false;
        }
        
        // 检查十六进制字符
        for (size_t i = 1; i < color.length(); ++i) {
            char c = color[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                strError = "Background color contains invalid hexadecimal characters";
                return false;
            }
        }
    }
    
    // 验证图层名称（如果有）
    for (const auto& layer : request.m_vecLayers) {
        if (layer.empty()) {
            strError = "Layer names cannot be empty";
            return false;
        }
        
        // 检查图层名称是否只包含允许的字符
        for (char c : layer) {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
                  (c >= '0' && c <= '9') || c == '_' || c == '-')) {
                strError = "Layer name '" + layer + "' contains invalid characters. " +
                           "Only letters, numbers, underscores and hyphens are allowed";
                return false;
            }
        }
    }
    
    return true;
}

} // namespace cycle