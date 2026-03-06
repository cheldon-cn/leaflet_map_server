#include "RenderEngine.h"
#include "PngEncoder.h"

// 包含现有绘图模块
#include "cn_draw_facade.h"
#include "cn_raster_image_device.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>
#include <regex>

namespace cycle {

// MapRequest方法实现
std::string MapRequest::GenerateCacheKey() const {
    std::ostringstream oss;
    oss << m_bbox.m_dMinX << "," << m_bbox.m_dMinY << ","
        << m_bbox.m_dMaxX << "," << m_bbox.m_dMaxY << ","
        << m_nWidth << "x" << m_nHeight << ",bg:" << m_strBackgroundColor;
    
    for (const auto& layer : m_vecLayers) {
        oss << ",layer:" << layer;
    }
    
    // 简单哈希（实际应使用MD5或类似）
    std::string strKey = oss.str();
    size_t nHash = std::hash<std::string>{}(strKey);
    return std::to_string(nHash);
}

// RenderEngine实现
RenderEngine::RenderEngine(const ServerConfig& config)
    : m_config(config) {
    if (!Initialize()) {
        SetError("Failed to initialize RenderEngine");
    }
}

RenderEngine::~RenderEngine() {
    Cleanup();
}

RenderEngine::RenderEngine(RenderEngine&& other) noexcept
    : m_config(other.m_config)
   // , m_pDrawFacade(std::move(other.m_pDrawFacade))
    , m_pRasterDevice(std::move(other.m_pRasterDevice))
    , m_strError(std::move(other.m_strError))
    , m_strOutputDir(std::move(other.m_strOutputDir))
    , m_bInitialized(other.m_bInitialized)
    , m_colorCache(std::move(other.m_colorCache)) {
    other.m_bInitialized = false;
}

RenderEngine& RenderEngine::operator=(RenderEngine&& other) noexcept {
    if (this != &other) {
        Cleanup();
        
        m_config = other.m_config;
   //     m_pDrawFacade = std::move(other.m_pDrawFacade);
        m_pRasterDevice = std::move(other.m_pRasterDevice);
        m_strError = std::move(other.m_strError);
        m_strOutputDir = std::move(other.m_strOutputDir);
        m_bInitialized = other.m_bInitialized;
        m_colorCache = std::move(other.m_colorCache);
        
        other.m_bInitialized = false;
    }
    return *this;
}

bool RenderEngine::Initialize() {
    if (m_bInitialized) {
        return true;
    }
    
    try {
        // 创建绘图门面
      //  m_pDrawFacade = std::make_unique<CNDrawFacade>();
        
        // 创建栅格图像设备
        SNImageDeviceConfig deviceConfig;
        deviceConfig.eFormat = ImageFormat::ifPNG;
        deviceConfig.bTransparent = true;
        
     //   m_pRasterDevice = std::make_unique<CNRasterImageDevice>(deviceConfig);
        
        m_bInitialized = true;
        return true;
    } catch (const std::exception& e) {
        SetError(std::string("Exception in Initialize: ") + e.what());
        return false;
    } catch (...) {
        SetError("Unknown exception in Initialize");
        return false;
    }
}

void RenderEngine::Cleanup() {
  //  m_pDrawFacade.reset();
    m_pRasterDevice.reset();
    m_bInitialized = false;
}

void RenderEngine::SetError(const std::string& strError) {
    m_strError = strError;
}

void RenderEngine::SetOutputDir(const std::string& strDir) {
    m_strOutputDir = strDir;
    // 确保目录存在
    std::string cmd = "mkdir -p \"" + m_strOutputDir + "\"";
    system(cmd.c_str());
}

bool RenderEngine::CreateBlankImage(int nWidth, int nHeight, const std::string& strBgColor,
                                    std::vector<uint8_t>& vecImageData) {
    if (nWidth <= 0 || nHeight <= 0) {
        SetError("Invalid image dimensions");
        return false;
    }
    
    // 解析背景颜色
    uint8_t r = 255, g = 255, b = 255, a = 255;
    
    // 简单颜色解析（复制自CNRasterImageDevice的逻辑）
    if (!strBgColor.empty() && strBgColor[0] == '#') {
        // #RRGGBB 或 #RRGGBBAA 格式
        std::string hex = strBgColor.substr(1);
        if (hex.length() == 6 || hex.length() == 8) {
            r = static_cast<uint8_t>(std::stoi(hex.substr(0, 2), nullptr, 16));
            g = static_cast<uint8_t>(std::stoi(hex.substr(2, 2), nullptr, 16));
            b = static_cast<uint8_t>(std::stoi(hex.substr(4, 2), nullptr, 16));
            if (hex.length() == 8) {
                a = static_cast<uint8_t>(std::stoi(hex.substr(6, 2), nullptr, 16));
            }
        }
    }
    
    // 创建RGBA图像数据
    size_t nDataSize = static_cast<size_t>(nWidth) * nHeight * 4;
    vecImageData.resize(nDataSize);
    
    for (size_t i = 0; i < nDataSize; i += 4) {
        vecImageData[i] = r;     // R
        vecImageData[i + 1] = g; // G
        vecImageData[i + 2] = b; // B
        vecImageData[i + 3] = a; // A
    }
    
    return true;
}

bool RenderEngine::WorldToPixel(double dWorldX, double dWorldY,
                                const BoundingBox& bbox, int nWidth, int nHeight,
                                int& nPixelX, int& nPixelY) {
    if (!bbox.IsValid() || nWidth <= 0 || nHeight <= 0) {
        return false;
    }
    
    // 线性映射
    double dXPercent = (dWorldX - bbox.m_dMinX) / bbox.Width();
    double dYPercent = (dWorldY - bbox.m_dMinY) / bbox.Height();
    
    // Y轴反转（图像坐标原点在左上角）
    nPixelX = static_cast<int>(dXPercent * nWidth);
    nPixelY = static_cast<int>((1.0 - dYPercent) * nHeight);
    
    // 边界检查
    nPixelX = std::max(0, std::min(nWidth - 1, nPixelX));
    nPixelY = std::max(0, std::min(nHeight - 1, nPixelY));
    
    return true;
}

bool RenderEngine::DrawFeatures(const std::vector<std::shared_ptr<CNFeature>>& vecFeatures,
                                const LayerStyle& style,
                                std::vector<uint8_t>& vecImageData,
                                int nWidth, int nHeight,
                                const BoundingBox& bbox) {
    // 简单实现：绘制一个矩形作为示例
    // 实际应遍历要素并调用几何栅格化器
    
    // 解析样式颜色
    uint8_t r = 255, g = 0, b = 0, a = style.m_nOpacity;
    
    // 简单颜色解析
    std::string color = style.m_strFillColor;
    if (!color.empty() && color[0] == '#') {
        std::string hex = color.substr(1);
        if (hex.length() >= 6) {
            r = static_cast<uint8_t>(std::stoi(hex.substr(0, 2), nullptr, 16));
            g = static_cast<uint8_t>(std::stoi(hex.substr(2, 2), nullptr, 16));
            b = static_cast<uint8_t>(std::stoi(hex.substr(4, 2), nullptr, 16));
        }
    }
    
    // 在图像中心绘制一个矩形
    int centerX = nWidth / 2;
    int centerY = nHeight / 2;
    int rectWidth = nWidth / 4;
    int rectHeight = nHeight / 4;
    
    int startX = centerX - rectWidth / 2;
    int startY = centerY - rectHeight / 2;
    int endX = startX + rectWidth;
    int endY = startY + rectHeight;
    
    // 确保在边界内
    startX = std::max(0, startX);
    startY = std::max(0, startY);
    endX = std::min(nWidth - 1, endX);
    endY = std::min(nHeight - 1, endY);
    
    // 绘制矩形
    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            size_t index = (static_cast<size_t>(y) * nWidth + x) * 4;
            if (index + 3 < vecImageData.size()) {
                vecImageData[index] = r;     // R
                vecImageData[index + 1] = g; // G
                vecImageData[index + 2] = b; // B
                vecImageData[index + 3] = a; // A
            }
        }
    }
    
    // 绘制边框
    uint8_t strokeR = 0, strokeG = 0, strokeB = 0;
    std::string strokeColor = style.m_strStrokeColor;
    if (!strokeColor.empty() && strokeColor[0] == '#') {
        std::string hex = strokeColor.substr(1);
        if (hex.length() >= 6) {
            strokeR = static_cast<uint8_t>(std::stoi(hex.substr(0, 2), nullptr, 16));
            strokeG = static_cast<uint8_t>(std::stoi(hex.substr(2, 2), nullptr, 16));
            strokeB = static_cast<uint8_t>(std::stoi(hex.substr(4, 2), nullptr, 16));
        }
    }
    
    int strokeWidth = std::max(1, style.m_nStrokeWidth);
    
    // 上边框
    for (int w = 0; w < strokeWidth; ++w) {
        int y = startY + w;
        if (y < nHeight) {
            for (int x = startX; x <= endX; ++x) {
                size_t index = (static_cast<size_t>(y) * nWidth + x) * 4;
                if (index + 3 < vecImageData.size()) {
                    vecImageData[index] = strokeR;
                    vecImageData[index + 1] = strokeG;
                    vecImageData[index + 2] = strokeB;
                    vecImageData[index + 3] = a;
                }
            }
        }
    }
    
    // 下边框
    for (int w = 0; w < strokeWidth; ++w) {
        int y = endY - w;
        if (y >= 0) {
            for (int x = startX; x <= endX; ++x) {
                size_t index = (static_cast<size_t>(y) * nWidth + x) * 4;
                if (index + 3 < vecImageData.size()) {
                    vecImageData[index] = strokeR;
                    vecImageData[index + 1] = strokeG;
                    vecImageData[index + 2] = strokeB;
                    vecImageData[index + 3] = a;
                }
            }
        }
    }
    
    // 左边框
    for (int w = 0; w < strokeWidth; ++w) {
        int x = startX + w;
        if (x < nWidth) {
            for (int y = startY; y <= endY; ++y) {
                size_t index = (static_cast<size_t>(y) * nWidth + x) * 4;
                if (index + 3 < vecImageData.size()) {
                    vecImageData[index] = strokeR;
                    vecImageData[index + 1] = strokeG;
                    vecImageData[index + 2] = strokeB;
                    vecImageData[index + 3] = a;
                }
            }
        }
    }
    
    // 右边框
    for (int w = 0; w < strokeWidth; ++w) {
        int x = endX - w;
        if (x >= 0) {
            for (int y = startY; y <= endY; ++y) {
                size_t index = (static_cast<size_t>(y) * nWidth + x) * 4;
                if (index + 3 < vecImageData.size()) {
                    vecImageData[index] = strokeR;
                    vecImageData[index + 1] = strokeG;
                    vecImageData[index + 2] = strokeB;
                    vecImageData[index + 3] = a;
                }
            }
        }
    }
    
    return true;
}

bool RenderEngine::RenderMap(const MapRequest& request, std::vector<uint8_t>& vecPngData) {
    if (!m_bInitialized) {
        SetError("RenderEngine not initialized");
        return false;
    }
    
    if (!request.m_bbox.IsValid()) {
        SetError("Invalid bounding box");
        return false;
    }
    
    if (request.m_nWidth <= 0 || request.m_nHeight <= 0 ||
        request.m_nWidth > m_config.m_nMaxImageWidth || 
        request.m_nHeight > m_config.m_nMaxImageHeight) {
        SetError("Invalid image dimensions");
        return false;
    }
    
    // 创建空白图像
    std::vector<uint8_t> vecImageData;
    if (!CreateBlankImage(request.m_nWidth, request.m_nHeight, 
                         request.m_strBackgroundColor, vecImageData)) {
        return false;
    }
    
    // 如果没有指定图层，使用默认图层
    if (request.m_vecLayers.empty()) {
        // 绘制示例要素
        LayerStyle defaultStyle;
        defaultStyle.m_strFillColor = "#cccccc";
        defaultStyle.m_strStrokeColor = "#000000";
        defaultStyle.m_nStrokeWidth = 2;
        
        std::vector<std::shared_ptr<CNFeature>> emptyFeatures; // 空要素列表
        if (!DrawFeatures(emptyFeatures, defaultStyle, vecImageData,
                         request.m_nWidth, request.m_nHeight, request.m_bbox)) {
            return false;
        }
    } else {
        // 为每个图层应用样式
        for (const auto& layerName : request.m_vecLayers) {
            LayerStyle style;
            auto it = request.m_mapStyles.find(layerName);
            if (it != request.m_mapStyles.end()) {
                style = it->second;
            }
            
            // 实际应查询该图层的要素
            std::vector<std::shared_ptr<CNFeature>> layerFeatures;
            // TODO: 从数据库查询要素
            
            if (!DrawFeatures(layerFeatures, style, vecImageData,
                             request.m_nWidth, request.m_nHeight, request.m_bbox)) {
                return false;
            }
        }
    }
    
    // 编码为PNG
    if (!PngEncoder::EncodeToMemory(vecImageData.data(), 
                                    request.m_nWidth, request.m_nHeight,
                                    vecPngData)) {
        SetError("Failed to encode PNG: " + PngEncoder().GetError());
        return false;
    }
    
    return true;
}

bool RenderEngine::RenderMapToFile(const MapRequest& request, const std::string& strFilePath) {
    std::vector<uint8_t> vecPngData;
    if (!RenderMap(request, vecPngData)) {
        return false;
    }
    
    // 保存到文件
    std::ofstream file(strFilePath, std::ios::binary);
    if (!file.is_open()) {
        SetError("Failed to open file for writing: " + strFilePath);
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(vecPngData.data()), vecPngData.size());
    return !file.fail();
}

} // namespace cycle