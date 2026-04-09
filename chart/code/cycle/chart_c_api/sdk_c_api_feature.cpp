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
#include <ogc/feature/geom_field_defn.h>
#include <ogc/feature/feature.h>
#include <ogc/geom/geometry.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

static char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

static ogc_field_type_e ToCFieldType(CNFieldType type) {
    switch (type) {
        case CNFieldType::kInteger: return OGC_FIELD_TYPE_INTEGER;
        case CNFieldType::kInteger64: return OGC_FIELD_TYPE_INTEGER64;
        case CNFieldType::kReal: return OGC_FIELD_TYPE_REAL;
        case CNFieldType::kString: return OGC_FIELD_TYPE_STRING;
        case CNFieldType::kBinary: return OGC_FIELD_TYPE_BINARY;
        case CNFieldType::kDate: return OGC_FIELD_TYPE_DATE;
        case CNFieldType::kTime: return OGC_FIELD_TYPE_TIME;
        case CNFieldType::kDateTime: return OGC_FIELD_TYPE_DATETIME;
        case CNFieldType::kIntegerList: return OGC_FIELD_TYPE_INTEGER_LIST;
        case CNFieldType::kRealList: return OGC_FIELD_TYPE_REAL_LIST;
        case CNFieldType::kStringList: return OGC_FIELD_TYPE_STRING_LIST;
        default: return OGC_FIELD_TYPE_STRING;
    }
}

CNFieldType FromCFieldType(ogc_field_type_e type) {
    switch (type) {
        case OGC_FIELD_TYPE_INTEGER: return CNFieldType::kInteger;
        case OGC_FIELD_TYPE_INTEGER64: return CNFieldType::kInteger64;
        case OGC_FIELD_TYPE_REAL: return CNFieldType::kReal;
        case OGC_FIELD_TYPE_STRING: return CNFieldType::kString;
        case OGC_FIELD_TYPE_BINARY: return CNFieldType::kBinary;
        case OGC_FIELD_TYPE_DATE: return CNFieldType::kDate;
        case OGC_FIELD_TYPE_TIME: return CNFieldType::kTime;
        case OGC_FIELD_TYPE_DATETIME: return CNFieldType::kDateTime;
        case OGC_FIELD_TYPE_INTEGER_LIST: return CNFieldType::kIntegerList;
        case OGC_FIELD_TYPE_REAL_LIST: return CNFieldType::kRealList;
        case OGC_FIELD_TYPE_STRING_LIST: return CNFieldType::kStringList;
        default: return CNFieldType::kString;
    }
}

ogc_geom_type_e ToCGeomType(GeomType type) {
    switch (type) {
        case GeomType::kPoint: return OGC_GEOM_TYPE_POINT;
        case GeomType::kLineString: return OGC_GEOM_TYPE_LINESTRING;
        case GeomType::kPolygon: return OGC_GEOM_TYPE_POLYGON;
        case GeomType::kMultiPoint: return OGC_GEOM_TYPE_MULTIPOINT;
        case GeomType::kMultiLineString: return OGC_GEOM_TYPE_MULTILINESTRING;
        case GeomType::kMultiPolygon: return OGC_GEOM_TYPE_MULTIPOLYGON;
        case GeomType::kGeometryCollection: return OGC_GEOM_TYPE_GEOMETRYCOLLECTION;
        default: return OGC_GEOM_TYPE_UNKNOWN;
    }
}

static GeomType FromCGeomType(ogc_geom_type_e type) {
    switch (type) {
        case OGC_GEOM_TYPE_POINT: return GeomType::kPoint;
        case OGC_GEOM_TYPE_LINESTRING: return GeomType::kLineString;
        case OGC_GEOM_TYPE_POLYGON: return GeomType::kPolygon;
        case OGC_GEOM_TYPE_MULTIPOINT: return GeomType::kMultiPoint;
        case OGC_GEOM_TYPE_MULTILINESTRING: return GeomType::kMultiLineString;
        case OGC_GEOM_TYPE_MULTIPOLYGON: return GeomType::kMultiPolygon;
        case OGC_GEOM_TYPE_GEOMETRYCOLLECTION: return GeomType::kGeometryCollection;
        default: return GeomType::kUnknown;
    }
}

}

ogc_field_defn_t* ogc_field_defn_create(const char* name, ogc_field_type_e type) {
    CNFieldDefn* defn = CreateCNFieldDefn(name);
    if (defn) {
        defn->SetType(FromCFieldType(type));
    }
    return reinterpret_cast<ogc_field_defn_t*>(defn);
}

void ogc_field_defn_destroy(ogc_field_defn_t* defn) {
    delete reinterpret_cast<CNFieldDefn*>(defn);
}

const char* ogc_field_defn_get_name(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFieldDefn*>(defn)->GetName();
    }
    return "";
}

ogc_field_type_e ogc_field_defn_get_type(const ogc_field_defn_t* defn) {
    if (defn) {
        return ToCFieldType(reinterpret_cast<const CNFieldDefn*>(defn)->GetType());
    }
    return OGC_FIELD_TYPE_STRING;
}

int ogc_field_defn_get_width(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFieldDefn*>(defn)->GetWidth();
    }
    return 0;
}

int ogc_field_defn_get_precision(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFieldDefn*>(defn)->GetPrecision();
    }
    return 0;
}

void ogc_field_defn_set_width(ogc_field_defn_t* defn, int width) {
    if (defn) {
        reinterpret_cast<CNFieldDefn*>(defn)->SetWidth(width);
    }
}

void ogc_field_defn_set_precision(ogc_field_defn_t* defn, int precision) {
    if (defn) {
        reinterpret_cast<CNFieldDefn*>(defn)->SetPrecision(precision);
    }
}

int ogc_field_defn_is_nullable(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFieldDefn*>(defn)->IsNullable() ? 1 : 0;
    }
    return 0;
}
void ogc_field_defn_set_nullable(ogc_field_defn_t* defn, int nullable) {
    if (defn) {
        reinterpret_cast<CNFieldDefn*>(defn)->SetNullable(nullable != 0);
    }
}
const char* ogc_field_defn_get_default_value(const ogc_field_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFieldDefn*>(defn)->GetDefaultValue();
    }
    return "";
}
void ogc_field_defn_set_default_value(ogc_field_defn_t* defn, const char* value) {
    if (defn && value) {
        reinterpret_cast<CNFieldDefn*>(defn)->SetDefaultValue(value);
    }
}

ogc_field_value_t* ogc_field_value_create(void) {
    return reinterpret_cast<ogc_field_value_t*>(new CNFieldValue());
}

ogc_field_value_t* ogc_field_value_create_int(int value) {
    return reinterpret_cast<ogc_field_value_t*>(new CNFieldValue(value));
}

ogc_field_value_t* ogc_field_value_create_int64(long long value) {
    return reinterpret_cast<ogc_field_value_t*>(new CNFieldValue(static_cast<int64_t>(value)));
}

ogc_field_value_t* ogc_field_value_create_double(double value) {
    return reinterpret_cast<ogc_field_value_t*>(new CNFieldValue(value));
}

ogc_field_value_t* ogc_field_value_create_string(const char* value) {
    return reinterpret_cast<ogc_field_value_t*>(new CNFieldValue(SafeString(value)));
}

void ogc_field_value_destroy(ogc_field_value_t* value) {
    delete reinterpret_cast<CNFieldValue*>(value);
}

ogc_field_type_e ogc_field_value_get_type(const ogc_field_value_t* value) {
    if (value) {
        return ToCFieldType(reinterpret_cast<const CNFieldValue*>(value)->GetType());
    }
    return OGC_FIELD_TYPE_STRING;
}

int ogc_field_value_is_null(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const CNFieldValue*>(value)->IsNull() ? 1 : 0;
    }
    return 1;
}

int ogc_field_value_is_set(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const CNFieldValue*>(value)->IsSet() ? 1 : 0;
    }
    return 0;
}

int ogc_field_value_get_as_integer(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const CNFieldValue*>(value)->GetInteger();
    }
    return 0;
}

long long ogc_field_value_get_as_integer64(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const CNFieldValue*>(value)->GetInteger64();
    }
    return 0;
}

double ogc_field_value_get_as_real(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const CNFieldValue*>(value)->GetReal();
    }
    return 0.0;
}

const char* ogc_field_value_get_as_string(const ogc_field_value_t* value) {
    if (value) {
        return reinterpret_cast<const CNFieldValue*>(value)->GetString().c_str();
    }
    return "";
}

void ogc_field_value_set_integer(ogc_field_value_t* value, int val) {
    if (value) {
        reinterpret_cast<CNFieldValue*>(value)->SetInteger(val);
    }
}

void ogc_field_value_set_integer64(ogc_field_value_t* value, long long val) {
    if (value) {
        reinterpret_cast<CNFieldValue*>(value)->SetInteger64(static_cast<int64_t>(val));
    }
}

void ogc_field_value_set_real(ogc_field_value_t* value, double val) {
    if (value) {
        reinterpret_cast<CNFieldValue*>(value)->SetReal(val);
    }
}

void ogc_field_value_set_string(ogc_field_value_t* value, const char* val) {
    if (value) {
        reinterpret_cast<CNFieldValue*>(value)->SetString(SafeString(val));
    }
}

void ogc_field_value_set_null(ogc_field_value_t* value) {
    if (value) {
        reinterpret_cast<CNFieldValue*>(value)->SetNull();
    }
}

ogc_feature_defn_t* ogc_feature_defn_create(const char* name) {
    if (name) {
        return reinterpret_cast<ogc_feature_defn_t*>(CNFeatureDefn::Create(name));
    }
    return reinterpret_cast<ogc_feature_defn_t*>(CNFeatureDefn::Create(nullptr));
}

void ogc_feature_defn_destroy(ogc_feature_defn_t* defn) {
    if (defn) {
        reinterpret_cast<CNFeatureDefn*>(defn)->ReleaseReference();
    }
}

const char* ogc_feature_defn_get_name(const ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFeatureDefn*>(defn)->GetName();
    }
    return "";
}

size_t ogc_feature_defn_get_field_count(const ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFeatureDefn*>(defn)->GetFieldCount();
    }
    return 0;
}

ogc_field_defn_t* ogc_feature_defn_get_field_defn(const ogc_feature_defn_t* defn, size_t index) {
    if (defn) {
        return reinterpret_cast<ogc_field_defn_t*>(
            reinterpret_cast<CNFeatureDefn*>(const_cast<ogc_feature_defn_t*>(defn))->GetFieldDefn(index));
    }
    return nullptr;
}

int ogc_feature_defn_get_field_index(const ogc_feature_defn_t* defn, const char* name) {
    if (defn && name) {
        return reinterpret_cast<const CNFeatureDefn*>(defn)->GetFieldIndex(name);
    }
    return -1;
}

void ogc_feature_defn_add_field_defn(ogc_feature_defn_t* defn, ogc_field_defn_t* field_defn) {
    if (defn && field_defn) {
        reinterpret_cast<CNFeatureDefn*>(defn)->AddFieldDefn(reinterpret_cast<CNFieldDefn*>(field_defn));
    }
}

size_t ogc_feature_defn_get_geom_field_count(const ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<const CNFeatureDefn*>(defn)->GetGeomFieldCount();
    }
    return 0;
}

ogc_geom_type_e ogc_feature_defn_get_geometry_type(const ogc_feature_defn_t* defn) {
    if (defn) {
        auto* featureDefn = reinterpret_cast<const CNFeatureDefn*>(defn);
        if (featureDefn->GetGeomFieldCount() > 0) {
            auto* geomFieldDefn = featureDefn->GetGeomFieldDefn(0);
            if (geomFieldDefn) {
                return ToCGeomType(geomFieldDefn->GetGeomType());
            }
        }
    }
    return OGC_GEOM_TYPE_UNKNOWN;
}

void ogc_feature_defn_set_geometry_type(ogc_feature_defn_t* defn, ogc_geom_type_e type) {
    if (defn) {
        auto* featureDefn = reinterpret_cast<CNFeatureDefn*>(defn);
        GeomType geomType = FromCGeomType(type);
        if (featureDefn->GetGeomFieldCount() > 0) {
            auto* geomFieldDefn = featureDefn->GetGeomFieldDefn(0);
            if (geomFieldDefn) {
                geomFieldDefn->SetGeomType(geomType);
            }
        }
    }
}

ogc_feature_t* ogc_feature_create(ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<ogc_feature_t*>(
            new CNFeature(reinterpret_cast<CNFeatureDefn*>(defn)));
    }
    return nullptr;
}

void ogc_feature_destroy(ogc_feature_t* feature) {
    delete reinterpret_cast<CNFeature*>(feature);
}

long long ogc_feature_get_fid(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->GetFID();
    }
    return 0;
}

void ogc_feature_set_fid(ogc_feature_t* feature, long long fid) {
    if (feature) {
        reinterpret_cast<CNFeature*>(feature)->SetFID(fid);
    }
}

ogc_feature_defn_t* ogc_feature_get_defn(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<ogc_feature_defn_t*>(
            reinterpret_cast<const CNFeature*>(feature)->GetFeatureDefn());
    }
    return nullptr;
}

size_t ogc_feature_get_field_count(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->GetFieldCount();
    }
    return 0;
}

int ogc_feature_is_field_null(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->IsFieldNull(index) ? 1 : 0;
    }
    return 1;
}
int ogc_feature_is_field_set(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->IsFieldSet(index) ? 1 : 0;
    }
    return 0;
}

int ogc_feature_get_field_as_integer(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->GetFieldAsInteger(index);
    }
    return 0;
}

long long ogc_feature_get_field_as_integer64(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->GetFieldAsInteger64(index);
    }
    return 0;
}

double ogc_feature_get_field_as_real(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        return reinterpret_cast<const CNFeature*>(feature)->GetFieldAsReal(index);
    }
    return 0.0;
}

const char* ogc_feature_get_field_as_string(const ogc_feature_t* feature, size_t index) {
    if (feature) {
        static thread_local std::string result;
        result = reinterpret_cast<const CNFeature*>(feature)->GetFieldAsString(index);
        return result.c_str();
    }
    return "";
}

void ogc_feature_set_field_integer(ogc_feature_t* feature, size_t index, int value) {
    if (feature) {
        reinterpret_cast<CNFeature*>(feature)->SetFieldInteger(index, value);
    }
}

void ogc_feature_set_field_integer64(ogc_feature_t* feature, size_t index, long long value) {
    if (feature) {
        reinterpret_cast<CNFeature*>(feature)->SetFieldInteger64(index, static_cast<int64_t>(value));
    }
}

void ogc_feature_set_field_real(ogc_feature_t* feature, size_t index, double value) {
    if (feature) {
        reinterpret_cast<CNFeature*>(feature)->SetFieldReal(index, value);
    }
}

void ogc_feature_set_field_string(ogc_feature_t* feature, size_t index, const char* value) {
    if (feature && value) {
        reinterpret_cast<CNFeature*>(feature)->SetFieldString(index, value);
    }
}

void ogc_feature_set_field_null(ogc_feature_t* feature, size_t index) {
    if (feature) {
        reinterpret_cast<CNFeature*>(feature)->SetFieldNull(index);
    }
}

ogc_geometry_t* ogc_feature_get_geometry(const ogc_feature_t* feature) {
    if (feature) {
        auto* f = reinterpret_cast<const CNFeature*>(feature);
        if (f->GetGeomFieldCount() > 0) {
            GeometryPtr geom = f->GetGeometry();
            if (geom) {
                GeometryPtr cloned = geom->Clone();
                return reinterpret_cast<ogc_geometry_t*>(cloned.release());
            }
        }
    }
    return nullptr;
}

const ogc_geometry_t* ogc_feature_get_geometry_const(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<const ogc_geometry_t*>(
            reinterpret_cast<const CNFeature*>(feature)->GetGeometry().get());
    }
    return nullptr;
}

void ogc_feature_set_geometry(ogc_feature_t* feature, ogc_geometry_t* geom) {
    if (feature && geom) {
        GeometryPtr g(reinterpret_cast<Geometry*>(geom));
        reinterpret_cast<CNFeature*>(feature)->SetGeometry(std::move(g));
    }
}


void ogc_feature_set_geometry_directly(ogc_feature_t* feature, ogc_geometry_t* geom) {
    if (feature && geom) {
        ogc::GeometryPtr g(reinterpret_cast<ogc::Geometry*>(geom));
        reinterpret_cast<CNFeature*>(feature)->SetGeometry(std::move(g));
    }
}

ogc_feature_t* ogc_feature_clone(const ogc_feature_t* feature) {
    if (feature) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<const CNFeature*>(feature)->Clone());
    }
    return nullptr;
}

int ogc_feature_equal(const ogc_feature_t* a, const ogc_feature_t* b) {
    if (a && b) {
        return reinterpret_cast<const CNFeature*>(a)->Equal(
            *reinterpret_cast<const CNFeature*>(b)) ? 1 : 0;
    }
    return 0;
}
#ifdef __cplusplus
}
#endif
