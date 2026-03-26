#ifndef OGC_DRAW_DRAW_ENGINE_H
#define OGC_DRAW_DRAW_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/engine_type.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/draw/types.h"
#include "ogc/geometry.h"
#include "ogc/envelope.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace draw {

class OGC_GRAPH_API DrawEngine {
public:
    virtual ~DrawEngine() = default;
    
    virtual EngineType GetType() const = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    
    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    
    virtual bool IsInitialized() const = 0;
    
    virtual void SetDevice(DrawDevicePtr device) = 0;
    virtual DrawDevicePtr GetDevice() const = 0;
    virtual bool HasDevice() const = 0;
    
    virtual DrawResult BeginDraw(const DrawParams& params) = 0;
    virtual DrawResult EndDraw() = 0;
    virtual DrawResult AbortDraw() = 0;
    
    virtual bool IsDrawing() const = 0;
    
    virtual void SetTransform(const TransformMatrix& transform) = 0;
    virtual TransformMatrix GetTransform() const = 0;
    virtual void ResetTransform() = 0;
    virtual void MultiplyTransform(const TransformMatrix& transform) = 0;
    
    virtual void SetWorldTransform(double offsetX, double offsetY, double scale) = 0;
    virtual void WorldToScreen(double worldX, double worldY, double& screenX, double& screenY) const = 0;
    virtual void ScreenToWorld(double screenX, double screenY, double& worldX, double& worldY) const = 0;
    
    virtual void SetClipEnvelope(const Envelope& envelope) = 0;
    virtual Envelope GetClipEnvelope() const = 0;
    virtual void ClearClipEnvelope() = 0;
    virtual bool IsClipped() const = 0;
    
    virtual DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) = 0;
    virtual DrawResult DrawGeometries(const std::vector<Geometry*>& geometries, const DrawStyle& style) = 0;
    
    virtual DrawResult DrawPoint(double x, double y, const DrawStyle& style) = 0;
    virtual DrawResult DrawLine(double x1, double y1, double x2, double y2, const DrawStyle& style) = 0;
    virtual DrawResult DrawRect(double x, double y, double width, double height, const DrawStyle& style) = 0;
    virtual DrawResult DrawCircle(double cx, double cy, double radius, const DrawStyle& style) = 0;
    virtual DrawResult DrawPolygon(const std::vector<double>& x, const std::vector<double>& y, const DrawStyle& style) = 0;
    virtual DrawResult DrawPolyline(const std::vector<double>& x, const std::vector<double>& y, const DrawStyle& style) = 0;
    
    virtual DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) = 0;
    virtual DrawResult DrawTextAlongLine(const std::vector<double>& x, const std::vector<double>& y, const std::string& text, const Font& font, const Color& color) = 0;
    
    virtual DrawResult DrawImage(double x, double y, double width, double height, const uint8_t* data, int dataWidth, int dataHeight, int channels) = 0;
    
    virtual void Clear(const Color& color) = 0;
    virtual void Flush() = 0;
    
    virtual Envelope GetVisibleExtent() const = 0;
    virtual double GetScale() const = 0;
    virtual double GetDpi() const = 0;
    
    virtual void SetBackground(const Color& color) = 0;
    virtual Color GetBackground() const = 0;
    
    virtual void SetTolerance(double tolerance) = 0;
    virtual double GetTolerance() const = 0;
    
    virtual bool SupportsFeature(const std::string& featureName) const = 0;
    
    static DrawEnginePtr Create(EngineType type);
};

}  
}  

#endif  
