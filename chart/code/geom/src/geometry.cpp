#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/visitor.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ogc {

namespace {

bool PointInPolygon(const Coordinate& point, const LinearRing* ring) {
    if (!ring || ring->IsEmpty()) return false;
    return ring->ContainsPoint(point);
}

bool PointOnRingBoundary(const Coordinate& point, const LinearRing* ring, double tolerance) {
    if (!ring) return false;
    return ring->IsPointOnBoundary(point, tolerance);
}

bool LineIntersectsLine(const Coordinate& p1, const Coordinate& p2,
                        const Coordinate& q1, const Coordinate& q2) {
    double d1 = (q2.x - q1.x) * (p1.y - q2.y) - (q2.y - q1.y) * (p1.x - q2.x);
    double d2 = (q2.x - q1.x) * (p2.y - q2.y) - (q2.y - q1.y) * (p2.x - q2.x);
    double d3 = (p2.x - p1.x) * (q1.y - p2.y) - (p2.y - p1.y) * (q1.x - p2.x);
    double d4 = (p2.x - p1.x) * (q2.y - p2.y) - (p2.y - p1.y) * (q2.x - p2.x);
    
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        return true;
    }
    
    auto onSegment = [](const Coordinate& p, const Coordinate& q, const Coordinate& r) {
        return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
               q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
    };
    
    if (std::abs(d1) < DEFAULT_TOLERANCE && onSegment(q1, p1, q2)) return true;
    if (std::abs(d2) < DEFAULT_TOLERANCE && onSegment(q1, p2, q2)) return true;
    if (std::abs(d3) < DEFAULT_TOLERANCE && onSegment(p1, q1, p2)) return true;
    if (std::abs(d4) < DEFAULT_TOLERANCE && onSegment(p1, q2, p2)) return true;
    
    return false;
}

bool LineStringIntersectsLineString(const LineString* line1, const LineString* line2) {
    if (!line1 || !line2) return false;
    
    const auto& coords1 = line1->GetCoordinates();
    const auto& coords2 = line2->GetCoordinates();
    
    for (size_t i = 0; i + 1 < coords1.size(); ++i) {
        for (size_t j = 0; j + 1 < coords2.size(); ++j) {
            if (LineIntersectsLine(coords1[i], coords1[i + 1],
                                   coords2[j], coords2[j + 1])) {
                return true;
            }
        }
    }
    
    return false;
}

bool LineStringIntersectsRing(const LineString* line, const LinearRing* ring) {
    if (!line || !ring) return false;
    return LineStringIntersectsLineString(line, ring);
}

bool RingContainsRing(const LinearRing* outer, const LinearRing* inner) {
    if (!outer || !inner) return false;
    
    const auto& innerCoords = inner->GetCoordinates();
    for (const auto& coord : innerCoords) {
        if (!outer->ContainsPoint(coord)) {
            return false;
        }
    }
    return true;
}

}

bool Geometry::IsValid(std::string* reason) const {
    if (IsEmpty()) {
        if (reason) *reason = "Geometry is empty";
        return true;
    }
    return true;
}

bool Geometry::IsSimple() const {
    return true;
}

GeomResult Geometry::Transform(int targetSRID, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::Transform(const std::string& targetCRS, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

double Geometry::Distance(const Geometry* other) const {
    if (!other || IsEmpty() || other->IsEmpty()) {
        return std::numeric_limits<double>::max();
    }
    
    return GetEnvelope().Distance(other->GetEnvelope());
}

bool Geometry::IsWithinDistance(const Geometry* other, double distance) const {
    return Distance(other) <= distance;
}

double Geometry::Area() const {
    return 0.0;
}

double Geometry::Length() const {
    return 0.0;
}

const Envelope& Geometry::GetEnvelope() const {
    if (!m_envelope_cache) {
        std::unique_lock<std::mutex> lock(m_cache_mutex);
        if (!m_envelope_cache) {
            m_envelope_cache.reset(new Envelope(ComputeEnvelope()));
        }
    }
    return *m_envelope_cache;
}

GeometryPtr Geometry::GetEnvelopeGeometry() const {
    const Envelope& env = GetEnvelope();
    if (env.IsNull()) return nullptr;
    
    return Polygon::CreateRectangle(env.GetMinX(), env.GetMinY(),
                                    env.GetMaxX(), env.GetMaxY());
}

Coordinate Geometry::GetCentroid() const {
    if (!m_centroid_cache) {
        std::unique_lock<std::mutex> lock(m_cache_mutex);
        if (!m_centroid_cache) {
            m_centroid_cache.reset(new Coordinate(ComputeCentroid()));
        }
    }
    return *m_centroid_cache;
}

Coordinate Geometry::GetPointOnSurface() const {
    return GetCentroid();
}

Coordinate Geometry::GetInteriorPoint() const {
    return GetCentroid();
}

Coordinate Geometry::GetCoordinateN(size_t index) const {
    GLM_THROW(GeomResult::kOutOfRange, "Index out of range");
    return Coordinate();
}

bool Geometry::GetCoordinateData(const double** data, size_t* count) const {
    *data = nullptr;
    *count = 0;
    return false;
}

bool Geometry::Equals(const Geometry* other, double tolerance) const {
    if (!other) return false;
    if (this == other) return true;
    if (GetGeometryType() != other->GetGeometryType()) return false;
    if (IsEmpty() && other->IsEmpty()) return true;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    auto coords1 = GetCoordinates();
    auto coords2 = other->GetCoordinates();
    
    if (coords1.size() != coords2.size()) return false;
    
    for (size_t i = 0; i < coords1.size(); ++i) {
        if (!coords1[i].Equals(coords2[i], tolerance)) {
            return false;
        }
    }
    
    return true;
}

bool Geometry::Disjoint(const Geometry* other) const {
    if (!other) return true;
    if (IsEmpty() || other->IsEmpty()) return true;
    
    if (EnvelopeDisjoint(other)) return true;
    
    return !Intersects(other);
}

bool Geometry::Intersects(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    if (EnvelopeDisjoint(other)) return false;
    
    auto thisType = GetGeometryType();
    auto otherType = other->GetGeometryType();
    
    if (thisType == GeomType::kPoint && otherType == GeomType::kPoint) {
        auto thisPoint = static_cast<const Point*>(this);
        auto otherPoint = static_cast<const Point*>(other);
        return thisPoint->GetCoordinate().Equals(otherPoint->GetCoordinate());
    }
    
    if (thisType == GeomType::kPoint && otherType == GeomType::kPolygon) {
        auto point = static_cast<const Point*>(this);
        auto polygon = static_cast<const Polygon*>(other);
        const LinearRing* exterior = polygon->GetExteriorRing();
        if (!exterior) return false;
        
        const Coordinate& coord = point->GetCoordinate();
        if (exterior->ContainsPoint(coord)) {
            for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
                const LinearRing* hole = polygon->GetInteriorRingN(i);
                if (hole && hole->ContainsPoint(coord)) {
                    return false;
                }
            }
            return true;
        }
        return exterior->IsPointOnBoundary(coord);
    }
    
    if (thisType == GeomType::kPolygon && otherType == GeomType::kPoint) {
        return other->Intersects(this);
    }
    
    if (thisType == GeomType::kPoint && otherType == GeomType::kLineString) {
        auto point = static_cast<const Point*>(this);
        auto line = static_cast<const LineString*>(other);
        const Coordinate& coord = point->GetCoordinate();
        
        const auto& lineCoords = line->GetCoordinates();
        for (size_t i = 0; i + 1 < lineCoords.size(); ++i) {
            double dist = line->PointToLineDistance(coord, lineCoords[i], lineCoords[i + 1]);
            if (dist <= DEFAULT_TOLERANCE) return true;
        }
        return false;
    }
    
    if (thisType == GeomType::kLineString && otherType == GeomType::kPoint) {
        return other->Intersects(this);
    }
    
    if ((thisType == GeomType::kLineString || thisType == GeomType::kPolygon) &&
        (otherType == GeomType::kLineString || otherType == GeomType::kPolygon)) {
        auto line1 = (thisType == GeomType::kLineString) 
            ? static_cast<const LineString*>(this)
            : static_cast<const LineString*>(static_cast<const Polygon*>(this)->GetExteriorRing());
        auto line2 = (otherType == GeomType::kLineString)
            ? static_cast<const LineString*>(other)
            : static_cast<const LineString*>(static_cast<const Polygon*>(other)->GetExteriorRing());
        
        if (LineStringIntersectsLineString(line1, line2)) return true;
        
        if (thisType == GeomType::kPolygon && otherType == GeomType::kPolygon) {
            auto thisPoly = static_cast<const Polygon*>(this);
            auto otherPoly = static_cast<const Polygon*>(other);
            
            Coordinate centroid = thisPoly->GetCentroid();
            if (PointInPolygon(centroid, otherPoly->GetExteriorRing())) return true;
            
            centroid = otherPoly->GetCentroid();
            if (PointInPolygon(centroid, thisPoly->GetExteriorRing())) return true;
        }
        
        return false;
    }
    
    return GetEnvelope().Intersects(other->GetEnvelope());
}

bool Geometry::Touches(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    if (EnvelopeDisjoint(other)) return false;
    
    auto thisType = GetGeometryType();
    auto otherType = other->GetGeometryType();
    
    if (thisType == GeomType::kPoint && otherType == GeomType::kPolygon) {
        auto point = static_cast<const Point*>(this);
        auto polygon = static_cast<const Polygon*>(other);
        const LinearRing* exterior = polygon->GetExteriorRing();
        if (!exterior) return false;
        
        const Coordinate& coord = point->GetCoordinate();
        if (exterior->IsPointOnBoundary(coord)) return true;
        
        for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
            const LinearRing* hole = polygon->GetInteriorRingN(i);
            if (hole && hole->IsPointOnBoundary(coord)) return true;
        }
        return false;
    }
    
    if (thisType == GeomType::kPolygon && otherType == GeomType::kPoint) {
        return other->Touches(this);
    }
    
    return false;
}

bool Geometry::Crosses(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    auto thisDim = GetDimension();
    auto otherDim = other->GetDimension();
    
    if (thisDim == Dimension::Curve && otherDim == Dimension::Surface) {
        if (!Intersects(other)) return false;
        if (Within(other)) return false;
        return true;
    }
    
    if (thisDim == Dimension::Surface && otherDim == Dimension::Curve) {
        return other->Crosses(this);
    }
    
    if (thisDim == Dimension::Curve && otherDim == Dimension::Curve) {
        if (!Intersects(other)) return false;
        if (Equals(other)) return false;
        return true;
    }
    
    return false;
}

bool Geometry::Within(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    return other->Contains(this);
}

bool Geometry::Contains(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    if (!GetEnvelope().Contains(other->GetEnvelope())) return false;
    
    auto thisType = GetGeometryType();
    auto otherType = other->GetGeometryType();
    
    if (thisType == GeomType::kPolygon && otherType == GeomType::kPoint) {
        auto polygon = static_cast<const Polygon*>(this);
        auto point = static_cast<const Point*>(other);
        const LinearRing* exterior = polygon->GetExteriorRing();
        if (!exterior) return false;
        
        const Coordinate& coord = point->GetCoordinate();
        if (!exterior->ContainsPoint(coord)) return false;
        
        for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
            const LinearRing* hole = polygon->GetInteriorRingN(i);
            if (hole && hole->ContainsPoint(coord)) {
                return false;
            }
        }
        return true;
    }
    
    if (thisType == GeomType::kPolygon && otherType == GeomType::kPolygon) {
        auto thisPoly = static_cast<const Polygon*>(this);
        auto otherPoly = static_cast<const Polygon*>(other);
        
        const LinearRing* thisExterior = thisPoly->GetExteriorRing();
        const LinearRing* otherExterior = otherPoly->GetExteriorRing();
        
        if (!thisExterior || !otherExterior) return false;
        
        if (!RingContainsRing(thisExterior, otherExterior)) return false;
        
        for (size_t i = 0; i < thisPoly->GetNumInteriorRings(); ++i) {
            const LinearRing* hole = thisPoly->GetInteriorRingN(i);
            if (hole && RingContainsRing(hole, otherExterior)) {
                return false;
            }
        }
        
        return true;
    }
    
    return false;
}

bool Geometry::Overlaps(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    if (EnvelopeDisjoint(other)) return false;
    
    auto thisDim = GetDimension();
    auto otherDim = other->GetDimension();
    
    if (thisDim != otherDim) return false;
    
    if (!Intersects(other)) return false;
    if (Equals(other)) return false;
    if (Within(other)) return false;
    if (Contains(other)) return false;
    
    return true;
}

bool Geometry::Covers(const Geometry* other) const {
    if (!other) return false;
    if (IsEmpty() || other->IsEmpty()) return false;
    
    if (!GetEnvelope().Contains(other->GetEnvelope())) return false;
    
    return Contains(other);
}

bool Geometry::CoveredBy(const Geometry* other) const {
    if (!other) return false;
    return other->Covers(this);
}

std::string Geometry::Relate(const Geometry* other) const {
    std::string matrix(9, 'F');
    
    if (!other) return matrix;
    
    bool ii = Intersects(other);
    bool ib = Touches(other);
    bool ie = Disjoint(other);
    bool bi = false;
    bool bb = ib;
    bool be = ie;
    bool ei = ie;
    bool eb = ie;
    bool ee = true;
    
    matrix[0] = ii ? 'T' : 'F';
    matrix[1] = ib ? 'T' : 'F';
    matrix[2] = ie ? 'T' : 'F';
    matrix[3] = bi ? 'T' : 'F';
    matrix[4] = bb ? 'T' : 'F';
    matrix[5] = be ? 'T' : 'F';
    matrix[6] = ei ? 'T' : 'F';
    matrix[7] = eb ? 'T' : 'F';
    matrix[8] = ee ? 'T' : 'F';
    
    return matrix;
}

bool Geometry::Relate(const Geometry* other, const std::string& pattern) const {
    if (pattern.size() != 9) return false;
    
    std::string matrix = Relate(other);
    
    for (size_t i = 0; i < 9; ++i) {
        char p = pattern[i];
        char m = matrix[i];
        
        if (p == '*') continue;
        if (p == 'T' && m == 'F') return false;
        if (p == 'F' && m == 'T') return false;
        if (std::isdigit(p) && m != p) return false;
    }
    
    return true;
}

GeomResult Geometry::Intersection(const Geometry* other, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::Union(const Geometry* other, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::Difference(const Geometry* other, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::SymmetricDifference(const Geometry* other, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::Buffer(double distance, GeometryPtr& result, int segments) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::SingleSidedBuffer(double distance, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeomResult Geometry::OffsetCurve(double distance, GeometryPtr& result) const {
    return GeomResult::kNotImplemented;
}

GeometryPtr Geometry::ConvexHull() const {
    return nullptr;
}

GeometryPtr Geometry::Boundary() const {
    return nullptr;
}

GeomResult Geometry::MakeValid(GeometryPtr& result) const {
    result = Clone();
    return GeomResult::kSuccess;
}

void Geometry::Normalize() {
}

GeometryPtr Geometry::Simplify(double tolerance) const {
    return Clone();
}

GeometryPtr Geometry::SimplifyPreserveTopology(double tolerance) const {
    return Clone();
}

GeometryPtr Geometry::Densify(double maxSegmentLength) const {
    return Clone();
}

GeometryPtr Geometry::Reverse() const {
    return Clone();
}

GeometryPtr Geometry::SnapToGrid(double gridSize) const {
    return Clone();
}

GeometryPtr Geometry::SnapTo(const Geometry* reference, double tolerance) const {
    return Clone();
}

GeometryPtr Geometry::AffineTransform(double a, double b, double c,
                                        double d, double e, double f) const {
    return Clone();
}

GeometryPtr Geometry::Translate(double dx, double dy) const {
    return AffineTransform(1, 0, dx, 0, 1, dy);
}

GeometryPtr Geometry::Scale(double scaleX, double scaleY) const {
    return AffineTransform(scaleX, 0, 0, 0, scaleY, 0);
}

GeometryPtr Geometry::Rotate(double angle) const {
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    return AffineTransform(cosA, -sinA, 0, sinA, cosA, 0);
}

std::string Geometry::AsHex() const {
    auto wkb = AsBinary();
    std::ostringstream oss;
    for (uint8_t byte : wkb) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

std::string Geometry::AsGeoJSON(int precision) const {
    return "{}";
}

std::string Geometry::AsGML() const {
    return "";
}

std::string Geometry::AsKML() const {
    return "";
}

void Geometry::Apply(GeometryVisitor& visitor) {
    visitor.Visit(this);
}

void Geometry::Apply(GeometryConstVisitor& visitor) const {
    visitor.Visit(this);
}

GeometryPtr Geometry::GetLinearGeometry() const {
    return Clone();
}

const Geometry* Geometry::GetGeometryN(size_t index) const {
    if (index == 0) return this;
    return nullptr;
}

GeometryPtr Geometry::GetShortestLine(const Geometry* other) const {
    return nullptr;
}

GeometryPtr Geometry::GetMaximumDistanceLine(const Geometry* other) const {
    return nullptr;
}

std::pair<Coordinate, double> Geometry::GetMinimumBoundingCircle() const {
    return {Coordinate(), 0.0};
}

GeometryPtr Geometry::GetMinimumBoundingRectangle() const {
    return nullptr;
}

Coordinate Geometry::ComputeCentroid() const {
    return Coordinate();
}

bool Geometry::EnvelopeDisjoint(const Geometry* other) const {
    return !GetEnvelope().Intersects(other->GetEnvelope());
}

bool Geometry::EnvelopeIntersects(const Geometry* other) const {
    return GetEnvelope().Intersects(other->GetEnvelope());
}

}
