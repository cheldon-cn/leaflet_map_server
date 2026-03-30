#include "ogc/mokrender/display_device_output.h"

namespace ogc {
namespace mokrender {

class DisplayDeviceOutput::Impl {
public:
    int width, height;
    double dpi;
    bool initialized;
    
    Impl() : width(800), height(600), dpi(96.0), initialized(false) {}
};

DisplayDeviceOutput::DisplayDeviceOutput() : m_impl(new Impl()) {
}

DisplayDeviceOutput::~DisplayDeviceOutput() {
}

MokRenderResult DisplayDeviceOutput::Initialize(const RenderConfig& config) {
    m_impl->width = config.outputWidth;
    m_impl->height = config.outputHeight;
    m_impl->dpi = config.dpi;
    m_impl->initialized = true;
    return MokRenderResult();
}

MokRenderResult DisplayDeviceOutput::Render() {
    if (!m_impl->initialized) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Not initialized");
    }
    return MokRenderResult();
}

}
}
