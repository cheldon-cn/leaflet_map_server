#include "ogc/feature/geojson_converter.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_type.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/point.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/linestring.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace ogc {

struct CNGeoJSONConverter::Impl {
    bool indent_;
    int precision_;
    
    Impl() : indent_(false), precision_(15) {}
};

CNGeoJSONConverter::CNGeoJSONConverter()
    : impl_(new Impl()) {
}

CNGeoJSONConverter::~CNGeoJSONConverter() {
}

void CNGeoJSONConverter::SetIndent(bool indent) {
    impl_->indent_ = indent;
}

bool CNGeoJSONConverter::GetIndent() const {
    return impl_->indent_;
}

void CNGeoJSONConverter::SetPrecision(int precision) {
    impl_->precision_ = precision;
}

int CNGeoJSONConverter::GetPrecision() const {
    return impl_->precision_;
}

std::string CNGeoJSONConverter::GeometryToGeoJSON(GeometryPtr geometry) {
    if (!geometry) return "null";
    
    std::ostringstream oss;
    oss << std::setprecision(impl_->precision_);
    
    GeomType type = geometry->GetGeometryType();
    
    switch (type) {
        case GeomType::kPoint: {
            auto point = static_cast<const class Point*>(geometry.get());
            oss << "{\"type\":\"Point\",\"coordinates\":";
            auto coord = point->GetCoordinate();
            oss << "[" << coord.x << "," << coord.y << "]";
            oss << "}";
            break;
        }
        case GeomType::kLineString: {
            auto line = static_cast<const class LineString*>(geometry.get());
            oss << "{\"type\":\"LineString\",\"coordinates\":[";
            size_t num_points = line->GetNumPoints();
            for (size_t i = 0; i < num_points; ++i) {
                if (i > 0) oss << ",";
                auto coord = line->GetCoordinateN(i);
                oss << "[" << coord.x << "," << coord.y << "]";
            }
            oss << "]}";
            break;
        }
        case GeomType::kPolygon: {
            auto polygon = static_cast<const class Polygon*>(geometry.get());
            oss << "{\"type\":\"Polygon\",\"coordinates\":[";
            auto exterior = polygon->GetExteriorRing();
            size_t num_points = exterior->GetNumPoints();
            oss << "[";
            for (size_t i = 0; i < num_points; ++i) {
                if (i > 0) oss << ",";
                auto coord = exterior->GetCoordinateN(i);
                oss << "[" << coord.x << "," << coord.y << "]";
            }
            oss << "]";
            size_t num_rings = polygon->GetNumInteriorRings();
            for (size_t r = 0; r < num_rings; ++r) {
                auto ring = polygon->GetInteriorRingN(r);
                num_points = ring->GetNumPoints();
                oss << ",[";
                for (size_t i = 0; i < num_points; ++i) {
                    if (i > 0) oss << ",";
                    auto coord = ring->GetCoordinateN(i);
                    oss << "[" << coord.x << "," << coord.y << "]";
                }
                oss << "]";
            }
            oss << "]}";
            break;
        }
        default:
            oss << "null";
            break;
    }
    
    return oss.str();
}

std::string CNGeoJSONConverter::FeatureToGeoJSON(const CNFeature* feature, bool as_collection) {
    if (!feature) return "{}";
    
    std::ostringstream oss;
    oss << std::setprecision(impl_->precision_);
    
    if (as_collection) {
        oss << "{\"type\":\"FeatureCollection\",\"features\":[";
    } else {
        oss << "{\"type\":\"Feature\",\"geometry\":";
    }
    
    GeometryPtr geom = feature->GetGeometry();
    oss << GeometryToGeoJSON(std::move(geom));
    
    if (!as_collection) {
        oss << ",\"properties\":{";
        CNFeatureDefn* defn = feature->GetFeatureDefn();
        if (defn) {
            size_t field_count = defn->GetFieldCount();
            for (size_t i = 0; i < field_count; ++i) {
                if (i > 0) oss << ",";
                CNFieldDefn* field_defn = defn->GetFieldDefn(i);
                if (field_defn) {
                    oss << "\"" << field_defn->GetName() << "\":";
                    const CNFieldValue& field_val = feature->GetField(i);
                    CNFieldType type = field_val.GetType();
                    if (type == CNFieldType::kInteger) {
                        oss << field_val.GetInteger();
                    } else if (type == CNFieldType::kInteger64) {
                        oss << field_val.GetInteger64();
                    } else if (type == CNFieldType::kReal) {
                        oss << field_val.GetReal();
                    } else if (type == CNFieldType::kString) {
                        oss << "\"" << field_val.GetString() << "\"";
                    } else {
                        oss << "null";
                    }
                }
            }
        }
        oss << "}";
        
        int64_t fid = feature->GetFID();
        if (fid > 0) {
            oss << ",\"id\":" << fid;
        }
    }
    
    if (as_collection) {
        oss << "]}";
    } else {
        oss << "}";
    }
    
    return oss.str();
}

std::string CNGeoJSONConverter::ToGeoJSON(CNFeature* feature) {
    return FeatureToGeoJSON(feature, false);
}

std::string CNGeoJSONConverter::ToGeoJSON(const CNFeatureCollection* collection) {
    if (!collection) return "{\"type\":\"FeatureCollection\",\"features\":[]}";
    
    std::ostringstream oss;
    oss << "{\"type\":\"FeatureCollection\",\"features\":[";
    
    size_t count = collection->GetFeatureCount();
    for (size_t i = 0; i < count; ++i) {
        if (i > 0) oss << ",";
        const CNFeature* feature = collection->GetFeature(i);
        oss << FeatureToGeoJSON(feature, false);
    }
    
    oss << "]}";
    return oss.str();
}

CNFeature* CNGeoJSONConverter::FromGeoJSON(const std::string& json) {
    if (json.empty()) return nullptr;
    
    CNFeature* feature = new CNFeature();
    
    size_t geom_pos = json.find("\"geometry\"");
    if (geom_pos != std::string::npos) {
    }
    
    size_t props_pos = json.find("\"properties\"");
    if (props_pos != std::string::npos) {
    }
    
    return feature;
}

CNFeatureCollection* CNGeoJSONConverter::FromGeoJSONToCollection(const std::string& json) {
    if (json.empty()) return nullptr;
    
    CNFeatureCollection* collection = new CNFeatureCollection();
    return collection;
}

} // namespace ogc
