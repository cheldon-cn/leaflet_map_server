#include "ogc/db/geojson_converter.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/linearring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/envelope.h"
#include "ogc/envelope3d.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cctype>

namespace ogc {
namespace db {

std::string GeoJsonConverter::GetGeometryTypeName(GeomType type) {
    switch (type) {
        case GeomType::kPoint: return kGeoJsonTypePoint;
        case GeomType::kLineString: return kGeoJsonTypeLineString;
        case GeomType::kPolygon: return kGeoJsonTypePolygon;
        case GeomType::kMultiPoint: return kGeoJsonTypeMultiPoint;
        case GeomType::kMultiLineString: return kGeoJsonTypeMultiLineString;
        case GeomType::kMultiPolygon: return kGeoJsonTypeMultiPolygon;
        case GeomType::kGeometryCollection: return kGeoJsonTypeGeometryCollection;
        default: return "Unknown";
    }
}

Result GeoJsonConverter::GeometryToJson(const Geometry* geometry, std::string& json, const GeoJsonOptions& options) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    std::ostringstream oss;
    
    if (options.prettyPrint) {
        oss << "{\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"type\": \"" << GetGeometryTypeName(geometry->GetGeometryType()) << "\",\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"coordinates\": ";
    } else {
        oss << "{\"type\":\"" << GetGeometryTypeName(geometry->GetGeometryType()) << "\",\"coordinates\":";
    }
    
    WriteGeometry(geometry, oss, options);
    
    if (options.includeBoundingBox) {
        const Envelope& env = geometry->GetEnvelope();
        std::vector<double> bbox;
        EnvelopeToBoundingBox(env, bbox);
        
        if (options.prettyPrint) {
            oss << ",\n";
            oss << std::string(options.indentSpaces, ' ');
            oss << "\"bbox\": [";
        } else {
            oss << ",\"bbox\":[";
        }
        
        for (size_t i = 0; i < bbox.size(); ++i) {
            if (i > 0) oss << ",";
            WriteCoordinate(bbox[i], oss, options);
        }
        oss << "]";
    }
    
    if (options.includeCRS) {
        if (options.prettyPrint) {
            oss << ",\n";
            oss << std::string(options.indentSpaces, ' ');
            oss << "\"crs\": {\"type\": \"name\", \"properties\": {\"name\": \"EPSG:" 
                << geometry->GetSRID() << "\"}}";
        } else {
            oss << ",\"crs\":{\"type\":\"name\",\"properties\":{\"name\":\"EPSG:" 
                << geometry->GetSRID() << "\"}}";
        }
    }
    
    if (options.prettyPrint) {
        oss << "\n}";
    } else {
        oss << "}";
    }
    
    json = oss.str();
    return Result::Success();
}

void GeoJsonConverter::WriteGeometry(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint:
            WritePoint(geometry, oss, options);
            break;
        case GeomType::kLineString:
            WriteLineString(geometry, oss, options);
            break;
        case GeomType::kPolygon:
            WritePolygon(geometry, oss, options);
            break;
        case GeomType::kMultiPoint:
            WriteMultiPoint(geometry, oss, options);
            break;
        case GeomType::kMultiLineString:
            WriteMultiLineString(geometry, oss, options);
            break;
        case GeomType::kMultiPolygon:
            WriteMultiPolygon(geometry, oss, options);
            break;
        case GeomType::kGeometryCollection:
            WriteGeometryCollection(geometry, oss, options);
            break;
        default:
            oss << "[]";
            break;
    }
}

void GeoJsonConverter::WritePoint(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const Point* point = static_cast<const Point*>(geometry);
    const Coordinate& coord = point->GetCoordinate();
    WriteCoordinates(coord, oss, options);
}

void GeoJsonConverter::WriteLineString(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const LineString* line = static_cast<const LineString*>(geometry);
    const auto& coords = line->GetCoordinates();
    
    if (coords.empty()) {
        oss << "[]";
        return;
    }
    
    oss << "[";
    for (size_t i = 0; i < coords.size(); ++i) {
        if (i > 0) oss << ",";
        WriteCoordinates(coords[i], oss, options);
    }
    oss << "]";
}

void GeoJsonConverter::WritePolygon(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const Polygon* polygon = static_cast<const Polygon*>(geometry);
    
    const LinearRing* exterior = polygon->GetExteriorRing();
    if (!exterior) {
        oss << "[]";
        return;
    }
    
    oss << "[[";
    const auto& exteriorCoords = exterior->GetCoordinates();
    for (size_t i = 0; i < exteriorCoords.size(); ++i) {
        if (i > 0) oss << ",";
        WriteCoordinates(exteriorCoords[i], oss, options);
    }
    oss << "]";
    
    for (size_t r = 0; r < polygon->GetNumInteriorRings(); ++r) {
        const LinearRing* interior = polygon->GetInteriorRingN(r);
        if (interior) {
            oss << ",[";
            const auto& interiorCoords = interior->GetCoordinates();
            for (size_t i = 0; i < interiorCoords.size(); ++i) {
                if (i > 0) oss << ",";
                WriteCoordinates(interiorCoords[i], oss, options);
            }
            oss << "]";
        }
    }
    
    oss << "]]";
}

void GeoJsonConverter::WriteMultiPoint(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const MultiPoint* mpoint = static_cast<const MultiPoint*>(geometry);
    
    if (mpoint->GetNumGeometries() == 0) {
        oss << "[]";
        return;
    }
    
    oss << "[";
    for (size_t i = 0; i < mpoint->GetNumGeometries(); ++i) {
        if (i > 0) oss << ",";
        const Point* pt = static_cast<const Point*>(mpoint->GetGeometryN(i));
        if (pt) {
            WriteCoordinates(pt->GetCoordinate(), oss, options);
        }
    }
    oss << "]";
}

void GeoJsonConverter::WriteMultiLineString(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const MultiLineString* mline = static_cast<const MultiLineString*>(geometry);
    
    if (mline->GetNumGeometries() == 0) {
        oss << "[]";
        return;
    }
    
    oss << "[";
    for (size_t i = 0; i < mline->GetNumGeometries(); ++i) {
        if (i > 0) oss << ",";
        const LineString* line = static_cast<const LineString*>(mline->GetGeometryN(i));
        if (line) {
            const auto& coords = line->GetCoordinates();
            oss << "[";
            for (size_t j = 0; j < coords.size(); ++j) {
                if (j > 0) oss << ",";
                WriteCoordinates(coords[j], oss, options);
            }
            oss << "]";
        }
    }
    oss << "]";
}

void GeoJsonConverter::WriteMultiPolygon(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const MultiPolygon* mpoly = static_cast<const MultiPolygon*>(geometry);
    
    if (mpoly->GetNumGeometries() == 0) {
        oss << "[]";
        return;
    }
    
    oss << "[";
    for (size_t i = 0; i < mpoly->GetNumGeometries(); ++i) {
        if (i > 0) oss << ",";
        const Polygon* poly = static_cast<const Polygon*>(mpoly->GetGeometryN(i));
        if (poly) {
            const LinearRing* exterior = poly->GetExteriorRing();
            if (exterior) {
                oss << "[[";
                const auto& exteriorCoords = exterior->GetCoordinates();
                for (size_t j = 0; j < exteriorCoords.size(); ++j) {
                    if (j > 0) oss << ",";
                    WriteCoordinates(exteriorCoords[j], oss, options);
                }
                oss << "]";
                
                for (size_t r = 0; r < poly->GetNumInteriorRings(); ++r) {
                    const LinearRing* interior = poly->GetInteriorRingN(r);
                    if (interior) {
                        oss << ",[";
                        const auto& interiorCoords = interior->GetCoordinates();
                        for (size_t j = 0; j < interiorCoords.size(); ++j) {
                            if (j > 0) oss << ",";
                            WriteCoordinates(interiorCoords[j], oss, options);
                        }
                        oss << "]";
                    }
                }
                oss << "]";
            }
        }
    }
    oss << "]";
}

void GeoJsonConverter::WriteGeometryCollection(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options) {
    const GeometryCollection* collection = static_cast<const GeometryCollection*>(geometry);
    
    if (collection->GetNumGeometries() == 0) {
        oss << "[]";
        return;
    }
    
    oss << "[";
    for (size_t i = 0; i < collection->GetNumGeometries(); ++i) {
        if (i > 0) oss << ",";
        const Geometry* geom = collection->GetGeometryN(i);
        if (geom) {
            oss << "{\"type\":\"" << GetGeometryTypeName(geom->GetGeometryType()) << "\",\"coordinates\":";
            WriteGeometry(geom, oss, options);
            oss << "}";
        }
    }
    oss << "]";
}

void GeoJsonConverter::WriteCoordinate(double value, std::ostringstream& oss, const GeoJsonOptions& options) {
    if (options.precision >= 0) {
        oss << std::fixed << std::setprecision(options.precision) << value;
    } else {
        oss << value;
    }
}

void GeoJsonConverter::WriteCoordinates(const Coordinate& coord, std::ostringstream& oss, const GeoJsonOptions& options) {
    oss << "[";
    WriteCoordinate(coord.x, oss, options);
    oss << ",";
    WriteCoordinate(coord.y, oss, options);
    
    if (coord.Is3D()) {
        oss << ",";
        WriteCoordinate(coord.z, oss, options);
    }
    
    if (coord.IsMeasured()) {
        oss << ",";
        WriteCoordinate(coord.m, oss, options);
    }
    
    oss << "]";
}

Result GeoJsonConverter::GeometryFromJson(const std::string& json, std::unique_ptr<Geometry>& geometry) {
    size_t pos = 0;
    SkipWhitespace(json, pos);
    
    if (pos >= json.length() || json[pos] != '{') {
        return Result::Error(DbResult::kInvalidGeometry, "Invalid GeoJSON: expected {");
    }
    ++pos;
    
    return ReadGeometry(json, pos, geometry);
}

Result GeoJsonConverter::ReadGeometry(const std::string& json, size_t& pos, std::unique_ptr<Geometry>& geometry) {
    SkipWhitespace(json, pos);
    
    if (!MatchString(json, pos, "\"type\"")) {
        return Result::Error(DbResult::kInvalidGeometry, "Invalid GeoJSON: missing type");
    }
    
    SkipWhitespace(json, pos);
    if (pos >= json.length() || json[pos] != ':') {
        return Result::Error(DbResult::kInvalidGeometry, "Invalid GeoJSON: expected :");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    std::string type = ReadString(json, pos);
    
    SkipWhitespace(json, pos);
    if (pos >= json.length() || json[pos] != ',') {
        return Result::Error(DbResult::kInvalidGeometry, "Invalid GeoJSON: expected ,");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    if (!MatchString(json, pos, "\"coordinates\"")) {
        return Result::Error(DbResult::kInvalidGeometry, "Invalid GeoJSON: missing coordinates");
    }
    
    SkipWhitespace(json, pos);
    if (pos >= json.length() || json[pos] != ':') {
        return Result::Error(DbResult::kInvalidGeometry, "Invalid GeoJSON: expected :");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    
    if (type == kGeoJsonTypePoint) {
        Coordinate coord;
        Result result = ReadCoordinate(json, pos, coord);
        if (!result.IsSuccess()) return result;
        geometry = Point::Create(coord);
    }
    else if (type == kGeoJsonTypeLineString) {
        CoordinateList coords;
        Result result = ReadCoordinateArray(json, pos, coords);
        if (!result.IsSuccess()) return result;
        geometry = LineString::Create(std::move(coords));
    }
    else if (type == kGeoJsonTypePolygon) {
        std::vector<std::vector<Coordinate>> rings;
        Result result = ReadPolygonCoordinates(json, pos, rings);
        if (!result.IsSuccess()) return result;
        
        if (rings.empty()) {
            return Result::Error(DbResult::kInvalidGeometry, "Polygon has no rings");
        }
        
        auto exterior = LinearRing::Create(rings[0], true);
        auto poly = Polygon::Create(std::move(exterior));
        
        for (size_t i = 1; i < rings.size(); ++i) {
            auto interior = LinearRing::Create(rings[i], true);
            poly->AddInteriorRing(std::move(interior));
        }
        
        geometry = std::move(poly);
    }
    else {
        return Result::Error(DbResult::kNotSupported, "Unsupported GeoJSON type: " + type);
    }
    
    return Result::Success();
}

Result GeoJsonConverter::ReadCoordinate(const std::string& json, size_t& pos, Coordinate& coord) {
    SkipWhitespace(json, pos);
    
    if (pos >= json.length() || json[pos] != '[') {
        return Result::Error(DbResult::kInvalidGeometry, "Expected [ for coordinate");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    coord.x = ReadNumber(json, pos);
    
    SkipWhitespace(json, pos);
    if (pos >= json.length() || json[pos] != ',') {
        return Result::Error(DbResult::kInvalidGeometry, "Expected , in coordinate");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    coord.y = ReadNumber(json, pos);
    
    SkipWhitespace(json, pos);
    if (pos < json.length() && json[pos] == ',') {
        ++pos;
        SkipWhitespace(json, pos);
        coord.z = ReadNumber(json, pos);
        
        SkipWhitespace(json, pos);
        if (pos < json.length() && json[pos] == ',') {
            ++pos;
            SkipWhitespace(json, pos);
            coord.m = ReadNumber(json, pos);
        }
    }
    
    SkipWhitespace(json, pos);
    if (pos >= json.length() || json[pos] != ']') {
        return Result::Error(DbResult::kInvalidGeometry, "Expected ] for coordinate");
    }
    ++pos;
    
    return Result::Success();
}

Result GeoJsonConverter::ReadCoordinateArray(const std::string& json, size_t& pos, std::vector<Coordinate>& coords) {
    SkipWhitespace(json, pos);
    
    if (pos >= json.length() || json[pos] != '[') {
        return Result::Error(DbResult::kInvalidGeometry, "Expected [ for coordinate array");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    if (pos < json.length() && json[pos] == ']') {
        ++pos;
        return Result::Success();
    }
    
    while (true) {
        Coordinate coord;
        Result result = ReadCoordinate(json, pos, coord);
        if (!result.IsSuccess()) return result;
        coords.push_back(coord);
        
        SkipWhitespace(json, pos);
        if (pos < json.length() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.length() && json[pos] == ']') {
            ++pos;
            break;
        } else {
            return Result::Error(DbResult::kInvalidGeometry, "Invalid coordinate array");
        }
    }
    
    return Result::Success();
}

Result GeoJsonConverter::ReadPolygonCoordinates(const std::string& json, size_t& pos, std::vector<std::vector<Coordinate>>& rings) {
    SkipWhitespace(json, pos);
    
    if (pos >= json.length() || json[pos] != '[') {
        return Result::Error(DbResult::kInvalidGeometry, "Expected [ for polygon");
    }
    ++pos;
    
    SkipWhitespace(json, pos);
    if (pos < json.length() && json[pos] == ']') {
        ++pos;
        return Result::Success();
    }
    
    while (true) {
        std::vector<Coordinate> ringCoords;
        Result result = ReadCoordinateArray(json, pos, ringCoords);
        if (!result.IsSuccess()) return result;
        rings.push_back(std::move(ringCoords));
        
        SkipWhitespace(json, pos);
        if (pos < json.length() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.length() && json[pos] == ']') {
            ++pos;
            break;
        } else {
            return Result::Error(DbResult::kInvalidGeometry, "Invalid polygon");
        }
    }
    
    return Result::Success();
}

void GeoJsonConverter::SkipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.length() && std::isspace(json[pos])) {
        ++pos;
    }
}

std::string GeoJsonConverter::ReadString(const std::string& json, size_t& pos) {
    SkipWhitespace(json, pos);
    
    std::string result;
    if (pos < json.length() && json[pos] == '"') {
        ++pos;
        while (pos < json.length() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.length()) {
                ++pos;
                result += json[pos++];
            } else {
                result += json[pos++];
            }
        }
        if (pos < json.length()) ++pos;
    }
    
    return result;
}

double GeoJsonConverter::ReadNumber(const std::string& json, size_t& pos) {
    SkipWhitespace(json, pos);
    
    std::string numStr;
    while (pos < json.length() && (std::isdigit(json[pos]) || json[pos] == '-' || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+')) {
        numStr += json[pos++];
    }
    
    try {
        return std::stod(numStr);
    } catch (...) {
        return 0.0;
    }
}

bool GeoJsonConverter::MatchString(const std::string& json, size_t& pos, const char* expected) {
    SkipWhitespace(json, pos);
    
    size_t savedPos = pos;
    while (*expected) {
        if (pos >= json.length() || json[pos] != *expected) {
            pos = savedPos;
            return false;
        }
        ++pos;
        ++expected;
    }
    
    return true;
}

Result GeoJsonConverter::FeatureToJson(const GeoJsonFeature& feature, std::string& json, const GeoJsonOptions& options) {
    std::ostringstream oss;
    
    if (options.prettyPrint) {
        oss << "{\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"type\": \"Feature\",\n";
        
        if (!feature.id.empty()) {
            oss << std::string(options.indentSpaces, ' ');
            oss << "\"id\": \"" << feature.id << "\",\n";
        }
        
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"geometry\": ";
    } else {
        oss << "{\"type\":\"Feature\"";
        
        if (!feature.id.empty()) {
            oss << ",\"id\":\"" << feature.id << "\"";
        }
        
        oss << ",\"geometry\":";
    }
    
    if (feature.geometry) {
        GeometryToJson(feature.geometry.get(), json, options);
        oss << json;
    } else {
        oss << "null";
    }
    
    if (options.prettyPrint) {
        oss << ",\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"properties\": {";
        
        bool first = true;
        for (const auto& prop : feature.properties) {
            if (!first) oss << ",";
            oss << "\"" << prop.first << "\":\"" << prop.second << "\"";
            first = false;
        }
        oss << "}\n}";
    } else {
        oss << ",\"properties\":{";
        
        bool first = true;
        for (const auto& prop : feature.properties) {
            if (!first) oss << ",";
            oss << "\"" << prop.first << "\":\"" << prop.second << "\"";
            first = false;
        }
        oss << "}}";
    }
    
    json = oss.str();
    return Result::Success();
}

Result GeoJsonConverter::FeatureFromJson(const std::string& json, GeoJsonFeature& feature) {
    return Result::Error(DbResult::kNotImplemented, "FeatureFromJson not implemented");
}

Result GeoJsonConverter::FeatureCollectionToJson(const std::vector<GeoJsonFeature>& features, std::string& json, const GeoJsonOptions& options) {
    std::ostringstream oss;
    
    if (options.prettyPrint) {
        oss << "{\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"type\": \"FeatureCollection\",\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "\"features\": [\n";
        
        for (size_t i = 0; i < features.size(); ++i) {
            if (i > 0) oss << ",\n";
            oss << std::string(options.indentSpaces * 2, ' ');
            FeatureToJson(features[i], json, options);
            oss << json;
        }
        
        oss << "\n";
        oss << std::string(options.indentSpaces, ' ');
        oss << "]\n}";
    } else {
        oss << "{\"type\":\"FeatureCollection\",\"features\":[";
        
        for (size_t i = 0; i < features.size(); ++i) {
            if (i > 0) oss << ",";
            FeatureToJson(features[i], json, options);
            oss << json;
        }
        
        oss << "]}";
    }
    
    json = oss.str();
    return Result::Success();
}

Result GeoJsonConverter::FeatureCollectionFromJson(const std::string& json, std::vector<GeoJsonFeature>& features) {
    return Result::Error(DbResult::kNotImplemented, "FeatureCollectionFromJson not implemented");
}

Result GeoJsonConverter::EnvelopeToBoundingBox(const Envelope& envelope, std::vector<double>& bbox) {
    bbox.clear();
    
    if (envelope.IsEmpty()) {
        return Result::Error(DbResult::kInvalidGeometry, "Envelope is empty");
    }
    
    bbox.push_back(envelope.GetMinX());
    bbox.push_back(envelope.GetMinY());
    bbox.push_back(envelope.GetMaxX());
    bbox.push_back(envelope.GetMaxY());
    
    return Result::Success();
}

bool GeoJsonConverter::ValidateGeoJson(const std::string& json, std::string& errorMessage) {
    if (json.empty()) {
        errorMessage = "Empty JSON";
        return false;
    }
    
    size_t pos = 0;
    SkipWhitespace(json, pos);
    
    if (pos >= json.length() || json[pos] != '{') {
        errorMessage = "Expected { at start";
        return false;
    }
    
    return true;
}

}
}
