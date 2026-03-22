# 图形绘制框架任务计划 - 核心模块

**版本**: 5.0  
**日期**: 2026年3月21日  
**分类**: 核心模块 (Core Modules)

---

## 一、核心基础设施 (Core Infrastructure)

### 1.1 公共定义模块

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和错误码定义 | P0 | M1 | 6h | 📋 Todo | - |
| T1.2 | 设备类型和引擎类型枚举 | P0 | M1 | 4h | 📋 Todo | T1.1 |
| T1.3 | DrawParams绘制参数结构 | P0 | M1 | 8h | 📋 Todo | T1.1 |
| T1.4 | DrawStyle绘制样式结构 | P0 | M1 | 8h | 📋 Todo | T1.1 |
| T1.5 | 类型别名和前向声明 | P0 | M1 | 2h | 📋 Todo | T1.1 |
| T1.6 | 错误处理机制 | P0 | M1 | 6h | 📋 Todo | T1.1 |

#### T1.1 - 枚举和错误码定义

**Description**: 定义图形绘制模块使用的所有枚举类型

**Acceptance Criteria**
- [ ] DrawResult错误码枚举（kSuccess, kFailed, kInvalidParams等）
- [ ] DeviceState设备状态枚举（kCreated, kInitialized, kActive等）
- [ ] getResultString()和getResultDescription()函数
- [ ] 所有枚举使用enum class
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:4h, M:6h, P:10h → Expected: 6.3h

---

#### T1.2 - 设备类型和引擎类型枚举

**Description**: 定义设备类型(DeviceType)和引擎类型(EngineType)枚举

**Acceptance Criteria**
- [ ] DeviceType枚举（kRasterImage, kDisplayDevice, kPdfDevice等）
- [ ] EngineType枚举（kQtEngine, kCairoEngine, kGdiEngine等）
- [ ] ImageFormat枚举（PNG, JPEG, BMP, TIFF等）
- [ ] VectorFormat枚举（SVG, DXF, PDF, EPS等）
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:3h, M:4h, P:6h → Expected: 4.2h

---

#### T1.3 - DrawParams绘制参数结构

**Description**: 实现绘制参数结构，包含空间范围、逻辑尺寸、DPI设置等

**Acceptance Criteria**
- [ ] 空间范围参数(extentMinX/Y, extentMaxX/Y)
- [ ] 逻辑尺寸和物理尺寸计算
- [ ] DPI比率计算(getDpiRatio)
- [ ] GIS扩展字段(sourceSrs, targetSrs, enableLOD等)
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:6h, M:8h, P:12h → Expected: 8.3h

---

#### T1.4 - DrawStyle绘制样式结构

**Description**: 实现绘制样式结构，包含边框样式、填充样式、透明度等

**Acceptance Criteria**
- [ ] 边框样式(strokeColor, strokeWidth, strokeCap, strokeJoin)
- [ ] 填充样式(fillColor, fillRule, fillPattern)
- [ ] 透明度设置(opacity, strokeOpacity, fillOpacity)
- [ ] 混合模式(blendMode)
- [ ] 静态工厂方法(solid, strokeOnly, fillOnly)
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:6h, M:8h, P:12h → Expected: 8.3h

---

#### T1.5 - 类型别名和前向声明

**Description**: 定义智能指针类型别名和前向声明

**Acceptance Criteria**
- [ ] DrawDevicePtr, DrawEnginePtr, DrawDriverPtr等智能指针别名
- [ ] 前向声明DrawDevice, DrawEngine, DrawDriver等类
- [ ] 遵循命名空间规范(ogc::draw)

**Estimated Effort**: O:1h, M:2h, P:4h → Expected: 2.2h

---

#### T1.6 - 错误处理机制

**Description**: 实现错误处理机制，包括DrawError结构、ErrorHandlingPolicy策略

**Acceptance Criteria**
- [ ] DrawError错误信息结构
- [ ] ErrorHandlingPolicy策略枚举
- [ ] DrawException异常类
- [ ] DRAW_CHECK、DRAW_SET_ERROR宏定义
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:4h, M:6h, P:10h → Expected: 6.3h

---

### 1.2 工具类模块

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T2.1 | TransformMatrix变换矩阵 | P0 | M1 | 10h | 📋 Todo | T1.3 |
| T2.2 | Color颜色类 | P1 | M1 | 8h | 📋 Todo | T1.1 |
| T2.3 | Font字体类 | P1 | M1 | 6h | 📋 Todo | T1.1 |
| T2.4 | 日志系统 | P1 | M1 | 8h | 📋 Todo | T1.1 |

#### T2.1 - TransformMatrix变换矩阵

**Description**: 实现2D变换矩阵，支持平移、旋转、缩放、组合变换

**Acceptance Criteria**
- [ ] 单位矩阵、平移矩阵、缩放矩阵、旋转矩阵
- [ ] 矩阵乘法和点变换
- [ ] 矩阵求逆(inverse)
- [ ] 判断是否为单位矩阵(isIdentity)
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T2.2 - Color颜色类

**Description**: 实现颜色类，支持RGB/RGBA/HSV等颜色模型

**Acceptance Criteria**
- [ ] RGB/RGBA构造和转换
- [ ] HSV/HSL转换
- [ ] 颜色混合
- [ ] 预定义颜色常量
- [ ] 颜色解析（#RRGGBB格式）
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:6h, M:8h, P:12h → Expected: 8.3h

---

#### T2.3 - Font字体类

**Description**: 实现字体类，支持字体属性设置和字体度量

**Acceptance Criteria**
- [ ] 字体名称、大小、样式属性
- [ ] 粗体、斜体、下划线设置
- [ ] 字体度量信息
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:4h, M:6h, P:10h → Expected: 6.3h

---

#### T2.4 - 日志系统

**Description**: 实现日志系统，支持多级别日志、多输出目标

**Acceptance Criteria**
- [ ] LogLevel枚举（kTrace, kDebug, kInfo等）
- [ ] ConsoleLogger控制台日志器
- [ ] FileLogger文件日志器
- [ ] LogManager日志管理器
- [ ] 便捷宏（LOG_INFO, LOG_ERROR等）
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:6h, M:8h, P:12h → Expected: 8.3h

---

### 1.3 构建配置

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T21.1 | CMakeLists.txt配置 | P0 | M1 | 4h | 📋 Todo | - |
| T21.2 | 目录结构创建 | P0 | M1 | 2h | 📋 Todo | - |

#### T21.1 - CMakeLists.txt配置

**Description**: 配置CMake构建系统，支持跨平台编译

**Acceptance Criteria**
- [ ] CMakeLists.txt主配置文件
- [ ] 支持Windows/Linux/macOS编译
- [ ] 依赖geom, feature, layer模块
- [ ] 无编译警告

**Estimated Effort**: O:3h, M:4h, P:6h → Expected: 4.2h

---

#### T21.2 - 目录结构创建

**Description**: 创建模块目录结构

**Acceptance Criteria**
- [ ] include/ogc/draw目录结构
- [ ] src源代码目录
- [ ] tests测试目录
- [ ] 符合项目规范

**Estimated Effort**: O:1h, M:2h, P:4h → Expected: 2.2h

---

## 二、设备层 (Device Layer)

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T3.1 | DrawDevice设备基类 | P0 | M2 | 14h | 📋 Todo | T1.3, T2.1 |
| T3.2 | RasterImageDevice栅格图像设备 | P0 | M2 | 12h | 📋 Todo | T3.1 |
| T3.3 | PdfDevice PDF设备 | P1 | M2 | 14h | 📋 Todo | T3.1 |
| T3.4 | TileDevice瓦片设备 | P1 | M2 | 10h | 📋 Todo | T3.1 |
| T3.5 | DisplayDevice显示设备 | P1 | M2 | 10h | 📋 Todo | T3.1 |
| T3.6 | 线程安全设计 | P0 | M2 | 8h | 📋 Todo | T3.1 |

#### T3.1 - DrawDevice设备基类

**Description**: 实现绘制设备抽象基类，管理输出目标和设备生命周期

**Acceptance Criteria**
- [ ] 设备类型和状态管理(getType, getState)
- [ ] 设备信息接口(getWidth, getHeight, getDpi)
- [ ] 生命周期接口(initialize, beginDraw, endDraw, clear)
- [ ] 文件保存接口(saveToFile)
- [ ] 线程安全(getLock, tryGetLock)
- [ ] 引擎关联(getEngine, setEngine)
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:10h, M:14h, P:20h → Expected: 14.3h

---

#### T3.2 - RasterImageDevice栅格图像设备

**Description**: 实现栅格图像设备，支持PNG/JPEG/BMP等格式输出

**Acceptance Criteria**
- [ ] 创建栅格图像设备(create工厂方法)
- [ ] 图像数据管理(getImageData, getImageDataSize)
- [ ] 像素操作(getPixel, setPixel)
- [ ] 多格式保存(PNG, JPEG, BMP)
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:12h, P:18h → Expected: 12.3h

---

#### T3.3 - PdfDevice PDF设备

**Description**: 实现PDF设备，支持PDF文档输出和多页面管理

**Acceptance Criteria**
- [ ] 创建PDF设备(create工厂方法)
- [ ] 多页面管理(addPage, setPageIndex, getPageCount)
- [ ] PDF元数据设置(setMetadata)
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:10h, M:14h, P:20h → Expected: 14.3h

---

#### T3.4 - TileDevice瓦片设备

**Description**: 实现瓦片设备，支持瓦片渲染和瓦片数据管理

**Acceptance Criteria**
- [ ] 创建瓦片设备(create工厂方法)
- [ ] 瓦片键管理(getTileKey, setTileKey)
- [ ] 瓦片数据获取(getTileData, getTileDataSize)
- [ ] 瓦片金字塔关联(setPyramid)
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T3.5 - DisplayDevice显示设备

**Description**: 实现显示设备，支持屏幕和窗口输出

**Acceptance Criteria**
- [ ] 创建显示设备(create工厂方法)
- [ ] 原生窗口关联(setNativeWindow)
- [ ] 双缓冲支持
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T3.6 - 线程安全设计

**Description**: 实现线程安全保证，包括设备锁、并发使用模式

**Acceptance Criteria**
- [ ] 设备锁机制(getLock, tryGetLock)
- [ ] 线程安全文档和示例
- [ ] DevicePool设备资源池(可选)
- [ ] ThreadSanitizer测试通过

**Estimated Effort**: O:6h, M:8h, P:12h → Expected: 8.3h

---

## 三、引擎层 (Engine Layer)

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T4.1 | DrawEngine引擎基类 | P0 | M3 | 18h | 📋 Todo | T3.1 |
| T4.2 | 坐标变换和裁剪接口 | P0 | M3 | 10h | 📋 Todo | T4.1 |
| T4.3 | 图层管理接口 | P1 | M3 | 8h | 📋 Todo | T4.1 |
| T4.4 | 图像绘制接口 | P1 | M3 | 6h | 📋 Todo | T4.1 |

#### T4.1 - DrawEngine引擎基类

**Description**: 实现绘制引擎抽象基类，封装具体图形库的绘制逻辑

**Acceptance Criteria**
- [ ] 引擎类型和状态管理(getType, isInitialized)
- [ ] 设备关联(getDevice)
- [ ] 基本图元绘制(drawPoint, drawLine, drawPolyline, drawPolygon)
- [ ] 几何对象绘制(drawGeometry, drawFeature)
- [ ] 文本绘制(drawText)
- [ ] 样式管理(setStyle, saveState, restoreState)
- [ ] 坐标变换(worldToDevice, deviceToWorld, setTransform)
- [ ] 裁剪接口(setClipRect, setClipGeometry, clearClip)
- [ ] 图层管理(createLayer, setActiveLayer, setLayerOpacity)
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:14h, M:18h, P:24h → Expected: 18.3h

---

#### T4.2 - 坐标变换和裁剪接口

**Description**: 实现坐标变换和裁剪功能

**Acceptance Criteria**
- [ ] 世界坐标到设备坐标转换
- [ ] 设备坐标到世界坐标转换
- [ ] 矩形裁剪
- [ ] 几何裁剪
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T4.3 - 图层管理接口

**Description**: 实现图层管理功能，支持多图层绘制

**Acceptance Criteria**
- [ ] 图层创建(createLayer)
- [ ] 图层激活(setActiveLayer)
- [ ] 图层透明度(setLayerOpacity)
- [ ] 图层合并(mergeLayer)
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:6h, M:8h, P:12h → Expected: 8.3h

---

#### T4.4 - 图像绘制接口

**Description**: 实现图像绘制功能，支持图片加载和绘制

**Acceptance Criteria**
- [ ] 图像加载(loadImage)
- [ ] 图像绘制(drawImage)
- [ ] 图像缩放和旋转
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:4h, M:6h, P:10h → Expected: 6.3h

---

## 四、驱动层 (Driver Layer)

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T5.1 | DrawDriver驱动基类 | P0 | M3 | 12h | 📋 Todo | T4.1 |
| T5.2 | DriverManager驱动管理器 | P0 | M3 | 10h | 📋 Todo | T5.1 |
| T6.1 | DrawContext绘制上下文 | P0 | M3 | 14h | 📋 Todo | T4.1, T5.1 |
| T6.2 | DrawFacade门面类 | P0 | M3 | 12h | 📋 Todo | T6.1 |

#### T5.1 - DrawDriver驱动基类

**Description**: 实现绘制驱动抽象基类，协调设备和引擎的创建

**Acceptance Criteria**
- [ ] 驱动类型和名称管理
- [ ] 设备创建(createDevice)
- [ ] 引擎创建(createEngine)
- [ ] 设备-引擎绑定(bind)
- [ ] 驱动初始化和销毁
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:10h, M:12h, P:16h → Expected: 12.3h

---

#### T5.2 - DriverManager驱动管理器

**Description**: 实现驱动管理器，支持驱动注册、查找和生命周期管理

**Acceptance Criteria**
- [ ] 驱动注册(registerDriver)
- [ ] 驱动查找(getDriver)
- [ ] 驱动枚举(listDrivers)
- [ ] 单例模式实现
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T6.1 - DrawContext绘制上下文

**Description**: 实现绘制上下文，封装绘制状态和参数

**Acceptance Criteria**
- [ ] 设备和引擎关联
- [ ] 绘制参数管理
- [ ] 样式状态管理
- [ ] 变换矩阵管理
- [ ] 绘制会话管理
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:10h, M:14h, P:20h → Expected: 14.3h

---

#### T6.2 - DrawFacade门面类

**Description**: 实现门面类，提供简化的高层绘制接口

**Acceptance Criteria**
- [ ] 一键绘制接口(draw)
- [ ] 快速图像导出(exportImage)
- [ ] 批量绘制支持
- [ ] 简化的API设计
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:10h, M:12h, P:16h → Expected: 12.3h

---

## 五、符号化系统 (Symbolization System)

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T7.1 | Symbolizer符号化器基类 | P0 | M4 | 10h | 📋 Todo | T4.1 |
| T7.2 | PointSymbolizer点符号化器 | P0 | M4 | 10h | 📋 Todo | T7.1 |
| T7.3 | LineSymbolizer线符号化器 | P0 | M4 | 10h | 📋 Todo | T7.1 |
| T7.4 | PolygonSymbolizer面符号化器 | P0 | M4 | 10h | 📋 Todo | T7.1 |
| T7.5 | TextSymbolizer文本符号化器 | P1 | M4 | 12h | 📋 Todo | T7.1 |
| T7.6 | RasterSymbolizer栅格符号化器 | P2 | M4 | 10h | 📋 Todo | T7.1 |

#### T7.1 - Symbolizer符号化器基类

**Description**: 实现符号化器抽象基类，定义符号化接口

**Acceptance Criteria**
- [ ] 符号化接口(symbolize)
- [ ] 符号类型管理
- [ ] 规则关联
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T7.2 - PointSymbolizer点符号化器

**Description**: 实现点符号化器，支持多种点符号样式

**Acceptance Criteria**
- [ ] 基本点符号(圆形、方形、三角形等)
- [ ] 图标符号(图片)
- [ ] 符号大小和颜色
- [ ] 符号旋转
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T7.3 - LineSymbolizer线符号化器

**Description**: 实现线符号化器，支持多种线样式

**Acceptance Criteria**
- [ ] 实线、虚线样式
- [ ] 线宽和颜色
- [ ] 线端点和连接样式
- [ ] 偏移和重复模式
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T7.4 - PolygonSymbolizer面符号化器

**Description**: 实现面符号化器，支持多种填充样式

**Acceptance Criteria**
- [ ] 纯色填充
- [ ] 图案填充
- [ ] 渐变填充
- [ ] 边框样式
- [ ] 单元测试覆盖率 ≥90%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

#### T7.5 - TextSymbolizer文本符号化器

**Description**: 实现文本符号化器，支持文本标注

**Acceptance Criteria**
- [ ] 文本字体和大小
- [ ] 文本颜色和描边
- [ ] 文本位置和对齐
- [ ] 文本避让
- [ ] 单元测试覆盖率 ≥85%

**Estimated Effort**: O:10h, M:12h, P:16h → Expected: 12.3h

---

#### T7.6 - RasterSymbolizer栅格符号化器

**Description**: 实现栅格符号化器，支持栅格数据渲染

**Acceptance Criteria**
- [ ] 颜色映射
- [ ] 对比度和亮度调整
- [ ] 透明度设置
- [ ] 单元测试覆盖率 ≥80%

**Estimated Effort**: O:8h, M:10h, P:14h → Expected: 10.3h

---

## 核心模块统计

| 分类 | 任务数 | 总工时 |
|------|--------|--------|
| 公共定义模块 | 6 | 34h |
| 工具类模块 | 4 | 32h |
| 构建配置 | 2 | 6h |
| 设备层 | 6 | 68h |
| 引擎层 | 4 | 42h |
| 驱动层 | 4 | 48h |
| 符号化系统 | 6 | 62h |
| **核心模块合计** | **32** | **292h** |
