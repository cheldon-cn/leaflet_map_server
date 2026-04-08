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
#include <ogc/layer/memory_layer.h>
#include <ogc/layer/layer_group.h>
#include <ogc/layer/datasource.h>
#include <ogc/layer/driver_manager.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::layer;
using namespace ogc::feature;
using namespace ogc::geom;

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
    return reinterpret_cast<ogc_layer_t*>(Layer::Create(SafeString(name)).release());
}

void ogc_layer_destroy(ogc_layer_t* layer) {
    delete reinterpret_cast<Layer*>(layer);
}

const char* ogc_layer_get_name(const ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const Layer*>(layer)->GetName().c_str();
    }
    return "";
}

void ogc_layer_set_name(ogc_layer_t* layer, const char* name) {
    if (layer) {
        reinterpret_cast<Layer*>(layer)->SetName(SafeString(name));
    }
}

int ogc_layer_is_visible(const ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const Layer*>(layer)->IsVisible() ? 1 : 0;
    }
    return 0;
}

void ogc_layer_set_visible(ogc_layer_t* layer, int visible) {
    if (layer) {
        reinterpret_cast<Layer*>(layer)->SetVisible(visible != 0);
    }
}

double ogc_layer_get_opacity(const ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const Layer*>(layer)->GetOpacity();
    }
    return 1.0;
}

void ogc_layer_set_opacity(ogc_layer_t* layer, double opacity) {
    if (layer) {
        reinterpret_cast<Layer*>(layer)->SetOpacity(opacity);
    }
}

int ogc_layer_get_z_order(const ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const Layer*>(layer)->GetZOrder();
    }
    return 0;
}

void ogc_layer_set_z_order(ogc_layer_t* layer, int z_order) {
    if (layer) {
        reinterpret_cast<Layer*>(layer)->SetZOrder(z_order);
    }
}

ogc_envelope_t* ogc_layer_get_extent(const ogc_layer_t* layer) {
    if (layer) {
        const Envelope& env = reinterpret_cast<const Layer*>(layer)->GetExtent();
        return reinterpret_cast<ogc_envelope_t*>(new Envelope(env));
    }
    return nullptr;
}

int ogc_layer_refresh(ogc_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<Layer*>(layer)->Refresh() ? 1 : 0;
    }
    return 0;
}

ogc_vector_layer_t* ogc_vector_layer_create(const char* name, ogc_feature_defn_t* defn) {
    if (defn) {
        return reinterpret_cast<ogc_vector_layer_t*>(
            VectorLayer::Create(SafeString(name), reinterpret_cast<FeatureDefn*>(defn)).release());
    }
    return nullptr;
}

void ogc_vector_layer_destroy(ogc_vector_layer_t* layer) {
    delete reinterpret_cast<VectorLayer*>(layer);
}

size_t ogc_vector_layer_get_feature_count(const ogc_vector_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const VectorLayer*>(layer)->GetFeatureCount();
    }
    return 0;
}

ogc_feature_t* ogc_vector_layer_get_feature(ogc_vector_layer_t* layer, long long fid) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<VectorLayer*>(layer)->GetFeature(fid));
    }
    return nullptr;
}

ogc_feature_t* ogc_vector_layer_get_next_feature(ogc_vector_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<VectorLayer*>(layer)->GetNextFeature());
    }
    return nullptr;
}

void ogc_vector_layer_reset_reading(ogc_vector_layer_t* layer) {
    if (layer) {
        reinterpret_cast<VectorLayer*>(layer)->ResetReading();
    }
}

int ogc_vector_layer_add_feature(ogc_vector_layer_t* layer, ogc_feature_t* feature) {
    if (layer && feature) {
        return reinterpret_cast<VectorLayer*>(layer)->AddFeature(
            reinterpret_cast<Feature*>(feature)) ? 1 : 0;
    }
    return 0;
}

int ogc_vector_layer_set_feature(ogc_vector_layer_t* layer, ogc_feature_t* feature) {
    if (layer && feature) {
        return reinterpret_cast<VectorLayer*>(layer)->SetFeature(
            reinterpret_cast<Feature*>(feature)) ? 1 : 0;
    }
    return 0;
}

int ogc_vector_layer_delete_feature(ogc_vector_layer_t* layer, long long fid) {
    if (layer) {
        return reinterpret_cast<VectorLayer*>(layer)->DeleteFeature(fid) ? 1 : 0;
    }
    return 0;
}

ogc_feature_defn_t* ogc_vector_layer_get_layer_defn(ogc_vector_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<ogc_feature_defn_t*>(
            reinterpret_cast<VectorLayer*>(layer)->GetLayerDefn());
    }
    return nullptr;
}

ogc_geometry_t* ogc_vector_layer_get_geometry_by_fid(ogc_vector_layer_t* layer, long long fid) {
    if (layer) {
        Geometry* geom = reinterpret_cast<VectorLayer*>(layer)->GetGeometryByFID(fid);
        if (geom) {
            GeometryPtr cloned = geom->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

ogc_raster_layer_t* ogc_raster_layer_create(const char* name, const char* filepath) {
    return reinterpret_cast<ogc_raster_layer_t*>(
        RasterLayer::Create(SafeString(name), SafeString(filepath)).release());
}

void ogc_raster_layer_destroy(ogc_raster_layer_t* layer) {
    delete reinterpret_cast<RasterLayer*>(layer);
}

int ogc_raster_layer_get_width(const ogc_raster_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const RasterLayer*>(layer)->GetWidth();
    }
    return 0;
}

int ogc_raster_layer_get_height(const ogc_raster_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const RasterLayer*>(layer)->GetHeight();
    }
    return 0;
}

int ogc_raster_layer_get_band_count(const ogc_raster_layer_t* layer) {
    if (layer) {
        return reinterpret_cast<const RasterLayer*>(layer)->GetBandCount();
    }
    return 0;
}

double ogc_raster_layer_get_no_data_value(const ogc_raster_layer_t* layer, int band) {
    if (layer) {
        return reinterpret_cast<const RasterLayer*>(layer)->GetNoDataValue(band);
    }
    return 0.0;
}

int ogc_raster_layer_read_block(ogc_raster_layer_t* layer, int x, int y, int width, int height, 
                                 void* buffer, int band) {
    if (layer && buffer) {
        return reinterpret_cast<RasterLayer*>(layer)->ReadBlock(x, y, width, height, buffer, band) ? 1 : 0;
    }
    return 0;
}

ogc_memory_layer_t* ogc_memory_layer_create(const char* name) {
    return reinterpret_cast<ogc_memory_layer_t*>(
        MemoryLayer::Create(SafeString(name)).release());
}

void ogc_memory_layer_destroy(ogc_memory_layer_t* layer) {
    delete reinterpret_cast<MemoryLayer*>(layer);
}

ogc_layer_group_t* ogc_layer_group_create(const char* name) {
    return reinterpret_cast<ogc_layer_group_t*>(
        LayerGroup::Create(SafeString(name)).release());
}

void ogc_layer_group_destroy(ogc_layer_group_t* group) {
    delete reinterpret_cast<LayerGroup*>(group);
}

size_t ogc_layer_group_get_layer_count(const ogc_layer_group_t* group) {
    if (group) {
        return reinterpret_cast<const LayerGroup*>(group)->GetLayerCount();
    }
    return 0;
}

ogc_layer_t* ogc_layer_group_get_layer(ogc_layer_group_t* group, size_t index) {
    if (group) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<LayerGroup*>(group)->GetLayer(index));
    }
    return nullptr;
}

int ogc_layer_group_add_layer(ogc_layer_group_t* group, ogc_layer_t* layer) {
    if (group && layer) {
        return reinterpret_cast<LayerGroup*>(group)->AddLayer(
            LayerPtr(reinterpret_cast<Layer*>(layer))) ? 1 : 0;
    }
    return 0;
}

int ogc_layer_group_remove_layer(ogc_layer_group_t* group, size_t index) {
    if (group) {
        return reinterpret_cast<LayerGroup*>(group)->RemoveLayer(index) ? 1 : 0;
    }
    return 0;
}

ogc_datasource_t* ogc_datasource_open(const char* path, int update) {
    return reinterpret_cast<ogc_datasource_t*>(
        DataSource::Open(SafeString(path), update != 0).release());
}

void ogc_datasource_destroy(ogc_datasource_t* ds) {
    delete reinterpret_cast<DataSource*>(ds);
}

const char* ogc_datasource_get_name(const ogc_datasource_t* ds) {
    if (ds) {
        return reinterpret_cast<const DataSource*>(ds)->GetName().c_str();
    }
    return "";
}

size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds) {
    if (ds) {
        return reinterpret_cast<const DataSource*>(ds)->GetLayerCount();
    }
    return 0;
}

ogc_layer_t* ogc_datasource_get_layer(ogc_datasource_t* ds, size_t index) {
    if (ds) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<DataSource*>(ds)->GetLayer(index));
    }
    return nullptr;
}

ogc_layer_t* ogc_datasource_get_layer_by_name(ogc_datasource_t* ds, const char* name) {
    if (ds && name) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<DataSource*>(ds)->GetLayerByName(SafeString(name)));
    }
    return nullptr;
}

int ogc_datasource_delete_layer(ogc_datasource_t* ds, size_t index) {
    if (ds) {
        return reinterpret_cast<DataSource*>(ds)->DeleteLayer(index) ? 1 : 0;
    }
    return 0;
}

int ogc_datasource_sync(ogc_datasource_t* ds) {
    if (ds) {
        return reinterpret_cast<DataSource*>(ds)->Sync() ? 1 : 0;
    }
    return 0;
}

int ogc_driver_manager_register_driver(const char* name, const char* path) {
    return DriverManager::RegisterDriver(SafeString(name), SafeString(path)) ? 1 : 0;
}

int ogc_driver_manager_unregister_driver(const char* name) {
    return DriverManager::UnregisterDriver(SafeString(name)) ? 1 : 0;
}

size_t ogc_driver_manager_get_driver_count(void) {
    return DriverManager::GetDriverCount();
}

const char* ogc_driver_manager_get_driver_name(size_t index) {
    std::string name = DriverManager::GetDriverName(index);
    return AllocString(name);
}

ogc_datasource_t* ogc_driver_manager_open(const char* path, int update) {
    return reinterpret_cast<ogc_datasource_t*>(
        DriverManager::Open(SafeString(path), update != 0).release());
}

#ifdef __cplusplus
}
#endif
