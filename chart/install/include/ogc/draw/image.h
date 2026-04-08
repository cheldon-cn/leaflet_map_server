#ifndef OGC_DRAW_IMAGE_H
#define OGC_DRAW_IMAGE_H

#include "ogc/draw/export.h"
#include "ogc/draw/color.h"
#include <vector>
#include <cstdint>
#include <string>

namespace ogc {
namespace draw {

enum class ImageFormat {
    kUnknown = 0,
    kRGB = 1,
    kRGBA = 2,
    kGrayscale = 3,
    kGrayscaleAlpha = 4,
    kBGR = 5,
    kBGRA = 6
};

class OGC_DRAW_API Image {
public:
    Image();
    Image(int width, int height, int channels);
    Image(int width, int height, int channels, const uint8_t* data);
    
    int GetWidth() const;
    int GetHeight() const;
    int GetChannels() const;
    ImageFormat GetFormat() const;
    
    uint8_t* GetData();
    const uint8_t* GetData() const;
    size_t GetDataSize() const;
    
    bool IsValid() const;
    bool IsEmpty() const;
    
    void Resize(int width, int height, int channels);
    void Clear();
    
    bool LoadFromFile(const std::string& path);
    bool SaveToFile(const std::string& path) const;
    
    Image Clone() const;
    
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);
    
    Image Scaled(int newWidth, int newHeight) const;
    Image Cropped(int x, int y, int width, int height) const;
    
    static Image FromData(int width, int height, int channels, const uint8_t* data);
    static Image Solid(int width, int height, const Color& color);

private:
    int m_width;
    int m_height;
    int m_channels;
    std::vector<uint8_t> m_data;
};

inline int Image::GetWidth() const { return m_width; }
inline int Image::GetHeight() const { return m_height; }
inline int Image::GetChannels() const { return m_channels; }
inline uint8_t* Image::GetData() { return m_data.empty() ? nullptr : m_data.data(); }
inline const uint8_t* Image::GetData() const { return m_data.empty() ? nullptr : m_data.data(); }
inline size_t Image::GetDataSize() const { return m_data.size(); }
inline bool Image::IsValid() const { return m_width > 0 && m_height > 0 && m_channels > 0 && !m_data.empty(); }
inline bool Image::IsEmpty() const { return m_data.empty(); }

}  
}  

#endif  
