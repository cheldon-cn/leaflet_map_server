# Feature 模块 - 头文件索引

## 概述

Feature 模块提供要素（Feature）数据模型，遵循 OGC Simple Feature Access 标准。该模块包含要素定义、字段定义、字段值、日期时间、批量处理、序列化等功能。

## 头文件列表

| 文件 | 描述 | 核心类/结构 |
|------|-------------|-------------|
| [export.h](include/ogc/feature/export.h) | 导出宏定义 | `OGC_API` |
| [feature.h](include/ogc/feature/feature.h) | 核心要素类 | `CNFeature` |
| [feature_defn.h](include/ogc/feature/feature_defn.h) | 要素定义接口 | `CNFeatureDefn` |
| [feature_collection.h](include/ogc/feature/feature_collection.h) | 要素集合 | `CNFeatureCollection` |
| [feature_iterator.h](include/ogc/feature/feature_iterator.h) | 要素迭代器 | `CNFeatureIterator` |
| [field_defn.h](include/ogc/feature/field_defn.h) | 字段定义接口 | `CNFieldDefn` |
| [field_value.h](include/ogc/feature/field_value.h) | 字段值容器 | `CNFieldValue` |
| [field_type.h](include/ogc/feature/field_type.h) | 字段类型枚举 | `CNFieldType`, `CNFieldSubType` |
| [geom_field_defn.h](include/ogc/feature/geom_field_defn.h) | 几何字段定义 | `CNGeomFieldDefn` |
| [datetime.h](include/ogc/feature/datetime.h) | 日期时间结构 | `CNDateTime` |
| [batch_processor.h](include/ogc/feature/batch_processor.h) | 批量处理 | `CNBatchProcessor`, `BatchResult` |
| [wkb_wkt_converter.h](include/ogc/feature/wkb_wkt_converter.h) | WKB/WKT序列化 | `CNWkbWktConverter` |
| [geojson_converter.h](include/ogc/feature/geojson_converter.h) | GeoJSON序列化 | `CNGeoJSONConverter` |
| [spatial_query.h](include/ogc/feature/spatial_query.h) | 空间查询 | - |
| [feature_guard.h](include/ogc/feature/feature_guard.h) | 要素生命周期管理 | - |

## 类继承关系

```
CNFieldDefn (抽象接口)
    └── 实现类

CNGeomFieldDefn (抽象接口)
    └── 实现类

CNFeatureDefn
    ├── 包含 CNFieldDefn 列表
    └── 包含 CNGeomFieldDefn 列表

CNFeature
    └── 持有 CNFeatureDefn* 和字段值

CNFeatureCollection
    └── 持有 CNFeature* 列表

CNFeatureIterator
    └── 遍历 CNFeatureCollection
```

## 核心类详细说明

### CNFeature
要素类，存储几何数据和属性数据。

```cpp
class CNFeature {
public:
    CNFeature();
    explicit CNFeature(CNFeatureDefn* definition);
    
    CNFeatureDefn* GetFeatureDefn() const;
    void SetFeatureDefn(CNFeatureDefn* definition);
    
    int64_t GetFID() const;
    void SetFID(int64_t fid);
    
    size_t GetFieldCount() const;
    CNFieldValue& GetField(size_t index);
    // ... 更多方法
};
```

### CNFeatureDefn
要素定义类，定义要素的结构（字段和几何字段）。

```cpp
class CNFeatureDefn {
public:
    static CNFeatureDefn* Create(const char* name = nullptr);
    
    size_t GetFieldCount() const;
    CNFieldDefn* GetFieldDefn(size_t index) const;
    void AddFieldDefn(CNFieldDefn* field, size_t occurrence = 1);
    
    size_t GetGeomFieldCount() const;
    CNGeomFieldDefn* GetGeomFieldDefn(size_t index) const;
    // ... 更多方法
};
```

### CNFieldValue
字段值容器，支持多种数据类型。

```cpp
class CNFieldValue {
public:
    CNFieldValue();
    explicit CNFieldValue(CNFieldType type);
    
    CNFieldValue(int32_t value);
    CNFieldValue(int64_t value);
    CNFieldValue(double value);
    CNFieldValue(bool value);
    CNFieldValue(const std::string& value);
    CNFieldValue(const CNDateTime& value);
    CNFieldValue(GeometryPtr value);
    // ... 更多构造函数
    
    CNFieldType GetType() const;
    bool IsNull() const;
    // ... 更多方法
};
```

### CNFieldType / CNFieldSubType
字段类型枚举，遵循 OGC Simple Feature Access 和 GDAL/OGR 标准。

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
    // ...
};

enum class CNFieldSubType : int32_t {
    kNone = 0,
    kBoolean = 1,
    kInt16 = 2,
    kFloat32 = 3
};
```

### CNDateTime
日期时间结构，支持日期、时间、日期时间三种类型，遵循 ISO 8601 标准。

```cpp
struct CNDateTime {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int millisecond = 0;
    
    enum class TimeZoneType : int8_t {
        kUnknown = 0,
        kLocal = 1,
        kUTC = 2,
        kFixedOffset = 3
    };
};
```

## 序列化

### CNWkbWktConverter
WKB/WKT 格式转换器。

```cpp
class CNWkbWktConverter {
public:
    std::string ToWKT(const CNFeature* feature);
    std::string ToWKT(const CNFeatureCollection* collection);
    
    std::vector<uint8_t> ToWKB(CNFeature* feature);
    std::vector<uint8_t> ToWKB(GeometryPtr geometry);
    
    CNFeature* FromWKB(const std::vector<uint8_t>& wkb);
    CNFeature* FromWKT(const std::string& wkt);
};
```

### CNGeoJSONConverter
GeoJSON 格式转换器。

```cpp
class CNGeoJSONConverter {
public:
    std::string ToGeoJSON(CNFeature* feature);
    std::string ToGeoJSON(const CNFeatureCollection* collection);
    
    CNFeature* FromGeoJSON(const std::string& json);
    CNFeatureCollection* FromGeoJSONToCollection(const std::string& json);
};
```

## 批量处理

### CNBatchProcessor
要素批量处理类，支持创建、更新、删除、读取操作。

```cpp
class CNBatchProcessor {
public:
    void SetBatchSize(size_t size);
    size_t GetBatchSize() const;
    
    void SetProgressCallback(std::function<void(size_t, size_t)> callback);
    void SetErrorCallback(std::function<void(size_t, const std::string&)> callback);
    
    BatchResult Process(CNFeatureCollection* collection, BatchOperation operation);
    BatchResult ProcessFeatures(const std::vector<CNFeature*>& features, BatchOperation operation);
};
```

## 使用示例

### 创建要素

```cpp
// 创建要素定义
CNFeatureDefn* defn = CNFeatureDefn::Create("MyFeature");

// 添加字段
CNFieldDefn* field = CreateCNFieldDefn("name");
field->SetType(CNFieldType::kString);
defn->AddFieldDefn(field);

// 创建要素
CNFeature feature(defn);
feature.SetFID(1);
feature.SetField(0, CNFieldValue("Test Feature"));
```

### 序列化

```cpp
// WKT 序列化
CNWkbWktConverter converter;
std::string wkt = converter.ToWKT(&feature);

// GeoJSON 序列化
CNGeoJSONConverter jsonConverter;
std::string json = jsonConverter.ToGeoJSON(&feature);
```

## 依赖关系

```
feature.h
    ├── feature_defn.h
    ├── field_value.h
    └── geometry.h

feature_defn.h
    ├── field_defn.h
    └── geom_field_defn.h

field_value.h
    ├── field_type.h
    ├── datetime.h
    └── geometry.h

wkb_wkt_converter.h
    ├── feature.h
    └── feature_collection.h

geojson_converter.h
    ├── feature.h
    └── feature_collection.h
```
