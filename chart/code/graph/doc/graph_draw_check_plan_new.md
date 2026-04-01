# Graph 与 Draw 模块重构任务计划 (新方案)

## 概述

基于 [graph_draw_check.md](./graph_draw_check.md) 的分析结果，按照以下原则进行重构：

1. **draw 模块为基准**: DrawDevice 及其派生类、DrawEngine、DrawContext 以 draw 模块实现为基准
2. **graph 模块依赖 draw**: graph 模块不再重复实现设备/引擎/驱动类，直接依赖 draw 模块
3. **基础类型保留在 draw**: 通用类、结构体、函数整理到 draw 模块，进行类型合并和拆分

### 职责划分

| 模块 | 职责 |
|------|------|
| **draw** | 底层绘制引擎、设备抽象、平台适配、GPU 加速、基础类型 |
| **graph** | GIS 渲染逻辑、符号化、样式解析、图层管理、标注 |

### 依赖关系

```
graph 模块 (上层 - GIS 渲染逻辑)
    ↓ 依赖
draw 模块 (底层 - 绘制引擎 + 基础类型)
```

### 类型整理规则

1. **结构体拆分**: 每个独立类型拆分到单独的头文件和源文件（如 color.h/color.cpp）
2. **类转换**: 如果结构体中方法函数超过 5 个，需要转换为类
3. **归类整理**: 可划归一类的结构体可整理到一个头文件中

---

## 一、draw 模块工作

### 1.1 类型合并与拆分

#### 1.1.1 需要合并的类型

| 类型 | 原位置 | 目标文件 | 说明 |
|------|--------|----------|------|
| **Color** | draw: draw_types.h<br>graph: color.h | color.h / color.cpp | 合并两个版本，方法>5，转为类 |
| **Point** | draw: draw_types.h | geometry_types.h | 与 Size/Rect 归类 |
| **Size** | draw: draw_types.h | geometry_types.h | 与 Point/Rect 归类 |
| **Rect** | draw: draw_types.h | geometry_types.h | 与 Point/Size 归类 |
| **TransformMatrix** | draw: transform_matrix.h<br>graph: transform_matrix.h | transform_matrix.h | 合并两个版本 |
| **DeviceType** | draw: device_type.h<br>graph: device_type.h | device_type.h | 合并枚举值 |
| **EngineType** | draw: engine_type.h<br>graph: engine_type.h | engine_type.h | 合并枚举值 |
| **DrawResult** | draw: draw_result.h<br>graph: draw_result.h | draw_result.h | 合并枚举值 |
| **DeviceState** | draw: draw_device.h | device_state.h | 提取为独立文件 |
| **Font** | draw: font.h<br>graph: font.h | font.h / font.cpp | 合并两个版本，转为类 |
| **DrawStyle** | draw: draw_style.h<br>graph: draw_style.h | draw_style.h / draw_style.cpp | 合并两个版本，转为类 |
| **Image** | draw: draw_types.h | image.h | 拆分为独立文件 |
| **TextMetrics** | draw: draw_types.h | text_metrics.h | 拆分为独立文件 |
| **Region** | draw: draw_types.h | region.h | 拆分为独立文件 |

#### 1.1.2 类型整理后的文件结构

```
code/draw/include/ogc/draw/
├── color.h                    # Color 类
├── geometry_types.h           # Point, Size, Rect 结构体
├── transform_matrix.h         # TransformMatrix 类
├── device_type.h              # DeviceType 枚举
├── engine_type.h              # EngineType 枚举
├── draw_result.h              # DrawResult 枚举
├── device_state.h             # DeviceState 枚举
├── font.h                     # Font 类
├── draw_style.h               # DrawStyle 类
├── image.h                    # Image 结构体
├── text_metrics.h             # TextMetrics 结构体
├── region.h                   # Region 结构体
├── draw_types.h               # 类型别名和引用 (保留向后兼容)
├── draw_device.h              # DrawDevice 基类
├── draw_engine.h              # DrawEngine 基类
├── draw_context.h             # DrawContext 类
└── ... (其他设备/引擎文件)

code/draw/src/
├── color.cpp                  # Color 实现
├── transform_matrix.cpp       # TransformMatrix 实现
├── font.cpp                   # Font 实现
├── draw_style.cpp             # DrawStyle 实现
├── image.cpp                  # Image 实现
├── text_metrics.cpp           # TextMetrics 实现
├── region.cpp                 # Region 实现
└── ... (其他实现文件)
```

### 1.2 draw 模块任务列表

| Task ID | Task Name | Priority | Effort | Description |
|---------|-----------|----------|--------|-------------|
| D-1 | 合并 Color 类型 | P0 | 3h | 合并两个版本，拆分文件，转为类 |
| D-2 | 合并几何类型 | P0 | 2h | Point/Size/Rect 整理到 geometry_types.h |
| D-3 | 合并 TransformMatrix | P0 | 4h | 合并两个版本，确保兼容 |
| D-4 | 合并 DeviceType 枚举 | P0 | 1h | 合并枚举值，添加 kWebGL |
| D-5 | 合并 EngineType 枚举 | P0 | 1h | 合并枚举值 |
| D-6 | 合并 DrawResult 枚举 | P0 | 1h | 合并所有错误码 |
| D-7 | 提取 DeviceState 枚举 | P1 | 1h | 从 draw_device.h 提取 |
| D-8 | 合并 Font 类 | P1 | 3h | 合并两个版本 |
| D-9 | 合并 DrawStyle 类 | P1 | 4h | 合并 StrokeStyle/FillStyle 和 Pen/Brush |
| D-10 | 拆分 Image/TextMetrics/Region | P1 | 2h | 拆分为独立文件 |
| D-11 | 更新 draw_types.h | P0 | 1h | 更新引用，保持向后兼容 |
| D-12 | 更新 DrawDevice 接口 | P1 | 2h | 适配新类型 |
| D-13 | 更新 DrawEngine 接口 | P1 | 2h | 适配新类型 |
| D-14 | 更新 DrawContext 接口 | P1 | 1h | 适配新类型 |
| D-15 | 更新所有引擎实现 | P1 | 4h | 适配新接口 |
| D-16 | 更新所有设备实现 | P1 | 4h | 适配新接口 |
| D-17 | 更新 draw 模块测试 | P1 | 4h | 适配新结构 |

**draw 模块总工时**: 约 40 小时

---

## 二、graph 模块工作

### 2.1 需要删除的文件

#### 2.1.1 设备类 (删除，使用 draw 模块)

| 文件 | 操作 | 替代方案 |
|------|------|---------|
| `include/ogc/draw/draw_device.h` | ❌ 删除 | 使用 `ogc/draw/draw_device.h` |
| `include/ogc/draw/tile_device.h` | ❌ 删除 | 使用 `ogc/draw/tile_device.h` |
| `include/ogc/draw/display_device.h` | ❌ 删除 | 使用 `ogc/draw/display_device.h` |
| `include/ogc/draw/raster_image_device.h` | ❌ 删除 | 使用 `ogc/draw/raster_image_device.h` |
| `include/ogc/draw/pdf_device.h` | ❌ 删除 | 使用 `ogc/draw/pdf_device.h` |
| `src/draw_device.cpp` | ❌ 删除 | - |
| `src/tile_device.cpp` | ❌ 删除 | - |
| `src/display_device.cpp` | ❌ 删除 | - |
| `src/raster_image_device.cpp` | ❌ 删除 | - |
| `src/pdf_device.cpp` | ❌ 删除 | - |

#### 2.1.2 引擎类 (删除，使用 draw 模块)

| 文件 | 操作 | 替代方案 |
|------|------|---------|
| `include/ogc/draw/draw_engine.h` | ❌ 删除 | 使用 `ogc/draw/draw_engine.h` |
| `include/ogc/draw/draw_context.h` | ❌ 删除 | 使用 `ogc/draw/draw_context.h` |
| `src/draw_engine.cpp` | ❌ 删除 | - |
| `src/draw_context.cpp` | ❌ 删除 | - |

#### 2.1.3 驱动类 (删除，不再使用)

| 文件 | 操作 | 说明 |
|------|------|------|
| `include/ogc/draw/draw_driver.h` | ❌ 删除 | 驱动抽象类，不再使用 |
| `include/ogc/draw/driver_manager.h` | ❌ 删除 | 驱动管理器，不再使用 |
| `src/draw_driver.cpp` | ❌ 删除 | - |
| `src/driver_manager.cpp` | ❌ 删除 | - |
| `tests/test_driver_manager.cpp` | ❌ 删除 | - |

#### 2.1.4 基础类型 (删除，使用 draw 模块)

| 文件 | 操作 | 替代方案 |
|------|------|---------|
| `include/ogc/draw/transform_matrix.h` | ❌ 删除 | 使用 `ogc/draw/transform_matrix.h` |
| `include/ogc/draw/color.h` | ❌ 删除 | 使用 `ogc/draw/color.h` |
| `include/ogc/draw/draw_style.h` | ❌ 删除 | 使用 `ogc/draw/draw_style.h` |
| `include/ogc/draw/font.h` | ❌ 删除 | 使用 `ogc/draw/font.h` |
| `include/ogc/draw/device_type.h` | ❌ 删除 | 使用 `ogc/draw/device_type.h` |
| `include/ogc/draw/engine_type.h` | ❌ 删除 | 使用 `ogc/draw/engine_type.h` |
| `include/ogc/draw/draw_result.h` | ❌ 删除 | 使用 `ogc/draw/draw_result.h` |
| `include/ogc/draw/types.h` | ❌ 删除 | 使用 `ogc/draw/draw_types.h` |
| `src/transform_matrix.cpp` | ❌ 删除 | - |
| `src/color.cpp` | ❌ 删除 | - |
| `src/draw_style.cpp` | ❌ 删除 | - |
| `src/font.cpp` | ❌ 删除 | - |

#### 2.1.5 缓存类 (保留或适配)

| 文件 | 操作 | 说明 |
|------|------|------|
| `include/ogc/draw/tile_cache.h` | ⚠️ 评估 | 与 draw 的 TextureCache 功能重叠 |
| `include/ogc/draw/memory_tile_cache.h` | ⚠️ 评估 | 可保留，GIS 特有需求 |
| `include/ogc/draw/disk_tile_cache.h` | ⚠️ 评估 | 可保留，GIS 特有需求 |
| `include/ogc/draw/multi_level_tile_cache.h` | ✅ 保留 | graph 独有功能 |

### 2.2 graph 模块保留内容 (GIS 特有)

#### 2.2.1 符号化系统 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **Symbolizer** | `symbolizer.h` | 符号化器基类 |
| **PointSymbolizer** | `point_symbolizer.h` | 点符号化器 |
| **LineSymbolizer** | `line_symbolizer.h` | 线符号化器 |
| **PolygonSymbolizer** | `polygon_symbolizer.h` | 多边形符号化器 |
| **TextSymbolizer** | `text_symbolizer.h` | 文本符号化器 |
| **RasterSymbolizer** | `raster_symbolizer.h` | 栅格符号化器 |
| **CompositeSymbolizer** | `composite_symbolizer.h` | 组合符号化器 |
| **IconSymbolizer** | `icon_symbolizer.h` | 图标符号化器 |

#### 2.2.2 规则引擎 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **SymbolizerRule** | `symbolizer_rule.h` | 符号化规则 |
| **RuleEngine** | `rule_engine.h` | 规则引擎 |

#### 2.2.3 过滤器系统 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **Filter** | `filter.h` | 过滤器基类 |
| **ComparisonFilter** | `comparison_filter.h` | 比较过滤器 |
| **LogicalFilter** | `logical_filter.h` | 逻辑过滤器 |
| **SpatialFilter** | `spatial_filter.h` | 空间过滤器 |

#### 2.2.4 样式解析器 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **SldParser** | `sld_parser.h` | SLD 解析器 |
| **MapboxStyleParser** | `mapbox_style_parser.h` | Mapbox Style 解析器 |

#### 2.2.5 标注引擎 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **LabelEngine** | `label_engine.h` | 标注引擎 |
| **LabelPlacement** | `label_placement.h` | 标注放置算法 |
| **LabelConflictResolver** | `label_conflict.h` | 标注冲突解决 |

#### 2.2.6 图层管理 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **LayerManager** | `layer_manager.h` | 图层管理器 |
| **ILayerRenderer** | `layer_manager.h` | 图层渲染接口 |

#### 2.2.7 瓦片渲染 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **TileRenderer** | `tile_renderer.h` | 瓦片渲染器 |
| **TileKey** | `tile_key.h` | 瓦片键 |

#### 2.2.8 LOD 系统 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **LODLevel** | `lod.h` | LOD 级别定义 |
| **LODStrategy** | `lod.h` | LOD 策略接口 |
| **LODManager** | `lod_manager.h` | LOD 管理器 |

#### 2.2.9 坐标转换 (保留)

| 类 | 文件 | 说明 |
|----|------|------|
| **CoordinateTransform** | `coordinate_transform.h` | 坐标转换 |
| **CoordinateTransformer** | `coordinate_transformer.h` | 坐标转换器 |
| **ProjTransformer** | `proj_transformer.h` | 投影转换 |
| **TransformManager** | `transform_manager.h` | 变换管理器 |

#### 2.2.10 其他保留类

| 类 | 文件 | 说明 |
|----|------|------|
| **DrawFacade** | `draw_facade.h` | 绘制门面类 |
| **Clipper** | `clipper.h` | 裁剪器 |
| **ImageDraw** | `image_draw.h` | 图像绘制 |
| **PerformanceMetrics** | `performance_metrics.h` | 性能指标 |
| **PerformanceMonitor** | `performance_monitor.h` | 性能监控 |
| **RenderQueue** | `render_queue.h` | 渲染队列 |
| **RenderTask** | `render_task.h` | 渲染任务 |
| **BasicRenderTask** | `basic_render_task.h` | 基础渲染任务 |
| **AsyncRenderer** | `async_renderer.h` | 异步渲染器 |
| **ThreadSafe** | `thread_safe.h` | 线程安全工具 |
| **Log** | `log.h` | 日志系统 |

### 2.3 graph 模块任务列表

| Task ID | Task Name | Priority | Effort | Description |
|---------|-----------|----------|--------|-------------|
| G-1 | 删除设备类文件 | P0 | 2h | 删除 10 个重复文件 |
| G-2 | 删除引擎类文件 | P0 | 1h | 删除 4 个重复文件 |
| G-3 | 删除驱动类文件 | P0 | 1h | 删除 5 个驱动相关文件 |
| G-4 | 删除基础类型文件 | P0 | 1h | 删除 13 个重复文件 |
| G-5 | 更新头文件引用 | P0 | 4h | 更新所有 #include |
| G-6 | 更新 CMakeLists.txt | P0 | 2h | 添加 draw 依赖 |
| G-7 | 更新符号化系统 | P1 | 4h | 适配新接口 |
| G-8 | 更新规则引擎 | P1 | 2h | 适配新接口 |
| G-9 | 更新过滤器系统 | P1 | 2h | 适配新接口 |
| G-10 | 更新标注引擎 | P1 | 4h | 适配新接口 |
| G-11 | 更新图层管理 | P1 | 2h | 适配新接口 |
| G-12 | 更新瓦片渲染 | P1 | 4h | 适配新接口 |
| G-13 | 更新 LOD 系统 | P2 | 2h | 适配新接口 |
| G-14 | 更新坐标转换 | P2 | 2h | 适配新接口 |
| G-15 | 更新 DrawFacade | P1 | 2h | 适配新接口 |
| G-16 | 更新 graph 模块测试 | P1 | 8h | 适配新结构 |

**graph 模块总工时**: 约 43 小时

---

## 三、任务摘要

### 3.1 总任务列表

| Task ID | Task Name | Module | Priority | Effort | Dependencies |
|---------|-----------|--------|----------|--------|--------------|
| D-1 | 合并 Color 类型 | draw | P0 | 3h | - |
| D-2 | 合并几何类型 | draw | P0 | 2h | - |
| D-3 | 合并 TransformMatrix | draw | P0 | 4h | - |
| D-4 | 合并 DeviceType 枚举 | draw | P0 | 1h | - |
| D-5 | 合并 EngineType 枚举 | draw | P0 | 1h | - |
| D-6 | 合并 DrawResult 枚举 | draw | P0 | 1h | - |
| D-7 | 提取 DeviceState 枚举 | draw | P1 | 1h | - |
| D-8 | 合并 Font 类 | draw | P1 | 3h | D-1 |
| D-9 | 合并 DrawStyle 类 | draw | P1 | 4h | D-1 |
| D-10 | 拆分 Image/TextMetrics/Region | draw | P1 | 2h | - |
| D-11 | 更新 draw_types.h | draw | P0 | 1h | D-1~10 |
| D-12 | 更新 DrawDevice 接口 | draw | P1 | 2h | D-11 |
| D-13 | 更新 DrawEngine 接口 | draw | P1 | 2h | D-11 |
| D-14 | 更新 DrawContext 接口 | draw | P1 | 1h | D-12, D-13 |
| D-15 | 更新所有引擎实现 | draw | P1 | 4h | D-12~14 |
| D-16 | 更新所有设备实现 | draw | P1 | 4h | D-12~14 |
| D-17 | 更新 draw 模块测试 | draw | P1 | 4h | D-15, D-16 |
| G-1 | 删除设备类文件 | graph | P0 | 2h | D-12, D-16 |
| G-2 | 删除引擎类文件 | graph | P0 | 1h | D-13, D-15 |
| G-3 | 删除驱动类文件 | graph | P0 | 1h | - |
| G-4 | 删除基础类型文件 | graph | P0 | 1h | D-11 |
| G-5 | 更新头文件引用 | graph | P0 | 4h | G-1~4 |
| G-6 | 更新 CMakeLists.txt | graph | P0 | 2h | G-5 |
| G-7 | 更新符号化系统 | graph | P1 | 4h | G-6 |
| G-8 | 更新规则引擎 | graph | P1 | 2h | G-7 |
| G-9 | 更新过滤器系统 | graph | P1 | 2h | G-6 |
| G-10 | 更新标注引擎 | graph | P1 | 4h | G-6 |
| G-11 | 更新图层管理 | graph | P1 | 2h | G-6 |
| G-12 | 更新瓦片渲染 | graph | P1 | 4h | G-6 |
| G-13 | 更新 LOD 系统 | graph | P2 | 2h | G-6 |
| G-14 | 更新坐标转换 | graph | P2 | 2h | D-3 |
| G-15 | 更新 DrawFacade | graph | P1 | 2h | G-6 |
| G-16 | 更新 graph 模块测试 | graph | P1 | 8h | G-7~15 |

### 3.2 工时统计

| 模块 | 任务数 | 总工时 |
|------|--------|--------|
| **draw** | 17 | 40h |
| **graph** | 16 | 43h |
| **总计** | 33 | 83h |

---

## 四、里程碑规划

### M1: draw 模块类型整理 (Week 1)

**目标**: 合并和拆分基础类型，确保向后兼容

**关键交付物**:
- 合并后的类型文件
- 更新后的 draw_types.h
- 单元测试

| Task | Priority | Effort | Status |
|------|----------|--------|--------|
| D-1: 合并 Color 类型 | P0 | 3h | 📋 Todo |
| D-2: 合并几何类型 | P0 | 2h | 📋 Todo |
| D-3: 合并 TransformMatrix | P0 | 4h | 📋 Todo |
| D-4: 合并 DeviceType 枚举 | P0 | 1h | 📋 Todo |
| D-5: 合并 EngineType 枚举 | P0 | 1h | 📋 Todo |
| D-6: 合并 DrawResult 枚举 | P0 | 1h | 📋 Todo |
| D-7: 提取 DeviceState 枚举 | P1 | 1h | 📋 Todo |
| D-8: 合并 Font 类 | P1 | 3h | 📋 Todo |
| D-9: 合并 DrawStyle 类 | P1 | 4h | 📋 Todo |
| D-10: 拆分 Image/TextMetrics/Region | P1 | 2h | 📋 Todo |
| D-11: 更新 draw_types.h | P0 | 1h | 📋 Todo |

### M2: draw 模块接口更新 (Week 2)

**目标**: 更新设备/引擎接口，适配新类型

**关键交付物**:
- 更新后的 DrawDevice/DrawEngine/DrawContext
- 所有引擎/设备实现适配完成
- 测试套件通过

| Task | Priority | Effort | Status |
|------|----------|--------|--------|
| D-12: 更新 DrawDevice 接口 | P1 | 2h | 📋 Todo |
| D-13: 更新 DrawEngine 接口 | P1 | 2h | 📋 Todo |
| D-14: 更新 DrawContext 接口 | P1 | 1h | 📋 Todo |
| D-15: 更新所有引擎实现 | P1 | 4h | 📋 Todo |
| D-16: 更新所有设备实现 | P1 | 4h | 📋 Todo |
| D-17: 更新 draw 模块测试 | P1 | 4h | 📋 Todo |

### M3: graph 模块重构 (Week 3)

**目标**: 删除重复代码，建立对 draw 的依赖

**关键交付物**:
- 清理后的 graph 动态库
- 所有 GIS 功能保持正常
- 测试套件通过

| Task | Priority | Effort | Status |
|------|----------|--------|--------|
| G-1: 删除设备类文件 | P0 | 2h | 📋 Todo |
| G-2: 删除引擎类文件 | P0 | 1h | 📋 Todo |
| G-3: 删除驱动类文件 | P0 | 1h | 📋 Todo |
| G-4: 删除基础类型文件 | P0 | 1h | 📋 Todo |
| G-5: 更新头文件引用 | P0 | 4h | 📋 Todo |
| G-6: 更新 CMakeLists.txt | P0 | 2h | 📋 Todo |
| G-7: 更新符号化系统 | P1 | 4h | 📋 Todo |
| G-8: 更新规则引擎 | P1 | 2h | 📋 Todo |
| G-9: 更新过滤器系统 | P1 | 2h | 📋 Todo |
| G-10: 更新标注引擎 | P1 | 4h | 📋 Todo |
| G-11: 更新图层管理 | P1 | 2h | 📋 Todo |
| G-12: 更新瓦片渲染 | P1 | 4h | 📋 Todo |
| G-13: 更新 LOD 系统 | P2 | 2h | 📋 Todo |
| G-14: 更新坐标转换 | P2 | 2h | 📋 Todo |
| G-15: 更新 DrawFacade | P1 | 2h | 📋 Todo |
| G-16: 更新测试 | P1 | 8h | 📋 Todo |

---

## 五、里程碑详情

### M1: draw 模块类型整理

#### 目标

合并 graph 和 draw 模块中的重复类型定义，拆分到独立文件，确保向后兼容。

#### 范围

- 合并 Color、TransformMatrix、Font、DrawStyle 等类型
- 拆分结构体到独立文件
- 更新 draw_types.h 保持向后兼容

#### 任务详情

##### D-1: 合并 Color 类型

**描述**: 合并 draw 和 graph 两个版本的 Color，拆分到独立文件，转为类。

**输入**:
- `code/draw/include/ogc/draw/draw_types.h` 中的 Color 定义
- `code/graph/include/ogc/draw/color.h` 中的 Color 定义

**输出**:
- `code/draw/include/ogc/draw/color.h`
- `code/draw/src/color.cpp`

**步骤**:
1. 分析两个版本的差异
2. 设计统一的 Color 类接口
3. 实现所有颜色操作方法
4. 添加预定义颜色常量
5. 创建单元测试

**验收标准**:
- [ ] Color 类编译通过
- [ ] 方法数量 > 5，已转为类
- [ ] 所有颜色操作功能正常
- [ ] 单元测试覆盖主要功能

---

##### D-2: 合并几何类型

**描述**: 将 Point、Size、Rect 整理到 geometry_types.h 文件。

**输入**:
- `code/draw/include/ogc/draw/draw_types.h` 中的 Point、Size、Rect 定义

**输出**:
- `code/draw/include/ogc/draw/geometry_types.h`

**步骤**:
1. 创建 geometry_types.h 文件
2. 移动 Point、Size、Rect 定义
3. 添加运算符重载
4. 创建单元测试

**验收标准**:
- [ ] 三个结构体编译通过
- [ ] 几何运算功能正常
- [ ] 单元测试覆盖主要功能

---

##### D-3: 合并 TransformMatrix 类

**描述**: 合并 draw 和 graph 两个版本的 TransformMatrix。

**输入**:
- `code/draw/include/ogc/draw/transform_matrix.h`
- `code/graph/include/ogc/draw/transform_matrix.h`

**输出**:
- `code/draw/include/ogc/draw/transform_matrix.h`
- `code/draw/src/transform_matrix.cpp`

**步骤**:
1. 分析两个版本的差异
2. 设计统一的 3x3 矩阵接口
3. 实现所有变换操作 (平移、缩放、旋转、倾斜)
4. 实现矩阵组合和逆矩阵
5. 创建单元测试

**验收标准**:
- [ ] 支持 2D 仿射变换
- [ ] 矩阵运算正确
- [ ] 与原有两个版本功能兼容
- [ ] 单元测试覆盖所有操作

---

##### D-4: 合并 DeviceType 枚举

**描述**: 合并 draw 和 graph 两个版本的 DeviceType 枚举。

**输入**:
- `code/draw/include/ogc/draw/device_type.h`
- `code/graph/include/ogc/draw/device_type.h`

**输出**:
- `code/draw/include/ogc/draw/device_type.h`

**步骤**:
1. 收集两个版本的所有枚举值
2. 统一命名风格 (kCamelCase)
3. 添加缺失的设备类型 (kWebGL)
4. 添加 ToString/FromString 方法

**验收标准**:
- [ ] 包含所有设备类型
- [ ] 命名风格一致
- [ ] 提供字符串转换方法

---

##### D-5: 合并 EngineType 枚举

**描述**: 合并 draw 和 graph 两个版本的 EngineType 枚举。

**输入**:
- `code/draw/include/ogc/draw/engine_type.h`
- `code/graph/include/ogc/draw/engine_type.h`

**输出**:
- `code/draw/include/ogc/draw/engine_type.h`

**步骤**:
1. 收集两个版本的所有枚举值
2. 统一命名风格
3. 添加 ToString/FromString 方法

**验收标准**:
- [ ] 包含所有引擎类型
- [ ] 命名风格一致
- [ ] 提供字符串转换方法

---

##### D-6: 合并 DrawResult 枚举

**描述**: 合并 draw 和 graph 两个版本的 DrawResult 枚举。

**输入**:
- `code/draw/include/ogc/draw/draw_result.h`
- `code/graph/include/ogc/draw/draw_result.h`

**输出**:
- `code/draw/include/ogc/draw/draw_result.h`

**步骤**:
1. 收集两个版本的所有枚举值
2. 统一错误码定义
3. 添加 ToString 方法
4. 添加 IsSuccess/IsError 辅助方法

**验收标准**:
- [ ] 包含所有结果状态
- [ ] 提供辅助判断方法
- [ ] 提供字符串转换方法

---

##### D-7: 提取 DeviceState 枚举

**描述**: 从 draw_device.h 提取 DeviceState 枚举到独立文件。

**输入**:
- `code/draw/include/ogc/draw/draw_device.h` 中的 DeviceState 定义

**输出**:
- `code/draw/include/ogc/draw/device_state.h`

**步骤**:
1. 提取枚举定义
2. 创建独立头文件
3. 添加 ToString 方法

**验收标准**:
- [ ] 包含所有设备状态
- [ ] 提供字符串转换方法

---

##### D-8: 合并 Font 类

**描述**: 合并 draw 和 graph 两个版本的 Font 类。

**输入**:
- `code/draw/include/ogc/draw/font.h`
- `code/graph/include/ogc/draw/font.h`

**输出**:
- `code/draw/include/ogc/draw/font.h`
- `code/draw/src/font.cpp`

**步骤**:
1. 分析两个版本的差异
2. 设计统一的 Font 接口
3. 实现字体属性 (family, size, weight, style)
4. 创建单元测试

**验收标准**:
- [ ] 支持常用字体属性
- [ ] 提供字体比较方法
- [ ] 单元测试覆盖主要功能

---

##### D-9: 合并 DrawStyle 类

**描述**: 合并 draw 和 graph 两个版本的绘制样式类。

**输入**:
- `code/draw/include/ogc/draw/draw_style.h` (Pen, Brush, StrokeStyle, FillStyle)
- `code/graph/include/ogc/draw/draw_style.h`

**输出**:
- `code/draw/include/ogc/draw/draw_style.h`
- `code/draw/src/draw_style.cpp`

**步骤**:
1. 分析两个版本的差异
2. 设计统一的 DrawStyle 接口
3. 实现 StrokeStyle 和 FillStyle
4. 兼容 Pen/Brush 概念
5. 创建单元测试

**验收标准**:
- [ ] 支持描边和填充样式
- [ ] 兼容原有 API
- [ ] 单元测试覆盖主要功能

---

##### D-10: 拆分 Image/TextMetrics/Region

**描述**: 从 draw_types.h 拆分 Image、TextMetrics、Region 到独立文件。

**输入**:
- `code/draw/include/ogc/draw/draw_types.h` 中的相关定义

**输出**:
- `code/draw/include/ogc/draw/image.h`
- `code/draw/include/ogc/draw/text_metrics.h`
- `code/draw/include/ogc/draw/region.h`
- 对应的 cpp 文件

**步骤**:
1. 提取 Image 结构体
2. 提取 TextMetrics 结构体
3. 提取 Region 结构体
4. 创建单元测试

**验收标准**:
- [ ] 三个结构体编译通过
- [ ] 功能完整

---

##### D-11: 更新 draw_types.h

**描述**: 更新 draw_types.h，引用新拆分的类型文件，保持向后兼容。

**输入**:
- `code/draw/include/ogc/draw/draw_types.h`

**输出**:
- 更新后的 `draw_types.h`

**步骤**:
1. 添加新类型文件的引用
2. 使用 using 引入类型别名
3. 保留向后兼容的类型定义
4. 更新相关文档

**验收标准**:
- [ ] 编译通过
- [ ] 向后兼容
- [ ] 无警告

---

### M2: draw 模块接口更新

#### 目标

更新 draw 模块的设备/引擎接口，适配新的类型定义。

#### 范围

- 更新 DrawDevice、DrawEngine、DrawContext 接口
- 更新所有引擎和设备实现
- 更新测试用例

#### 任务详情

##### D-12: 更新 DrawDevice 接口

**描述**: 确保 DrawDevice 接口完整，适配新类型。

**输入**:
- `code/draw/include/ogc/draw/draw_device.h`

**输出**:
- 更新后的 DrawDevice 接口

**步骤**:
1. 更新类型引用
2. 确保接口方法完整
3. 添加缺失的虚函数
4. 更新文档注释

**验收标准**:
- [ ] 接口完整
- [ ] 文档完整
- [ ] 编译通过

---

##### D-13: 更新 DrawEngine 接口

**描述**: 确保 DrawEngine 接口完整，适配新类型。

**输入**:
- `code/draw/include/ogc/draw/draw_engine.h`

**输出**:
- 更新后的 DrawEngine 接口

**步骤**:
1. 更新类型引用
2. 确保接口方法完整
3. 添加设备创建/销毁方法
4. 更新文档注释

**验收标准**:
- [ ] 接口完整
- [ ] 文档完整
- [ ] 编译通过

---

##### D-14: 更新 DrawContext 接口

**描述**: 确保 DrawContext 接口完整，适配新类型。

**输入**:
- `code/draw/include/ogc/draw/draw_context.h`

**输出**:
- 更新后的 DrawContext 接口

**步骤**:
1. 更新类型引用
2. 确保绑定设备的方法完整
3. 更新文档注释

**验收标准**:
- [ ] 接口完整
- [ ] 文档完整
- [ ] 编译通过

---

##### D-15: 更新所有引擎实现

**描述**: 更新所有引擎实现类，适配新接口。

**输入**:
- `code/draw/src/` 中的所有引擎实现

**输出**:
- 更新后的引擎实现

**步骤**:
1. 更新 Simple2DEngine
2. 更新 TileBasedEngine
3. 更新 VectorEngine
4. 更新平台引擎 (GDIPlus, Direct2D, Cairo, Metal, WebGL)
5. 更新格式引擎 (Pdf, Svg)
6. 更新 QtEngine

**验收标准**:
- [ ] 所有引擎编译通过
- [ ] 功能测试通过

---

##### D-16: 更新所有设备实现

**描述**: 更新所有设备实现类，适配新接口。

**输入**:
- `code/draw/src/` 中的所有设备实现

**输出**:
- 更新后的设备实现

**步骤**:
1. 更新 TileDevice
2. 更新 DisplayDevice
3. 更新 RasterImageDevice
4. 更新 PdfDevice
5. 更新 SvgDevice
6. 更新 WebGLDevice
7. 更新 QtDisplayDevice

**验收标准**:
- [ ] 所有设备编译通过
- [ ] 功能测试通过

---

##### D-17: 更新 draw 模块测试

**描述**: 更新 draw 模块的测试用例，适配新结构。

**输入**:
- `code/draw/tests/` 中的测试文件

**输出**:
- 更新后的测试文件

**步骤**:
1. 更新测试中的类型引用
2. 添加新接口的测试
3. 运行所有测试确保通过

**验收标准**:
- [ ] 所有测试通过
- [ ] 测试覆盖率不降低

---

### M3: graph 模块重构

#### 目标

删除 graph 模块中的重复代码，建立对 draw 模块的依赖关系。

#### 范围

- 删除设备、引擎、驱动、基础类型文件
- 更新头文件引用
- 更新 GIS 功能适配新接口

#### 任务详情

##### G-1: 删除设备类文件

**描述**: 删除 graph 模块中重复的设备类文件。

**输入**:
- `code/graph/include/ogc/draw/draw_device.h`
- `code/graph/include/ogc/draw/tile_device.h`
- `code/graph/include/ogc/draw/display_device.h`
- `code/graph/include/ogc/draw/raster_image_device.h`
- `code/graph/include/ogc/draw/pdf_device.h`
- `code/graph/src/draw_device.cpp`
- `code/graph/src/tile_device.cpp`
- `code/graph/src/display_device.cpp`
- `code/graph/src/raster_image_device.cpp`
- `code/graph/src/pdf_device.cpp`

**输出**:
- 文件已删除

**步骤**:
1. 备份原有文件
2. 删除头文件
3. 删除源文件
4. 更新 CMakeLists.txt 移除文件引用

**验收标准**:
- [ ] 文件已删除
- [ ] CMakeLists.txt 已更新

---

##### G-2: 删除引擎类文件

**描述**: 删除 graph 模块中重复的引擎类文件。

**输入**:
- `code/graph/include/ogc/draw/draw_engine.h`
- `code/graph/include/ogc/draw/draw_context.h`
- `code/graph/src/draw_engine.cpp`
- `code/graph/src/draw_context.cpp`

**输出**:
- 文件已删除

**步骤**:
1. 备份原有文件
2. 删除头文件
3. 删除源文件
4. 更新 CMakeLists.txt 移除文件引用

**验收标准**:
- [ ] 文件已删除
- [ ] CMakeLists.txt 已更新

---

##### G-3: 删除驱动类文件

**描述**: 删除 graph 模块中的驱动类文件，不再使用驱动模式。

**输入**:
- `code/graph/include/ogc/draw/draw_driver.h`
- `code/graph/include/ogc/draw/driver_manager.h`
- `code/graph/src/draw_driver.cpp`
- `code/graph/src/driver_manager.cpp`
- `code/graph/tests/test_driver_manager.cpp`

**输出**:
- 文件已删除

**步骤**:
1. 备份原有文件
2. 删除头文件
3. 删除源文件
4. 删除测试文件
5. 更新 CMakeLists.txt 移除文件引用

**验收标准**:
- [ ] 文件已删除
- [ ] CMakeLists.txt 已更新
- [ ] 测试列表已更新

---

##### G-4: 删除基础类型文件

**描述**: 删除 graph 模块中重复的基础类型文件。

**输入**:
- `code/graph/include/ogc/draw/transform_matrix.h`
- `code/graph/include/ogc/draw/color.h`
- `code/graph/include/ogc/draw/draw_style.h`
- `code/graph/include/ogc/draw/font.h`
- `code/graph/include/ogc/draw/device_type.h`
- `code/graph/include/ogc/draw/engine_type.h`
- `code/graph/include/ogc/draw/draw_result.h`
- `code/graph/include/ogc/draw/types.h`
- 对应的 src 文件

**输出**:
- 文件已删除

**步骤**:
1. 备份原有文件
2. 删除所有头文件
3. 删除所有源文件
4. 更新 CMakeLists.txt 移除文件引用

**验收标准**:
- [ ] 文件已删除
- [ ] CMakeLists.txt 已更新

---

##### G-5: 更新头文件引用

**描述**: 更新 graph 模块中所有文件的 #include 引用。

**输入**:
- `code/graph/` 中的所有源文件

**输出**:
- 更新后的源文件

**步骤**:
1. 搜索所有 `#include "ogc/draw/draw_device.h"` 替换为 `#include <ogc/draw/draw_device.h>`
2. 搜索所有 `#include "ogc/draw/draw_engine.h"` 替换为 `#include <ogc/draw/draw_engine.h>`
3. 搜索所有 `#include "ogc/draw/draw_context.h"` 替换为 `#include <ogc/draw/draw_context.h>`
4. 更新基础类型引用
5. 更新命名空间引用

**验收标准**:
- [ ] 所有引用更新完成
- [ ] 编译通过

---

##### G-6: 更新 CMakeLists.txt

**描述**: 更新 graph 模块的 CMake 配置，添加 draw 依赖。

**输入**:
- `code/graph/CMakeLists.txt`

**输出**:
- 更新后的 CMakeLists.txt

**步骤**:
1. 添加 draw 为依赖项
2. 更新包含目录
3. 更新链接库
4. 移除已删除文件的引用

**验收标准**:
- [ ] 依赖关系正确
- [ ] 编译通过

---

##### G-7: 更新符号化系统

**描述**: 更新符号化系统，适配新的设备/引擎接口。

**输入**:
- `code/graph/src/symbolizer.cpp`
- `code/graph/src/point_symbolizer.cpp`
- `code/graph/src/line_symbolizer.cpp`
- `code/graph/src/polygon_symbolizer.cpp`
- `code/graph/src/text_symbolizer.cpp`
- `code/graph/src/raster_symbolizer.cpp`
- `code/graph/src/composite_symbolizer.cpp`
- `code/graph/src/icon_symbolizer.cpp`

**输出**:
- 更新后的符号化器实现

**步骤**:
1. 更新 DrawDevice 引用
2. 更新 DrawContext 引用
3. 更新 DrawStyle 引用
4. 确保绘制功能正常

**验收标准**:
- [ ] 所有符号化器编译通过
- [ ] 功能测试通过

---

##### G-8: 更新规则引擎

**描述**: 更新规则引擎，适配新接口。

**输入**:
- `code/graph/src/symbolizer_rule.cpp`
- `code/graph/src/rule_engine.cpp`

**输出**:
- 更新后的规则引擎实现

**步骤**:
1. 更新类型引用
2. 确保规则匹配功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-9: 更新过滤器系统

**描述**: 更新过滤器系统，适配新接口。

**输入**:
- `code/graph/src/filter.cpp`
- `code/graph/src/comparison_filter.cpp`
- `code/graph/src/logical_filter.cpp`
- `code/graph/src/spatial_filter.cpp`

**输出**:
- 更新后的过滤器实现

**步骤**:
1. 更新类型引用
2. 确保过滤功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-10: 更新标注引擎

**描述**: 更新标注引擎，适配新的设备/引擎接口。

**输入**:
- `code/graph/src/label_engine.cpp`
- `code/graph/src/label_placement.cpp`
- `code/graph/src/label_conflict.cpp`

**输出**:
- 更新后的标注引擎实现

**步骤**:
1. 更新 DrawDevice 引用
2. 更新 DrawContext 引用
3. 更新 TextSymbolizer 引用
4. 确保标注功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-11: 更新图层管理

**描述**: 更新图层管理，适配新接口。

**输入**:
- `code/graph/src/layer_manager.cpp`

**输出**:
- 更新后的图层管理实现

**步骤**:
1. 更新类型引用
2. 确保图层管理功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-12: 更新瓦片渲染

**描述**: 更新瓦片渲染器，适配新的设备/引擎接口。

**输入**:
- `code/graph/src/tile_renderer.cpp`

**输出**:
- 更新后的瓦片渲染器实现

**步骤**:
1. 更新 TileDevice 引用
2. 更新 DrawEngine 引用
3. 更新 DrawContext 引用
4. 确保瓦片渲染功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-13: 更新 LOD 系统

**描述**: 更新 LOD 系统，适配新接口。

**输入**:
- `code/graph/src/lod.cpp`
- `code/graph/src/lod_manager.cpp`

**输出**:
- 更新后的 LOD 系统实现

**步骤**:
1. 更新类型引用
2. 确保 LOD 功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-14: 更新坐标转换

**描述**: 更新坐标转换系统，适配 draw 的 TransformMatrix。

**输入**:
- `code/graph/src/coordinate_transform.cpp`
- `code/graph/src/coordinate_transformer.cpp`
- `code/graph/src/proj_transformer.cpp`
- `code/graph/src/transform_manager.cpp`

**输出**:
- 更新后的坐标转换实现

**步骤**:
1. 更新 TransformMatrix 引用
2. 确保坐标转换功能正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-15: 更新 DrawFacade

**描述**: 更新 DrawFacade 门面类，适配新接口。

**输入**:
- `code/graph/src/draw_facade.cpp`

**输出**:
- 更新后的 DrawFacade 实现

**步骤**:
1. 更新 DrawDevice 引用
2. 更新 DrawEngine 引用
3. 更新 DrawContext 引用
4. 确保门面接口正常

**验收标准**:
- [ ] 编译通过
- [ ] 功能测试通过

---

##### G-16: 更新 graph 模块测试

**描述**: 更新 graph 模块的测试用例，适配新结构。

**输入**:
- `code/graph/tests/` 中的测试文件

**输出**:
- 更新后的测试文件

**步骤**:
1. 更新测试中的类型引用
2. 移除已删除类的测试
3. 添加新接口的测试
4. 运行所有测试确保通过

**验收标准**:
- [ ] 所有测试通过
- [ ] 测试覆盖率不降低

---

## 六、依赖关系图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           依赖关系图                                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  M1: draw 模块类型整理                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ [D-1: Color] ──┬── [D-8: Font]                                  │   │
│  │                └── [D-9: DrawStyle]                             │   │
│  │ [D-2: 几何类型]                                                 │   │
│  │ [D-3: TransformMatrix]                                          │   │
│  │ [D-4: DeviceType]                                               │   │
│  │ [D-5: EngineType]                                               │   │
│  │ [D-6: DrawResult]                                               │   │
│  │ [D-7: DeviceState]                                              │   │
│  │ [D-10: Image/TextMetrics/Region]                                │   │
│  │ [D-11: draw_types.h] ← D-1~10                                   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  M2: draw 模块接口更新                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ [D-12: DrawDevice] ──┬── [D-15: 引擎实现]                        │   │
│  │ [D-13: DrawEngine] ──┤                                          │   │
│  │ [D-14: DrawContext] ┘                                           │   │
│  │         │                                                        │   │
│  │         ▼                                                        │   │
│  │ [D-16: 设备实现] ── [D-17: 测试]                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  M3: graph 模块重构                                                     │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ [G-1~G-4: 删除文件] ← D-12~17                                    │   │
│  │         │                                                        │   │
│  │         ▼                                                        │   │
│  │ [G-5: 更新引用] ── [G-6: CMakeLists]                             │   │
│  │         │                                                        │   │
│  │         ▼                                                        │   │
│  │ [G-7: 符号化] ── [G-8: 规则引擎]                                  │   │
│  │ [G-9: 过滤器]                                                    │   │
│  │ [G-10: 标注引擎]                                                 │   │
│  │ [G-11: 图层管理]                                                 │   │
│  │ [G-12: 瓦片渲染]                                                 │   │
│  │ [G-13: LOD 系统]                                                 │   │
│  │ [G-14: 坐标转换] ← D-3                                           │   │
│  │ [G-15: DrawFacade]                                               │   │
│  │         │                                                        │   │
│  │         ▼                                                        │   │
│  │ [G-16: 测试]                                                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 七、风险登记

| Risk ID | Risk | Probability | Impact | Mitigation |
|---------|------|-------------|--------|------------|
| R1 | API 不兼容 | High | High | 提供迁移指南和兼容层 |
| R2 | 编译错误 | Medium | Medium | 分阶段重构，逐步验证 |
| R3 | 测试失败 | Medium | High | 每个阶段完成后运行测试 |
| R4 | 性能下降 | Low | Medium | 性能基准测试 |
| R5 | 循环依赖 | Low | High | 仔细设计模块边界 |

---

## 八、变更日志

| Version | Date | Changes |
|---------|------|---------|
| v1.0 | 2026-03-31 | 初始计划创建 |
| v1.1 | 2026-03-31 | 添加驱动类删除任务 (G-3)，补充里程碑详情和任务详细说明 |
| v2.0 | 2026-03-31 | 移除 drawbase 模块，基础类型保留在 draw 模块，添加类型合并和拆分规则 |

---

*文档生成时间: 2026-03-31*
