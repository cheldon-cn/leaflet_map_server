# Feature Module - Header Index

## 模块描述

Feature模块提供空间要素的数据结构和操作接口。包括要素定义、字段定义、字段值类型、要素集合等功能。该模块是OGC Simple Feature规范的核心实现，支持属性数据和几何数据的统一管理。

## 核心特性

- 完整的OGC要素模型实现
- 支持多种字段类型（整型、浮点、字符串、日期、二进制、列表类型）
- 要素集合和批量处理
- GeoJSON/WKT/WKB格式转换
- 空间查询支持
- 线程安全的要素迭代器

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/feature/export.h) | DLL导出宏 | `OGC_FEATURE_API` |
| [field_type.h](ogc/feature/field_type.h) | 字段类型 | `CNFieldType`, `CNFieldSubType` |
| [field_value.h](ogc/feature/field_value.h) | 字段值 | `CNFieldValue` |
| [field_defn.h](ogc/feature/field_defn.h) | 字段定义 | `CNFieldDefn` |
| [geom_field_defn.h](ogc/feature/geom_field_defn.h) | 几何字段定义 | `CNGeomFieldDefn` |
| [datetime.h](ogc/feature/datetime.h) | 日期时间 | `CNDateTime` |
| [feature_defn.h](ogc/feature/feature_defn.h) | 要素定义 | `CNFeatureDefn` |
| [feature.h](ogc/feature/feature.h) | 要素类 | `CNFeature` |
| [feature_collection.h](ogc/feature/feature_collection.h) | 要素集合 | `CNFeatureCollection` |
| [spatial_query.h](ogc/feature/spatial_query.h) | 空间查询 | `CNSpatialQuery`, `SpatialOperation` |
| [batch_processor.h](ogc/feature/batch_processor.h) | 批量处理器 | `CNBatchProcessor`, `BatchOperation`, `BatchResult` |
| [feature_iterator.h](ogc/feature/feature_iterator.h) | 要素迭代器 | `CNFeatureIterator` |
| [feature_guard.h](ogc/feature/feature_guard.h) | 要素守卫 | `FeatureGuard` |
| [geojson_converter.h](ogc/feature/geojson_converter.h) | GeoJSON转换 | `GeoJsonConverter` |
| [wkb_wkt_converter.h](ogc/feature/wkb_wkt_converter.h) | WKB/WKT转换 | `WkbWktConverter` |

---

## 类继承关系图

```
CNFeatureDefn
    └── (引用) CNFieldDefn
    └── (引用) CNGeomFieldDefn

CNFeature
    └── (包含) CNFieldValue[]
    └── (包含) Geometry*

CNFeatureCollection
    └── (包含) CNFeature[]

CNFeatureIterator (独立类，迭代CNFeatureCollection)
CNSpatialQuery (独立类，构建空间查询条件)
CNBatchProcessor (独立类，批量处理要素)
```

---

## 依赖关系图

```
export.h
    │
    ├──► field_type.h ──► field_value.h
    │                          │
    │                          └──► datetime.h
    │
    ├──► field_defn.h ◄────────┘
    │
    ├──► geom_field_defn.h
    │
    └──► feature_defn.h ◄──────┐
             │                 │
             └──► feature.h ──► feature_collection.h
                       │
                       ├──► spatial_query.h
                       ├──► batch_processor.h
                       ├──► feature_iterator.h
                       ├──► geojson_converter.h
                       └──► wkb_wkt_converter.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | feature.h, feature_defn.h, feature_collection.h |
| **Field** | field_defn.h, field_type.h, field_value.h, geom_field_defn.h |
| **Query** | spatial_query.h, feature_iterator.h |
| **Batch** | batch_processor.h |
| **Convert** | geojson_converter.h, wkb_wkt_converter.h |
| **Utility** | datetime.h, feature_guard.h |

---

## 关键类

### CNFeature
**File**: [feature.h](ogc/feature/feature.h)  
**Description**: 核心要素类，包含属性字段和几何对象

```cpp
class CNFeature {
public:
    CNFeature();
    explicit CNFeature(CNFeatureDefn* definition);
    
    CNFeatureDefn* GetFeatureDefn() const;
    void SetFeatureDefn(CNFeatureDefn* definition);
    
    int64_t GetFID() const;
    void SetFID(int64_t fid);
    
    // 字段访问
    size_t GetFieldCount() const;
    CNFieldValue& GetField(size_t index);
    bool IsFieldSet(size_t index) const;
    bool IsFieldNull(size_t index) const;
    
    int32_t GetFieldAsInteger(size_t index) const;
    int64_t GetFieldAsInteger64(size_t index) const;
    double GetFieldAsReal(size_t index) const;
    std::string GetFieldAsString(size_t index) const;
    CNDateTime GetFieldAsDateTime(size_t index) const;
    std::vector<uint8_t> GetFieldAsBinary(size_t index) const;
    
    // 字段设置
    void SetFieldInteger(size_t index, int32_t value);
    void SetFieldInteger64(size_t index, int64_t value);
    void SetFieldReal(size_t index, double value);
    void SetFieldString(size_t index, const std::string& value);
    void SetFieldDateTime(size_t index, const CNDateTime& value);
    void SetFieldBinary(size_t index, const std::vector<uint8_t>& value);
    void SetFieldNull(size_t index);
    
    // 几何字段
    size_t GetGeomFieldCount() const;
    GeometryPtr GetGeometry(size_t index = 0) const;
    void SetGeometry(GeometryPtr geometry, size_t index = 0);
    GeometryPtr StealGeometry(size_t index = 0);
    
    std::unique_ptr<Envelope> GetEnvelope() const;
    CNFeature* Clone() const;
};
```

### CNFeatureDefn
**File**: [feature_defn.h](ogc/feature/feature_defn.h)  
**Description**: 要素定义，包含字段定义和几何字段定义

```cpp
class CNFeatureDefn {
public:
    static CNFeatureDefn* Create(const char* name = nullptr);
    
    const char* GetName() const;
    void SetName(const char* name);
    
    // 字段定义管理
    size_t GetFieldCount() const;
    CNFieldDefn* GetFieldDefn(size_t index) const;
    int GetFieldIndex(const char* name) const;
    void AddFieldDefn(CNFieldDefn* field);
    
    // 几何字段定义管理
    size_t GetGeomFieldCount() const;
    CNGeomFieldDefn* GetGeomFieldDefn(size_t index) const;
    void AddGeomFieldDefn(CNGeomFieldDefn* field);
    
    // 引用计数
    void AddReference() const;
    void ReleaseReference() const;
    
    CNFeatureDefn* Clone() const;
};
```

### CNFeatureCollection
**File**: [feature_collection.h](ogc/feature/feature_collection.h)  
**Description**: 要素集合

```cpp
class CNFeatureCollection {
public:
    CNFeatureCollection();
    
    size_t GetFeatureCount() const;
    CNFeature* GetFeature(size_t index) const;
    void AddFeature(CNFeature* feature);
    void InsertFeature(size_t index, CNFeature* feature);
    std::unique_ptr<CNFeature> RemoveFeature(size_t index);
    void Clear();
    
    // 空间操作
    std::unique_ptr<Envelope> GetEnvelope() const;
    CNFeatureCollection* FilterByField(const std::string& fieldName, const CNFieldValue& value) const;
    
    // 序列化
    std::string ToJSON() const;
    void FromJSON(const std::string& json);
};
```

### CNFeatureIterator
**File**: [feature_iterator.h](ogc/feature/feature_iterator.h)  
**Description**: 要素迭代器

```cpp
class CNFeatureIterator {
public:
    explicit CNFeatureIterator(const CNFeatureCollection* collection);
    
    void Reset();
    bool HasNext() const;
    CNFeature* Next();
    
    CNFeature* operator*() const;
    CNFeature* operator->() const;
    CNFeatureIterator& operator++();
    
    size_t GetPosition() const;
    size_t GetTotalCount() const;
};
```

### CNSpatialQuery
**File**: [spatial_query.h](ogc/feature/spatial_query.h)  
**Description**: 空间查询构建器

```cpp
class CNSpatialQuery {
public:
    CNSpatialQuery();
    
    CNSpatialQuery& SetGeometry(GeometryPtr geometry);
    CNSpatialQuery& SetOperation(SpatialOperation op);
    CNSpatialQuery& SetBufferDistance(double distance);
    CNSpatialQuery& SetSRID(int srid);
    
    GeometryPtr GetGeometry() const;
    SpatialOperation GetOperation() const;
    
    std::string ToWKT() const;
    
    static CNSpatialQuery Intersects(GeometryPtr geometry);
    static CNSpatialQuery Contains(GeometryPtr geometry);
    static CNSpatialQuery Within(GeometryPtr geometry);
    static CNSpatialQuery WithinDistance(GeometryPtr geometry, double distance);
};
```

### CNFieldValue
**File**: [field_value.h](ogc/feature/field_value.h)  
**Description**: 字段值，支持多种类型

```cpp
class CNFieldValue {
public:
    CNFieldValue();
    CNFieldValue(int32_t value);
    CNFieldValue(int64_t value);
    CNFieldValue(double value);
    CNFieldValue(const std::string& value);
    CNFieldValue(const CNDateTime& value);
    CNFieldValue(const std::vector<uint8_t>& value);
    
    CNFieldType GetType() const;
    bool IsNull() const;
    void SetNull();
    
    int32_t GetInteger() const;
    int64_t GetInteger64() const;
    double GetReal() const;
    std::string GetString() const;
    CNDateTime GetDateTime() const;
    std::vector<uint8_t> GetBinary() const;
};
```

---

## 接口

### CNFeature 迭代方法
```cpp
size_t GetFieldCount() const;
CNFieldValue& GetField(size_t index);
bool IsFieldSet(size_t index) const;
bool IsFieldNull(size_t index) const;
```

### CNFeature 字段访问方法
```cpp
int32_t GetFieldAsInteger(size_t index) const;
int64_t GetFieldAsInteger64(size_t index) const;
double GetFieldAsReal(size_t index) const;
std::string GetFieldAsString(size_t index) const;
CNDateTime GetFieldAsDateTime(size_t index) const;
std::vector<uint8_t> GetFieldAsBinary(size_t index) const;
```

### CNFeature 几何方法
```cpp
size_t GetGeomFieldCount() const;
GeometryPtr GetGeometry(size_t index = 0) const;
void SetGeometry(GeometryPtr geometry, size_t index = 0);
GeometryPtr StealGeometry(size_t index = 0);
```

### CNFeatureIterator 迭代接口
```cpp
void Reset();
bool HasNext() const;
CNFeature* Next();
size_t GetPosition() const;
size_t GetTotalCount() const;
```

---

## 类型定义

### CNFieldType (字段类型枚举)
```cpp
enum class CNFieldType : int32_t {
    kInteger = 0,
    kInteger64 = 1,
    kReal = 2,
    kString = 3,
    kDate = 4,
    kTime = 5,
    kDateTime = 6,
    kBinary = 7,
    kIntegerList = 8,
    kInteger64List = 9,
    kRealList = 10,
    kStringList = 11,
    kBoolean = 12,
    kWideString = 13,
    
    kNull = 100,
    kUnset = 101,
    kUnknown = 102
};
```

### CNFieldSubType (字段子类型枚举)
```cpp
enum class CNFieldSubType : int32_t {
    kNone = 0,
    kBoolean = 1,
    kInt16 = 2,
    kFloat32 = 3,
    kMaxSubType = 3
};
```

### SpatialOperation (空间操作枚举)
```cpp
enum class SpatialOperation {
    kIntersects,
    kContains,
    kWithin,
    kOverlaps,
    kTouches,
    kCrosses,
    kDisjoint,
    kEquals
};
```

### BatchOperation (批量操作枚举)
```cpp
enum class BatchOperation {
    kCreate,
    kUpdate,
    kDelete,
    kRead
};
```

### BatchResult (批量操作结果)
```cpp
struct BatchResult {
    size_t success_count;
    size_t failure_count;
    std::vector<size_t> failed_indices;
    std::vector<std::string> error_messages;
};
```

### CNDateTime
```cpp
class CNDateTime {
public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    double timeZone;
    
    std::string ToString() const;
    static CNDateTime Now();
    static CNDateTime Parse(const std::string& str);
};
```

---

## 使用示例

### 创建要素

```cpp
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"

using namespace ogc;

// 创建要素定义
auto defn = CNFeatureDefn::Create("PointFeature");

// 添加字段定义
auto nameField = CNFieldDefn::Create("name", CNFieldType::kString);
defn->AddFieldDefn(nameField);

auto valueField = CNFieldDefn::Create("value", CNFieldType::kReal);
defn->AddFieldDefn(valueField);

// 创建要素
CNFeature feature(defn);
feature.SetFID(1);
feature.SetFieldString(0, "Point A");
feature.SetFieldReal(1, 123.45);

// 设置几何
auto point = Point::Create(116.4, 39.9);
feature.SetGeometry(std::move(point));
```

### 要素集合操作

```cpp
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature_iterator.h"

CNFeatureCollection collection;

// 添加要素
collection.AddFeature(&feature1);
collection.AddFeature(&feature2);

// 使用迭代器遍历
CNFeatureIterator iter(&collection);
while (iter.HasNext()) {
    CNFeature* f = iter.Next();
    // 处理要素
}

// 过滤
auto filtered = collection.FilterByField("value", CNFieldValue(100.0));

// 序列化
std::string json = collection.ToJSON();
```

### 空间查询

```cpp
#include "ogc/feature/spatial_query.h"

// 创建空间查询
auto query = CNSpatialQuery::Intersects(polygonGeom);
query.SetBufferDistance(100.0);

// 转换为WKT
std::string wkt = query.ToWKT();
```

### GeoJSON转换

```cpp
#include "ogc/feature/geojson_converter.h"

GeoJsonConverter converter;

// 要素转GeoJSON
std::string json = converter.ToGeoJson(feature);

// GeoJSON转要素
CNFeature feature;
converter.FromGeoJson(json, feature);
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加批量处理器 |
| v1.2 | 2026-03-10 | Team | 添加空间查询支持 |
| v1.3 | 2026-04-06 | index-validator | 修正章节顺序、补充类型定义、修正接口名称 |

---

**Generated**: 2026-04-06  
**Module Version**: v1.3  
**C++ Standard**: C++11
