#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API LineGenerator : public IFeatureGenerator {
public:
    LineGenerator();
    ~LineGenerator() override;
    
    MokRenderResult Initialize(double minX, double minY,
                               double maxX, double maxY, int srid) override;
    void* GenerateFeature() override;
    std::string GetLayerName() const override;
    std::string GetGeometryType() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
