#ifndef OGC_DRAW_IMAGE_H
#define OGC_DRAW_IMAGE_H

#include "ogc/draw/export.h"
#include "ogc/draw/color.h"
#include <vector>
#include <cstdint>
#include <string>
#include <memory>

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
    ~Image();
    
    Image(const Image& other);
    Image& operator=(const Image& other);
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;
    
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
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  
}  

#endif  
