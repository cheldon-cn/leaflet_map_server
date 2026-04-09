#pragma once

#include "geometry.h"
#include "point.h"
#include "linestring.h"
#include "linearring.h"
#include "polygon.h"
#include "multipoint.h"
#include "multilinestring.h"
#include "multipolygon.h"
#include "geometrycollection.h"

namespace ogc {

/**
 * @brief 几何工厂�?
 * 
 * 提供创建各种几何对象的静态方法，支持从WKT、WKB、GeoJSON等格式解�?
 */
class OGC_GEOM_API GeometryFactory {
public:
    static GeometryFactory& GetInstance();
    
    GeomResult FromWKT(const std::string& wkt, GeometryPtr& result);
    
    PointPtr CreatePointFromWKT(const std::string& wkt);
    LineStringPtr CreateLineStringFromWKT(const std::string& wkt);
    PolygonPtr CreatePolygonFromWKT(const std::string& wkt);
    
    GeomResult FromWKB(const std::vector<uint8_t>& wkb, GeometryPtr& result);
    GeomResult FromWKB(const uint8_t* data, size_t size, GeometryPtr& result);
    
    GeomResult FromGeoJSON(const std::string& json, GeometryPtr& result);
    
    PointPtr CreatePoint(double x, double y);
    PointPtr CreatePoint(double x, double y, double z);
    PointPtr CreatePoint(const Coordinate& coord);
    
    LineStringPtr CreateLineString(const CoordinateList& coords);
    LinearRingPtr CreateLinearRing(const CoordinateList& coords);
    
    PolygonPtr CreatePolygon(const CoordinateList& exteriorRing);
    PolygonPtr CreatePolygon(const std::vector<CoordinateList>& rings);
    
    MultiPointPtr CreateMultiPoint(const CoordinateList& coords);
    
    PolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    PolygonPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
    PolygonPtr CreateRegularPolygon(double centerX, double centerY, double radius, int sides);
    PolygonPtr CreateTriangle(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3);
    
    GeometryPtr CreateEmptyGeometry(GeomType type);
    
    void SetDefaultSRID(int srid) noexcept { m_defaultSRID = srid; }
    int GetDefaultSRID() const noexcept { return m_defaultSRID; }
    
private:
    GeometryFactory() = default;
    
    int m_defaultSRID = 0;
    
    class WKTReader;
    class WKBReader;
    class GeoJSONReader;
};

}
