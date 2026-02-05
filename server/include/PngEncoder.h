#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace cycle {

class PngEncoder {
public:
    PngEncoder();
    ~PngEncoder();
    
    // 禁止拷贝
    PngEncoder(const PngEncoder&) = delete;
    PngEncoder& operator=(const PngEncoder&) = delete;
    
    // 允许移动
    PngEncoder(PngEncoder&& other) noexcept;
    PngEncoder& operator=(PngEncoder&& other) noexcept;
    
    // 编码RGBA数据为PNG
    bool EncodeRGBA(const uint8_t* pData, int nWidth, int nHeight, 
                    std::vector<uint8_t>& vecOutput);
    
    // 编码RGBA数据并保存到文件
    bool SaveRGBA(const uint8_t* pData, int nWidth, int nHeight, 
                  const std::string& strFilePath);
    
    // 从RGBA数据生成PNG字节流
    static bool EncodeToMemory(const uint8_t* pData, int nWidth, int nHeight,
                               std::vector<uint8_t>& vecOutput);
    
    // 从RGBA数据保存PNG文件
    static bool SaveToFile(const uint8_t* pData, int nWidth, int nHeight,
                           const std::string& strFilePath);
    
    // 设置压缩级别 (0-9, 0=无压缩, 9=最大压缩)
    void SetCompressionLevel(int nLevel);
    
    // 设置是否使用隔行扫描
    void SetInterlaced(bool bInterlaced);
    
    // 获取错误信息
    const std::string& GetError() const { return m_strError; }

    // 错误处理
    void SetError(const std::string& strError);
private:
    // 初始化libpng资源
    bool Initialize();
    
    // 清理资源
    void Cleanup();
    
    
private:
    // libpng结构体指针（避免暴露libpng头文件）
    void* m_pPngStruct = nullptr;
    void* m_pPngInfo = nullptr;
    std::string m_strError;
    int m_nCompressionLevel = 6; // 默认压缩级别
    bool m_bInterlaced = false;
    bool m_bInitialized = false;
};

} // namespace cycle