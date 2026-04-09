/**
 * @file sdk_c_api_layer.cpp
 * @brief OGC Chart SDK C API - Layer Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/layer/layer.h>
#include <ogc/layer/vector_layer.h>
#include <ogc/layer/raster_layer.h>
#include <ogc/layer/raster_dataset.h>
#include <ogc/layer/memory_layer.h>
#include <ogc/layer/layer_group.h>
#include <ogc/layer/datasource.h>
#include <ogc/layer/driver_manager.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;

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

ogc_layer_t* ogc_layer_create(const char* name) {
    return reinterpret_cast<ogc_layer_t*>(new CNMemoryLayer(SafeString(name)));
}

void ogc_layer_destroy(ogc_layer_t* layer) {
    delete reinterpret_cast<CNLayer*>(layer);
}

const char* ogc_layer_get_name(const ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const CNLayer*>(layer)->GetName().c_str();
    }
    return "";
}

void ogc_layer_set_name(ogc_layer_t* layer, const char* name) {
}

int ogc_layer_is_visible(const ogc_layer_t* layer) {
    return 1;
}

void ogc_layer_set_visible(ogc_layer_t* layer, int visible) {
}

double ogc_layer_get_opacity(const ogc_layer_t* layer) {
    return 1.0;
}

void ogc_layer_set_opacity(ogc_layer_t* layer, double opacity) {
}

int ogc_layer_get_z_order(const ogc_layer_t* layer) {
    return 0;
}

void ogc_layer_set_z_order(ogc_layer_t* layer, int z_order) {
}

ogc_envelope_t* ogc_layer_get_extent(const ogc_layer_t* layer) {
    if (layer) {
        Envelope env;
        reinterpret_cast<const CNLayer*>(layer)->GetExtent(env);
        return reinterpret_cast<ogc_envelope_t*>(new Envelope(env));
    }
    return nullptr;
}

int ogc_layer_refresh(ogc_layer_t* layer) {
    return 1;
}

ogc_vector_layer_t* ogc_vector_layer_create(const char* name, ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<ogc_vector_layer_t*>(
            new CNMemoryLayer(SafeString(name), std::shared_ptr<CNFeatureDefn>(
                reinterpret_cast<CNFeatureDefn*>(defn), [](CNFeatureDefn*){})));
    }
    return nullptr;
}

void ogc_vector_layer_destroy(ogc_vector_layer_t* layer) {
    delete reinterpret_cast<CNLayer*>(layer);
}

size_t ogc_vector_layer_get_feature_count(const ogc_vector_layer_t* layer) {
    if (layer) {
        return static_cast<size_t>(reinterpret_cast<const CNLayer*>(layer)->GetFeatureCount());
    }
    return 0;
}

ogc_feature_t* ogc_vector_layer_get_feature(ogc_vector_layer_t* layer, long long fid) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetFeature(fid).release());
    }
    return nullptr;
}

ogc_feature_t* ogc_vector_layer_get_next_feature(ogc_vector_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetNextFeature().release());
    }
    return nullptr;
}

void ogc_vector_layer_reset_reading(ogc_vector_layer_t* layer) {
    if (layer) {
        reinterpret_cast<CNLayer*>(layer)->ResetReading();
    }
}

int ogc_vector_layer_add_feature(ogc_vector_layer_t* layer, ogc_feature_t* feature) {
    if (layer && feature) {
        return reinterpret_cast<CNLayer*>(layer)->CreateFeature(
            reinterpret_cast<CNFeature*>(feature)) == CNStatus::kSuccess ? 1 : 0;
    }
    return 0;
}

int ogc_vector_layer_set_feature(ogc_vector_layer_t* layer, ogc_feature_t* feature) {
    if (layer && feature) {
        return reinterpret_cast<CNLayer*>(layer)->SetFeature(
            reinterpret_cast<CNFeature*>(feature)) == CNStatus::kSuccess ? 1 : 0;
    }
    return 0;
}

int ogc_vector_layer_delete_feature(ogc_vector_layer_t* layer, long long fid) {
    if (layer) {
        return reinterpret_cast<CNLayer*>(layer)->DeleteFeature(fid) == CNStatus::kSuccess ? 1 : 0;
    }
    return 0;
}

ogc_feature_defn_t* ogc_vector_layer_get_layer_defn(ogc_vector_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_defn_t*>(
            reinterpret_cast<CNLayer*>(layer)->GetFeatureDefn());
    }
    return nullptr;
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

ogc_raster_layer_t* ogc_raster_layer_create(const char* name, const char* filepath) {
    if (filepath) {
        auto dataset = CNRasterDataset::Open(SafeString(filepath));
        if (dataset) {
            return reinterpret_cast<ogc_raster_layer_t*>(dataset.release());
        }
    }
    return nullptr;
}

void ogc_raster_layer_destroy(ogc_raster_layer_t* layer) {
    delete reinterpret_cast<CNRasterDataset*>(layer);
}

int ogc_raster_layer_get_width(const ogc_raster_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const CNRasterDataset*>(layer)->GetWidth();
    }
    return 0;
}

int ogc_raster_layer_get_height(const ogc_raster_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const CNRasterDataset*>(layer)->GetHeight();
    }
    return 0;
}

int ogc_raster_layer_get_band_count(const ogc_raster_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const CNRasterDataset*>(layer)->GetBandCount();
    }
    return 0;
}

double ogc_raster_layer_get_no_data_value(const ogc_raster_layer_t* layer, int band) {
    if (layer) {
        auto* band_ptr = reinterpret_cast<const CNRasterDataset*>(layer)->GetBand(band);
        if (band_ptr) {
            return band_ptr->GetNoDataValue();
        }
    }
    return 0.0;
}

int ogc_raster_layer_read_block(ogc_raster_layer_t* layer, int x, int y, int width, int height, 
                                 void* buffer, int band) {
    if (layer && buffer) {
        auto* dataset = reinterpret_cast<CNRasterDataset*>(layer);
        auto* band_ptr = dataset->GetBand(band);
        if (band_ptr) {
            CNDataType data_type = band_ptr->GetDataType();
            return band_ptr->ReadRaster(x, y, width, height, buffer, data_type) == CNStatus::kSuccess ? 1 : 0;
        }
    }
    return 0;
}

ogc_memory_layer_t* ogc_memory_layer_create(const char* name) {
    return reinterpret_cast<ogc_memory_layer_t*>(new CNMemoryLayer(SafeString(name)));
}

void ogc_memory_layer_destroy(ogc_memory_layer_t* layer) {
    delete reinterpret_cast<CNMemoryLayer*>(layer);
}

ogc_layer_group_t* ogc_layer_group_create(const char* name) {
    return reinterpret_cast<ogc_layer_group_t*>(new CNLayerGroup(SafeString(name)));
}

void ogc_layer_group_destroy(ogc_layer_group_t* group) {
    delete reinterpret_cast<CNLayerGroup*>(group);
}

size_t ogc_layer_group_get_layer_count(const ogc_layer_group_t* group) {
    if (group) {
        return static_cast<size_t>(reinterpret_cast<const CNLayerGroup*>(group)->GetLayerCount());
    }
    return 0;
}

ogc_layer_t* ogc_layer_group_get_layer(ogc_layer_group_t* group, size_t index) {
    if (group) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<CNLayerGroup*>(group)->GetLayer(index));
    }
    return nullptr;
}

int ogc_layer_group_add_layer(ogc_layer_group_t* group, ogc_layer_t* layer) {
    if (group && layer) {
        reinterpret_cast<CNLayerGroup*>(group)->AddLayer(
            std::unique_ptr<CNLayer>(reinterpret_cast<CNLayer*>(layer)));
        return 1;
    }
    return 0;
}

int ogc_layer_group_remove_layer(ogc_layer_group_t* group, size_t index) {
    return 0;
}

ogc_datasource_t* ogc_datasource_open(const char* path, int update) {
    return reinterpret_cast<ogc_datasource_t*>(
        CNDataSource::Open(SafeString(path), update != 0).release());
}

void ogc_datasource_destroy(ogc_datasource_t* ds) {
    delete reinterpret_cast<CNDataSource*>(ds);
}

const char* ogc_datasource_get_name(const ogc_datasource_t* ds) {
    if (ds) {
        return reinterpret_cast<const CNDataSource*>(ds)->GetName().c_str();
    }
    return "";
}

size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds) {
    if (ds) {
        return static_cast<size_t>(reinterpret_cast<const CNDataSource*>(ds)->GetLayerCount());
    }
    return 0;
}

ogc_layer_t* ogc_datasource_get_layer(ogc_datasource_t* ds, size_t index) {
    if (ds) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<CNDataSource*>(ds)->GetLayer(static_cast<int>(index)));
    }
    return nullptr;
}

ogc_layer_t* ogc_datasource_get_layer_by_name(ogc_datasource_t* ds, const char* name) {
    if (ds && name) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<CNDataSource*>(ds)->GetLayerByName(SafeString(name)));
    }
    return nullptr;
}

int ogc_datasource_delete_layer(ogc_datasource_t* ds, size_t index) {
    return 0;
}

int ogc_datasource_sync(ogc_datasource_t* ds) {
    if (ds) {
        auto* datasource = reinterpret_cast<CNDataSource*>(ds);
        int count = datasource->GetLayerCount();
        for (int i = 0; i < count; ++i) {
            auto* layer = datasource->GetLayer(i);
            if (layer) {
                layer->SyncToDisk();
            }
        }
        return 1;
    }
    return 0;
}

int ogc_driver_manager_register_driver(const char* name, const char* path) {
    return 0;
}

int ogc_driver_manager_unregister_driver(const char* name) {
    return 0;
}

size_t ogc_driver_manager_get_driver_count(void) {
    return 0;
}

const char* ogc_driver_manager_get_driver_name(size_t index) {
    return "";
}

ogc_datasource_t* ogc_driver_manager_open(const char* path, int update) {
    return reinterpret_cast<ogc_datasource_t*>(
        CNDataSource::Open(SafeString(path), update != 0).release());
}

#ifdef __cplusplus
}
#endif
