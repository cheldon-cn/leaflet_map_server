#include "ogc/multilinestring.h"
#include "ogc/visitor.h"
#include "ogc/serialization_utils.h"
#include <sstream>
#include <iomanip>

namespace ogc {

MultiLineStringPtr MultiLineString::Create() {
    return MultiLineStringPtr(new MultiLineString());
}

MultiLineStringPtr MultiLineString::Create(std::vector<LineStringPtr> lines) {
    auto mls = new MultiLineString();
    mls->m_lines = std::move(lines);
    return MultiLineStringPtr(mls);
}

int MultiLineString::GetCoordinateDimension() const noexcept {
    if (m_lines.empty()) return 2;
    return m_lines.front()->GetCoordinateDimension();
}

bool MultiLineString::Is3D() const noexcept {
    for (const auto& line : m_lines) {
        if (line->Is3D()) return true;
    }
    return false;
}

bool MultiLineString::IsMeasured() const noexcept {
    for (const auto& line : m_lines) {
        if (line->IsMeasured()) return true;
    }
    return false;
}

const Geometry* MultiLineString::GetGeometryN(size_t index) const {
    if (index >= m_lines.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "Geometry index out of range");
    }
    return m_lines[index].get();
}

void MultiLineString::AddLineString(LineStringPtr line) {
    m_lines.push_back(std::move(line));
    InvalidateCache();
}

void MultiLineString::RemoveLineString(size_t index) {
    if (index >= m_lines.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "LineString index out of range");
    }
    m_lines.erase(m_lines.begin() + index);
    InvalidateCache();
}

void MultiLineString::Clear() {
    m_lines.clear();
    InvalidateCache();
}

const LineString* MultiLineString::GetLineStringN(size_t index) const {
    if (index >= m_lines.size()) {
        GLM_THROW(GeomResult::kOutOfRange, "LineString index out of range");
    }
    return m_lines[index].get();
}

double MultiLineString::Length() const {
    double total = 0.0;
    for (const auto& line : m_lines) {
        total += line->Length();
    }
    return total;
}

double MultiLineString::Length3D() const {
    double total = 0.0;
    for (const auto& line : m_lines) {
        total += line->Length3D();
    }
    return total;
}

bool MultiLineString::IsClosed() const {
    for (const auto& line : m_lines) {
        if (!line->IsClosed()) return false;
    }
    return !m_lines.empty();
}

MultiLineStringPtr MultiLineString::Merge() const {
    return Create();
}

size_t MultiLineString::GetNumCoordinates() const noexcept {
    size_t count = 0;
    for (const auto& line : m_lines) {
        count += line->GetNumCoordinates();
    }
    return count;
}

Coordinate MultiLineString::GetCoordinateN(size_t index) const {
    size_t count = 0;
    for (const auto& line : m_lines) {
        size_t n = line->GetNumCoordinates();
        if (index < count + n) {
            return line->GetCoordinateN(index - count);
        }
        count += n;
    }
    GLM_THROW(GeomResult::kOutOfRange, "Coordinate index out of range");
    return Coordinate();
}

CoordinateList MultiLineString::GetCoordinates() const {
    CoordinateList coords;
    for (const auto& line : m_lines) {
        auto lineCoords = line->GetCoordinates();
        coords.insert(coords.end(), lineCoords.begin(), lineCoords.end());
    }
    return coords;
}

std::string MultiLineString::AsText(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    
    if (IsEmpty()) {
        return "MULTILINESTRING EMPTY";
    }
    
    oss << "MULTILINESTRING(";
    for (size_t i = 0; i < m_lines.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "(";
        const auto& coords = m_lines[i]->GetCoordinates();
        for (size_t j = 0; j < coords.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << coords[j].x << " " << coords[j].y;
            if (coords[j].Is3D()) oss << " " << coords[j].z;
        }
        oss << ")";
    }
    oss << ")";
    return oss.str();
}

std::vector<uint8_t> MultiLineString::AsBinary() const {
    std::vector<uint8_t> wkb;
    
    if (IsEmpty()) {
        wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(GeomType::kMultiLineString));
        return wkb;
    }
    
    wkb::WriteByteOrder(wkb);
    wkb::WriteGeometryType(wkb, static_cast<uint32_t>(GeomType::kMultiLineString), Is3D(), IsMeasured());
    wkb::WriteUInt32LE(wkb, static_cast<uint32_t>(m_lines.size()));
    
    for (const auto& line : m_lines) {
        auto lineWkb = line->AsBinary();
        wkb.insert(wkb.end(), lineWkb.begin(), lineWkb.end());
    }
    
    return wkb;
}

GeometryPtr MultiLineString::Clone() const {
    auto clone = Create();
    for (const auto& line : m_lines) {
        clone->m_lines.push_back(LineStringPtr(
            static_cast<LineString*>(line->Clone().release())));
    }
    return clone;
}

GeometryPtr MultiLineString::CloneEmpty() const {
    return Create();
}

std::string MultiLineString::AsGeoJSON(int precision) const {
    if (IsEmpty()) {
        return "{\"type\":\"MultiLineString\",\"coordinates\":[]}";
    }
    
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < m_lines.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& coords = m_lines[i]->GetCoordinates();
        oss << "[";
        for (size_t j = 0; j < coords.size(); ++j) {
            if (j > 0) oss << ",";
            if (IsMeasured()) {
                oss << geojson::Coordinate4D(coords[j].x, coords[j].y, coords[j].z, coords[j].m, precision);
            } else if (Is3D()) {
                oss << geojson::Coordinate3D(coords[j].x, coords[j].y, coords[j].z, precision);
            } else {
                oss << geojson::Coordinate(coords[j].x, coords[j].y, precision);
            }
        }
        oss << "]";
    }
    oss << "]";
    
    return geojson::MultiLineString(oss.str(), precision);
}

std::string MultiLineString::AsGML() const {
    if (IsEmpty()) {
        return "<gml:MultiCurve/>";
    }
    
    std::ostringstream oss;
    for (const auto& line : m_lines) {
        oss << gml::CurveMember(line->AsGML());
    }
    
    return gml::MultiLineString(oss.str());
}

std::string MultiLineString::AsKML() const {
    if (IsEmpty()) {
        return "<MultiGeometry/>";
    }
    
    std::ostringstream oss;
    for (const auto& line : m_lines) {
        oss << line->AsKML();
    }
    
    return kml::MultiGeometry(oss.str());
}

void MultiLineString::Apply(GeometryVisitor& visitor) {
    visitor.VisitMultiLineString(this);
}

void MultiLineString::Apply(GeometryConstVisitor& visitor) const {
    visitor.VisitMultiLineString(this);
}

Envelope MultiLineString::ComputeEnvelope() const {
    Envelope env;
    for (const auto& line : m_lines) {
        env.ExpandToInclude(line->GetEnvelope());
    }
    return env;
}

Coordinate MultiLineString::ComputeCentroid() const {
    if (m_lines.empty()) return Coordinate::Empty();
    auto coords = GetCoordinates();
    if (coords.empty()) return Coordinate::Empty();
    
    double sumX = 0.0, sumY = 0.0;
    for (const auto& coord : coords) {
        sumX += coord.x;
        sumY += coord.y;
    }
    return Coordinate(sumX / coords.size(), sumY / coords.size());
}

}
