# Layer Module - Header Index

## Overview

Layer模块提供空间数据图层的抽象和管理功能。支持多种数据源格式，包括内存图层、矢量图层、GeoJSON、PostGIS、Shapefile等。该模块实现了统一的图层访问接口，支持空间过滤、属性查询和事务操作。

## Header File List

| File | Description | Core Classes |
|------|-------------|--------------|
| [layer.h](ogc/layer/layer.h) | 图层基类定义 | `CNLayer` |
| [datasource.h](ogc/layer/datasource.h) | 数据源基类定义 | `CNDataSource` |
| [driver.h](ogc/layer/driver.h) | 驱动基类定义 | `CNDriver` |
| [driver_manager.h](ogc/layer/driver_manager.h) | 驱动管理器 | `CNDriverManager` |
| [layer_type.h](ogc/layer/layer_type.h) | 图层类型枚举 | `CNLayerType`, `CNLayerCapability` |
| [vector_layer.h](ogc/layer/vector_layer.h) | 矢量图层基类 | `CNVectorLayer` |
| [memory_layer.h](ogc/layer/memory_layer.h) | 内存图层实现 | `CNMemoryLayer` |
| [geojson_layer.h](ogc/layer/geojson_layer.h) | GeoJSON图层 | `CNGeoJSONLayer` |
| [geopackage_layer.h](ogc/layer/geopackage_layer.h) | GeoPackage图层 | `CNGeoPackageLayer` |
| [postgis_layer.h](ogc/layer/postgis_layer.h) | PostGIS图层 | `CNPostGISLayer` |
| [shapefile_layer.h](ogc/layer/shapefile_layer.h) | Shapefile图层 | `CNShapefileLayer` |
| [wfs_layer.h](ogc/layer/wfs_layer.h) | WFS服务图层 | `CNWFSLayer` |
| [raster_layer.h](ogc/layer/raster_layer.h) | 栅格图层 | `CNRasterLayer` |
| [raster_dataset.h](ogc/layer/raster_dataset.h) | 栅格数据集 | `CNRasterDataset` |
| [layer_group.h](ogc/layer/layer_group.h) | 图层组 | `CNLayerGroup` |
| [layer_observer.h](ogc/layer/layer_observer.h) | 图层观察者 | `CNLayerObserver` |
| [layer_snapshot.h](ogc/layer/layer_snapshot.h) | 图层快照 | `CNLayerSnapshot` |
| [thread_safe_layer.h](ogc/layer/thread_safe_layer.h) | 线程安全图层 | `CNThreadSafeLayer` |
| [feature_stream.h](ogc/layer/feature_stream.h) | 要素流 | `CNFeatureStream` |
| [connection_pool.h](ogc/layer/connection_pool.h) | 连接池 | `CNConnectionPool` |
| [gdal_adapter.h](ogc/layer/gdal_adapter.h) | GDAL适配器 | `CNGDALAdapter` |
| [geometry_compat.h](ogc/layer/geometry_compat.h) | 几何兼容层 | `CNGeometry` |
| [read_write_lock.h](ogc/layer/read_write_lock.h) | 读写锁 | `CNReadWriteLock` |
| [export.h](ogc/layer/export.h) | 导出宏定义 | `OGC_LAYER_API` |

## Class Inheritance Diagram

```
CNLayer (abstract base)
    ├── CNVectorLayer
    │   ├── CNMemoryLayer
    │   ├── CNGeoJSONLayer
    │   ├── CNGeoPackageLayer
    │   ├── CNPostGISLayer
    │   ├── CNShapefileLayer
    │   └── CNWFSLayer
    ├── CNRasterLayer
    └── CNThreadSafeLayer (decorator)

CNDataSource (abstract base)
    ├── CNMemoryDataSource
    ├── CNGeoJSONDataSource
    ├── CNGeoPackageDataSource
    ├── CNPostGISDataSource
    └── CNShapefileDataSource

CNDriver (abstract base)
    ├── CNGeoJSONDriver
    ├── CNGeoPackageDriver
    ├── CNPostGISDriver
    └── CNShapefileDriver
```

## Core Classes Detail

### CNLayer

**File**: [layer.h](ogc/layer/layer.h)

**Base Classes**: None (abstract interface)

**Purpose**: 空间数据图层的抽象基类，定义了所有图层类型的通用接口。

**Key Methods**:
- `GetName()` - 获取图层名称
- `GetLayerType()` - 获取图层类型
- `GetFeatureDefn()` - 获取要素定义
- `GetGeomType()` - 获取几何类型
- `GetExtent()` - 获取图层范围
- `GetFeatureCount()` - 获取要素数量
- `ResetReading()` / `GetNextFeature()` - 遍历要素
- `GetFeature(fid)` - 按ID获取要素
- `SetFeature()` / `CreateFeature()` / `DeleteFeature()` - 编辑要素
- `SetSpatialFilter()` / `SetAttributeFilter()` - 设置过滤条件
- `StartTransaction()` / `CommitTransaction()` / `RollbackTransaction()` - 事务操作
- `TestCapability()` - 测试图层能力

### CNVectorLayer

**File**: [vector_layer.h](ogc/layer/vector_layer.h)

**Base Classes**: `CNLayer`

**Purpose**: 矢量图层基类，扩展了图层接口以支持矢量数据特有操作。

**Key Methods**:
- `GetDataSourcePath()` - 获取数据源路径
- `GetFormatName()` - 获取格式名称
- `IsReadOnly()` - 是否只读
- `SetCoordinateTransform()` - 设置坐标转换

### CNMemoryLayer

**File**: [memory_layer.h](ogc/layer/memory_layer.h)

**Base Classes**: `CNLayer`

**Purpose**: 内存中的图层实现，支持完整的CRUD操作和空间索引。

**Key Methods**:
- `Reserve()` / `ShrinkToFit()` / `Clear()` - 内存管理
- `BuildSpatialIndex()` - 构建空间索引
- `SpatialQuery()` - 空间查询
- `SetAutoFIDGeneration()` / `SetFIDReuse()` - FID管理

### CNDataSource

**File**: [datasource.h](ogc/layer/datasource.h)

**Base Classes**: None (abstract interface)

**Purpose**: 数据源抽象基类，管理一个或多个图层。

**Key Methods**:
- `GetName()` / `GetPath()` - 获取数据源信息
- `IsReadOnly()` - 是否只读
- `GetLayerCount()` / `GetLayer()` / `GetLayerByName()` - 图层访问
- `CreateLayer()` / `DeleteLayer()` - 图层管理
- `Open()` / `Create()` - 静态工厂方法

### CNDriver

**File**: [driver.h](ogc/layer/driver.h)

**Base Classes**: None (abstract interface)

**Purpose**: 驱动抽象基类，负责打开和创建特定格式的数据源。

**Key Methods**:
- `GetName()` / `GetDescription()` - 驱动信息
- `GetExtensions()` / `GetMimeTypes()` - 支持的格式
- `CanOpen()` / `CanCreate()` - 能力检测
- `Open()` / `Create()` / `Delete()` - 数据源操作

## Type Aliases

```cpp
using CNLayerPtr = std::unique_ptr<CNLayer>;
using CNLayerRef = std::shared_ptr<CNLayer>;
using CNDataSourcePtr = std::unique_ptr<CNDataSource>;
using CNDataSourceRef = std::shared_ptr<CNDataSource>;
using CNDriverPtr = std::unique_ptr<CNDriver>;
using CNDriverRef = std::shared_ptr<CNDriver>;
using CNVectorLayerPtr = std::unique_ptr<CNVectorLayer>;
using CNMemoryLayerPtr = std::unique_ptr<CNMemoryLayer>;
```

## Dependencies

```
layer
  ├── feature (CNFeature, CNFeatureDefn, CNFieldDefn)
  ├── geom (Geometry, Envelope, Coordinate)
  └── external (GDAL, PostgreSQL, SQLite)
```

## Quick Usage Examples

```cpp
#include "ogc/layer/datasource.h"
#include "ogc/layer/layer.h"
#include "ogc/layer/memory_layer.h"

// 打开数据源
auto ds = CNDataSource::Open("/path/to/data.geojson");
if (ds) {
    auto layer = ds->GetLayer(0);
    layer->ResetReading();
    while (auto feature = layer->GetNextFeature()) {
        // 处理要素
    }
}

// 创建内存图层
auto memLayer = std::make_unique<CNMemoryLayer>("test", GeomType::kPoint);
memLayer->CreateFeature(feature);
memLayer->BuildSpatialIndex();
```
