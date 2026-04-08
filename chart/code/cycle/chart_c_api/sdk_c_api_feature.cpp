/**
 * @file sdk_c_api_feature.cpp
 * @brief OGC Chart SDK C API - Feature Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/feature/field_type.h>
#include <ogc/feature/field_defn.h>
#include <ogc/feature/field_value.h>
#include <ogc/feature/feature_defn.h>
#include <ogc/feature/feature.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc::feature;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

ogc_field_type_e ToCFieldType(CNFieldType type) {
    switch (type) {
        case CNFieldType::Integer: return OGC_FIELD_TYPE_INTEGER;
        case CNFieldType::Integer64: return OGC_FIELD_TYPE_INTEGER64;
        case CNFieldType::Real: return OGC_FIELD_TYPE_REAL;
        case CNFieldType::String: return OGC_FIELD_TYPE_STRING;
        case CNFieldType::Binary: return OGC_FIELD_TYPE_BINARY;
        case CNFieldType::Date: return OGC_FIELD_TYPE_DATE;
        case CNFieldType::Time: return OGC_FIELD_TYPE_TIME;
        case CNFieldType::DateTime: return OGC_FIELD_TYPE_DATETIME;
        case CNFieldType::IntegerList: return OGC_FIELD_TYPE_INTEGER_LIST;
        case CNFieldType::RealList: return OGC_FIELD_TYPE_REAL_LIST;
        case CNFieldType::StringList: return OGC_FIELD_TYPE_STRING_LIST;
        default: return OGC_FIELD_TYPE_STRING;
    }
}

CNFieldType FromCFieldType(ogc_field_type_e type) {
    switch (type) {
        case OGC_FIELD_TYPE_INTEGER: return CNFieldType::Integer;
        case OGC_FIELD_TYPE_INTEGER64: return CNFieldType::Integer64;
        case OGC_FIELD_TYPE_REAL: return CNFieldType::Real;
        case OGC_FIELD_TYPE_STRING: return CNFieldType::String;
        case OGC_FIELD_TYPE_BINARY: return CNFieldType::Binary;
        case OGC_FIELD_TYPE_DATE: return CNFieldType::Date;
        case OGC_FIELD_TYPE_TIME: return CNFieldType::Time;
        case OGC_FIELD_TYPE_DATETIME: return CNFieldType::DateTime;
        case OGC_FIELD_TYPE_INTEGER_LIST: return CNFieldType::IntegerList;
        case OGC_FIELD_TYPE_REAL_LIST: return CNFieldType::RealList;
        case OGC_FIELD_TYPE_STRING_LIST: return CNFieldType::StringList;
        default: return CNFieldType::String;
    }
}

}  

ogc_field_defn_t* ogc_field_defn_create(const char* name, ogc_field_type_e type) {
    return reinterpret_cast<ogc_field_defn_t*>(
        FieldDefn::Create(SafeString(name), FromCFieldType(type)).release());
}

void ogc_field_defn_destroy(ogc_field_defn_t* defn) {
    delete reinterpret_cast<FieldDefn*>(defn);
}

const char* ogc_field_defn_get_name(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FieldDefn*>(defn)->GetName().c_str();
    }
    return "";
}

ogc_field_type_e ogc_field_defn_get_type(const ogc_field_defn_t* defn) {
    if (defn) {
        return ToCFieldType(reinterpret_cast<const FieldDefn*>(defn)->GetType());
    }
    return OGC_FIELD_TYPE_STRING;
}

void ogc_field_defn_set_type(ogc_field_defn_t* defn, ogc_field_type_e type) {
    if (defn) {
        reinterpret_cast<FieldDefn*>(defn)->SetType(FromCFieldType(type));
    }
}

int ogc_field_defn_get_width(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FieldDefn*>(defn)->GetWidth();
    }
    return 0;
}

void ogc_field_defn_set_width(ogc_field_defn_t* defn, int width) {
    if (defn) {
        reinterpret_cast<FieldDefn*>(defn)->SetWidth(width);
    }
}

int ogc_field_defn_get_precision(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FieldDefn*>(defn)->GetPrecision();
    }
    return 0;
}

void ogc_field_defn_set_precision(ogc_field_defn_t* defn, int precision) {
    if (defn) {
        reinterpret_cast<FieldDefn*>(defn)->SetPrecision(precision);
    }
}

int ogc_field_defn_is_nullable(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FieldDefn*>(defn)->IsNullable() ? 1 : 0;
    }
    return 0;
}

void ogc_field_defn_set_nullable(ogc_field_defn_t* defn, int nullable) {
    if (defn) {
        reinterpret_cast<FieldDefn*>(defn)->SetNullable(nullable != 0);
    }
}

const char* ogc_field_defn_get_default_value(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FieldDefn*>(defn)->GetDefaultValue().c_str();
    }
    return "";
}

void ogc_field_defn_set_default_value(ogc_field_defn_t* defn, const char* value) {
    if (defn) {
        reinterpret_cast<FieldDefn*>(defn)->SetDefaultValue(SafeString(value));
    }
}

ogc_field_value_t* ogc_field_value_create(void) {
    return reinterpret_cast<ogc_field_value_t*>(new FieldValue());
}

ogc_field_value_t* ogc_field_value_create_int(int value) {
    return reinterpret_cast<ogc_field_value_t*>(new FieldValue(value));
}

ogc_field_value_t* ogc_field_value_create_int64(long long value) {
    return reinterpret_cast<ogc_field_value_t*>(new FieldValue(static_cast<int64>(value)));
}

ogc_field_value_t* ogc_field_value_create_double(double value) {
    return reinterpret_cast<ogc_field_value_t*>(new FieldValue(value));
}

ogc_field_value_t* ogc_field_value_create_string(const char* value) {
    return reinterpret_cast<ogc_field_value_t*>(new FieldValue(SafeString(value)));
}

void ogc_field_value_destroy(ogc_field_value_t* value) {
    delete reinterpret_cast<FieldValue*>(value);
}

ogc_field_type_e ogc_field_value_get_type(const ogc_field_value_t* value) {
    if (value) {
        return ToCFieldType(reinterpret_cast<const FieldValue*>(value)->GetType());
    }
    return OGC_FIELD_TYPE_STRING;
}

int ogc_field_value_is_null(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const FieldValue*>(value)->IsNull() ? 1 : 0;
    }
    return 1;
}

int ogc_field_value_get_int(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const FieldValue*>(value)->GetInteger();
    }
    return 0;
}

long long ogc_field_value_get_int64(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const FieldValue*>(value)->GetInteger64();
    }
    return 0;
}

double ogc_field_value_get_double(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const FieldValue*>(value)->GetReal();
    }
    return 0.0;
}

const char* ogc_field_value_get_string(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const FieldValue*>(value)->GetString().c_str();
    }
    return "";
}

void ogc_field_value_set_null(ogc_field_value_t* value) {
    if (value) {
        reinterpret_cast<FieldValue*>(value)->SetNull();
    }
}

void ogc_field_value_set_int(ogc_field_value_t* value, int v) {
    if (value) {
        reinterpret_cast<FieldValue*>(value)->SetInteger(v);
    }
}

void ogc_field_value_set_int64(ogc_field_value_t* value, long long v) {
    if (value) {
        reinterpret_cast<FieldValue*>(value)->SetInteger64(static_cast<int64>(v));
    }
}

void ogc_field_value_set_double(ogc_field_value_t* value, double v) {
    if (value) {
        reinterpret_cast<FieldValue*>(value)->SetReal(v);
    }
}

void ogc_field_value_set_string(ogc_field_value_t* value, const char* v) {
    if (value) {
        reinterpret_cast<FieldValue*>(value)->SetString(SafeString(v));
    }
}

ogc_feature_defn_t* ogc_feature_defn_create(const char* name) {
    return reinterpret_cast<ogc_feature_defn_t*>(
        FeatureDefn::Create(SafeString(name)).release());
}

void ogc_feature_defn_destroy(ogc_feature_defn_t* defn) {
    delete reinterpret_cast<FeatureDefn*>(defn);
}

const char* ogc_feature_defn_get_name(const ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FeatureDefn*>(defn)->GetName().c_str();
    }
    return "";
}

size_t ogc_feature_defn_get_field_count(const ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const FeatureDefn*>(defn)->GetFieldCount();
    }
    return 0;
}

ogc_field_defn_t* ogc_feature_defn_get_field_defn(const ogc_feature_defn_t* defn, size_t index) {
    if (defn) {
        return reinterpret_cast<ogc_field_defn_t*>(
            reinterpret_cast<FeatureDefn*>(const_cast<ogc_feature_defn_t*>(defn))->GetFieldDefn(index));
    }
    return nullptr;
}

int ogc_feature_defn_get_field_index(const ogc_feature_defn_t* defn, const char* name) {
    if (defn && name) {
        return reinterpret_cast<const FeatureDefn*>(defn)->GetFieldIndex(SafeString(name));
    }
    return -1;
}

void ogc_feature_defn_add_field_defn(ogc_feature_defn_t* defn, ogc_field_defn_t* field_defn) {
    if (defn && field_defn) {
        FieldDefnPtr fd(reinterpret_cast<FieldDefn*>(field_defn));
        reinterpret_cast<FeatureDefn*>(defn)->AddFieldDefn(std::move(fd));
    }
}

int ogc_feature_defn_get_geometry_field_count(const ogc_feature_defn_t* defn) {
    if (defn) {
        return static_cast<int>(reinterpret_cast<const FeatureDefn*>(defn)->GetGeomFieldCount());
    }
    return 0;
}

ogc_geom_type_e ogc_feature_defn_get_geometry_type(const ogc_feature_defn_t* defn) {
    if (defn) {
        auto type = reinterpret_cast<const FeatureDefn*>(defn)->GetGeomType();
        switch (type) {
            case ogc::geom::GeomType::kPoint: return OGC_GEOM_TYPE_POINT;
            case ogc::geom::GeomType::kLineString: return OGC_GEOM_TYPE_LINESTRING;
            case ogc::geom::GeomType::kPolygon: return OGC_GEOM_TYPE_POLYGON;
            case ogc::geom::GeomType::kMultiPoint: return OGC_GEOM_TYPE_MULTIPOINT;
            case ogc::geom::GeomType::kMultiLineString: return OGC_GEOM_TYPE_MULTILINESTRING;
            case ogc::geom::GeomType::kMultiPolygon: return OGC_GEOM_TYPE_MULTIPOLYGON;
            case ogc::geom::GeomType::kGeometryCollection: return OGC_GEOM_TYPE_GEOMETRYCOLLECTION;
            default: return OGC_GEOM_TYPE_UNKNOWN;
        }
    }
    return OGC_GEOM_TYPE_UNKNOWN;
}

void ogc_feature_defn_set_geometry_type(ogc_feature_defn_t* defn, ogc_geom_type_e type) {
    if (defn) {
        ogc::geom::GeomType geomType = ogc::geom::GeomType::kUnknown;
        switch (type) {
            case OGC_GEOM_TYPE_POINT: geomType = ogc::geom::GeomType::kPoint; break;
            case OGC_GEOM_TYPE_LINESTRING: geomType = ogc::geom::GeomType::kLineString; break;
            case OGC_GEOM_TYPE_POLYGON: geomType = ogc::geom::GeomType::kPolygon; break;
            case OGC_GEOM_TYPE_MULTIPOINT: geomType = ogc::geom::GeomType::kMultiPoint; break;
            case OGC_GEOM_TYPE_MULTILINESTRING: geomType = ogc::geom::GeomType::kMultiLineString; break;
            case OGC_GEOM_TYPE_MULTIPOLYGON: geomType = ogc::geom::GeomType::kMultiPolygon; break;
            case OGC_GEOM_TYPE_GEOMETRYCOLLECTION: geomType = ogc::geom::GeomType::kGeometryCollection; break;
            default: break;
        }
        reinterpret_cast<FeatureDefn*>(defn)->SetGeomType(geomType);
    }
}

ogc_feature_t* ogc_feature_create(ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<ogc_feature_t*>(
            Feature::Create(reinterpret_cast<FeatureDefn*>(defn)).release());
    }
    return nullptr;
}

void ogc_feature_destroy(ogc_feature_t* feature) {
    delete reinterpret_cast<Feature*>(feature);
}

long long ogc_feature_get_fid(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->GetFID();
    }
    return 0;
}

void ogc_feature_set_fid(ogc_feature_t* feature, long long fid) {
    if (feature) {
        reinterpret_cast<Feature*>(feature)->SetFID(fid);
    }
}

ogc_feature_defn_t* ogc_feature_get_defn(ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<ogc_feature_defn_t*>(
            reinterpret_cast<Feature*>(feature)->GetDefn());
    }
    return nullptr;
}

size_t ogc_feature_get_field_count(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->GetFieldCount();
    }
    return 0;
}

int ogc_feature_is_field_null(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->IsFieldNull(index) ? 1 : 0;
    }
    return 1;
}

int ogc_feature_is_field_set(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->IsFieldSet(index) ? 1 : 0;
    }
    return 0;
}

void ogc_feature_set_field_null(ogc_feature_t* feature, size_t index) {
    if (feature) {
        reinterpret_cast<Feature*>(feature)->SetFieldNull(index);
    }
}

int ogc_feature_get_field_as_int(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->GetFieldAsInteger(index);
    }
    return 0;
}

long long ogc_feature_get_field_as_int64(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->GetFieldAsInteger64(index);
    }
    return 0;
}

double ogc_feature_get_field_as_double(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->GetFieldAsDouble(index);
    }
    return 0.0;
}

const char* ogc_feature_get_field_as_string(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const Feature*>(feature)->GetFieldAsString(index).c_str();
    }
    return "";
}

void ogc_feature_set_field_int(ogc_feature_t* feature, size_t index, int value) {
    if (feature) {
        reinterpret_cast<Feature*>(feature)->SetField(index, value);
    }
}

void ogc_feature_set_field_int64(ogc_feature_t* feature, size_t index, long long value) {
    if (feature) {
        reinterpret_cast<Feature*>(feature)->SetField(index, static_cast<int64>(value));
    }
}

void ogc_feature_set_field_double(ogc_feature_t* feature, size_t index, double value) {
    if (feature) {
        reinterpret_cast<Feature*>(feature)->SetField(index, value);
    }
}

void ogc_feature_set_field_string(ogc_feature_t* feature, size_t index, const char* value) {
    if (feature) {
        reinterpret_cast<Feature*>(feature)->SetField(index, SafeString(value));
    }
}

int ogc_feature_get_field_index(const ogc_feature_t* feature, const char* name) {
    if (feature && name) {
        return reinterpret_cast<const Feature*>(feature)->GetFieldIndex(SafeString(name));
    }
    return -1;
}

ogc_geometry_t* ogc_feature_get_geometry(ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<ogc_geometry_t*>(
            reinterpret_cast<Feature*>(feature)->GetGeometry());
    }
    return nullptr;
}

const ogc_geometry_t* ogc_feature_get_geometry_const(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<const ogc_geometry_t*>(
            reinterpret_cast<const Feature*>(feature)->GetGeometry());
    }
    return nullptr;
}

void ogc_feature_set_geometry(ogc_feature_t* feature, ogc_geometry_t* geom) {
    if (feature && geom) {
        ogc::geom::GeometryPtr g(reinterpret_cast<ogc::geom::Geometry*>(geom));
        reinterpret_cast<Feature*>(feature)->SetGeometry(std::move(g));
    }
}

void ogc_feature_set_geometry_directly(ogc_feature_t* feature, ogc_geometry_t* geom) {
    if (feature && geom) {
        ogc::geom::GeometryPtr g(reinterpret_cast<ogc::geom::Geometry*>(geom));
        reinterpret_cast<Feature*>(feature)->SetGeometry(std::move(g));
    }
}

ogc_feature_t* ogc_feature_clone(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<const Feature*>(feature)->Clone().release());
    }
    return nullptr;
}

int ogc_feature_equal(const ogc_feature_t* a, const ogc_feature_t* b) {
    if (a && b) {
        return reinterpret_cast<const Feature*>(a)->Equal(
            reinterpret_cast<const Feature*>(b)) ? 1 : 0;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
