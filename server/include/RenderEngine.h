#pragma once

#include "Config.h"
#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include <map>

// 前向声明现有绘图类
namespace cycle {
class CNDrawFacade;
class CNRasterImageDevice;
class CNFeature;
} // namespace cycle

namespace cycle {

// 边界框（WGS84坐标）
struct BoundingBox {
    double m_dMinX = 0.0;
    double m_dMinY = 0.0;
    double m_dMaxX = 0.0;
    double m_dMaxY = 0.0;
    
    BoundingBox() = default;
    BoundingBox(double dMinX, double dMinY, double dMaxX, double dMaxY)
        : m_dMinX(dMinX), m_dMinY(dMinY), m_dMaxX(dMaxX), m_dMaxY(dMaxY) {}
    
    bool IsValid() const {
        return m_dMinX < m_dMaxX && m_dMinY < m_dMaxY;
    }
    
    double Width() const { return m_dMaxX - m_dMinX; }
    double Height() const { return m_dMaxY - m_dMinY; }
};

// 图层样式
struct LayerStyle {
    std::string m_strFillColor = "#cccccc";   // 填充颜色
    std::string m_strStrokeColor = "#000000"; // 边框颜色
    int m_nStrokeWidth = 1;                   // 边框宽度
    int m_nOpacity = 255;                     // 不透明度 (0-255)
    
    // 点样式
    int m_nPointRadius = 3;                   // 点半径
    std::string m_strPointColor = "#ff0000";  // 点颜色
    
    // 线样式
    std::string m_strLineColor = "#0000ff";   // 线颜色
    int m_nLineWidth = 2;                     // 线宽
};

// 地图请求
struct MapRequest {
    BoundingBox m_bbox;                       // 边界框
    int m_nWidth = 1024;                      // 输出宽度
    int m_nHeight = 768;                      // 输出高度
    std::vector<std::string> m_vecLayers;     // 图层列表
    std::map<std::string, LayerStyle> m_mapStyles; // 图层样式映射
    std::string m_strBackgroundColor = "#ffffff"; // 背景颜色
    std::string m_strFilter;                  // 属性过滤条件
    
    // 生成唯一缓存键
    std::string GenerateCacheKey() const;
};

// 渲染引擎
class RenderEngine {
public:
    RenderEngine(const ServerConfig& config);
    ~RenderEngine();
    
    // 禁止拷贝
    RenderEngine(const RenderEngine&) = delete;
    RenderEngine& operator=(const RenderEngine&) = delete;
    
    // 允许移动
    RenderEngine(RenderEngine&& other) noexcept;
    RenderEngine& operator=(RenderEngine&& other) noexcept;
    
    // 渲染地图并返回PNG数据
    bool RenderMap(const MapRequest& request, std::vector<uint8_t>& vecPngData);
    
    // 渲染地图并保存到文件
    bool RenderMapToFile(const MapRequest& request, const std::string& strFilePath);
    
    // 获取错误信息
    const std::string& GetError() const { return m_strError; }
    
    // 设置输出目录（leaf文件夹下的子目录）
    void SetOutputDir(const std::string& strDir);
    
    // 获取当前输出目录
    const std::string& GetOutputDir() const { return m_strOutputDir; }
    
private:
    // 初始化绘图设施
    bool Initialize();
    
    // 清理资源
    void Cleanup();
    
    // 错误处理
    void SetError(const std::string& strError);
    
    // 创建空白图像
    bool CreateBlankImage(int nWidth, int nHeight, const std::string& strBgColor,
                          std::vector<uint8_t>& vecImageData);
    
    // 绘制要素（简单实现：绘制矩形作为示例）
    bool DrawFeatures(const std::vector<std::shared_ptr<CNFeature>>& vecFeatures,
                      const LayerStyle& style,
                      std::vector<uint8_t>& vecImageData,
                      int nWidth, int nHeight,
                      const BoundingBox& bbox);
    
    // 坐标转换：地理坐标 -> 像素坐标
    bool WorldToPixel(double dWorldX, double dWorldY,
                      const BoundingBox& bbox, int nWidth, int nHeight,
                      int& nPixelX, int& nPixelY);
    
private:
    ServerConfig m_config;
  //  std::unique_ptr<CNDrawFacade> m_pDrawFacade;
    std::unique_ptr<CNRasterImageDevice> m_pRasterDevice;
    std::string m_strError;
    std::string m_strOutputDir = "./leaf/output";
    bool m_bInitialized = false;
    
    // 简单颜色解析缓存
    mutable std::map<std::string, std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> m_colorCache;
};

} // namespace cycle