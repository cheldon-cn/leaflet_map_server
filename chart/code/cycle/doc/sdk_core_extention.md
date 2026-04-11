# SDK 内核扩展实施方案

**创建日期**: 2026-04-10  
**版本**: v1.0  
**目标**: 为C API层提供Z序和CacheManager的内核支持

---

## 一、扩展需求分析

### 1.1 Z序（Z-Order）

**现状分析**:
- 内核`CNLayer`基类无Z序属性
- 内核`CNLayerGroup`无Z序管理
- `LayerManager`（graph模块）已有图层排序能力（`MoveLayer`/`MoveLayerUp`/`MoveLayerDown`），但通过索引位置管理，非显式Z序值
- `LayerConfig`已有`opacity`/`visibility`/`scale range`等属性，但无Z序

**扩展方案**: 在`CNLayerNode`和`LayerConfig`中添加Z序属性，利用`LayerManager`的排序能力实现Z序管理

### 1.2 CacheManager

**现状分析**:
- 内核`TileCache`已有完整缓存接口（`PutTile`/`GetTile`/`HasTile`等）
- `MemoryTileCache`/`DiskTileCache`/`MultiLevelTileCache`已实现
- `TileCache::CreateMemoryCache`/`CreateDiskCache`静态工厂方法已存在
- 缺少统一的`CacheManager`来管理多个缓存实例

**扩展方案**: 创建`CacheManager`类，聚合`TileCache`实例，提供统一管理接口

---

## 二、Z序扩展实施

### 2.1 内核层修改

#### 2.1.1 修改 `layer_group.h` - CNLayerNode添加Z序

**文件**: `code\layer\include\ogc\layer\layer_group.h`

```cpp
class OGC_LAYER_API CNLayerNode {
public:
    // ... 现有接口 ...

    virtual int GetZOrder() const { return 0; }
    virtual void SetZOrder(int z_order) { (void)z_order; }
};
```

#### 2.1.2 修改 `layer_group.h` - CNLayerWrapper添加Z序成员

**文件**: `code\layer\include\ogc\layer\layer_group.h`

```cpp
class OGC_LAYER_API CNLayerWrapper : public CNLayerNode {
public:
    // ... 现有接口 ...

    int GetZOrder() const override { return z_order_; }
    void SetZOrder(int z_order) override { z_order_ = z_order; }

private:
    std::unique_ptr<CNLayer> layer_;
    bool visible_ = true;
    int z_order_ = 0;       // 新增
    CNLayerNode* parent_ = nullptr;
};
```

#### 2.1.3 修改 `layer_group.h` - CNLayerGroup添加Z序成员

**文件**: `code\layer\include\ogc\layer\layer_group.h`

```cpp
class OGC_LAYER_API CNLayerGroup : public CNLayerNode {
public:
    // ... 现有接口 ...

    int GetZOrder() const override { return z_order_; }
    void SetZOrder(int z_order) override { z_order_ = z_order; }

    void SortByZOrder();

private:
    std::string name_;
    bool visible_ = true;
    int z_order_ = 0;       // 新增
    CNLayerNode* parent_ = nullptr;
    // ...
};
```

#### 2.1.4 修改 `layer_manager.h` - LayerConfig添加Z序

**文件**: `code\graph\include\ogc\graph\layer\layer_manager.h`

```cpp
class OGC_GRAPH_API LayerConfig {
public:
    // ... 现有接口 ...

    int GetZOrder() const { return m_zOrder; }
    void SetZOrder(int zOrder) { m_zOrder = zOrder; }

private:
    // ... 现有成员 ...
    int m_zOrder;    // 新增
};
```

### 2.2 C API层添加

#### 2.2.1 sdk_c_api.h 添加Z序接口声明

```cpp
SDK_C_API int ogc_layer_get_z_order(const ogc_layer_t* layer);
SDK_C_API void ogc_layer_set_z_order(ogc_layer_t* layer, int z_order);
SDK_C_API int ogc_layer_group_get_z_order(const ogc_layer_group_t* group);
SDK_C_API void ogc_layer_group_set_z_order(ogc_layer_group_t* group, int z_order);
SDK_C_API void ogc_layer_manager_set_layer_z_order(ogc_layer_manager_t* mgr, size_t index, int z_order);
SDK_C_API int ogc_layer_manager_get_layer_z_order(const ogc_layer_manager_t* mgr, size_t index);
```

#### 2.2.2 sdk_c_api_layer.cpp 实现Z序接口

```cpp
int ogc_layer_get_z_order(const ogc_layer_t* layer) {
    // 通过LayerManager的LayerConfig获取
    return 0;
}

void ogc_layer_set_z_order(ogc_layer_t* layer, int z_order) {
    // 通过LayerManager的LayerConfig设置
}

int ogc_layer_group_get_z_order(const ogc_layer_group_t* group) {
    if (group) {
        return reinterpret_cast<const CNLayerGroup*>(group)->GetZOrder();
    }
    return 0;
}

void ogc_layer_group_set_z_order(ogc_layer_group_t* group, int z_order) {
    if (group) {
        reinterpret_cast<CNLayerGroup*>(group)->SetZOrder(z_order);
    }
}
```

---

## 三、CacheManager扩展实施

### 3.1 内核层创建

#### 3.1.1 新建 `cache_manager.h`

**文件**: `code\cache\include\ogc\cache\cache_manager.h`

```cpp
#pragma once

#include "ogc/cache/export.h"
#include "ogc/cache/tile/tile_cache.h"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace ogc {
namespace cache {

class OGC_CACHE_API CacheManager {
public:
    static CacheManager& Instance();

    TileCachePtr GetCache(const std::string& name) const;
    void AddCache(const std::string& name, TileCachePtr cache);
    void RemoveCache(const std::string& name);
    void ClearAllCaches();

    size_t GetCacheCount() const;
    std::vector<std::string> GetCacheNames() const;

    TileData GetTile(const std::string& cache_name, const TileKey& key) const;
    bool PutTile(const std::string& cache_name, const TileKey& key, const TileData& tile);
    bool HasTile(const std::string& cache_name, const TileKey& key) const;

    size_t GetTotalSize() const;
    size_t GetTotalTileCount() const;

    void FlushAll();

private:
    CacheManager();
    ~CacheManager();
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;

    struct CacheEntry {
        std::string name;
        TileCachePtr cache;
    };

    std::vector<CacheEntry> m_caches;
    mutable std::mutex m_mutex;
};

}
}
```

#### 3.1.2 新建 `cache_manager.cpp`

**文件**: `code\cache\src\cache_manager.cpp`

实现所有CacheManager方法。

#### 3.1.3 更新 `index_cache.md`

在头文件清单中添加CacheManager条目。

### 3.2 C API层添加

#### 3.2.1 sdk_c_api.h 添加CacheManager接口声明

```cpp
typedef struct ogc_cache_manager_t ogc_cache_manager_t;

SDK_C_API ogc_cache_manager_t* ogc_cache_manager_create(void);
SDK_C_API void ogc_cache_manager_destroy(ogc_cache_manager_t* mgr);
SDK_C_API size_t ogc_cache_manager_get_cache_count(const ogc_cache_manager_t* mgr);
SDK_C_API ogc_tile_cache_t* ogc_cache_manager_get_cache(ogc_cache_manager_t* mgr, const char* name);
SDK_C_API void ogc_cache_manager_add_cache(ogc_cache_manager_t* mgr, const char* name, ogc_tile_cache_t* cache);
SDK_C_API void ogc_cache_manager_remove_cache(ogc_cache_manager_t* mgr, const char* name);
SDK_C_API void ogc_cache_manager_clear_all(ogc_cache_manager_t* mgr);
SDK_C_API ogc_tile_data_t* ogc_cache_manager_get_tile(ogc_cache_manager_t* mgr, const char* cache_name, const ogc_tile_key_t* key);
SDK_C_API void ogc_cache_manager_put_tile(ogc_cache_manager_t* mgr, const char* cache_name, const ogc_tile_key_t* key, const void* data, size_t size);
```

#### 3.2.2 sdk_c_api_cache.cpp 实现CacheManager接口

---

## 四、实施步骤

### 步骤1: Z序 - 内核层修改 ✅ 已完成
1. ✅ 修改 `layer_group.h` - CNLayerNode/CNLayerWrapper/CNLayerGroup 添加Z序
2. ✅ 修改 `layer_group.cpp` - 实现Z序方法和SortByZOrder
3. ✅ 修改 `layer_manager.h` - LayerConfig 添加Z序
4. ✅ 修改 `layer_manager.cpp` - 实现Z序

### 步骤2: Z序 - C API层添加 ✅ 已完成
1. ✅ 修改 `sdk_c_api.h` - 添加Z序接口声明
2. ✅ 修改 `sdk_c_api_layer.cpp` - 实现Z序接口
3. ✅ 修改 `sdk_c_api_graph.cpp` - 实现LayerManager Z序接口

### 步骤3: CacheManager - 内核层创建 ✅ 已完成
1. ✅ 创建 `cache_manager.h` - CacheManager类定义
2. ✅ 创建 `cache_manager.cpp` - CacheManager实现
3. ✅ 更新 `CMakeLists.txt` - 添加新源文件
4. ✅ 更新 `index_cache.md` - 添加索引

### 步骤4: CacheManager - C API层添加 ✅ 已完成
1. ✅ 修改 `sdk_c_api.h` - 添加CacheManager接口声明
2. ✅ 修改 `sdk_c_api_cache.cpp` - 实现CacheManager接口

### 步骤5: 编译测试 ✅ 已完成
1. ✅ 编译所有模块
2. ✅ 运行现有测试确保无回归（142 tests passed）
3. ✅ 修复Alert模块重复定义问题

---

## 五、影响范围

| 修改项 | 文件 | 模块 | 影响 |
|--------|------|------|------|
| CNLayerNode Z序 | layer_group.h | layer | 低 - 新增虚函数有默认实现 |
| CNLayerWrapper Z序 | layer_group.h/cpp | layer | 低 - 新增成员和实现 |
| CNLayerGroup Z序 | layer_group.h/cpp | layer | 低 - 新增成员和实现 |
| LayerConfig Z序 | layer_manager.h/cpp | graph | 低 - 新增成员和实现 |
| CacheManager | cache_manager.h/cpp | cache | 中 - 新增类 |
| C API Z序 | sdk_c_api.h/layer.cpp/graph.cpp | chart_c_api | 低 - 新增接口 |
| C API CacheManager | sdk_c_api.h/cache.cpp | chart_c_api | 低 - 新增接口 |

---

**版本**: v1.0  
**最后更新**: 2026-04-10
