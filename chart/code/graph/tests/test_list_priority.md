# Graph模块单元测试优先级列表

## 概述

本文档基于海图数据显示的核心需求，对graph模块已实现类进行单元测试优先级评估。

### 海图数据显示核心流程
```
数据加载 -> 坐标转换 -> 样式解析 -> 符号化渲染 -> 瓦片缓存 -> 图像输出
```

### 优先级定义
- **P0-关键**: 核心渲染路径，直接影响数据显示
- **P1-重要**: 重要功能模块，影响渲染质量或性能
- **P2-一般**: 辅助功能模块，提升用户体验
- **P3-低**: 可选功能，不影响核心流程

---

## 一、已完成测试

### P0级别 - 基础类型

| 测试文件 | 测试类 | 状态 | 说明 |
|---------|--------|------|------|
| test_color.cpp | Color | ✅ 已完成 | 颜色是渲染基础 |
| test_font.cpp | Font | ✅ 已完成 | 字体是标注基础 |
| test_draw_style.cpp | DrawStyle, StrokeStyle, FillStyle | ✅ 已完成 | 样式是渲染核心 |
| test_draw_params.cpp | DrawParams | ✅ 已完成 | 绘图参数配置 |
| test_draw_result.cpp | DrawResult | ✅ 已完成 | 结果状态枚举 |
| test_draw_error.cpp | DrawError | ✅ 已完成 | 错误处理 |
| test_device_type.cpp | DeviceType | ✅ 已完成 | 设备类型枚举 |
| test_engine_type.cpp | EngineType | ✅ 已完成 | 引擎类型枚举 |
| test_transform_matrix.cpp | TransformMatrix | ✅ 已完成 | 坐标变换矩阵 |

### 第一阶段 - 核心渲染路径（P0）

| 测试文件 | 测试类 | 状态 | 说明 |
|---------|--------|------|------|
| test_tile_key.cpp | TileKey | ✅ 已完成 | 瓦片索引核心，海图瓦片定位必需 |
| test_raster_image_device.cpp | RasterImageDevice | ✅ 已完成 | 图像输出设备，最终渲染目标 |
| test_draw_context.cpp | DrawContext | ✅ 已完成 | 绘图上下文，渲染状态管理 |
| test_tile_device.cpp | TileDevice | ✅ 已完成 | 瓦片设备，瓦片渲染核心 |
| test_coordinate_transform.cpp | CoordinateTransform | ✅ 已完成 | 坐标转换，地理坐标->屏幕坐标 |

### 第二阶段 - 渲染质量与性能（P1）

| 测试文件 | 测试类 | 状态 | 说明 |
|---------|--------|------|------|
| test_tile_cache.cpp | TileCache, MemoryTileCache | ✅ 已完成 | 瓦片缓存接口与内存缓存 |
| test_disk_tile_cache.cpp | DiskTileCache | ✅ 已完成 | 磁盘缓存，持久化缓存 |
| test_multi_level_tile_cache.cpp | MultiLevelTileCache | ✅ 已完成 | 多级缓存，优化缓存策略 |
| test_tile_renderer.cpp | TileRenderer | ✅ 已完成 | 瓦片渲染器，单瓦片渲染 |
| test_symbolizer.cpp | Symbolizer | ✅ 已完成 | 符号化基类，渲染规则基础 |
| test_line_symbolizer.cpp | LineSymbolizer | ✅ 已完成 | 线符号化，航道/海岸线渲染 |
| test_polygon_symbolizer.cpp | PolygonSymbolizer | ✅ 已完成 | 面符号化，海域/陆地渲染 |
| test_point_symbolizer.cpp | PointSymbolizer | ✅ 已完成 | 点符号化，灯塔/浮标渲染 |
| test_text_symbolizer.cpp | TextSymbolizer | ✅ 已完成 | 文字符号化，地名/标注渲染 |

### 第三阶段 - 辅助功能（P2）

| 测试文件 | 测试类 | 状态 | 说明 |
|---------|--------|------|------|
| test_device_state.cpp | DeviceState | ✅ 已完成 | 设备状态管理 |
| test_image_draw.cpp | ImageDraw, ImageData | ✅ 已完成 | 图像绘制辅助 |
| test_clipper.cpp | Clipper | ✅ 已完成 | 裁剪算法，多边形裁剪 |
| test_label_placement.cpp | LabelPlacement | ✅ 已完成 | 标注位置计算 |
| test_label_conflict.cpp | LabelConflict | ✅ 已完成 | 标注冲突检测 |
| test_label_engine.cpp | LabelEngine | ✅ 已完成 | 标注引擎 |
| test_layer_manager.cpp | LayerManager | ✅ 已完成 | 图层管理 |
| test_transform_manager.cpp | TransformManager | ✅ 已完成 | 变换管理器 |
| test_raster_symbolizer.cpp | RasterSymbolizer | ✅ 已完成 | 栅格符号化 |
| test_icon_symbolizer.cpp | IconSymbolizer | ✅ 已完成 | 图标符号化 |

---

## 二、待实现测试（按优先级排序）

### P0-关键：核心渲染路径（未完成）

| 优先级 | 测试目标 | 依赖关系 | 海图显示影响 | 预计工作量 | 状态 |
|--------|----------|----------|--------------|------------|------|
| P0-4 | DrawEngine | DrawDevice, DrawContext | 绘图引擎，渲染流程核心 | 2h | ⏸️ 暂缓（抽象类） |
| P0-7 | DrawDriver | DrawDevice, DrawEngine | 驱动管理，设备创建 | 1h | ⏸️ 暂缓（抽象类） |

### P2-一般：辅助功能（未完成）

| 优先级 | 测试目标 | 依赖关系 | 海图显示影响 | 预计工作量 | 状态 |
|--------|----------|----------|--------------|------------|------|
| （无） | - | - | - | - | - |

### P3-低：可选功能

| 优先级 | 测试目标 | 依赖关系 | 海图显示影响 | 状态 | 测试文件 |
|--------|----------|----------|--------------|------|----------|
| P3-1 | Filter | 无 | 过滤器基类 | ✅ 已完成 | test_filter.cpp |
| P3-2 | ComparisonFilter | Filter | 比较过滤器 | ✅ 已完成 | test_comparison_filter.cpp |
| P3-3 | LogicalFilter | Filter | 逻辑过滤器 | ✅ 已完成 | test_logical_filter.cpp |
| P3-4 | SpatialFilter | Filter | 空间过滤器 | ✅ 已完成 | test_spatial_filter.cpp |
| P3-5 | RuleEngine | Filter, Symbolizer | 规则引擎 | ✅ 已完成 | test_rule_engine.cpp |
| P3-6 | SymbolizerRule | Symbolizer | 符号化规则 | ✅ 已完成 | test_symbolizer_rule.cpp |
| P3-7 | CompositeSymbolizer | Symbolizer | 组合符号化 | ✅ 已完成 | test_composite_symbolizer.cpp |
| P3-8 | SldParser | RuleEngine | SLD样式解析 | ⏸️ 待实现 | - |
| P3-9 | MapboxStyleParser | RuleEngine | Mapbox样式解析 | ⏸️ 待实现 | - |
| P3-10 | Lod | 无 | 细节层次 | ✅ 已完成 | test_lod.cpp |
| P3-11 | LodManager | Lod | LOD管理器 | ✅ 已完成 | test_lod_manager.cpp |
| P3-12 | PerformanceMetrics | 无 | 性能指标 | ✅ 已完成 | test_performance_metrics.cpp |
| P3-13 | PerformanceMonitor | PerformanceMetrics | 性能监控 | ✅ 已完成 | test_performance_monitor.cpp |
| P3-14 | RenderTask | 无 | 渲染任务 | ✅ 已完成 | test_render_task.cpp |
| P3-15 | RenderQueue | RenderTask | 渲染队列 | ✅ 已完成 | test_render_queue.cpp |
| P3-16 | AsyncRenderer | RenderQueue | 异步渲染器 | ✅ 已完成 | test_async_renderer.cpp |
| P3-17 | BasicRenderTask | RenderTask | 基础渲染任务 | ✅ 已完成 | test_basic_render_task.cpp |
| P3-18 | DisplayDevice | DrawDevice | 显示设备 | ✅ 已完成 | test_display_device.cpp |
| P3-19 | PdfDevice | DrawDevice | PDF设备 | ✅ 已完成 | test_pdf_device.cpp |
| P3-20 | DrawFacade | DrawEngine | 绘图门面 | ✅ 已完成 | test_draw_facade.cpp |
| P3-21 | DriverManager | DrawDriver | 驱动管理器 | ✅ 已完成 | test_driver_manager.cpp |
| P3-22 | ProjTransformer | 无 | 投影转换 | ✅ 已完成 | test_proj_transformer.cpp |
| P3-23 | CoordinateTransformer | CoordinateTransform, ProjTransformer | 坐标转换器 | ✅ 已完成 | test_coordinate_transformer.cpp |
| P3-24 | ThreadSafe | 无 | 线程安全工具 | ✅ 已完成 | test_thread_safe.cpp |
| P3-25 | Log | 无 | 日志工具 | ✅ 已完成 | test_log.cpp |

---

## 四、集成测试优先级列表

### 集成测试概述
集成测试验证多个模块协同工作的正确性，是确保海图数据显示功能完整性的关键。

### 海图显示集成测试场景
```
场景1: 基础渲染流程 - DrawEngine + RasterImageDevice
场景2: 瓦片渲染流程 - TileRenderer + TileDevice + TileCache
场景3: 符号化渲染 - Symbolizer + DrawContext + Geometry
场景4: 完整显示流程 - 数据加载 -> 渲染 -> 输出
```

### P0-关键：核心渲染集成

| 优先级 | 测试场景 | 涉及模块 | 验证目标 | 状态 | 测试文件 |
|--------|----------|----------|----------|------|----------|
| P0-IT1 | 基础图形渲染 | DrawEngine + RasterImageDevice | 点、线、面基本图形正确渲染 | ✅ 已完成 | test_it_basic_render.cpp |
| P0-IT2 | 坐标变换渲染 | TransformMatrix + DrawEngine + RasterImageDevice | 平移、缩放、旋转变换正确 | ✅ 已完成 | test_it_transform_render.cpp |
| P0-IT3 | 瓦片索引定位 | TileKey + TileDevice | 瓦片坐标计算与定位正确 | ✅ 已完成 | test_it_tile_index.cpp |
| P0-IT4 | 绘图上下文管理 | DrawContext + DrawDevice | 状态保存/恢复、裁剪正确 | ✅ 已完成 | test_it_draw_context.cpp |
| P0-IT5 | 图像输出验证 | RasterImageDevice | 输出PNG/JPEG图像正确 | ✅ 已完成 | test_it_image_output.cpp |

### P1-重要：渲染质量集成

| 优先级 | 测试场景 | 涉及模块 | 验证目标 | 状态 | 测试文件 |
|--------|----------|----------|----------|------|----------|
| P1-IT1 | 瓦片渲染流程 | TileRenderer + TileDevice + DrawEngine | 单瓦片完整渲染流程 | ✅ 已完成 | test_it_tile_render.cpp |
| P1-IT2 | 内存缓存集成 | MemoryTileCache + TileRenderer | 缓存命中/未命中处理 | ✅ 已完成 | test_it_memory_cache.cpp |
| P1-IT3 | 磁盘缓存集成 | DiskTileCache + TileRenderer | 持久化缓存读写 | ✅ 已完成 | test_it_disk_cache.cpp |
| P1-IT4 | 多级缓存集成 | MultiLevelTileCache + TileRenderer | 缓存层级查找正确 | ✅ 已完成 | test_it_multi_level_cache.cpp |
| P1-IT5 | 线符号化渲染 | LineSymbolizer + DrawContext + Geometry | 航道、海岸线渲染正确 | ✅ 已完成 | test_it_line_symbolizer.cpp |
| P1-IT6 | 面符号化渲染 | PolygonSymbolizer + DrawContext + Geometry | 海域、陆地渲染正确 | ✅ 已完成 | test_it_polygon_symbolizer.cpp |
| P1-IT7 | 点符号化渲染 | PointSymbolizer + DrawContext + Geometry | 灯塔、浮标渲染正确 | ✅ 已完成 | test_it_point_symbolizer.cpp |
| P1-IT8 | 文字标注渲染 | TextSymbolizer + Font + DrawContext | 地名、标注渲染正确 | ✅ 已完成 | test_it_text_symbolizer.cpp |

### P2-一般：功能集成

| 优先级 | 测试场景 | 涉及模块 | 验证目标 | 状态 | 测试文件 |
|--------|----------|----------|----------|------|----------|
| P2-IT1 | 图层管理渲染 | LayerManager + DrawEngine | 多图层叠加渲染正确 | ✅ 已完成 | test_it_layer_manager.cpp |
| P2-IT2 | 标注冲突处理 | LabelEngine + LabelConflict | 标注避让正确 | ✅ 已完成 | test_it_label_conflict.cpp |
| P2-IT3 | 裁剪渲染 | Clipper + DrawDevice | 多边形裁剪渲染正确 | ✅ 已完成 | test_it_clipper.cpp |
| P2-IT4 | 图标符号化 | IconSymbolizer + ImageDraw | 图标渲染正确 | ✅ 已完成 | test_it_icon_symbolizer.cpp |
| P2-IT5 | 栅格符号化 | RasterSymbolizer + DrawDevice | 栅格数据渲染正确 | ✅ 已完成 | test_it_raster_symbolizer.cpp |

### P3-低：高级功能集成

| 优先级 | 测试场景 | 涉及模块 | 验证目标 | 状态 | 测试文件 |
|--------|----------|----------|----------|------|----------|
| P3-IT1 | 异步渲染流程 | AsyncRenderer + RenderQueue | 异步渲染正确完成 | ✅ 已完成 | test_it_async_render.cpp |
| P3-IT2 | 规则引擎渲染 | RuleEngine + Symbolizer + Filter | 条件渲染正确 | ✅ 已完成 | test_it_rule_engine_render.cpp |
| P3-IT3 | SLD样式渲染 | SldParser + RuleEngine + Symbolizer | SLD样式应用正确 | ✅ 已完成 | test_it_sld_render.cpp |
| P3-IT4 | Mapbox样式渲染 | MapboxStyleParser + RuleEngine | Mapbox样式应用正确 | ✅ 已完成 | test_it_mapbox_style_render.cpp |
| P3-IT5 | LOD渲染 | LodManager + TileRenderer | 多层次细节渲染正确 | ✅ 已完成 | test_it_lod_render.cpp |
| P3-IT6 | 性能监控集成 | PerformanceMonitor + DrawEngine | 性能指标采集正确 | ✅ 已完成 | test_it_performance_monitor.cpp |
| P3-IT7 | 投影转换渲染 | ProjTransformer + CoordinateTransformer + DrawEngine | 投影转换渲染正确 | ✅ 已完成 | test_it_proj_transform_render.cpp |
| P3-IT8 | PDF输出 | PdfDevice + DrawEngine | PDF输出正确 | ✅ 已完成 | test_it_pdf_output.cpp |
| P3-IT9 | 显示设备集成 | DisplayDevice + DrawEngine | 屏幕显示正确 | ✅ 已完成 | test_it_display_device.cpp |
| P3-IT10 | 门面接口集成 | DrawFacade + DrawEngine + DrawDevice | 高层接口正确 | ✅ 已完成 | test_it_draw_facade.cpp |

---

## 五、测试实施建议

### 单元测试实施阶段

#### 第一阶段：核心渲染路径（P0）
**目标**: 实现基本的海图数据显示能力

1. TileKey - 瓦片索引基础
2. RasterImageDevice - 图像输出
3. DrawContext - 绘图上下文
4. DrawEngine - 绘图引擎
5. TileDevice - 瓦片设备
6. CoordinateTransform - 坐标转换
7. DrawDriver - 驱动管理

**预计总工作量**: 约10小时

#### 第二阶段：渲染质量与性能（P1）
**目标**: 提升渲染质量和性能

1. 缓存系统（TileCache系列）
2. 瓦片渲染器
3. 符号化系统（Symbolizer系列）

**预计总工作量**: 约14小时

#### 第三阶段：辅助功能（P2）
**目标**: 完善标注和图层管理

**预计总工作量**: 约10小时

#### 第四阶段：可选功能（P3）
**目标**: 样式解析、性能优化等

**预计总工作量**: 约20小时

### 集成测试实施阶段

#### 第一阶段：核心渲染集成（P0-IT）
**目标**: 验证基础渲染流程正确性

| 测试项 | 依赖单元测试 | 验证方法 |
|--------|--------------|----------|
| P0-IT1 基础图形渲染 | DrawEngine, RasterImageDevice | 图像像素比对 |
| P0-IT2 坐标变换渲染 | TransformMatrix, DrawEngine | 坐标计算验证 |
| P0-IT3 瓦片索引定位 | TileKey, TileDevice | 索引计算验证 |
| P0-IT4 绘图上下文管理 | DrawContext, DrawDevice | 状态验证 |
| P0-IT5 图像输出验证 | RasterImageDevice | 文件输出验证 |

**预计总工作量**: 约7小时

#### 第二阶段：渲染质量集成（P1-IT）
**目标**: 验证瓦片渲染和符号化正确性

| 测试项 | 依赖单元测试 | 验证方法 |
|--------|--------------|----------|
| P1-IT1 瓦片渲染流程 | TileRenderer, TileDevice | 瓦片图像验证 |
| P1-IT2~IT4 缓存集成 | TileCache系列 | 缓存命中率验证 |
| P1-IT5~IT8 符号化渲染 | Symbolizer系列 | 渲染结果验证 |

**预计总工作量**: 约13小时

#### 第三阶段：功能集成（P2-IT）
**目标**: 验证辅助功能集成正确性

**预计总工作量**: 约6小时

#### 第四阶段：高级功能集成（P3-IT）
**目标**: 验证高级功能集成正确性

**预计总工作量**: 约15小时

---

## 六、测试覆盖策略

### 单元测试覆盖要求
- **DrawDevice系列**: 测试所有绘图操作、状态管理、错误处理
- **Symbolizer系列**: 测试样式应用、几何渲染、属性绑定
- **TileCache系列**: 测试缓存命中/未命中、容量限制、线程安全

### 集成测试覆盖要求
- **渲染流程**: 验证从数据到图像的完整流程
- **缓存机制**: 验证多级缓存协同工作
- **符号化系统**: 验证样式规则正确应用

### 测试数据准备
1. 简单几何数据（点、线、面）
2. 海图要素数据（灯塔、航道、海域）
3. 样式配置文件（SLD/Mapbox）
4. 预期输出图像（用于比对）

---

## 七、更新记录

| 日期 | 更新内容 | 更新人 |
|------|----------|--------|
| 2026-03-22 | 创建文档，完成P0级别基础单元测试 | AI |
| 2026-03-22 | 添加集成测试优先级列表 | AI |
| 2026-03-22 | 完成第一阶段单元测试：TileKey, RasterImageDevice, DrawContext, TileDevice, CoordinateTransform | AI |
| 2026-03-22 | 完成第二阶段单元测试：TileCache, MemoryTileCache, DiskTileCache, MultiLevelTileCache, TileRenderer, Symbolizer系列 | AI |
| 2026-03-22 | 完成第三阶段单元测试：DeviceState, ImageDraw | AI |
| 2026-03-22 | 完成P0集成测试：基础渲染、坐标变换、瓦片索引、绘图上下文、图像输出 | AI |
| 2026-03-22 | 完成P1集成测试：瓦片渲染、内存缓存、磁盘缓存、多级缓存、线符号化 | AI |
| 2026-03-23 | 完成P2单元测试：Clipper, LabelPlacement, LabelConflict, LabelEngine, LayerManager, TransformManager, RasterSymbolizer, IconSymbolizer | AI |
| 2026-03-23 | 完成P2集成测试：图层管理、标注冲突处理、裁剪渲染、 图标符号化、 栅格符号化 | AI |
| 2026-03-23 | 完成P1集成测试：面符号化、点符号化、文字标注渲染 | AI |
| 2026-03-23 | 完成P2集成测试：图层管理、标注冲突处理、裁剪渲染、图标符号化、栅格符号化 | AI |
| 2026-03-23 | 完成P3单元测试：Filter, ComparisonFilter, LogicalFilter, SpatialFilter, RuleEngine | AI |
| 2026-03-23 | 完成P3单元测试：SymbolizerRule, CompositeSymbolizer, Lod, LodManager, PerformanceMetrics, PerformanceMonitor, RenderTask, RenderQueue | AI |
| 2026-03-23 | 完成P3单元测试：DriverManager, CoordinateTransformer, ThreadSafe, Log | AI |
| 2026-03-23 | 完成P3单元测试：AsyncRenderer, BasicRenderTask, DisplayDevice, PdfDevice, DrawFacade | AI |
| 2026-03-23 | 完成P3单元测试：ProjTransformer | AI |
| 2026-03-24 | 完成P3集成测试：异步渲染、规则引擎渲染、LOD渲染、性能监控、投影转换、PDF输出、门面接口 | AI |
| 2026-03-24 | 完成P3集成测试：SLD样式渲染、Mapbox样式渲染、显示设备集成 | AI |
