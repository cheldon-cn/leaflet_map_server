# 模块列表

> **更新日期**: 2026-04-10  
> **项目**: OGCGeometry v2.3.0

---

## ⚠️ 前置说明

> **重要**: 在获取各模块详细信息之前，必须先了解以下内容：
> 
> 1. **编译与安装目录** - 了解项目的构建输出结构和安装路径配置
> 2. **外部依赖** - 确认所需第三方库的安装和配置
> 3. **模块依赖关系** - 理解模块间的依赖层级，确保编译顺序正确
>
> 请先阅读以下章节，再查阅各模块详情。

---

## 模块概览

| 模块 | 路径 | 说明 |
|------|------|------|
| base | code/base | 基础模块：日志、性能监控、线程安全 |
| geom | code/geom | 几何模块：OGC标准几何类型、空间操作、空间索引 |
| chart/parser | code/chart/parser | 海图解析：S57/S100/S102格式解析、GDAL集成 |
| feature | code/feature | 要素模块：地理要素、属性管理 |
| layer | code/layer | 图层模块：图层管理、渲染控制 |
| database | code/database | 数据库模块：PostgreSQL、SQLite支持 |
| draw | code/draw | 绘制模块：多引擎渲染、设备抽象 |
| proj | code/proj | 投影模块：坐标转换、投影支持 |
| cache | code/cache | 缓存模块：瓦片缓存、磁盘缓存 |
| symbology | code/symbology | 符号化模块：S52样式、符号渲染 |
| alert | code/alert | 预警模块：航行预警、规则引擎 |
| alarm | code/alarm | 告警模块：告警系统、REST接口 |
| graph | code/graph | 图形模块：集成绘制、并发渲染 |
| navi | code/navi | 导航模块：航线规划、导航服务 |
| cycle/chart_c_api | code/cycle/chart_c_api | C API：跨语言接口 |

---

## 编译与安装目录

> **基准目录**: 本文档中所有路径均相对于**项目根目录**（即 `e:\program\trae\chart/`）

### 目录结构

| 目录类型 | 路径 | 说明 |
|----------|------|------|
| 编译根目录 | `build/` | CMake构建输出目录 |
| 动态库/测试 | `build/test/` | `.dll`、`.exe` 文件 |
| 静态库/导入库 | `build/lib/` | `.lib` 文件 |
| 安装根目录 | `install/` | `make install` 输出目录 |
| 头文件 | `install/include/ogc/` | 各模块头文件 |
| 动态库/可执行 | `install/bin/` | 运行时文件 |
| 静态库/导入库 | `install/lib/` | 链接时文件 |

### 各模块编译产物

| 模块 | 动态库 | 导入库 | 头文件目录 |
|------|--------|--------|------------|
| base | ogc_base.dll | ogc_base.lib | include/ogc/base/ |
| geom | ogc_geometry.dll | ogc_geometry.lib | include/ogc/geom/ |
| chart/parser | - | chart_parser.lib | include/ogc/parser/ |
| feature | ogc_feature.dll | ogc_feature.lib | include/ogc/feature/ |
| layer | ogc_layer.dll | ogc_layer.lib | include/ogc/layer/ |
| database | ogc_database.dll | ogc_database.lib | include/ogc/db/ |
| draw | ogc_draw.dll | ogc_draw.lib | include/ogc/draw/ |
| proj | ogc_proj.dll | ogc_proj.lib | include/ogc/proj/ |
| cache | ogc_cache.dll | ogc_cache.lib | include/ogc/cache/ |
| symbology | ogc_symbology.dll | ogc_symbology.lib | include/ogc/symbology/ |
| alert | ogc_alert.dll | ogc_alert.lib | include/ogc/alert/ |
| alarm | alert_system.dll | alert_system.lib | include/ogc/alert/ |
| graph | ogc_graph.dll | ogc_graph.lib | include/ogc/graph/ |
| navi | ogc_navi.dll | ogc_navi.lib | include/ogc/navi/ |
| cycle/chart_c_api | - | - | include/ogc/capi/ |

---

## 模块依赖关系

```
base (无依赖)
  ↓
geom → feature → layer → draw → graph
  ↓       ↓        ↓
proj   database  symbology
  ↓                ↓
cache            alert → alarm
                   ↓
                 navi
```

---

## 外部依赖

### 依赖库

| 依赖库 | 使用模块 | 说明 |
|--------|----------|------|
| GDAL | chart/parser | 地理数据抽象库 |
| libpq | database | PostgreSQL客户端库 |
| sqlite3 | database, graph | 嵌入式数据库 |
| libspatialite | database | 空间数据库扩展 |
| libcurl | graph | HTTP客户端库 |
| libxml2 | chart/parser | XML解析库（可选） |
| GoogleTest | 所有测试 | 单元测试框架 |

### 依赖路径配置

```cmake
set(POSTGRESQL_ROOT "D:/program/PostgreSQL/13" CACHE PATH "PostgreSQL root directory")
set(SQLITE3_ROOT "F:/win/3rd/sqlite3" CACHE PATH "SQLite3 root directory")
set(GEOS_ROOT "F:/win/3rd/GEOS3.10" CACHE PATH "GEOS root directory")
set(PROJ_ROOT "F:/win/3rd/PROJ" CACHE PATH "PROJ root directory")
set(CURL_ROOT "F:/win/3rd/libcurl_x64" CACHE PATH "cURL library root directory")
set(GTEST_ROOT "F:/win/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(GDAL_ROOT "F:/win/3rd/gdal-3.9.3" CACHE PATH "GDAL root directory")
set(LIBXML2_ROOT "F:/win/3rd/libxml2" CACHE PATH "libxml2 root directory")
```

---

## 编译状态

| 状态 | 模块数 | 模块列表 |
|------|--------|----------|
| ✅ 成功 | 15 | base, geom, chart/parser, feature, layer, database, draw, proj, cache, symbology, alert, alarm, graph, navi, cycle/chart_c_api |
| ❌ 失败 | 0 | - |
| ⏭️ 跳过 | 0 | - |

### 测试统计

| 项目 | 测试文件数 | 用例数 |
|------|------------|--------|
| 总计 | 82 | 2436 |
| ✅ 通过 | 80 | 2429 |
| ❌ 失败 | 0 | 0 |
| ⏭️ 跳过 | 2 | 7 |

**文件通过率**: 97.6% | **用例通过率**: 99.7%

**详细报告**: [0_test_report.md](./0_test_report.md)

---

## 各模块测试文件详情

### 总体统计

| 项目 | 数量 |
|------|------|
| 测试文件总数 | 82 |
| 测试用例总数 | 2436 |
| 通过文件数 | 80 |
| 跳过文件数 | 2 |
| 编译失败文件数 | 1 |

### 模块汇总

| 模块 | 测试文件数 | 用例数 | 说明 |
|------|------------|--------|------|
| base | 4 | 65 | 日志、性能监控、线程安全测试 |
| geom | 1 | 11 | 几何类型集成测试 |
| chart/parser | 1 | 226 | S57/S100/S102格式解析测试 |
| feature | 1 | 228 | 地理要素、属性管理测试 |
| layer | 1 | 339 | 图层管理、渲染控制测试 |
| database | 2 | 31 | 数据库集成测试（1个跳过） |
| draw | 20 | 317 | 渲染引擎、标签、LOD测试 |
| proj | 4 | 84 | 坐标转换、投影测试 |
| cache | 5 | 118 | 瓦片缓存、离线存储测试（1个跳过） |
| symbology | 15 | 289 | 符号化、过滤器、S52样式测试 |
| alert | 1 | 221 | 航行预警测试 |
| alarm | 10 | 111 | REST接口、WebSocket、天气预警测试 |
| graph | 16 | 407 | 图形渲染、交互、并发测试 |
| navi | 1 | 281 | 航线规划、导航服务测试 |
| cycle/chart_c_api | 1 | 0 | C API跨语言接口测试（编译失败） |

### base模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_log.exe | 16 | ✅ | 日志系统测试 | |
| test_performance_metrics.exe | 20 | ✅ | 性能指标测试 | |
| test_performance_monitor.exe | 19 | ✅ | 性能监控测试 | |
| test_thread_safe.exe | 10 | ✅ | 线程安全测试 | |

### geom模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_it_geometry.exe | 11 | ✅ | 几何类型集成测试 | |

### chart/parser模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| chart_parser_tests.exe | 226 | ✅ | S57/S100/S102格式解析测试 | |

### feature模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| ogc_feature_tests.exe | 228 | ✅ | 地理要素、属性管理测试 | |

### layer模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| ogc_layer_tests.exe | 339 | ✅ | 图层管理、渲染控制测试 | |

### database模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| ogc_database_test.exe | 26 | ⏭️ | 数据库连接测试 | 执行超过10秒 |
| test_it_database.exe | 5 | ✅ | 数据库集成测试 | PostgreSQL/SQLite |

### draw模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_draw_error.exe | 7 | ✅ | 绘制错误处理测试 | |
| test_draw_facade.exe | 23 | ✅ | 绘制门面接口测试 | |
| test_draw_params.exe | 11 | ✅ | 绘制参数测试 | |
| test_draw_result.exe | 4 | ✅ | 绘制结果测试 | |
| test_image_draw.exe | 23 | ✅ | 图像绘制测试 | |
| test_it_basic_render.exe | 14 | ✅ | 基础渲染集成测试 | |
| test_it_draw_context.exe | 10 | ✅ | 绘制上下文集成测试 | |
| test_it_image_output.exe | 14 | ✅ | 图像输出集成测试 | |
| test_it_pdf_output.exe | 9 | ✅ | PDF输出集成测试 | |
| test_it_transform_render.exe | 5 | ✅ | 变换渲染集成测试 | |
| test_label_conflict.exe | 16 | ✅ | 标签冲突检测测试 | |
| test_label_engine.exe | 18 | ✅ | 标签引擎测试 | |
| test_label_placement.exe | 18 | ✅ | 标签放置测试 | |
| test_lod.exe | 21 | ✅ | LOD细节层次测试 | |
| test_lod_manager.exe | 24 | ✅ | LOD管理器测试 | |
| test_render_queue.exe | 21 | ✅ | 渲染队列测试 | |
| test_render_task.exe | 24 | ✅ | 渲染任务测试 | |
| test_tile_key.exe | 14 | ✅ | 瓦片键测试 | |
| test_tile_cache.exe | 27 | ✅ | 瓦片缓存测试 | |
| test_transform_manager.exe | 18 | ✅ | 变换管理器测试 | |

### proj模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_coord_system_preset.exe | 22 | ✅ | 坐标系预设测试 | |
| test_coordinate_transform.exe | 25 | ✅ | 坐标转换测试 | |
| test_coordinate_transformer.exe | 15 | ✅ | 坐标转换器测试 | |
| test_proj_transformer.exe | 22 | ✅ | 投影转换器测试 | |

### cache模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_data_encryption.exe | 35 | ✅ | 数据加密测试 | |
| test_disk_tile_cache.exe | 23 | ✅ | 磁盘瓦片缓存测试 | |
| test_multi_level_tile_cache.exe | 30 | ✅ | 多级瓦片缓存测试 | |
| test_offline_storage_manager.exe | - | ⏭️ | 离线存储管理器测试 | 执行超过10秒 |
| test_offline_sync_manager.exe | 30 | ✅ | 离线同步管理器测试 | |

### symbology模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_comparison_filter.exe | 18 | ✅ | 比较过滤器测试 | |
| test_composite_symbolizer.exe | 13 | ✅ | 复合符号化器测试 | |
| test_filter.exe | 14 | ✅ | 过滤器测试 | |
| test_icon_symbolizer.exe | 20 | ✅ | 图标符号化器测试 | |
| test_line_symbolizer.exe | 20 | ✅ | 线符号化器测试 | |
| test_logical_filter.exe | 17 | ✅ | 逻辑过滤器测试 | |
| test_point_symbolizer.exe | 15 | ✅ | 点符号化器测试 | |
| test_polygon_symbolizer.exe | 19 | ✅ | 多边形符号化器测试 | |
| test_raster_symbolizer.exe | 24 | ✅ | 栅格符号化器测试 | |
| test_s52_style_manager.exe | 44 | ✅ | S52样式管理器测试 | |
| test_s52_symbol_renderer.exe | 17 | ✅ | S52符号渲染器测试 | |
| test_spatial_filter.exe | 20 | ✅ | 空间过滤器测试 | |
| test_symbolizer.exe | 17 | ✅ | 符号化器测试 | |
| test_symbolizer_rule.exe | 24 | ✅ | 符号化规则测试 | |
| test_text_symbolizer.exe | 27 | ✅ | 文本符号化器测试 | |

### alert模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| ogc_alert_test.exe | 221 | ✅ | 航行预警测试 | |

### alarm模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| chart_data_adapter_test.exe | - | ✅ | 海图数据适配器测试 | |
| external_data_gateway_test.exe | 26 | ✅ | 外部数据网关测试 | |
| integration_test.exe | 25 | ✅ | 集成测试 | |
| performance_test.exe | 7 | ✅ | 性能测试 | |
| rest_controller_test.exe | 19 | ✅ | REST控制器测试 | |
| ship_info_test.exe | 5 | ✅ | 船舶信息测试 | |
| threshold_config_test.exe | 6 | ✅ | 阈值配置测试 | |
| weather_alert_engine_test.exe | 9 | ✅ | 天气预警引擎测试 | |
| weather_data_adapter_test.exe | 19 | ✅ | 天气数据适配器测试 | |
| websocket_service_test.exe | 20 | ✅ | WebSocket服务测试 | |

### graph模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| test_acceptance.exe | 13 | ✅ | 验收测试 | |
| test_async_renderer.exe | 28 | ✅ | 异步渲染器测试 | |
| test_basic_render_task.exe | 23 | ✅ | 基础渲染任务测试 | |
| test_clipper.exe | 22 | ✅ | 裁剪器测试 | |
| test_day_night_mode_manager.exe | 42 | ✅ | 昼夜模式管理器测试 | |
| test_device_type.exe | 2 | ✅ | 设备类型测试 | |
| test_engine_type.exe | 3 | ✅ | 引擎类型测试 | |
| test_graph_boundary.exe | 45 | ✅ | 图形边界测试 | |
| test_graph_concurrent.exe | 14 | ✅ | 图形并发测试 | |
| test_graph_performance.exe | 5 | ✅ | 图形性能测试 | |
| test_interaction.exe | 44 | ✅ | 交互测试 | |
| test_it_clipper.exe | 25 | ✅ | 裁剪器集成测试 | |
| test_layer_manager.exe | 25 | ✅ | 图层管理器测试 | |
| test_location_display_handler.exe | 35 | ✅ | 位置显示处理器测试 | |
| test_pan_zoom_handler.exe | 43 | ✅ | 平移缩放处理器测试 | |
| test_selection_handler.exe | 38 | ✅ | 选择处理器测试 | |

### navi模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| ogc_navi_tests.exe | 281 | ✅ | 航线规划、导航服务测试 | |

### cycle/chart_c_api模块

| 测试文件 | 用例数 | 状态 | 测试描述 | 备注说明 |
|----------|--------|------|----------|----------|
| sdk_c_api_tests.exe | 136 | ✅ | C API跨语言接口测试 | |
