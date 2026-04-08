#ifndef OGC_DRAW_GPU_ACCELERATED_ENGINE_H
#define OGC_DRAW_GPU_ACCELERATED_ENGINE_H

#include <ogc/draw/export.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/gpu_resource_manager.h>
#include <ogc/draw/render_memory_pool.h>
#include <vector>
#include <stack>
#include <memory>

namespace ogc {
namespace draw {

struct DrawBatch {
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    DrawStyle style;
    int primitiveType = 0;
};

struct EngineState {
    TransformMatrix transform;
    Region clipRegion;
    DrawStyle style;
    double opacity = 1.0;
};

class OGC_DRAW_API GPUAcceleratedEngine : public DrawEngine {
public:
    explicit GPUAcceleratedEngine(DrawDevice* device);
    ~GPUAcceleratedEngine() override;
    
    std::string GetName() const override { return "GPUAcceleratedEngine"; }
    EngineType GetType() const override { return EngineType::kGPU; }
    DrawDevice* GetDevice() const override { return m_device; }
    
    DrawResult Begin() override;
    void End() override;
    bool IsActive() const override { return m_active; }
    
    DrawResult DrawPoints(const double* x, const double* y, int count,
                          const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                         const double* x2, const double* y2, int count,
                         const DrawStyle& style) override;
    DrawResult DrawLineString(const double* x, const double* y, int count,
                              const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count,
                           const DrawStyle& style, bool fill = true) override;
    DrawResult DrawRect(double x, double y, double w, double h,
                        const DrawStyle& style, bool fill = true) override;
    DrawResult DrawCircle(double cx, double cy, double radius,
                          const DrawStyle& style, bool fill = true) override;
    DrawResult DrawEllipse(double cx, double cy, double rx, double ry,
                           const DrawStyle& style, bool fill = true) override;
    DrawResult DrawArc(double cx, double cy, double rx, double ry,
                       double startAngle, double sweepAngle,
                       const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text,
                        const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image,
                         double scaleX = 1.0, double scaleY = 1.0) override;
    DrawResult DrawImageRect(double x, double y, double w, double h,
                             const Image& image) override;
    DrawResult DrawGeometry(const Geometry* geometry,
                            const DrawStyle& style) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    TransformMatrix GetTransform() const override { return m_transform; }
    void ResetTransform() override;
    
    void SetClipRect(double x, double y, double w, double h) override;
    void SetClipRegion(const Region& region) override;
    void ResetClip() override;
    Region GetClipRegion() const override { return m_clipRegion; }
    
    void Save(StateFlags flags = static_cast<StateFlags>(StateFlag::kAll)) override;
    void Restore() override;
    
    void SetOpacity(double opacity) override;
    double GetOpacity() const override { return m_opacity; }
    
    void SetTolerance(double tolerance) override;
    double GetTolerance() const override { return m_tolerance; }
    
    void SetAntialiasingEnabled(bool enabled) override;
    bool IsAntialiasingEnabled() const override { return m_antialiasing; }
    
    TextMetrics MeasureText(const std::string& text, const Font& font) override;
    
    void Clear(const Color& color) override;
    void Flush() override;
    
    bool SupportsFeature(const std::string& featureName) const override;
    
private:
    void FlushBatch();
    void SetupDefaultShader();
    void UpdateUniforms();
    
    Point TransformPoint(double x, double y) const;
    void AddVertex(std::vector<float>& vertices, double x, double y, 
                   const Color& color, double opacity);
    
    bool InitializeShaders();
    bool InitializeBuffers();
    void CleanupResources();
    
    DrawDevice* m_device;
    bool m_active;
    
    TransformMatrix m_transform;
    Region m_clipRegion;
    double m_opacity;
    double m_tolerance;
    bool m_antialiasing;
    DrawStyle m_currentStyle;
    
    std::stack<EngineState> m_stateStack;
    
    std::unique_ptr<ShaderProgram> m_basicShader;
    std::unique_ptr<ShaderProgram> m_lineShader;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    
    DrawBatch m_currentBatch;
    std::vector<DrawBatch> m_batches;
    
    bool m_initialized;
};

}
}

#endif
