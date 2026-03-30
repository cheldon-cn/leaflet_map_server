#ifndef OGC_DRAW_DIRECT2D_ENGINE_H
#define OGC_DRAW_DIRECT2D_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/gpu_accelerated_engine.h"

#ifdef _WIN32

#include <windows.h>

#ifdef DrawText
#undef DrawText
#endif
#ifdef DrawTextA
#undef DrawTextA
#endif
#ifdef DrawTextW
#undef DrawTextW
#endif

#include <d2d1.h>
#include <dwrite.h>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <wrl/client.h>

namespace ogc {
namespace draw {

using Microsoft::WRL::ComPtr;

class OGC_DRAW_API Direct2DEngine : public GPUAcceleratedEngine {
public:
    explicit Direct2DEngine(DrawDevice* device);
    ~Direct2DEngine() override;
    
    std::string GetName() const override { return "Direct2DEngine"; }
    EngineType GetType() const override { return EngineType::kGPU; }
    
    DrawResult Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawLineString(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawRect(double x, double y, double w, double h, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawCircle(double cx, double cy, double radius, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) override;
    void Restore() override;
    
    void Clear(const Color& color) override;
    void Flush() override;
    
    ID2D1RenderTarget* GetRenderTarget() const { return m_renderTarget.Get(); }
    ID2D1Factory* GetFactory() const { return m_factory.Get(); }
    
    static bool IsDirect2DAvailable();
    static HRESULT InitializeDirect2D();
    static void ShutdownDirect2D();

protected:
    bool CreateDeviceResources();
    void DiscardDeviceResources();
    
    ComPtr<ID2D1Brush> CreateBrush(const Color& color);
    ComPtr<ID2D1StrokeStyle> CreateStrokeStyle(const Pen& pen);
    ComPtr<IDWriteTextFormat> CreateTextFormat(const Font& font);
    
    D2D1_COLOR_F ToD2DColor(const Color& color) const;
    D2D1_POINT_2F ToD2DPoint(double x, double y) const;
    D2D1_RECT_F ToD2DRect(double x, double y, double w, double h) const;
    D2D1_ELLIPSE ToD2DEllipse(double cx, double cy, double rx, double ry) const;
    
    void ApplyTransform();
    void ApplyClipRegion();

private:
    ComPtr<ID2D1Factory> m_factory;
    ComPtr<ID2D1RenderTarget> m_renderTarget;
    ComPtr<ID2D1SolidColorBrush> m_solidBrush;
    ComPtr<IDWriteFactory> m_writeFactory;
    
    bool m_isActive;
    std::stack<D2D1_MATRIX_3X2_F> m_transformStack;
    
    static ComPtr<ID2D1Factory> s_factory;
    static ComPtr<IDWriteFactory> s_writeFactory;
    static int s_instanceCount;
    static bool s_initialized;
};

}

}

#endif

#endif
