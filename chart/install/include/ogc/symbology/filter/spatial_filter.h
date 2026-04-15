#ifndef OGC_SYMBOLOGY_SPATIAL_FILTER_H
#define OGC_SYMBOLOGY_SPATIAL_FILTER_H

#include "ogc/symbology/filter/filter.h"
#include "ogc/geom/envelope.h"
#include <memory>

namespace ogc {
namespace symbology {

enum class SpatialOperator {
    kBbox,
    kIntersects,
    kWithin,
    kContains,
    kEquals,
    kOverlaps,
    kTouches,
    kCrosses,
    kDisjoint
};

class OGC_SYMBOLOGY_API SpatialFilter : public Filter {
public:
    SpatialFilter(SpatialOperator op, const Geometry* geometry);
    SpatialFilter(SpatialOperator op, const Envelope& envelope);
    SpatialFilter(SpatialOperator op, std::shared_ptr<Geometry> geometry);
    ~SpatialFilter() override;
    
    FilterType GetType() const override { return FilterType::kSpatial; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    std::string ToString() const override;
    FilterPtr Clone() const override;
    
    SpatialOperator GetOperator() const;
    void SetOperator(SpatialOperator op);
    
    const Geometry* GetGeometry() const;
    void SetGeometry(const Geometry* geometry);
    void SetGeometry(std::shared_ptr<Geometry> geometry);
    
    const Envelope& GetEnvelope() const;
    void SetEnvelope(const Envelope& envelope);
    
    bool HasGeometry() const;
    bool HasEnvelope() const;
    
    void SetPropertyName(const std::string& name);
    std::string GetPropertyName() const;
    
    static std::string OperatorToString(SpatialOperator op);
    static SpatialOperator StringToOperator(const std::string& str);
    
private:
    bool EvaluateGeometry(const Geometry* testGeom) const;
    bool EvaluateEnvelope(const Geometry* testGeom) const;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

typedef std::shared_ptr<SpatialFilter> SpatialFilterPtr;

}
}

#endif
