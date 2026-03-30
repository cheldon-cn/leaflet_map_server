#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>

namespace ogc {
namespace draw {
class DrawContext;
class DrawDevice;
}

class Envelope;

namespace mokrender {

class OGC_MOKRENDER_API RenderContext {
public:
    RenderContext();
    ~RenderContext();
    
    MokRenderResult Initialize(const RenderConfig& config);
    void SetExtent(double minX, double minY, double maxX, double maxY);
    void GetExtent(double& minX, double& minY, double& maxX, double& maxY) const;
    void SetDPI(double dpi);
    double GetDPI() const;
    
    void SetDrawContext(std::shared_ptr<draw::DrawContext> context);
    std::shared_ptr<draw::DrawContext> GetDrawContext() const;
    
    Envelope GetVisibleExtent() const;
    double GetScale() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
