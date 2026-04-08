# Layer Module - Header Index

## 模块描述

Layer模块提供矢量图层和栅格图层的抽象接口与实现，支持多种数据源格式（Shapefile、GeoJSON、PostGIS、GeoPackage、WFS等），提供图层组管理、数据源连接池、线程安全访问等功能。

## 核心特性

- 多数据源支持：Shapefile、GeoJSON、PostGIS、GeoPackage、WFS
- 图层抽象接口，支持矢量和栅格图层
- 图层组管理和图层快照
- 数据源连接池管理
- 线程安全图层访问
- GDAL/OGR适配器
- 要素流式读取

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/layer/export.h) | DLL导出宏 | `OGC_LAYER_API` |
| [layer_type.h](ogc/layer/layer_type.h) | 图层类型定义 | `CNLayerType`, `CNStatus`, `CNLayerCapability` |
| [layer.h](ogc/layer/layer.h) | 图层抽象基类 | `CNLayer` |
| [vector_layer.h](ogc/layer/vector_layer.h) | 矢量图层 | `CNVectorLayer` |
| [raster_layer.h](ogc/layer/raster_layer.h) | 栅格图层 | `CNRasterLayer` |
| [raster_dataset.h](ogc/layer/raster_dataset.h) | 栅格数据集 | `CNRasterDataset` |
| [memory_layer.h](ogc/layer/memory_layer.h) | 内存图层 | `CNMemoryLayer` |
| [layer_group.h](ogc/layer/layer_group.h) | 图层组 | `CNLayerGroup` |
| [datasource.h](ogc/layer/datasource.h) | 数据源 | `CNDataSource` |
| [driver.h](ogc/layer/driver.h) | 驱动接口 | `CNDriver` |
| [driver_manager.h](ogc/layer/driver_manager.h) | 驱动管理器 | `CNDriverManager` |
| [shapefile_layer.h](ogc/layer/shapefile_layer.h) | Shapefile图层 | `ShapefileLayer` |
| [geojson_layer.h](ogc/layer/geojson_layer.h) | GeoJSON图层 | `GeoJsonLayer` |
| [postgis_layer.h](ogc/layer/postgis_layer.h) | PostGIS图层 | `PostgisLayer` |
| [geopackage_layer.h](ogc/layer/geopackage_layer.h) | GeoPackage图层 | `GeoPackageLayer` |
| [wfs_layer.h](ogc/layer/wfs_layer.h) | WFS图层 | `WfsLayer` |
| [gdal_adapter.h](ogc/layer/gdal_adapter.h) | GDAL适配器 | `GdalAdapter` |
| [connection_pool.h](ogc/layer/connection_pool.h) | 连接池 | `LayerConnectionPool` |
| [thread_safe_layer.h](ogc/layer/thread_safe_layer.h) | 线程安全图层 | `ThreadSafeLayer` |
| [layer_snapshot.h](ogc/layer/layer_snapshot.h) | 图层快照 | `LayerSnapshot` |
| [layer_observer.h](ogc/layer/layer_observer.h) | 图层观察者 | `LayerObserver` |
| [feature_stream.h](ogc/layer/feature_stream.h) | 要素流 | `FeatureStream` |
| [read_write_lock.h](ogc/layer/read_write_lock.h) | 读写锁 | `ReadWriteLock` |
| [geometry_compat.h](ogc/layer/geometry_compat.h) | 几何兼容层 | `CNGeometry` |

---

## 类继承关系图

```
CNLayer (abstract)
    ├── CNVectorLayer (abstract)
    │       ├── ShapefileLayer
    │       ├── GeoJsonLayer
    │       ├── PostgisLayer
    │       ├── GeoPackageLayer
    │       ├── WfsLayer
    │       └── CNMemoryLayer
    └── CNRasterLayer
            └── CNRasterDataset

CNDataSource (abstract)
    ├── ShapefileDataSource
    ├── GeoJsonDataSource
    ├── PostgisDataSource
    └── GeoPackageDataSource

CNDriver (abstract)
    ├── ShapefileDriver
    ├── GeoJsonDriver
    └── PostgisDriver
```

---

## 依赖关系图

```
layer_type.h
    │
    └──► layer.h ──► vector_layer.h
             │              │
             │              ├──► shapefile_layer.h
             │              ├──► geojson_layer.h
             │              ├──► postgis_layer.h
             │              ├──► geopackage_layer.h
             │              └──► memory_layer.h
             │
             ├──► raster_layer.h ──► raster_dataset.h
             │
             ├──► layer_group.h
             │
             └──► datasource.h ──► driver.h ──► driver_manager.h
                    │
                    └──► connection_pool.h
                    
thread_safe_layer.h ──► read_write_lock.h
layer_snapshot.h
layer_observer.h
feature_stream.h
gdal_adapter.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | layer_type.h, layer.h, vector_layer.h, raster_layer.h |
| **Data Source** | datasource.h, driver.h, driver_manager.h |
| **Format Layers** | shapefile_layer.h, geojson_layer.h, postgis_layer.h, geopackage_layer.h, wfs_layer.h |
| **Memory** | memory_layer.h, raster_dataset.h |
| **Group** | layer_group.h |
| **Thread Safety** | thread_safe_layer.h, read_write_lock.h, connection_pool.h |
| **Utility** | gdal_adapter.h, layer_snapshot.h, layer_observer.h, feature_stream.h |

---

## 关键类

### CNLayer (抽象基类)
**File**: [layer.h](ogc/layer/layer.h)  
**Description**: 图层抽象基类

```cpp
class CNLayer {
public:
    virtual const std::string& GetName() const = 0;
    virtual CNLayerType GetLayerType() const = 0;
    virtual CNFeatureDefn* GetFeatureDefn() = 0;
    virtual const CNFeatureDefn* GetFeatureDefn() const = 0;
    virtual GeomType GetGeomType() const = 0;
    virtual void* GetSpatialRef() = 0;
    virtual const void* GetSpatialRef() const = 0;
    virtual CNStatus GetExtent(Envelope& extent, bool force = true) const = 0;
    virtual int64_t GetFeatureCount(bool force = true) const = 0;
    
    // 读取接口
    virtual void ResetReading() = 0;
    virtual std::unique_ptr<CNFeature> GetNextFeature() = 0;
    virtual CNFeature* GetNextFeatureRef();
    virtual std::unique_ptr<CNFeature> GetFeature(int64_t fid) = 0;
    
    // 写入接口
    virtual CNStatus SetFeature(const CNFeature* feature);
    virtual CNStatus CreateFeature(CNFeature* feature);
    virtual CNStatus DeleteFeature(int64_t fid);
    virtual int64_t CreateFeatureBatch(const std::vector<CNFeature*>& features);
    virtual CNStatus CreateField(const CNFieldDefn* field_defn, bool approx_ok = false);
    virtual CNStatus DeleteField(int field_index);
    virtual CNStatus ReorderFields(const std::vector<int>& new_order);
    virtual CNStatus AlterFieldDefn(int field_index, const CNFieldDefn* new_defn, int flags);
    
    // 空间过滤
    virtual void SetSpatialFilterRect(double min_x, double min_y, double max_x, double max_y);
    virtual void SetSpatialFilter(const CNGeometry* geometry) = 0;
    virtual const CNGeometry* GetSpatialFilter() const = 0;
    virtual CNStatus SetAttributeFilter(const std::string& query) = 0;
    virtual void ClearAttributeFilter();
    
    // 事务
    virtual CNStatus StartTransaction();
    virtual CNStatus CommitTransaction();
    virtual CNStatus RollbackTransaction();
    
    // 能力检测
    virtual bool TestCapability(CNLayerCapability capability) const = 0;
    virtual std::vector<CNLayerCapability> GetCapabilities() const;
    
    // 元数据
    virtual std::string GetMetadata(const std::string& key) const;
    virtual CNStatus SetMetadata(const std::string& key, const std::string& value);
    
    // 其他
    virtual CNStatus SyncToDisk();
    virtual std::unique_ptr<CNLayer> Clone() const = 0;
};
```

### CNVectorLayer
**File**: [vector_layer.h](ogc/layer/vector_layer.h)  
**Base**: `CNLayer`  
**Description**: 矢量图层

```cpp
class CNVectorLayer : public CNLayer {
public:
    CNLayerType GetLayerType() const override { return CNLayerType::kVector; }
    virtual std::string GetDataSourcePath() const = 0;
    virtual std::string GetFormatName() const = 0;
    virtual bool IsReadOnly() const = 0;
};
```

### CNDataSource
**File**: [datasource.h](ogc/layer/datasource.h)  
**Description**: 数据源抽象类

```cpp
class CNDataSource {
public:
    virtual const std::string& GetName() const = 0;
    virtual std::string GetPath() const = 0;
    virtual bool IsReadOnly() const = 0;
    virtual int GetLayerCount() const = 0;
    virtual CNLayer* GetLayer(int index) = 0;
    virtual CNLayer* GetLayerByName(const std::string& name) = 0;
    virtual CNLayer* CreateLayer(const std::string& name, GeomType geomType, void* srs) = 0;
    
    static std::unique_ptr<CNDataSource> Open(const std::string& path, bool update = false);
};
```

### CNLayerGroup
**File**: [layer_group.h](ogc/layer/layer_group.h)  
**Description**: 图层组

```cpp
class CNLayerGroup {
public:
    void AddLayer(CNLayer* layer);
    void RemoveLayer(const std::string& name);
    CNLayer* GetLayer(const std::string& name) const;
    size_t GetLayerCount() const;
    void SetVisible(bool visible);
    bool IsVisible() const;
};
```

---

## 接口

### CNLayer 读取方法
```cpp
virtual void ResetReading() = 0;
virtual std::unique_ptr<CNFeature> GetNextFeature() = 0;
virtual std::unique_ptr<CNFeature> GetFeature(int64_t fid) = 0;
```

### CNLayer 写入方法
```cpp
virtual CNStatus SetFeature(const CNFeature* feature);
virtual CNStatus CreateFeature(CNFeature* feature);
virtual CNStatus DeleteFeature(int64_t fid);
virtual CNStatus CreateField(const CNFieldDefn* field_defn, bool approx_ok = false);
```

### CNLayer 空间过滤方法
```cpp
virtual void SetSpatialFilterRect(double min_x, double min_y, double max_x, double max_y);
virtual void SetSpatialFilter(const CNGeometry* geometry) = 0;
virtual const CNGeometry* GetSpatialFilter() const = 0;
virtual CNStatus SetAttributeFilter(const std::string& query) = 0;
```

### CNLayer 事务方法
```cpp
virtual CNStatus StartTransaction();
virtual CNStatus CommitTransaction();
virtual CNStatus RollbackTransaction();
```

---

## 类型定义

### CNLayerType (图层类型枚举)
```cpp
enum class CNLayerType {
    kUnknown = 0,
    kMemory,
    kVector,
    kRaster,
    kGroup,
    kWFS,
    kWMS,
    kWMTS
};
```

### CNStatus (状态码)
```cpp
enum class CNStatus {
    kSuccess = 0,
    kError = 1,
    kInvalidParameter = 2,
    kNullPointer = 3,
    kOutOfRange = 4,
    kNotSupported = 5,
    kNotImplemented = 6,
    kInvalidFeature = 100,
    kInvalidGeometry = 101,
    kInvalidFID = 102,
    kFeatureNotFound = 103,
    kDuplicateFID = 104,
    kFieldNotFound = 105,
    kTypeMismatch = 106,
    kIOError = 200,
    kFileNotFound = 201,
    kFileExists = 202,
    kPermissionDenied = 203,
    kDiskFull = 204,
    kCorruptData = 205,
    kNoTransaction = 300,
    kTransactionNotActive = 300,
    kTransactionActive = 301,
    kCommitFailed = 302,
    kRollbackFailed = 303,
    kLockFailed = 400,
    kTimeout = 401,
    kDeadlock = 402,
    kOutOfMemory = 500,
    kBufferOverflow = 501
};
```

### CNLayerCapability (图层能力)
```cpp
enum class CNLayerCapability {
    kSequentialRead,
    kRandomRead,
    kFastSpatialFilter,
    kFastFeatureCount,
    kFastGetExtent,
    kSequentialWrite,
    kRandomWrite,
    kCreateFeature,
    kDeleteFeature,
    kCreateField,
    kDeleteField,
    kReorderFields,
    kAlterFieldDefn,
    kTransactions,
    kStringsAsUTF8,
    kIgnoreFields,
    kCurveGeometries
};
```

### CNSpatialFilterType (空间过滤类型)
```cpp
enum class CNSpatialFilterType {
    kNone,
    kRectangular,
    kGeometric
};
```

### CNSpatialRelation (空间关系)
```cpp
enum class CNSpatialRelation {
    kEquals,
    kDisjoint,
    kIntersects,
    kTouches,
    kCrosses,
    kWithin,
    kContains,
    kOverlaps,
    kCovers,
    kCoveredBy
};
```

### SpatialIndexType (空间索引类型)
```cpp
enum class SpatialIndexType {
    kNone,
    kRTree,
    kQuadTree,
    kGrid,
    kSTRtree
};
```

---

## 使用示例

### 打开数据源

```cpp
#include "ogc/layer/datasource.h"
#include "ogc/layer/shapefile_layer.h"

using namespace ogc;

// 打开Shapefile
auto ds = CNDataSource::Open("/path/to/data.shp");
if (ds) {
    CNLayer* layer = ds->GetLayer(0);
    // 处理图层
}
```

### 遍历要素

```cpp
#include "ogc/layer/layer.h"

layer->ResetReading();
while (auto feature = layer->GetNextFeature()) {
    // 处理要素
    int64_t fid = feature->GetFID();
    auto geom = feature->GetGeometry();
}
```

### 创建内存图层

```cpp
#include "ogc/layer/memory_layer.h"

auto memLayer = std::make_unique<CNMemoryLayer>("temp");
memLayer->CreateFeature(&feature);
```

### 图层组管理

```cpp
#include "ogc/layer/layer_group.h"

CNLayerGroup group;
group.AddLayer(layer1);
group.AddLayer(layer2);
group.SetVisible(true);

// 遍历组内图层
for (size_t i = 0; i < group.GetLayerCount(); ++i) {
    CNLayer* layer = group.GetLayer(i);
}
```

### 空间过滤

```cpp
// 设置矩形过滤
layer->SetSpatialFilterRect(0, 0, 100, 100);

// 设置几何过滤
layer->SetSpatialFilter(polygonGeom);

// 清除过滤
layer->SetSpatialFilter(nullptr);
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加WFS/WMS支持 |
| v1.2 | 2026-03-10 | Team | 添加线程安全图层 |
| v1.3 | 2026-04-06 | index-validator | 修正章节顺序、补充类型定义、修正接口名称 |
| v1.4 | 2026-04-09 | index-validator | 补充CNLayer类缺失方法(GetFeatureDefn const、GetSpatialRef、CreateFeatureBatch、GetCapabilities、Clone等) |

---

**Generated**: 2026-04-06  
**Module Version**: v1.3  
**C++ Standard**: C++11
