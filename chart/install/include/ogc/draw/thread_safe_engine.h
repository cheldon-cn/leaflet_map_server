#ifndef OGC_DRAW_THREAD_SAFE_ENGINE_H
#define OGC_DRAW_THREAD_SAFE_ENGINE_H

#include <ogc/draw/export.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/geometry.h>
#include <mutex>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API ThreadSafeEngine : public DrawEngine {
public:
    explicit ThreadSafeEngine(std::unique_ptr<DrawEngine> engine);
    ~ThreadSafeEngine() override;
    
    ThreadSafeEngine(const ThreadSafeEngine&) = delete;
    ThreadSafeEngine& operator=(const ThreadSafeEngine&) = delete;
    
    std::string GetName() const override;
    EngineType GetType() const override;
    DrawDevice* GetDevice() const override;
    
    DrawResult Begin() override;
    void End() override;
    bool IsActive() const override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawLineString(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) override;
    DrawResult DrawRect(double x, double y, double width, double height, const DrawStyle& style, bool fill) override;
    DrawResult DrawCircle(double cx, double cy, double r, const DrawStyle& style, bool fill) override;
    DrawResult DrawEllipse(double cx, double cy, double rx, double ry, const DrawStyle& style, bool fill) override;
    DrawResult DrawArc(double cx, double cy, double rx, double ry, double startAngle, double sweepAngle, const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0) override;
    DrawResult DrawImageRect(double x, double y, double w, double h, const Image& image) override;
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    TransformMatrix GetTransform() const override;
    void ResetTransform() override;
    
    void SetClipRect(double x, double y, double width, double height) override;
    void SetClipRegion(const Region& region) override;
    Region GetClipRegion() const override;
    void ResetClip() override;
    
    void SetOpacity(double opacity) override;
    double GetOpacity() const override;
    
    void SetTolerance(double tolerance) override;
    double GetTolerance() const override;
    
    void SetAntialiasingEnabled(bool enabled) override;
    bool IsAntialiasingEnabled() const override;
    
    void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) override;
    void Restore() override;
    
    TextMetrics MeasureText(const std::string& text, const Font& font) override;
    
    void Clear(const Color& color) override;
    void Flush() override;
    
    bool SupportsFeature(const std::string& featureName) const override;
    
    DrawEngine* GetWrappedEngine() const { return m_engine.get(); }
    
    void Lock();
    void Unlock();
    bool TryLock();
    
private:
    std::unique_ptr<DrawEngine> m_engine;
    mutable std::mutex m_mutex;
};

}
}

#endif
