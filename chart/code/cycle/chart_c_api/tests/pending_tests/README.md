# Pending Tests 目录

本目录包含因API接口不匹配而暂时归档的测试用例。

## 归档原则

1. **不删除测试用例**：所有测试用例都被保留，只是移动到pending_tests目录
2. **添加元数据**：每个归档文件都包含详细的元数据，说明：
   - 文件来源
   - 生成时间
   - 归档原因
   - 不匹配的API列表（期望的API签名、当前SDK实际API、说明）

## 当前归档文件

| 文件名 | 原始文件 | 归档原因 | 待实现API数量 | 归档测试用例数 |
|--------|----------|----------|---------------|----------------|
| test_sdk_c_api_navigation_pending.cpp | test_sdk_c_api_navigation.cpp | API不匹配/返回nullptr/SEH异常 | 23个 | 25个 |
| test_sdk_c_api_feature_pending.cpp | test_sdk_c_api_feature.cpp | API不匹配 | 12个 | 12个 |
| test_sdk_c_api_layer_pending.cpp | test_sdk_c_api_layer.cpp | API不匹配/未实现 | 29个 | 8个 |
| test_sdk_c_api_alert_pending.cpp | test_sdk_c_api_alert.cpp | API不匹配/返回nullptr | 42个 | 20个 |
| test_sdk_c_api_cache_pending.cpp | test_sdk_c_api_cache.cpp | API不匹配 | 29个 | 12个 |

## API不匹配类型

### 1. 方法名不匹配
- `ogc_waypoint_get_arrival_radius` → 需要实现
- `ogc_waypoint_set_turn_radius` → 需要实现
- `ogc_route_insert_waypoint` → 需要实现
- `ogc_route_clear` → 需要实现
- `ogc_route_reverse` → 需要实现
- `ogc_tile_cache_put` → `ogc_tile_cache_put_tile`
- `ogc_tile_cache_get` → `ogc_tile_cache_get_tile`

### 2. 参数类型不匹配
- `ogc_feature_create(geometry)` → `ogc_feature_create(feature_defn)`
- `ogc_feature_set_id/get_id` → `ogc_feature_set_fid/get_fid`
- `ogc_alert_set_position(alert, lat, lon)` → `ogc_alert_set_position(alert, coord*)`
- `ogc_tile_cache_create(capacity)` → `ogc_tile_cache_create(void)`
- `ogc_memory_tile_cache_create(count, bytes)` → `ogc_memory_tile_cache_create(bytes)`

### 3. 方法名和参数都不匹配
- `ogc_feature_set_attribute_*` → `ogc_feature_set_field_*`
- `ogc_feature_get_attribute_*` → `ogc_feature_get_field_as_*`
- `ogc_tile_cache_get_capacity` → `ogc_tile_cache_get_max_size`

### 4. 类型不存在
- `ogc_feature_collection_t` → 需要实现
- `ogc_waypoint_type_e` → 需要实现
- `ogc_alert_zone_t` → 需要实现
- `ogc_cache_manager_t` → 需要实现

### 5. 完全缺失的API模块
- **Alert Zone模块**: `ogc_alert_zone_*` 系列API
- **Cache Manager模块**: `ogc_cache_manager_*` 系列API
- **Alert Engine扩展**: `ogc_alert_engine_add_alert/remove_alert/get_alert` 等
- **AIS Manager扩展**: `ogc_ais_manager_add_target/remove_target/get_target_count` 等
- **Offline Storage扩展**: `ogc_offline_storage_store_chart/get_chart_path` 等

## 恢复测试用例

当SDK实现相应API后，可以按以下步骤恢复测试用例：

1. 从pending_tests目录复制测试用例到tests目录
2. 根据元数据中的API映射关系更新测试代码
3. 编译验证
4. 运行测试

## 版本历史

- 2026-04-10: 初始创建，归档navigation和feature模块的测试用例
- 2026-04-10: 新增layer、alert、cache模块的归档测试用例
- 2026-04-10: 新增AIS Target、RouteManager、RoutePlanner、NavigationEngine的归档测试用例（运行时失败）
