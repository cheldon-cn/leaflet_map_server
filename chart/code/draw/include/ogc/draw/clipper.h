#ifndef OGC_DRAW_CLIPPER_H
#define OGC_DRAW_CLIPPER_H

#include "ogc/draw/export.h"
#include "ogc/geom/envelope.h"
#include "ogc/geom/geometry.h"
#include <memory>
#include <vector>

namespace ogc {
namespace draw {

enum class ClipResult {
    kInside,
    kOutside,
    kPartial,
    kError
};

class Clipper;
typedef std::unique_ptr<Clipper> ClipperPtr;

class OGC_DRAW_API Clipper {
public:
    Clipper();
    explicit Clipper(const Envelope& clipEnvelope);
    ~Clipper() = default;
    
    void SetClipEnvelope(const Envelope& envelope);
    Envelope GetClipEnvelope() const;
    void ClearClipEnvelope();
    bool HasClipEnvelope() const;
    
    ClipResult TestPoint(double x, double y) const;
    ClipResult TestPoint(const Coordinate& coord) const;
    
    ClipResult TestEnvelope(const Envelope& env) const;
    
    ClipResult TestGeometry(const Geometry* geometry) const;
    
    GeometryPtr ClipPoint(const Geometry* geometry) const;
    GeometryPtr ClipLineString(const Geometry* geometry) const;
    GeometryPtr ClipPolygon(const Geometry* geometry) const;
    GeometryPtr ClipGeometry(const Geometry* geometry) const;
    
    std::vector<GeometryPtr> ClipGeometries(const std::vector<const Geometry*>& geometries) const;
    
    Envelope ClipEnvelope(const Envelope& env) const;
    
    void SetTolerance(double tolerance);
    double GetTolerance() const;
    
    bool IsPointInside(double x, double y) const;
    bool IsPointOnBoundary(double x, double y) const;
    
    static ClipperPtr Create();
    static ClipperPtr Create(const Envelope& clipEnvelope);
    
private:
    Envelope m_clipEnvelope;
    bool m_hasClipEnvelope;
    double m_tolerance;
    
    GeometryPtr ClipLineStringInternal(const Geometry* geometry) const;
    GeometryPtr ClipPolygonInternal(const Geometry* geometry) const;
    
    std::vector<Coordinate> ClipLineToEnvelope(const Coordinate& p1, const Coordinate& p2, 
                                                const Envelope& env) const;
    
    int ComputeOutCode(double x, double y, const Envelope& env) const;
    
    bool CohenSutherlandLineClip(double& x0, double& y0, double& x1, double& y1, 
                                  const Envelope& env) const;
    
    std::vector<std::vector<Coordinate>> SutherlandHodgmanPolygonClip(
        const std::vector<Coordinate>& polygon, const Envelope& env) const;
    
    std::vector<Coordinate> ClipPolygonEdge(const std::vector<Coordinate>& polygon,
                                             double edgeMin, double edgeMax, 
                                             int edge, bool isHorizontal) const;
    
    bool IsInsideEdge(double x, double y, double edge, int edgeType, bool isHorizontal) const;
    Coordinate IntersectEdge(const Coordinate& p1, const Coordinate& p2,
                             double edge, bool isHorizontal) const;
};

}
}

#endif
