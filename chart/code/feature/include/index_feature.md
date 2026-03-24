# Feature Module - Header Index

## Overview

Feature模块提供空间要素的数据结构和操作接口。包括要素定义、字段定义、字段值类型、要素集合等功能。该模块是OGC Simple Feature规范的核心实现，支持属性数据和几何数据的统一管理。

## Header File List

| File | Description | Core Classes |
|------|-------------|--------------|
| [feature.h](ogc/feature/feature.h) | 要素类定义 | `CNFeature` |
| [feature_defn.h](ogc/feature/feature_defn.h) | 要素定义 | `CNFeatureDefn` |
| [field_defn.h](ogc/feature/field_defn.h) | 字段定义 | `CNFieldDefn` |
| [field_type.h](ogc/feature/field_type.h) | 字段类型枚举 | `CNFieldType`, `CNFieldSubType` |
| [field_value.h](ogc/feature/field_value.h) | 字段值类型 | `CNFieldValue` |
| [geom_field_defn.h](ogc/feature/geom_field_defn.h) | 几何字段定义 | `CNGeomFieldDefn` |
| [feature_collection.h](ogc/feature/feature_collection.h) | 要素集合 | `CNFeatureCollection` |
| [feature_iterator.h](ogc/feature/feature_iterator.h) | 要素迭代器 | `CNFeatureIterator` |
| [feature_guard.h](ogc/feature/feature_guard.h) | 要素守卫 | `CNFeatureGuard` |
| [datetime.h](ogc/feature/datetime.h) | 日期时间类型 | `CNDateTime` |
| [spatial_query.h](ogc/feature/spatial_query.h) | 空间查询工具 | `CNSpatialQuery` |
| [batch_processor.h](ogc/feature/batch_processor.h) | 批量处理器 | `CNBatchProcessor` |
| [geojson_converter.h](ogc/feature/geojson_converter.h) | GeoJSON转换器 | `CNGeoJSONConverter` |
| [wkb_wkt_converter.h](ogc/feature/wkb_wkt_converter.h) | WKB/WKT转换器 | `CNWkbConverter`, `CNWktConverter` |
| [export.h](ogc/feature/export.h) | 导出宏定义 | `OGC_FEATURE_API` |

## Class Inheritance Diagram

```
CNFeatureDefn
    └── (used by CNFeature)

CNFieldDefn (abstract interface)
    └── CNFieldDefnImpl

CNGeomFieldDefn
    └── (extends field definition for geometry)

CNFeature
    └── CNFeatureCollection (container)

CNFieldValue (variant type)
    ├── Integer
    ├── Integer64
    ├── Real
    ├── String
    ├── DateTime
    └── Binary
```

## Core Classes Detail

### CNFeature

**File**: [feature.h](ogc/feature/feature.h)

**Base Classes**: None

**Purpose**: 空间要素类，包含属性字段和几何数据，是OGC Simple Feature的核心数据结构。

**Key Methods**:
- `GetFID()` / `SetFID()` - 要素ID管理
- `GetFeatureDefn()` - 获取要素定义
- `GetFieldCount()` - 获取字段数量
- `GetField(index)` / `GetField(name)` - 获取字段值
- `SetField()` / `SetFieldNull()` - 设置字段值
- `GetFieldAsInteger()` / `GetFieldAsReal()` / `GetFieldAsString()` - 类型转换获取
- `GetGeometry()` / `SetGeometry()` / `StealGeometry()` - 几何数据管理
- `GetEnvelope()` - 获取范围
- `Clone()` - 克隆要素

### CNFeatureDefn

**File**: [feature_defn.h](ogc/feature/feature_defn.h)

**Base Classes**: None

**Purpose**: 要素定义，描述要素的字段结构和几何类型。

**Key Methods**:
- `GetName()` / `SetName()` - 名称管理
- `GetFieldCount()` / `GetFieldDefn()` / `GetFieldIndex()` - 字段访问
- `AddFieldDefn()` / `DeleteField()` - 字段管理
- `GetGeomFieldCount()` / `GetGeomFieldDefn()` - 几何字段访问
- `GetGeomType()` / `SetGeomType()` - 几何类型
- `IsSame()` - 比较定义是否相同

### CNFieldDefn

**File**: [field_defn.h](ogc/feature/field_defn.h)

**Base Classes**: None (abstract interface)

**Purpose**: 字段定义，描述单个属性字段的结构。

**Key Methods**:
- `GetName()` / `SetName()` - 字段名称
- `GetType()` / `SetType()` - 字段类型
- `GetSubType()` / `SetSubType()` - 字段子类型
- `GetWidth()` / `SetWidth()` - 字段宽度
- `GetPrecision()` / `SetPrecision()` - 小数精度
- `IsNullable()` / `SetNullable()` - 是否可空
- `IsUnique()` / `SetUnique()` - 是否唯一
- `GetDefaultValue()` / `SetDefaultValue()` - 默认值

### CNFieldValue

**File**: [field_value.h](ogc/feature/field_value.h)

**Purpose**: 字段值的变体类型，支持多种数据类型。

**Supported Types**:
- `int32_t` - 32位整数
- `int64_t` - 64位整数
- `double` - 浮点数
- `std::string` - 字符串
- `CNDateTime` - 日期时间
- `std::vector<uint8_t>` - 二进制数据

### CNFeatureCollection

**File**: [feature_collection.h](ogc/feature/feature_collection.h)

**Purpose**: 要素集合，管理多个要素的容器。

**Key Methods**:
- `AddFeature()` / `RemoveFeature()` - 添加/移除要素
- `GetFeature()` / `GetFeatureCount()` - 要素访问
- `GetExtent()` - 获取集合范围
- `FilterByGeometry()` / `FilterByAttribute()` - 过滤操作

## Type Aliases

```cpp
using CNFeaturePtr = CNFeature*;
using CNFeatureDefnPtr = std::shared_ptr<CNFeatureDefn>;
using CNFieldDefnPtr = std::shared_ptr<CNFieldDefn>;
using CNFeatureCollectionPtr = std::unique_ptr<CNFeatureCollection>;
```

## Enumerations

### CNFieldType

```cpp
enum class CNFieldType {
    kInteger,       // 32位整数
    kInteger64,     // 64位整数
    kReal,          // 浮点数
    kString,        // 字符串
    kBinary,        // 二进制
    kDate,          // 日期
    kTime,          // 时间
    kDateTime,      // 日期时间
    kBoolean        // 布尔值
};
```

### CNFieldSubType

```cpp
enum class CNFieldSubType {
    kNone,
    kBoolean,
    kInt16,
    kFloat32
};
```

## Dependencies

```
feature
  └── geom (Geometry, Envelope, Coordinate)
```

## Quick Usage Examples

```cpp
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"

// 创建要素定义
auto defn = CNFeatureDefn::Create("MyFeature");
defn->AddFieldDefn(CNFieldDefn::Create("id", CNFieldType::kInteger64));
defn->AddFieldDefn(CNFieldDefn::Create("name", CNFieldType::kString));

// 创建要素
CNFeature feature(defn);
feature.SetFID(1);
feature.SetFieldInteger64(0, 100);
feature.SetFieldString(1, "Test Feature");
feature.SetGeometry(Point::Create(116.4, 39.9));

// 读取要素
int64_t id = feature.GetFieldAsInteger64(0);
std::string name = feature.GetFieldAsString(1);
auto geom = feature.GetGeometry();
```
