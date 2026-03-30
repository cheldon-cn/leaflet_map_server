#ifdef __APPLE__

#include "ogc/draw/metal_engine.h"
#include <cmath>

namespace ogc {
namespace draw {

MetalEngine::MetalEngine() {
    m_transform = TransformMatrix::Identity();
}

MetalEngine::MetalEngine(DrawDevice* device) 
    : DrawEngine(device) {
    m_transform = TransformMatrix::Identity();
}

MetalEngine::~MetalEngine() {
}

bool MetalEngine::Initialize() {
    m_device = MTLCreateSystemDefaultDevice();
    if (!m_device) {
        return false;
    }
    
    m_commandQueue = [m_device newCommandQueue];
    if (!m_commandQueue) {
        return false;
    }
    
    return CreatePipelineState();
}

void MetalEngine::SetDevice(id<MTLDevice> device) {
    m_device = device;
}

void MetalEngine::SetCommandQueue(id<MTLCommandQueue> queue) {
    m_commandQueue = queue;
}

void MetalEngine::SetRenderCommandEncoder(id<MTLRenderCommandEncoder> encoder) {
    m_renderEncoder = encoder;
}

bool MetalEngine::CreatePipelineState() {
    return true;
}

bool MetalEngine::Begin() {
    return m_device != nullptr;
}

void MetalEngine::End() {
    m_renderEncoder = nullptr;
}

void MetalEngine::ApplyTransform() {
}

DrawResult MetalEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_renderEncoder || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    return DrawResult::Success;
}

DrawResult MetalEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_renderEncoder || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    return DrawResult::Success;
}

DrawResult MetalEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_renderEncoder || count < 3) {
        return DrawResult::InvalidParameter;
    }
    
    return DrawResult::Success;
}

DrawResult MetalEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_renderEncoder || text.empty()) {
        return DrawResult::InvalidParameter;
    }
    
    return DrawResult::Success;
}

DrawResult MetalEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_renderEncoder || image.data.empty()) {
        return DrawResult::InvalidParameter;
    }
    
    return DrawResult::Success;
}

void MetalEngine::SetTransform(const TransformMatrix& matrix) {
    m_transform = matrix;
    ApplyTransform();
}

void MetalEngine::SetClipRegion(const Region& region) {
    m_clipRegion = region;
}

void MetalEngine::PushState() {
    m_stateStack.push_back(m_transform);
}

void MetalEngine::PopState() {
    if (!m_stateStack.empty()) {
        m_transform = m_stateStack.back();
        m_stateStack.pop_back();
        ApplyTransform();
    }
}

}
}

#endif
