# Layer 模块 - 头文件索引

## 概述

Layer 模块提供图层（Layer）数据模型和访问接口，支持多种数据源（内存、文件、数据库、网络服务）。该模块是 OGC C++ Library 的核心模块之一，抽象了各种矢量/栅格数据的访问方式。

## 头文件列表

| 文件 | 描述 | 核心类/结构 |
|------|-------------|-------------|
| [export.h](include/ogc/layer/export.h) | 导出宏定义 | `OGC_LAYER_API` |
| [layer.h](include/ogc/layer/layer.h) | 图层抽象基类 | `CNLayer` |
| [layer_type.h](include/ogc/layer/layer_type.h) | 图层类型枚举 | `CNLayerType`, `CNStatus` |
| [datasource.h](include/ogc/layer/datasource.h) | 数据源抽象基类 | `CNDataSource` |
| [driver.h](include/ogc/layer/driver.h) | 驱动抽象基类 | `CNDriver` |
| [driver_manager.h](include/ogc/layer/driver_manager.h) | 驱动管理器 | `CNDriverManager` |
| [memory_layer.h](include/ogc/layer/memory_layer.h) | 内存图层 | `CNMemoryLayer` |
| [vector_layer.h](include/ogc/layer/vector_layer.h) | 矢量图层基类 | `CNVectorLayer` |
| [shapefile_layer.h](include/ogc/layer/shapefile_layer.h) | Shapefile支持 | `CNShapefileLayer` |
| [geojson_layer.h](include/ogc/layer/geojson_layer.h) | GeoJSON支持 | `CNGeoJSONLayer` |
| [geopackage_layer.h](include/ogc/layer/geopackage_layer.h) | GeoPackage支持 | `CNGeoPackageLayer` |
| [postgis_layer.h](include/ogc/layer/postgis_layer.h) | PostGIS图层 | `CNPostGISLayer` |
| [wfs_layer.h](include/ogc/layer/wfs_layer.h) | WFS客户端 | `CNWFSLayer` |
| [raster_layer.h](include/ogc/layer/raster_layer.h) | 栅格图层 | `CNRasterLayer` |
| [raster_dataset.h](include/ogc/layer/raster_dataset.h) | 栅格数据集 | `CNRasterDataset` |
| [connection_pool.h](include/ogc/layer/connection_pool.h) | 连接池 | `CNLayerConnectionPool` |
| [read_write_lock.h](include/ogc/layer/read_write_lock.h) | 读写锁 | `CNReadWriteLock` |
| [thread_safe_layer.h](include/ogc/layer/thread_safe_layer.h) | 线程安全包装 | `CNThreadSafeLayer` |
| [layer_snapshot.h](include/ogc/layer/layer_snapshot.h) | 图层快照 | `CNLayerSnapshot` |
| [layer_group.h](include/ogc/layer/layer_group.h) | 图层组 | `CNLayerGroup`, `CNLayerNode` |
| [layer_observer.h](include/ogc/layer/layer_observer.h) | 事件观察者 | `CNLayerObserver`, `CNObservableLayer` |
| [feature_stream.h](include/ogc/layer/feature_stream.h) | 流式读取 | `CNFeatureStream`, `CNBackpressureConfig` |
| [gdal_adapter.h](include/ogc/layer/gdal_adapter.h) | GDAL适配器 | `CNGDALAdapter` |

## 核心枚举

### CNLayerType
图层类型枚举。

```cpp
enum class CNLayerType {
    kUnknown = 0,
    kMemory,      // 内存图层
    kVector,      // 矢量图层
    kRaster,     // 栅格图层
    kGroup,      // 图层组
    kWFS,        // WFS服务
    kWMS,        // WMS服务
    kWMTS       // WMTS服务
};
```

### CNLayerCapability
图层能力枚举。

```cpp
enum class CNLayerCapability {
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

### CNStatus
操作状态枚举。

```cpp
enum class CNStatus {
    kSuccess = 0,
    kError = 1,
    kInvalidParameter = 2,
    kNullPointer = 3,
    kOutOfRange = 4,
    kNotSupported = 5,
    kNotImplemented = 6,
    // ... 更多状态码
};
```

## 类继承关系

```
CNLayer (抽象基类)
    ├── CNMemoryLayer
    ├── CNVectorLayer
    │       ├── CNShapefileLayer
    │       ├── CNGeoJSONLayer
    │       ├── CNGeoPackageLayer
    │       └── CNPostGISLayer
    ├── CNRasterLayer
    ├── CNWFSLayer
    └── CNThreadSafeLayer (装饰器)

CNDataSource (抽象基类)
    └── 实现类

CNLayerNode
    └── CNLayerGroup
```

## 核心类详细说明

### CNLayer (抽象基类)
所有图层类型的基类。

```cpp
class CNLayer {
public:
    virtual ~CNLayer() = default;
    
    virtual const std::string& GetName() const = 0;
    virtual CNLayerType GetLayerType() const = 0;
    virtual CNFeatureDefn* GetFeatureDefn() = 0;
    virtual GeomType GetGeomType() const = 0;
    virtual CNSpatialReference* GetSpatialRef() = 0;
    
    virtual CNStatus GetExtent(Envelope& extent, bool force = true) const = 0;
    virtual int64_t GetFeatureCount(bool force = true) const = 0;
    
    virtual void ResetReading() = 0;
    virtual std::unique_ptr<CNFeature> GetNextFeature() = 0;
    virtual std::unique_ptr<CNFeature> GetFeature(int64_t fid) = 0;
    
    virtual CNStatus SetFeature(const CNFeature* feature) = 0;
    virtual CNStatus CreateFeature(CNFeature* feature) = 0;
    virtual CNStatus DeleteFeature(int64_t fid) = 0;
    
    virtual CNStatus CreateField(const CNFieldDefn* field_defn, bool approx_ok = false) = 0;
    virtual CNStatus DeleteField(int field_index) = 0;
    
    virtual CNStatus StartTransaction() = 0;
    virtual CNStatus CommitTransaction() = 0;
    virtual CNStatus RollbackTransaction() = 0;
    
    virtual std::unique_ptr<CNLayer> Clone() const = 0;
};
```

### CNDataSource (抽象基类)
所有数据源的基类。

```cpp
class CNDataSource {
public:
    virtual ~CNDataSource() = default;
    
    virtual const std::string& GetName() const = 0;
    virtual std::string GetPath() const = 0;
    virtual bool IsReadOnly() const = 0;
    virtual const char* GetDriverName() const = 0;
    
    virtual int GetLayerCount() const = 0;
    virtual CNLayer* GetLayer(int index) = 0;
    virtual CNLayer* GetLayerByName(const std::string& name) = 0;
    
    virtual CNLayer* CreateLayer(
        const std::string& name,
        GeomType geom_type,
        void* srs = nullptr) = 0;
    
    virtual CNStatus DeleteLayer(const std::string& name) = 0;
    
    virtual CNStatus StartTransaction() = 0;
    virtual CNStatus CommitTransaction() = 0;
    virtual CNStatus RollbackTransaction() = 0;
};
```

### CNMemoryLayer
内存图层实现，支持高效的特征存储和操作。

```cpp
class CNMemoryLayer : public CNLayer {
public:
    CNMemoryLayer(
        const std::string& name,
        GeomType geom_type,
        void* srs = nullptr);

    CNMemoryLayer(
        const std::string& name,
        std::shared_ptr<CNFeatureDefn> feature_defn);

    ~CNMemoryLayer() override;

    // 内存图层特定方法
    void Reserve(int64_t expected_count);
    void ShrinkToFit();
    void Clear();
    int64_t GetMemoryUsage() const;
    void SetAutoFIDGeneration(bool auto_gen);
    void SetFIDReuse(bool reuse);
    void BuildSpatialIndex(SpatialIndexType index_type = SpatialIndexType::kRTree);
    ISpatialIndex<int64_t>* GetSpatialIndex();
};
```

### CNLayerGroup
图层组（树形结构）。

```cpp
class CNLayerGroup : public CNLayerNode {
public:
    CNLayerGroup() = default;
    explicit CNLayerGroup(const std::string& name);

    CNLayerNodeType GetNodeType() const override;
    
    size_t GetChildCount() const;
    CNLayerNode* GetChild(size_t index);
    void AddChild(std::unique_ptr<CNLayerNode> child);
    std::unique_ptr<CNLayerNode> RemoveChild(size_t index);
    CNLayerNode* FindChild(const std::string& name);
    
    CNStatus StartTransaction() override;
    CNStatus CommitTransaction() override;
    CNStatus RollbackTransaction() override;
};
```

### CNFeatureStream
要素流式读取接口，支持背压机制。

```cpp
class CNFeatureStream {
public:
    virtual ~CNFeatureStream() = default;
    
    virtual std::vector<std::unique_ptr<CNFeature>> ReadNextBatch(size_t batch_size = 1000) = 0;
    virtual bool IsEndOfStream() const = 0;
    virtual int64_t GetReadCount() const = 0;
    virtual void Reset() = 0;
    virtual void Close() = 0;
    virtual CNBackpressureStatus GetBackpressureStatus() const = 0;
};
```

### CNLayerObserver
图层事件观察者接口。

```cpp
class CNLayerObserver {
public:
    virtual ~CNLayerObserver() = default;
    
    virtual void OnLayerChanged(CNLayer* layer, const CNLayerEventArgs& args) = 0;
    virtual bool OnLayerChanging(CNLayer* layer, const CNLayerEventArgs& args);
};

enum class CNLayerEventType {
    kFeatureAdded,
    kFeatureDeleted,
    kFeatureModified,
    kSchemaChanged,
    kSpatialFilterChanged,
    kAttributeFilterChanged,
    kCleared,
    kTransactionStarted,
    kTransactionCommitted,
    kTransactionRolledBack
};
```

### CNBackpressureConfig
流式读取背压配置。

```cpp
enum class CNBackpressureStrategy {
    kNone,    // 无背压
    kBlock,   // 阻塞策略
    kDrop,    // 丢弃策略
    kBuffer,  // 缓冲策略
    kThrottle // 节流策略
};

struct CNBackpressureConfig {
    CNBackpressureStrategy strategy = CNBackpressureStrategy::kBlock;
    size_t max_buffer_size = 10000;
    int block_timeout_ms = 5000;
    double throttle_factor = 0.8;
    bool enable_monitoring = true;
};
```

## 使用示例

### 打开 Shapefile

```cpp
CNDriverManager& driverMgr = CNDriverManager::GetInstance();
CNDriver* driver = driverMgr.GetDriverByName("ESRI Shapefile");

CNDataSource* ds = driver->Open("D:/data/cities.shp", false);
CNLayer* layer = ds->GetLayer(0);

while (auto feature = layer->GetNextFeature()) {
    std::cout << feature->GetFID() << std::endl;
}
```

### 创建内存图层

```cpp
CNMemoryLayer* layer = new CNMemoryLayer("test", GeomType::kPoint);

// 添加字段
CNFieldDefn* field = CreateCNFieldDefn("name");
field->SetType(CNFieldType::kString);
layer->CreateField(field);

// 添加要素
CNFeature* feature = new CNFeature(layer->GetFeatureDefn());
feature->SetFID(1);
feature->SetField(0, CNFieldValue("Test"));
layer->CreateFeature(feature);
```

### 使用图层组

```cpp
CNLayerGroup* group = new CNLayerGroup("root");
group->AddChild(std::unique_ptr<CNLayer>(layer1));
group->AddChild(std::unique_ptr<CNLayer>(layer2));

group->StartTransaction();
// 修改操作
group->CommitTransaction();
```

## 依赖关系

```
layer.h
    ├── layer_type.h
    ├── feature_defn.h
    └── geometry.h

datasource.h
    └── layer.h

driver.h
    └── datasource.h

memory_layer.h
    └── layer.h

vector_layer.h
    └── layer.h

layer_group.h
    └── layer.h

layer_observer.h
    └── layer.h

feature_stream.h
    └── layer.h
```
