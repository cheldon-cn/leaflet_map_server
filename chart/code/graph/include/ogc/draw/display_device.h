#ifndef OGC_DRAW_DISPLAY_DEVICE_H
#define OGC_DRAW_DISPLAY_DEVICE_H

#include "ogc/draw/draw_device.h"
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

struct DisplayMetrics {
    int width;
    int height;
    double dpi;
    double scaleFactor;
    bool isPrimary;
    
    DisplayMetrics() : width(0), height(0), dpi(96.0), scaleFactor(1.0), isPrimary(false) {}
};

enum class DisplayMode {
    kWindowed,
    kFullscreen,
    kBorderless,
    kMaximized
};

enum class VSyncMode {
    kDisabled,
    kEnabled,
    kAdaptive
};

using ResizeCallback = std::function<void(int width, int height)>;
using PaintCallback = std::function<void()>;
using CloseCallback = std::function<void()>;

class OGC_GRAPH_API DisplayDevice : public DrawDevice {
public:
    DisplayDevice();
    DisplayDevice(int width, int height, const std::string& title = "");
    ~DisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    std::string GetName() const override { return "DisplayDevice"; }
    
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
    
    void SetTitle(const std::string& title);
    std::string GetTitle() const;
    
    void SetDisplayMode(DisplayMode mode);
    DisplayMode GetDisplayMode() const;
    
    void SetVSync(VSyncMode mode);
    VSyncMode GetVSync() const;
    
    void Resize(int width, int height);
    
    void SetResizeCallback(const ResizeCallback& callback);
    void SetPaintCallback(const PaintCallback& callback);
    void SetCloseCallback(const CloseCallback& callback);
    
    void* GetNativeHandle() const;
    void* GetNativeDisplay() const;
    
    bool ProcessEvents();
    void SwapBuffers();
    
    void MakeCurrent();
    void DoneCurrent();
    
    bool IsVisible() const;
    void SetVisible(bool visible);
    
    void SetBackgroundColor(const Color& color);
    Color GetBackgroundColor() const;
    
    static DisplayMetrics GetPrimaryDisplayMetrics();
    static std::vector<DisplayMetrics> GetAllDisplayMetrics();
    
    static std::shared_ptr<DisplayDevice> Create(int width, int height, const std::string& title = "");

private:
    void TransformPoint(double x, double y, double& outX, double& outY) const;
    bool IsPointVisible(double x, double y) const;
    bool IsRectVisible(double x, double y, double width, double height) const;
    void DrawLineBresenham(int x0, int y0, int x1, int y1, const Color& color);
    void BlendPixel(int x, int y, const Color& color);
    
    int m_width;
    int m_height;
    double m_dpi;
    DeviceState m_state;
    TransformMatrix m_transform;
    bool m_drawing;
    bool m_antialiasing;
    double m_opacity;
    DrawParams m_params;
    
    std::string m_title;
    DisplayMode m_displayMode;
    VSyncMode m_vsyncMode;
    
    std::vector<uint8_t> m_framebuffer;
    int m_channels;
    
    double m_clipX, m_clipY, m_clipWidth, m_clipHeight;
    bool m_hasClipRect;
    
    bool m_visible;
    Color m_backgroundColor;
    
    ResizeCallback m_resizeCallback;
    PaintCallback m_paintCallback;
    CloseCallback m_closeCallback;
    
    void* m_nativeHandle;
    void* m_nativeDisplay;
};

using DisplayDevicePtr = std::shared_ptr<DisplayDevice>;

}
}

#endif
