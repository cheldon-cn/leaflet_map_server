/**
 * @file sdk_c_api_cache.cpp
 * @brief OGC Chart SDK C API - Cache Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/cache/tile_cache.h>
#include <ogc/cache/tile_key.h>
#include <ogc/cache/offline_storage.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::cache;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_tile_cache_t* ogc_tile_cache_create(size_t max_size) {
    return reinterpret_cast<ogc_tile_cache_t*>(TileCache::Create(max_size).release());
}

void ogc_tile_cache_destroy(ogc_tile_cache_t* cache) {
    delete reinterpret_cast<TileCache*>(cache);
}

int ogc_tile_cache_get(ogc_tile_cache_t* cache, const ogc_tile_key_t* key, void** data, size_t* size) {
    if (cache && key && data && size) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<TileCache*>(cache)->Get(tk, data, size) ? 1 : 0;
    }
    return 0;
}

int ogc_tile_cache_put(ogc_tile_cache_t* cache, const ogc_tile_key_t* key, const void* data, size_t size) {
    if (cache && key && data && size > 0) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<TileCache*>(cache)->Put(tk, data, size) ? 1 : 0;
    }
    return 0;
}

int ogc_tile_cache_remove(ogc_tile_cache_t* cache, const ogc_tile_key_t* key) {
    if (cache && key) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<TileCache*>(cache)->Remove(tk) ? 1 : 0;
    }
    return 0;
}

int ogc_tile_cache_contains(ogc_tile_cache_t* cache, const ogc_tile_key_t* key) {
    if (cache && key) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<TileCache*>(cache)->Contains(tk) ? 1 : 0;
    }
    return 0;
}

void ogc_tile_cache_clear(ogc_tile_cache_t* cache) {
    if (cache) {
        reinterpret_cast<TileCache*>(cache)->Clear();
    }
}

size_t ogc_tile_cache_get_size(const ogc_tile_cache_t* cache) {
    if (cache) {
        return reinterpret_cast<const TileCache*>(cache)->GetSize();
    }
    return 0;
}

size_t ogc_tile_cache_get_max_size(const ogc_tile_cache_t* cache) {
    if (cache) {
        return reinterpret_cast<const TileCache*>(cache)->GetMaxSize();
    }
    return 0;
}

void ogc_tile_cache_set_max_size(ogc_tile_cache_t* cache, size_t max_size) {
    if (cache) {
        reinterpret_cast<TileCache*>(cache)->SetMaxSize(max_size);
    }
}

size_t ogc_tile_cache_get_hit_count(const ogc_tile_cache_t* cache) {
    if (cache) {
        return reinterpret_cast<const TileCache*>(cache)->GetHitCount();
    }
    return 0;
}

size_t ogc_tile_cache_get_miss_count(const ogc_tile_cache_t* cache) {
    if (cache) {
        return reinterpret_cast<const TileCache*>(cache)->GetMissCount();
    }
    return 0;
}

void ogc_tile_cache_reset_stats(ogc_tile_cache_t* cache) {
    if (cache) {
        reinterpret_cast<TileCache*>(cache)->ResetStats();
    }
}

ogc_tile_key_t ogc_tile_key_create(int level, int x, int y) {
    ogc_tile_key_t key;
    key.level = level;
    key.x = x;
    key.y = y;
    return key;
}

int ogc_tile_key_equals(const ogc_tile_key_t* a, const ogc_tile_key_t* b) {
    if (a && b) {
        return (a->level == b->level && a->x == b->x && a->y == b->y) ? 1 : 0;
    }
    return 0;
}

ogc_tile_key_t ogc_tile_key_get_parent(const ogc_tile_key_t* key) {
    ogc_tile_key_t result = {0, 0, 0};
    if (key && key->level > 0) {
        result.level = key->level - 1;
        result.x = key->x / 2;
        result.y = key->y / 2;
    }
    return result;
}

void ogc_tile_key_get_children(const ogc_tile_key_t* key, ogc_tile_key_t* children) {
    if (key && children) {
        int child_level = key->level + 1;
        int child_x = key->x * 2;
        int child_y = key->y * 2;
        
        children[0].level = child_level;
        children[0].x = child_x;
        children[0].y = child_y;
        
        children[1].level = child_level;
        children[1].x = child_x + 1;
        children[1].y = child_y;
        
        children[2].level = child_level;
        children[2].x = child_x;
        children[2].y = child_y + 1;
        
        children[3].level = child_level;
        children[3].x = child_x + 1;
        children[3].y = child_y + 1;
    }
}

ogc_envelope_t* ogc_tile_key_get_extent(const ogc_tile_key_t* key) {
    if (key) {
        TileKey tk(key->level, key->x, key->y);
        ogc::geom::Envelope env = tk.GetExtent();
        return reinterpret_cast<ogc_envelope_t*>(new ogc::geom::Envelope(env));
    }
    return nullptr;
}

ogc_offline_storage_t* ogc_offline_storage_create(const char* path) {
    return reinterpret_cast<ogc_offline_storage_t*>(
        OfflineStorage::Create(SafeString(path)).release());
}

void ogc_offline_storage_destroy(ogc_offline_storage_t* storage) {
    delete reinterpret_cast<OfflineStorage*>(storage);
}

int ogc_offline_storage_open(ogc_offline_storage_t* storage) {
    if (storage) {
        return reinterpret_cast<OfflineStorage*>(storage)->Open() ? 1 : 0;
    }
    return 0;
}

void ogc_offline_storage_close(ogc_offline_storage_t* storage) {
    if (storage) {
        reinterpret_cast<OfflineStorage*>(storage)->Close();
    }
}

int ogc_offline_storage_is_open(const ogc_offline_storage_t* storage) {
    if (storage) {
        return reinterpret_cast<const OfflineStorage*>(storage)->IsOpen() ? 1 : 0;
    }
    return 0;
}

int ogc_offline_storage_store_tile(ogc_offline_storage_t* storage, const ogc_tile_key_t* key, const void* data, size_t size) {
    if (storage && key && data && size > 0) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<OfflineStorage*>(storage)->StoreTile(tk, data, size) ? 1 : 0;
    }
    return 0;
}

int ogc_offline_storage_load_tile(ogc_offline_storage_t* storage, const ogc_tile_key_t* key, void** data, size_t* size) {
    if (storage && key && data && size) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<OfflineStorage*>(storage)->LoadTile(tk, data, size) ? 1 : 0;
    }
    return 0;
}

int ogc_offline_storage_delete_tile(ogc_offline_storage_t* storage, const ogc_tile_key_t* key) {
    if (storage && key) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<OfflineStorage*>(storage)->DeleteTile(tk) ? 1 : 0;
    }
    return 0;
}

int ogc_offline_storage_has_tile(const ogc_offline_storage_t* storage, const ogc_tile_key_t* key) {
    if (storage && key) {
        TileKey tk(key->level, key->x, key->y);
        return reinterpret_cast<const OfflineStorage*>(storage)->HasTile(tk) ? 1 : 0;
    }
    return 0;
}

size_t ogc_offline_storage_get_tile_count(const ogc_offline_storage_t* storage) {
    if (storage) {
        return reinterpret_cast<const OfflineStorage*>(storage)->GetTileCount();
    }
    return 0;
}

long long ogc_offline_storage_get_total_size(const ogc_offline_storage_t* storage) {
    if (storage) {
        return reinterpret_cast<const OfflineStorage*>(storage)->GetTotalSize();
    }
    return 0;
}

int ogc_offline_storage_clear(ogc_offline_storage_t* storage) {
    if (storage) {
        return reinterpret_cast<OfflineStorage*>(storage)->Clear() ? 1 : 0;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
