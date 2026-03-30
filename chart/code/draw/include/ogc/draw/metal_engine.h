#ifndef OGC_DRAW_METAL_ENGINE_H
#define OGC_DRAW_METAL_ENGINE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"

#ifdef __APPLE__

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

namespace ogc {
namespace draw {

class OGC_DRAW_API MetalEngine : public DrawEngine {
public:
    MetalEngine();
    explicit MetalEngine(DrawDevice* device);
    ~MetalEngine() override;
    
    std::string GetName() const override { return "MetalEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    void PushState() override;
    void PopState() override;
    
    bool IsGPUAccelerated() const override { return true; }
    
    bool Initialize();
    void SetDevice(id<MTLDevice> device);
    void SetCommandQueue(id<MTLCommandQueue> queue);
    void SetRenderCommandEncoder(id<MTLRenderCommandEncoder> encoder);
    
    id<MTLDevice> GetDevice() const { return m_device; }
    id<MTLCommandQueue> GetCommandQueue() const { return m_commandQueue; }

private:
    id<MTLDevice> m_device = nullptr;
    id<MTLCommandQueue> m_commandQueue = nullptr;
    id<MTLRenderCommandEncoder> m_renderEncoder = nullptr;
    id<MTLRenderPipelineState> m_pipelineState = nullptr;
    
    TransformMatrix m_transform;
    Region m_clipRegion;
    std::vector<TransformMatrix> m_stateStack;
    
    bool CreatePipelineState();
    void ApplyTransform();
};

}
}

#endif

#endif
