/**
 * @file sdk_c_api_common.cpp
 * @brief OGC Chart SDK C API - Common utilities and helper functions
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/geom/coordinate.h>
#include <ogc/geom/geometry.h>
#include <ogc/feature/field_type.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc;
using namespace ogc::geom;
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

}  

ogc_geom_type_e ToCType(GeomType type) {
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

GeomType FromCType(ogc_geom_type_e type) {
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

const char* ogc_error_get_message(ogc_error_code_e code) {
    switch (code) {
        case OGC_SUCCESS: return "Success";
        case OGC_ERROR_INVALID_PARAM: return "Invalid parameter";
        case OGC_ERROR_NULL_POINTER: return "Null pointer";
        case OGC_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case OGC_ERROR_NOT_FOUND: return "Not found";
        case OGC_ERROR_OPERATION_FAILED: return "Operation failed";
        case OGC_ERROR_INVALID_STATE: return "Invalid state";
        case OGC_ERROR_IO_ERROR: return "I/O error";
        case OGC_ERROR_PARSE_ERROR: return "Parse error";
        case OGC_ERROR_UNSUPPORTED: return "Unsupported operation";
        case OGC_ERROR_TIMEOUT: return "Timeout";
        case OGC_ERROR_PERMISSION_DENIED: return "Permission denied";
        default: return "Unknown error";
    }
}

void ogc_geometry_array_destroy(ogc_geometry_t** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            ogc_geometry_destroy(array[i]);
            array[i] = nullptr;
        }
    }
}

void ogc_feature_array_destroy(ogc_feature_t** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            ogc_feature_destroy(array[i]);
            array[i] = nullptr;
        }
    }
}

void ogc_string_array_destroy(char** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            std::free(array[i]);
            array[i] = nullptr;
        }
    }
}

void ogc_layer_array_destroy(ogc_layer_t** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            ogc_layer_destroy(array[i]);
            array[i] = nullptr;
        }
    }
}

void ogc_alert_array_destroy(ogc_alert_t** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            ogc_alert_destroy(array[i]);
            array[i] = nullptr;
        }
    }
}

void ogc_waypoint_array_destroy(ogc_waypoint_t** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            ogc_waypoint_destroy(array[i]);
            array[i] = nullptr;
        }
    }
}

void ogc_ais_target_array_destroy(ogc_ais_target_t** array, size_t count) {
    if (array == nullptr) return;
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != nullptr) {
            ogc_ais_target_destroy(array[i]);
            array[i] = nullptr;
        }
    }
}

#ifdef __cplusplus
}
#endif
