#ifndef OGC_MOKRENDER_MULTIPOINT_GENERATOR_H
#define OGC_MOKRENDER_MULTIPOINT_GENERATOR_H

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API MultiPointGenerator : public IFeatureGenerator {
public:
    MultiPointGenerator();
    ~MultiPointGenerator() override;
    
    MokRenderResult Initialize(double minX, double minY, double maxX, double maxY, int srid) override;
    void* GenerateFeature() override;
    std::string GetLayerName() const override;
    std::string GetGeometryType() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
