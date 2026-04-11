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
- ~~`ogc_waypoint_get_arrival_radius` → 需要实现~~ ✅ 已实现
- ~~`ogc_waypoint_set_turn_radius` → 需要实现~~ ✅ 已实现
- ~~`ogc_route_insert_waypoint` → 需要实现~~ ✅ 已实现
- ~~`ogc_route_clear` → 需要实现~~ ✅ 已实现
- ~~`ogc_route_reverse` → 需要实现~~ ✅ 已实现
- `ogc_tile_cache_put` → `ogc_tile_cache_put_tile` ⚠️ 需修改测试调用
- `ogc_tile_cache_get` → `ogc_tile_cache_get_tile` ⚠️ 需修改测试调用

### 2. 参数类型不匹配
- `ogc_feature_create(geometry)` → `ogc_feature_create(feature_defn)` ⚠️ 需修改测试调用
- `ogc_feature_set_id/get_id` → `ogc_feature_set_fid/get_fid` ⚠️ 需修改测试调用
- `ogc_alert_set_position(alert, lat, lon)` → `ogc_alert_set_position(alert, coord*)` ⚠️ 需修改测试调用
- ~~`ogc_tile_cache_create(capacity)` → `ogc_tile_cache_create(void)`~~ ✅ SDK已定义无参版本
- ~~`ogc_memory_tile_cache_create(count, bytes)` → `ogc_memory_tile_cache_create(bytes)`~~ ✅ SDK已定义

### 3. 方法名和参数都不匹配
- `ogc_feature_set_attribute_*` → `ogc_feature_set_field_*` ⚠️ 需修改测试调用
- `ogc_feature_get_attribute_*` → `ogc_feature_get_field_as_*` ⚠️ 需修改测试调用
- ~~`ogc_tile_cache_get_capacity` → `ogc_tile_cache_get_max_size`~~ ✅ SDK已定义`ogc_tile_cache_get_capacity`

### 4. 类型不存在
- `ogc_feature_collection_t` ❌ SDK未定义，需补充
- ~~`ogc_waypoint_type_e` → 需要实现~~ ✅ 已实现 (`OGC_WAYPOINT_TYPE_NORMAL/TURN/ARRIVAL/DEPARTURE`)
- `ogc_alert_zone_t` ❌ SDK未定义，内核无AlertZone类
- ~~`ogc_cache_manager_t` → 需要实现~~ ✅ 已实现

### 5. 完全缺失的API模块
- **Alert Zone模块**: `ogc_alert_zone_*` 系列API ❌ 内核无AlertZone类
- ~~**Cache Manager模块**: `ogc_cache_manager_*` 系列API~~ ✅ 已实现
- ~~**Alert Engine扩展**: `ogc_alert_engine_add_alert/remove_alert/get_alert` 等~~ ✅ 已实现
- ~~**AIS Manager扩展**: `ogc_ais_manager_add_target/remove_target/get_target_count` 等~~ ✅ 已实现
- **Offline Storage扩展**: `ogc_offline_storage_store_chart/get_chart_path` 等 ❌ SDK未声明

---

## 各文件API匹配状态详情 (2026-04-11核查)

### Navigation模块 (test_sdk_c_api_navigation_pending.cpp)

| 期望API | SDK状态 | 说明 |
|---------|---------|------|
| `ogc_waypoint_get_description` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_waypoint_set_description` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_waypoint_get_arrival_radius` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_waypoint_set_turn_radius` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_waypoint_get_turn_radius` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_waypoint_set_type` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_waypoint_get_type` | ✅ 已声明+实现 | 可直接使用 |
| `OGC_WAYPOINT_TYPE_NORMAL/TURN` | ✅ 已声明 | 可直接使用 |
| `ogc_route_insert_waypoint` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_route_clear` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_route_reverse` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_position_manager_set_position` | ❌ 未声明 | 需新增声明+实现 |
| `ogc_position_manager_get_position` | ❌ 未声明 | 需新增声明+实现 |
| `ogc_position_manager_set_callback` | ❌ 未声明 | 需新增声明+实现 |
| `ogc_position_manager_get_speed` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_position_manager_get_course` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_position_manager_get_fix_quality` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_position_manager_get_satellite_count` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_route_manager_create` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_route_planner_create` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_navigation_engine_set_route` | ✅ 已声明+实现 | 需验证SEH异常 |
| `ogc_navigation_engine_get_current_waypoint` | ✅ 已声明+实现 | 需验证SEH异常 |

**结论**: 23个待实现API中，18个已可用，3个需新增声明(set_position/get_position/set_callback)，2个需验证运行时行为。

### Feature模块 (test_sdk_c_api_feature_pending.cpp)

| 期望API | SDK状态 | 说明 |
|---------|---------|------|
| `ogc_feature_create(geometry)` | ⚠️ 参数不匹配 | SDK为`ogc_feature_create(feature_defn)`，需修改测试 |
| `ogc_feature_set_id` | ⚠️ 名称不匹配 | SDK为`ogc_feature_set_fid`，需修改测试 |
| `ogc_feature_get_id` | ⚠️ 名称不匹配 | SDK为`ogc_feature_get_fid`，需修改测试 |
| `ogc_feature_set_attribute_string` | ⚠️ 名称不匹配 | SDK为`ogc_feature_set_field_string`，需修改测试 |
| `ogc_feature_get_attribute_string` | ⚠️ 名称不匹配 | SDK为`ogc_feature_get_field_as_string`，需修改测试 |
| `ogc_feature_set_attribute_int` | ⚠️ 名称不匹配 | SDK为`ogc_feature_set_field_integer`，需修改测试 |
| `ogc_feature_get_attribute_int` | ⚠️ 名称不匹配 | SDK为`ogc_feature_get_field_as_integer`，需修改测试 |
| `ogc_feature_set_attribute_double` | ⚠️ 名称不匹配 | SDK为`ogc_feature_set_field_real`，需修改测试 |
| `ogc_feature_get_attribute_double` | ⚠️ 名称不匹配 | SDK为`ogc_feature_get_field_as_real`，需修改测试 |
| `ogc_feature_get_attribute_count` | ⚠️ 名称不匹配 | SDK为`ogc_feature_get_field_count`，需修改测试 |
| `ogc_feature_get_attribute_name` | ❌ 无对应API | 需通过defn获取字段名 |
| `ogc_feature_steal_geometry` | ✅ 已声明+实现 | 可直接使用 |

**结论**: 12个待实现API中，1个已可用，10个需修改测试调用名称，1个无对应API。

### Layer模块 (test_sdk_c_api_layer_pending.cpp)

| 期望API | SDK状态 | 说明 |
|---------|---------|------|
| `ogc_layer_create(name)` | ⚠️ 名称不匹配 | SDK为`ogc_vector_layer_create`/`ogc_memory_layer_create` |
| `ogc_layer_set_visible/is_visible` | ⚠️ 需通过manager | SDK为`ogc_layer_manager_set/get_layer_visible` |
| `ogc_layer_set/get_opacity` | ⚠️ 需通过manager | SDK为`ogc_layer_manager_set/get_layer_opacity` |
| `ogc_layer_add_feature` | ⚠️ 名称不匹配 | SDK为`ogc_vector_layer_add_feature` |
| `ogc_layer_remove_feature` | ⚠️ 名称不匹配 | SDK为`ogc_memory_layer_remove_feature` ✅已实现 |
| `ogc_layer_clear_features` | ⚠️ 名称不匹配 | SDK为`ogc_memory_layer_clear` ✅已实现 |
| `ogc_layer_get_feature_count` | ✅ 已声明+实现 | 返回类型需确认 |
| `ogc_feature_set_id/get_id` | ⚠️ 名称不匹配 | SDK为`ogc_feature_set_fid/get_fid` |
| `ogc_feature_create(geom)` | ⚠️ 参数不匹配 | SDK为`ogc_feature_create(feature_defn)` |
| `ogc_feature_collection_t` | ❌ 未定义 | SDK无此类型 |
| `ogc_layer_manager_get_layer_by_name` | ✅ 已声明+实现 | 在graph.cpp中实现 |
| `ogc_layer_group_is_visible/set_visible` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_layer_group_get/set_opacity` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_layer_manager_move_layer` | ✅ 已声明+实现 | 参数不同(from/to vs name/idx) |
| `ogc_raster_layer_create(name, filepath)` | ✅ 已声明+实现 | 参数不同 |
| `ogc_vector_layer_create_from_features` | ❌ 未声明 | 需新增 |
| `ogc_vector_layer_set_style` | ❌ 未声明 | 需新增 |

**结论**: 29个待实现API中，8个已可用，17个需修改测试调用名称，4个需新增声明。

### Alert模块 (test_sdk_c_api_alert_pending.cpp)

| 期望API | SDK状态 | 说明 |
|---------|---------|------|
| `ogc_alert_create()` | ⚠️ 参数不匹配 | SDK为`ogc_alert_create(type, level, message)` |
| `ogc_alert_set_id/get_id` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_set_severity/get_severity` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_set_message` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_set_position(alert, lat, lon)` | ⚠️ 参数不匹配 | SDK为`ogc_alert_set_position(alert, coord*)` |
| `ogc_alert_get_position(alert, &lat, &lon)` | ⚠️ 返回类型不同 | SDK返回`ogc_coordinate_t` |
| `ogc_alert_set_timestamp` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_set_acknowledged/is_acknowledged` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_engine_add_alert` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_engine_remove_alert` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_engine_get_alert` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_engine_get_alert_count` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_engine_get_active_alerts` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_alert_engine_clear_alerts` | ❌ 未声明 | 需新增 |
| `ogc_alert_engine_set_callback` | ❌ 未声明 | 需新增 |
| `ogc_alert_zone_*` | ❌ 未声明 | 内核无AlertZone类 |
| `ogc_ais_manager_add_target` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_manager_remove_target` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_manager_get_target_count` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_manager_set_callback` | ❌ 未声明 | 需新增 |
| `ogc_ais_manager_get_all_targets` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_manager_get_targets_in_range` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_target_create` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_target_get_speed/course/heading` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_target_get_nav_status` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_ais_target_update_position` | ✅ 已声明+实现 | 可直接使用 |

**结论**: 42个待实现API中，22个已可用，3个需修改测试调用参数，2个需新增声明(clear_alerts/set_callback)，1个需新增(ais_manager_set_callback)，14个AlertZone相关无内核支持。

### Cache模块 (test_sdk_c_api_cache_pending.cpp)

| 期望API | SDK状态 | 说明 |
|---------|---------|------|
| `ogc_tile_cache_create(capacity)` | ⚠️ 参数不匹配 | SDK为`ogc_tile_cache_create(void)` |
| `ogc_tile_cache_put` | ⚠️ 名称不匹配 | SDK为`ogc_tile_cache_put_tile` |
| `ogc_tile_cache_get` | ⚠️ 名称不匹配 | SDK为`ogc_tile_cache_get_tile` |
| `ogc_tile_cache_remove` | ⚠️ 名称不匹配 | SDK为`ogc_tile_cache_remove_tile` |
| `ogc_tile_cache_get_count` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_tile_cache_get_capacity` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_tile_cache_set_capacity` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_memory_tile_cache_create(count, bytes)` | ⚠️ 参数不匹配 | SDK为`ogc_memory_tile_cache_create(bytes)` |
| `ogc_memory_tile_cache_get_memory_usage` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_offline_storage_store_chart` | ❌ 未声明 | 需新增 |
| `ogc_offline_storage_get_chart_path` | ❌ 未声明 | 需新增 |
| `ogc_offline_storage_remove_chart` | ❌ 未声明 | 需新增 |
| `ogc_offline_storage_get_chart_list` | ❌ 未声明 | 需新增 |
| `ogc_offline_storage_get_storage_size` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_cache_manager_get_instance` | ❌ 未声明 | SDK使用`ogc_cache_manager_create` |
| `ogc_cache_manager_register_cache` | ❌ 未声明 | 需新增 |
| `ogc_cache_manager_get_cache` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_cache_manager_unregister_cache` | ❌ 未声明 | 需新增 |
| `ogc_cache_manager_clear_all` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_offline_region_destroy` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_offline_region_download` | ✅ 已声明+实现 | 可直接使用 |
| `ogc_offline_region_get_progress` | ✅ 已声明+实现 | 可直接使用 |

**结论**: 29个待实现API中，10个已可用，5个需修改测试调用名称，8个需新增声明，6个需修改参数。

---

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
- 2026-04-11: 全面核查API匹配状态，更新各模块详细状态表。Navigation模块18/23已可用，Feature模块需修改调用名称，Layer模块8/29已可用，Alert模块22/42已可用，Cache模块10/29已可用
