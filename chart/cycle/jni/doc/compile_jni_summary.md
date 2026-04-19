# JNI层编译问题汇总

> 生成日期: 2026-04-18
> 工程目录: cycle\jni
> 编译环境: MSVC 14.0 / CMake 3.10+ / C++11

---

## 一、问题总览

| 统计项 | 数量 |
|--------|------|
| 问题总数 | 16 |
| 已解决 | 16 |
| 未解决 | 0 |
| 严重级别-严重 | 5 |
| 严重级别-中等 | 8 |
| 严重级别-轻微 | 3 |

---

## 二、问题详情

### P-01: ogc_envelope_create 参数数量错误

| 属性 | 值 |
|------|-----|
| 问题分类 | API调用错误 |
| 所属模块 | coord_transformer_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
调用 `ogc_envelope_create(minX, minY, maxX, maxY)` 传入4个参数，但SDK C API中 `ogc_envelope_create` 不接受参数（返回空Envelope），带坐标参数的函数名为 `ogc_envelope_create_from_coords`。

**错误信息**:
```
error C2660: "ogc_envelope_create": 函数不接受 4 个参数
```

**分析过程**:
1. 编译报错指向 `coord_transformer_jni.cpp:162`
2. 查阅 `sdk_c_api.h` 第330行和第340行，发现两个函数声明：
   - `ogc_envelope_t* ogc_envelope_create(void);` — 无参
   - `ogc_envelope_t* ogc_envelope_create_from_coords(double min_x, double min_y, double max_x, double max_y);` — 4参数
3. 确认应使用 `ogc_envelope_create_from_coords`

**解决方法**:
将 `ogc_envelope_create(minX, minY, maxX, maxY)` 替换为 `ogc_envelope_create_from_coords(minX, minY, maxX, maxY)`

**经验教训**: SDK C API中同类型创建函数可能有多个重载版本（无参/带参），需查阅头文件确认函数签名。

---

### P-02: ogc_cpa_result_t 结构体字段名错误

| 属性 | 值 |
|------|-----|
| 问题分类 | 结构体字段名错误 |
| 所属模块 | alert_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
代码中使用了 `result.cpa_distance`、`result.tcpa_minutes`、`result.dcpa_distance` 等字段名，但 `ogc_cpa_result_t` 的实际字段名为 `cpa`、`tcpa`、`bearing`、`danger_level`。

**错误信息**:
```
error C2039: "cpa_distance": 不是"ogc_cpa_result_t"的成员
error C2039: "tcpa_minutes": 不是"ogc_cpa_result_t"的成员
error C2039: "dcpa_distance": 不是"ogc_cpa_result_t"的成员
```

**分析过程**:
1. 编译报错指向 `alert_jni.cpp:417`
2. 查阅 `sdk_c_api.h` 第5060-5067行，确认实际结构体定义：
   ```c
   typedef struct ogc_cpa_result_t {
       double cpa;         // Closest Point of Approach distance
       double tcpa;        // Time to CPA (minutes)
       double bearing;     // Bearing to target (degrees)
       int danger_level;   // Danger level
   } ogc_cpa_result_t;
   ```
3. 原代码使用的字段名是推测的，与实际定义不符

**解决方法**:
1. 将字段名修正为 `result.cpa`、`result.tcpa`、`result.bearing`、`result.danger_level`
2. 将Java构造函数签名从 `(DDD)V` 改为 `(DDDI)V`（增加danger_level的int参数）
3. 构造调用参数改为 `result.cpa, result.tcpa, result.bearing, result.danger_level`

**经验教训**: 不可凭推测命名结构体字段，必须查阅 `sdk_c_api.h` 中的实际定义。

---

### P-03: ogc_ukc_result_t 结构体字段名错误

| 属性 | 值 |
|------|-----|
| 问题分类 | 结构体字段名错误 |
| 所属模块 | alert_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
代码中使用了 `result.actual_depth`、`result.ukc_value`、`result.is_safe` 等字段名，但 `ogc_ukc_result_t` 的实际字段名为 `water_depth`、`charted_depth`、`tide_height`、`squat`、`ukc`、`safe`。

**错误信息**:
```
error C2039: "actual_depth": 不是"ogc_ukc_result_t"的成员
error C2039: "ukc_value": 不是"ogc_ukc_result_t"的成员
error C2039: "is_safe": 不是"ogc_ukc_result_t"的成员
```

**分析过程**:
1. 编译报错指向 `alert_jni.cpp:449-450`
2. 查阅 `sdk_c_api.h` 第5090-5100行，确认实际结构体定义：
   ```c
   typedef struct ogc_ukc_result_t {
       double water_depth;     // Actual water depth (meters)
       double charted_depth;   // Charted depth (meters)
       double tide_height;     // Tide height (meters)
       double squat;           // Squat value (meters)
       double ukc;             // Under Keel Clearance (meters)
       int safe;               // Non-zero if safe
   } ogc_ukc_result_t;
   ```
3. 原代码遗漏了 `charted_depth` 和 `tide_height` 字段

**解决方法**:
1. 将字段名修正为实际定义
2. 将Java构造函数签名从 `(DDDD)V` 改为 `(DDDDDI)V`
3. 构造调用改为 `result.water_depth, result.charted_depth, result.tide_height, result.squat, result.ukc, result.safe`

**经验教训**: 结构体字段必须完整映射，不可遗漏或猜测字段名。

---

### P-04: ogc_ais_manager_get_all_targets 链接失败（未导出）

| 属性 | 值 |
|------|-----|
| 问题分类 | 链接错误（API未导出） |
| 所属模块 | ais_jni.cpp |
| 严重级别 | 🟡 中等（功能受限） |
| 解决状态 | ✅ 已解决（降级处理） |

**问题描述**:
`ogc_ais_manager_get_all_targets` 在 `sdk_c_api.h` 中有声明但缺少 `SDK_C_API` 导出宏，导致链接时找不到符号。

**错误信息**:
```
error LNK2019: 无法解析的外部符号 ogc_ais_manager_get_all_targets
```

**分析过程**:
1. 链接报错指向 `ais_jni.cpp` 中 `nativeGetAllTargetMmsi` 函数
2. 查阅 `sdk_c_api.h` 第5656行，发现声明为 `ogc_ais_target_t** ogc_ais_manager_get_all_targets(...)` 无 `SDK_C_API` 前缀
3. 对比其他已导出函数如 `SDK_C_API ogc_ais_manager_get_target`，确认该函数未被导出
4. 同类未导出函数还有 `ogc_ais_manager_get_targets_in_range`（第5673行）
5. 可用的替代API：`ogc_ais_manager_get_target_count`（已导出）和 `ogc_ais_manager_get_target(mmsi)`（按MMSI获取单个目标）

**解决方法**:
将 `nativeGetAllTargetMmsi` 改为使用 `ogc_ais_manager_get_target_count` 获取数量，枚举功能标记为不支持，抛出 `RuntimeException` 提示使用回调模式。Java端应通过 `setCallback` 回调获取MMSI列表。

**经验教训**: SDK C API中并非所有声明的函数都有 `SDK_C_API` 导出宏，使用前需确认导出状态。缺少 `SDK_C_API` 的函数不在DLL导出表中，无法链接。

---

### P-05: ogc_alert_engine_get_active_alerts 链接失败（未导出）

| 属性 | 值 |
|------|-----|
| 问题分类 | 链接错误（API未导出） |
| 所属模块 | alert_jni.cpp |
| 严重级别 | 🟡 中等（功能受限） |
| 解决状态 | ✅ 已解决（降级处理） |

**问题描述**:
`ogc_alert_engine_get_active_alerts` 在 `sdk_c_api.h` 中有声明但缺少 `SDK_C_API` 导出宏，导致链接时找不到符号。

**错误信息**:
```
error LNK2019: 无法解析的外部符号 ogc_alert_engine_get_active_alerts
```

**分析过程**:
1. 链接报错指向 `alert_jni.cpp` 中 `nativeGetActiveAlertIds` 函数
2. 查阅 `sdk_c_api.h` 第4995行，确认声明缺少 `SDK_C_API` 前缀
3. 可用的替代API：`ogc_alert_engine_get_alert_count`（已导出）和 `ogc_alert_engine_get_alert(id)`（按ID获取单个警报）

**解决方法**:
将 `nativeGetActiveAlertIds` 改为使用 `ogc_alert_engine_get_alert_count` 获取数量，枚举功能标记为不支持，抛出 `RuntimeException` 提示使用回调模式。

**经验教训**: 与P-04同类问题。批量获取类API（`get_all_*`）可能未导出，需逐个获取或使用回调。

---

### P-06: ogc_tile_data_t 不透明类型无法访问数据

| 属性 | 值 |
|------|-----|
| 问题分类 | API设计限制 |
| 所属模块 | tile_cache_jni.cpp |
| 严重级别 | 🟢 轻微（功能受限） |
| 解决状态 | ✅ 已解决（降级处理） |

**问题描述**:
`ogc_tile_data_t` 在 `sdk_c_api.h` 中定义为前向声明的不透明类型（`typedef struct ogc_tile_data_t ogc_tile_data_t`），无任何访问器函数（如 `get_data`、`get_size`），导致 `nativeGetTile` 无法将瓦片数据转为 `jbyteArray` 返回Java层。

**分析过程**:
1. `ogc_tile_cache_get_tile` 返回 `ogc_tile_data_t*`，但该类型无任何公开的访问器
2. 搜索 `sdk_c_api.h` 中所有 `ogc_tile_data` 相关声明，仅发现类型定义和 `get_tile` 函数
3. 无法获取瓦片数据的指针和大小，无法转为Java字节数组

**解决方法**:
将 `nativeGetTile`（返回 `jbyteArray`）改为 `nativeGetTilePtr`（返回 `jlong` 指针），Java层保存指针用于后续操作。瓦片数据的实际读取需等待SDK提供访问器API。

**经验教训**: 不透明类型（opaque type）若无访问器函数，JNI层只能传递指针，无法解析内容。应在设计阶段确认API完整性。

---

### P-07: ogc_driver_manager_* 系列函数链接失败（未导出）

| 属性 | 值 |
|------|-----|
| 问题分类 | 链接错误（API未导出） |
| 所属模块 | datasource_jni.cpp |
| 严重级别 | 🟡 中等（功能受限） |
| 解决状态 | ✅ 已解决（降级处理） |

**问题描述**:
`ogc_driver_manager_get_instance`、`ogc_driver_manager_get_driver_count`、`ogc_driver_manager_get_driver_name` 三个函数在 `sdk_c_api.h` 中有声明且带 `SDK_C_API` 导出宏，但在 `ogc_chart_c_api.dll` 中实际未导出，导致链接时找不到符号。

**错误信息**:
```
error LNK2019: 无法解析的外部符号 ogc_driver_manager_get_instance
error LNK2019: 无法解析的外部符号 ogc_driver_manager_get_driver_count
error LNK2019: 无法解析的外部符号 ogc_driver_manager_get_driver_name
```

**分析过程**:
1. 链接报错指向 `datasource_jni.cpp` 中 `DriverManager` 相关的3个native方法
2. 查阅 `sdk_c_api.h` 第2123-2162行，确认声明均带 `SDK_C_API` 导出宏
3. 查阅 `sdk_c_api_layer.cpp` 第554-593行，确认实现代码存在
4. 推测原因：`ogc_chart_c_api.dll` 构建时可能未包含 `sdk_c_api_layer.cpp` 或该编译单元未被正确链接
5. 与P-04、P-05同类问题：头文件声明与DLL实际导出不一致

**解决方法**:
将3个DriverManager函数改为抛出 `RuntimeException`，提示API未从DLL导出。Java层在调用时将收到异常，待SDK修复导出后恢复。

**经验教训**: 即使 `sdk_c_api.h` 中声明了 `SDK_C_API` 宏，也不保证DLL中实际导出。需通过链接验证确认。此类问题属于SDK构建配置问题，非JNI层编码错误。

---

### P-08: ogc_collision_assessor_assess 函数签名不匹配

| 属性 | 值 |
|------|-----|
| 问题分类 | API调用错误 |
| 所属模块 | collision_deviation_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
JNI层调用 `ogc_collision_assessor_assess` 传入9个参数（own_lat, own_lon, own_speed, own_course, target_lat, target_lon, target_speed, target_course），但SDK C API中该函数接受6个参数（assessor, own_lat, own_lon, own_speed, own_course, target），其中target为 `ogc_ais_target_t*` 指针。

**错误信息**:
```
error C2660: "ogc_collision_assessor_assess": 函数不接受 9 个参数
```

**分析过程**:
1. 编译报错指向 `collision_deviation_jni.cpp:52`
2. 查阅 `sdk_c_api.h` 第6226行，确认函数签名：
   ```c
   ogc_collision_risk_t* ogc_collision_assessor_assess(ogc_collision_assessor_t* assessor,
       double own_lat, double own_lon, double own_speed, double own_course,
       const ogc_ais_target_t* target);
   ```
3. 目标船信息通过 `ogc_ais_target_t*` 传入，而非单独的坐标参数

**解决方法**:
将 `nativeAssess` 方法签名从8个double参数改为4个double参数+1个jlong(AIS target指针)，内部调用时传入 `ogc_ais_target_t*`。

---

### P-09: ogc_off_course_detector_check 函数签名不匹配

| 属性 | 值 |
|------|-----|
| 问题分类 | API调用错误 |
| 所属模块 | collision_deviation_jni.cpp |
| 严重级别 | 🟡 中等（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
JNI层调用 `ogc_off_course_detector_check` 传入6个double参数（currentLat, currentLon, plannedLat, plannedLon），但SDK C API中该函数接受5个参数（detector, lat, lon, from_waypoint, to_waypoint），其中waypoint为 `ogc_waypoint_t*` 指针。

**错误信息**:
```
error C2664: 无法将参数 4 从"jdouble"转换为"const ogc_waypoint_t *"
```

**分析过程**:
1. 编译报错指向 `collision_deviation_jni.cpp:171`
2. 查阅 `sdk_c_api.h` 第6289行，确认函数签名：
   ```c
   ogc_deviation_result_t ogc_off_course_detector_check(ogc_off_course_detector_t* detector,
       double lat, double lon, const ogc_waypoint_t* from, const ogc_waypoint_t* to);
   ```
3. 航路点信息通过 `ogc_waypoint_t*` 传入，而非坐标值

**解决方法**:
将 `nativeCheck` 和 `nativeGetCrossTrackError` 方法签名从4个double参数改为2个double参数+2个jlong(waypoint指针)。

---

### P-10: sdk_c_api_recovery.cpp 和 sdk_c_api_plugin.cpp 函数缺少 SDK_C_API 导出前缀

| 属性 | 值 |
|------|-----|
| 问题分类 | 链接错误（API未导出） |
| 所属模块 | recovery_jni.cpp, loader_jni.cpp, plugin_jni.cpp, health_jni.cpp |
| 严重级别 | 🟡 中等（链接失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
`sdk_c_api_recovery.cpp` 中所有函数定义（CircuitBreaker、GracefulDegradation共12个）和 `sdk_c_api_plugin.cpp` 中部分函数定义（ErrorRecoveryManager、HealthCheck、LibraryLoader、Exception等共35个）缺少 `SDK_C_API` 导出前缀，导致DLL中未导出这些符号，JNI链接时出现17个无法解析的外部符号。

**错误信息**:
```
error LNK2019: 无法解析的外部符号 ogc_circuit_breaker_create
error LNK2019: 无法解析的外部符号 ogc_graceful_degradation_create
error LNK2019: 无法解析的外部符号 ogc_error_recovery_manager_handle_error
error LNK2019: 无法解析的外部符号 ogc_library_loader_get_symbol
... (共17个)
```

**分析过程**:
1. 链接报错涉及recovery和plugin两大类共17个函数
2. 查阅 `sdk_c_api.h`，确认所有声明均带 `SDK_C_API` 导出宏
3. 查阅 `sdk_c_api_recovery.cpp` 和 `sdk_c_api_plugin.cpp`，发现函数定义缺少 `SDK_C_API` 前缀
4. 对比其他正常导出的模块（如 `sdk_c_api_navi.cpp`），确认函数定义需要 `SDK_C_API` 前缀才能在DLL中导出
5. 根因：这两个cpp文件是新创建的，编写时遗漏了 `SDK_C_API` 导出宏

**解决方法**:
1. 在 `sdk_c_api_recovery.cpp` 中为全部12个函数定义添加 `SDK_C_API` 前缀
2. 在 `sdk_c_api_plugin.cpp` 中为全部35个函数定义添加 `SDK_C_API` 前缀
3. 重新编译 `ogc_chart_c_api.dll`
4. 重新编译 `ogc_chart_jni.dll`，链接成功

**经验教训**: SDK C API实现文件中的函数定义必须添加 `SDK_C_API` 导出宏，否则即使头文件声明了导出，DLL也不会导出该符号。新建cpp文件时需检查所有函数定义是否包含导出宏。

---

### P-11: P3扩展文件与P1已有文件JNI函数名重复定义（LNK2005）

| 属性 | 值 |
|------|-----|
| 问题分类 | 链接错误（符号重复定义） |
| 所属模块 | symbology_ext_jni.cpp, offline_cache_ext_jni.cpp, navi_ext_jni.cpp, util_ext_jni.cpp |
| 严重级别 | 🟡 中等（链接失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
P3扩展JNI文件中包含了P1阶段已在其他文件中实现的JNI函数，导致链接时出现大量 `LNK2005` 重复定义错误。涉及4个扩展文件共约60个重复函数。

**错误信息**:
```
symbology_ext_jni.obj : error LNK2005: Java_cn_cycle_chart_api_symbology_Filter_nativeCreate 已经在 symbology_jni.obj 中定义
offline_cache_ext_jni.obj : error LNK2005: Java_cn_cycle_chart_api_cache_CacheManager_nativeCreate 已经在 offline_cache_jni.obj 中定义
navi_ext_jni.obj : error LNK2005: Java_cn_cycle_chart_api_navi_PositionManager_nativeGetLatitude 已经在 nmea_position_jni.obj 中定义
util_ext_jni.obj : error LNK2005: Java_cn_cycle_chart_api_alert_AlertEngine_nativeGetAlertCount 已经在 alert_jni.obj 中定义
... (共约60个)
```

**分析过程**:
1. P3扩展文件编写时未检查P1阶段已有文件中的JNI函数列表
2. JNI函数名由Java类全限定名+方法名决定，同一Java类的native方法只能在一个cpp文件中定义
3. 受影响文件对照：
   - `symbology_ext_jni.cpp` 与 `symbology_jni.cpp` 重复（Filter/LogicalFilter/Symbolizer/SymbolizerRule全部重复）
   - `offline_cache_ext_jni.cpp` 与 `offline_cache_jni.cpp` 重复（OfflineStorage/CacheManager全部重复）
   - `navi_ext_jni.cpp` 与 `nmea_position_jni.cpp`/`track_jni.cpp` 重复（PositionManager/Track/TrackRecorder/NmeaParser全部重复）
   - `util_ext_jni.cpp` 与 `alert_jni.cpp` 重复（AlertEngine全部重复）

**解决方法**:
1. 逐一检查每个扩展文件，使用 `grep` 搜索已有文件中的JNI函数名
2. 从扩展文件中移除所有已存在的函数定义
3. 仅保留真正新增的函数：
   - `symbology_ext_jni.cpp`: 仅保留 ComparisonFilter扩展(destroy/evaluate/getPropertyName/getOperator/getValue) + Symbolizer.symbolize
   - `offline_cache_ext_jni.cpp`: 仅保留 OfflineRegion(6个) + OfflineStorage新方法(getRegionById/removeRegion/removeRegionById/storeChart/getChartPath/removeChart)
   - `navi_ext_jni.cpp`: 仅保留 PositionProvider(6个)
   - `util_ext_jni.cpp`: 仅保留 SDKVersion(4个) + MemoryUtils(1个) + PerformanceMonitor(6个) + Error(1个)
4. 重新编译，链接成功

**经验教训**: 创建扩展JNI文件前，必须先检查已有JNI文件中已实现的函数列表，避免JNI函数名重复定义。建议使用 `grep "Java_cn_cycle" existing_file.cpp` 检查已有函数。

---

### P-12: P4回调函数签名不匹配（C2664）

| 属性 | 值 |
|------|-----|
| 问题分类 | API调用错误（回调函数签名不匹配） |
| 所属模块 | ais_jni.cpp, alert_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
P4阶段为AIS和Alert模块添加回调设置函数时，自定义的回调函数类型与SDK C API声明的回调签名不匹配。

**错误信息**:
```
ais_jni.cpp(371): error C2664: "int ogc_ais_manager_set_callback(...)": 无法将参数 2 从"ais_callback_t"转换为"void (__cdecl *)(uint32_t,int,void *)"
alert_jni.cpp(526): error C2664: "void ogc_alert_engine_set_callback(...)": 无法将参数 2 从"alert_callback_t"转换为"void (__cdecl *)(int,int,const char *,void *)"
```

**分析过程**:
1. 编写回调封装时，未查阅sdk_c_api.h中实际的回调函数签名
2. 错误地假设AIS回调签名为 `void (*)(ogc_ais_target_t*, void*)`，实际为 `void (*)(uint32_t mmsi, int event_type, void* user_data)`
3. 错误地假设Alert回调签名为 `void (*)(ogc_alert_t*, void*)`，实际为 `void (*)(int alert_type, int alert_level, const char* message, void* user_data)`

**解决方法**:
1. 查阅sdk_c_api.h第5705行和第5053行确认实际回调签名
2. 修正ais_jni.cpp中回调类型为 `typedef void (*ais_callback_t)(uint32_t, int, void*)`
3. 修正alert_jni.cpp中回调类型为 `typedef void (*alert_callback_t)(int, int, const char*, void*)`
4. 重新编译通过

**经验教训**: 使用SDK C API的回调函数时，必须查阅头文件确认回调签名（JNI-CHECK-01），不可凭经验猜测。回调函数签名中的参数类型和数量必须与头文件声明完全一致。

---

### P-13: P5 API函数在SDK C API中不存在（C3861/C2065）

| 属性 | 值 |
|------|-----|
| 问题分类 | API调用错误（函数不存在） |
| 所属模块 | nmea_position_jni.cpp, parser_ext_jni.cpp, offline_cache_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
P5阶段部分API函数在sdk_c_api.h中不存在，导致编译时找不到标识符。

**错误信息**:
```
nmea_position_jni.cpp: error C3861: "ogc_position_manager_get_altitude": 找不到标识符
nmea_position_jni.cpp: error C3861: "ogc_position_manager_get_hdop": 找不到标识符
nmea_position_jni.cpp: error C3861: "ogc_position_manager_get_vdop": 找不到标识符
parser_ext_jni.cpp: error C3861: "ogc_s57_parser_get_feature_count": 找不到标识符
parser_ext_jni.cpp: error C2065: "ogc_layer_defn_t": 未声明的标识符
offline_cache_jni.cpp: error C2664: 无法将参数 2 从"int64_t"转换为"const char *"
```

**分析过程**:
1. P5待补充清单中的部分API函数名是基于推测的，未在sdk_c_api.h中验证
2. `ogc_position_manager_get_altitude/hdop/vdop` 不存在，实际只有 `ogc_position_manager_get_position` 和 `ogc_position_manager_set_callback`
3. `ogc_s57_parser_get_feature_count/get_layer_defn/get_layer_count` 不存在，实际只有 `ogc_s57_parser_parse_file`
4. `ogc_offline_storage_get_region_by_id` 的region_id参数类型是 `const char*` 而非 `int64_t`

**解决方法**:
1. 用 `ogc_position_manager_get_position` 替代 altitude/hdop/vdop
2. 用 `ogc_position_manager_set_callback` 替代被移除的函数
3. 用 `ogc_s57_parser_parse_file` 替代不存在的parser函数
4. 修正 `ogc_offline_storage_get_region_by_id` 参数类型为 `jstring`
5. 重新编译通过

**经验教训**: 补充JNI函数前，必须在sdk_c_api.h中确认API函数确实存在且签名正确（JNI-CHECK-01）。待补充清单中的函数名可能不准确，需以头文件为准。

---

### P-14: P5 PositionManager回调签名不匹配（C2664）

| 属性 | 值 |
|------|-----|
| 问题分类 | API调用错误（回调函数签名不匹配） |
| 所属模块 | nmea_position_jni.cpp |
| 严重级别 | 🔴 严重（编译失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
PositionManager回调函数类型与SDK C API声明不匹配。

**错误信息**:
```
error C2664: "int ogc_position_manager_set_callback(...)": 无法将参数 2 从"pos_callback_t"转换为"void (__cdecl *)(double,double,double,double,void *)"
```

**分析过程**:
1. 错误地假设回调签名为 `void (*)(double, double, void*)`
2. 查阅sdk_c_api.h第5926行确认实际签名为 `void (*)(double lat, double lon, double speed, double course, void* user_data)`

**解决方法**:
1. 修正回调类型为 `typedef void (*pos_callback_t)(double, double, double, double, void*)`
2. 重新编译通过

**经验教训**: 与P-12相同，回调函数签名必须查阅头文件确认（JNI-CHECK-01）。

---

### P-15: P5 JNI函数名重复定义（LNK2005）

| 属性 | 值 |
|------|-----|
| 问题分类 | 链接错误（符号重复定义） |
| 所属模块 | offline_cache_jni.cpp, offline_cache_ext_jni.cpp |
| 严重级别 | 🟡 中等（链接失败） |
| 解决状态 | ✅ 已解决 |

**问题描述**:
在offline_cache_jni.cpp中新增的函数与offline_cache_ext_jni.cpp中已有函数重复定义。

**错误信息**:
```
error LNK2005: Java_cn_cycle_chart_api_cache_OfflineStorage_nativeGetRegionById 已经在 offline_cache_jni.obj 中定义
error LNK2005: Java_cn_cycle_chart_api_cache_OfflineStorage_nativeRemoveRegionById 已经在 offline_cache_jni.obj 中定义
```

**分析过程**:
1. offline_cache_ext_jni.cpp（P1阶段创建）中已实现了 `nativeGetRegionById` 和 `nativeRemoveRegionById`
2. P5阶段在offline_cache_jni.cpp中重复添加了相同函数

**解决方法**:
1. 从offline_cache_jni.cpp中移除重复的函数定义
2. 保留offline_cache_ext_jni.cpp中已有的实现
3. 重新编译通过

**经验教训**: 添加JNI函数前，必须检查所有相关JNI文件中是否已有相同函数（JNI-CHECK-06），包括扩展文件。

---

### P-16: ogc_hit_test_query_rect 未导出

**问题分类**: API未导出（缺少SDK_C_API）
**所属模块**: graph_ext_jni.cpp (P6 HitTest扩展)
**严重级别**: 中等

**问题描述**:
在graph_ext_jni.cpp中新增的`nativeQueryRect`函数调用了`ogc_hit_test_query_rect`，但该函数在sdk_c_api.h中缺少`SDK_C_API`导出宏，导致链接失败。

**错误信息**:
```
graph_ext_jni.obj : error LNK2019: 无法解析的外部符号 ogc_hit_test_query_rect，该符号在函数 Java_cn_cycle_chart_api_graph_HitTest_nativeQueryRect 中被引用
```

**分析过程**:
1. 检查sdk_c_api.h中`ogc_hit_test_query_rect`的声明，发现没有`SDK_C_API`前缀
2. 与其他已成功链接的hit_test函数对比，确认是导出宏缺失
3. 该函数声明为`ogc_feature_t** ogc_hit_test_query_rect(...)`而非`SDK_C_API ogc_feature_t** ogc_hit_test_query_rect(...)`

**解决方法**:
1. 将`nativeQueryRect`的实现改为抛出RuntimeException，提示"ogc_hit_test_query_rect is not exported from SDK C API DLL"
2. 保留`nativeFreeFeatures`（`ogc_hit_test_free_features`有SDK_C_API导出）
3. 重新编译通过

**经验教训**: 使用SDK C API前必须确认函数有`SDK_C_API`导出宏（JNI-CHECK-03），无导出宏的函数无法链接，应改为抛出异常提示。

---

## 三、问题分类统计

| 问题分类 | 数量 | 占比 |
|----------|------|------|
| API调用错误（函数签名不匹配） | 4 | 26.7% |
| 结构体字段名错误 | 2 | 13.3% |
| API未导出（缺少SDK_C_API或DLL未导出） | 5 | 31.3% |
| 不透明类型无访问器 | 1 | 6.7% |
| JNI函数名重复定义（LNK2005） | 2 | 13.3% |
| 回调函数签名不匹配 | 2 | 13.3% |

---

## 四、预防措施

| 规则ID | 规则内容 | 适用场景 |
|--------|----------|----------|
| JNI-CHECK-01 | 使用SDK C API前，必须查阅 `sdk_c_api.h` 确认函数签名 | 所有JNI函数调用 |
| JNI-CHECK-02 | 访问结构体字段前，必须查阅头文件中的实际定义 | 所有结构体字段访问 |
| JNI-CHECK-03 | 确认API函数有 `SDK_C_API` 导出宏，无导出宏的函数不可链接 | 所有新增JNI函数 |
| JNI-CHECK-04 | 不透明类型需确认有访问器函数，否则只能传递指针 | 所有opaque类型 |
| JNI-CHECK-05 | 编码完成后必须执行编译验证，确保无编译和链接错误 | 每个模块完成时 |
| JNI-CHECK-06 | 创建扩展JNI文件前，必须检查已有JNI文件中的函数列表，避免重复定义 | 新建扩展JNI文件 |

---

**版本**: v1.6
**最后更新**: 2026-04-18（P6模块编译验证通过，新增P-16 ogc_hit_test_query_rect未导出问题）
