#ifndef OGC_DRAW_TILE_DEVICE_H
#define OGC_DRAW_TILE_DEVICE_H

#include "ogc/draw/draw_device.h"
#include "ogc/draw/tile_key.h"
#include <vector>
#include <map>

namespace ogc {
namespace draw {

struct TileData {
    int x;
    int y;
    int z;
    int width;
    int height;
    std::vector<uint8_t> data;
    bool valid;
    
    TileData() : x(0), y(0), z(0), width(256), height(256), valid(false) {}
    TileData(int x_, int y_, int z_, int w = 256, int h = 256)
        : x(x_), y(y_), z(z_), width(w), height(h), valid(false) {}
};

class OGC_GRAPH_API TileDevice : public DrawDevice {
public:
    TileDevice();
    TileDevice(int tileSize);
    ~TileDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kTile; }
    std::string GetName() const override { return "TileDevice"; }
    
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
    
    void SetTileSize(int size);
    int GetTileSize() const;
    
    void SetCurrentTile(int x, int y, int z);
    TileKey GetCurrentTileKey() const;
    
    DrawResult BeginTile(int x, int y, int z);
    DrawResult EndTile();
    
    TileData GetCurrentTileData() const;
    std::vector<TileData> GetAllTileData() const;
    
    bool SaveTile(const std::string& directory, const std::string& format = "png");
    bool LoadTile(const std::string& filepath, int x, int y, int z);
    
    void SetTileFormat(const std::string& format);
    std::string GetTileFormat() const;
    
    static std::shared_ptr<TileDevice> Create(int tileSize = 256);

private:
    void TransformPoint(double x, double y, double& outX, double& outY) const;
    bool IsPointInTile(double x, double y) const;
    bool IsRectInTile(double x, double y, double width, double height) const;
    void DrawLineBresenham(int x0, int y0, int x1, int y1, const Color& color);
    void FillScanline(int y, int x0, int x1, const Color& color);
    void BlendPixel(int x, int y, const Color& color);
    std::string GetTileFilePath(const std::string& directory, int x, int y, int z) const;
    
    int m_tileSize;
    double m_dpi;
    DeviceState m_state;
    TransformMatrix m_transform;
    bool m_drawing;
    bool m_antialiasing;
    double m_opacity;
    DrawParams m_params;
    
    int m_currentX;
    int m_currentY;
    int m_currentZ;
    
    std::vector<uint8_t> m_tileData;
    int m_channels;
    
    double m_clipX, m_clipY, m_clipWidth, m_clipHeight;
    bool m_hasClipRect;
    
    std::string m_tileFormat;
    
    std::map<std::tuple<int, int, int>, TileData> m_tiles;
};

using TileDevicePtr = std::shared_ptr<TileDevice>;

}
}

#endif
