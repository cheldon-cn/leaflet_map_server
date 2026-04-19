# Java Wrapper 索引

> 生成日期: 2026-04-19
> 源目录: cycle/javawrapper/src/main/java/cn/cycle/chart
> Java文件总数: 116
> 模块数: 24

---

## 一、总体统计

| 模块 | 类数量 | native方法数 | public方法数 | 说明 |
|------|--------|-------------|-------------|------|
| api | 3 | 11 | 0 |  |
| adapter | 8 | 4 | 72 | 适配器层 - 事件处理、画布适配 |
| alert | 4 | 2 | 36 | 告警模块 - 告警引擎、告警规则 |
| base | 2 | 16 | 0 | 基础模块 - 错误码、内存工具 |
| cache | 9 | 42 | 29 | 缓存模块 - 瓦片缓存、离线存储 |
| core | 4 | 49 | 46 | 核心模块 - 图表查看器、视口、渲染上下文 |
| draw | 9 | 59 | 0 | 绘制模块 - 画笔、画刷、字体、图像 |
| exception | 3 | 9 | 12 | 异常模块 - JNI异常、图表异常、渲染异常 |
| feature | 5 | 47 | 37 | 要素模块 - 字段定义、要素信息 |
| geometry | 12 | 38 | 71 | 几何模块 - 点、线、面、几何集合 |
| graph | 8 | 42 | 0 | 图形模块 - 命中测试、标签引擎、渲染队列 |
| health | 1 | 8 | 0 | 健康检查 - 组件状态检测 |
| layer | 8 | 68 | 36 | 图层模块 - 图层管理、数据源、驱动 |
| loader | 2 | 10 | 0 | 加载器 - 动态库加载、安全加载 |
| navi | 14 | 79 | 91 | 导航模块 - AIS、航线、航迹、定位 |
| parser | 4 | 29 | 0 | 解析器 - S57解析、增量解析 |
| perf | 2 | 8 | 0 | 性能监控 - 渲染统计、性能指标 |
| plugin | 2 | 0 | 10 | 插件模块 - 图表插件、插件管理 |
| proj | 2 | 24 | 0 | 投影模块 - 坐标转换、变换矩阵 |
| recovery | 3 | 21 | 0 | 容错模块 - 熔断器、降级、错误恢复 |
| style | 2 | 0 | 31 | 样式模块 - 颜色、绘制样式 |
| symbology | 5 | 28 | 18 | 符号化 - 过滤器、符号化规则 |
| util | 1 | 0 | 19 | 工具类 - 坐标变换工具 |
| jni | 3 | 2 | 6 | JNI桥接 - 本地对象管理、库加载 |
| **合计** | **116** | **596** | **514** | |

---

## 二、模块详细索引

### 1. api 模块

> 

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Error | NativeObject | final | 1 | 0 | api/Error.java |
| 2 | MemoryUtils | NativeObject | final | 5 | 0 | api/MemoryUtils.java |
| 3 | SDKVersion | NativeObject | final | 5 | 0 | api/SDKVersion.java |

#### Error - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | String | nativeGetMessage | `int code` |

#### MemoryUtils - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | void | nativeFree | `long ptr` |
| 2 | void | nativeFreeMemory | `long ptr` |
| 3 | void | nativeFreeString | `long ptr` |
| 4 | void | nativeStringArrayDestroy | `long arrayPtr, int count` |
| 5 | void | nativeGeometryArrayDestroy | `long arrayPtr, int count` |

#### SDKVersion - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | String | nativeGetVersion | `` |
| 2 | int | nativeGetVersionMajor | `` |
| 3 | int | nativeGetVersionMinor | `` |
| 4 | int | nativeGetVersionPatch | `` |
| 5 | String | nativeGetBuildDate | `` |

### 2. adapter 模块

> 适配器层 - 事件处理、画布适配

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | CanvasAdapter | - |  | 0 | 8 | api/adapter/CanvasAdapter.java |
| 2 |  | - |  | 0 | 0 | api/adapter/ChartEventHandler.java |
| 3 | ChartKeyEvent | - |  | 0 | 9 | api/adapter/ChartKeyEvent.java |
| 4 | ChartMouseEvent | - |  | 0 | 12 | api/adapter/ChartMouseEvent.java |
| 5 | ChartScrollEvent | - |  | 0 | 12 | api/adapter/ChartScrollEvent.java |
| 6 | ImageDevice | NativeObject |  | 4 | 5 | api/adapter/ImageDevice.java |
| 7 | PanHandler | - |  | 0 | 8 | api/adapter/PanHandler.java |
| 8 | ZoomHandler | - |  | 0 | 18 | api/adapter/ZoomHandler.java |

#### ImageDevice - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int width, int height` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeResize | `long ptr, int width, int height` |
| 4 | void | nativeClear | `long ptr` |

### 3. alert 模块

> 告警模块 - 告警引擎、告警规则

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Alert | - |  | 0 | 22 | api/alert/Alert.java |
| 2 | AlertEngine | - |  | 0 | 14 | api/alert/AlertEngine.java |
| 3 | CpaCalculator | NativeObject | final | 1 | 0 | api/alert/CpaCalculator.java |
| 4 | UkcCalculator | NativeObject | final | 1 | 0 | api/alert/UkcCalculator.java |

#### CpaCalculator - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | Object | nativeCalculate | `double ownLat, double ownLon, double ownSpeed, double ownCourse, double tgtLat, double tgtLon, double tgtSpeed, double tgtCourse` |

#### UkcCalculator - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | Object | nativeCalculate | `double chartedDepth, double tideHeight, double draft, double speed, double squatCoeff` |

### 4. base 模块

> 基础模块 - 错误码、内存工具

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Logger | NativeObject | final | 10 | 0 | api/base/Logger.java |
| 2 | PerformanceMonitor | NativeObject | final | 6 | 0 | api/base/PerformanceMonitor.java |

#### Logger - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetInstance | `` |
| 2 | void | nativeSetLevel | `long ptr, int level` |
| 3 | void | nativeTrace | `long ptr, String message` |
| 4 | void | nativeDebug | `long ptr, String message` |
| 5 | void | nativeInfo | `long ptr, String message` |
| 6 | void | nativeWarning | `long ptr, String message` |
| 7 | void | nativeError | `long ptr, String message` |
| 8 | void | nativeFatal | `long ptr, String message` |
| 9 | void | nativeSetLogFile | `long ptr, String filepath` |
| 10 | void | nativeSetConsoleOutput | `long ptr, int enable` |

#### PerformanceMonitor - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetInstance | `` |
| 2 | void | nativeStartFrame | `long ptr` |
| 3 | void | nativeEndFrame | `long ptr` |
| 4 | double | nativeGetFps | `long ptr` |
| 5 | long | nativeGetMemoryUsed | `long ptr` |
| 6 | void | nativeReset | `long ptr` |

### 5. cache 模块

> 缓存模块 - 瓦片缓存、离线存储

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | CacheManager | NativeObject | final | 10 | 0 | api/cache/CacheManager.java |
| 2 | DataEncryption | NativeObject | final | 2 | 0 | api/cache/DataEncryption.java |
| 3 | DiskTileCache | NativeObject | final | 1 | 0 | api/cache/DiskTileCache.java |
| 4 | MemoryTileCache | NativeObject | final | 2 | 0 | api/cache/MemoryTileCache.java |
| 5 | MultiLevelTileCache | NativeObject | final | 1 | 0 | api/cache/MultiLevelTileCache.java |
| 6 | OfflineRegion | NativeObject | final | 6 | 0 | api/cache/OfflineRegion.java |
| 7 | OfflineStorage | NativeObject | final | 20 | 0 | api/cache/OfflineStorage.java |
| 8 | TileCache | - |  | 0 | 16 | api/cache/TileCache.java |
| 9 | TileKey | - |  | 0 | 13 | api/cache/TileKey.java |

#### CacheManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeGetCacheCount | `long ptr` |
| 4 | long | nativeGetCache | `long ptr, String name` |
| 5 | void | nativeAddCache | `long ptr, String name, long cachePtr` |
| 6 | void | nativeRemoveCache | `long ptr, String name` |
| 7 | void | nativeClearAll | `long ptr` |
| 8 | long | nativeGetTotalSize | `long ptr` |
| 9 | long | nativeGetTotalTileCount | `long ptr` |
| 10 | void | nativeFlushAll | `long ptr` |

#### DataEncryption - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String key` |
| 2 | void | nativeDestroy | `long ptr` |

#### DiskTileCache - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String cacheDir, long maxSizeBytes` |

#### MemoryTileCache - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `long maxSizeBytes` |
| 2 | long | nativeGetMemoryUsage | `long ptr` |

#### MultiLevelTileCache - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `long memoryCachePtr, long diskCachePtr` |

#### OfflineRegion - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `long storagePtr, double minX, double minY, double maxX, double maxY, int minZoom, int maxZoom` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | boolean | nativeDownload | `long ptr` |
| 4 | float | nativeGetProgress | `long ptr` |
| 5 | String | nativeGetId | `long ptr` |
| 6 | String | nativeGetName | `long ptr` |

#### OfflineStorage - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetRegionById | `long ptr, String regionId` |
| 2 | void | nativeRemoveRegion | `long ptr, long index` |
| 3 | boolean | nativeRemoveRegionById | `long ptr, String regionId` |
| 4 | boolean | nativeStoreChart | `long ptr, String chartId, byte[] data` |
| 5 | String | nativeGetChartPath | `long ptr, String chartId` |
| 6 | boolean | nativeRemoveChart | `long ptr, String chartId` |
| 7 | long | nativeCreate | `String storagePath` |
| 8 | void | nativeDestroy | `long ptr` |
| 9 | long | nativeCreateRegion | `long ptr, double minX, double minY, double maxX, double maxY, int minZoom, int maxZoom` |
| 10 | long | nativeGetRegionCount | `long ptr` |
| 11 | long | nativeGetRegion | `long ptr, long index` |
| 12 | int | nativeStartDownload | `long ptr, String regionId` |
| 13 | int | nativePauseDownload | `long ptr, String regionId` |
| 14 | int | nativeResumeDownload | `long ptr, String regionId` |
| 15 | int | nativeCancelDownload | `long ptr, String regionId` |
| 16 | boolean | nativeIsDownloading | `long ptr, String regionId` |
| 17 | double | nativeGetDownloadProgress | `long ptr, String regionId` |
| 18 | long | nativeGetStorageSize | `long ptr` |
| 19 | long | nativeGetUsedSize | `long ptr` |
| 20 | long | nativeGetChartCount | `long ptr` |

### 6. core 模块

> 核心模块 - 图表查看器、视口、渲染上下文

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | ChartConfig | NativeObject | final | 6 | 6 | api/core/ChartConfig.java |
| 2 | ChartViewer | NativeObject | final | 15 | 26 | api/core/ChartViewer.java |
| 3 | RenderContext | NativeObject | final | 15 | 0 | api/core/RenderContext.java |
| 4 | Viewport | NativeObject | final | 13 | 14 | api/core/Viewport.java |

#### ChartConfig - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | int | nativeGetDisplayMode | `long ptr` |
| 2 | void | nativeSetDisplayMode | `long ptr, int mode` |
| 3 | boolean | nativeGetShowGrid | `long ptr` |
| 4 | void | nativeSetShowGrid | `long ptr, boolean show` |
| 5 | double | nativeGetDpi | `long ptr` |
| 6 | void | nativeSetDpi | `long ptr, double dpi` |

#### ChartViewer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | int | nativeInitialize | `long ptr` |
| 3 | void | nativeShutdown | `long ptr` |
| 4 | void | nativeDisposeImpl | `long ptr` |
| 5 | int | nativeLoadChart | `long ptr, String filePath` |
| 6 | int | nativeRender | `long ptr, long devicePtr, int width, int height` |
| 7 | void | nativeSetViewport | `long ptr, double centerX, double centerY, double scale` |
| 8 | void | nativeGetViewport | `long ptr, double[] outViewport` |
| 9 | long | nativeGetViewportPtr | `long ptr` |
| 10 | void | nativePan | `long ptr, double dx, double dy` |
| 11 | void | nativeZoom | `long ptr, double factor, double centerX, double centerY` |
| 12 | long | nativeQueryFeature | `long ptr, double x, double y` |
| 13 | void | nativeScreenToWorld | `long ptr, double screenX, double screenY, double[] outCoord` |
| 14 | void | nativeWorldToScreen | `long ptr, double worldX, double worldY, double[] outCoord` |
| 15 | void | nativeGetFullExtent | `long ptr, double[] outExtent` |

#### RenderContext - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `long devicePtr, long enginePtr` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeBeginDraw | `long ptr` |
| 4 | void | nativeEndDraw | `long ptr` |
| 5 | void | nativeClear | `long ptr, int r, int g, int b, int a` |
| 6 | void | nativeDrawGeometry | `long ptr, long geomPtr, long stylePtr` |
| 7 | void | nativeDrawPoint | `long ptr, double x, double y, long stylePtr` |
| 8 | void | nativeDrawLine | `long ptr, double x1, double y1, double x2, double y2, long stylePtr` |
| 9 | void | nativeDrawRect | `long ptr, double x, double y, double w, double h, long stylePtr` |
| 10 | void | nativeFillRect | `long ptr, double x, double y, double w, double h, long stylePtr` |
| 11 | void | nativeDrawText | `long ptr, String text, double x, double y, long stylePtr` |
| 12 | void | nativeSetTransform | `long ptr, double[] matrix` |
| 13 | void | nativeResetTransform | `long ptr` |
| 14 | void | nativeClip | `long ptr, long geomPtr` |
| 15 | void | nativeResetClip | `long ptr` |

#### Viewport - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | double | nativeGetCenterX | `long ptr` |
| 2 | double | nativeGetCenterY | `long ptr` |
| 3 | double | nativeGetScale | `long ptr` |
| 4 | double | nativeGetRotation | `long ptr` |
| 5 | void | nativeSetCenter | `long ptr, double x, double y` |
| 6 | void | nativeSetScale | `long ptr, double scale` |
| 7 | void | nativeSetRotation | `long ptr, double rotation` |
| 8 | void | nativeGetExtent | `long ptr, double[] outExtent` |
| 9 | void | nativeSetExtent | `long ptr, double minX, double minY, double maxX, double maxY` |
| 10 | void | nativePan | `long ptr, double dx, double dy` |
| 11 | void | nativeZoom | `long ptr, double factor` |
| 12 | void | nativeZoomAt | `long ptr, double factor, double centerX, double centerY` |
| 13 | void | nativeSetSize | `long ptr, int width, int height` |

### 7. draw 模块

> 绘制模块 - 画笔、画刷、字体、图像

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Brush | NativeObject | final | 6 | 0 | api/draw/Brush.java |
| 2 | Color | NativeObject | final | 4 | 0 | api/draw/Color.java |
| 3 | DrawDevice | NativeObject | final | 5 | 0 | api/draw/DrawDevice.java |
| 4 | DrawEngine | NativeObject | final | 4 | 0 | api/draw/DrawEngine.java |
| 5 | DrawStyle | NativeObject | final | 8 | 0 | api/draw/DrawStyle.java |
| 6 | Font | NativeObject | final | 8 | 0 | api/draw/Font.java |
| 7 | Image | NativeObject | final | 9 | 0 | api/draw/Image.java |
| 8 | ImageDevice | NativeObject | final | 7 | 0 | api/draw/ImageDevice.java |
| 9 | Pen | NativeObject | final | 8 | 0 | api/draw/Pen.java |

#### Brush - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetColor | `long ptr` |
| 4 | void | nativeSetColor | `long ptr, int packedColor` |
| 5 | int | nativeGetStyle | `long ptr` |
| 6 | void | nativeSetStyle | `long ptr, int style` |

#### Color - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | int | nativeFromRgba | `byte r, byte g, byte b, byte a` |
| 2 | int | nativeFromRgb | `byte r, byte g, byte b` |
| 3 | int | nativeFromHex | `int hex` |
| 4 | int | nativeToHex | `int packedColor` |

#### DrawDevice - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int type, int width, int height` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetWidth | `long ptr` |
| 4 | int | nativeGetHeight | `long ptr` |
| 5 | long | nativeGetImage | `long ptr` |

#### DrawEngine - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int type` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | boolean | nativeInitialize | `long ptr, long devicePtr` |
| 4 | void | nativeFinalize | `long ptr` |

#### DrawStyle - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeGetPen | `long ptr` |
| 4 | long | nativeGetBrush | `long ptr` |
| 5 | long | nativeGetFont | `long ptr` |
| 6 | void | nativeSetPen | `long ptr, long penPtr` |
| 7 | void | nativeSetBrush | `long ptr, long brushPtr` |
| 8 | void | nativeSetFont | `long ptr, long fontPtr` |

#### Font - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String family, double size` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetFamily | `long ptr` |
| 4 | double | nativeGetSize | `long ptr` |
| 5 | boolean | nativeIsBold | `long ptr` |
| 6 | boolean | nativeIsItalic | `long ptr` |
| 7 | void | nativeSetBold | `long ptr, boolean bold` |
| 8 | void | nativeSetItalic | `long ptr, boolean italic` |

#### Image - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int width, int height, int channels` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetWidth | `long ptr` |
| 4 | int | nativeGetHeight | `long ptr` |
| 5 | int | nativeGetChannels | `long ptr` |
| 6 | long | nativeLoadFromFile | `String path` |
| 7 | boolean | nativeSaveToFile | `long ptr, String path` |
| 8 | long | nativeGetDataPtr | `long ptr` |
| 9 | long | nativeGetDataConstPtr | `long ptr` |

#### ImageDevice - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `long width, long height` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | boolean | nativeResize | `long ptr, long width, long height` |
| 4 | void | nativeClear | `long ptr` |
| 5 | int | nativeGetWidth | `long ptr` |
| 6 | int | nativeGetHeight | `long ptr` |
| 7 | long | nativeGetNativePtr | `long ptr` |

#### Pen - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetColor | `long ptr` |
| 4 | double | nativeGetWidth | `long ptr` |
| 5 | void | nativeSetColor | `long ptr, int packedColor` |
| 6 | void | nativeSetWidth | `long ptr, double width` |
| 7 | int | nativeGetStyle | `long ptr` |
| 8 | void | nativeSetStyle | `long ptr, int style` |

### 8. exception 模块

> 异常模块 - JNI异常、图表异常、渲染异常

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | ChartException | RuntimeException | final | 5 | 4 | api/exception/ChartException.java |
| 2 | JniException | RuntimeException | final | 2 | 4 | api/exception/JniException.java |
| 3 | RenderException | RuntimeException | final | 2 | 4 | api/exception/RenderException.java |

#### ChartException - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int errorCode, String message` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetErrorCode | `long ptr` |
| 4 | String | nativeGetMessage | `long ptr` |
| 5 | String | nativeGetContext | `long ptr` |

#### JniException - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String message, String javaClass` |
| 2 | void | nativeDestroy | `long ptr` |

#### RenderException - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String message, String layerName` |
| 2 | void | nativeDestroy | `long ptr` |

### 9. feature 模块

> 要素模块 - 字段定义、要素信息

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Feature | NativeObject | final | 13 | 0 | api/feature/Feature.java |
| 2 | FeatureDefn | NativeObject | final | 8 | 0 | api/feature/FeatureDefn.java |
| 3 | FeatureInfo | NativeObject | final | 18 | 21 | api/feature/FeatureInfo.java |
| 4 | FieldDefn | NativeObject | final | 8 | 6 | api/feature/FieldDefn.java |
| 5 | FieldValue | - | final | 0 | 10 | api/feature/FieldValue.java |

#### Feature - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreateWithDefn | `long defnPtr` |
| 2 | long | nativeGetDefn | `long ptr` |
| 3 | boolean | nativeIsFieldSet | `long ptr, long index` |
| 4 | boolean | nativeIsFieldNull | `long ptr, long index` |
| 5 | int | nativeGetFieldAsIntegerByName | `long ptr, String name` |
| 6 | double | nativeGetFieldAsRealByName | `long ptr, String name` |
| 7 | String | nativeGetFieldAsStringByName | `long ptr, String name` |
| 8 | boolean | nativeSetFieldIntegerByName | `long ptr, String name, int value` |
| 9 | boolean | nativeSetFieldRealByName | `long ptr, String name, double value` |
| 10 | boolean | nativeSetFieldStringByName | `long ptr, String name, String value` |
| 11 | long | nativeStealGeometry | `long ptr` |
| 12 | long | nativeGetEnvelope | `long ptr` |
| 13 | long | nativeClone | `long ptr` |

#### FeatureDefn - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String name` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetName | `long ptr` |
| 4 | long | nativeGetFieldCount | `long ptr` |
| 5 | long | nativeGetFieldDefn | `long ptr, long index` |
| 6 | int | nativeGetFieldIndex | `long ptr, String name` |
| 7 | void | nativeAddFieldDefn | `long ptr, long fieldPtr` |
| 8 | long | nativeGetGeomFieldCount | `long ptr` |

#### FeatureInfo - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetFid | `long ptr` |
| 2 | void | nativeSetFid | `long ptr, long fid` |
| 3 | long | nativeGetFieldCount | `long ptr` |
| 4 | boolean | nativeIsFieldSet | `long ptr, long index` |
| 5 | boolean | nativeIsFieldNull | `long ptr, long index` |
| 6 | int | nativeGetFieldAsInteger | `long ptr, long index` |
| 7 | double | nativeGetFieldAsReal | `long ptr, long index` |
| 8 | String | nativeGetFieldAsString | `long ptr, long index` |
| 9 | void | nativeSetFieldInteger | `long ptr, long index, int value` |
| 10 | void | nativeSetFieldReal | `long ptr, long index, double value` |
| 11 | void | nativeSetFieldString | `long ptr, long index, String value` |
| 12 | void | nativeSetFieldNull | `long ptr, long index` |
| 13 | long | nativeGetGeometry | `long ptr` |
| 14 | void | nativeSetGeometry | `long ptr, long geomPtr` |
| 15 | String | nativeGetFeatureName | `long ptr` |
| 16 | String | nativeGetFeatureClass | `long ptr` |
| 17 | String | nativeGetFieldName | `long ptr, long index` |
| 18 | void | nativeDestroy | `long ptr` |

#### FieldDefn - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String name, int type` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetName | `long ptr` |
| 4 | int | nativeGetType | `long ptr` |
| 5 | int | nativeGetWidth | `long ptr` |
| 6 | void | nativeSetWidth | `long ptr, int width` |
| 7 | int | nativeGetPrecision | `long ptr` |
| 8 | void | nativeSetPrecision | `long ptr, int precision` |

### 10. geometry 模块

> 几何模块 - 点、线、面、几何集合

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Coordinate | - | final | 0 | 11 | api/geometry/Coordinate.java |
| 2 | Envelope | - | final | 0 | 24 | api/geometry/Envelope.java |
| 3 | Geometry | NativeObject | abstract | 0 | 14 | api/geometry/Geometry.java |
| 4 | GeometryCollection | Geometry | final | 4 | 0 | api/geometry/GeometryCollection.java |
| 5 | GeometryFactory | NativeObject | final | 8 | 0 | api/geometry/GeometryFactory.java |
| 6 | LineString | Geometry |  | 2 | 5 | api/geometry/LineString.java |
| 7 | LinearRing | LineString | final | 1 | 2 | api/geometry/LinearRing.java |
| 8 | MultiLineString | Geometry | final | 4 | 0 | api/geometry/MultiLineString.java |
| 9 | MultiPoint | Geometry | final | 4 | 0 | api/geometry/MultiPoint.java |
| 10 | MultiPolygon | Geometry | final | 4 | 0 | api/geometry/MultiPolygon.java |
| 11 | Point | Geometry | final | 6 | 8 | api/geometry/Point.java |
| 12 | Polygon | Geometry | final | 5 | 7 | api/geometry/Polygon.java |

#### GeometryCollection - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | long | nativeGetNumGeometries | `long ptr` |
| 3 | long | nativeGetGeometryN | `long ptr, long index` |
| 4 | void | nativeAddGeometry | `long ptr, long geomPtr` |

#### GeometryFactory - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeCreatePoint | `long ptr, double x, double y` |
| 4 | long | nativeCreateLineString | `long ptr` |
| 5 | long | nativeCreatePolygon | `long ptr` |
| 6 | long | nativeCreateFromWkt | `long ptr, String wkt` |
| 7 | long | nativeCreateFromWkb | `long ptr, byte[] wkb` |
| 8 | long | nativeCreateFromGeoJson | `long ptr, String geojson` |

#### LineString - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | double | nativeGetLength | `long ptr` |
| 2 | void | nativeAddPoint | `long ptr, double x, double y` |

#### LinearRing - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | boolean | nativeIsClosed | `long ptr` |

#### MultiLineString - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | long | nativeGetNumGeometries | `long ptr` |
| 3 | long | nativeGetGeometryN | `long ptr, long index` |
| 4 | void | nativeAddGeometry | `long ptr, long lineStringPtr` |

#### MultiPoint - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | long | nativeGetNumGeometries | `long ptr` |
| 3 | long | nativeGetGeometryN | `long ptr, long index` |
| 4 | void | nativeAddGeometry | `long ptr, long pointPtr` |

#### MultiPolygon - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | long | nativeGetNumGeometries | `long ptr` |
| 3 | long | nativeGetGeometryN | `long ptr, long index` |
| 4 | void | nativeAddGeometry | `long ptr, long polygonPtr` |

#### Point - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | double | nativeGetX | `long ptr` |
| 2 | double | nativeGetY | `long ptr` |
| 3 | double | nativeGetZ | `long ptr` |
| 4 | void | nativeSetX | `long ptr, double x` |
| 5 | void | nativeSetY | `long ptr, double y` |
| 6 | void | nativeSetZ | `long ptr, double z` |

#### Polygon - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetNumInteriorRings | `long ptr` |
| 2 | long | nativeGetExteriorRing | `long ptr` |
| 3 | long | nativeGetInteriorRingN | `long ptr, int index` |
| 4 | void | nativeAddInteriorRing | `long ptr, long ringPtr` |
| 5 | double | nativeGetArea | `long ptr` |

### 11. graph 模块

> 图形模块 - 命中测试、标签引擎、渲染队列

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | HitTest | NativeObject | final | 4 | 0 | api/graph/HitTest.java |
| 2 | LODManager | NativeObject | final | 6 | 0 | api/graph/LODManager.java |
| 3 | LabelEngine | NativeObject | final | 6 | 0 | api/graph/LabelEngine.java |
| 4 | LabelInfo | NativeObject | final | 7 | 0 | api/graph/LabelInfo.java |
| 5 | RenderOptimizer | NativeObject | final | 5 | 0 | api/graph/RenderOptimizer.java |
| 6 | RenderQueue | NativeObject | final | 6 | 0 | api/graph/RenderQueue.java |
| 7 | RenderTask | NativeObject | final | 5 | 0 | api/graph/RenderTask.java |
| 8 | TransformManager | NativeObject | final | 3 | 0 | api/graph/TransformManager.java |

#### HitTest - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeQueryPoint | `long ptr, long layerPtr, double x, double y, double tolerance` |
| 4 | void | nativeFreeFeatures | `long featuresPtr` |

#### LODManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetCurrentLevel | `long ptr` |
| 4 | void | nativeSetCurrentLevel | `long ptr, int level` |
| 5 | int | nativeGetLevelCount | `long ptr` |
| 6 | double | nativeGetScaleForLevel | `long ptr, int level` |

#### LabelEngine - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeSetMaxLabels | `long ptr, int maxLabels` |
| 4 | int | nativeGetMaxLabels | `long ptr` |
| 5 | void | nativeSetCollisionDetection | `long ptr, boolean enable` |
| 6 | void | nativeClear | `long ptr` |

#### LabelInfo - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String text, double x, double y` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetText | `long ptr` |
| 4 | double | nativeGetX | `long ptr` |
| 5 | double | nativeGetY | `long ptr` |
| 6 | void | nativeSetText | `long ptr, String text` |
| 7 | void | nativeSetPosition | `long ptr, double x, double y` |

#### RenderOptimizer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeSetCacheEnabled | `long ptr, boolean enable` |
| 4 | boolean | nativeIsCacheEnabled | `long ptr` |
| 5 | void | nativeClearCache | `long ptr` |

#### RenderQueue - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativePush | `long ptr, long taskPtr` |
| 4 | long | nativePop | `long ptr` |
| 5 | long | nativeGetSize | `long ptr` |
| 6 | void | nativeClear | `long ptr` |

#### RenderTask - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | boolean | nativeExecute | `long ptr` |
| 4 | boolean | nativeIsComplete | `long ptr` |
| 5 | void | nativeCancel | `long ptr` |

#### TransformManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeSetViewport | `long ptr, double cx, double cy, double scale, double rotation` |

### 12. health 模块

> 健康检查 - 组件状态检测

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | HealthCheck | NativeObject | final | 8 | 0 | api/health/HealthCheck.java |

#### HealthCheck - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String name` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeExecute | `long ptr` |
| 4 | void | nativeDestroyResult | `long resultPtr` |
| 5 | int | nativeGetResultStatus | `long resultPtr` |
| 6 | String | nativeGetResultComponentName | `long resultPtr` |
| 7 | String | nativeGetResultMessage | `long resultPtr` |
| 8 | double | nativeGetResultDuration | `long resultPtr` |

### 13. layer 模块

> 图层模块 - 图层管理、数据源、驱动

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | DataSource | NativeObject | final | 9 | 0 | api/layer/DataSource.java |
| 2 | DriverManager | NativeObject | final | 6 | 0 | api/layer/DriverManager.java |
| 3 | Layer | NativeObject |  | 6 | 15 | api/layer/Layer.java |
| 4 | LayerGroup | NativeObject | final | 14 | 0 | api/layer/LayerGroup.java |
| 5 | LayerManager | NativeObject | final | 12 | 11 | api/layer/LayerManager.java |
| 6 | MemoryLayer | NativeObject | final | 6 | 0 | api/layer/MemoryLayer.java |
| 7 | RasterLayer | NativeObject | final | 6 | 0 | api/layer/RasterLayer.java |
| 8 | VectorLayer | Layer | final | 9 | 10 | api/layer/VectorLayer.java |

#### DataSource - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeOpen | `String path` |
| 2 | void | nativeClose | `long ptr` |
| 3 | boolean | nativeIsOpen | `long ptr` |
| 4 | String | nativeGetPath | `long ptr` |
| 5 | long | nativeGetLayerCount | `long ptr` |
| 6 | long | nativeGetLayer | `long ptr, long index` |
| 7 | long | nativeGetLayerByName | `long ptr, String name` |
| 8 | long | nativeCreateLayer | `long ptr, String name, int geomType` |
| 9 | int | nativeDeleteLayer | `long ptr, String name` |

#### DriverManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetInstance | `` |
| 2 | int | nativeRegisterDriver | `long ptr, long driverPtr` |
| 3 | int | nativeUnregisterDriver | `long ptr, String name` |
| 4 | long | nativeGetDriver | `long ptr, String name` |
| 5 | int | nativeGetDriverCount | `long ptr` |
| 6 | String | nativeGetDriverName | `long ptr, int index` |

#### Layer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | String | nativeGetName | `long ptr` |
| 2 | void | nativeSetName | `long ptr, String name` |
| 3 | int | nativeGetType | `long ptr` |
| 4 | void | nativeSetVisible | `long ptr, boolean visible` |
| 5 | void | nativeSetOpacity | `long ptr, double opacity` |
| 6 | void | nativeDestroy | `long ptr` |

#### LayerGroup - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String name` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetName | `long ptr` |
| 4 | long | nativeGetLayerCount | `long ptr` |
| 5 | long | nativeGetLayer | `long ptr, long index` |
| 6 | void | nativeAddLayer | `long ptr, long layerPtr` |
| 7 | void | nativeRemoveLayer | `long ptr, long index` |
| 8 | void | nativeMoveLayer | `long ptr, long from, long to` |
| 9 | boolean | nativeIsVisible | `long ptr` |
| 10 | void | nativeSetVisible | `long ptr, boolean visible` |
| 11 | double | nativeGetOpacity | `long ptr` |
| 12 | void | nativeSetOpacity | `long ptr, double opacity` |
| 13 | int | nativeGetZOrder | `long ptr` |
| 14 | void | nativeSetZOrder | `long ptr, int zOrder` |

#### LayerManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeGetLayerCount | `long ptr` |
| 4 | long | nativeGetLayer | `long ptr, long index` |
| 5 | int | nativeGetLayerType | `long ptr, long index` |
| 6 | void | nativeAddLayer | `long ptr, long layerPtr` |
| 7 | void | nativeRemoveLayer | `long ptr, long index` |
| 8 | void | nativeMoveLayer | `long ptr, long fromIndex, long toIndex` |
| 9 | boolean | nativeGetLayerVisible | `long ptr, long index` |
| 10 | void | nativeSetLayerVisible | `long ptr, long index, boolean visible` |
| 11 | double | nativeGetLayerOpacity | `long ptr, long index` |
| 12 | void | nativeSetLayerOpacity | `long ptr, long index, double opacity` |

#### MemoryLayer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String name` |
| 2 | boolean | nativeAddFeature | `long ptr, long featurePtr` |
| 3 | boolean | nativeRemoveFeature | `long ptr, long fid` |
| 4 | void | nativeClear | `long ptr` |
| 5 | long | nativeGetFeatureCount | `long ptr` |
| 6 | long | nativeCreateFromFeatures | `String name, long[] featurePtrs` |

#### RasterLayer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String name, String filepath` |
| 2 | int | nativeGetWidth | `long ptr` |
| 3 | int | nativeGetHeight | `long ptr` |
| 4 | int | nativeGetBandCount | `long ptr` |
| 5 | double | nativeGetNoDataValue | `long ptr, int bandIndex` |
| 6 | long | nativeGetExtent | `long ptr` |

#### VectorLayer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetFeatureCount | `long ptr` |
| 2 | long | nativeGetFeature | `long ptr, long fid` |
| 3 | long | nativeGetNextFeature | `long ptr` |
| 4 | void | nativeSetSpatialFilterRect | `long ptr, double minX, double minY, double maxX, double maxY` |
| 5 | boolean | nativeGetExtent | `long ptr, double[] extent` |
| 6 | void | nativeResetReading | `long ptr` |
| 7 | int | nativeAddFeature | `long ptr, long featurePtr` |
| 8 | long | nativeGetFieldCount | `long ptr` |
| 9 | long | nativeGetFieldDefn | `long ptr, int index` |

### 14. loader 模块

> 加载器 - 动态库加载、安全加载

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | LibraryLoader | NativeObject | final | 6 | 0 | api/loader/LibraryLoader.java |
| 2 | SecureLibraryLoader | NativeObject | final | 4 | 0 | api/loader/SecureLibraryLoader.java |

#### LibraryLoader - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeLoad | `long ptr, String libraryPath` |
| 4 | void | nativeUnload | `long ptr, long handlePtr` |
| 5 | long | nativeGetSymbol | `long ptr, long handlePtr, String symbolName` |
| 6 | String | nativeGetError | `long ptr` |

#### SecureLibraryLoader - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String allowedPaths` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeLoad | `long ptr, String libraryPath` |
| 4 | int | nativeVerifySignature | `long ptr, long handlePtr, String publicKey` |

### 15. navi 模块

> 导航模块 - AIS、航线、航迹、定位

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | AisTarget | - |  | 0 | 36 | api/navi/AisManager.java |
| 2 | AisTarget | NativeObject | final | 12 | 0 | api/navi/AisTarget.java |
| 3 | CollisionAssessor | NativeObject | final | 7 | 0 | api/navi/CollisionAssessor.java |
| 4 | NavigationEngine | - |  | 0 | 17 | api/navi/NavigationEngine.java |
| 5 | NmeaParser | NativeObject | final | 4 | 0 | api/navi/NmeaParser.java |
| 6 | OffCourseDetector | NativeObject | final | 5 | 0 | api/navi/OffCourseDetector.java |
| 7 | PositionManager | NativeObject | final | 13 | 0 | api/navi/PositionManager.java |
| 8 | PositionProvider | NativeObject | final | 6 | 0 | api/navi/PositionProvider.java |
| 9 | Route | - |  | 0 | 18 | api/navi/Route.java |
| 10 | RouteManager | NativeObject | final | 9 | 0 | api/navi/RouteManager.java |
| 11 | RoutePlanner | NativeObject | final | 3 | 0 | api/navi/RoutePlanner.java |
| 12 | Track | NativeObject | final | 10 | 0 | api/navi/Track.java |
| 13 | TrackRecorder | NativeObject | final | 10 | 0 | api/navi/TrackRecorder.java |
| 14 | Waypoint | - |  | 0 | 20 | api/navi/Waypoint.java |

#### AisTarget - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int mmsi` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetMmsi | `long ptr` |
| 4 | String | nativeGetName | `long ptr` |
| 5 | double | nativeGetLatitude | `long ptr` |
| 6 | double | nativeGetLongitude | `long ptr` |
| 7 | double | nativeGetSpeed | `long ptr` |
| 8 | double | nativeGetCourse | `long ptr` |
| 9 | double | nativeGetHeading | `long ptr` |
| 10 | int | nativeGetNavStatus | `long ptr` |
| 11 | void | nativeUpdatePosition | `long ptr, double lat, double lon, double speed, double course` |
| 12 | void | nativeArrayDestroy | `long arrayPtr, int count` |

#### CollisionAssessor - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativeAssess | `long ptr, double ownLat, double ownLon, double ownSpeed, double ownCourse, long targetPtr` |
| 4 | void | nativeDestroyRisk | `long riskPtr` |
| 5 | double | nativeGetRiskCpa | `long riskPtr` |
| 6 | double | nativeGetRiskTcpa | `long riskPtr` |
| 7 | int | nativeGetRiskLevel | `long riskPtr` |

#### NmeaParser - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeParse | `long ptr, String sentence` |
| 4 | String | nativeGetSentenceType | `long ptr` |

#### OffCourseDetector - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeSetThreshold | `long ptr, double meters` |
| 4 | boolean | nativeCheck | `long ptr, double currentLat, double currentLon, long fromWaypointPtr, long toWaypointPtr` |
| 5 | double | nativeGetCrossTrackError | `long ptr, double currentLat, double currentLon, long fromWaypointPtr, long toWaypointPtr` |

#### PositionManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeInitialize | `long ptr` |
| 4 | void | nativeShutdown | `long ptr` |
| 5 | double | nativeGetLatitude | `long ptr` |
| 6 | double | nativeGetLongitude | `long ptr` |
| 7 | double | nativeGetSpeed | `long ptr` |
| 8 | double | nativeGetCourse | `long ptr` |
| 9 | double | nativeGetHeading | `long ptr` |
| 10 | int | nativeGetFixQuality | `long ptr` |
| 11 | int | nativeGetSatelliteCount | `long ptr` |
| 12 | int | nativeSetPosition | `long ptr, double latitude, double longitude` |
| 13 | void | nativeSetCallback | `long ptr, long callbackPtr` |

#### PositionProvider - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreateSerial | `String port, int baudRate` |
| 2 | long | nativeCreateNetwork | `String host, int port` |
| 3 | void | nativeDestroy | `long ptr` |
| 4 | boolean | nativeConnect | `long ptr` |
| 5 | void | nativeDisconnect | `long ptr` |
| 6 | boolean | nativeIsConnected | `long ptr` |

#### RouteManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetRouteCount | `long ptr` |
| 4 | long | nativeGetRoute | `long ptr, int index` |
| 5 | long | nativeGetRouteById | `long ptr, String id` |
| 6 | void | nativeAddRoute | `long ptr, long routePtr` |
| 7 | void | nativeRemoveRoute | `long ptr, String id` |
| 8 | long | nativeGetActiveRoute | `long ptr` |
| 9 | void | nativeSetActiveRoute | `long ptr, String id` |

#### RoutePlanner - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | long | nativePlanRoute | `long ptr, long startPtr, long endPtr` |

#### Track - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetId | `long ptr` |
| 4 | String | nativeGetName | `long ptr` |
| 5 | void | nativeSetName | `long ptr, String name` |
| 6 | long | nativeGetPointCount | `long ptr` |
| 7 | long | nativeGetPointPtr | `long ptr, long index` |
| 8 | void | nativeAddPoint | `long ptr, double lat, double lon, double speed, double course, double timestamp` |
| 9 | void | nativeClear | `long ptr` |
| 10 | long | nativeToLineString | `long ptr` |

#### TrackRecorder - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeStart | `long ptr, String trackName` |
| 4 | void | nativeStop | `long ptr` |
| 5 | void | nativePause | `long ptr` |
| 6 | void | nativeResume | `long ptr` |
| 7 | boolean | nativeIsRecording | `long ptr` |
| 8 | long | nativeGetCurrentTrack | `long ptr` |
| 9 | void | nativeSetInterval | `long ptr, int seconds` |
| 10 | void | nativeSetMinDistance | `long ptr, double meters` |

### 16. parser 模块

> 解析器 - S57解析、增量解析

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | ChartParser | NativeObject | final | 3 | 0 | api/parser/ChartParser.java |
| 2 | IParser | NativeObject | abstract | 7 | 0 | api/parser/IParser.java |
| 3 | IncrementalParser | NativeObject | final | 12 | 0 | api/parser/IncrementalParser.java |
| 4 | S57Parser | NativeObject | final | 7 | 0 | api/parser/S57Parser.java |

#### ChartParser - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetInstance | `` |
| 2 | int | nativeInitialize | `long ptr` |
| 3 | void | nativeShutdown | `long ptr` |

#### IParser - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int format` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeOpen | `long ptr, String path` |
| 4 | void | nativeClose | `long ptr` |
| 5 | long | nativeParse | `long ptr` |
| 6 | long | nativeParseIncremental | `long ptr, int batchSize` |
| 7 | int | nativeGetFormat | `long ptr` |

#### IncrementalParser - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | void | nativeStart | `long ptr, String path, int batchSize` |
| 4 | void | nativePause | `long ptr` |
| 5 | void | nativeResume | `long ptr` |
| 6 | void | nativeCancel | `long ptr` |
| 7 | long | nativeParseNext | `long ptr` |
| 8 | boolean | nativeHasMore | `long ptr` |
| 9 | float | nativeGetProgress | `long ptr` |
| 10 | boolean | nativeHasFileChanged | `long ptr, String path` |
| 11 | void | nativeMarkProcessed | `long ptr, String path` |
| 12 | void | nativeClearState | `long ptr, String path` |

#### S57Parser - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | boolean | nativeOpen | `long ptr, String path` |
| 4 | long | nativeParse | `long ptr` |
| 5 | void | nativeSetFeatureFilter | `long ptr, Object[] features` |
| 6 | void | nativeSetSpatialFilter | `long ptr, double minX, double minY, double maxX, double maxY` |
| 7 | long | nativeParseFile | `long ptr, String path, long configPtr` |

### 17. perf 模块

> 性能监控 - 渲染统计、性能指标

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | PerformanceMonitor | NativeObject | final | 7 | 0 | api/perf/PerformanceMonitor.java |
| 2 | RenderStats | NativeObject | final | 1 | 0 | api/perf/RenderStats.java |

#### PerformanceMonitor - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeGetInstance | `` |
| 2 | void | nativeStartFrame | `long ptr` |
| 3 | void | nativeEndFrame | `long ptr` |
| 4 | double | nativeGetFPS | `long ptr` |
| 5 | long | nativeGetMemoryUsed | `long ptr` |
| 6 | void | nativeReset | `long ptr` |
| 7 | void | nativeGetStats | `long statsPtr` |

#### RenderStats - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | void | nativeReset | `long ptr` |

### 18. plugin 模块

> 插件模块 - 图表插件、插件管理

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 |  | - |  | 0 | 0 | api/plugin/ChartPlugin.java |
| 2 | PluginManager | - |  | 0 | 10 | api/plugin/PluginManager.java |

### 19. proj 模块

> 投影模块 - 坐标转换、变换矩阵

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | CoordTransformer | NativeObject | final | 10 | 0 | api/proj/CoordTransformer.java |
| 2 | TransformMatrix | NativeObject | final | 14 | 0 | api/proj/TransformMatrix.java |

#### CoordTransformer - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `String sourceCrs, String targetCrs` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | boolean | nativeIsValid | `long ptr` |
| 4 | Object | nativeTransform | `long ptr, double x, double y` |
| 5 | Object | nativeTransformInverse | `long ptr, double x, double y` |
| 6 | void | nativeTransformArray | `long ptr, double[] xArr, double[] yArr` |
| 7 | Object | nativeTransformEnvelope | `long ptr, double minX, double minY, double maxX, double maxY` |
| 8 | long | nativeTransformGeometry | `long ptr, long geomPtr` |
| 9 | String | nativeGetSourceCrs | `long ptr` |
| 10 | String | nativeGetTargetCrs | `long ptr` |

#### TransformMatrix - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | long | nativeCreateIdentity | `` |
| 3 | long | nativeCreateTranslation | `double tx, double ty` |
| 4 | long | nativeCreateScale | `double sx, double sy` |
| 5 | long | nativeCreateRotation | `double angle` |
| 6 | void | nativeDestroy | `long ptr` |
| 7 | long | nativeMultiply | `long aPtr, long bPtr` |
| 8 | long | nativeInverse | `long ptr` |
| 9 | void | nativeTransformPoint | `long ptr, double[] xyArr` |
| 10 | void | nativeSetElements | `long ptr, double[] elementsArr` |
| 11 | void | nativeTranslate | `long ptr, double dx, double dy` |
| 12 | void | nativeScale | `long ptr, double sx, double sy` |
| 13 | void | nativeRotate | `long ptr, double angleDegrees` |
| 14 | void | nativeMultiplyInPlace | `long resultPtr, long aPtr, long bPtr` |

### 20. recovery 模块

> 容错模块 - 熔断器、降级、错误恢复

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | CircuitBreaker | NativeObject | final | 7 | 0 | api/recovery/CircuitBreaker.java |
| 2 | ErrorRecoveryManager | NativeObject | final | 7 | 0 | api/recovery/ErrorRecoveryManager.java |
| 3 | GracefulDegradation | NativeObject | final | 7 | 0 | api/recovery/GracefulDegradation.java |

#### CircuitBreaker - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int failureThreshold, int recoveryTimeoutMs` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetState | `long ptr` |
| 4 | boolean | nativeAllowRequest | `long ptr` |
| 5 | void | nativeRecordSuccess | `long ptr` |
| 6 | void | nativeRecordFailure | `long ptr` |
| 7 | void | nativeReset | `long ptr` |

#### ErrorRecoveryManager - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeInitialize | `long ptr` |
| 4 | void | nativeShutdown | `long ptr` |
| 5 | int | nativeHandleError | `long ptr, int errorCode, String context` |
| 6 | void | nativeSetDegradationLevel | `long ptr, int level` |
| 7 | void | nativeRegisterStrategy | `long ptr, int errorCode, long strategyPtr` |

#### GracefulDegradation - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetLevel | `long ptr` |
| 4 | void | nativeSetLevel | `long ptr, int level` |
| 5 | boolean | nativeIsFeatureEnabled | `long ptr, String feature` |
| 6 | void | nativeDisableFeature | `long ptr, String feature` |
| 7 | void | nativeEnableFeature | `long ptr, String feature` |

### 21. style 模块

> 样式模块 - 颜色、绘制样式

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Color | - | final | 0 | 13 | api/style/Color.java |
| 2 | DrawStyle | - | final | 0 | 18 | api/style/DrawStyle.java |

### 22. symbology 模块

> 符号化 - 过滤器、符号化规则

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | ComparisonFilter | NativeObject | final | 6 | 0 | api/symbology/ComparisonFilter.java |
| 2 | Filter | NativeObject | abstract | 5 | 0 | api/symbology/Filter.java |
| 3 | LogicalFilter | NativeObject | final | 2 | 0 | api/symbology/LogicalFilter.java |
| 4 | Symbolizer | - |  | 0 | 18 | api/symbology/Symbolizer.java |
| 5 | SymbolizerRule | NativeObject | final | 15 | 0 | api/symbology/SymbolizerRule.java |

#### ComparisonFilter - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | void | nativeDestroy | `long ptr` |
| 2 | boolean | nativeEvaluate | `long ptr, long featurePtr` |
| 3 | String | nativeGetPropertyName | `long ptr` |
| 4 | int | nativeGetOperator | `long ptr` |
| 5 | String | nativeGetValue | `long ptr` |
| 6 | long | nativeCreate | `String property, int op, String value` |

#### Filter - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | int | nativeGetType | `long ptr` |
| 4 | boolean | nativeEvaluate | `long ptr, long featurePtr` |
| 5 | String | nativeToString | `long ptr` |

#### LogicalFilter - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `int op` |
| 2 | void | nativeAddFilter | `long ptr, long filterPtr` |

#### SymbolizerRule - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | long | nativeCreate | `` |
| 2 | void | nativeDestroy | `long ptr` |
| 3 | String | nativeGetName | `long ptr` |
| 4 | void | nativeSetName | `long ptr, String name` |
| 5 | long | nativeGetFilter | `long ptr` |
| 6 | void | nativeSetFilter | `long ptr, long filterPtr` |
| 7 | long | nativeGetSymbolizer | `long ptr, long index` |
| 8 | void | nativeSetSymbolizer | `long ptr, long symPtr` |
| 9 | double | nativeGetMinScale | `long ptr` |
| 10 | double | nativeGetMaxScale | `long ptr` |
| 11 | void | nativeSetMinScale | `long ptr, double scale` |
| 12 | void | nativeSetMaxScale | `long ptr, double scale` |
| 13 | void | nativeAddSymbolizer | `long ptr, long symPtr` |
| 14 | boolean | nativeIsApplicable | `long ptr, long featurePtr, double scale` |
| 15 | void | nativeSetScaleRange | `long ptr, double minScale, double maxScale` |

### 23. util 模块

> 工具类 - 坐标变换工具

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | Transform | - |  | 0 | 19 | api/util/Transform.java |

### 24. jni 模块

> JNI桥接 - 本地对象管理、库加载

| # | 类名 | 父类 | 类型 | native方法 | public方法 | 文件路径 |
|---|------|------|------|-----------|-----------|----------|
| 1 | JniBridge | - | final | 2 | 3 | jni/JniBridge.java |
| 2 | NativeHandle | - | final | 0 | 2 | jni/NativeHandle.java |
| 3 | NativeObject | - | abstract | 0 | 1 | jni/NativeObject.java |

#### JniBridge - native方法列表

| # | 返回类型 | 方法名 | 参数 |
|---|---------|--------|------|
| 1 | void | nativeInitialize | `` |
| 2 | void | nativeShutdown | `` |

---

## 三、功能缺失评估

### api 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| Error | 1 | 仅有native声明，缺少高级封装方法 |
| MemoryUtils | 5 | 仅有native声明，缺少高级封装方法 |
| SDKVersion | 5 | 仅有native声明，缺少高级封装方法 |

### alert 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| CpaCalculator | 1 | 仅有native声明，缺少高级封装方法 |
| UkcCalculator | 1 | 仅有native声明，缺少高级封装方法 |

### base 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| Logger | 10 | 仅有native声明，缺少高级封装方法 |
| PerformanceMonitor | 6 | 仅有native声明，缺少高级封装方法 |

### cache 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| CacheManager | 10 | 仅有native声明，缺少高级封装方法 |
| DataEncryption | 2 | 仅有native声明，缺少高级封装方法 |
| DiskTileCache | 1 | 仅有native声明，缺少高级封装方法 |
| MemoryTileCache | 2 | 仅有native声明，缺少高级封装方法 |
| MultiLevelTileCache | 1 | 仅有native声明，缺少高级封装方法 |
| OfflineRegion | 6 | 仅有native声明，缺少高级封装方法 |
| OfflineStorage | 20 | 仅有native声明，缺少高级封装方法 |

### core 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| RenderContext | 15 | 仅有native声明，缺少高级封装方法 |

### draw 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| Brush | 6 | 仅有native声明，缺少高级封装方法 |
| Color | 4 | 仅有native声明，缺少高级封装方法 |
| DrawDevice | 5 | 仅有native声明，缺少高级封装方法 |
| DrawEngine | 4 | 仅有native声明，缺少高级封装方法 |
| DrawStyle | 8 | 仅有native声明，缺少高级封装方法 |
| Font | 8 | 仅有native声明，缺少高级封装方法 |
| Image | 9 | 仅有native声明，缺少高级封装方法 |
| ImageDevice | 7 | 仅有native声明，缺少高级封装方法 |
| Pen | 8 | 仅有native声明，缺少高级封装方法 |

### feature 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| Feature | 13 | 仅有native声明，缺少高级封装方法 |
| FeatureDefn | 8 | 仅有native声明，缺少高级封装方法 |

### geometry 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| GeometryCollection | 4 | 仅有native声明，缺少高级封装方法 |
| GeometryFactory | 8 | 仅有native声明，缺少高级封装方法 |
| MultiLineString | 4 | 仅有native声明，缺少高级封装方法 |
| MultiPoint | 4 | 仅有native声明，缺少高级封装方法 |
| MultiPolygon | 4 | 仅有native声明，缺少高级封装方法 |

### graph 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| HitTest | 4 | 仅有native声明，缺少高级封装方法 |
| LODManager | 6 | 仅有native声明，缺少高级封装方法 |
| LabelEngine | 6 | 仅有native声明，缺少高级封装方法 |
| LabelInfo | 7 | 仅有native声明，缺少高级封装方法 |
| RenderOptimizer | 5 | 仅有native声明，缺少高级封装方法 |
| RenderQueue | 6 | 仅有native声明，缺少高级封装方法 |
| RenderTask | 5 | 仅有native声明，缺少高级封装方法 |
| TransformManager | 3 | 仅有native声明，缺少高级封装方法 |

### health 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| HealthCheck | 8 | 仅有native声明，缺少高级封装方法 |

### layer 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| DataSource | 9 | 仅有native声明，缺少高级封装方法 |
| DriverManager | 6 | 仅有native声明，缺少高级封装方法 |
| LayerGroup | 14 | 仅有native声明，缺少高级封装方法 |
| MemoryLayer | 6 | 仅有native声明，缺少高级封装方法 |
| RasterLayer | 6 | 仅有native声明，缺少高级封装方法 |

### loader 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| LibraryLoader | 6 | 仅有native声明，缺少高级封装方法 |
| SecureLibraryLoader | 4 | 仅有native声明，缺少高级封装方法 |

### navi 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| AisTarget | 12 | 仅有native声明，缺少高级封装方法 |
| CollisionAssessor | 7 | 仅有native声明，缺少高级封装方法 |
| NmeaParser | 4 | 仅有native声明，缺少高级封装方法 |
| OffCourseDetector | 5 | 仅有native声明，缺少高级封装方法 |
| PositionManager | 13 | 仅有native声明，缺少高级封装方法 |
| PositionProvider | 6 | 仅有native声明，缺少高级封装方法 |
| RouteManager | 9 | 仅有native声明，缺少高级封装方法 |
| RoutePlanner | 3 | 仅有native声明，缺少高级封装方法 |
| Track | 10 | 仅有native声明，缺少高级封装方法 |
| TrackRecorder | 10 | 仅有native声明，缺少高级封装方法 |

### parser 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| ChartParser | 3 | 仅有native声明，缺少高级封装方法 |
| IParser | 7 | 仅有native声明，缺少高级封装方法 |
| IncrementalParser | 12 | 仅有native声明，缺少高级封装方法 |
| S57Parser | 7 | 仅有native声明，缺少高级封装方法 |

### perf 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| PerformanceMonitor | 7 | 仅有native声明，缺少高级封装方法 |
| RenderStats | 1 | 仅有native声明，缺少高级封装方法 |

### proj 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| CoordTransformer | 10 | 仅有native声明，缺少高级封装方法 |
| TransformMatrix | 14 | 仅有native声明，缺少高级封装方法 |

### recovery 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| CircuitBreaker | 7 | 仅有native声明，缺少高级封装方法 |
| ErrorRecoveryManager | 7 | 仅有native声明，缺少高级封装方法 |
| GracefulDegradation | 7 | 仅有native声明，缺少高级封装方法 |

### symbology 模块

| 类名 | native方法数 | 缺失描述 |
|------|------------|---------|
| ComparisonFilter | 6 | 仅有native声明，缺少高级封装方法 |
| Filter | 5 | 仅有native声明，缺少高级封装方法 |
| LogicalFilter | 2 | 仅有native声明，缺少高级封装方法 |
| SymbolizerRule | 15 | 仅有native声明，缺少高级封装方法 |
