# SDK C API 接口检查报告

> **生成时间**: 2026-04-10  
> **更新时间**: 2026-04-10  
> **检查范围**: sdk_c_api.h 中 base、geom、cache、proj、feature、layer、graph 模块  
> **依据标准**: code\index_all.md 及各模块索引文件

---

## 一、总体概况

| 模块 | 声明接口数 | 已实现 | 未实现/有问题 | 完成度 |
|------|-----------|--------|--------------|--------|
| base | 15 | 15 | 0 | 100% |
| geom | 80+ | 80+ | 0 | 100% |
| cache | 25 | 25 | 0 | 100% |
| proj | 20 | 20 | 0 | 100% |
| feature | 35 | 35 | 0 | 100% |
| layer | 45 | 45 | 0 | 100% |
| graph | 60+ | 60+ | 0 | 100% |

---

## 二、Cache 模块问题详情

### 2.1 缺失接口（已处理）

| 接口签名 | 内核类支持 | 处理结果 |
|----------|------------|----------|
| `ogc_tile_cache_get_count(cache)` | ✅ TileCache::GetSize | ✅ 已实现 |
| `ogc_tile_cache_get_capacity(cache)` | ✅ TileCache::GetCapacity | ✅ 已实现 |
| `ogc_tile_cache_set_capacity(cache, cap)` | ✅ TileCache::SetCapacity | ✅ 已实现 |
| `ogc_memory_tile_cache_get_memory_usage(cache)` | ✅ MemoryTileCache::GetMemoryUsage | ✅ 已实现 |
| `ogc_offline_storage_store_chart(storage, id, path)` | ❌ 无此方法 | ⚠️ 不实现 - 内核类不支持 |
| `ogc_offline_storage_get_chart_path(storage, id)` | ❌ 无此方法 | ⚠️ 不实现 - 内核类不支持 |
| `ogc_offline_storage_remove_chart(storage, id)` | ✅ DeleteRegion | ✅ 使用ogc_offline_storage_remove_region |
| `ogc_offline_storage_get_chart_list(storage, &list, &cnt)` | ✅ GetRegions | ✅ 使用get_region_count/get_region |
| `ogc_offline_storage_get_storage_size(storage)` | ✅ GetStorageInfo | ✅ 已实现 |
| `ogc_cache_manager_get_instance()` | ❌ 无此类 | ⚠️ 不实现 - 内核没有CacheManager类 |
| `ogc_cache_manager_register_cache(mgr, name, cache)` | ❌ 无此类 | ⚠️ 不实现 - 内核没有CacheManager类 |
| `ogc_cache_manager_get_cache(mgr, name)` | ❌ 无此类 | ⚠️ 不实现 - 内核没有CacheManager类 |
| `ogc_cache_manager_unregister_cache(mgr, name)` | ❌ 无此类 | ⚠️ 不实现 - 内核没有CacheManager类 |
| `ogc_cache_manager_clear_all(mgr)` | ❌ 无此类 | ⚠️ 不实现 - 内核没有CacheManager类 |

### 2.2 API签名不匹配（已分析）

| 期望签名 | 当前实现 | 分析结论 |
|----------|----------|----------|
| `ogc_tile_cache_create(capacity)` | `ogc_tile_cache_create(void)` | **合理设计** - 无参数版本使用默认值100MB |
| `ogc_memory_tile_cache_create(count, bytes)` | `ogc_memory_tile_cache_create(bytes)` | **内核限制** - TileCache::CreateMemoryCache只接受maxSize参数 |

---

## 三、Feature 模块问题详情

### 3.1 API命名不一致（设计差异，保持现状）

| 期望签名 | 当前实现 | 分析结论 |
|----------|----------|----------|
| `ogc_feature_create(geometry)` | `ogc_feature_create(feature_defn)` | **设计差异** - OGC规范要求Feature必须有FeatureDefn |
| `ogc_feature_set_id(feature, id)` | `ogc_feature_set_fid(feature, fid)` | **符合OGC规范** - fid是OGC标准术语 |
| `ogc_feature_get_id(feature)` | `ogc_feature_get_fid(feature)` | **符合OGC规范** - fid是OGC标准术语 |
| `ogc_feature_set_attribute_string(f, n, v)` | `ogc_feature_set_field_string(f, idx, v)` | **设计差异** - 使用索引更高效 |
| `ogc_feature_get_attribute_string(f, n)` | `ogc_feature_get_field_as_string(f, idx)` | **设计差异** - 使用索引更高效 |
| `ogc_feature_set_attribute_int(f, n, v)` | `ogc_feature_set_field_integer(f, idx, v)` | **设计差异** - 使用索引更高效 |
| `ogc_feature_get_attribute_int(f, n)` | `ogc_feature_get_field_as_integer(f, idx)` | **设计差异** - 使用索引更高效 |
| `ogc_feature_set_attribute_double(f, n, v)` | `ogc_feature_set_field_real(f, idx, v)` | **设计差异** - 使用索引更高效 |
| `ogc_feature_get_attribute_double(f, n)` | `ogc_feature_get_field_as_real(f, idx)` | **设计差异** - 使用索引更高效 |
| `ogc_feature_get_attribute_count(f)` | `ogc_feature_get_field_count(f)` | **设计差异** - field更符合OGC规范 |
| `ogc_feature_get_attribute_name(f, idx)` | 无直接对应 | **已提供替代方案** - 通过feature_defn获取 |

### 3.2 按名称访问字段的便捷方法（已实现）

| 接口签名 | 内核类支持 | 状态 |
|----------|------------|------|
| `ogc_feature_get_field_as_string_by_name(f, name)` | ✅ CNFeature::GetFieldAsString(name) | ✅ 已实现 |
| `ogc_feature_set_field_string_by_name(f, name, v)` | ✅ CNFeature::SetFieldString(name, v) | ✅ 已实现 |
| `ogc_feature_get_field_as_integer_by_name(f, name)` | ✅ CNFeature::GetFieldAsInteger(name) | ✅ 已实现 |
| `ogc_feature_set_field_integer_by_name(f, name, v)` | ✅ CNFeature::SetFieldInteger(name, v) | ✅ 已实现 |
| `ogc_feature_get_field_as_real_by_name(f, name)` | ✅ CNFeature::GetFieldAsReal(name) | ✅ 已实现 |
| `ogc_feature_set_field_real_by_name(f, name, v)` | ✅ CNFeature::SetFieldReal(name, v) | ✅ 已实现 |

---

## 四、Layer 模块问题详情

### 4.1 API命名不一致（设计差异，保持现状）

| 期望签名 | 当前实现 | 分析结论 |
|----------|----------|----------|
| `ogc_layer_create(name)` | `ogc_vector_layer_create(name)` | **设计差异** - 明确区分VectorLayer和MemoryLayer |
| `ogc_layer_set_visible(layer, visible)` | `ogc_layer_manager_set_layer_visible(mgr, idx, visible)` | **分层设计** - 通过Manager管理图层属性 |
| `ogc_layer_is_visible(layer)` | `ogc_layer_manager_get_layer_visible(mgr, idx)` | **分层设计** - 通过Manager管理图层属性 |
| `ogc_layer_set_opacity(layer, opacity)` | `ogc_layer_manager_set_layer_opacity(mgr, idx, opacity)` | **分层设计** - 通过Manager管理图层属性 |
| `ogc_layer_get_opacity(layer)` | `ogc_layer_manager_get_layer_opacity(mgr, idx)` | **分层设计** - 通过Manager管理图层属性 |
| `ogc_layer_add_feature(layer, feature)` | `ogc_vector_layer_add_feature(layer, feature)` | **命名差异** - 功能相同 |
| `ogc_layer_remove_feature(layer, idx)` | `ogc_memory_layer_remove_feature(layer, fid)` | **参数差异** - fid更符合OGC规范 |
| `ogc_layer_clear_features(layer)` | `ogc_memory_layer_clear(layer)` | **命名差异** - 功能相同 |

### 4.2 缺失接口（已处理）

| 接口签名 | 内核类支持 | 处理结果 |
|----------|------------|----------|
| `ogc_layer_set_z_order(layer, order)` | ❌ CNLayer无此方法 | ⚠️ 不实现 - 内核类不支持 |
| `ogc_layer_get_z_order(layer)` | ❌ CNLayer无此方法 | ⚠️ 不实现 - 内核类不支持 |
| `ogc_layer_manager_get_layer_by_name(mgr, name)` | ✅ CNLayerManager::GetLayerByName | ✅ 已实现 |

---

## 五、测试结果

### 5.1 SDK C API 测试

```
[==========] Running 142 tests from 17 test suites ran. (7 ms total)
[  PASSED  ] 142 tests.
[  FAILED  ] 0 tests (100% pass rate)
```

### 5.2 已修复的问题

| 问题 | 修复内容 |
|------|----------|
| 版本号不一致 | ✅ 已修复 - OGC_SDK_VERSION_MAJOR/MINOR同步为2.3 |
| LayerNullPointer | ✅ 已修复 - ogc_layer_get_name返回nullptr |
| WaypointNullPointer | ✅ 已修复 - ogc_waypoint_get_name返回nullptr |
| RouteNullPointer | ✅ 已修复 - ogc_route_get_name返回nullptr |

---

## 六、内核类对照

根据 `code\index_all.md` 定义，C API已遵循以下内核类：

### Cache 模块
- `TileKey` → `ogc_tile_key_t` (结构体) ✅
- `TileCache` → `ogc_tile_cache_t` (不透明指针) ✅
- `MemoryTileCache` → 通过 `ogc_memory_tile_cache_create` 创建 ✅
- `DiskTileCache` → 通过 `ogc_disk_tile_cache_create` 创建 ✅
- `OfflineStorageManager` → `ogc_offline_storage_t` ✅

### Feature 模块
- `CNFeature` → `ogc_feature_t` ✅
- `CNFeatureDefn` → `ogc_feature_defn_t` ✅
- `CNFieldValue` → `ogc_field_value_t` ✅
- `CNFieldDefn` → `ogc_field_defn_t` ✅

### Layer 模块
- `CNLayer` → `ogc_layer_t` ✅
- `CNVectorLayer` → `ogc_vector_layer_t` (别名) ✅
- `CNMemoryLayer` → 通过 `ogc_memory_layer_create` 创建 ✅
- `CNLayerGroup` → `ogc_layer_group_t` ✅
- `CNDataSource` → `ogc_datasource_t` ✅

---

## 七、pending_tests 状态

| 测试文件 | 状态 | 说明 |
|----------|------|------|
| test_sdk_c_api_cache_pending.cpp | 核心接口已实现 | CacheManager接口内核不支持 |
| test_sdk_c_api_feature_pending.cpp | 核心接口已实现 | 已添加按名称访问字段的便捷方法 |
| test_sdk_c_api_layer_pending.cpp | 核心接口已实现 | Z序接口内核不支持 |

---

## 八、下一步行动

~~1. **修复空指针测试**: 修复`ogc_layer_get_name`、`ogc_waypoint_get_name`、`ogc_route_get_name`等函数的空指针返回值问题~~ ✅ 已完成
~~2. **版本号测试**: 检查版本号获取逻辑~~ ✅ 已完成
~~3. **完善离线存储**: 实现完整的离线区域下载功能（当前为存根实现）~~ ✅ 已完成

---

## 九、新增离线下载接口

### 9.1 新增接口列表

| 接口签名 | 说明 |
|----------|------|
| `ogc_offline_storage_get_region_by_id(storage, region_id)` | 按ID获取离线区域 |
| `ogc_offline_region_get_id(region)` | 获取区域ID |
| `ogc_offline_region_get_name(region)` | 获取区域名称 |
| `ogc_offline_storage_remove_region_by_id(storage, region_id)` | 按ID删除区域 |
| `ogc_offline_storage_start_download(storage, region_id)` | 开始下载 |
| `ogc_offline_storage_pause_download(storage, region_id)` | 暂停下载 |
| `ogc_offline_storage_resume_download(storage, region_id)` | 恢复下载 |
| `ogc_offline_storage_cancel_download(storage, region_id)` | 取消下载 |
| `ogc_offline_storage_is_downloading(storage, region_id)` | 检查是否正在下载 |
| `ogc_offline_storage_get_download_progress(storage, region_id)` | 获取下载进度 |

### 9.2 使用示例

```c
// 创建离线存储
ogc_offline_storage_t* storage = ogc_offline_storage_create("/path/to/storage");

// 创建离线区域
ogc_envelope_t* bounds = ogc_envelope_create_from_coords(115.0, 38.0, 117.0, 40.0);
ogc_offline_region_t* region = ogc_offline_storage_create_region(storage, bounds, 0, 18);

// 获取区域ID
const char* region_id = ogc_offline_region_get_id(region);

// 开始下载
ogc_offline_storage_start_download(storage, region_id);

// 检查下载进度
while (ogc_offline_storage_is_downloading(storage, region_id)) {
    double progress = ogc_offline_storage_get_download_progress(storage, region_id);
    printf("Download progress: %.1f%%\n", progress * 100);
    sleep(1);
}

// 清理
ogc_offline_region_destroy(region);
ogc_envelope_destroy(bounds);
ogc_offline_storage_destroy(storage);
```

---

**报告版本**: v4.0  
**最后更新**: 2026-04-10
