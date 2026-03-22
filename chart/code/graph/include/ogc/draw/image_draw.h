#ifndef OGC_DRAW_IMAGE_DRAW_H
#define OGC_DRAW_IMAGE_DRAW_H

#include "ogc/draw/draw_result.h"
#include "ogc/draw/color.h"
#include "ogc/envelope.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace draw {

enum class ImageFormat {
    kUnknown,
    kBMP,
    kJPEG,
    kPNG,
    kTIFF,
    kGIF,
    kWEBP,
    kRAW
};

struct ImageInfo {
    int width;
    int height;
    int channels;
    int bitsPerChannel;
    ImageFormat format;
    double dpiX;
    double dpiY;
    bool hasAlpha;
};

class ImageData;
using ImageDataPtr = std::unique_ptr<ImageData>;

class ImageData {
public:
    ImageData();
    ImageData(int width, int height, int channels);
    ImageData(int width, int height, int channels, const unsigned char* data);
    ~ImageData();
    
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannels() const { return m_channels; }
    int GetSize() const { return m_width * m_height * m_channels; }
    
    unsigned char* GetData() { return m_data.get(); }
    const unsigned char* GetData() const { return m_data.get(); }
    
    bool IsValid() const { return m_data != nullptr && m_width > 0 && m_height > 0; }
    bool IsEmpty() const { return !IsValid(); }
    
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);
    
    void Fill(const Color& color);
    void Clear();
    
    ImageDataPtr Clone() const;
    
    bool Save(const std::string& path, ImageFormat format) const;
    static ImageDataPtr Load(const std::string& path);
    
    ImageDataPtr Resize(int newWidth, int newHeight) const;
    ImageDataPtr Crop(int x, int y, int width, int height) const;
    ImageDataPtr FlipHorizontal() const;
    ImageDataPtr FlipVertical() const;
    ImageDataPtr Rotate90() const;
    ImageDataPtr Rotate180() const;
    ImageDataPtr Rotate270() const;
    
private:
    int m_width;
    int m_height;
    int m_channels;
    std::unique_ptr<unsigned char[]> m_data;
};

class IImageReader {
public:
    virtual ~IImageReader() = default;
    
    virtual bool CanRead(const std::string& path) const = 0;
    virtual bool CanRead(ImageFormat format) const = 0;
    
    virtual ImageDataPtr Read(const std::string& path) = 0;
    virtual ImageInfo GetImageInfo(const std::string& path) = 0;
    
    virtual std::vector<ImageFormat> GetSupportedFormats() const = 0;
};

class IImageWriter {
public:
    virtual ~IImageWriter() = default;
    
    virtual bool CanWrite(ImageFormat format) const = 0;
    
    virtual bool Write(const std::string& path, const ImageData* data, ImageFormat format) = 0;
    virtual bool Write(const std::string& path, const ImageData* data, ImageFormat format, int quality) = 0;
    
    virtual std::vector<ImageFormat> GetSupportedFormats() const = 0;
};

class ImageDraw {
public:
    ImageDraw();
    explicit ImageDraw(ImageData* imageData);
    ~ImageDraw();
    
    void SetImageData(ImageData* imageData);
    ImageData* GetImageData() const { return m_imageData; }
    
    void SetPixel(int x, int y, const Color& color);
    Color GetPixel(int x, int y) const;
    
    void DrawLine(int x1, int y1, int x2, int y2, const Color& color, int thickness = 1);
    void DrawLineAA(int x1, int y1, int x2, int y2, const Color& color, int thickness = 1);
    
    void DrawRect(int x, int y, int width, int height, const Color& color, int thickness = 1);
    void FillRect(int x, int y, int width, int height, const Color& color);
    
    void DrawCircle(int cx, int cy, int radius, const Color& color, int thickness = 1);
    void FillCircle(int cx, int cy, int radius, const Color& color);
    
    void DrawEllipse(int cx, int cy, int rx, int ry, const Color& color, int thickness = 1);
    void FillEllipse(int cx, int cy, int rx, int ry, const Color& color);
    
    void DrawPolygon(const std::vector<std::pair<int, int>>& points, const Color& color, int thickness = 1);
    void FillPolygon(const std::vector<std::pair<int, int>>& points, const Color& color);
    
    void DrawText(int x, int y, const std::string& text, const Color& color, const std::string& fontFamily = "", int fontSize = 12);
    
    void DrawImage(int x, int y, const ImageData* image, double opacity = 1.0);
    void DrawImage(int x, int y, const ImageData* image, int srcX, int srcY, int srcWidth, int srcHeight, double opacity = 1.0);
    
    void Blend(int x, int y, const Color& color, double alpha);
    
    void Clear(const Color& color);
    
    int GetWidth() const;
    int GetHeight() const;
    
    bool IsValid() const { return m_imageData != nullptr; }
    
    void SetClipRect(int x, int y, int width, int height);
    void ClearClipRect();
    bool HasClipRect() const { return m_hasClipRect; }
    
    bool IsPointInClipRect(int x, int y) const;
    
private:
    ImageData* m_imageData;
    int m_clipX;
    int m_clipY;
    int m_clipWidth;
    int m_clipHeight;
    bool m_hasClipRect;
    
    void DrawLineBresenham(int x1, int y1, int x2, int y2, const Color& color, int thickness);
    void DrawLineXiaolinWu(int x1, int y1, int x2, int y2, const Color& color, int thickness);
    
    void DrawCircleMidpoint(int cx, int cy, int radius, const Color& color, int thickness);
    void FillCircleScanline(int cx, int cy, int radius, const Color& color);
    
    void DrawEllipseMidpoint(int cx, int cy, int rx, int ry, const Color& color, int thickness);
    void FillEllipseScanline(int cx, int cy, int rx, int ry, const Color& color);
    
    void FillPolygonScanline(const std::vector<std::pair<int, int>>& points, const Color& color);
    
    unsigned char BlendChannel(unsigned char dst, unsigned char src, double alpha);
};

ImageFormat GetImageFormatFromExtension(const std::string& path);
std::string GetExtensionFromImageFormat(ImageFormat format);

}
}

#endif
