#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>
#include <string>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API RasterDeviceOutput {
public:
    RasterDeviceOutput();
    ~RasterDeviceOutput();
    
    MokRenderResult Initialize(const RenderConfig& config);
    MokRenderResult Render(const std::string& outputPath);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
