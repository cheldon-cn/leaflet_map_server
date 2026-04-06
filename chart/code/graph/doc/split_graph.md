# ogc_graph 模块拆分方案

> **版本**: v1.1  
> **日期**: 2026-04-06  
> **状态**: 规划中

---

## 一、拆分背景

### 1.1 当前问题

| 问题 | 说明 |
|------|------|
| 职责混杂 | 单个模块包含60+个文件，职责不清晰 |
| 依赖复杂 | 模块间依赖关系不明确 |
| 复用困难 | 功能耦合度高，难以独立复用 |
| 维护成本高 | 代码量大，修改影响范围广 |

### 1.2 拆分目标

- **职责单一**：每个模块职责明确
- **依赖清晰**：上层依赖下层，无循环依赖
- **独立复用**：基础模块可独立使用
- **易于维护**：模块边界清晰，修改影响可控

---

## 二、拆分方案概览

### 2.1 拆分结果

| 新模块 | 文件数 | 核心职责 |
|--------|--------|----------|
| ogc_base | 4 | 基础工具（日志、线程安全、性能监控） |
| ogc_proj | 4 | 坐标转换、投影变换 |
| ogc_cache | 9 | 瓦片缓存、离线存储 |
| ogc_symbology | 18 | 样式解析、过滤规则、符号化器 |
| ogc_graph | 22 | 地图渲染核心（保留） |
| ogc_navi | 1 | 航迹记录（合并） |

> **说明**: ogc_graph 保留22个文件（含export.h），包含渲染核心、标签、交互、图层管理、LOD等功能。这些功能在地图应用中紧密关联，保留在同一模块可降低跨模块调用开销，提高开发效率。

### 2.2 模块分层架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           模块分层架构                                   │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  Layer 1: 基础层                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │                          ogc_base                               │    │
│  │              (日志、线程安全、性能监控) 4个文件                  │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│                                    ↓                                    │
│  Layer 2: 核心层                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                      │
│  │ogc_geometry │  │  ogc_proj   │  │  ogc_draw   │                      │
│  │  (几何核心)  │  │ (坐标转换)  │  │ (绘图引擎)  │                      │
│  │   (已有)    │  │  (新建)     │  │  (已有+1)   │                      │
│  └─────────────┘  └─────────────┘  └─────────────┘                      │
│         ↓               ↓               ↓                               │
│  Layer 3: 数据层                                                         │
│  ┌─────────────┐  ┌─────────────┐                                       │
│  │ ogc_feature │  │  ogc_layer  │                                       │
│  │  (要素)     │  │  (图层)     │                                       │
│  │   (已有)    │  │   (已有)    │                                       │
│  └─────────────┘  └─────────────┘                                       │
│         ↓               ↓                                               │
│  Layer 4: 缓存层                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │                          ogc_cache                              │    │
│  │                (瓦片缓存 + 离线存储) 9个文件                     │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│                                    ↓                                    │
│  Layer 5: 符号化层                                                       │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │                        ogc_symbology                            │    │
│  │               (样式解析 + 过滤规则 + 符号化器) 18个文件          │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│                                    ↓                                    │
│  Layer 6: 地图层                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │                          ogc_graph                              │    │
│  │          (渲染核心、标签、交互、图层管理、LOD) 20个文件          │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│                                    ↓                                    │
│  Layer 7: 业务层                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                      │
│  │  ogc_navi   │  │  ogc_alert  │  │  ogc_alarm  │                      │
│  │  (导航)     │  │  (预警)     │  │  (告警)     │                      │
│  │ (已有+1)    │  │   (已有)    │  │   (已有)    │                       │
│  └─────────────┘  └─────────────┘  └─────────────┘                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2.3 模块依赖关系

```
                    ogc_base
                        ↓
            ┌───────────┼───────────┐
            ↓           ↓           ↓
       ogc_geometry  ogc_proj   ogc_draw
            ↓           ↓           ↓
            └─────┬─────┴───────────┘
                  ↓
            ogc_feature
                  ↓
            ogc_layer
                  ↓
            ogc_cache
                  ↓
           ogc_symbology
                  ↓
            ogc_graph
                  ↓
         ┌────────┼────────┐
         ↓        ↓        ↓
     ogc_navi ogc_alert ogc_alarm
```

### 2.4 循环依赖检测

为确保无循环依赖，使用以下方法验证：

**检测工具**：
- `cpp-dependencies`：生成依赖关系图，检测循环
- `clang -M`：生成头文件依赖，手动检查

**验证步骤**：
```bash
# 1. 生成依赖关系图
cpp-dependencies --generate-graph .

# 2. 检测循环依赖
cpp-dependencies --check-cycles .

# 3. 输出依赖矩阵
cpp-dependencies --output-matrix dependencies.txt
```

**预期结果**：
- 所有依赖关系为单向（上层→下层）
- 无循环依赖警告
- 依赖矩阵为下三角矩阵

---

## 三、各模块详细设计

### 3.1 ogc_base (基础工具库)

#### 职责定义

提供全局通用的基础能力，无业务依赖。

#### 文件清单

| 文件 | 职责 | 来源 |
|------|------|------|
| log.h | 日志记录 | ogc_graph |
| thread_safe.h | 线程安全工具 | ogc_graph |
| performance_metrics.h | 性能指标定义 | ogc_graph |
| performance_monitor.h | 性能监控管理 | ogc_graph |

#### 目录结构

```
ogc_base/
├── include/ogc/base/
│   ├── export.h
│   ├── log.h
│   ├── thread_safe.h
│   ├── performance_metrics.h
│   └── performance_monitor.h
└── src/
    ├── log.cpp
    ├── thread_safe.cpp
    ├── performance_metrics.cpp
    └── performance_monitor.cpp
```

#### 依赖关系

```
ogc_base
└── 无外部依赖（仅依赖C++11标准库）
```

#### 设计原则

- 零业务依赖
- 可被所有模块复用
- 独立测试、独立演进

---

### 3.2 ogc_proj (坐标转换库)

#### 职责定义

提供坐标转换和投影变换能力，依赖PROJ库。

#### 文件清单

| 文件 | 职责 | 来源 |
|------|------|------|
| coordinate_transform.h | 坐标转换核心 | ogc_graph |
| coordinate_transformer.h | 转换器接口 | ogc_graph |
| proj_transformer.h | PROJ转换器实现 | ogc_graph |
| coord_system_preset.h | 坐标系预设 | ogc_graph |

#### 目录结构

```
ogc_proj/
├── include/ogc/proj/
│   ├── export.h
│   ├── coordinate_transform.h
│   ├── coordinate_transformer.h
│   ├── proj_transformer.h
│   └── coord_system_preset.h
└── src/
    ├── coordinate_transform.cpp
    ├── coordinate_transformer.cpp
    ├── proj_transformer.cpp
    └── coord_system_preset.cpp
```

#### 依赖关系

```
ogc_proj
├── ogc_geometry (几何类型)
└── PROJ (外部库)
```

#### 设计原则

- 依赖PROJ库，具有独立技术栈
- 可被多个模块复用（navi、graph、alert）
- GIS专业能力封装

---

### 3.3 ogc_cache (缓存库)

#### 职责定义

提供瓦片缓存和离线存储能力。

#### 文件清单

| 类别 | 文件 | 职责 | 来源 |
|------|------|------|------|
| 瓦片缓存 | tile_cache.h | 瓦片缓存基类 | ogc_graph |
| 瓦片缓存 | tile_key.h | 瓦片键定义 | ogc_graph |
| 瓦片缓存 | tile_renderer.h | 瓦片渲染器 | ogc_graph |
| 瓦片缓存 | memory_tile_cache.h | 内存缓存 | ogc_graph |
| 瓦片缓存 | disk_tile_cache.h | 磁盘缓存 | ogc_graph |
| 瓦片缓存 | multi_level_tile_cache.h | 多级缓存 | ogc_graph |
| 离线功能 | offline_storage_manager.h | 离线存储管理 | ogc_graph |
| 离线功能 | offline_sync_manager.h | 离线同步管理 | ogc_graph |
| 离线功能 | data_encryption.h | 数据加密 | ogc_graph |

#### 目录结构

```
ogc_cache/
├── include/ogc/cache/
│   ├── export.h
│   │
│   ├── tile/                       # 瓦片缓存
│   │   ├── tile_cache.h
│   │   ├── tile_key.h
│   │   ├── tile_renderer.h
│   │   ├── memory_tile_cache.h
│   │   ├── disk_tile_cache.h
│   │   └── multi_level_tile_cache.h
│   │
│   └── offline/                    # 离线功能
│       ├── offline_storage_manager.h
│       ├── offline_sync_manager.h
│       └── data_encryption.h
│
└── src/
    ├── tile/
    │   ├── tile_cache.cpp
    │   ├── tile_key.cpp
    │   ├── tile_renderer.cpp
    │   ├── memory_tile_cache.cpp
    │   ├── disk_tile_cache.cpp
    │   └── multi_level_tile_cache.cpp
    │
    └── offline/
        ├── offline_storage_manager.cpp
        ├── offline_sync_manager.cpp
        └── data_encryption.cpp
```

#### 依赖关系

```
ogc_cache
├── ogc_geometry (几何类型)
├── ogc_draw (绘图引擎)
└── ogc_layer (图层)
```

#### 设计原则

- 数据流统一：离线下载 → 存储 → 缓存 → 渲染
- 存储策略统一：内存/磁盘/加密统一管理
- 场景关联：离线地图和瓦片缓存紧密相关

---

### 3.4 ogc_symbology (符号化库)

#### 职责定义

提供样式解析、过滤规则和符号化渲染能力。

#### 文件清单

| 类别 | 文件 | 职责 | 来源 |
|------|------|------|------|
| 样式解析 | sld_parser.h | SLD样式解析 | ogc_graph |
| 样式解析 | mapbox_style_parser.h | Mapbox样式解析 | ogc_graph |
| 样式解析 | s52_style_manager.h | S52海图样式管理 | ogc_graph |
| 样式解析 | s52_symbol_renderer.h | S52符号渲染 | ogc_graph |
| 过滤规则 | filter.h | 过滤器基类 | ogc_graph |
| 过滤规则 | comparison_filter.h | 比较过滤器 | ogc_graph |
| 过滤规则 | logical_filter.h | 逻辑过滤器 | ogc_graph |
| 过滤规则 | spatial_filter.h | 空间过滤器 | ogc_graph |
| 过滤规则 | symbolizer_rule.h | 符号化规则 | ogc_graph |
| 过滤规则 | rule_engine.h | 规则引擎 | ogc_graph |
| 符号化器 | symbolizer.h | 符号化器基类 | ogc_graph |
| 符号化器 | point_symbolizer.h | 点符号化器 | ogc_graph |
| 符号化器 | line_symbolizer.h | 线符号化器 | ogc_graph |
| 符号化器 | polygon_symbolizer.h | 多边形符号化器 | ogc_graph |
| 符号化器 | text_symbolizer.h | 文本符号化器 | ogc_graph |
| 符号化器 | raster_symbolizer.h | 栅格符号化器 | ogc_graph |
| 符号化器 | icon_symbolizer.h | 图标符号化器 | ogc_graph |
| 符号化器 | composite_symbolizer.h | 组合符号化器 | ogc_graph |

#### 目录结构

```
ogc_symbology/
├── include/ogc/symbology/
│   ├── export.h
│   │
│   ├── style/                      # 样式解析
│   │   ├── sld_parser.h
│   │   ├── mapbox_style_parser.h
│   │   ├── s52_style_manager.h
│   │   └── s52_symbol_renderer.h
│   │
│   ├── filter/                     # 过滤规则
│   │   ├── filter.h
│   │   ├── comparison_filter.h
│   │   ├── logical_filter.h
│   │   ├── spatial_filter.h
│   │   ├── symbolizer_rule.h
│   │   └── rule_engine.h
│   │
│   └── symbolizer/                 # 符号化器
│       ├── symbolizer.h
│       ├── point_symbolizer.h
│       ├── line_symbolizer.h
│       ├── polygon_symbolizer.h
│       ├── text_symbolizer.h
│       ├── raster_symbolizer.h
│       ├── icon_symbolizer.h
│       └── composite_symbolizer.h
│
└── src/
    ├── style/
    │   ├── sld_parser.cpp
    │   ├── mapbox_style_parser.cpp
    │   ├── s52_style_manager.cpp
    │   └── s52_symbol_renderer.cpp
    │
    ├── filter/
    │   ├── filter.cpp
    │   ├── comparison_filter.cpp
    │   ├── logical_filter.cpp
    │   ├── spatial_filter.cpp
    │   ├── symbolizer_rule.cpp
    │   └── rule_engine.cpp
    │
    └── symbolizer/
        ├── symbolizer.cpp
        ├── point_symbolizer.cpp
        ├── line_symbolizer.cpp
        ├── polygon_symbolizer.cpp
        ├── text_symbolizer.cpp
        ├── raster_symbolizer.cpp
        ├── icon_symbolizer.cpp
        └── composite_symbolizer.cpp
```

#### 依赖关系

```
ogc_symbology
├── ogc_geometry (几何类型)
├── ogc_draw (绘图引擎)
└── ogc_feature (要素)
```

#### 设计原则

- 渲染链完整：样式解析 → 过滤规则 → 符号化器
- 标准集成：SLD/Mapbox样式标准本身就包含过滤规则和符号化定义
- 使用便利：用户只需引入一个库即可完成渲染功能

---

### 3.5 ogc_graph (地图渲染库 - 保留)

#### 职责定义

提供地图渲染核心能力，包括渲染任务调度、标签引擎、交互处理、图层管理和LOD。

#### 文件清单

| 类别 | 文件 | 职责 |
|------|------|------|
| 渲染核心 | draw_facade.h | 绘制门面 |
| 渲染核心 | draw_params.h | 绘制参数 |
| 渲染核心 | render_task.h | 渲染任务基类 |
| 渲染核心 | basic_render_task.h | 基础渲染任务 |
| 渲染核心 | render_queue.h | 渲染队列 |
| 渲染核心 | async_renderer.h | 异步渲染器 |
| 标签引擎 | label_engine.h | 标签引擎 |
| 标签引擎 | label_placement.h | 标签放置 |
| 标签引擎 | label_conflict.h | 标签冲突 |
| 交互处理 | interaction_handler.h | 交互处理器基类 |
| 交互处理 | pan_zoom_handler.h | 平移缩放 |
| 交互处理 | selection_handler.h | 选择处理 |
| 交互处理 | location_display_handler.h | 位置显示 |
| 交互处理 | hit_test.h | 命中测试 |
| 交互处理 | interaction_feedback.h | 交互反馈 |
| 图层管理 | layer_manager.h | 图层管理器 |
| 图层管理 | layer_control_panel.h | 图层控制面板 |
| LOD | lod.h | 细节层次定义 |
| LOD | lod_manager.h | 细节层次管理器 |
| 工具 | image_draw.h | 图像绘制 |
| 工具 | transform_manager.h | 变换管理器 |
| 工具 | day_night_mode_manager.h | 昼夜模式管理 |

#### 目录结构

```
ogc_graph/
├── include/ogc/graph/
│   ├── export.h
│   │
│   ├── render/                     # 渲染核心
│   │   ├── draw_facade.h
│   │   ├── draw_params.h
│   │   ├── render_task.h
│   │   ├── basic_render_task.h
│   │   ├── render_queue.h
│   │   └── async_renderer.h
│   │
│   ├── label/                      # 标签引擎
│   │   ├── label_engine.h
│   │   ├── label_placement.h
│   │   └── label_conflict.h
│   │
│   ├── interaction/                # 交互处理
│   │   ├── interaction_handler.h
│   │   ├── pan_zoom_handler.h
│   │   ├── selection_handler.h
│   │   ├── location_display_handler.h
│   │   ├── hit_test.h
│   │   └── interaction_feedback.h
│   │
│   ├── layer/                      # 图层管理
│   │   ├── layer_manager.h
│   │   └── layer_control_panel.h
│   │
│   ├── lod/                        # 细节层次
│   │   ├── lod.h
│   │   └── lod_manager.h
│   │
│   └── util/                       # 工具
│       ├── image_draw.h
│       ├── transform_manager.h
│       └── day_night_mode_manager.h
│
└── src/
    ├── render/
    ├── label/
    ├── interaction/
    ├── layer/
    ├── lod/
    └── util/
```

#### 依赖关系

```
ogc_graph
├── ogc_geometry (几何类型)
├── ogc_draw (绘图引擎)
├── ogc_cache (缓存)
├── ogc_symbology (符号化)
└── ogc_layer (图层)
```

#### 设计原则

- 地图业务核心：渲染任务调度、标签、交互、图层、LOD
- 不可拆分：瓦片渲染、标签、交互等功能紧密关联
- 依赖上层：依赖ogc_symbology、ogc_cache等模块

---

### 3.6 ogc_draw (绘图引擎 - 合并)

#### 新增文件

| 文件 | 职责 | 来源 |
|------|------|------|
| clipper.h | 几何裁剪 | ogc_graph |

#### 合并原因

- clipper 是几何裁剪工具，是绘图引擎的基础能力
- draw模块已有geometry.h处理几何，clipper可归入

---

### 3.7 ogc_navi (导航库 - 合并)

#### 新增文件

| 文件 | 职责 | 来源 |
|------|------|------|
| track_recorder.h | 航迹记录 | ogc_graph |

#### 合并原因

- 航迹记录是导航业务的核心功能
- navi已有track模块，track_recorder可归入

---

## 四、文件迁移清单

### 4.1 迁移到 ogc_base

| 原路径 | 新路径 |
|--------|--------|
| ogc/graph/include/ogc/draw/log.h | ogc/base/include/ogc/base/log.h |
| ogc/graph/include/ogc/draw/thread_safe.h | ogc/base/include/ogc/base/thread_safe.h |
| ogc/graph/include/ogc/draw/performance_metrics.h | ogc/base/include/ogc/base/performance_metrics.h |
| ogc/graph/include/ogc/draw/performance_monitor.h | ogc/base/include/ogc/base/performance_monitor.h |

### 4.2 迁移到 ogc_proj

| 原路径 | 新路径 |
|--------|--------|
| ogc/graph/include/ogc/draw/coordinate_transform.h | ogc/proj/include/ogc/proj/coordinate_transform.h |
| ogc/graph/include/ogc/draw/coordinate_transformer.h | ogc/proj/include/ogc/proj/coordinate_transformer.h |
| ogc/graph/include/ogc/draw/proj_transformer.h | ogc/proj/include/ogc/proj/proj_transformer.h |
| ogc/graph/include/ogc/draw/coord_system_preset.h | ogc/proj/include/ogc/proj/coord_system_preset.h |

### 4.3 迁移到 ogc_cache

| 原路径 | 新路径 |
|--------|--------|
| ogc/graph/include/ogc/draw/tile_cache.h | ogc/cache/include/ogc/cache/tile/tile_cache.h |
| ogc/graph/include/ogc/draw/tile_key.h | ogc/cache/include/ogc/cache/tile/tile_key.h |
| ogc/graph/include/ogc/draw/tile_renderer.h | ogc/cache/include/ogc/cache/tile/tile_renderer.h |
| ogc/graph/include/ogc/draw/memory_tile_cache.h | ogc/cache/include/ogc/cache/tile/memory_tile_cache.h |
| ogc/graph/include/ogc/draw/disk_tile_cache.h | ogc/cache/include/ogc/cache/tile/disk_tile_cache.h |
| ogc/graph/include/ogc/draw/multi_level_tile_cache.h | ogc/cache/include/ogc/cache/tile/multi_level_tile_cache.h |
| ogc/graph/include/ogc/draw/offline_storage_manager.h | ogc/cache/include/ogc/cache/offline/offline_storage_manager.h |
| ogc/graph/include/ogc/draw/offline_sync_manager.h | ogc/cache/include/ogc/cache/offline/offline_sync_manager.h |
| ogc/graph/include/ogc/draw/data_encryption.h | ogc/cache/include/ogc/cache/offline/data_encryption.h |

### 4.4 迁移到 ogc_symbology

| 原路径 | 新路径 |
|--------|--------|
| ogc/graph/include/ogc/draw/sld_parser.h | ogc/symbology/include/ogc/symbology/style/sld_parser.h |
| ogc/graph/include/ogc/draw/mapbox_style_parser.h | ogc/symbology/include/ogc/symbology/style/mapbox_style_parser.h |
| ogc/graph/include/ogc/draw/s52_style_manager.h | ogc/symbology/include/ogc/symbology/style/s52_style_manager.h |
| ogc/graph/include/ogc/draw/s52_symbol_renderer.h | ogc/symbology/include/ogc/symbology/style/s52_symbol_renderer.h |
| ogc/graph/include/ogc/draw/filter.h | ogc/symbology/include/ogc/symbology/filter/filter.h |
| ogc/graph/include/ogc/draw/comparison_filter.h | ogc/symbology/include/ogc/symbology/filter/comparison_filter.h |
| ogc/graph/include/ogc/draw/logical_filter.h | ogc/symbology/include/ogc/symbology/filter/logical_filter.h |
| ogc/graph/include/ogc/draw/spatial_filter.h | ogc/symbology/include/ogc/symbology/filter/spatial_filter.h |
| ogc/graph/include/ogc/draw/symbolizer_rule.h | ogc/symbology/include/ogc/symbology/filter/symbolizer_rule.h |
| ogc/graph/include/ogc/draw/rule_engine.h | ogc/symbology/include/ogc/symbology/filter/rule_engine.h |
| ogc/graph/include/ogc/draw/symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/symbolizer.h |
| ogc/graph/include/ogc/draw/point_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/point_symbolizer.h |
| ogc/graph/include/ogc/draw/line_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/line_symbolizer.h |
| ogc/graph/include/ogc/draw/polygon_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/polygon_symbolizer.h |
| ogc/graph/include/ogc/draw/text_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/text_symbolizer.h |
| ogc/graph/include/ogc/draw/raster_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/raster_symbolizer.h |
| ogc/graph/include/ogc/draw/icon_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/icon_symbolizer.h |
| ogc/graph/include/ogc/draw/composite_symbolizer.h | ogc/symbology/include/ogc/symbology/symbolizer/composite_symbolizer.h |

### 4.5 迁移到 ogc_draw

| 原路径 | 新路径 |
|--------|--------|
| ogc/graph/include/ogc/draw/clipper.h | ogc/draw/include/ogc/draw/clipper.h |

### 4.6 迁移到 ogc_navi

| 原路径 | 新路径 |
|--------|--------|
| ogc/graph/include/ogc/draw/track_recorder.h | ogc/navi/include/ogc/navi/track_recorder.h |

### 4.7 保留在 ogc_graph

| 文件 | 新路径（重组） |
|------|----------------|
| draw_facade.h | ogc/graph/include/ogc/graph/render/draw_facade.h |
| draw_params.h | ogc/graph/include/ogc/graph/render/draw_params.h |
| render_task.h | ogc/graph/include/ogc/graph/render/render_task.h |
| basic_render_task.h | ogc/graph/include/ogc/graph/render/basic_render_task.h |
| render_queue.h | ogc/graph/include/ogc/graph/render/render_queue.h |
| async_renderer.h | ogc/graph/include/ogc/graph/render/async_renderer.h |
| label_engine.h | ogc/graph/include/ogc/graph/label/label_engine.h |
| label_placement.h | ogc/graph/include/ogc/graph/label/label_placement.h |
| label_conflict.h | ogc/graph/include/ogc/graph/label/label_conflict.h |
| interaction_handler.h | ogc/graph/include/ogc/graph/interaction/interaction_handler.h |
| pan_zoom_handler.h | ogc/graph/include/ogc/graph/interaction/pan_zoom_handler.h |
| selection_handler.h | ogc/graph/include/ogc/graph/interaction/selection_handler.h |
| location_display_handler.h | ogc/graph/include/ogc/graph/interaction/location_display_handler.h |
| hit_test.h | ogc/graph/include/ogc/graph/interaction/hit_test.h |
| interaction_feedback.h | ogc/graph/include/ogc/graph/interaction/interaction_feedback.h |
| layer_manager.h | ogc/graph/include/ogc/graph/layer/layer_manager.h |
| layer_control_panel.h | ogc/graph/include/ogc/graph/layer/layer_control_panel.h |
| lod.h | ogc/graph/include/ogc/graph/lod/lod.h |
| lod_manager.h | ogc/graph/include/ogc/graph/lod/lod_manager.h |
| image_draw.h | ogc/graph/include/ogc/graph/util/image_draw.h |
| transform_manager.h | ogc/graph/include/ogc/graph/util/transform_manager.h |
| day_night_mode_manager.h | ogc/graph/include/ogc/graph/util/day_night_mode_manager.h |

---

## 五、测试文件迁移

### 5.1 迁移到 ogc_base/tests

| 原文件 | 新文件 |
|--------|--------|
| test_log.cpp | test_log.cpp |
| test_thread_safe.cpp | test_thread_safe.cpp |
| test_performance_metrics.cpp | test_performance_metrics.cpp |
| test_performance_monitor.cpp | test_performance_monitor.cpp |

### 5.2 迁移到 ogc_proj/tests

| 原文件 | 新文件 |
|--------|--------|
| test_coordinate_transform.cpp | test_coordinate_transform.cpp |
| test_coordinate_transformer.cpp | test_coordinate_transformer.cpp |
| test_proj_transformer.cpp | test_proj_transformer.cpp |
| test_coord_system_preset.cpp | test_coord_system_preset.cpp |

### 5.3 迁移到 ogc_cache/tests

| 原文件 | 新文件 |
|--------|--------|
| test_tile_cache.cpp | test_tile_cache.cpp |
| test_tile_key.cpp | test_tile_key.cpp |
| test_tile_renderer.cpp | test_tile_renderer.cpp |
| test_memory_tile_cache.cpp | test_memory_tile_cache.cpp |
| test_disk_tile_cache.cpp | test_disk_tile_cache.cpp |
| test_multi_level_tile_cache.cpp | test_multi_level_tile_cache.cpp |
| test_offline_storage_manager.cpp | test_offline_storage_manager.cpp |
| test_offline_sync_manager.cpp | test_offline_sync_manager.cpp |
| test_data_encryption.cpp | test_data_encryption.cpp |

### 5.4 迁移到 ogc_symbology/tests

| 原文件 | 新文件 |
|--------|--------|
| test_sld_parser.cpp | test_sld_parser.cpp |
| test_mapbox_style_parser.cpp | test_mapbox_style_parser.cpp |
| test_s52_style_manager.cpp | test_s52_style_manager.cpp |
| test_s52_symbol_renderer.cpp | test_s52_symbol_renderer.cpp |
| test_filter.cpp | test_filter.cpp |
| test_comparison_filter.cpp | test_comparison_filter.cpp |
| test_logical_filter.cpp | test_logical_filter.cpp |
| test_spatial_filter.cpp | test_spatial_filter.cpp |
| test_symbolizer_rule.cpp | test_symbolizer_rule.cpp |
| test_rule_engine.cpp | test_rule_engine.cpp |
| test_symbolizer.cpp | test_symbolizer.cpp |
| test_point_symbolizer.cpp | test_point_symbolizer.cpp |
| test_line_symbolizer.cpp | test_line_symbolizer.cpp |
| test_polygon_symbolizer.cpp | test_polygon_symbolizer.cpp |
| test_text_symbolizer.cpp | test_text_symbolizer.cpp |
| test_raster_symbolizer.cpp | test_raster_symbolizer.cpp |
| test_icon_symbolizer.cpp | test_icon_symbolizer.cpp |
| test_composite_symbolizer.cpp | test_composite_symbolizer.cpp |

### 5.5 迁移到 ogc_draw/tests

| 原文件 | 新文件 |
|--------|--------|
| test_clipper.cpp | test_clipper.cpp |

### 5.6 迁移到 ogc_navi/tests

| 原文件 | 新文件 |
|--------|--------|
| test_track_recorder.cpp | test_track_recorder.cpp |

### 5.7 保留在 ogc_graph/tests

| 文件 | 说明 |
|------|------|
| test_draw_facade.cpp | 渲染核心测试 |
| test_draw_params.cpp | 渲染核心测试 |
| test_render_task.cpp | 渲染核心测试 |
| test_basic_render_task.cpp | 渲染核心测试 |
| test_render_queue.cpp | 渲染核心测试 |
| test_async_renderer.cpp | 渲染核心测试 |
| test_label_engine.cpp | 标签引擎测试 |
| test_label_placement.cpp | 标签引擎测试 |
| test_label_conflict.cpp | 标签引擎测试 |
| test_interaction.cpp | 交互测试 |
| test_pan_zoom_handler.cpp | 交互测试 |
| test_selection_handler.cpp | 交互测试 |
| test_location_display_handler.cpp | 交互测试 |
| test_layer_manager.cpp | 图层管理测试 |
| test_layer_control_panel.cpp | 图层管理测试 |
| test_lod.cpp | LOD测试 |
| test_lod_manager.cpp | LOD测试 |
| test_image_draw.cpp | 工具测试 |
| test_transform_manager.cpp | 工具测试 |
| test_day_night_mode_manager.cpp | 工具测试 |
| test_it_*.cpp | 集成测试（保留） |

---

## 六、CMake配置变更

### 6.1 新增模块 CMakeLists.txt

#### ogc_base/CMakeLists.txt

```cmake
project(ogc_base)

set(SOURCES
    src/log.cpp
    src/thread_safe.cpp
    src/performance_metrics.cpp
    src/performance_monitor.cpp
)

set(HEADERS
    include/ogc/base/export.h
    include/ogc/base/log.h
    include/ogc/base/thread_safe.h
    include/ogc/base/performance_metrics.h
    include/ogc/base/performance_monitor.h
)

add_library(ogc_base SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_base
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(ogc_base PRIVATE OGC_BASE_EXPORTS)
```

#### ogc_proj/CMakeLists.txt

```cmake
project(ogc_proj)

find_package(PROJ REQUIRED)

set(SOURCES
    src/coordinate_transform.cpp
    src/coordinate_transformer.cpp
    src/proj_transformer.cpp
    src/coord_system_preset.cpp
)

set(HEADERS
    include/ogc/proj/export.h
    include/ogc/proj/coordinate_transform.h
    include/ogc/proj/coordinate_transformer.h
    include/ogc/proj/proj_transformer.h
    include/ogc/proj/coord_system_preset.h
)

add_library(ogc_proj SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_proj
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_proj
    PUBLIC
        ogc_geometry
        PROJ::proj
)

target_compile_definitions(ogc_proj PRIVATE OGC_PROJ_EXPORTS)
```

#### ogc_cache/CMakeLists.txt

```cmake
project(ogc_cache)

set(SOURCES
    src/tile/tile_cache.cpp
    src/tile/tile_key.cpp
    src/tile/tile_renderer.cpp
    src/tile/memory_tile_cache.cpp
    src/tile/disk_tile_cache.cpp
    src/tile/multi_level_tile_cache.cpp
    src/offline/offline_storage_manager.cpp
    src/offline/offline_sync_manager.cpp
    src/offline/data_encryption.cpp
)

set(HEADERS
    include/ogc/cache/export.h
    include/ogc/cache/tile/tile_cache.h
    include/ogc/cache/tile/tile_key.h
    include/ogc/cache/tile/tile_renderer.h
    include/ogc/cache/tile/memory_tile_cache.h
    include/ogc/cache/tile/disk_tile_cache.h
    include/ogc/cache/tile/multi_level_tile_cache.h
    include/ogc/cache/offline/offline_storage_manager.h
    include/ogc/cache/offline/offline_sync_manager.h
    include/ogc/cache/offline/data_encryption.h
)

add_library(ogc_cache SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_cache
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_cache
    PUBLIC
        ogc_geometry
        ogc_draw
        ogc_layer
)

target_compile_definitions(ogc_cache PRIVATE OGC_CACHE_EXPORTS)
```

#### ogc_symbology/CMakeLists.txt

```cmake
project(ogc_symbology)

set(SOURCES
    src/style/sld_parser.cpp
    src/style/mapbox_style_parser.cpp
    src/style/s52_style_manager.cpp
    src/style/s52_symbol_renderer.cpp
    src/filter/filter.cpp
    src/filter/comparison_filter.cpp
    src/filter/logical_filter.cpp
    src/filter/spatial_filter.cpp
    src/filter/symbolizer_rule.cpp
    src/filter/rule_engine.cpp
    src/symbolizer/symbolizer.cpp
    src/symbolizer/point_symbolizer.cpp
    src/symbolizer/line_symbolizer.cpp
    src/symbolizer/polygon_symbolizer.cpp
    src/symbolizer/text_symbolizer.cpp
    src/symbolizer/raster_symbolizer.cpp
    src/symbolizer/icon_symbolizer.cpp
    src/symbolizer/composite_symbolizer.cpp
)

set(HEADERS
    include/ogc/symbology/export.h
    include/ogc/symbology/style/sld_parser.h
    include/ogc/symbology/style/mapbox_style_parser.h
    include/ogc/symbology/style/s52_style_manager.h
    include/ogc/symbology/style/s52_symbol_renderer.h
    include/ogc/symbology/filter/filter.h
    include/ogc/symbology/filter/comparison_filter.h
    include/ogc/symbology/filter/logical_filter.h
    include/ogc/symbology/filter/spatial_filter.h
    include/ogc/symbology/filter/symbolizer_rule.h
    include/ogc/symbology/filter/rule_engine.h
    include/ogc/symbology/symbolizer/symbolizer.h
    include/ogc/symbology/symbolizer/point_symbolizer.h
    include/ogc/symbology/symbolizer/line_symbolizer.h
    include/ogc/symbology/symbolizer/polygon_symbolizer.h
    include/ogc/symbology/symbolizer/text_symbolizer.h
    include/ogc/symbology/symbolizer/raster_symbolizer.h
    include/ogc/symbology/symbolizer/icon_symbolizer.h
    include/ogc/symbology/symbolizer/composite_symbolizer.h
)

add_library(ogc_symbology SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_symbology
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_symbology
    PUBLIC
        ogc_geometry
        ogc_draw
        ogc_feature
)

target_compile_definitions(ogc_symbology PRIVATE OGC_SYMBOLOGY_EXPORTS)
```

### 6.2 更新 ogc_graph/CMakeLists.txt

```cmake
project(ogc_graph)

set(SOURCES
    src/render/draw_facade.cpp
    src/render/draw_params.cpp
    src/render/render_task.cpp
    src/render/basic_render_task.cpp
    src/render/render_queue.cpp
    src/render/async_renderer.cpp
    src/label/label_engine.cpp
    src/label/label_placement.cpp
    src/label/label_conflict.cpp
    src/interaction/interaction_handler.cpp
    src/interaction/pan_zoom_handler.cpp
    src/interaction/selection_handler.cpp
    src/interaction/location_display_handler.cpp
    src/interaction/hit_test.cpp
    src/interaction/interaction_feedback.cpp
    src/layer/layer_manager.cpp
    src/layer/layer_control_panel.cpp
    src/lod/lod.cpp
    src/lod/lod_manager.cpp
    src/util/image_draw.cpp
    src/util/transform_manager.cpp
    src/util/day_night_mode_manager.cpp
)

set(HEADERS
    include/ogc/graph/export.h
    include/ogc/graph/render/draw_facade.h
    include/ogc/graph/render/draw_params.h
    include/ogc/graph/render/render_task.h
    include/ogc/graph/render/basic_render_task.h
    include/ogc/graph/render/render_queue.h
    include/ogc/graph/render/async_renderer.h
    include/ogc/graph/label/label_engine.h
    include/ogc/graph/label/label_placement.h
    include/ogc/graph/label/label_conflict.h
    include/ogc/graph/interaction/interaction_handler.h
    include/ogc/graph/interaction/pan_zoom_handler.h
    include/ogc/graph/interaction/selection_handler.h
    include/ogc/graph/interaction/location_display_handler.h
    include/ogc/graph/interaction/hit_test.h
    include/ogc/graph/interaction/interaction_feedback.h
    include/ogc/graph/layer/layer_manager.h
    include/ogc/graph/layer/layer_control_panel.h
    include/ogc/graph/lod/lod.h
    include/ogc/graph/lod/lod_manager.h
    include/ogc/graph/util/image_draw.h
    include/ogc/graph/util/transform_manager.h
    include/ogc/graph/util/day_night_mode_manager.h
)

add_library(ogc_graph SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_graph
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_graph
    PUBLIC
        ogc_geometry
        ogc_draw
        ogc_cache
        ogc_symbology
        ogc_layer
)

target_compile_definitions(ogc_graph PRIVATE OGC_GRAPH_EXPORTS)
```

---

## 七、实施计划

### 7.1 阶段划分

| 阶段 | 内容 | 预计时间 | 依赖阶段 |
|------|------|----------|----------|
| Phase 0 | 创建代码分支、准备回滚方案 | 0.5天 | - |
| Phase 1 | 创建新模块目录结构 | 0.5天 | Phase 0 |
| Phase 2 | 迁移ogc_base文件 | 1天 | Phase 1 |
| Phase 3 | 迁移ogc_proj文件 | 1天 | Phase 1 |
| Phase 4 | 迁移ogc_cache文件 | 1天 | Phase 1 |
| Phase 5 | 迁移ogc_symbology文件 | 2天 | Phase 1 |
| Phase 6 | 重组ogc_graph文件 | 1天 | Phase 2-5 |
| Phase 7 | 更新CMake配置 | 1天 | Phase 6 |
| Phase 8 | 更新#include路径 | 2天 | Phase 7 |
| Phase 9 | 迁移测试文件 | 1天 | Phase 8 |
| Phase 10 | 循环依赖检测验证 | 0.5天 | Phase 9 |
| Phase 11 | 编译验证 | 2天 | Phase 10 |
| Phase 12 | 测试验证 | 2天 | Phase 11 |

### 7.2 并行开发策略

以下阶段可并行执行：

```
Phase 0 (准备阶段)
    ↓
Phase 1 (目录结构)
    ↓
┌─────────────────────────────────────────────────────────────────┐
│                      并行实施阶段                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Phase 2          Phase 3          Phase 4          Phase 5    │
│  ogc_base         ogc_proj         ogc_cache        ogc_symbology│
│  (1天)            (1天)            (1天)            (2天)       │
│                                                                 │
│  依赖: 无         依赖: 无         依赖: 无         依赖: 无     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
    ↓
Phase 6 (重组ogc_graph) - 等待Phase 2-5完成
    ↓
... 后续阶段串行执行
```

**并行风险控制**：
- 各并行阶段独立工作目录
- 使用独立分支开发
- 合并前进行冲突检测

### 7.3 回滚方案

#### 回滚准备

| 步骤 | 内容 | 时间点 |
|------|------|--------|
| 1 | 创建主分支标签 `pre-split-v1.0` | Phase 0 |
| 2 | 创建开发分支 `feature/module-split` | Phase 0 |
| 3 | 每个阶段完成后创建检查点标签 | 各阶段结束 |

#### 检查点标签

| 检查点 | 标签名 | 说明 |
|--------|--------|------|
| CP0 | `split-cp0-ready` | 准备阶段完成 |
| CP1 | `split-cp1-structure` | 目录结构创建完成 |
| CP2 | `split-cp2-base` | ogc_base迁移完成 |
| CP3 | `split-cp3-proj` | ogc_proj迁移完成 |
| CP4 | `split-cp4-cache` | ogc_cache迁移完成 |
| CP5 | `split-cp5-symbology` | ogc_symbology迁移完成 |
| CP6 | `split-cp6-graph` | ogc_graph重组完成 |
| CP7 | `split-cp7-cmake` | CMake配置完成 |
| CP8 | `split-cp8-include` | #include更新完成 |
| CP9 | `split-cp9-test` | 测试迁移完成 |
| CP10 | `split-cp10-deps` | 依赖检测完成 |
| CP11 | `split-cp11-compile` | 编译验证完成 |
| CP12 | `split-cp12-final` | 最终验收完成 |

#### 回滚脚本

```bash
#!/bin/bash
# rollback_split.sh

TARGET_CP=$1

if [ -z "$TARGET_CP" ]; then
    echo "Usage: ./rollback_split.sh <checkpoint>"
    echo "Available checkpoints: cp0-cp12"
    exit 1
fi

git checkout split-$TARGET_CP-*
echo "Rollback to $TARGET_CP completed"
```

### 7.4 风险控制

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| #include路径遗漏 | 编译失败 | 全局搜索替换，逐模块验证 |
| 导出宏遗漏 | 链接失败 | 检查所有头文件导出宏 |
| 依赖关系错误 | 编译顺序错误 | 按分层顺序编译，使用工具检测 |
| 测试遗漏 | 功能回归 | 运行完整测试套件 |
| 循环依赖 | 编译/链接失败 | Phase 10 专项检测 |
| 并行冲突 | 代码冲突 | 独立分支开发，合并前检测 |

---

## 八、接口兼容性说明

### 8.1 头文件路径变更

| 原路径 | 新路径 |
|--------|--------|
| `#include <ogc/draw/log.h>` | `#include <ogc/base/log.h>` |
| `#include <ogc/draw/coordinate_transform.h>` | `#include <ogc/proj/coordinate_transform.h>` |
| `#include <ogc/draw/tile_cache.h>` | `#include <ogc/cache/tile/tile_cache.h>` |
| `#include <ogc/draw/sld_parser.h>` | `#include <ogc/symbology/style/sld_parser.h>` |
| `#include <ogc/draw/symbolizer.h>` | `#include <ogc/symbology/symbolizer/symbolizer.h>` |
| `#include <ogc/draw/filter.h>` | `#include <ogc/symbology/filter/filter.h>` |

### 8.2 命名空间变更

| 原命名空间 | 新命名空间 | 迁移策略 |
|------------|------------|----------|
| `ogc::draw::Log` | `ogc::base::Log` | 全局替换 |
| `ogc::draw::CoordinateTransform` | `ogc::proj::CoordinateTransform` | 全局替换 |
| `ogc::draw::TileCache` | `ogc::cache::TileCache` | 全局替换 |
| `ogc::draw::SldParser` | `ogc::symbology::SldParser` | 全局替换 |
| `ogc::draw::Symbolizer` | `ogc::symbology::Symbolizer` | 全局替换 |
| `ogc::draw::Filter` | `ogc::symbology::Filter` | 全局替换 |

### 8.3 兼容层方案（可选）

为降低迁移成本，可提供兼容层头文件：

```cpp
// ogc/draw/log.h (兼容层)
#ifndef OGC_DRAW_LOG_COMPAT_H
#define OGC_DRAW_LOG_COMPAT_H

#include <ogc/base/log.h>

namespace ogc {
namespace draw {
    using Log = ogc::base::Log;
}
}

#endif
```

**兼容层策略**：
1. 保留原头文件路径，内部转发到新路径
2. 使用 `using` 声明保持命名空间兼容
3. 标记为废弃（`[[deprecated]]`），提示用户迁移

### 8.4 迁移影响评估

| 影响范围 | 评估方法 | 预估工作量 |
|----------|----------|------------|
| 项目内部代码 | 全局搜索 `#include <ogc/draw/` | 2天 |
| 外部依赖项目 | 检查其他项目引用 | 1天 |
| 文档更新 | 更新API文档 | 0.5天 |

---

## 九、验收标准

### 9.1 编译验收

- [ ] 所有模块独立编译通过
- [ ] 无编译警告
- [ ] 无链接错误
- [ ] **通过循环依赖检测工具验证**

### 9.2 测试验收

- [ ] 所有单元测试通过
- [ ] 所有集成测试通过
- [ ] 测试覆盖率不低于拆分前
- [ ] **回归测试通过（核心功能无回归）**
- [ ] **性能基准测试通过（性能无明显下降）**

### 9.3 架构验收

- [ ] 模块依赖关系符合分层设计
- [ ] 无循环依赖
- [ ] 头文件路径正确
- [ ] **依赖矩阵为下三角矩阵**

---

## 十、性能影响评估

### 10.1 编译时间影响

| 指标 | 拆分前 | 拆分后 | 变化 |
|------|--------|--------|------|
| 全量编译时间 | 基准 | +5%~10% | 略增 |
| 增量编译时间 | 基准 | -20%~30% | 改善 |
| 并行编译效率 | 低 | 高 | 改善 |

**原因分析**：
- 全量编译增加：模块数量增加，CMake配置开销
- 增量编译改善：修改单个模块只需重新编译该模块

### 10.2 运行时影响

| 指标 | 拆分前 | 拆分后 | 变化 |
|------|--------|--------|------|
| 动态库数量 | 1个 | 5个 | 增加 |
| 动态库加载时间 | 基准 | +10~50ms | 略增 |
| 内存占用 | 基准 | +1%~2% | 略增 |
| 运行时性能 | 基准 | 无变化 | 无影响 |

**优化建议**：
- 使用静态链接减少动态库数量
- 延迟加载非核心模块

### 10.3 性能基准测试

拆分前后需对比以下指标：

| 测试项 | 测试方法 | 验收标准 |
|--------|----------|----------|
| 瓦片渲染性能 | 渲染1000个瓦片耗时 | 变化 < 5% |
| 样式解析性能 | 解析100个SLD文件耗时 | 变化 < 5% |
| 内存占用 | 峰值内存使用 | 变化 < 5% |
| 启动时间 | 应用启动耗时 | 变化 < 10% |

---

## 十一、附录

### 11.1 文件统计

| 模块 | 头文件 | 源文件 | 测试文件 | 合计 |
|------|--------|--------|----------|------|
| ogc_base | 5(含export.h) | 4 | 4 | 13 |
| ogc_proj | 5(含export.h) | 4 | 4 | 13 |
| ogc_cache | 10(含export.h) | 9 | 9 | 28 |
| ogc_symbology | 19(含export.h) | 18 | 18 | 55 |
| ogc_graph | 23(含export.h) | 22 | 20+ | 65+ |
| ogc_draw (+1) | +1 | +1 | +1 | +3 |
| ogc_navi (+1) | +1 | +1 | +1 | +3 |

> **说明**: 头文件数包含export.h导出宏定义文件

### 11.2 命名空间

| 模块 | 命名空间 | 原命名空间 |
|------|----------|------------|
| ogc_base | `ogc::base` | `ogc::draw` |
| ogc_proj | `ogc::proj` | `ogc::draw` |
| ogc_cache | `ogc::cache` | `ogc::draw` |
| ogc_symbology | `ogc::symbology` | `ogc::draw` |
| ogc_graph | `ogc::graph` | `ogc::draw` |

### 11.3 命名空间迁移指南

#### 迁移步骤

1. **更新头文件包含**
```cpp
// 旧代码
#include <ogc/draw/log.h>

// 新代码
#include <ogc/base/log.h>
```

2. **更新命名空间引用**
```cpp
// 旧代码
using ogc::draw::Log;

// 新代码
using ogc::base::Log;
```

3. **更新类名引用**
```cpp
// 旧代码
ogc::draw::Log::Info("message");

// 新代码
ogc::base::Log::Info("message");
```

#### 批量替换命令

```bash
# 替换头文件路径
find . -name "*.h" -o -name "*.cpp" | xargs sed -i 's|<ogc/draw/log.h>|<ogc/base/log.h>|g'
find . -name "*.h" -o -name "*.cpp" | xargs sed -i 's|<ogc/draw/coordinate_transform.h>|<ogc/proj/coordinate_transform.h>|g'
find . -name "*.h" -o -name "*.cpp" | xargs sed -i 's|<ogc/draw/tile_cache.h>|<ogc/cache/tile/tile_cache.h>|g'

# 替换命名空间
find . -name "*.h" -o -name "*.cpp" | xargs sed -i 's|ogc::draw::Log|ogc::base::Log|g'
find . -name "*.h" -o -name "*.cpp" | xargs sed -i 's|ogc::draw::CoordinateTransform|ogc::proj::CoordinateTransform|g'
```

---

**版本历史**:

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v1.0 | 2026-04-06 | 初始版本 |
| v1.1 | 2026-04-06 | 根据评审报告改进：增加循环依赖检测、回滚方案、并行开发策略、接口兼容性说明、测试策略完善、性能影响评估、命名空间迁移指南 |
