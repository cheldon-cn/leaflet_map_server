#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API SpatialQueryEngine : public ISpatialQueryEngine {
public:
    SpatialQueryEngine();
    ~SpatialQueryEngine() override;
    
    MokRenderResult Initialize(const std::string& dbPath) override;
    std::vector<void*> QueryByExtent(double minX, double minY,
                                     double maxX, double maxY,
                                     const std::string& layerName) override;
    void Close() override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
