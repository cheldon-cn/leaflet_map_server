#include "ogc/draw/coordinate_transformer.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/linearring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include <cmath>

namespace ogc {
namespace draw {

namespace {
    const double PI = 3.14159265358979323846;
    const double EARTH_RADIUS = 6378137.0;
    const double MAX_LAT = 85.0511287798066;
    
    double DegToRad(double deg) {
        return deg * PI / 180.0;
    }
    
    double RadToDeg(double rad) {
        return rad * 180.0 / PI;
    }
}

class SimpleCoordinateTransformer : public CoordinateTransformer {
public:
    SimpleCoordinateTransformer(const std::string& sourceCRS, const std::string& targetCRS)
        : m_sourceCRS(sourceCRS)
        , m_targetCRS(targetCRS)
        , m_valid(true) {
    }
    
    bool IsValid() const override {
        return m_valid;
    }
    
    Coordinate Transform(const Coordinate& coord) const override {
        Coordinate result = coord;
        Transform(result.x, result.y);
        return result;
    }
    
    Coordinate TransformInverse(const Coordinate& coord) const override {
        Coordinate result = coord;
        TransformInverse(result.x, result.y);
        return result;
    }
    
    void Transform(double& x, double& y) const override {
        if (IsWGS84ToWebMercator()) {
            WGS84ToWebMercator(x, y);
        } else if (IsWebMercatorToWGS84()) {
            WebMercatorToWGS84(x, y);
        }
    }
    
    void TransformInverse(double& x, double& y) const override {
        if (IsWGS84ToWebMercator()) {
            WebMercatorToWGS84(x, y);
        } else if (IsWebMercatorToWGS84()) {
            WGS84ToWebMercator(x, y);
        }
    }
    
    void TransformArray(double* x, double* y, size_t count) const override {
        for (size_t i = 0; i < count; ++i) {
            Transform(x[i], y[i]);
        }
    }
    
    void TransformArrayInverse(double* x, double* y, size_t count) const override {
        for (size_t i = 0; i < count; ++i) {
            TransformInverse(x[i], y[i]);
        }
    }
    
    Envelope Transform(const Envelope& env) const override {
        double minX = env.GetMinX();
        double minY = env.GetMinY();
        double maxX = env.GetMaxX();
        double maxY = env.GetMaxY();
        
        Transform(minX, minY);
        Transform(maxX, maxY);
        
        return Envelope(minX, minY, maxX, maxY);
    }
    
    Envelope TransformInverse(const Envelope& env) const override {
        double minX = env.GetMinX();
        double minY = env.GetMinY();
        double maxX = env.GetMaxX();
        double maxY = env.GetMaxY();
        
        TransformInverse(minX, minY);
        TransformInverse(maxX, maxY);
        
        return Envelope(minX, minY, maxX, maxY);
    }
    
    GeometryPtr Transform(const Geometry* geometry) const override {
        if (!geometry || !m_valid) {
            return nullptr;
        }
        
        GeomType type = geometry->GetGeometryType();
        
        switch (type) {
            case GeomType::kPoint:
                return TransformPoint(static_cast<const Point*>(geometry));
            case GeomType::kLineString:
                return TransformLineString(static_cast<const LineString*>(geometry));
            case GeomType::kPolygon:
                return TransformPolygon(static_cast<const Polygon*>(geometry));
            case GeomType::kMultiPoint:
                return TransformMultiPoint(static_cast<const MultiPoint*>(geometry));
            case GeomType::kMultiLineString:
                return TransformMultiLineString(static_cast<const MultiLineString*>(geometry));
            case GeomType::kMultiPolygon:
                return TransformMultiPolygon(static_cast<const MultiPolygon*>(geometry));
            default:
                return nullptr;
        }
    }
    
    GeometryPtr TransformInverse(const Geometry* geometry) const override {
        if (!geometry || !m_valid) {
            return nullptr;
        }
        
        GeomType type = geometry->GetGeometryType();
        
        switch (type) {
            case GeomType::kPoint:
                return TransformPointInverse(static_cast<const Point*>(geometry));
            case GeomType::kLineString:
                return TransformLineStringInverse(static_cast<const LineString*>(geometry));
            case GeomType::kPolygon:
                return TransformPolygonInverse(static_cast<const Polygon*>(geometry));
            case GeomType::kMultiPoint:
                return TransformMultiPointInverse(static_cast<const MultiPoint*>(geometry));
            case GeomType::kMultiLineString:
                return TransformMultiLineStringInverse(static_cast<const MultiLineString*>(geometry));
            case GeomType::kMultiPolygon:
                return TransformMultiPolygonInverse(static_cast<const MultiPolygon*>(geometry));
            default:
                return nullptr;
        }
    }
    
    std::string GetSourceCRS() const override {
        return m_sourceCRS;
    }
    
    std::string GetTargetCRS() const override {
        return m_targetCRS;
    }
    
    std::string GetName() const override {
        return m_sourceCRS + " -> " + m_targetCRS;
    }
    
    std::string GetDescription() const override {
        return "Coordinate transformation from " + m_sourceCRS + " to " + m_targetCRS;
    }
    
    CoordinateTransformerPtr Clone() const override {
        return std::make_shared<SimpleCoordinateTransformer>(m_sourceCRS, m_targetCRS);
    }

private:
    bool IsWGS84ToWebMercator() const {
        return (m_sourceCRS == "EPSG:4326" || m_sourceCRS == "WGS84") &&
               (m_targetCRS == "EPSG:3857" || m_targetCRS == "WebMercator");
    }
    
    bool IsWebMercatorToWGS84() const {
        return (m_sourceCRS == "EPSG:3857" || m_sourceCRS == "WebMercator") &&
               (m_targetCRS == "EPSG:4326" || m_targetCRS == "WGS84");
    }
    
    void WGS84ToWebMercator(double& x, double& y) const {
        double lon = x;
        double lat = std::max(-MAX_LAT, std::min(MAX_LAT, y));
        
        x = EARTH_RADIUS * DegToRad(lon);
        y = EARTH_RADIUS * std::log(std::tan(PI / 4.0 + DegToRad(lat) / 2.0));
    }
    
    void WebMercatorToWGS84(double& x, double& y) const {
        double lon = RadToDeg(x / EARTH_RADIUS);
        double lat = RadToDeg(2.0 * std::atan(std::exp(y / EARTH_RADIUS)) - PI / 2.0);
        
        x = lon;
        y = lat;
    }
    
    GeometryPtr TransformPoint(const Point* point) const {
        if (!point) return nullptr;
        
        Coordinate coord = point->GetCoordinate();
        Transform(coord.x, coord.y);
        return Point::Create(coord);
    }
    
    GeometryPtr TransformLineString(const LineString* line) const {
        if (!line) return nullptr;
        
        CoordinateList coords = line->GetCoordinates();
        for (auto& coord : coords) {
            Transform(coord.x, coord.y);
        }
        return LineString::Create(coords);
    }
    
    GeometryPtr TransformPolygon(const Polygon* polygon) const {
        if (!polygon) return nullptr;
        
        auto exteriorRing = polygon->GetExteriorRing();
        if (!exteriorRing) return nullptr;
        
        CoordinateList exteriorCoords = exteriorRing->GetCoordinates();
        for (auto& coord : exteriorCoords) {
            Transform(coord.x, coord.y);
        }
        
        auto exteriorRingPtr = LinearRing::Create(exteriorCoords);
        return Polygon::Create(std::move(exteriorRingPtr));
    }
    
    GeometryPtr TransformMultiPoint(const MultiPoint* multiPoint) const {
        if (!multiPoint) return nullptr;
        
        CoordinateList coords = multiPoint->GetCoordinates();
        for (auto& coord : coords) {
            Transform(coord.x, coord.y);
        }
        return MultiPoint::Create(coords);
    }
    
    GeometryPtr TransformMultiLineString(const MultiLineString* multiLine) const {
        if (!multiLine) return nullptr;
        
        std::vector<LineStringPtr> lines;
        for (size_t i = 0; i < multiLine->GetNumLineStrings(); ++i) {
            auto line = multiLine->GetLineStringN(i);
            if (!line) continue;
            
            CoordinateList coords = line->GetCoordinates();
            for (auto& coord : coords) {
                Transform(coord.x, coord.y);
            }
            lines.push_back(LineString::Create(coords));
        }
        return MultiLineString::Create(std::move(lines));
    }
    
    GeometryPtr TransformMultiPolygon(const MultiPolygon* multiPolygon) const {
        if (!multiPolygon) return nullptr;
        
        std::vector<PolygonPtr> polygons;
        for (size_t i = 0; i < multiPolygon->GetNumPolygons(); ++i) {
            const Polygon* poly = multiPolygon->GetPolygonN(i);
            if (!poly) continue;
            
            auto transformed = TransformPolygon(poly);
            if (transformed) {
                polygons.push_back(PolygonPtr(static_cast<Polygon*>(transformed.release())));
            }
        }
        return MultiPolygon::Create(std::move(polygons));
    }
    
    GeometryPtr TransformPointInverse(const Point* point) const {
        if (!point) return nullptr;
        
        Coordinate coord = point->GetCoordinate();
        TransformInverse(coord.x, coord.y);
        return Point::Create(coord);
    }
    
    GeometryPtr TransformLineStringInverse(const LineString* line) const {
        if (!line) return nullptr;
        
        CoordinateList coords = line->GetCoordinates();
        for (auto& coord : coords) {
            TransformInverse(coord.x, coord.y);
        }
        return LineString::Create(coords);
    }
    
    GeometryPtr TransformPolygonInverse(const Polygon* polygon) const {
        if (!polygon) return nullptr;
        
        auto exteriorRing = polygon->GetExteriorRing();
        if (!exteriorRing) return nullptr;
        
        CoordinateList exteriorCoords = exteriorRing->GetCoordinates();
        for (auto& coord : exteriorCoords) {
            TransformInverse(coord.x, coord.y);
        }
        
        auto exteriorRingPtr = LinearRing::Create(exteriorCoords);
        return Polygon::Create(std::move(exteriorRingPtr));
    }
    
    GeometryPtr TransformMultiPointInverse(const MultiPoint* multiPoint) const {
        if (!multiPoint) return nullptr;
        
        CoordinateList coords = multiPoint->GetCoordinates();
        for (auto& coord : coords) {
            TransformInverse(coord.x, coord.y);
        }
        return MultiPoint::Create(coords);
    }
    
    GeometryPtr TransformMultiLineStringInverse(const MultiLineString* multiLine) const {
        if (!multiLine) return nullptr;
        
        std::vector<LineStringPtr> lines;
        for (size_t i = 0; i < multiLine->GetNumLineStrings(); ++i) {
            auto line = multiLine->GetLineStringN(i);
            if (!line) continue;
            
            CoordinateList coords = line->GetCoordinates();
            for (auto& coord : coords) {
                TransformInverse(coord.x, coord.y);
            }
            lines.push_back(LineString::Create(coords));
        }
        return MultiLineString::Create(std::move(lines));
    }
    
    GeometryPtr TransformMultiPolygonInverse(const MultiPolygon* multiPolygon) const {
        if (!multiPolygon) return nullptr;
        
        std::vector<PolygonPtr> polygons;
        for (size_t i = 0; i < multiPolygon->GetNumPolygons(); ++i) {
            const Polygon* poly = multiPolygon->GetPolygonN(i);
            if (!poly) continue;
            
            auto transformed = TransformPolygonInverse(poly);
            if (transformed) {
                polygons.push_back(PolygonPtr(static_cast<Polygon*>(transformed.release())));
            }
        }
        return MultiPolygon::Create(std::move(polygons));
    }
    
    std::string m_sourceCRS;
    std::string m_targetCRS;
    bool m_valid;
};

CoordinateTransformerPtr CoordinateTransformer::Create(const std::string& sourceCRS, const std::string& targetCRS) {
    return std::make_shared<SimpleCoordinateTransformer>(sourceCRS, targetCRS);
}

CoordinateTransformerPtr CoordinateTransformer::CreateWGS84ToWebMercator() {
    return Create("EPSG:4326", "EPSG:3857");
}

CoordinateTransformerPtr CoordinateTransformer::CreateWebMercatorToWGS84() {
    return Create("EPSG:3857", "EPSG:4326");
}

}
}
