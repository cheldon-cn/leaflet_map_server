#ifndef OGC_DRAW_DRAW_DEVICE_H
#define OGC_DRAW_DRAW_DEVICE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/device_type.h"
#include "ogc/draw/device_state.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/types.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>

namespace ogc {
namespace draw {

class OGC_GRAPH_API DrawDevice {
public:
    virtual ~DrawDevice() = default;
    
    virtual DeviceType GetType() const = 0;
    virtual std::string GetName() const = 0;
    
    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    
    virtual DeviceState GetState() const = 0;
    virtual bool IsReady() const = 0;
    
    virtual DrawResult BeginDraw(const DrawParams& params) = 0;
    virtual DrawResult EndDraw() = 0;
    virtual DrawResult AbortDraw() = 0;
    
    virtual bool IsDrawing() const = 0;
    
    virtual void SetTransform(const TransformMatrix& transform) = 0;
    virtual TransformMatrix GetTransform() const = 0;
    virtual void ResetTransform() = 0;
    
    virtual void SetClipRect(double x, double y, double width, double height) = 0;
    virtual void ClearClipRect() = 0;
    virtual bool HasClipRect() const = 0;
    
    virtual DrawResult DrawPoint(double x, double y, const DrawStyle& style) = 0;
    virtual DrawResult DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) = 0;
    virtual DrawResult DrawRect(double x, double y, double width, double height, const DrawStyle& style) = 0;
    virtual DrawResult DrawCircle(double cx, double cy, double radius, const DrawStyle& style) = 0;
    virtual DrawResult DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style) = 0;
    virtual DrawResult DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) = 0;
    virtual DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawPolyline(const double* x, const double* y, int count, const DrawStyle& style) = 0;
    
    virtual DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) = 0;
    
    virtual DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) = 0;
    virtual DrawResult DrawTextWithBackground(double x, double y, const std::string& text, const Font& font, const Color& textColor, const Color& bgColor) = 0;
    
    virtual DrawResult DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels) = 0;
    virtual DrawResult DrawImageRegion(double destX, double destY, double destWidth, double destHeight, const uint8_t* data, int dataWidth, int dataHeight, int channels, int srcX, int srcY, int srcWidth, int srcHeight) = 0;
    
    virtual void Clear(const Color& color) = 0;
    virtual void Fill(const Color& color) = 0;
    
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual double GetDpi() const = 0;
    
    virtual void SetDpi(double dpi) = 0;
    
    virtual DrawParams GetDrawParams() const = 0;
    
    virtual bool SupportsTransparency() const { return true; }
    virtual bool SupportsAntialiasing() const { return true; }
    virtual bool SupportsTextRotation() const { return true; }
    
    virtual void SetAntialiasing(bool enable) = 0;
    virtual bool IsAntialiasingEnabled() const = 0;
    
    virtual void SetOpacity(double opacity) = 0;
    virtual double GetOpacity() const = 0;
    
    static DrawDevicePtr Create(DeviceType type);
};

}  
}  

#endif  
