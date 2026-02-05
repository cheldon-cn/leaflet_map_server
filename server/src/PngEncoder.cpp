#include "PngEncoder.h"
#include <cstring>
#include <fstream>
#include <stdexcept>

// 检查是否定义了HAVE_LIBPNG，如果未定义则尝试检测
#ifndef HAVE_LIBPNG
// 尝试包含libpng头文件
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4619) // 禁用libpng警告
#endif

#include <png.h>

#ifdef _WIN32
#pragma warning(pop)
#endif

#define HAVE_LIBPNG 1
#endif

namespace cycle {

// libpng错误处理回调
#ifdef HAVE_LIBPNG
static void PngErrorCallback(png_structp pPng, png_const_charp msg) {
    PngEncoder* encoder = static_cast<PngEncoder*>(png_get_error_ptr(pPng));
    if (encoder) {
        encoder->SetError(std::string("libpng error: ") + msg);
    }
    // 长跳转回setjmp点
    longjmp(png_jmpbuf(pPng), 1);
}

static void PngWarningCallback(png_structp pPng, png_const_charp msg) {
    // 忽略警告
    (void)pPng;
    (void)msg;
}
#endif

PngEncoder::PngEncoder() {
#ifdef HAVE_LIBPNG
    if (!Initialize()) {
        Cleanup();
    }
#endif
}

PngEncoder::~PngEncoder() {
    Cleanup();
}

PngEncoder::PngEncoder(PngEncoder&& other) noexcept
    : m_pPngStruct(other.m_pPngStruct)
    , m_pPngInfo(other.m_pPngInfo)
    , m_strError(std::move(other.m_strError))
    , m_nCompressionLevel(other.m_nCompressionLevel)
    , m_bInterlaced(other.m_bInterlaced)
    , m_bInitialized(other.m_bInitialized) {
    other.m_pPngStruct = nullptr;
    other.m_pPngInfo = nullptr;
    other.m_bInitialized = false;
}

PngEncoder& PngEncoder::operator=(PngEncoder&& other) noexcept {
    if (this != &other) {
        Cleanup();
        
        m_pPngStruct = other.m_pPngStruct;
        m_pPngInfo = other.m_pPngInfo;
        m_strError = std::move(other.m_strError);
        m_nCompressionLevel = other.m_nCompressionLevel;
        m_bInterlaced = other.m_bInterlaced;
        m_bInitialized = other.m_bInitialized;
        
        other.m_pPngStruct = nullptr;
        other.m_pPngInfo = nullptr;
        other.m_bInitialized = false;
    }
    return *this;
}

bool PngEncoder::Initialize() {
#ifdef HAVE_LIBPNG
    if (m_bInitialized) {
        return true;
    }
    
    m_pPngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
                                          this, 
                                          PngErrorCallback, 
                                          PngWarningCallback);
    if (!m_pPngStruct) {
        SetError("Failed to create PNG write structure");
        return false;
    }
    
    m_pPngInfo = png_create_info_struct(static_cast<png_structp>(m_pPngStruct));
    if (!m_pPngInfo) {
        SetError("Failed to create PNG info structure");
        return false;
    }
    
    m_bInitialized = true;
    return true;
#else
    SetError("libpng not available");
    return false;
#endif
}

void PngEncoder::Cleanup() {
#ifdef HAVE_LIBPNG
    if (m_pPngStruct) {
        png_structp pstruct = static_cast<png_structp>(m_pPngStruct);
        png_infop pInfo = static_cast<png_infop>(m_pPngInfo);
        png_destroy_write_struct(
            static_cast<png_structpp>(&pstruct),
            static_cast<png_infopp>(&pInfo));
        m_pPngStruct = nullptr;
        m_pPngInfo = nullptr;
    }
#endif
    m_bInitialized = false;
}

void PngEncoder::SetError(const std::string& strError) {
    m_strError = strError;
}

bool PngEncoder::EncodeRGBA(const uint8_t* pData, int nWidth, int nHeight, 
                            std::vector<uint8_t>& vecOutput) {
#ifdef HAVE_LIBPNG
    if (!pData || nWidth <= 0 || nHeight <= 0) {
        SetError("Invalid input parameters");
        return false;
    }
    
    if (!m_bInitialized && !Initialize()) {
        return false;
    }
    
    // 设置错误处理跳转
    if (setjmp(png_jmpbuf(static_cast<png_structp>(m_pPngStruct)))) {
        SetError("libpng error during encoding");
        return false;
    }
    
    // 使用内存写入函数
    png_set_write_fn(static_cast<png_structp>(m_pPngStruct),
                     &vecOutput,
                     [](png_structp pPng, png_bytep pData, png_size_t nLength) {
                         std::vector<uint8_t>* pVec = static_cast<std::vector<uint8_t>*>(png_get_io_ptr(pPng));
                         size_t nOldSize = pVec->size();
                         pVec->resize(nOldSize + nLength);
                         std::memcpy(pVec->data() + nOldSize, pData, nLength);
                     },
                     [](png_structp pPng) {
                         // 刷新函数，不需要实现
                         (void)pPng;
                     });
    
    // 设置PNG头信息
    png_set_IHDR(static_cast<png_structp>(m_pPngStruct),
                 static_cast<png_infop>(m_pPngInfo),
                 nWidth, nHeight,
                 8, // 位深度
                 PNG_COLOR_TYPE_RGBA,
                 m_bInterlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    // 设置压缩级别
    png_set_compression_level(static_cast<png_structp>(m_pPngStruct), m_nCompressionLevel);
    
    // 写入头信息
    png_write_info(static_cast<png_structp>(m_pPngStruct), static_cast<png_infop>(m_pPngInfo));
    
    // 写入行数据
    png_bytep* pRowPointers = new png_bytep[nHeight];
    for (int y = 0; y < nHeight; ++y) {
        pRowPointers[y] = const_cast<png_bytep>(pData + y * nWidth * 4);
    }
    
    png_write_image(static_cast<png_structp>(m_pPngStruct), pRowPointers);
    png_write_end(static_cast<png_structp>(m_pPngStruct), nullptr);
    
    delete[] pRowPointers;
    return true;
#else
    (void)pData;
    (void)nWidth;
    (void)nHeight;
    (void)vecOutput;
    SetError("libpng not available");
    return false;
#endif
}

bool PngEncoder::SaveRGBA(const uint8_t* pData, int nWidth, int nHeight, 
                          const std::string& strFilePath) {
#ifdef HAVE_LIBPNG
    if (!pData || nWidth <= 0 || nHeight <= 0) {
        SetError("Invalid input parameters");
        return false;
    }
    
    if (!m_bInitialized && !Initialize()) {
        return false;
    }
    
    // 设置错误处理跳转
    if (setjmp(png_jmpbuf(static_cast<png_structp>(m_pPngStruct)))) {
        SetError("libpng error during file writing");
        return false;
    }
    
    FILE* pFile = nullptr;
    fopen_s(&pFile,strFilePath.c_str(), "wb");
    if (!pFile) {
        SetError("Failed to open file for writing: " + strFilePath);
        return false;
    }
    
    // 设置文件写入函数
    png_init_io(static_cast<png_structp>(m_pPngStruct), pFile);
    
    // 设置PNG头信息
    png_set_IHDR(static_cast<png_structp>(m_pPngStruct),
                 static_cast<png_infop>(m_pPngInfo),
                 nWidth, nHeight,
                 8, // 位深度
                 PNG_COLOR_TYPE_RGBA,
                 m_bInterlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    // 设置压缩级别
    png_set_compression_level(static_cast<png_structp>(m_pPngStruct), m_nCompressionLevel);
    
    // 写入头信息
    png_write_info(static_cast<png_structp>(m_pPngStruct), static_cast<png_infop>(m_pPngInfo));
    
    // 写入行数据
    png_bytep* pRowPointers = new png_bytep[nHeight];
    for (int y = 0; y < nHeight; ++y) {
        pRowPointers[y] = const_cast<png_bytep>(pData + y * nWidth * 4);
    }
    
    png_write_image(static_cast<png_structp>(m_pPngStruct), pRowPointers);
    png_write_end(static_cast<png_structp>(m_pPngStruct), nullptr);
    
    delete[] pRowPointers;
    fclose(pFile);
    return true;
#else
    (void)pData;
    (void)nWidth;
    (void)nHeight;
    (void)strFilePath;
    SetError("libpng not available");
    return false;
#endif
}

bool PngEncoder::EncodeToMemory(const uint8_t* pData, int nWidth, int nHeight,
                                std::vector<uint8_t>& vecOutput) {
    PngEncoder encoder;
    return encoder.EncodeRGBA(pData, nWidth, nHeight, vecOutput);
}

bool PngEncoder::SaveToFile(const uint8_t* pData, int nWidth, int nHeight,
                            const std::string& strFilePath) {
    PngEncoder encoder;
    return encoder.SaveRGBA(pData, nWidth, nHeight, strFilePath);
}

void PngEncoder::SetCompressionLevel(int nLevel) {
    if (nLevel >= 0 && nLevel <= 9) {
        m_nCompressionLevel = nLevel;
    }
}

void PngEncoder::SetInterlaced(bool bInterlaced) {
    m_bInterlaced = bInterlaced;
}

} // namespace cycle