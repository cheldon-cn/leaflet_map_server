#include "ogc/geom/factory.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipoint.h"
#include "ogc/geom/multilinestring.h"
#include "ogc/geom/multipolygon.h"
#include "ogc/geom/geometrycollection.h"
#include <sstream>
#include <cctype>
#include <cstring>
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
        } else if (type == "MULTILINESTRING") {
            return ParseMultiLineString(hasZ, hasM);
        } else if (type == "MULTIPOLYGON") {
            return ParseMultiPolygon(hasZ, hasM);
        } else if (type == "GEOMETRYCOLLECTION") {
            return ParseGeometryCollection(hasZ, hasM);
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
        if (!Expect('(')) return nullptr;
        
        auto multiPoint = MultiPoint::Create();
        
        while (true) {
            SkipWhitespace();
            if (m_pos >= m_wkt.size()) break;
            
            if (m_wkt[m_pos] == ')') {
                m_pos++;
                break;
            }
            
            SkipWhitespace();
            if (m_wkt[m_pos] == '(') {
                auto point = ParsePoint(hasZ, hasM);
                if (point) {
                    multiPoint->AddPoint(PointPtr(dynamic_cast<Point*>(point.release())));
                }
                SkipWhitespace();
                if (m_pos < m_wkt.size() && m_wkt[m_pos] == ',') {
                    m_pos++;
                }
            } else {
                auto point = ParsePoint(hasZ, hasM);
                if (point) {
                    multiPoint->AddPoint(PointPtr(dynamic_cast<Point*>(point.release())));
                }
                SkipWhitespace();
                if (m_pos < m_wkt.size() && m_wkt[m_pos] == ',') {
                    m_pos++;
                } else {
                    break;
                }
            }
        }
        
        return multiPoint;
    }
    
    GeometryPtr ParseMultiLineString(bool hasZ, bool hasM) {
        if (!Expect('(')) return nullptr;
        
        auto multiLine = MultiLineString::Create();
        
        while (true) {
            SkipWhitespace();
            if (m_pos >= m_wkt.size()) break;
            
            if (m_wkt[m_pos] == ')') {
                m_pos++;
                break;
            }
            
            auto line = ParseLineString(hasZ, hasM);
            if (line) {
                multiLine->AddLineString(LineStringPtr(dynamic_cast<LineString*>(line.release())));
            }
            
            SkipWhitespace();
            if (m_pos < m_wkt.size() && m_wkt[m_pos] == ',') {
                m_pos++;
            } else {
                break;
            }
        }
        
        Expect(')');
        return multiLine;
    }
    
    GeometryPtr ParseMultiPolygon(bool hasZ, bool hasM) {
        if (!Expect('(')) return nullptr;
        
        auto multiPoly = MultiPolygon::Create();
        
        while (true) {
            SkipWhitespace();
            if (m_pos >= m_wkt.size()) break;
            
            if (m_wkt[m_pos] == ')') {
                m_pos++;
                break;
            }
            
            auto poly = ParsePolygon(hasZ, hasM);
            if (poly) {
                multiPoly->AddPolygon(PolygonPtr(dynamic_cast<Polygon*>(poly.release())));
            }
            
            SkipWhitespace();
            if (m_pos < m_wkt.size() && m_wkt[m_pos] == ',') {
                m_pos++;
            } else {
                break;
            }
        }
        
        Expect(')');
        return multiPoly;
    }
    
    GeometryPtr ParseGeometryCollection(bool hasZ, bool hasM) {
        if (!Expect('(')) return nullptr;
        
        auto collection = GeometryCollection::Create();
        
        while (true) {
            SkipWhitespace();
            if (m_pos >= m_wkt.size()) break;
            
            if (m_wkt[m_pos] == ')') {
                m_pos++;
                break;
            }
            
            std::string subType = ReadWord();
            std::transform(subType.begin(), subType.end(), subType.begin(), ::toupper);
            
            GeometryPtr geom;
            if (subType == "POINT") {
                geom = ParsePoint(hasZ, hasM);
            } else if (subType == "LINESTRING") {
                geom = ParseLineString(hasZ, hasM);
            } else if (subType == "POLYGON") {
                geom = ParsePolygon(hasZ, hasM);
            } else if (subType == "MULTIPOINT") {
                geom = ParseMultiPoint(hasZ, hasM);
            } else if (subType == "MULTILINESTRING") {
                geom = ParseMultiLineString(hasZ, hasM);
            } else if (subType == "MULTIPOLYGON") {
                geom = ParseMultiPolygon(hasZ, hasM);
            }
            
            if (geom) {
                collection->AddGeometry(std::move(geom));
            }
            
            SkipWhitespace();
            if (m_pos < m_wkt.size() && m_wkt[m_pos] == ',') {
                m_pos++;
            } else {
                break;
            }
        }
        
        Expect(')');
        return collection;
    }
};

class GeometryFactory::WKBReader {
public:
    explicit WKBReader(const std::vector<uint8_t>& wkb)
        : m_wkb(wkb), m_pos(0) {}
    
    GeometryPtr Parse() {
        if (m_wkb.size() < 9) return nullptr;
        
        bool hasZ = false;
        bool hasM = false;
        uint8_t byteOrder = m_wkb[m_pos++];
        
        uint32_t typeInfo = ReadUInt32(byteOrder);
        uint32_t geometryType = typeInfo & 0xFFFF;
        
        if (typeInfo & 0x10000000) hasZ = true;
        if (typeInfo & 0x20000000) hasM = true;
        
        switch (geometryType) {
            case 1: return ParsePoint(byteOrder, hasZ, hasM);
            case 2: return ParseLineString(byteOrder, hasZ, hasM);
            case 3: return ParsePolygon(byteOrder, hasZ, hasM);
            case 4: return ParseMultiPoint(byteOrder, hasZ, hasM);
            case 5: return ParseMultiLineString(byteOrder, hasZ, hasM);
            case 6: return ParseMultiPolygon(byteOrder, hasZ, hasM);
            case 7: return ParseGeometryCollection(byteOrder, hasZ, hasM);
        }
        
        return nullptr;
    }
    
private:
    const std::vector<uint8_t>& m_wkb;
    size_t m_pos;
    
    uint8_t ReadByte() {
        if (m_pos >= m_wkb.size()) return 0;
        return m_wkb[m_pos++];
    }
    
    uint32_t ReadUInt32(uint8_t byteOrder) {
        if (m_pos + 4 > m_wkb.size()) return 0;
        uint32_t val = 0;
        if (byteOrder == 0) {
            val = m_wkb[m_pos] | (m_wkb[m_pos+1] << 8) | (m_wkb[m_pos+2] << 16) | (m_wkb[m_pos+3] << 24);
        } else {
            val = (m_wkb[m_pos] << 24) | (m_wkb[m_pos+1] << 16) | (m_wkb[m_pos+2] << 8) | m_wkb[m_pos+3];
        }
        m_pos += 4;
        return val;
    }
    
    uint64_t ReadUInt64(uint8_t byteOrder) {
        if (m_pos + 8 > m_wkb.size()) return 0;
        uint64_t val = 0;
        if (byteOrder == 0) {
            for (int i = 0; i < 8; i++) val |= ((uint64_t)m_wkb[m_pos+i] << (i*8));
        } else {
            for (int i = 0; i < 8; i++) val |= ((uint64_t)m_wkb[m_pos+i] << ((7-i)*8));
        }
        m_pos += 8;
        return val;
    }
    
    double ReadDouble(uint8_t byteOrder) {
        if (m_pos + 8 > m_wkb.size()) return 0.0;
        uint64_t bits = ReadUInt64(byteOrder);
        double val;
        memcpy(&val, &bits, sizeof(double));
        return val;
    }
    
    Coordinate ReadCoordinate(uint8_t byteOrder, bool hasZ, bool hasM) {
        double x = ReadDouble(byteOrder);
        double y = ReadDouble(byteOrder);
        double z = 0.0;
        double m = 0.0;
        
        if (hasZ) z = ReadDouble(byteOrder);
        if (hasM) m = ReadDouble(byteOrder);
        
        return Coordinate(x, y, z);
    }
    
    GeometryPtr ParsePoint(uint8_t byteOrder, bool hasZ, bool hasM) {
        auto coord = ReadCoordinate(byteOrder, hasZ, hasM);
        return Point::Create(coord);
    }
    
    GeometryPtr ParseLineString(uint8_t byteOrder, bool hasZ, bool hasM) {
        uint32_t numPoints = ReadUInt32(byteOrder);
        auto line = LineString::Create();
        
        for (uint32_t i = 0; i < numPoints; i++) {
            auto coord = ReadCoordinate(byteOrder, hasZ, hasM);
            line->AddPoint(coord);
        }
        
        return line;
    }
    
    GeometryPtr ParsePolygon(uint8_t byteOrder, bool hasZ, bool hasM) {
        uint32_t numRings = ReadUInt32(byteOrder);
        auto polygon = Polygon::Create();
        
        for (uint32_t r = 0; r < numRings; r++) {
            uint32_t numPoints = ReadUInt32(byteOrder);
            auto ring = LinearRing::Create();
            
            for (uint32_t i = 0; i < numPoints; i++) {
                auto coord = ReadCoordinate(byteOrder, hasZ, hasM);
                ring->AddPoint(coord);
            }
            
            if (r == 0) {
                polygon->SetExteriorRing(std::move(ring));
            } else {
                polygon->AddInteriorRing(std::move(ring));
            }
        }
        
        return polygon;
    }
    
    GeometryPtr ParseMultiPoint(uint8_t byteOrder, bool hasZ, bool hasM) {
        uint32_t numGeoms = ReadUInt32(byteOrder);
        auto multiPoint = MultiPoint::Create();
        
        for (uint32_t i = 0; i < numGeoms; i++) {
            if (m_pos + 5 > m_wkb.size()) break;
            m_pos += 4;
            auto point = ParsePoint(byteOrder, hasZ, hasM);
            if (point) {
                multiPoint->AddPoint(PointPtr(dynamic_cast<Point*>(point.release())));
            }
        }
        
        return multiPoint;
    }
    
    GeometryPtr ParseMultiLineString(uint8_t byteOrder, bool hasZ, bool hasM) {
        uint32_t numGeoms = ReadUInt32(byteOrder);
        auto multiLine = MultiLineString::Create();
        
        for (uint32_t i = 0; i < numGeoms; i++) {
            if (m_pos + 5 > m_wkb.size()) break;
            m_pos += 4;
            auto line = ParseLineString(byteOrder, hasZ, hasM);
            if (line) {
                multiLine->AddLineString(LineStringPtr(dynamic_cast<LineString*>(line.release())));
            }
        }
        
        return multiLine;
    }
    
    GeometryPtr ParseMultiPolygon(uint8_t byteOrder, bool hasZ, bool hasM) {
        uint32_t numGeoms = ReadUInt32(byteOrder);
        auto multiPoly = MultiPolygon::Create();
        
        for (uint32_t i = 0; i < numGeoms; i++) {
            if (m_pos + 5 > m_wkb.size()) break;
            m_pos += 4;
            auto poly = ParsePolygon(byteOrder, hasZ, hasM);
            if (poly) {
                multiPoly->AddPolygon(PolygonPtr(dynamic_cast<Polygon*>(poly.release())));
            }
        }
        
        return multiPoly;
    }
    
    GeometryPtr ParseGeometryCollection(uint8_t byteOrder, bool hasZ, bool hasM) {
        uint32_t numGeoms = ReadUInt32(byteOrder);
        auto collection = GeometryCollection::Create();
        
        for (uint32_t i = 0; i < numGeoms; i++) {
            if (m_pos + 5 > m_wkb.size()) break;
            m_pos += 4;
            auto geom = Parse();
            if (geom) {
                collection->AddGeometry(std::move(geom));
            }
        }
        
        return collection;
    }
};

class GeometryFactory::GeoJSONReader {
public:
    explicit GeoJSONReader(const std::string& json)
        : m_json(json), m_pos(0) {}

    GeometryPtr Parse() {
        SkipWhitespace();
        if (m_pos >= m_json.size() || m_json[m_pos] != '{') return nullptr;

        m_pos++;
        SkipWhitespace();

        std::string type = ParseStringMember("type");
        if (type.empty()) return nullptr;

        if (type == "Point") return ParsePoint();
        if (type == "LineString") return ParseLineString();
        if (type == "Polygon") return ParsePolygon();
        if (type == "MultiPoint") return ParseMultiPoint();
        if (type == "MultiLineString") return ParseMultiLineString();
        if (type == "MultiPolygon") return ParseMultiPolygon();
        if (type == "GeometryCollection") return ParseGeometryCollection();
        if (type == "Feature") return ParseFeature();
        if (type == "FeatureCollection") return ParseFeatureCollection();

        return nullptr;
    }

private:
    const std::string& m_json;
    size_t m_pos;

    void SkipWhitespace() {
        while (m_pos < m_json.size() && std::isspace(m_json[m_pos])) m_pos++;
    }

    std::string ParseStringMember(const std::string& key) {
        size_t keyPos = m_json.find("\"" + key + "\"", m_pos);
        if (keyPos == std::string::npos) return "";

        size_t colonPos = m_json.find(":", keyPos);
        if (colonPos == std::string::npos) return "";

        size_t valuePos = colonPos + 1;
        while (valuePos < m_json.size() && std::isspace(m_json[valuePos])) valuePos++;

        if (valuePos >= m_json.size() || m_json[valuePos] != '"') return "";

        size_t start = valuePos + 1;
        size_t end = start;
        while (end < m_json.size() && m_json[end] != '"') {
            if (m_json[end] == '\\') end++;
            end++;
        }

        m_pos = end + 1;
        return m_json.substr(start, end - start);
    }

    double ParseNumber() {
        SkipWhitespace();
        size_t start = m_pos;
        if (m_pos < m_json.size() && (m_json[m_pos] == '-' || m_json[m_pos] == '+')) m_pos++;
        while (m_pos < m_json.size() && (std::isdigit(m_json[m_pos]) || m_json[m_pos] == '.' || m_json[m_pos] == 'e' || m_json[m_pos] == 'E' || m_json[m_pos] == '-' || m_json[m_pos] == '+')) {
            m_pos++;
        }
        std::string numStr = m_json.substr(start, m_pos - start);
        return std::stod(numStr);
    }

    Coordinate ParseCoordinate() {
        SkipWhitespace();
        if (m_pos >= m_json.size() || m_json[m_pos] != '[') return Coordinate();
        m_pos++;

        double x = ParseNumber();
        SkipWhitespace();
        if (m_pos >= m_json.size() || m_json[m_pos] != ',') return Coordinate(x, 0, 0);
        m_pos++;

        double y = ParseNumber();
        double z = 0.0;

        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == ',') {
            m_pos++;
            z = ParseNumber();
        }

        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;

        return Coordinate(x, y, z);
    }

    std::vector<Coordinate> ParseCoordinatesArray() {
        std::vector<Coordinate> coords;
        SkipWhitespace();
        if (m_pos >= m_json.size() || m_json[m_pos] != '[') return coords;
        m_pos++;

        while (m_pos < m_json.size() && m_json[m_pos] != ']') {
            coords.push_back(ParseCoordinate());
            SkipWhitespace();
            if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
        }

        if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
        return coords;
    }

    GeometryPtr ParsePoint() {
        size_t keyPos = m_json.find("\"coordinates\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = keyPos + 12;
        m_pos = colonPos + 1;
        auto coord = ParseCoordinate();
        return Point::Create(coord);
    }

    GeometryPtr ParseLineString() {
        size_t keyPos = m_json.find("\"coordinates\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = keyPos + 12;
        m_pos = colonPos + 1;

        auto coords = ParseCoordinatesArray();
        auto line = LineString::Create();
        for (const auto& coord : coords) {
            line->AddPoint(coord);
        }
        return line;
    }

    GeometryPtr ParsePolygon() {
        size_t keyPos = m_json.find("\"coordinates\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = keyPos + 12;
        m_pos = colonPos + 1;

        std::vector<std::vector<Coordinate>> rings;
        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == '[') {
            m_pos++;
            while (m_pos < m_json.size() && m_json[m_pos] != ']') {
                rings.push_back(ParseCoordinatesArray());
                SkipWhitespace();
                if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
            }
            if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
        }

        auto polygon = Polygon::Create();
        for (size_t r = 0; r < rings.size(); r++) {
            auto ring = LinearRing::Create();
            for (const auto& coord : rings[r]) {
                ring->AddPoint(coord);
            }

            if (r == 0) {
                polygon->SetExteriorRing(std::move(ring));
            } else {
                polygon->AddInteriorRing(std::move(ring));
            }
        }
        return polygon;
    }

    GeometryPtr ParseMultiPoint() {
        size_t keyPos = m_json.find("\"coordinates\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = keyPos + 12;
        m_pos = colonPos + 1;

        auto coords = ParseCoordinatesArray();
        auto multiPoint = MultiPoint::Create();
        for (const auto& coord : coords) {
            auto point = Point::Create(coord);
            multiPoint->AddPoint(PointPtr(dynamic_cast<Point*>(point.release())));
        }
        return multiPoint;
    }

    GeometryPtr ParseMultiLineString() {
        size_t keyPos = m_json.find("\"coordinates\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = keyPos + 12;
        m_pos = colonPos + 1;

        std::vector<std::vector<Coordinate>> lines;
        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == '[') {
            m_pos++;
            while (m_pos < m_json.size() && m_json[m_pos] != ']') {
                lines.push_back(ParseCoordinatesArray());
                SkipWhitespace();
                if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
            }
            if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
        }

        auto multiLine = MultiLineString::Create();
        for (const auto& lineCoords : lines) {
            auto line = LineString::Create();
            for (const auto& coord : lineCoords) {
                line->AddPoint(coord);
            }
            multiLine->AddLineString(LineStringPtr(dynamic_cast<LineString*>(line.release())));
        }
        return multiLine;
    }

    GeometryPtr ParseMultiPolygon() {
        size_t keyPos = m_json.find("\"coordinates\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = keyPos + 12;
        m_pos = colonPos + 1;

        std::vector<std::vector<std::vector<Coordinate>>> polys;
        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == '[') {
            m_pos++;
            while (m_pos < m_json.size() && m_json[m_pos] != ']') {
                std::vector<std::vector<Coordinate>> rings;
                SkipWhitespace();
                if (m_pos < m_json.size() && m_json[m_pos] == '[') {
                    m_pos++;
                    while (m_pos < m_json.size() && m_json[m_pos] != ']') {
                        rings.push_back(ParseCoordinatesArray());
                        SkipWhitespace();
                        if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
                    }
                    if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
                }
                polys.push_back(rings);
                SkipWhitespace();
                if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
            }
            if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
        }

        auto multiPoly = MultiPolygon::Create();
        for (const auto& polyCoords : polys) {
            auto polygon = Polygon::Create();
            for (size_t r = 0; r < polyCoords.size(); r++) {
                auto ring = LinearRing::Create();
                for (const auto& coord : polyCoords[r]) {
                    ring->AddPoint(coord);
                }

                if (r == 0) {
                    polygon->SetExteriorRing(std::move(ring));
                } else {
                    polygon->AddInteriorRing(std::move(ring));
                }
            }
            multiPoly->AddPolygon(PolygonPtr(dynamic_cast<Polygon*>(polygon.release())));
        }
        return multiPoly;
    }

    GeometryPtr ParseGeometryCollection() {
        auto geoms = ParseGeometriesMember("geometries");
        if (geoms.empty()) return nullptr;

        auto collection = GeometryCollection::Create();
        for (auto& geom : geoms) {
            collection->AddGeometry(std::move(geom));
        }
        return collection;
    }

    GeometryPtr ParseFeature() {
        auto geom = ParseGeometryMember("geometry");
        if (!geom) return nullptr;
        return geom;
    }

    GeometryPtr ParseFeatureCollection() {
        return ParseGeometryCollection();
    }

    Coordinate ParseCoordinatesMember(const std::string& key) {
        size_t keyPos = m_json.find("\"" + key + "\"", m_pos);
        if (keyPos == std::string::npos) return Coordinate();

        size_t colonPos = m_json.find(":", keyPos);
        if (colonPos == std::string::npos) return Coordinate();

        size_t savedPos = m_pos;
        m_pos = colonPos + 1;
        Coordinate coord = ParseCoordinate();
        m_pos = savedPos;
        return coord;
    }

    std::vector<Coordinate> ParseCoordsArrayMember(const std::string& key) {
        size_t keyPos = m_json.find("\"" + key + "\"", m_pos);
        if (keyPos == std::string::npos) return {};

        size_t colonPos = m_json.find(":", keyPos);
        if (colonPos == std::string::npos) return {};

        size_t savedPos = m_pos;
        m_pos = colonPos + 1;
        auto coords = ParseCoordinatesArray();
        m_pos = savedPos;
        return coords;
    }

    std::vector<std::vector<Coordinate>> ParseCoordsArrayOfArrayMember(const std::string& key) {
        size_t keyPos = m_json.find("\"" + key + "\"", m_pos);
        if (keyPos == std::string::npos) return {};

        size_t colonPos = m_json.find(":", keyPos);
        if (colonPos == std::string::npos) return {};

        size_t savedPos = m_pos;
        m_pos = colonPos + 1;

        std::vector<std::vector<Coordinate>> result;
        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == '[') {
            m_pos++;
            while (m_pos < m_json.size() && m_json[m_pos] != ']') {
                result.push_back(ParseCoordinatesArray());
                SkipWhitespace();
                if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
            }
            if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
        }

        m_pos = savedPos;
        return result;
    }

    std::vector<GeometryPtr> ParseGeometriesMember(const std::string& key) {
        size_t keyPos = m_json.find("\"" + key + "\"", m_pos);
        if (keyPos == std::string::npos) return {};

        size_t colonPos = m_json.find(":", keyPos);
        if (colonPos == std::string::npos) return {};

        size_t savedPos = m_pos;
        m_pos = colonPos + 1;

        std::vector<GeometryPtr> geoms;
        SkipWhitespace();
        if (m_pos < m_json.size() && m_json[m_pos] == '[') {
            m_pos++;
            while (m_pos < m_json.size() && m_json[m_pos] != ']') {
                GeometryFactory::GeoJSONReader reader(m_json.substr(m_pos - 1));
                auto geom = reader.Parse();
                if (geom) geoms.push_back(std::move(geom));
                SkipWhitespace();
                if (m_pos < m_json.size() && m_json[m_pos] == ',') m_pos++;
            }
            if (m_pos < m_json.size() && m_json[m_pos] == ']') m_pos++;
        }

        m_pos = savedPos;
        return geoms;
    }

    GeometryPtr ParseGeometryMember(const std::string& key) {
        size_t keyPos = m_json.find("\"" + key + "\"", m_pos);
        if (keyPos == std::string::npos) return nullptr;

        size_t colonPos = m_json.find(":", keyPos);
        if (colonPos == std::string::npos) return nullptr;

        size_t savedPos = m_pos;
        m_pos = colonPos + 1;

        GeometryPtr geom = Parse();

        m_pos = savedPos;
        return geom;
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
    try {
        WKBReader reader(wkb);
        result = reader.Parse();
        if (result) {
            result->SetSRID(m_defaultSRID);
            return GeomResult::kSuccess;
        }
        return GeomResult::kParseError;
    } catch (const std::exception&) {
        return GeomResult::kParseError;
    }
}

GeomResult GeometryFactory::FromWKB(const uint8_t* data, size_t size, GeometryPtr& result) {
    if (!data || size < 5) return GeomResult::kParseError;
    std::vector<uint8_t> wkb(data, data + size);
    return FromWKB(wkb, result);
}

GeomResult GeometryFactory::FromGeoJSON(const std::string& json, GeometryPtr& result) {
    try {
        GeoJSONReader reader(json);
        result = reader.Parse();
        if (result) {
            result->SetSRID(m_defaultSRID);
            return GeomResult::kSuccess;
        }
        return GeomResult::kParseError;
    } catch (const std::exception&) {
        return GeomResult::kParseError;
    }
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
    auto multiPoint = MultiPoint::Create(coords);
    multiPoint->SetSRID(m_defaultSRID);
    return multiPoint;
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
