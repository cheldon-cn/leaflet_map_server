#ifndef OGC_DRAW_COORDINATE_TRANSFORMER_H
#define OGC_DRAW_COORDINATE_TRANSFORMER_H

#include "ogc/draw/export.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace draw {

class CoordinateTransformer;
typedef std::shared_ptr<CoordinateTransformer> CoordinateTransformerPtr;

class OGC_GRAPH_API CoordinateTransformer {
public:
    virtual ~CoordinateTransformer() = default;
    
    virtual bool IsValid() const = 0;
    
    virtual Coordinate Transform(const Coordinate& coord) const = 0;
    
    virtual Coordinate TransformInverse(const Coordinate& coord) const = 0;
    
    virtual void Transform(double& x, double& y) const = 0;
    
    virtual void TransformInverse(double& x, double& y) const = 0;
    
    virtual void TransformArray(double* x, double* y, size_t count) const = 0;
    
    virtual void TransformArrayInverse(double* x, double* y, size_t count) const = 0;
    
    virtual Envelope Transform(const Envelope& env) const = 0;
    
    virtual Envelope TransformInverse(const Envelope& env) const = 0;
    
    virtual GeometryPtr Transform(const Geometry* geometry) const = 0;
    
    virtual GeometryPtr TransformInverse(const Geometry* geometry) const = 0;
    
    virtual std::string GetSourceCRS() const = 0;
    
    virtual std::string GetTargetCRS() const = 0;
    
    virtual std::string GetName() const = 0;
    
    virtual std::string GetDescription() const = 0;
    
    virtual CoordinateTransformerPtr Clone() const = 0;
    
    static CoordinateTransformerPtr Create(const std::string& sourceCRS, const std::string& targetCRS);
    static CoordinateTransformerPtr CreateWGS84ToWebMercator();
    static CoordinateTransformerPtr CreateWebMercatorToWGS84();
};

}
}

#endif
