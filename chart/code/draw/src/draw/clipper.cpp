#include "ogc/draw/clipper.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipoint.h"
#include "ogc/geom/multilinestring.h"
#include "ogc/geom/multipolygon.h"
#include "ogc/geom/geometrycollection.h"
#include <algorithm>

namespace ogc {
namespace draw {

Clipper::Clipper()
    : m_hasClipEnvelope(false)
    , m_tolerance(1e-9)
{
}

Clipper::Clipper(const Envelope& clipEnvelope)
    : m_clipEnvelope(clipEnvelope)
    , m_hasClipEnvelope(true)
    , m_tolerance(1e-9)
{
}

void Clipper::SetClipEnvelope(const Envelope& envelope)
{
    m_clipEnvelope = envelope;
    m_hasClipEnvelope = true;
}

Envelope Clipper::GetClipEnvelope() const
{
    return m_clipEnvelope;
}

void Clipper::ClearClipEnvelope()
{
    m_clipEnvelope = Envelope();
    m_hasClipEnvelope = false;
}

bool Clipper::HasClipEnvelope() const
{
    return m_hasClipEnvelope;
}

ClipResult Clipper::TestPoint(double x, double y) const
{
    if (!m_hasClipEnvelope) {
        return ClipResult::kInside;
    }
    
    Coordinate coord(x, y);
    if (m_clipEnvelope.Contains(coord)) {
        return ClipResult::kInside;
    }
    
    bool onBoundary = (std::abs(x - m_clipEnvelope.GetMinX()) <= m_tolerance ||
                       std::abs(x - m_clipEnvelope.GetMaxX()) <= m_tolerance ||
                       std::abs(y - m_clipEnvelope.GetMinY()) <= m_tolerance ||
                       std::abs(y - m_clipEnvelope.GetMaxY()) <= m_tolerance);
    
    if (onBoundary && x >= m_clipEnvelope.GetMinX() - m_tolerance && 
        x <= m_clipEnvelope.GetMaxX() + m_tolerance &&
        y >= m_clipEnvelope.GetMinY() - m_tolerance && 
        y <= m_clipEnvelope.GetMaxY() + m_tolerance) {
        return ClipResult::kPartial;
    }
    
    return ClipResult::kOutside;
}

ClipResult Clipper::TestPoint(const Coordinate& coord) const
{
    return TestPoint(coord.x, coord.y);
}

ClipResult Clipper::TestEnvelope(const Envelope& env) const
{
    if (!m_hasClipEnvelope) {
        return ClipResult::kInside;
    }
    
    if (m_clipEnvelope.Contains(env)) {
        return ClipResult::kInside;
    }
    
    if (!m_clipEnvelope.Intersects(env)) {
        return ClipResult::kOutside;
    }
    
    return ClipResult::kPartial;
}

ClipResult Clipper::TestGeometry(const Geometry* geometry) const
{
    if (!geometry || !m_hasClipEnvelope) {
        return ClipResult::kInside;
    }
    
    Envelope geomEnv = geometry->GetEnvelope();
    return TestEnvelope(geomEnv);
}

GeometryPtr Clipper::ClipPoint(const Geometry* geometry) const
{
    const Point* point = dynamic_cast<const Point*>(geometry);
    if (!point) {
        return nullptr;
    }
    
    Coordinate coord = point->GetCoordinate();
    ClipResult result = TestPoint(coord);
    
    if (result == ClipResult::kInside || result == ClipResult::kPartial) {
        return point->Clone();
    }
    
    return nullptr;
}

GeometryPtr Clipper::ClipLineString(const Geometry* geometry) const
{
    if (!geometry || !m_hasClipEnvelope) {
        return geometry ? geometry->Clone() : nullptr;
    }
    
    return ClipLineStringInternal(geometry);
}

GeometryPtr Clipper::ClipPolygon(const Geometry* geometry) const
{
    if (!geometry || !m_hasClipEnvelope) {
        return geometry ? geometry->Clone() : nullptr;
    }
    
    return ClipPolygonInternal(geometry);
}

GeometryPtr Clipper::ClipGeometry(const Geometry* geometry) const
{
    if (!geometry) {
        return nullptr;
    }
    
    if (!m_hasClipEnvelope) {
        return geometry->Clone();
    }
    
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint:
            return ClipPoint(geometry);
        case GeomType::kLineString:
            return ClipLineString(geometry);
        case GeomType::kPolygon:
            return ClipPolygon(geometry);
        case GeomType::kMultiPoint: {
            const MultiPoint* multiPoint = dynamic_cast<const MultiPoint*>(geometry);
            if (!multiPoint) return nullptr;
            
            CoordinateList coords;
            for (size_t i = 0; i < multiPoint->GetNumGeometries(); ++i) {
                const Point* point = multiPoint->GetPointN(i);
                if (point) {
                    GeometryPtr clipped = ClipPoint(point);
                    if (clipped) {
                        const Point* clippedPoint = dynamic_cast<const Point*>(clipped.get());
                        if (clippedPoint) {
                            coords.push_back(clippedPoint->GetCoordinate());
                        }
                    }
                }
            }
            if (coords.empty()) return nullptr;
            return MultiPoint::Create(coords);
        }
        case GeomType::kMultiLineString: {
            const MultiLineString* multiLineString = dynamic_cast<const MultiLineString*>(geometry);
            if (!multiLineString) return nullptr;
            
            std::vector<LineStringPtr> lineStrings;
            for (size_t i = 0; i < multiLineString->GetNumGeometries(); ++i) {
                const LineString* lineString = multiLineString->GetLineStringN(i);
                if (lineString) {
                    GeometryPtr clipped = ClipLineString(lineString);
                    if (clipped) {
                        LineStringPtr clippedLine(static_cast<LineString*>(clipped.release()));
                        if (clippedLine) {
                            lineStrings.push_back(std::move(clippedLine));
                        }
                    }
                }
            }
            if (lineStrings.empty()) return nullptr;
            return MultiLineString::Create(std::move(lineStrings));
        }
        case GeomType::kMultiPolygon: {
            const MultiPolygon* multiPolygon = dynamic_cast<const MultiPolygon*>(geometry);
            if (!multiPolygon) return nullptr;
            
            std::vector<PolygonPtr> polygons;
            for (size_t i = 0; i < multiPolygon->GetNumGeometries(); ++i) {
                const Polygon* polygon = multiPolygon->GetPolygonN(i);
                if (polygon) {
                    GeometryPtr clipped = ClipPolygon(polygon);
                    if (clipped) {
                        PolygonPtr clippedPolygon(static_cast<Polygon*>(clipped.release()));
                        if (clippedPolygon) {
                            polygons.push_back(std::move(clippedPolygon));
                        }
                    }
                }
            }
            if (polygons.empty()) return nullptr;
            return MultiPolygon::Create(std::move(polygons));
        }
        case GeomType::kGeometryCollection: {
            const GeometryCollection* collection = dynamic_cast<const GeometryCollection*>(geometry);
            if (!collection) return nullptr;
            
            std::vector<GeometryPtr> geometries;
            for (size_t i = 0; i < collection->GetNumGeometries(); ++i) {
                const Geometry* geom = collection->GetGeometryN(i);
                GeometryPtr clipped = ClipGeometry(geom);
                if (clipped) {
                    geometries.push_back(std::move(clipped));
                }
            }
            if (geometries.empty()) return nullptr;
            return GeometryCollection::Create(std::move(geometries));
        }
        default:
            return geometry->Clone();
    }
}

std::vector<GeometryPtr> Clipper::ClipGeometries(const std::vector<const Geometry*>& geometries) const
{
    std::vector<GeometryPtr> result;
    result.reserve(geometries.size());
    
    for (const Geometry* geometry : geometries) {
        GeometryPtr clipped = ClipGeometry(geometry);
        if (clipped) {
            result.push_back(std::move(clipped));
        }
    }
    
    return result;
}

Envelope Clipper::ClipEnvelope(const Envelope& env) const
{
    if (!m_hasClipEnvelope) {
        return env;
    }
    
    return m_clipEnvelope.Intersection(env);
}

void Clipper::SetTolerance(double tolerance)
{
    m_tolerance = tolerance;
}

double Clipper::GetTolerance() const
{
    return m_tolerance;
}

bool Clipper::IsPointInside(double x, double y) const
{
    if (!m_hasClipEnvelope) {
        return true;
    }
    Coordinate coord(x, y);
    return m_clipEnvelope.Contains(coord);
}

bool Clipper::IsPointOnBoundary(double x, double y) const
{
    if (!m_hasClipEnvelope) {
        return false;
    }
    
    return (std::abs(x - m_clipEnvelope.GetMinX()) <= m_tolerance ||
            std::abs(x - m_clipEnvelope.GetMaxX()) <= m_tolerance ||
            std::abs(y - m_clipEnvelope.GetMinY()) <= m_tolerance ||
            std::abs(y - m_clipEnvelope.GetMaxY()) <= m_tolerance);
}

ClipperPtr Clipper::Create()
{
    return ClipperPtr(new Clipper());
}

ClipperPtr Clipper::Create(const Envelope& clipEnvelope)
{
    return ClipperPtr(new Clipper(clipEnvelope));
}

GeometryPtr Clipper::ClipLineStringInternal(const Geometry* geometry) const
{
    const LineString* lineString = dynamic_cast<const LineString*>(geometry);
    if (!lineString) {
        return nullptr;
    }
    
    if (lineString->GetNumPoints() < 2) {
        ClipResult result = TestGeometry(lineString);
        if (result == ClipResult::kInside || result == ClipResult::kPartial) {
            return lineString->Clone();
        }
        return nullptr;
    }
    
    std::vector<Coordinate> clippedCoords;
    
    for (size_t i = 0; i < lineString->GetNumPoints() - 1; ++i) {
        Coordinate p1 = lineString->GetCoordinateN(i);
        Coordinate p2 = lineString->GetCoordinateN(i + 1);
        
        double x0 = p1.x, y0 = p1.y;
        double x1 = p2.x, y1 = p2.y;
        
        if (CohenSutherlandLineClip(x0, y0, x1, y1, m_clipEnvelope)) {
            if (clippedCoords.empty() || 
                std::abs(clippedCoords.back().x - x0) > m_tolerance ||
                std::abs(clippedCoords.back().y - y0) > m_tolerance) {
                clippedCoords.push_back(Coordinate(x0, y0));
            }
            clippedCoords.push_back(Coordinate(x1, y1));
        }
    }
    
    if (clippedCoords.size() < 2) {
        return nullptr;
    }
    
    return LineString::Create(clippedCoords);
}

GeometryPtr Clipper::ClipPolygonInternal(const Geometry* geometry) const
{
    const Polygon* polygon = dynamic_cast<const Polygon*>(geometry);
    if (!polygon) {
        return nullptr;
    }
    
    const LinearRing* shell = polygon->GetExteriorRing();
    if (!shell || shell->GetNumPoints() < 3) {
        return nullptr;
    }
    
    std::vector<Coordinate> shellCoords;
    for (size_t i = 0; i < shell->GetNumPoints(); ++i) {
        shellCoords.push_back(shell->GetCoordinateN(i));
    }
    
    std::vector<std::vector<Coordinate>> clippedPolygons = 
        SutherlandHodgmanPolygonClip(shellCoords, m_clipEnvelope);
    
    if (clippedPolygons.empty()) {
        return nullptr;
    }
    
    if (clippedPolygons.size() == 1) {
        LinearRingPtr exteriorRing = LinearRing::Create(clippedPolygons[0]);
        PolygonPtr result = Polygon::Create(std::move(exteriorRing));
        
        for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
            const LinearRing* hole = polygon->GetInteriorRingN(i);
            if (hole) {
                std::vector<Coordinate> holeCoords;
                for (size_t j = 0; j < hole->GetNumPoints(); ++j) {
                    holeCoords.push_back(hole->GetCoordinateN(j));
                }
                std::vector<std::vector<Coordinate>> clippedHoles = 
                    SutherlandHodgmanPolygonClip(holeCoords, m_clipEnvelope);
                for (const auto& clippedHole : clippedHoles) {
                    LinearRingPtr interiorRing = LinearRing::Create(clippedHole);
                    result->AddInteriorRing(std::move(interiorRing));
                }
            }
        }
        
        return result;
    }
    
    std::vector<PolygonPtr> polygons;
    for (const auto& polyCoords : clippedPolygons) {
        if (polyCoords.size() >= 3) {
            LinearRingPtr ring = LinearRing::Create(polyCoords);
            polygons.push_back(Polygon::Create(std::move(ring)));
        }
    }
    
    if (polygons.empty()) {
        return nullptr;
    }
    
    if (polygons.size() == 1) {
        return std::move(polygons[0]);
    }
    
    return MultiPolygon::Create(std::move(polygons));
}

int Clipper::ComputeOutCode(double x, double y, const Envelope& env) const
{
    int code = 0;
    
    if (x < env.GetMinX()) {
        code |= 1;
    } else if (x > env.GetMaxX()) {
        code |= 2;
    }
    
    if (y < env.GetMinY()) {
        code |= 4;
    } else if (y > env.GetMaxY()) {
        code |= 8;
    }
    
    return code;
}

bool Clipper::CohenSutherlandLineClip(double& x0, double& y0, double& x1, double& y1, 
                                       const Envelope& env) const
{
    int outcode0 = ComputeOutCode(x0, y0, env);
    int outcode1 = ComputeOutCode(x1, y1, env);
    bool accept = false;
    
    while (true) {
        if (!(outcode0 | outcode1)) {
            accept = true;
            break;
        } else if (outcode0 & outcode1) {
            break;
        } else {
            double x, y;
            int outcodeOut = outcode0 ? outcode0 : outcode1;
            
            if (outcodeOut & 8) {
                x = x0 + (x1 - x0) * (env.GetMaxY() - y0) / (y1 - y0);
                y = env.GetMaxY();
            } else if (outcodeOut & 4) {
                x = x0 + (x1 - x0) * (env.GetMinY() - y0) / (y1 - y0);
                y = env.GetMinY();
            } else if (outcodeOut & 2) {
                y = y0 + (y1 - y0) * (env.GetMaxX() - x0) / (x1 - x0);
                x = env.GetMaxX();
            } else {
                y = y0 + (y1 - y0) * (env.GetMinX() - x0) / (x1 - x0);
                x = env.GetMinX();
            }
            
            if (outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = ComputeOutCode(x0, y0, env);
            } else {
                x1 = x;
                y1 = y;
                outcode1 = ComputeOutCode(x1, y1, env);
            }
        }
    }
    
    return accept;
}

std::vector<std::vector<Coordinate>> Clipper::SutherlandHodgmanPolygonClip(
    const std::vector<Coordinate>& polygon, const Envelope& env) const
{
    std::vector<std::vector<Coordinate>> result;
    std::vector<Coordinate> currentPolygon = polygon;
    
    currentPolygon = ClipPolygonEdge(currentPolygon, env.GetMinX(), env.GetMaxX(), 0, false);
    if (currentPolygon.empty()) return result;
    
    currentPolygon = ClipPolygonEdge(currentPolygon, env.GetMinX(), env.GetMaxX(), 1, false);
    if (currentPolygon.empty()) return result;
    
    currentPolygon = ClipPolygonEdge(currentPolygon, env.GetMinY(), env.GetMaxY(), 0, true);
    if (currentPolygon.empty()) return result;
    
    currentPolygon = ClipPolygonEdge(currentPolygon, env.GetMinY(), env.GetMaxY(), 1, true);
    if (currentPolygon.empty()) return result;
    
    if (currentPolygon.size() >= 3) {
        result.push_back(currentPolygon);
    }
    
    return result;
}

std::vector<Coordinate> Clipper::ClipPolygonEdge(const std::vector<Coordinate>& polygon,
                                                  double edgeMin, double edgeMax,
                                                  int edge, bool isHorizontal) const
{
    std::vector<Coordinate> result;
    
    if (polygon.empty()) {
        return result;
    }
    
    double edgeValue = (edge == 0) ? edgeMin : edgeMax;
    
    for (size_t i = 0; i < polygon.size(); ++i) {
        const Coordinate& current = polygon[i];
        const Coordinate& next = polygon[(i + 1) % polygon.size()];
        
        bool currentInside = IsInsideEdge(current.x, current.y, edgeValue, edge, isHorizontal);
        bool nextInside = IsInsideEdge(next.x, next.y, edgeValue, edge, isHorizontal);
        
        if (currentInside) {
            result.push_back(current);
        }
        
        if (currentInside != nextInside) {
            Coordinate intersection = IntersectEdge(current, next, edgeValue, isHorizontal);
            result.push_back(intersection);
        }
    }
    
    return result;
}

bool Clipper::IsInsideEdge(double x, double y, double edge, int edgeType, bool isHorizontal) const
{
    if (isHorizontal) {
        return (edgeType == 0) ? (y >= edge) : (y <= edge);
    } else {
        return (edgeType == 0) ? (x >= edge) : (x <= edge);
    }
}

Coordinate Clipper::IntersectEdge(const Coordinate& p1, const Coordinate& p2,
                                  double edge, bool isHorizontal) const
{
    double t;
    if (isHorizontal) {
        if (std::abs(p2.y - p1.y) < m_tolerance) {
            return p1;
        }
        t = (edge - p1.y) / (p2.y - p1.y);
    } else {
        if (std::abs(p2.x - p1.x) < m_tolerance) {
            return p1;
        }
        t = (edge - p1.x) / (p2.x - p1.x);
    }
    
    return Coordinate(
        p1.x + t * (p2.x - p1.x),
        p1.y + t * (p2.y - p1.y)
    );
}

}
}
