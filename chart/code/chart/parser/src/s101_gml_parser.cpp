#include "chart_parser/s101_gml_parser.h"
#include "chart_parser/error_handler.h"

#include <ogr_geometry.h>
#include <ogr_feature.h>

namespace chart {
namespace parser {

S101GMLParser::S101GMLParser() {
}

S101GMLParser::~S101GMLParser() {
}

bool S101GMLParser::ConvertGeometry(void* ogrGeometry, Geometry& outGeometry) {
    OGRGeometry* geom = static_cast<OGRGeometry*>(ogrGeometry);
    
    if (!geom) {
        LOG_ERROR("Null geometry");
        return false;
    }
    
    OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());
    
    switch (type) {
        case wkbPoint: {
            OGRPoint* point = static_cast<OGRPoint*>(geom);
            outGeometry.type = GeometryType::Point;
            outGeometry.points.clear();
            
            Point pt;
            pt.x = point->getX();
            pt.y = point->getY();
            pt.z = point->getZ();
            outGeometry.points.push_back(pt);
            break;
        }
        
        case wkbLineString: {
            OGRLineString* lineString = static_cast<OGRLineString*>(geom);
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
            break;
        }
        
        case wkbPolygon: {
            OGRPolygon* polygon = static_cast<OGRPolygon*>(geom);
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
            break;
        }
        
        default:
            LOG_WARN("Unsupported GML geometry type: {}", static_cast<int>(type));
            return false;
    }
    
    return true;
}

void S101GMLParser::ParseAttributes(void* feature, AttributeMap& attributes) {
    OGRFeature* ogrFeature = static_cast<OGRFeature*>(feature);
    
    if (!ogrFeature) {
        LOG_ERROR("Null feature");
        return;
    }
    
    OGRFeatureDefn* featureDefn = ogrFeature->GetDefnRef();
    if (!featureDefn) {
        return;
    }
    
    int fieldCount = featureDefn->GetFieldCount();
    
    for (int i = 0; i < fieldCount; ++i) {
        OGRFieldDefn* fieldDefn = featureDefn->GetFieldDefn(i);
        if (!fieldDefn) {
            continue;
        }
        
        std::string fieldName = fieldDefn->GetNameRef();
        
        if (!ogrFeature->IsFieldSet(i) || ogrFeature->IsFieldNull(i)) {
            continue;
        }
        
        AttributeValue value;
        OGRFieldType fieldType = fieldDefn->GetType();
        
        switch (fieldType) {
            case OFTInteger:
                value.type = AttributeValue::Type::Integer;
                value.intValue = ogrFeature->GetFieldAsInteger(i);
                break;
            
            case OFTInteger64:
                value.type = AttributeValue::Type::Integer;
                value.intValue = static_cast<int>(ogrFeature->GetFieldAsInteger64(i));
                break;
            
            case OFTReal:
                value.type = AttributeValue::Type::Double;
                value.doubleValue = ogrFeature->GetFieldAsDouble(i);
                break;
            
            case OFTString:
                value.type = AttributeValue::Type::String;
                value.stringValue = ogrFeature->GetFieldAsString(i);
                break;
            
            default:
                value.type = AttributeValue::Type::String;
                value.stringValue = ogrFeature->GetFieldAsString(i);
                break;
        }
        
        attributes[fieldName] = value;
    }
}

} // namespace parser
} // namespace chart
