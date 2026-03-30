#include "ogc/mokrender/render_context.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_style.h"
#include "ogc/envelope.h"

namespace ogc {
namespace mokrender {

class RenderContext::Impl {
public:
    double minX, minY, maxX, maxY;
    double dpi;
    int width, height;
    bool initialized;
    std::shared_ptr<draw::DrawContext> drawContext;
    std::shared_ptr<draw::DrawDevice> drawDevice;
    
    Impl() : minX(0), minY(0), maxX(1000), maxY(1000), 
             dpi(96.0), width(800), height(600), initialized(false) {}
};

RenderContext::RenderContext() : m_impl(new Impl()) {
}

RenderContext::~RenderContext() {
}

MokRenderResult RenderContext::Initialize(const RenderConfig& config) {
    m_impl->width = config.outputWidth;
    m_impl->height = config.outputHeight;
    m_impl->dpi = config.dpi;
    m_impl->initialized = true;
    return MokRenderResult();
}

void RenderContext::SetExtent(double minX, double minY, double maxX, double maxY) {
    m_impl->minX = minX;
    m_impl->minY = minY;
    m_impl->maxX = maxX;
    m_impl->maxY = maxY;
}

void RenderContext::GetExtent(double& minX, double& minY, double& maxX, double& maxY) const {
    minX = m_impl->minX;
    minY = m_impl->minY;
    maxX = m_impl->maxX;
    maxY = m_impl->maxY;
}

void RenderContext::SetDPI(double dpi) {
    m_impl->dpi = dpi;
}

double RenderContext::GetDPI() const {
    return m_impl->dpi;
}

void RenderContext::SetDrawContext(std::shared_ptr<draw::DrawContext> context) {
    m_impl->drawContext = context;
}

std::shared_ptr<draw::DrawContext> RenderContext::GetDrawContext() const {
    return m_impl->drawContext;
}

Envelope RenderContext::GetVisibleExtent() const {
    return Envelope(m_impl->minX, m_impl->minY, m_impl->maxX, m_impl->maxY);
}

double RenderContext::GetScale() const {
    if (m_impl->width == 0) return 1.0;
    double worldWidth = m_impl->maxX - m_impl->minX;
    return worldWidth / m_impl->width;
}

}
}
