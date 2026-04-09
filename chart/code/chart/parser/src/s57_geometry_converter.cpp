#include "parser/s57_geometry_converter.h"
#include "parser/error_handler.h"

#include <ogr_geometry.h>

namespace chart {
namespace parser {

S57GeometryConverter::S57GeometryConverter() {
}

S57GeometryConverter::~S57GeometryConverter() {
}

bool S57GeometryConverter::ConvertGeometry(void* ogrGeometry, Geometry& outGeometry) {
    OGRGeometry* geom = static_cast<OGRGeometry*>(ogrGeometry);
    
    if (!geom) {
        LOG_ERROR("Null geometry");
        return false;
    }
    
    OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());
    
    switch (type) {
        case wkbPoint:
            return ConvertPoint(geom, outGeometry);
        
        case wkbLineString:
        case wkbCurve:
            return ConvertLineString(geom, outGeometry);
        
        case wkbPolygon:
        case wkbSurface:
            return ConvertPolygon(geom, outGeometry);
        
        case wkbMultiPoint:
            return ConvertMultiPoint(geom, outGeometry);
        
        case wkbMultiLineString:
        case wkbMultiCurve:
            return ConvertMultiLineString(geom, outGeometry);
        
        case wkbMultiPolygon:
        case wkbMultiSurface:
            return ConvertMultiPolygon(geom, outGeometry);
        
        default:
            LOG_WARN("Unsupported geometry type: {}", static_cast<int>(type));
            return false;
    }
}

bool S57GeometryConverter::ConvertPoint(void* geom, Geometry& outGeometry) {
    OGRPoint* point = static_cast<OGRPoint*>(geom);
    
    outGeometry.type = GeometryType::Point;
    outGeometry.points.clear();
    outGeometry.rings.clear();
    
    Point pt;
    pt.x = point->getX();
    pt.y = point->getY();
    pt.z = point->getZ();
    
    outGeometry.points.push_back(pt);
    
    return true;
}

bool S57GeometryConverter::ConvertLineString(void* geom, Geometry& outGeometry) {
    OGRLineString* lineString = static_cast<OGRLineString*>(geom);
    
    outGeometry.type = GeometryType::Line;
    outGeometry.points.clear();
    outGeometry.rings.clear();
    
    int pointCount = lineString->getNumPoints();
    outGeometry.points.reserve(pointCount);
    
    for (int i = 0; i < pointCount; ++i) {
        Point pt;
        pt.x = lineString->getX(i);
        pt.y = lineString->getY(i);
        pt.z = lineString->getZ(i);
        outGeometry.points.push_back(pt);
    }
    
    return true;
}

bool S57GeometryConverter::ConvertPolygon(void* geom, Geometry& outGeometry) {
    OGRPolygon* polygon = static_cast<OGRPolygon*>(geom);
    
    outGeometry.type = GeometryType::Area;
    outGeometry.points.clear();
    outGeometry.rings.clear();
    
    OGRLinearRing* exteriorRing = polygon->getExteriorRing();
    if (exteriorRing) {
        std::vector<Point> ring;
        int pointCount = exteriorRing->getNumPoints();
        ring.reserve(pointCount);
        
        for (int i = 0; i < pointCount; ++i) {
            Point pt;
            pt.x = exteriorRing->getX(i);
            pt.y = exteriorRing->getY(i);
            pt.z = exteriorRing->getZ(i);
            ring.push_back(pt);
        }
        
        outGeometry.rings.push_back(ring);
    }
    
    int interiorRingCount = polygon->getNumInteriorRings();
    for (int i = 0; i < interiorRingCount; ++i) {
        OGRLinearRing* interiorRing = polygon->getInteriorRing(i);
        if (interiorRing) {
            std::vector<Point> ring;
            int pointCount = interiorRing->getNumPoints();
            ring.reserve(pointCount);
            
            for (int j = 0; j < pointCount; ++j) {
                Point pt;
                pt.x = interiorRing->getX(j);
                pt.y = interiorRing->getY(j);
                pt.z = interiorRing->getZ(j);
                ring.push_back(pt);
            }
            
            outGeometry.rings.push_back(ring);
        }
    }
    
    return true;
}

bool S57GeometryConverter::ConvertMultiPoint(void* geom, Geometry& outGeometry) {
    OGRMultiPoint* multiPoint = static_cast<OGRMultiPoint*>(geom);
    
    outGeometry.type = GeometryType::MultiPoint;
    outGeometry.points.clear();
    outGeometry.rings.clear();
    
    int numGeometries = multiPoint->getNumGeometries();
    outGeometry.points.reserve(numGeometries);
    
    for (int i = 0; i < numGeometries; ++i) {
        OGRGeometry* subGeom = multiPoint->getGeometryRef(i);
        if (subGeom && wkbFlatten(subGeom->getGeometryType()) == wkbPoint) {
            OGRPoint* point = static_cast<OGRPoint*>(subGeom);
            Point pt;
            pt.x = point->getX();
            pt.y = point->getY();
            pt.z = point->getZ();
            outGeometry.points.push_back(pt);
        }
    }
    
    return true;
}

bool S57GeometryConverter::ConvertMultiLineString(void* geom, Geometry& outGeometry) {
    OGRMultiLineString* multiLine = static_cast<OGRMultiLineString*>(geom);
    
    outGeometry.type = GeometryType::MultiLine;
    outGeometry.points.clear();
    outGeometry.rings.clear();
    
    int numGeometries = multiLine->getNumGeometries();
    
    for (int i = 0; i < numGeometries; ++i) {
        OGRGeometry* subGeom = multiLine->getGeometryRef(i);
        if (subGeom && wkbFlatten(subGeom->getGeometryType()) == wkbLineString) {
            OGRLineString* lineString = static_cast<OGRLineString*>(subGeom);
            std::vector<Point> ring;
            int pointCount = lineString->getNumPoints();
            ring.reserve(pointCount);
            
            for (int j = 0; j < pointCount; ++j) {
                Point pt;
                pt.x = lineString->getX(j);
                pt.y = lineString->getY(j);
                pt.z = lineString->getZ(j);
                ring.push_back(pt);
            }
            
            outGeometry.rings.push_back(ring);
        }
    }
    
    return true;
}

bool S57GeometryConverter::ConvertMultiPolygon(void* geom, Geometry& outGeometry) {
    OGRMultiPolygon* multiPolygon = static_cast<OGRMultiPolygon*>(geom);
    
    outGeometry.type = GeometryType::MultiArea;
    outGeometry.points.clear();
    outGeometry.rings.clear();
    
    int numGeometries = multiPolygon->getNumGeometries();
    
    for (int i = 0; i < numGeometries; ++i) {
        OGRGeometry* subGeom = multiPolygon->getGeometryRef(i);
        if (subGeom && wkbFlatten(subGeom->getGeometryType()) == wkbPolygon) {
            OGRPolygon* polygon = static_cast<OGRPolygon*>(subGeom);
            
            OGRLinearRing* exteriorRing = polygon->getExteriorRing();
            if (exteriorRing) {
                std::vector<Point> ring;
                int pointCount = exteriorRing->getNumPoints();
                ring.reserve(pointCount);
                
                for (int j = 0; j < pointCount; ++j) {
                    Point pt;
                    pt.x = exteriorRing->getX(j);
                    pt.y = exteriorRing->getY(j);
                    pt.z = exteriorRing->getZ(j);
                    ring.push_back(pt);
                }
                
                outGeometry.rings.push_back(ring);
            }
        }
    }
    
    return true;
}

} // namespace parser
} // namespace chart
