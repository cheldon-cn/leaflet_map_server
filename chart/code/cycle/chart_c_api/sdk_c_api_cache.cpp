/**
 * @file sdk_c_api_cache.cpp
 * @brief OGC Chart SDK C API - Cache Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#define _USE_MATH_DEFINES
#include "sdk_c_api.h"

#include <ogc/cache/tile/tile_cache.h>
#include <ogc/cache/tile/tile_key.h>
#include <ogc/cache/offline/offline_storage_manager.h>
#include <ogc/geom/envelope.h>

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

using namespace ogc;
using namespace ogc::cache;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

TileKey ToTileKey(const ogc_tile_key_t* key) {
    return TileKey(key->x, key->y, key->z);
}

}  

ogc_tile_key_t ogc_tile_key_create(int x, int y, int z) {
    ogc_tile_key_t key;
    key.x = x;
    key.y = y;
    key.z = z;
    return key;
}

int ogc_tile_key_equals(const ogc_tile_key_t* a, const ogc_tile_key_t* b) {
    if (a && b) {
        return (a->x == b->x && a->y == b->y && a->z == b->z) ? 1 : 0;
    }
    return 0;
}

char* ogc_tile_key_to_string(const ogc_tile_key_t* key) {
    if (key) {
        TileKey tk = ToTileKey(key);
        std::string str = tk.ToString();
        char* result = static_cast<char*>(malloc(str.size() + 1));
        if (result) {
            strcpy(result, str.c_str());
        }
        return result;
    }
    return nullptr;
}

ogc_tile_key_t ogc_tile_key_from_string(const char* str) {
    ogc_tile_key_t result = {0, 0, 0};
    if (str) {
        TileKey tk = TileKey::FromString(std::string(str));
        result.x = tk.x;
        result.y = tk.y;
        result.z = tk.z;
    }
    return result;
}

uint64_t ogc_tile_key_to_index(const ogc_tile_key_t* key) {
    if (key) {
        TileKey tk = ToTileKey(key);
        return tk.ToIndex();
    }
    return 0;
}

ogc_tile_key_t ogc_tile_key_get_parent(const ogc_tile_key_t* key) {
    ogc_tile_key_t result = {0, 0, 0};
    if (key) {
        TileKey tk = ToTileKey(key);
        TileKey parent = tk.GetParent();
        result.x = parent.x;
        result.y = parent.y;
        result.z = parent.z;
    }
    return result;
}

void ogc_tile_key_get_children(const ogc_tile_key_t* key, ogc_tile_key_t children[4]) {
    if (key && children) {
        TileKey tk = ToTileKey(key);
        TileKey childKeys[4];
        tk.GetChildren(childKeys);
        for (int i = 0; i < 4; ++i) {
            children[i].x = childKeys[i].x;
            children[i].y = childKeys[i].y;
            children[i].z = childKeys[i].z;
        }
    }
}

ogc_envelope_t* ogc_tile_key_to_envelope(const ogc_tile_key_t* key) {
    if (key) {
        TileKey tk = ToTileKey(key);
        double minLon = tk.x / static_cast<double>(1 << tk.z) * 360.0 - 180.0;
        double maxLon = (tk.x + 1) / static_cast<double>(1 << tk.z) * 360.0 - 180.0;
        double latMin = atan(sinh(M_PI * (1 - 2.0 * tk.y / (1 << tk.z)))) * 180.0 / M_PI;
        double latMax = atan(sinh(M_PI * (1 - 2.0 * (tk.y + 1) / (1 << tk.z)))) * 180.0 / M_PI;
        Envelope* env = new Envelope(minLon, latMin, maxLon, latMax);
        return reinterpret_cast<ogc_envelope_t*>(env);
    }
    return nullptr;
}

ogc_tile_cache_t* ogc_tile_cache_create(void) {
    return ogc_memory_tile_cache_create(100 * 1024 * 1024);
}

void ogc_tile_cache_destroy(ogc_tile_cache_t* cache) {
    if (cache) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(cache);
        delete ptr;
    }
}

int ogc_tile_cache_has_tile(const ogc_tile_cache_t* cache, const ogc_tile_key_t* key) {
    if (cache && key) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(const_cast<ogc_tile_cache_t*>(cache));
        TileKey tk = ToTileKey(key);
        return (*ptr)->HasTile(tk) ? 1 : 0;
    }
    return 0;
}

ogc_tile_data_t* ogc_tile_cache_get_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key) {
    if (cache && key) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(cache);
        TileKey tk = ToTileKey(key);
        TileData data = (*ptr)->GetTile(tk);
        if (data.IsValid()) {
            TileData* result = new TileData(data);
            return reinterpret_cast<ogc_tile_data_t*>(result);
        }
    }
    return nullptr;
}

void ogc_tile_cache_put_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key, const void* data, size_t size) {
    if (cache && key && data && size > 0) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(cache);
        TileKey tk = ToTileKey(key);
        std::vector<uint8_t> vecData(static_cast<const uint8_t*>(data), 
                                      static_cast<const uint8_t*>(data) + size);
        (*ptr)->PutTile(tk, vecData);
    }
}

void ogc_tile_cache_remove_tile(ogc_tile_cache_t* cache, const ogc_tile_key_t* key) {
    if (cache && key) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(cache);
        TileKey tk = ToTileKey(key);
        (*ptr)->RemoveTile(tk);
    }
}

void ogc_tile_cache_clear(ogc_tile_cache_t* cache) {
    if (cache) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(cache);
        (*ptr)->Clear();
    }
}

size_t ogc_tile_cache_get_size(const ogc_tile_cache_t* cache) {
    if (cache) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(const_cast<ogc_tile_cache_t*>(cache));
        return (*ptr)->GetSize();
    }
    return 0;
}

size_t ogc_tile_cache_get_max_size(const ogc_tile_cache_t* cache) {
    if (cache) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(const_cast<ogc_tile_cache_t*>(cache));
        return (*ptr)->GetMaxSize();
    }
    return 0;
}

void ogc_tile_cache_set_max_size(ogc_tile_cache_t* cache, size_t max_size) {
    if (cache) {
        TileCachePtr* ptr = reinterpret_cast<TileCachePtr*>(cache);
        (*ptr)->SetMaxSize(max_size);
    }
}

ogc_tile_cache_t* ogc_memory_tile_cache_create(size_t max_size_bytes) {
    TileCachePtr cache = TileCache::CreateMemoryCache(max_size_bytes);
    if (cache) {
        TileCachePtr* ptr = new TileCachePtr(std::move(cache));
        return reinterpret_cast<ogc_tile_cache_t*>(ptr);
    }
    return nullptr;
}

ogc_tile_cache_t* ogc_disk_tile_cache_create(const char* cache_dir, size_t max_size_bytes) {
    if (cache_dir) {
        TileCachePtr cache = TileCache::CreateDiskCache(std::string(cache_dir), max_size_bytes);
        if (cache) {
            TileCachePtr* ptr = new TileCachePtr(std::move(cache));
            return reinterpret_cast<ogc_tile_cache_t*>(ptr);
        }
    }
    return nullptr;
}

ogc_tile_cache_t* ogc_multi_level_tile_cache_create(ogc_tile_cache_t* memory_cache, ogc_tile_cache_t* disk_cache) {
    return nullptr;
}

ogc_offline_storage_t* ogc_offline_storage_create(const char* storage_path) {
    if (storage_path) {
        auto manager = OfflineStorageManager::Create(std::string(storage_path));
        if (manager) {
            return reinterpret_cast<ogc_offline_storage_t*>(manager.release());
        }
    }
    return nullptr;
}

void ogc_offline_storage_destroy(ogc_offline_storage_t* storage) {
    if (storage) {
        OfflineStorageManager* manager = reinterpret_cast<OfflineStorageManager*>(storage);
        delete manager;
    }
}

ogc_offline_region_t* ogc_offline_storage_create_region(ogc_offline_storage_t* storage, const ogc_envelope_t* bounds, int min_zoom, int max_zoom) {
    if (storage && bounds) {
        OfflineStorageManager* manager = reinterpret_cast<OfflineStorageManager*>(storage);
        Envelope* env = reinterpret_cast<Envelope*>(const_cast<ogc_envelope_t*>(bounds));
        std::string regionId = manager->CreateRegion("", env->GetMinX(), env->GetMaxX(), 
                                                      env->GetMinY(), env->GetMaxY(), 
                                                      min_zoom, max_zoom);
        if (!regionId.empty()) {
            OfflineRegion* region = new OfflineRegion();
            region->id = regionId;
            return reinterpret_cast<ogc_offline_region_t*>(region);
        }
    }
    return nullptr;
}

size_t ogc_offline_storage_get_region_count(const ogc_offline_storage_t* storage) {
    if (storage) {
        OfflineStorageManager* manager = reinterpret_cast<OfflineStorageManager*>(const_cast<ogc_offline_storage_t*>(storage));
        return manager->GetRegionCount();
    }
    return 0;
}

ogc_offline_region_t* ogc_offline_storage_get_region(const ogc_offline_storage_t* storage, size_t index) {
    if (storage) {
        OfflineStorageManager* manager = reinterpret_cast<OfflineStorageManager*>(const_cast<ogc_offline_storage_t*>(storage));
        std::vector<OfflineRegion> regions = manager->GetAllRegions();
        if (index < regions.size()) {
            OfflineRegion* result = new OfflineRegion(regions[index]);
            return reinterpret_cast<ogc_offline_region_t*>(result);
        }
    }
    return nullptr;
}

void ogc_offline_storage_remove_region(ogc_offline_storage_t* storage, size_t index) {
    if (storage) {
        OfflineStorageManager* manager = reinterpret_cast<OfflineStorageManager*>(storage);
        std::vector<OfflineRegion> regions = manager->GetAllRegions();
        if (index < regions.size()) {
            manager->DeleteRegion(regions[index].id);
        }
    }
}

#ifdef __cplusplus
}
#endif
