#ifndef OGC_DRAW_RASTER_IMAGE_DEVICE_H
#define OGC_DRAW_RASTER_IMAGE_DEVICE_H

#include "ogc/draw/draw_device.h"
#include <vector>

namespace ogc {
namespace draw {

enum class ImageFormat {
    kPNG = 0,
    kJPEG = 1,
    kBMP = 2,
    kTIFF = 3,
    kGIF = 4,
    kWebP = 5
};

class OGC_GRAPH_API RasterImageDevice : public DrawDevice {
public:
    RasterImageDevice();
    RasterImageDevice(int width, int height, int channels = 4);
    ~RasterImageDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kRasterImage; }
    std::string GetName() const override { return "RasterImageDevice"; }
    
    DrawResult Initialize() override;
    DrawResult Finalize() override;
    
    DeviceState GetState() const override;
    bool IsReady() const override;
    
    DrawResult BeginDraw(const DrawParams& params) override;
    DrawResult EndDraw() override;
    DrawResult AbortDraw() override;
    
    bool IsDrawing() const override;
    
    void SetTransform(const TransformMatrix& transform) override;
    TransformMatrix GetTransform() const override;
    void ResetTransform() override;
    
    void SetClipRect(double x, double y, double width, double height) override;
    void ClearClipRect() override;
    bool HasClipRect() const override;
    
    DrawResult DrawPoint(double x, double y, const DrawStyle& style) override;
    DrawResult DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) override;
    DrawResult DrawRect(double x, double y, double width, double height, const DrawStyle& style) override;
    DrawResult DrawCircle(double cx, double cy, double radius, const DrawStyle& style) override;
    DrawResult DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style) override;
    DrawResult DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawPolyline(const double* x, const double* y, int count, const DrawStyle& style) override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawTextWithBackground(double x, double y, const std::string& text, const Font& font, const Color& textColor, const Color& bgColor) override;
    
    DrawResult DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels) override;
    DrawResult DrawImageRegion(double destX, double destY, double destWidth, double destHeight, const uint8_t* data, int dataWidth, int dataHeight, int channels, int srcX, int srcY, int srcWidth, int srcHeight) override;
    
    void Clear(const Color& color) override;
    void Fill(const Color& color) override;
    
    int GetWidth() const override;
    int GetHeight() const override;
    double GetDpi() const override;
    
    void SetDpi(double dpi) override;
    
    DrawParams GetDrawParams() const override;
    
    void SetAntialiasing(bool enable) override;
    bool IsAntialiasingEnabled() const override;
    
    void SetOpacity(double opacity) override;
    double GetOpacity() const override;
    
    void Resize(int width, int height, int channels = 4);
    int GetChannels() const;
    
    uint8_t* GetData();
    const uint8_t* GetData() const;
    size_t GetDataSize() const;
    
    bool SaveToFile(const std::string& filepath, ImageFormat format = ImageFormat::kPNG) const;
    bool LoadFromFile(const std::string& filepath);
    
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);
    
    static std::shared_ptr<RasterImageDevice> Create(int width, int height, int channels = 4);

private:
    void TransformPoint(double x, double y, double& outX, double& outY) const;
    bool IsPointVisible(double x, double y) const;
    bool IsRectVisible(double x, double y, double width, double height) const;
    void DrawLineBresenham(int x0, int y0, int x1, int y1, const Color& color);
    void FillScanline(int y, int x0, int x1, const Color& color);
    void BlendPixel(int x, int y, const Color& color);
    
    int m_width;
    int m_height;
    int m_channels;
    double m_dpi;
    DeviceState m_state;
    TransformMatrix m_transform;
    bool m_drawing;
    bool m_antialiasing;
    double m_opacity;
    DrawParams m_params;
    std::vector<uint8_t> m_data;
    double m_clipX, m_clipY, m_clipWidth, m_clipHeight;
    bool m_hasClipRect;
};

}  
}  

#endif  
