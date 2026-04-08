# 海图显示系统设计文档

**文档编号**: CDS-DESIGN-001  
**版本**: v1.0  
**日期**: 2026-04-08  
**状态**: 正式版  
**需求文档**: [chart_display_system.md](../../doc/yangzt/chart_display_system.md)  
**实现分析**: [how_to_realise_cds.md](how_to_realise_cds.md)

---

## 文档修订历史

| 版本 | 日期 | 作者 | 修订内容 |
|------|------|------|----------|
| v1.0 | 2026-04-08 | Team | 初始版本，基于需求说明书和实现分析文档转换 |

---

## 目录

1. [概述](#1-概述)
2. [系统架构设计](#2-系统架构设计)
3. [模块设计](#3-模块设计)
4. [接口设计](#4-接口设计)
5. [数据结构设计](#5-数据结构设计)
6. [算法设计](#6-算法设计)
7. [性能设计](#7-性能设计)
8. [安全设计](#8-安全设计)
9. [部署设计](#9-部署设计)
10. [测试设计](#10-测试设计)
11. [附录](#11-附录)

---

## 1. 概述

### 1.1 文档目的

本文档是海图显示系统(Chart Display System, CDS)的详细设计文档，基于需求说明书和实现分析文档转换而成，为开发团队提供系统实现的详细技术规范。

### 1.2 系统定位

海图显示系统是整个海图导航平台的核心基础模块，负责海图数据的解析、存储、渲染和交互显示。系统需支持S57、S101及其他S100系列数据标准，实现跨平台（桌面端、移动端、Web端）统一渲染内核。

### 1.3 设计目标

| 目标 | 具体指标 |
|-----|---------|
| 渲染性能 | 百万级要素渲染时间<300ms |
| 数据支持 | S57、S101、S100系列数据格式 |
| 平台覆盖 | Windows、Linux、Android、Web |
| 离线能力 | 各端支持离线海图显示 |
| 显示精度 | 支持高精度地图显示（定位精度<1m） |

### 1.4 适用范围

- 内河航道（长江、珠江、京杭运河等）
- 湖泊水域（洞庭湖、太湖等）
- 入海口区域
- 近海区域

### 1.5 术语定义

| 术语 | 定义 |
|------|------|
| CDS | Chart Display System，海图显示系统 |
| S57 | IHO S-57电子海图数据标准 |
| S101 | IHO S-101电子海图数据标准（S57的替代标准） |
| S52 | IHO S-52电子海图显示规范 |
| JNI | Java Native Interface，Java本地接口 |
| PAL | Platform Abstraction Layer，平台抽象层 |

---

## 2. 系统架构设计

### 2.1 整体架构

系统采用分层架构设计，从上到下分为应用层、桥接层、业务功能层、核心基础层、数据存储层和基础工具层。

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           系统整体架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    应用层 (Application Layer)                        │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ Android App   │  │  JavaFX App   │  │  Web App      │           │   │
│  │  │  (待开发)     │  │  (待开发)     │  │  (待开发)     │           │   │
│  │  └───────┬───────┘  └───────┬───────┘  └───────────────┘           │   │
│  └──────────┼──────────────────┼──────────────────────────────────────┘   │
│             │                  │                                           │
│             ▼                  ▼                                           │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    桥接层 (Bridge Layer)                             │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ java_api      │  │  jni_bridge   │  │android_adapter│           │   │
│  │  │ (框架完成)    │  │  (框架完成)   │  │  (框架完成)   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│         ┌──────────────────────────┼──────────────────────────┐            │
│         ▼                          ▼                          ▼            │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    业务功能层 (Business Layer)                       │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ parser        │  │  navi         │  │  alarm        │           │   │
│  │  │ ✅ 完成       │  │  ✅ 完成      │  │  ✅ 完成      │           │   │
│  │  │ S57/S101解析  │  │  导航/航线    │  │ 警报服务层    │           │   │
│  │  └───────┬───────┘  └───────────────┘  └───────┬───────┘           │   │
│  │          │                                        │                  │   │
│  │          │                                        ▼ 依赖             │   │
│  │          │                              ┌─────────────────┐         │   │
│  │          │                              │ alert ✅ 完成   │         │   │
│  │          │                              │ 警报核心层      │         │   │
│  │          │                              └────────┬────────┘         │   │
│  └──────────┼───────────────────────────────────────┼──────────────────┘   │
│             │                                        │                      │
│             ▼                                        ▼                      │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    核心基础层 (Core Layer)                           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ graph ✅      │  │ draw ✅       │  │ symbology ✅  │           │   │
│  │  │ 渲染门面      │  │ 绘图引擎      │  │ 符号化        │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ cache ✅      │  │ layer ✅      │  │ feature ✅    │           │   │
│  │  │ 瓦片缓存      │  │ 图层管理      │  │ 要素模型      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    数据存储层 (Data Layer)                           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ database ✅   │  │  geom ✅      │  │  proj ✅      │           │   │
│  │  │ 数据库访问    │  │  几何操作     │  │  坐标转换     │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    基础工具层 (Utility Layer)                        │   │
│  │  ┌───────────────────────────────────────────────────────────────┐ │   │
│  │  │ base ✅ 完成                                                   │ │   │
│  │  │ 日志、线程安全、性能监控、内存管理                             │ │   │
│  │  └───────────────────────────────────────────────────────────────┘ │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 模块依赖关系

| 模块 | 依赖模块 | 说明 |
|------|----------|------|
| parser | feature, geom | 海图解析独立路径 |
| alarm | alert | 警报服务层依赖警报核心层 |
| graph | draw, symbology, cache, layer | 渲染门面依赖绘图和缓存 |
| layer | feature, database | 图层依赖要素和数据库 |
| feature | geom | 要素依赖几何 |
| database | base | 数据库依赖基础工具 |

### 2.3 技术选型

| 技术领域 | 选型 | 说明 |
|----------|------|------|
| 编程语言 | C++11 / Java 17 | 核心模块C++，应用层Java |
| 渲染技术 | OpenGL ES 3.0 / JavaFX Canvas | 移动端OpenGL ES，桌面端Canvas |
| 数据存储 | SQLite / PostGIS | 本地SQLite，服务端PostGIS |
| 坐标转换 | PROJ 9.x | 专业坐标转换库 |
| 构建工具 | CMake / Gradle | C++用CMake，Android用Gradle |
| 测试框架 | GoogleTest / JUnit | C++用GoogleTest，Java用JUnit |

---

## 3. 模块设计

### 3.1 已实现模块清单

| 模块 | 实现状态 | 核心类 | DLL/LIB |
|------|----------|--------|---------|
| **base** | ✅ 完成 | Logger, ThreadSafe, PerformanceMonitor | ogc_base.dll |
| **proj** | ✅ 完成 | CoordinateTransformer, ProjTransformer | ogc_proj.dll |
| **cache** | ✅ 完成 | TileCache, TileKey, OfflineStorageManager | ogc_cache.dll |
| **symbology** | ✅ 完成 | Symbolizer, Filter, SymbolizerRule | ogc_symbology.dll |
| **geom** | ✅ 完成 | Geometry, Point, LineString, Polygon | ogc_geom.dll |
| **database** | ✅ 完成 | DbConnection, DbConnectionPool | ogc_database.dll |
| **feature** | ✅ 完成 | CNFeature, CNFeatureDefn, CNFieldValue | ogc_feature.dll |
| **layer** | ✅ 完成 | CNLayer, CNVectorLayer, CNDataSource | ogc_layer.dll |
| **draw** | ✅ 完成 | DrawEngine, DrawDevice, DrawContext | ogc_draw.dll |
| **graph** | ✅ 完成 | DrawFacade, RenderTask, LabelEngine | ogc_graph.dll |
| **alert** | ✅ 完成 | AlertEngine, IAlertChecker, CpaCalculator | ogc_alert.dll |
| **navi** | ✅ 完成 | Route, AisManager, NavigationEngine | ogc_navi.dll |
| **parser** | ✅ 完成 | ChartParser, S57Parser, S101Parser | chart_parser.dll |

### 3.2 待开发模块

| 模块 | 开发优先级 | 预估工时 | 说明 |
|------|------------|----------|------|
| **S52样式库** | P0 | 2周 | S52显示规范实现 |
| **符号库** | P0 | 2周 | 海图符号渲染 |
| **JavaFX适配** | P0 | 4周 | 桌面端适配层 |
| **Android适配** | P1 | 6周 | 移动端适配层 |

### 3.3 JavaFX应用架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           JavaFX应用架构                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX应用层 (JavaFX Application)                 │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ MainView      │  │ ChartView     │  │ SettingsView  │           │   │
│  │  │ 主界面        │  │ 海图视图      │  │ 设置界面      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ LayerControl  │  │ PositionView  │  │ AlertPanel    │           │   │
│  │  │ 图层控制      │  │ 船位显示      │  │ 警报面板      │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    JavaFX适配层 (JavaFX Adapters)                    │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │CanvasAdapter  │  │EventHandler   │  │ ImageDevice   │           │   │
│  │  │ - DrawDevice  │  │ - MouseHandler│  │ - RasterDevice│           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ JNI                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java API层 (java_api模块)                         │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartViewer   │  │ ChartConfig   │  │ FeatureInfo   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C++核心层 (Native Libraries)                      │   │
│  │  ogc_graph | ogc_draw | ogc_layer | ogc_symbology | ogc_cache      │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.4 Android应用架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Android应用架构                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Android应用层 (Kotlin/Java)                       │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ MainActivity  │  │ ChartFragment │  │ SettingsAct   │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ ChartView     │  │ PositionView  │  │ LayerControl  │           │   │
│  │  │ (OpenGL ES)   │  │ (船位显示)    │  │ (图层控制)    │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ JNI                                    │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Java API层 (java_api模块)                         │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    Android适配层 (android_adapter模块)               │   │
│  │  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐           │   │
│  │  │ AndroidEngine │  │ AndroidTouch  │  │ AndroidLife   │           │   │
│  │  │ OpenGL ES渲染 │  │ 触摸事件处理  │  │ 生命周期管理  │           │   │
│  │  └───────────────┘  └───────────────┘  └───────────────┘           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    C++核心层 (Native Libraries)                      │   │
│  │  libogc_graph | libogc_draw | libogc_parser | libogc_layer         │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 4. 接口设计

### 4.1 核心接口定义

#### 4.1.1 渲染设备接口

```cpp
namespace ogc::pal {

class IRenderDevice {
public:
    virtual ~IRenderDevice() = default;
    virtual bool Initialize(int width, int height) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void DrawGeometry(const Geometry* geom, const DrawStyle& style) = 0;
    virtual void DrawText(const std::string& text, const Point& pos, const TextStyle& style) = 0;
    virtual void DrawImage(const Image* img, const Envelope& bounds) = 0;
    virtual void* GetNativeHandle() = 0;
};

}
```

#### 4.1.2 文件系统接口

```cpp
namespace ogc::pal {

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual bool FileExists(const std::string& path) = 0;
    virtual std::unique_ptr<std::istream> OpenRead(const std::string& path) = 0;
    virtual std::unique_ptr<std::ostream> OpenWrite(const std::string& path) = 0;
    virtual std::string GetAppDataDir() = 0;
    virtual std::string GetCacheDir() = 0;
};

}
```

#### 4.1.3 线程管理接口

```cpp
namespace ogc::pal {

class IThreadManager {
public:
    virtual ~IThreadManager() = default;
    virtual void RunOnMainThread(std::function<void()> task) = 0;
    virtual void RunOnBackground(std::function<void()> task) = 0;
    virtual bool IsMainThread() = 0;
};

}
```

### 4.2 Java API设计

#### 4.2.1 ChartViewer类

```java
package ogc.chart;

public class ChartViewer {
    static {
        System.loadLibrary("ogc_chart_jni");
    }
    
    public ChartViewer();
    public void loadChart(String path) throws ChartException;
    public void render(Object canvas, double width, double height);
    public void setViewport(double minX, double minY, double maxX, double maxY);
    public void pan(double dx, double dy);
    public void zoom(double factor, double centerX, double centerY);
    public FeatureInfo queryFeature(double x, double y);
    public void setDisplayMode(DisplayMode mode);
    public void close();
}
```

#### 4.2.2 ChartConfig类

```java
package ogc.chart;

public class ChartConfig {
    private DisplayMode displayMode = DisplayMode.DAY;
    private double minZoom = 0.001;
    private double maxZoom = 100.0;
    private boolean showDepthContours = true;
    private boolean showAidsToNavigation = true;
    
    public void setDisplayMode(DisplayMode mode);
    public void setZoomRange(double min, double max);
    public void setLayerVisible(String layerName, boolean visible);
    public DisplayMode getDisplayMode();
}
```

### 4.3 插件接口设计

```cpp
namespace ogc::plugin {

struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::vector<std::string> supportedFormats;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual PluginInfo GetInfo() const = 0;
    virtual bool Initialize(PluginContext* context) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
};

class IChartParserPlugin : public IPlugin {
public:
    virtual bool CanParse(const std::string& format) const = 0;
    virtual std::unique_ptr<IChartParser> CreateParser(const std::string& format) = 0;
};

}
```

---

## 5. 数据结构设计

### 5.1 核心数据结构

#### 5.1.1 几何对象模型

```cpp
namespace ogc {

class Geometry {
public:
    enum class GeometryType {
        Point, LineString, Polygon,
        MultiPoint, MultiLineString, MultiPolygon,
        GeometryCollection
    };
    
    virtual GeometryType GetGeometryType() const = 0;
    virtual Envelope GetEnvelope() const = 0;
    virtual std::string AsText() const = 0;
    virtual Geometry* Clone() const = 0;
    virtual bool IsEmpty() const = 0;
};

class Point : public Geometry {
private:
    double m_x, m_y, m_z;
    bool m_hasZ;
};

class LineString : public Geometry {
private:
    std::vector<Coordinate> m_coords;
};

class Polygon : public Geometry {
private:
    std::unique_ptr<LinearRing> m_exteriorRing;
    std::vector<std::unique_ptr<LinearRing>> m_interiorRings;
};

}
```

#### 5.1.2 要素模型

```cpp
namespace ogc {

class CNFeature {
public:
    long GetFID() const;
    CNFeatureDefn* GetDefnRef();
    int GetFieldCount() const;
    CNFieldValue* GetField(int i);
    Geometry* GetGeometryRef();
    void SetGeometry(Geometry* geom);
    void SetField(int i, const CNFieldValue& value);
    
private:
    long m_fid;
    CNFeatureDefn* m_defn;
    std::unique_ptr<Geometry> m_geometry;
    std::vector<CNFieldValue> m_fields;
};

}
```

#### 5.1.3 瓦片模型

```cpp
namespace ogc::cache {

struct TileKey {
    int x, y, z;
    
    bool operator<(const TileKey& other) const {
        if (z != other.z) return z < other.z;
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    std::string ToString() const {
        return std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y);
    }
};

struct TileData {
    TileKey key;
    std::vector<uint8_t> data;
    int64_t timestamp;
    bool isValid;
};

}
```

### 5.2 配置数据结构

```cpp
namespace ogc::config {

struct DisplayConfig {
    DisplayMode mode;
    double minZoom;
    double maxZoom;
    std::map<std::string, bool> layerVisibility;
    std::map<std::string, double> layerOpacity;
};

struct CacheConfig {
    size_t memoryCacheSize;
    size_t diskCacheSize;
    std::string cacheDir;
    int maxAge;
};

struct RenderConfig {
    int targetFps;
    bool enableAntialiasing;
    bool enableLabelCollision;
    int labelMinZoom;
};

}
```

---

## 6. 算法设计

### 6.1 瓦片调度算法

```cpp
class TileScheduler {
public:
    std::vector<TileKey> CalculateVisibleTiles(const Viewport& viewport) {
        std::vector<TileKey> tiles;
        
        int z = CalculateZoomLevel(viewport.scale);
        auto bounds = viewport.GetBounds();
        
        int minX = LonToTileX(bounds.minX, z);
        int maxX = LonToTileX(bounds.maxX, z);
        int minY = LatToTileY(bounds.maxY, z);
        int maxY = LatToTileY(bounds.minY, z);
        
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                tiles.push_back({x, y, z});
            }
        }
        
        return tiles;
    }
    
private:
    int CalculateZoomLevel(double scale);
    int LonToTileX(double lon, int z);
    int LatToTileY(double lat, int z);
};
```

### 6.2 缓存淘汰算法

```cpp
template<typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) : m_capacity(capacity) {}
    
    V* Get(const K& key) {
        auto it = m_cache.find(key);
        if (it == m_cache.end()) return nullptr;
        
        m_order.splice(m_order.begin(), m_order, it->second.second);
        return &it->second.first;
    }
    
    void Put(const K& key, V value) {
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            it->second.first = std::move(value);
            m_order.splice(m_order.begin(), m_order, it->second.second);
            return;
        }
        
        if (m_cache.size() >= m_capacity) {
            Evict();
        }
        
        m_order.push_front(key);
        m_cache[key] = {std::move(value), m_order.begin()};
    }
    
private:
    void Evict() {
        K key = m_order.back();
        m_order.pop_back();
        m_cache.erase(key);
    }
    
    size_t m_capacity;
    std::list<K> m_order;
    std::map<K, std::pair<V, typename std::list<K>::iterator>> m_cache;
};
```

### 6.3 空间索引算法

```cpp
class RTree {
public:
    std::vector<Feature*> Query(const Envelope& bounds) {
        std::vector<Feature*> results;
        QueryNode(m_root.get(), bounds, results);
        return results;
    }
    
    void Insert(Feature* feature) {
        InsertNode(m_root.get(), feature);
    }
    
private:
    void QueryNode(Node* node, const Envelope& bounds, std::vector<Feature*>& results);
    void InsertNode(Node* node, Feature* feature);
    Node* ChooseLeaf(Node* node, const Envelope& bounds);
    void SplitNode(Node* node);
    
    std::unique_ptr<Node> m_root;
};
```

---

## 7. 性能设计

### 7.1 性能指标

| 指标类别 | 指标名称 | 目标值 | 测量方法 |
|----------|----------|--------|----------|
| **渲染性能** | 帧率 | ≥30fps | 帧计数器统计 |
| | 首帧渲染时间 | <500ms | 从加载到首帧完成 |
| | 瓦片渲染时间 | <50ms/瓦片 | 单瓦片渲染耗时 |
| **加载性能** | 单张海图加载 | <2s | 从文件到内存 |
| | 瓦片加载时间 | <100ms | 从缓存到显示 |
| | 离线数据初始化 | <5s | 应用启动到可用 |
| **内存性能** | 峰值内存(Android) | <512MB | 内存分析工具 |
| | 峰值内存(桌面) | <1GB | 内存分析工具 |
| | 缓存内存上限 | 256MB | 可配置 |
| **响应性能** | 平移响应延迟 | <100ms | 触摸到画面更新 |
| | 缩放响应延迟 | <150ms | 手势到画面更新 |
| | 要素查询响应 | <50ms | 点击到信息显示 |

### 7.2 多级缓存架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           多级缓存架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    L1: 热数据缓存 (Hot Cache)                        │   │
│  │  - 容量: 64MB                                                        │   │
│  │  - 策略: LRU with Frequency                                         │   │
│  │  - 内容: 当前视口瓦片、最近访问要素                                  │   │
│  │  - 命中率目标: > 80%                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ 未命中                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    L2: 内存缓存 (Memory Cache)                       │   │
│  │  - 容量: 256MB (可配置)                                              │   │
│  │  - 策略: LRU with Size Awareness                                    │   │
│  │  - 内容: 渲染瓦片、符号化结果                                        │   │
│  │  - 命中率目标: > 60%                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│                                    ▼ 未命中                                 │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    L3: 磁盘缓存 (Disk Cache)                         │   │
│  │  - 容量: 2GB (可配置)                                                │   │
│  │  - 策略: Time-based LRU                                             │   │
│  │  - 内容: 原始瓦片数据、解析结果                                      │   │
│  │  - 命中率目标: > 40%                                                 │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7.3 性能优化策略

| 优化策略 | 实现方式 | 预期效果 |
|----------|----------|----------|
| 瓦片预加载 | 预加载相邻瓦片 | 减少等待时间 |
| 异步渲染 | 后台线程渲染 | 提高响应速度 |
| LOD渲染 | 多级细节渲染 | 降低渲染负载 |
| 批处理渲染 | 合并绘制调用 | 提高GPU利用率 |
| 纹理压缩 | ETC2/ASTC压缩 | 减少内存占用 |

---

## 8. 安全设计

### 8.1 数据安全

| 安全措施 | 实现方式 | 适用场景 |
|----------|----------|----------|
| 数据加密 | AES-256加密存储 | 敏感海图数据 |
| 访问控制 | 基于角色的权限管理 | 多用户环境 |
| 数据校验 | SHA-256完整性校验 | 数据传输/存储 |

### 8.2 传输安全

| 安全措施 | 实现方式 | 适用场景 |
|----------|----------|----------|
| 传输加密 | TLS 1.3 | 远程数据访问 |
| 证书验证 | 双向认证 | 安全通信 |
| 请求签名 | HMAC-SHA256 | API调用 |

### 8.3 内存安全

| 安全措施 | 实现方式 | 适用场景 |
|----------|----------|----------|
| 边界检查 | 安全容器、迭代器 | 数据处理 |
| 内存清理 | 敏感数据清零 | 密钥/密码处理 |
| 防泄漏 | 智能指针、RAII | 全部模块 |

### 8.4 合规要求

| 要求 | 说明 |
|-----|-----|
| 地理信息保密 | 符合国家地理信息保密规定 |
| 坐标偏转 | 涉密区域坐标偏转处理 |
| 数据脱敏 | 敏感数据脱敏处理 |

---

## 9. 部署设计

### 9.1 开发环境要求

| 组件 | 最低版本 | 推荐版本 |
|------|----------|----------|
| MSVC | 2017 | 2022 |
| GCC | 7.0 | 11.0 |
| Java | 11 | 17 LTS |
| C++标准 | C++11 | C++17 |

### 9.2 依赖库版本

| 依赖库 | 最低版本 | 推荐版本 |
|--------|----------|----------|
| PROJ | 8.0 | 9.2+ |
| GDAL | 3.6 | 3.8+ |
| SQLite | 3.35 | 3.40+ |
| libspatialite | 5.0 | 5.1+ |
| libpq | 14 | 16+ |

### 9.3 部署方式

| 平台 | 部署方式 | 工具 | 输出 |
|------|----------|------|------|
| JavaFX | 独立包 | jlink + jpackage | 安装程序 |
| Android | APK/AAB | Gradle | .apk/.aab |
| Web | WebAssembly | Emscripten | .wasm |

### 9.4 更新机制

| 更新类型 | 触发方式 | 实现方式 |
|----------|----------|----------|
| 应用更新 | 用户确认 | 应用商店/自更新 |
| 海图更新 | 自动检测 | 增量下载 |
| 配置更新 | 后台同步 | 云端配置 |

---

## 10. 测试设计

### 10.1 测试层次

| 测试层次 | 覆盖范围 | 工具 | 目标覆盖率 |
|----------|----------|------|------------|
| 单元测试 | 单个类/函数 | GoogleTest | > 80% |
| 集成测试 | 模块间交互 | GoogleTest | > 70% |
| 功能测试 | 端到端功能 | 手动/自动化 | 100%用例 |
| 性能测试 | 性能指标 | 自定义工具 | 满足指标 |
| 兼容性测试 | 多平台/多版本 | CI/CD | 全部通过 |

### 10.2 测试用例分类

| 类别 | 测试内容 | 优先级 |
|------|----------|--------|
| 海图加载 | S57/S101解析、显示 | P0 |
| 渲染正确性 | 几何、符号、标签渲染 | P0 |
| 交互功能 | 平移、缩放、选择 | P0 |
| 渲染性能 | 帧率、响应时间 | P0 |
| 内存使用 | 峰值、泄漏检测 | P0 |
| 平台兼容 | JavaFX/Android | P0 |

### 10.3 性能测试指标

| 指标 | 测试方法 | 通过标准 |
|------|----------|----------|
| 帧率 | 连续渲染10分钟 | ≥30fps，无卡顿 |
| 内存峰值 | 加载大型海图 | <512MB(Android)，<1GB(桌面) |
| 加载时间 | 冷启动加载 | <5s到可用状态 |
| 响应延迟 | 平移/缩放操作 | <100ms响应 |

---

## 11. 附录

### 11.1 实施路线图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           实施路线图 (20周)                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  第0-2周    第3-5周    第6-8周    第9-12周   第13-16周  第17-20周           │
│  ├──基础───┼──渲染───┼──数据───┼──交互───┼──样式───┼──Android──┤           │
│  │ 框架    │ 核心    │ 处理    │ 功能    │ 符号    │ 迁移      │           │
│                                                                             │
│  里程碑:                                                                    │
│  M0(第2周): 环境准备完成                                                    │
│  M1(第5周): JavaFX框架可运行                                                │
│  M2(第8周): 基本渲染功能完成                                                │
│  M3(第12周): 数据加载+交互功能完成                                          │
│  M4(第16周): S52样式完整                                                    │
│  M5(第20周): Android版本发布                                                │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 11.2 里程碑定义

| 里程碑 | 时间 | 交付物 | 验收标准 |
|--------|------|--------|----------|
| M0 | 第2周 | 环境准备 | 开发环境配置完成，依赖库编译通过，CI/CD流水线可用 |
| M1 | 第5周 | JavaFX框架 | 可运行，显示空白地图，启动时间<3s |
| M2 | 第8周 | 渲染功能 | 可渲染几何图形，帧率≥30fps，瓦片渲染<50ms |
| M3 | 第12周 | 数据+交互 | 可加载S57海图，要素显示正确率>95%，平移缩放帧率≥25fps |
| M4 | 第16周 | 样式完整 | S52样式覆盖率>90%，显示模式切换<500ms |
| M5 | 第20周 | Android版 | 功能完整度100%，帧率≥30fps，内存峰值<512MB |

### 11.3 风险评估

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| JNI内存泄漏 | 中 | 高 | 使用智能指针，定期内存分析 |
| 渲染性能不达标 | 中 | 高 | 提前性能测试，准备降级方案 |
| S52规范理解偏差 | 低 | 中 | 参考官方实现，早期验证 |
| 跨平台兼容问题 | 中 | 中 | 持续集成测试，多平台验证 |
| 工期延误 | 中 | 高 | 预留20%缓冲时间，优先核心功能 |

### 11.4 参考文档

| 文档 | 路径 |
|------|------|
| 需求说明书 | [chart_display_system.md](../../doc/yangzt/chart_display_system.md) |
| 实现分析文档 | [how_to_realise_cds.md](how_to_realise_cds.md) |
| 模块索引 | [code/index_all.md](../../code/index_all.md) |
| 编码规范 | [project_rules.md](../../.trae/rules/project_rules.md) |

---

**文档结束**
