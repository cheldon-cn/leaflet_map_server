/**
 * @file sdk_c_api_layer.cpp
 * @brief OGC Chart SDK C API - Layer Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/layer/layer.h>
#include <ogc/layer/memory_layer.h>
#include <ogc/layer/layer_type.h>
#include <ogc/layer/layer_group.h>
#include <ogc/layer/datasource.h>
#include <ogc/feature/feature.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
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

static ogc_geom_type_e ToCGeomType(GeomType type) {
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

}

ogc_layer_t* ogc_layer_create(const char* name, ogc_geom_type_e geom_type) {
    return reinterpret_cast<ogc_layer_t*>(
        new CNMemoryLayer(SafeString(name), FromCGeomType(geom_type)));
}

void ogc_layer_destroy(ogc_layer_t* layer) {
    delete reinterpret_cast<CNLayer*>(layer);
}

const char* ogc_layer_get_name(const ogc_layer_t* layer) {
    if (layer) {
        static thread_local std::string result;
        result = reinterpret_cast<const CNLayer*>(layer)->GetName();
        return result.c_str();
    }
    return "";
}

ogc_geom_type_e ogc_layer_get_geom_type(const ogc_layer_t* layer) {
    if (layer) {
        return ToCGeomType(reinterpret_cast<const CNLayer*>(layer)->GetGeomType());
    }
    return OGC_GEOM_TYPE_UNKNOWN;
}

int ogc_layer_get_extent(ogc_layer_t* layer, ogc_envelope_t* extent) {
    if (layer && extent) {
        Envelope env;
        CNStatus status = reinterpret_cast<CNLayer*>(layer)->GetExtent(env);
        if (status == CNStatus::kSuccess) {
            *reinterpret_cast<Envelope*>(extent) = env;
            return 0;
        }
    }
    return -1;
}

long long ogc_layer_get_feature_count(ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<CNLayer*>(layer)->GetFeatureCount();
    }
    return 0;
}

void ogc_layer_reset_reading(ogc_layer_t* layer) {
    if (layer) {
        reinterpret_cast<CNLayer*>(layer)->ResetReading();
    }
}

ogc_feature_t* ogc_layer_get_next_feature(ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetNextFeature().release());
    }
    return nullptr;
}

ogc_feature_t* ogc_layer_get_feature(ogc_layer_t* layer, long long fid) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetFeature(fid).release());
    }
    return nullptr;
}

void ogc_layer_set_spatial_filter_rect(ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y) {
    if (layer) {
        reinterpret_cast<CNLayer*>(layer)->SetSpatialFilterRect(min_x, min_y, max_x, max_y);
    }
}

void ogc_layer_set_attribute_filter(ogc_layer_t* layer, const char* filter) {
    if (layer && filter) {
        reinterpret_cast<CNLayer*>(layer)->SetAttributeFilter(SafeString(filter));
    }
}

ogc_feature_defn_t* ogc_layer_get_feature_defn(ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_defn_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetFeatureDefn());
    }
    return nullptr;
}

ogc_layer_t* ogc_vector_layer_create(const char* name) {
    return reinterpret_cast<ogc_layer_t*>(
        new CNMemoryLayer(SafeString(name), GeomType::kUnknown));
}

ogc_layer_t* ogc_vector_layer_create_from_datasource(const char* datasource_path, const char* layer_name) {
    return nullptr;
}

size_t ogc_vector_layer_get_feature_count(const ogc_layer_t* layer) {
    if (layer) {
        return static_cast<size_t>(reinterpret_cast<const CNLayer*>(layer)->GetFeatureCount());
    }
    return 0;
}

ogc_feature_t* ogc_vector_layer_get_next_feature(ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetNextFeature().release());
    }
    return nullptr;
}

ogc_feature_t* ogc_vector_layer_get_feature_by_id(ogc_layer_t* layer, int64_t fid) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetFeature(fid).release());
    }
    return nullptr;
}

void ogc_vector_layer_reset_reading(ogc_layer_t* layer) {
    if (layer) {
        reinterpret_cast<CNLayer*>(layer)->ResetReading();
    }
}

void ogc_vector_layer_set_spatial_filter(ogc_layer_t* layer, double min_x, double min_y, double max_x, double max_y) {
    if (layer) {
        reinterpret_cast<CNLayer*>(layer)->SetSpatialFilterRect(min_x, min_y, max_x, max_y);
    }
}

ogc_envelope_t* ogc_vector_layer_get_extent(ogc_layer_t* layer) {
    if (layer) {
        Envelope env;
        CNStatus status = reinterpret_cast<CNLayer*>(layer)->GetExtent(env);
        if (status == CNStatus::kSuccess) {
            return reinterpret_cast<ogc_envelope_t*>(new Envelope(env));
        }
    }
    return nullptr;
}

int ogc_vector_layer_add_feature(ogc_layer_t* layer, ogc_feature_t* feature) {
    if (layer && feature) {
        CNStatus status = reinterpret_cast<CNLayer*>(layer)->CreateFeature(
            reinterpret_cast<CNFeature*>(feature));
        return status == CNStatus::kSuccess ? 0 : -1;
    }
    return -1;
}

int ogc_vector_layer_update_feature(ogc_layer_t* layer, const ogc_feature_t* feature) {
    if (layer && feature) {
        CNStatus status = reinterpret_cast<CNLayer*>(layer)->SetFeature(
            reinterpret_cast<const CNFeature*>(feature));
        return status == CNStatus::kSuccess ? 0 : -1;
    }
    return -1;
}

int ogc_vector_layer_delete_feature(ogc_layer_t* layer, int64_t fid) {
    if (layer) {
        CNStatus status = reinterpret_cast<CNLayer*>(layer)->DeleteFeature(fid);
        return status == CNStatus::kSuccess ? 0 : -1;
    }
    return -1;
}

ogc_layer_t* ogc_memory_layer_create(const char* name) {
    return reinterpret_cast<ogc_layer_t*>(
        new CNMemoryLayer(SafeString(name), GeomType::kUnknown));
}

ogc_layer_t* ogc_memory_layer_create_from_features(const char* name, ogc_feature_t** features, size_t count) {
    if (!name || !features || count == 0) {
        return nullptr;
    }
    CNMemoryLayer* layer = new CNMemoryLayer(SafeString(name), GeomType::kUnknown);
    for (size_t i = 0; i < count; ++i) {
        if (features[i]) {
            layer->CreateFeature(reinterpret_cast<CNFeature*>(features[i]));
        }
    }
    return reinterpret_cast<ogc_layer_t*>(layer);
}

int ogc_memory_layer_add_feature(ogc_layer_t* layer, ogc_feature_t* feature) {
    if (layer && feature) {
        CNStatus status = reinterpret_cast<CNLayer*>(layer)->CreateFeature(
            reinterpret_cast<CNFeature*>(feature));
        return status == CNStatus::kSuccess ? 0 : -1;
    }
    return -1;
}


ogc_geometry_t* ogc_vector_layer_get_geometry_by_fid(ogc_vector_layer_t* layer, long long fid) {
    if (layer) {
        auto feature = reinterpret_cast<CNLayer*>(layer)->GetFeature(fid);
        if (feature) {
            auto geom = feature->GetGeometry();
            if (geom) {
                return reinterpret_cast<ogc_geometry_t*>(geom->Clone().release());
            }
        }
    }
    return nullptr;
}

ogc_layer_t* ogc_raster_layer_create(const char* name, const char* filepath) {
    (void)name;
    (void)filepath;
    return nullptr;
}

int ogc_raster_layer_get_width(const ogc_layer_t* layer) {
    (void)layer;
    return 0;
}

int ogc_raster_layer_get_height(const ogc_layer_t* layer) {
    (void)layer;
    return 0;
}

int ogc_raster_layer_get_band_count(const ogc_layer_t* layer) {
    (void)layer;
    return 0;
}

double ogc_raster_layer_get_no_data_value(const ogc_layer_t* layer, int band) {
    (void)layer;
    (void)band;
    return 0.0;
}

ogc_layer_group_t* ogc_layer_group_create(const char* name) {
    return reinterpret_cast<ogc_layer_group_t*>(new CNLayerGroup(SafeString(name)));
}

void ogc_layer_group_destroy(ogc_layer_group_t* group) {
    delete reinterpret_cast<CNLayerGroup*>(group);
}

const char* ogc_layer_group_get_name(const ogc_layer_group_t* group) {
    if (group) {
        return reinterpret_cast<const CNLayerGroup*>(group)->GetName().c_str();
    }
    return "";
}

size_t ogc_layer_group_get_layer_count(const ogc_layer_group_t* group) {
    if (group) {
        return static_cast<size_t>(reinterpret_cast<const CNLayerGroup*>(group)->GetLayerCount());
    }
    return 0;
}

ogc_layer_t* ogc_layer_group_get_layer(const ogc_layer_group_t* group, size_t index) {
    if (group) {
        return reinterpret_cast<ogc_layer_t*>(
            const_cast<CNLayerGroup*>(reinterpret_cast<const CNLayerGroup*>(group))->GetLayer(index));
    }
    return nullptr;
}

void ogc_layer_group_add_layer(ogc_layer_group_t* group, ogc_layer_t* layer) {
    if (group && layer) {
        reinterpret_cast<CNLayerGroup*>(group)->AddLayer(
            std::unique_ptr<CNLayer>(reinterpret_cast<CNLayer*>(layer)));
    }
}

void ogc_layer_group_remove_layer(ogc_layer_group_t* group, size_t index) {
    if (group) {
        auto child = reinterpret_cast<CNLayerGroup*>(group)->RemoveChild(index);
        (void)child;
    }
}

ogc_datasource_t* ogc_datasource_open(const char* path) {
    return reinterpret_cast<ogc_datasource_t*>(
        CNDataSource::Open(SafeString(path), false).release());
}

void ogc_datasource_close(ogc_datasource_t* ds) {
    delete reinterpret_cast<CNDataSource*>(ds);
}

int ogc_datasource_is_open(const ogc_datasource_t* ds) {
    return ds ? 1 : 0;
}

const char* ogc_datasource_get_path(const ogc_datasource_t* ds) {
    if (ds) {
        return reinterpret_cast<const CNDataSource*>(ds)->GetPath().c_str();
    }
    return "";
}

size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds) {
    if (ds) {
        return static_cast<size_t>(reinterpret_cast<const CNDataSource*>(ds)->GetLayerCount());
    }
    return 0;
}

ogc_layer_t* ogc_datasource_get_layer(const ogc_datasource_t* ds, size_t index) {
    if (ds) {
        return reinterpret_cast<ogc_layer_t*>(
            const_cast<CNDataSource*>(reinterpret_cast<const CNDataSource*>(ds))->GetLayer(static_cast<int>(index)));
    }
    return nullptr;
}

ogc_layer_t* ogc_datasource_get_layer_by_name(const ogc_datasource_t* ds, const char* name) {
    if (ds && name) {
        return reinterpret_cast<ogc_layer_t*>(
            const_cast<CNDataSource*>(reinterpret_cast<const CNDataSource*>(ds))->GetLayerByName(SafeString(name)));
    }
    return nullptr;
}

ogc_layer_t* ogc_datasource_create_layer(ogc_datasource_t* ds, const char* name, int geom_type) {
    return nullptr;
}

int ogc_datasource_delete_layer(ogc_datasource_t* ds, const char* name) {
    return 0;
}
#ifdef __cplusplus
}
#endif
