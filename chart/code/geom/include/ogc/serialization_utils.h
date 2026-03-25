#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

namespace ogc {

namespace wkb {

inline void WriteDoubleLE(std::vector<uint8_t>& buffer, double value) {
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
    for (int i = 0; i < 8; ++i) {
        buffer.push_back(bytes[i]);
    }
}

inline void WriteUInt32LE(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

inline void WriteByteOrder(std::vector<uint8_t>& buffer) {
    buffer.push_back(0x01);
}

inline void WriteGeometryType(std::vector<uint8_t>& buffer, uint32_t baseType, bool is3D, bool isMeasured) {
    uint32_t geomType = baseType;
    if (is3D) {
        geomType += 1000;
    }
    if (isMeasured) {
        geomType += 2000;
    }
    WriteUInt32LE(buffer, geomType);
}

inline void WriteCoordinate(std::vector<uint8_t>& buffer, double x, double y, double z, double m, bool is3D, bool isMeasured) {
    WriteDoubleLE(buffer, x);
    WriteDoubleLE(buffer, y);
    if (is3D) {
        WriteDoubleLE(buffer, z);
    }
    if (isMeasured) {
        WriteDoubleLE(buffer, m);
    }
}

}

namespace geojson {

inline std::string Quote(const std::string& s) {
    return "\"" + s + "\"";
}

inline std::string Number(double value, int precision = 15) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    std::string str = oss.str();
    
    size_t dotPos = str.find('.');
    if (dotPos != std::string::npos) {
        size_t lastNonZero = str.find_last_not_of('0');
        if (lastNonZero != std::string::npos && lastNonZero > dotPos) {
            str = str.substr(0, lastNonZero + 1);
        }
        if (str.back() == '.') {
            str.pop_back();
        }
    }
    
    return str;
}

inline std::string Coordinate(double x, double y, int precision = 15) {
    return "[" + Number(x, precision) + ", " + Number(y, precision) + "]";
}

inline std::string Coordinate3D(double x, double y, double z, int precision = 15) {
    return "[" + Number(x, precision) + ", " + Number(y, precision) + ", " + Number(z, precision) + "]";
}

inline std::string Coordinate4D(double x, double y, double z, double m, int precision = 15) {
    return "[" + Number(x, precision) + ", " + Number(y, precision) + ", " + Number(z, precision) + ", " + Number(m, precision) + "]";
}

inline std::string Point(const std::string& coords, int precision = 15) {
    (void)precision;
    return "{\"type\":\"Point\",\"coordinates\":" + coords + "}";
}

inline std::string LineString(const std::string& coords, int precision = 15) {
    (void)precision;
    return "{\"type\":\"LineString\",\"coordinates\":" + coords + "}";
}

inline std::string Polygon(const std::string& coords, int precision = 15) {
    (void)precision;
    return "{\"type\":\"Polygon\",\"coordinates\":" + coords + "}";
}

inline std::string MultiPoint(const std::string& coords, int precision = 15) {
    (void)precision;
    return "{\"type\":\"MultiPoint\",\"coordinates\":" + coords + "}";
}

inline std::string MultiLineString(const std::string& coords, int precision = 15) {
    (void)precision;
    return "{\"type\":\"MultiLineString\",\"coordinates\":" + coords + "}";
}

inline std::string MultiPolygon(const std::string& coords, int precision = 15) {
    (void)precision;
    return "{\"type\":\"MultiPolygon\",\"coordinates\":" + coords + "}";
}

inline std::string GeometryCollection(const std::string& geometries) {
    return "{\"type\":\"GeometryCollection\",\"geometries\":[" + geometries + "]}";
}

}

namespace gml {

inline std::string Number(double value, int precision = 15) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    std::string str = oss.str();
    
    size_t dotPos = str.find('.');
    if (dotPos != std::string::npos) {
        size_t lastNonZero = str.find_last_not_of('0');
        if (lastNonZero != std::string::npos && lastNonZero > dotPos) {
            str = str.substr(0, lastNonZero + 1);
        }
        if (str.back() == '.') {
            str.pop_back();
        }
    }
    
    return str;
}

inline std::string Pos(double x, double y, int precision = 15) {
    return Number(x, precision) + " " + Number(y, precision);
}

inline std::string Pos3D(double x, double y, double z, int precision = 15) {
    return Number(x, precision) + " " + Number(y, precision) + " " + Number(z, precision);
}

inline std::string PosList2D(const std::vector<std::pair<double, double>>& coords, int precision = 15) {
    std::ostringstream oss;
    for (size_t i = 0; i < coords.size(); ++i) {
        if (i > 0) oss << " ";
        oss << Number(coords[i].first, precision) << " " << Number(coords[i].second, precision);
    }
    return oss.str();
}

inline std::string PosList3D(const std::vector<std::tuple<double, double, double>>& coords, int precision = 15) {
    std::ostringstream oss;
    for (size_t i = 0; i < coords.size(); ++i) {
        if (i > 0) oss << " ";
        oss << Number(std::get<0>(coords[i]), precision) << " " 
            << Number(std::get<1>(coords[i]), precision) << " " 
            << Number(std::get<2>(coords[i]), precision);
    }
    return oss.str();
}

inline std::string Point(const std::string& pos, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    return "<gml:Point" + srs + "><gml:pos>" + pos + "</gml:pos></gml:Point>";
}

inline std::string LineString(const std::string& posList, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    return "<gml:LineString" + srs + "><gml:posList>" + posList + "</gml:posList></gml:LineString>";
}

inline std::string Polygon(const std::string& exterior, const std::string& interior, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    std::string result = "<gml:Polygon" + srs + "><gml:exterior><gml:LinearRing><gml:posList>" + exterior + "</gml:posList></gml:LinearRing></gml:exterior>";
    if (!interior.empty()) {
        result += "<gml:interior><gml:LinearRing><gml:posList>" + interior + "</gml:posList></gml:LinearRing></gml:interior>";
    }
    result += "</gml:Polygon>";
    return result;
}

inline std::string MultiPoint(const std::string& points, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    return "<gml:MultiPoint" + srs + ">" + points + "</gml:MultiPoint>";
}

inline std::string MultiLineString(const std::string& lineStrings, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    return "<gml:MultiCurve" + srs + ">" + lineStrings + "</gml:MultiCurve>";
}

inline std::string MultiPolygon(const std::string& polygons, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    return "<gml:MultiSurface" + srs + ">" + polygons + "</gml:MultiSurface>";
}

inline std::string GeometryCollection(const std::string& geometries, const std::string& srsName = "") {
    std::string srs = srsName.empty() ? "" : " srsName=\"" + srsName + "\"";
    return "<gml:MultiGeometry" + srs + ">" + geometries + "</gml:MultiGeometry>";
}

inline std::string PointMember(const std::string& point) {
    return "<gml:pointMember>" + point + "</gml:pointMember>";
}

inline std::string CurveMember(const std::string& lineString) {
    return "<gml:curveMember>" + lineString + "</gml:curveMember>";
}

inline std::string SurfaceMember(const std::string& polygon) {
    return "<gml:surfaceMember>" + polygon + "</gml:surfaceMember>";
}

inline std::string GeometryMember(const std::string& geometry) {
    return "<gml:geometryMember>" + geometry + "</gml:geometryMember>";
}

}

namespace kml {

inline std::string Number(double value, int precision = 15) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    std::string str = oss.str();
    
    size_t dotPos = str.find('.');
    if (dotPos != std::string::npos) {
        size_t lastNonZero = str.find_last_not_of('0');
        if (lastNonZero != std::string::npos && lastNonZero > dotPos) {
            str = str.substr(0, lastNonZero + 1);
        }
        if (str.back() == '.') {
            str.pop_back();
        }
    }
    
    return str;
}

inline std::string Coordinate(double x, double y, int precision = 15) {
    return Number(x, precision) + "," + Number(y, precision);
}

inline std::string Coordinate3D(double x, double y, double z, int precision = 15) {
    return Number(x, precision) + "," + Number(y, precision) + "," + Number(z, precision);
}

inline std::string Coordinates(const std::string& coords) {
    return "<coordinates>" + coords + "</coordinates>";
}

inline std::string Point(const std::string& coords) {
    return "<Point>" + Coordinates(coords) + "</Point>";
}

inline std::string LineString(const std::string& coords) {
    return "<LineString>" + Coordinates(coords) + "</LineString>";
}

inline std::string Polygon(const std::string& outerCoords, const std::string& innerCoords = "") {
    std::string result = "<Polygon><outerBoundaryIs><LinearRing>" + Coordinates(outerCoords) + "</LinearRing></outerBoundaryIs>";
    if (!innerCoords.empty()) {
        result += "<innerBoundaryIs><LinearRing>" + Coordinates(innerCoords) + "</LinearRing></innerBoundaryIs>";
    }
    result += "</Polygon>";
    return result;
}

inline std::string MultiGeometry(const std::string& geometries) {
    return "<MultiGeometry>" + geometries + "</MultiGeometry>";
}

}

}
