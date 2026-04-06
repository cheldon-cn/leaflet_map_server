#ifndef OGC_SYMBOLOGY_SPATIAL_FILTER_H
#define OGC_SYMBOLOGY_SPATIAL_FILTER_H

#include "ogc/symbology/filter/filter.h"
#include "ogc/envelope.h"
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
    
    FilterType GetType() const override { return FilterType::kSpatial; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    std::string ToString() const override;
    FilterPtr Clone() const override;
    
    SpatialOperator GetOperator() const { return m_operator; }
    void SetOperator(SpatialOperator op) { m_operator = op; }
    
    const Geometry* GetGeometry() const { return m_geometry.get(); }
    void SetGeometry(const Geometry* geometry);
    void SetGeometry(std::shared_ptr<Geometry> geometry);
    
    const Envelope& GetEnvelope() const { return m_envelope; }
    void SetEnvelope(const Envelope& envelope);
    
    bool HasGeometry() const { return m_geometry != nullptr; }
    bool HasEnvelope() const { return m_hasEnvelope; }
    
    void SetPropertyName(const std::string& name) { m_propertyName = name; }
    std::string GetPropertyName() const { return m_propertyName; }
    
    static std::string OperatorToString(SpatialOperator op);
    static SpatialOperator StringToOperator(const std::string& str);
    
private:
    bool EvaluateGeometry(const Geometry* testGeom) const;
    bool EvaluateEnvelope(const Geometry* testGeom) const;
    
    SpatialOperator m_operator;
    std::shared_ptr<Geometry> m_geometry;
    Envelope m_envelope;
    bool m_hasEnvelope;
    std::string m_propertyName;
};

typedef std::shared_ptr<SpatialFilter> SpatialFilterPtr;

}
}

#endif
