#ifndef OGC_DRAW_DRAW_CONTEXT_H
#define OGC_DRAW_DRAW_CONTEXT_H

#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_driver.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/draw/types.h"
#include "ogc/geometry.h"
#include "ogc/envelope.h"
#include <memory>
#include <stack>
#include <string>

namespace ogc {
namespace draw {

class DrawContext {
public:
    DrawContext();
    explicit DrawContext(DrawDevicePtr device);
    DrawContext(DrawDevicePtr device, DrawEnginePtr engine);
    ~DrawContext();
    
    DrawResult Initialize();
    DrawResult Finalize();
    bool IsInitialized() const;
    
    void SetDevice(DrawDevicePtr device);
    DrawDevicePtr GetDevice() const;
    bool HasDevice() const;
    
    void SetEngine(DrawEnginePtr engine);
    DrawEnginePtr GetEngine() const;
    bool HasEngine() const;
    
    void SetDriver(DrawDriverPtr driver);
    DrawDriverPtr GetDriver() const;
    bool HasDriver() const;
    
    DrawResult BeginDraw(const DrawParams& params);
    DrawResult EndDraw();
    DrawResult AbortDraw();
    
    bool IsDrawing() const;
    
    void PushTransform();
    void PopTransform();
    void SetTransform(const TransformMatrix& transform);
    TransformMatrix GetTransform() const;
    void MultiplyTransform(const TransformMatrix& transform);
    void ResetTransform();
    
    void Translate(double tx, double ty);
    void Scale(double sx, double sy);
    void Rotate(double angle);
    
    void PushClipRect(double x, double y, double width, double height);
    void PopClipRect();
    void ClearClipRect();
    
    void PushStyle(const DrawStyle& style);
    void PopStyle();
    void SetStyle(const DrawStyle& style);
    DrawStyle GetStyle() const;
    
    void PushOpacity(double opacity);
    void PopOpacity();
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    DrawResult DrawGeometry(const Geometry* geometry);
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style);
    
    DrawResult DrawPoint(double x, double y);
    DrawResult DrawLine(double x1, double y1, double x2, double y2);
    DrawResult DrawRect(double x, double y, double width, double height);
    DrawResult DrawCircle(double cx, double cy, double radius);
    DrawResult DrawPolygon(const double* x, const double* y, int count);
    DrawResult DrawPolyline(const double* x, const double* y, int count);
    
    DrawResult DrawText(double x, double y, const std::string& text);
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color);
    
    DrawResult DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels);
    
    void Clear(const Color& color);
    void Flush();
    
    void WorldToScreen(double worldX, double worldY, double& screenX, double& screenY) const;
    void ScreenToWorld(double screenX, double screenY, double& worldX, double& worldY) const;
    
    Envelope GetVisibleExtent() const;
    double GetScale() const;
    double GetDpi() const;
    
    const DrawParams& GetDrawParams() const;
    
    void SetBackground(const Color& color);
    Color GetBackground() const;
    
    void SetDefaultStyle(const DrawStyle& style);
    DrawStyle GetDefaultStyle() const;
    
    void SetDefaultFont(const Font& font);
    Font GetDefaultFont() const;
    
    void SetDefaultColor(const Color& color);
    Color GetDefaultColor() const;
    
    static DrawContextPtr Create(DrawDevicePtr device);
    static DrawContextPtr Create(DrawDevicePtr device, DrawEnginePtr engine);

private:
    DrawDevicePtr m_device;
    DrawEnginePtr m_engine;
    DrawDriverPtr m_driver;
    DrawParams m_params;
    DrawStyle m_defaultStyle;
    Font m_defaultFont;
    Color m_defaultColor;
    Color m_background;
    bool m_initialized;
    bool m_drawing;
    
    std::stack<TransformMatrix> m_transformStack;
    std::stack<DrawStyle> m_styleStack;
    std::stack<double> m_opacityStack;
    int m_clipRectDepth;
};

}  
}  

#endif  
