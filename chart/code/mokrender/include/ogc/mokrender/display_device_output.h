#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API DisplayDeviceOutput {
public:
    DisplayDeviceOutput();
    ~DisplayDeviceOutput();
    
    MokRenderResult Initialize(const RenderConfig& config);
    MokRenderResult Render();
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
