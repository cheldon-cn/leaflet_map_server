#pragma once

#include "common.h"
#include "result.h"
#include "resultset.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace ogc {
namespace db {

using Coordinate = ogc::Coordinate;

constexpr const char* kGeoJsonTypePoint = "Point";
constexpr const char* kGeoJsonTypeLineString = "LineString";
constexpr const char* kGeoJsonTypePolygon = "Polygon";
constexpr const char* kGeoJsonTypeMultiPoint = "MultiPoint";
constexpr const char* kGeoJsonTypeMultiLineString = "MultiLineString";
constexpr const char* kGeoJsonTypeMultiPolygon = "MultiPolygon";
constexpr const char* kGeoJsonTypeGeometryCollection = "GeometryCollection";
constexpr const char* kGeoJsonTypeFeature = "Feature";
constexpr const char* kGeoJsonTypeFeatureCollection = "FeatureCollection";

struct GeoJsonOptions {
    bool prettyPrint = false;
    int indentSpaces = 2;
    bool includeBoundingBox = false;
    bool includeCRS = false;
    bool includeAttributes = false;
    int precision = -1;
    std::string crsUri;
};

struct GeoJsonFeature {
    std::string id;
    GeometryPtr geometry;
    std::map<std::string, std::string> properties;
    std::map<std::string, double> bbox;
};

class OGC_DB_API GeoJsonConverter {
public:
    static Result GeometryToJson(const Geometry* geometry, std::string& json, const GeoJsonOptions& options = GeoJsonOptions());
    
    static Result GeometryFromJson(const std::string& json, std::unique_ptr<Geometry>& geometry);
    
    static Result FeatureToJson(const GeoJsonFeature& feature, std::string& json, const GeoJsonOptions& options = GeoJsonOptions());
    
    static Result FeatureFromJson(const std::string& json, GeoJsonFeature& feature);
    
    static Result FeatureCollectionToJson(const std::vector<GeoJsonFeature>& features, std::string& json, const GeoJsonOptions& options = GeoJsonOptions());
    
    static Result FeatureCollectionFromJson(const std::string& json, std::vector<GeoJsonFeature>& features);
    
    static Result EnvelopeToBoundingBox(const Envelope& envelope, std::vector<double>& bbox);
    
    static bool ValidateGeoJson(const std::string& json, std::string& errorMessage);
    
    static std::string GetGeometryTypeName(GeomType type);

private:
    static void WriteGeometry(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WritePoint(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WriteLineString(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WritePolygon(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WriteMultiPoint(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WriteMultiLineString(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WriteMultiPolygon(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WriteGeometryCollection(const Geometry* geometry, std::ostringstream& oss, const GeoJsonOptions& options);
    
    static void WriteCoordinate(double value, std::ostringstream& oss, const GeoJsonOptions& options);
    static void WriteCoordinates(const Coordinate& coord, std::ostringstream& oss, const GeoJsonOptions& options);
    
    static Result ReadGeometry(const std::string& json, size_t& pos, std::unique_ptr<Geometry>& geometry);
    static Result ReadCoordinate(const std::string& json, size_t& pos, Coordinate& coord);
    static Result ReadCoordinateArray(const std::string& json, size_t& pos, std::vector<Coordinate>& coords);
    static Result ReadPolygonCoordinates(const std::string& json, size_t& pos, std::vector<std::vector<Coordinate>>& rings);
    
    static void SkipWhitespace(const std::string& json, size_t& pos);
    static std::string ReadString(const std::string& json, size_t& pos);
    static double ReadNumber(const std::string& json, size_t& pos);
    static bool MatchString(const std::string& json, size_t& pos, const char* expected);
};

}
}
