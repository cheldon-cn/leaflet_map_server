#include "ogc/draw/image.h"
#include <cstring>

namespace ogc {
namespace draw {

Image::Image()
    : m_width(0)
    , m_height(0)
    , m_channels(0) {
}

Image::Image(int width, int height, int channels)
    : m_width(width)
    , m_height(height)
    , m_channels(channels) {
    m_data.resize(static_cast<size_t>(width) * height * channels);
}

Image::Image(int width, int height, int channels, const uint8_t* data)
    : m_width(width)
    , m_height(height)
    , m_channels(channels) {
    size_t size = static_cast<size_t>(width) * height * channels;
    m_data.resize(size);
    if (data) {
        std::memcpy(m_data.data(), data, size);
    }
}

ImageFormat Image::GetFormat() const {
    switch (m_channels) {
        case 1: return ImageFormat::kGrayscale;
        case 2: return ImageFormat::kGrayscaleAlpha;
        case 3: return ImageFormat::kRGB;
        case 4: return ImageFormat::kRGBA;
        default: return ImageFormat::kUnknown;
    }
}

void Image::Resize(int width, int height, int channels) {
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_data.resize(static_cast<size_t>(width) * height * channels);
}

void Image::Clear() {
    m_width = 0;
    m_height = 0;
    m_channels = 0;
    m_data.clear();
}

bool Image::LoadFromFile(const std::string& path) {
    (void)path;
    return false;
}

bool Image::SaveToFile(const std::string& path) const {
    (void)path;
    return false;
}

Image Image::Clone() const {
    return Image(m_width, m_height, m_channels, m_data.empty() ? nullptr : m_data.data());
}

Color Image::GetPixel(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || m_data.empty()) {
        return Color::Transparent();
    }
    
    size_t index = static_cast<size_t>((y * m_width + x) * m_channels);
    
    if (m_channels >= 3) {
        uint8_t r = m_data[index];
        uint8_t g = m_data[index + 1];
        uint8_t b = m_data[index + 2];
        uint8_t a = m_channels >= 4 ? m_data[index + 3] : 255;
        return Color(r, g, b, a);
    } else if (m_channels == 1) {
        uint8_t v = m_data[index];
        return Color(v, v, v, 255);
    }
    
    return Color::Transparent();
}

void Image::SetPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || m_data.empty()) {
        return;
    }
    
    size_t index = static_cast<size_t>((y * m_width + x) * m_channels);
    
    if (m_channels >= 3) {
        m_data[index] = color.GetRed();
        m_data[index + 1] = color.GetGreen();
        m_data[index + 2] = color.GetBlue();
        if (m_channels >= 4) {
            m_data[index + 3] = color.GetAlpha();
        }
    } else if (m_channels == 1) {
        m_data[index] = color.GetRed();
    }
}

Image Image::Scaled(int newWidth, int newHeight) const {
    (void)newWidth;
    (void)newHeight;
    return Clone();
}

Image Image::Cropped(int x, int y, int width, int height) const {
    if (x < 0 || y < 0 || width <= 0 || height <= 0) {
        return Image();
    }
    if (x + width > m_width || y + height > m_height) {
        return Image();
    }
    
    Image result(width, height, m_channels);
    for (int row = 0; row < height; ++row) {
        size_t srcIndex = static_cast<size_t>(((y + row) * m_width + x) * m_channels);
        size_t dstIndex = static_cast<size_t>(row * width * m_channels);
        std::memcpy(result.m_data.data() + dstIndex, m_data.data() + srcIndex, 
                    static_cast<size_t>(width) * m_channels);
    }
    return result;
}

Image Image::FromData(int width, int height, int channels, const uint8_t* data) {
    return Image(width, height, channels, data);
}

Image Image::Solid(int width, int height, const Color& color) {
    Image img(width, height, 4);
    for (int i = 0; i < width * height; ++i) {
        img.m_data[i * 4] = color.GetRed();
        img.m_data[i * 4 + 1] = color.GetGreen();
        img.m_data[i * 4 + 2] = color.GetBlue();
        img.m_data[i * 4 + 3] = color.GetAlpha();
    }
    return img;
}

}  
}  
