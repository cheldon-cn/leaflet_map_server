#include "ogc/geom/linestring.h"
#include "ogc/geom/visitor.h"
#include "ogc/geom/serialization_utils.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace ogc {

LineStringPtr LineString::Create() {
    return LineStringPtr(new LineString());
}

LineStringPtr LineString::Create(const CoordinateList& coords) {
    return LineStringPtr(new LineString(coords));
}

LineStringPtr LineString::Create(CoordinateList&& coords) {
    auto line = new LineString();
    line->m_coords = std::move(coords);
    return LineStringPtr(line);
}

bool LineString::Is3D() const noexcept {
    for (const auto& coord : m_coords) {
        if (coord.Is3D()) return true;
    }
    return false;
}

bool LineString::IsMeasured() const noexcept {
    for (const auto& coord : m_coords) {
        if (coord.IsMeasured()) return true;
    }
    return false;
}

int LineString::GetCoordinateDimension() const noexcept {
    if (m_coords.empty()) return 2;
    int dim = 2;
    if (Is3D()) dim++;
    if (IsMeasured()) dim++;
    return dim;
}

bool LineString::IsClosed() const noexcept {
    if (m_coords.size() < 2) return false;
    return m_coords.front().Equals(m_coords.back());
}

bool LineString::IsRing() const noexcept {
    return IsClosed() && IsSimple();
}

Coordinate LineString::GetCoordinateN(size_t index) const {
    if (index >= m_coords.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Index out of range");
    }
    return m_coords[index];
}

Coordinate LineString::GetPointN(size_t index) const {
    return GetCoordinateN(index);
}

Coordinate LineString::GetStartPoint() const {
    if (m_coords.empty()) return Coordinate::Empty();
    return m_coords.front();
}

Coordinate LineString::GetEndPoint() const {
    if (m_coords.empty()) return Coordinate::Empty();
    return m_coords.back();
}

void LineString::SetCoordinates(const CoordinateList& coords) {
    m_coords = coords;
    InvalidateCache();
}

void LineString::SetCoordinates(CoordinateList&& coords) {
    m_coords = std::move(coords);
    InvalidateCache();
}

void LineString::AddPoint(const Coordinate& coord) {
    m_coords.push_back(coord);
    InvalidateCache();
}

void LineString::InsertPoint(size_t index, const Coordinate& coord) {
    if (index > m_coords.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Index out of range");
    }
    m_coords.insert(m_coords.begin() + index, coord);
    InvalidateCache();
}

void LineString::RemovePoint(size_t index) {
    if (index >= m_coords.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Index out of range");
    }
    m_coords.erase(m_coords.begin() + index);
    InvalidateCache();
}

void LineString::Clear() {
    m_coords.clear();
    InvalidateCache();
}

LineString::Segment LineString::GetSegment(size_t index) const {
    if (index >= GetNumSegments()) {
        GLM_THROW(GeomResult::kOutOfRange, "Segment index out of range");
    }
    Segment seg;
    seg.start = m_coords[index];
    seg.end = m_coords[index + 1];
    seg.length = seg.start.Distance(seg.end);
    return seg;
}

size_t LineString::GetNumSegments() const noexcept {
    return m_coords.size() > 1 ? m_coords.size() - 1 : 0;
}

double LineString::Length() const {
    double len = 0.0;
    for (size_t i = 1; i < m_coords.size(); ++i) {
        len += m_coords[i - 1].Distance(m_coords[i]);
    }
    return len;
}

double LineString::Length3D() const {
    double len = 0.0;
    for (size_t i = 1; i < m_coords.size(); ++i) {
        len += m_coords[i - 1].Distance3D(m_coords[i]);
    }
    return len;
}

std::vector<double> LineString::GetCumulativeLength() const {
    std::vector<double> lengths;
    lengths.push_back(0.0);
    double cumLen = 0.0;
    for (size_t i = 1; i < m_coords.size(); ++i) {
        cumLen += m_coords[i - 1].Distance(m_coords[i]);
        lengths.push_back(cumLen);
    }
    return lengths;
}

double LineString::GetSegmentLengthPercentage(size_t index) const {
    double total = Length();
    if (total == 0.0) return 0.0;
    return GetSegment(index).length / total;
}

Coordinate LineString::Interpolate(double distance) const {
    if (m_coords.empty()) return Coordinate::Empty();
    if (distance <= 0) return m_coords.front();
    
    double cumDist = 0.0;
    for (size_t i = 1; i < m_coords.size(); ++i) {
        double segLen = m_coords[i - 1].Distance(m_coords[i]);
        if (cumDist + segLen >= distance) {
            double ratio = (distance - cumDist) / segLen;
            return m_coords[i - 1] + (m_coords[i] - m_coords[i - 1]) * ratio;
        }
        cumDist += segLen;
    }
    return m_coords.back();
}

double LineString::LocatePoint(const Coordinate& point) const {
    double totalLen = Length();
    if (totalLen == 0.0) return 0.0;
    
    double minDist = std::numeric_limits<double>::max();
    double location = 0.0;
    double cumDist = 0.0;
    
    for (size_t i = 1; i < m_coords.size(); ++i) {
        double segLen = m_coords[i - 1].Distance(m_coords[i]);
        double projLoc = ProjectPointOnSegment(point, m_coords[i - 1], m_coords[i]);
        double dist = point.Distance(InterpolateOnSegment(projLoc, m_coords[i - 1], m_coords[i]));
        
        if (dist < minDist) {
            minDist = dist;
            location = cumDist + projLoc * segLen;
        }
        cumDist += segLen;
    }
    
    return location / totalLen;
}

LineStringPtr LineString::GetSubLine(double startDistance, double endDistance) const {
    auto result = Create();
    double totalLen = Length();
    if (totalLen == 0.0) return result;
    
    startDistance = std::max(0.0, std::min(totalLen, startDistance));
    endDistance = std::max(startDistance, std::min(totalLen, endDistance));
    
    result->AddPoint(Interpolate(startDistance));
    
    double cumDist = 0.0;
    for (size_t i = 1; i < m_coords.size(); ++i) {
        double segLen = m_coords[i - 1].Distance(m_coords[i]);
        double segStart = cumDist;
        double segEnd = cumDist + segLen;
        
        if (segEnd > startDistance && segStart < endDistance) {
            if (segStart >= startDistance && segStart <= endDistance) {
                result->AddPoint(m_coords[i - 1]);
            }
            if (segEnd >= startDistance && segEnd <= endDistance) {
                result->AddPoint(m_coords[i]);
            }
        }
        cumDist += segLen;
    }
    
    result->AddPoint(Interpolate(endDistance));
    return result;
}

std::pair<LineStringPtr, LineStringPtr> LineString::Split(const Coordinate& point) const {
    double loc = LocatePoint(point);
    return {GetSubLine(0, loc * Length()), GetSubLine(loc * Length(), Length())};
}

std::vector<LineStringPtr> LineString::SplitAtDistances(const std::vector<double>& distances) const {
    std::vector<LineStringPtr> result;
    double prev = 0.0;
    for (double dist : distances) {
        result.push_back(GetSubLine(prev, dist));
        prev = dist;
    }
    result.push_back(GetSubLine(prev, Length()));
    return result;
}

LineStringPtr LineString::Offset(double distance) const {
    return Create();
}

double LineString::GetCurvatureAt(size_t index) const {
    if (index == 0 || index >= m_coords.size() - 1) return 0.0;
    
    Coordinate v1 = m_coords[index] - m_coords[index - 1];
    Coordinate v2 = m_coords[index + 1] - m_coords[index];
    
    double cross = v1.Cross(v2).z;
    double dot = v1.Dot(v2);
    double angle = std::atan2(cross, dot);
    
    return std::abs(angle);
}

double LineString::GetTotalCurvature() const {
    double total = 0.0;
    for (size_t i = 1; i + 1 < m_coords.size(); ++i) {
        total += GetCurvatureAt(i);
    }
    return total;
}

double LineString::GetBearingAt(size_t index) const {
    if (index >= m_coords.size() - 1) return 0.0;
    
    double dx = m_coords[index + 1].x - m_coords[index].x;
    double dy = m_coords[index + 1].y - m_coords[index].y;
    return std::atan2(dy, dx);
}

void LineString::RemoveDuplicatePoints(double tolerance) {
    if (m_coords.size() < 2) return;
    
    CoordinateList filtered;
    filtered.push_back(m_coords.front());
    
    for (size_t i = 1; i < m_coords.size(); ++i) {
        if (!m_coords[i].Equals(filtered.back(), tolerance)) {
            filtered.push_back(m_coords[i]);
        }
    }
    
    m_coords = std::move(filtered);
    InvalidateCache();
}

LineStringPtr LineString::Smooth(int iterations) const {
    auto result = Create(m_coords);
    
    for (int iter = 0; iter < iterations; ++iter) {
        CoordinateList smoothed;
        smoothed.push_back(result->m_coords.front());
        
        for (size_t i = 1; i + 1 < result->m_coords.size(); ++i) {
            Coordinate avg = (result->m_coords[i - 1] + result->m_coords[i] + result->m_coords[i + 1]) * (1.0 / 3.0);
            smoothed.push_back(avg);
        }
        
        smoothed.push_back(result->m_coords.back());
        result->m_coords = std::move(smoothed);
    }
    
    return result;
}

bool LineString::IsStraight(double tolerance) const {
    if (m_coords.size() < 3) return true;
    
    Coordinate start = m_coords.front();
    Coordinate end = m_coords.back();
    
    for (size_t i = 1; i + 1 < m_coords.size(); ++i) {
        double dist = PointToLineDistance(m_coords[i], start, end);
        if (dist > tolerance) return false;
    }
    
    return true;
}

std::string LineString::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "LINESTRING EMPTY";
    }
    
    oss << "LINESTRING(";
    for (size_t i = 0; i < m_coords.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << m_coords[i].x << " " << m_coords[i].y;
        if (m_coords[i].Is3D()) oss << " " << m_coords[i].z;
        if (m_coords[i].IsMeasured()) oss << " " << m_coords[i].m;
    }
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> LineString::AsBinary() const {
    std::vector<uint8_t> wkb;
    
    if (IsEmpty()) {
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(GeomType::kLineString));
        return wkb;
    }
    
    wkb::WriteByteOrder(wkb);
    wkb::WriteGeometryType(wkb, static_cast<uint32_t>(GeomType::kLineString), Is3D(), IsMeasured());
    wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(m_coords.size()));
    
    for (const auto& coord : m_coords) {
        wkb::WriteCoordinate(wkb, coord.x, coord.y, coord.z, coord.m, Is3D(), IsMeasured());
    }
    
    return wkb;
}

GeometryPtr LineString::Clone() const {
    return Create(m_coords);
}

GeometryPtr LineString::CloneEmpty() const {
    return Create();
}

Envelope LineString::ComputeEnvelope() const {
    return Envelope(m_coords);
}

Coordinate LineString::ComputeCentroid() const {
    if (m_coords.empty()) return Coordinate::Empty();
    
    double sumX = 0.0, sumY = 0.0;
    size_t count = 0;
    for (const auto& coord : m_coords) {
        if (!coord.IsEmpty()) {
            sumX += coord.x;
            sumY += coord.y;
            ++count;
        }
    }
    if (count == 0) return Coordinate::Empty();
    return Coordinate(sumX / count, sumY / count);
}

double LineString::ProjectPointOnSegment(const Coordinate& point,
                                           const Coordinate& segStart,
                                           const Coordinate& segEnd) const {
    Coordinate segVec = segEnd - segStart;
    double segLenSq = segVec.Dot(segVec);
    if (segLenSq < DEFAULT_TOLERANCE * DEFAULT_TOLERANCE) return 0.0;
    
    Coordinate pointVec = point - segStart;
    double t = pointVec.Dot(segVec) / segLenSq;
    return std::max(0.0, std::min(1.0, t));
}

Coordinate LineString::InterpolateOnSegment(double t,
                                              const Coordinate& segStart,
                                              const Coordinate& segEnd) const {
    return segStart + (segEnd - segStart) * t;
}

double LineString::PointToLineDistance(const Coordinate& point,
                                         const Coordinate& lineStart,
                                         const Coordinate& lineEnd) const {
    double t = ProjectPointOnSegment(point, lineStart, lineEnd);
    Coordinate proj = InterpolateOnSegment(t, lineStart, lineEnd);
    return point.Distance(proj);
}

std::string LineString::AsGeoJSON(int precision) const {
    if (IsEmpty()) {
        return "{\"type\":\"LineString\",\"coordinates\":[]}";
    }
    
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < m_coords.size(); ++i) {
        if (i > 0) oss << ",";
        if (IsMeasured()) {
            oss << geojson::Coordinate4D(m_coords[i].x, m_coords[i].y, m_coords[i].z, m_coords[i].m, precision);
        } else if (Is3D()) {
            oss << geojson::Coordinate3D(m_coords[i].x, m_coords[i].y, m_coords[i].z, precision);
        } else {
            oss << geojson::Coordinate(m_coords[i].x, m_coords[i].y, precision);
        }
    }
    oss << "]";
    
    return geojson::LineString(oss.str(), precision);
}

std::string LineString::AsGML() const {
    if (IsEmpty()) {
        return "<gml:LineString/>";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < m_coords.size(); ++i) {
        if (i > 0) oss << " ";
        if (Is3D()) {
            oss << gml::Number(m_coords[i].x) << " " 
                << gml::Number(m_coords[i].y) << " " 
                << gml::Number(m_coords[i].z);
        } else {
            oss << gml::Number(m_coords[i].x) << " " 
                << gml::Number(m_coords[i].y);
        }
    }
    
    return gml::LineString(oss.str());
}

std::string LineString::AsKML() const {
    if (IsEmpty()) {
        return "<LineString/>";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < m_coords.size(); ++i) {
        if (i > 0) oss << " ";
        if (Is3D()) {
            oss << kml::Coordinate3D(m_coords[i].x, m_coords[i].y, m_coords[i].z);
        } else {
            oss << kml::Coordinate(m_coords[i].x, m_coords[i].y);
        }
    }
    
    return kml::LineString(oss.str());
}

void LineString::Apply(GeometryVisitor& visitor) {
    visitor.VisitLineString(this);
}

void LineString::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitLineString(this);
}

}
