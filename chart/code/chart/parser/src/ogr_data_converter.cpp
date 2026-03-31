#include "chart_parser/ogr_data_converter.h"
#include "chart_parser/error_handler.h"

#include <ogr_geometry.h>
#include <ogr_feature.h>

namespace chart {
namespace parser {

namespace {
    REGISTER_DATA_CONVERTER("OGR", OGRDataConverter);
}

OGRDataConverter::OGRDataConverter() {
}

OGRDataConverter::~OGRDataConverter() {
}

bool OGRDataConverter::ConvertGeometry(void* sourceGeometry, Geometry& outGeometry) {
    OGRGeometry* geom = static_cast<OGRGeometry*>(sourceGeometry);
    
    if (!geom) {
        LOG_ERROR("Null geometry");
        return false;
    }
    
    OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());
    
    switch (type) {
        case wkbPoint:
            return ConvertPoint(geom, outGeometry);
        
        case wkbLineString:
            return ConvertLineString(geom, outGeometry);
        
        case wkbPolygon:
            return ConvertPolygon(geom, outGeometry);
        
        case wkbMultiPoint:
            return ConvertMultiPoint(geom, outGeometry);
        
        case wkbMultiLineString:
            return ConvertMultiLineString(geom, outGeometry);
        
        case wkbMultiPolygon:
            return ConvertMultiPolygon(geom, outGeometry);
        
        default:
            LOG_WARN("Unsupported OGR geometry type: {}", static_cast<int>(type));
            return false;
    }
}

bool OGRDataConverter::ConvertAttributes(void* sourceFeature, AttributeMap& outAttributes) {
    OGRFeature* feature = static_cast<OGRFeature*>(sourceFeature);
    
    if (!feature) {
        LOG_ERROR("Null feature");
        return false;
    }
    
    OGRFeatureDefn* featureDefn = feature->GetDefnRef();
    if (!featureDefn) {
        return false;
    }
    
    int fieldCount = featureDefn->GetFieldCount();
    
    for (int i = 0; i < fieldCount; ++i) {
        OGRFieldDefn* fieldDefn = featureDefn->GetFieldDefn(i);
        if (!fieldDefn) {
            continue;
        }
        
        std::string fieldName = fieldDefn->GetNameRef();
        
        if (!feature->IsFieldSet(i) || feature->IsFieldNull(i)) {
            continue;
        }
        
        AttributeValue value;
        OGRFieldType fieldType = fieldDefn->GetType();
        
        switch (fieldType) {
            case OFTInteger:
                value.type = AttributeValue::Type::Integer;
                value.intValue = feature->GetFieldAsInteger(i);
                break;
            
            case OFTInteger64:
                value.type = AttributeValue::Type::Integer;
                value.intValue = static_cast<int>(feature->GetFieldAsInteger64(i));
                break;
            
            case OFTReal:
                value.type = AttributeValue::Type::Double;
                value.doubleValue = feature->GetFieldAsDouble(i);
                break;
            
            case OFTString:
                value.type = AttributeValue::Type::String;
                value.stringValue = feature->GetFieldAsString(i);
                break;
            
            default:
                value.type = AttributeValue::Type::String;
                value.stringValue = feature->GetFieldAsString(i);
                break;
        }
        
        outAttributes[fieldName] = value;
    }
    
    return true;
}

bool OGRDataConverter::ConvertPoint(void* geometry, Geometry& outGeometry) {
    OGRPoint* point = static_cast<OGRPoint*>(geometry);
    
    outGeometry.type = GeometryType::Point;
    outGeometry.points.clear();
    
    Point pt;
    pt.x = point->getX();
    pt.y = point->getY();
    pt.z = point->getZ();
    outGeometry.points.push_back(pt);
    
    return true;
}

bool OGRDataConverter::ConvertLineString(void* geometry, Geometry& outGeometry) {
    OGRLineString* lineString = static_cast<OGRLineString*>(geometry);
    
    outGeometry.type = GeometryType::Line;
    outGeometry.points.clear();
    
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

bool OGRDataConverter::ConvertPolygon(void* geometry, Geometry& outGeometry) {
    OGRPolygon* polygon = static_cast<OGRPolygon*>(geometry);
    
    outGeometry.type = GeometryType::Area;
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

bool OGRDataConverter::ConvertMultiPoint(void* geometry, Geometry& outGeometry) {
    OGRMultiPoint* multiPoint = static_cast<OGRMultiPoint*>(geometry);
    
    outGeometry.type = GeometryType::MultiPoint;
    outGeometry.points.clear();
    
    int numGeometries = multiPoint->getNumGeometries();
    outGeometry.points.reserve(numGeometries);
    
    for (int i = 0; i < numGeometries; ++i) {
        OGRPoint* point = static_cast<OGRPoint*>(multiPoint->getGeometryRef(i));
        if (point) {
            Point pt;
            pt.x = point->getX();
            pt.y = point->getY();
            pt.z = point->getZ();
            outGeometry.points.push_back(pt);
        }
    }
    
    return true;
}

bool OGRDataConverter::ConvertMultiLineString(void* geometry, Geometry& outGeometry) {
    OGRMultiLineString* multiLine = static_cast<OGRMultiLineString*>(geometry);
    
    outGeometry.type = GeometryType::MultiLine;
    outGeometry.rings.clear();
    
    int numGeometries = multiLine->getNumGeometries();
    
    for (int i = 0; i < numGeometries; ++i) {
        OGRLineString* lineString = static_cast<OGRLineString*>(multiLine->getGeometryRef(i));
        if (lineString) {
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

bool OGRDataConverter::ConvertMultiPolygon(void* geometry, Geometry& outGeometry) {
    OGRMultiPolygon* multiPolygon = static_cast<OGRMultiPolygon*>(geometry);
    
    outGeometry.type = GeometryType::MultiArea;
    outGeometry.rings.clear();
    
    int numGeometries = multiPolygon->getNumGeometries();
    
    for (int i = 0; i < numGeometries; ++i) {
        OGRPolygon* polygon = static_cast<OGRPolygon*>(multiPolygon->getGeometryRef(i));
        if (polygon) {
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
