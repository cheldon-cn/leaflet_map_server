#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_engine.h"
#include <cstring>
#include <algorithm>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

namespace ogc {
namespace draw {

RasterImageDevice::RasterImageDevice(int width, int height, PixelFormat format)
    : m_width(width)
    , m_height(height)
    , m_format(format)
    , m_dpi(96.0)
    , m_bytesPerPixel(0)
    , m_stride(0)
    , m_pixelData(nullptr)
    , m_state(DeviceState::kUninitialized)
    , m_preferredEngineType(EngineType::kSimple2D) {
    m_bytesPerPixel = GetBytesPerPixel(format);
    m_stride = m_width * m_bytesPerPixel;
    AllocateBuffer();
    m_state = DeviceState::kReady;
}

RasterImageDevice::~RasterImageDevice() {
    m_pixelData.reset();
    m_state = DeviceState::kUninitialized;
}

int RasterImageDevice::GetBytesPerPixel(PixelFormat format) const {
    switch (format) {
        case PixelFormat::kGray8:
            return 1;
        case PixelFormat::kRGB888:
            return 3;
        case PixelFormat::kRGBA8888:
        case PixelFormat::kBGRA8888:
            return 4;
        default:
            return 0;
    }
}

void RasterImageDevice::AllocateBuffer() {
    if (m_width <= 0 || m_height <= 0 || m_bytesPerPixel <= 0) {
        return;
    }

    size_t dataSize = static_cast<size_t>(m_stride) * m_height;
    m_pixelData.reset(new uint8_t[dataSize]);
    std::memset(m_pixelData.get(), 0, dataSize);
}

DrawResult RasterImageDevice::Initialize() {
    if (m_pixelData) {
        m_state = DeviceState::kReady;
        return DrawResult::kSuccess;
    }
    
    if (m_width > 0 && m_height > 0) {
        AllocateBuffer();
        if (m_pixelData) {
            m_state = DeviceState::kReady;
            return DrawResult::kSuccess;
        }
    }
    
    m_state = DeviceState::kError;
    return DrawResult::kDeviceError;
}

DrawResult RasterImageDevice::Finalize() {
    m_pixelData.reset();
    m_state = DeviceState::kUninitialized;
    return DrawResult::kSuccess;
}

std::unique_ptr<DrawEngine> RasterImageDevice::CreateEngine() {
    return nullptr;
}

std::vector<EngineType> RasterImageDevice::GetSupportedEngineTypes() const {
    return { EngineType::kSimple2D };
}

DeviceCapabilities RasterImageDevice::QueryCapabilities() const {
    DeviceCapabilities caps;
    caps.supportsGPU = false;
    caps.supportsMultithreading = true;
    caps.supportsVSync = false;
    caps.supportsDoubleBuffer = false;
    caps.supportsPartialUpdate = true;
    caps.supportsAlpha = (m_format == PixelFormat::kRGBA8888 || m_format == PixelFormat::kBGRA8888);
    caps.supportsAntialiasing = false;
    caps.maxTextureSize = 8192;
    caps.maxRenderTargets = 1;
    caps.supportedFormats = { "RGBA8888", "RGB888", "Gray8", "BGRA8888" };
    return caps;
}

bool RasterImageDevice::IsFeatureAvailable(const std::string& featureName) const {
    DeviceCapabilities caps = QueryCapabilities();
    if (featureName == "Alpha") return caps.supportsAlpha;
    if (featureName == "PartialUpdate") return caps.supportsPartialUpdate;
    if (featureName == "Multithreading") return caps.supportsMultithreading;
    return false;
}

DrawResult RasterImageDevice::RecoverDevice() {
    if (m_state == DeviceState::kLost) {
        AllocateBuffer();
        if (m_pixelData) {
            m_state = DeviceState::kReady;
            return DrawResult::kSuccess;
        }
        return DrawResult::kDeviceError;
    }
    return DrawResult::kSuccess;
}

Color RasterImageDevice::GetPixel(int x, int y) const {
    if (!m_pixelData || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return Color();
    }

    const uint8_t* pixel = m_pixelData.get() + static_cast<size_t>(y) * m_stride + 
                           static_cast<size_t>(x) * m_bytesPerPixel;

    switch (m_format) {
        case PixelFormat::kGray8:
            return Color(pixel[0], pixel[0], pixel[0], 255);
        case PixelFormat::kRGB888:
            return Color(pixel[0], pixel[1], pixel[2], 255);
        case PixelFormat::kRGBA8888:
            return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
        case PixelFormat::kBGRA8888:
            return Color(pixel[2], pixel[1], pixel[0], pixel[3]);
        default:
            return Color();
    }
}

void RasterImageDevice::SetPixel(int x, int y, const Color& color) {
    if (!m_pixelData || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }

    uint8_t* pixel = m_pixelData.get() + static_cast<size_t>(y) * m_stride + 
                     static_cast<size_t>(x) * m_bytesPerPixel;

    switch (m_format) {
        case PixelFormat::kGray8: {
            uint8_t gray = static_cast<uint8_t>((color.r * 299 + color.g * 587 + color.b * 114) / 1000);
            pixel[0] = gray;
            break;
        }
        case PixelFormat::kRGB888:
            pixel[0] = color.r;
            pixel[1] = color.g;
            pixel[2] = color.b;
            break;
        case PixelFormat::kRGBA8888:
            pixel[0] = color.r;
            pixel[1] = color.g;
            pixel[2] = color.b;
            pixel[3] = color.a;
            break;
        case PixelFormat::kBGRA8888:
            pixel[0] = color.b;
            pixel[1] = color.g;
            pixel[2] = color.r;
            pixel[3] = color.a;
            break;
        default:
            break;
    }
}

void RasterImageDevice::Clear(const Color& color) {
    if (!m_pixelData) {
        return;
    }

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            SetPixel(x, y, color);
        }
    }
}

bool RasterImageDevice::SaveToFile(const std::string& path, ImageFormat format) {
    if (!m_pixelData) {
        return false;
    }

    ImageFormat actualFormat = format;
    if (format == ImageFormat::kAuto) {
        if (path.find(".png") != std::string::npos || 
            path.find(".PNG") != std::string::npos) {
            actualFormat = ImageFormat::kPNG;
        } else if (path.find(".jpg") != std::string::npos || 
                   path.find(".jpeg") != std::string::npos ||
                   path.find(".JPG") != std::string::npos ||
                   path.find(".JPEG") != std::string::npos) {
            actualFormat = ImageFormat::kJPEG;
        } else if (path.find(".bmp") != std::string::npos ||
                   path.find(".BMP") != std::string::npos) {
            actualFormat = ImageFormat::kBMP;
        } else {
            actualFormat = ImageFormat::kPNG;
        }
    }

    std::vector<uint8_t> tempBuffer;
    const uint8_t* dataToWrite = m_pixelData.get();
    int channels = m_bytesPerPixel;

    if (m_format == PixelFormat::kBGRA8888) {
        channels = 4;
        size_t dataSize = static_cast<size_t>(m_stride) * m_height;
        tempBuffer.resize(dataSize);
        for (size_t i = 0; i < dataSize; i += 4) {
            tempBuffer[i + 0] = m_pixelData[i + 2];
            tempBuffer[i + 1] = m_pixelData[i + 1];
            tempBuffer[i + 2] = m_pixelData[i + 0];
            tempBuffer[i + 3] = m_pixelData[i + 3];
        }
        dataToWrite = tempBuffer.data();
    }

    int stride = m_stride;
    int result = 0;

    switch (actualFormat) {
        case ImageFormat::kPNG:
            result = stbi_write_png(path.c_str(), m_width, m_height, channels, 
                                    dataToWrite, stride);
            break;
        case ImageFormat::kJPEG:
            result = stbi_write_jpg(path.c_str(), m_width, m_height, channels, 
                                    dataToWrite, 90);
            break;
        case ImageFormat::kBMP:
            result = stbi_write_bmp(path.c_str(), m_width, m_height, channels, 
                                    dataToWrite);
            break;
        default:
            return false;
    }

    return result != 0;
}

}  
}  
