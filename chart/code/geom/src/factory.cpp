#include "ogc/factory.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <stdexcept>

namespace ogc {

class GeometryFactory::WKTReader {
public:
    explicit WKTReader(const std::string& wkt) 
        : m_wkt(wkt), m_pos(0) {}
    
    GeometryPtr Parse() {
        SkipWhitespace();
        std::string type = ReadWord();
        std::transform(type.begin(), type.end(), type.begin(), ::toupper);
        
        SkipWhitespace();
        
        bool hasZ = false;
        bool hasM = false;
        
        if (m_pos < m_wkt.size()) {
            std::string next = PeekWord();
            std::transform(next.begin(), next.end(), next.begin(), ::toupper);
            
            if (next == "Z") {
                hasZ = true;
                ReadWord();
                SkipWhitespace();
            } else if (next == "M") {
                hasM = true;
                ReadWord();
                SkipWhitespace();
            } else if (next == "ZM") {
                hasZ = true;
                hasM = true;
                ReadWord();
                SkipWhitespace();
            }
        }
        
        if (type == "POINT") {
            return ParsePoint(hasZ, hasM);
        } else if (type == "LINESTRING") {
            return ParseLineString(hasZ, hasM);
        } else if (type == "POLYGON") {
            return ParsePolygon(hasZ, hasM);
        } else if (type == "MULTIPOINT") {
            return ParseMultiPoint(hasZ, hasM);
        }
        
        return nullptr;
    }
    
private:
    const std::string& m_wkt;
    size_t m_pos;
    
    void SkipWhitespace() {
        while (m_pos < m_wkt.size() && std::isspace(m_wkt[m_pos])) {
            m_pos++;
        }
    }
    
    std::string PeekWord() {
        size_t savedPos = m_pos;
        std::string word = ReadWord();
        m_pos = savedPos;
        return word;
    }
    
    std::string ReadWord() {
        SkipWhitespace();
        std::string word;
        while (m_pos < m_wkt.size() && 
               (std::isalnum(m_wkt[m_pos]) || m_wkt[m_pos] == '_')) {
            word += m_wkt[m_pos++];
        }
        return word;
    }
    
    bool Match(char c) {
        SkipWhitespace();
        if (m_pos < m_wkt.size() && m_wkt[m_pos] == c) {
            m_pos++;
            return true;
        }
        return false;
    }
    
    bool Expect(char c) {
        if (!Match(c)) {
            throw std::runtime_error("Expected character");
        }
        return true;
    }
    
    double ReadNumber() {
        SkipWhitespace();
        std::string num;
        
        if (m_pos < m_wkt.size() && (m_wkt[m_pos] == '-' || m_wkt[m_pos] == '+')) {
            num += m_wkt[m_pos++];
        }
        
        while (m_pos < m_wkt.size() && (std::isdigit(m_wkt[m_pos]) || m_wkt[m_pos] == '.')) {
            num += m_wkt[m_pos++];
        }
        
        if (m_pos < m_wkt.size() && (m_wkt[m_pos] == 'e' || m_wkt[m_pos] == 'E')) {
            num += m_wkt[m_pos++];
            if (m_pos < m_wkt.size() && (m_wkt[m_pos] == '-' || m_wkt[m_pos] == '+')) {
                num += m_wkt[m_pos++];
            }
            while (m_pos < m_wkt.size() && std::isdigit(m_wkt[m_pos])) {
                num += m_wkt[m_pos++];
            }
        }
        
        return std::stod(num);
    }
    
    Coordinate ReadCoordinate(bool hasZ, bool hasM) {
        double x = ReadNumber();
        double y = ReadNumber();
        double z = std::numeric_limits<double>::quiet_NaN();
        double m = std::numeric_limits<double>::quiet_NaN();
        
        SkipWhitespace();
        if (hasZ || (!hasM && m_pos < m_wkt.size() && 
                     (std::isdigit(m_wkt[m_pos]) || m_wkt[m_pos] == '-' || m_wkt[m_pos] == '+'))) {
            if (m_pos < m_wkt.size() && m_wkt[m_pos] != ',' && m_wkt[m_pos] != ')') {
                z = ReadNumber();
            }
        }
        
        SkipWhitespace();
        if (hasM) {
            if (m_pos < m_wkt.size() && m_wkt[m_pos] != ',' && m_wkt[m_pos] != ')') {
                m = ReadNumber();
            }
        }
        
        return Coordinate(x, y, z, m);
    }
    
    bool IsEmpty() {
        SkipWhitespace();
        std::string word = PeekWord();
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);
        if (word == "EMPTY") {
            ReadWord();
            return true;
        }
        return false;
    }
    
    GeometryPtr ParsePoint(bool hasZ, bool hasM) {
        if (IsEmpty()) {
            return Point::Create(Coordinate::Empty());
        }
        
        Expect('(');
        Coordinate coord = ReadCoordinate(hasZ, hasM);
        Expect(')');
        
        return Point::Create(coord);
    }
    
    GeometryPtr ParseLineString(bool hasZ, bool hasM) {
        if (IsEmpty()) {
            return LineString::Create();
        }
        
        Expect('(');
        CoordinateList coords;
        
        coords.push_back(ReadCoordinate(hasZ, hasM));
        
        while (Match(',')) {
            coords.push_back(ReadCoordinate(hasZ, hasM));
        }
        
        Expect(')');
        return LineString::Create(coords);
    }
    
    GeometryPtr ParsePolygon(bool hasZ, bool hasM) {
        if (IsEmpty()) {
            return Polygon::Create();
        }
        
        Expect('(');
        
        CoordinateList exteriorCoords;
        Expect('(');
        exteriorCoords.push_back(ReadCoordinate(hasZ, hasM));
        while (Match(',')) {
            exteriorCoords.push_back(ReadCoordinate(hasZ, hasM));
        }
        Expect(')');
        
        auto exteriorRing = LinearRing::Create(exteriorCoords);
        auto polygon = Polygon::Create(std::move(exteriorRing));
        
        while (Match(',')) {
            CoordinateList interiorCoords;
            Expect('(');
            interiorCoords.push_back(ReadCoordinate(hasZ, hasM));
            while (Match(',')) {
                interiorCoords.push_back(ReadCoordinate(hasZ, hasM));
            }
            Expect(')');
            
            auto interiorRing = LinearRing::Create(interiorCoords);
            polygon->AddInteriorRing(std::move(interiorRing));
        }
        
        Expect(')');
        return polygon;
    }
    
    GeometryPtr ParseMultiPoint(bool hasZ, bool hasM) {
        return nullptr;
    }
};

GeometryFactory& GeometryFactory::GetInstance() {
    static GeometryFactory instance;
    return instance;
}

GeomResult GeometryFactory::FromWKT(const std::string& wkt, GeometryPtr& result) {
    try {
        WKTReader reader(wkt);
        result = reader.Parse();
        if (result) {
            result->SetSRID(m_defaultSRID);
            return GeomResult::kSuccess;
        }
        return GeomResult::kParseError;
    } catch (const std::exception& e) {
        return GeomResult::kParseError;
    }
}

PointPtr GeometryFactory::CreatePointFromWKT(const std::string& wkt) {
    GeometryPtr result;
    if (FromWKT(wkt, result) == GeomResult::kSuccess && 
        result->GetGeometryType() == GeomType::kPoint) {
        return PointPtr(static_cast<Point*>(result.release()));
    }
    return nullptr;
}

LineStringPtr GeometryFactory::CreateLineStringFromWKT(const std::string& wkt) {
    GeometryPtr result;
    if (FromWKT(wkt, result) == GeomResult::kSuccess && 
        result->GetGeometryType() == GeomType::kLineString) {
        return LineStringPtr(static_cast<LineString*>(result.release()));
    }
    return nullptr;
}

PolygonPtr GeometryFactory::CreatePolygonFromWKT(const std::string& wkt) {
    GeometryPtr result;
    if (FromWKT(wkt, result) == GeomResult::kSuccess && 
        result->GetGeometryType() == GeomType::kPolygon) {
        return PolygonPtr(static_cast<Polygon*>(result.release()));
    }
    return nullptr;
}

GeomResult GeometryFactory::FromWKB(const std::vector<uint8_t>& wkb, GeometryPtr& result) {
    return FromWKB(wkb.data(), wkb.size(), result);
}

GeomResult GeometryFactory::FromWKB(const uint8_t* data, size_t size, GeometryPtr& result) {
    return GeomResult::kNotImplemented;
}

GeomResult GeometryFactory::FromGeoJSON(const std::string& json, GeometryPtr& result) {
    return GeomResult::kNotImplemented;
}

PointPtr GeometryFactory::CreatePoint(double x, double y) {
    auto point = Point::Create(x, y);
    point->SetSRID(m_defaultSRID);
    return point;
}

PointPtr GeometryFactory::CreatePoint(double x, double y, double z) {
    auto point = Point::Create(x, y, z);
    point->SetSRID(m_defaultSRID);
    return point;
}

PointPtr GeometryFactory::CreatePoint(const Coordinate& coord) {
    auto point = Point::Create(coord);
    point->SetSRID(m_defaultSRID);
    return point;
}

LineStringPtr GeometryFactory::CreateLineString(const CoordinateList& coords) {
    auto line = LineString::Create(coords);
    line->SetSRID(m_defaultSRID);
    return line;
}

LinearRingPtr GeometryFactory::CreateLinearRing(const CoordinateList& coords) {
    auto ring = LinearRing::Create(coords);
    ring->SetSRID(m_defaultSRID);
    return ring;
}

PolygonPtr GeometryFactory::CreatePolygon(const CoordinateList& exteriorRing) {
    auto ring = CreateLinearRing(exteriorRing);
    auto polygon = Polygon::Create(std::move(ring));
    polygon->SetSRID(m_defaultSRID);
    return polygon;
}

PolygonPtr GeometryFactory::CreatePolygon(const std::vector<CoordinateList>& rings) {
    if (rings.empty()) return Polygon::Create();
    
    auto exteriorRing = CreateLinearRing(rings[0]);
    auto polygon = Polygon::Create(std::move(exteriorRing));
    
    for (size_t i = 1; i < rings.size(); ++i) {
        auto interiorRing = CreateLinearRing(rings[i]);
        polygon->AddInteriorRing(std::move(interiorRing));
    }
    
    polygon->SetSRID(m_defaultSRID);
    return polygon;
}

MultiPointPtr GeometryFactory::CreateMultiPoint(const CoordinateList& coords) {
    return nullptr;
}

PolygonPtr GeometryFactory::CreateRectangle(double minX, double minY, double maxX, double maxY) {
    auto polygon = Polygon::CreateRectangle(minX, minY, maxX, maxY);
    polygon->SetSRID(m_defaultSRID);
    return polygon;
}

PolygonPtr GeometryFactory::CreateCircle(double centerX, double centerY, double radius, int segments) {
    auto polygon = Polygon::CreateCircle(centerX, centerY, radius, segments);
    polygon->SetSRID(m_defaultSRID);
    return polygon;
}

PolygonPtr GeometryFactory::CreateRegularPolygon(double centerX, double centerY, double radius, int sides) {
    auto polygon = Polygon::CreateRegularPolygon(centerX, centerY, radius, sides);
    polygon->SetSRID(m_defaultSRID);
    return polygon;
}

PolygonPtr GeometryFactory::CreateTriangle(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3) {
    auto polygon = Polygon::CreateTriangle(p1, p2, p3);
    polygon->SetSRID(m_defaultSRID);
    return polygon;
}

GeometryPtr GeometryFactory::CreateEmptyGeometry(GeomType type) {
    switch (type) {
        case GeomType::kPoint:
            return Point::Create(Coordinate::Empty());
        case GeomType::kLineString:
            return LineString::Create();
        case GeomType::kPolygon:
            return Polygon::Create();
        default:
            return nullptr;
    }
}

}
