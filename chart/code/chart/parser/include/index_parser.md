# Chart Parser Module - Header Index

## 模块描述

Chart Parser模块提供海图数据解析功能，支持多种海图格式（S-57、S-100、S-101、S-102等），提供增量解析、缓存管理、错误处理、性能监控等功能。基于GDAL/OGR库实现多格式支持。

## 核心特性

- 多格式支持：S-57、S-100、S-101、S-102、GML
- 增量解析和断点续传
- 解析缓存管理
- 错误处理和恢复
- 性能监控和基准测试
- GDAL/OGR适配器
- 要素类型映射
- 几何转换

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [chart_format.h](chart_parser/chart_format.h) | 海图格式定义 | `ChartFormat` |
| [chart_parser.h](chart_parser/chart_parser.h) | 解析器门面 | `ChartParser` |
| [iparser.h](chart_parser/iparser.h) | 解析器接口 | `IParser` |
| [parse_config.h](chart_parser/parse_config.h) | 解析配置 | `ParseConfig` |
| [parse_result.h](chart_parser/parse_result.h) | 解析结果 | `ParseResult` |
| [parse_cache.h](chart_parser/parse_cache.h) | 解析缓存 | `ParseCache` |
| [error_codes.h](chart_parser/error_codes.h) | 错误码定义 | `ErrorCode` |
| [error_handler.h](chart_parser/error_handler.h) | 错误处理器 | `ErrorHandler` |
| [incremental_parser.h](chart_parser/incremental_parser.h) | 增量解析器 | `IncrementalParser` |
| [gdal_initializer.h](chart_parser/gdal_initializer.h) | GDAL初始化 | `GdalInitializer` |
| [data_converter.h](chart_parser/data_converter.h) | 数据转换器 | `DataConverter` |
| [ogr_data_converter.h](chart_parser/ogr_data_converter.h) | OGR转换器 | `OgrDataConverter` |
| [s57_parser.h](chart_parser/s57_parser.h) | S-57解析器 | `S57Parser` |
| [s57_attribute_parser.h](chart_parser/s57_attribute_parser.h) | S-57属性解析 | `S57AttributeParser` |
| [s57_feature_type_mapper.h](chart_parser/s57_feature_type_mapper.h) | S-57要素映射 | `S57FeatureTypeMapper` |
| [s57_geometry_converter.h](chart_parser/s57_geometry_converter.h) | S-57几何转换 | `S57GeometryConverter` |
| [s100_parser.h](chart_parser/s100_parser.h) | S-100解析器 | `S100Parser` |
| [s101_parser.h](chart_parser/s101_parser.h) | S-101解析器 | `S101Parser` |
| [s101_gml_parser.h](chart_parser/s101_gml_parser.h) | S-101 GML解析 | `S101GmlParser` |
| [s102_parser.h](chart_parser/s102_parser.h) | S-102解析器 | `S102Parser` |
| [performance_benchmark.h](chart_parser/performance_benchmark.h) | 性能基准 | `PerformanceBenchmark` |

---

## 文件分类

| Category | Files |
|----------|-------|
| **Core** | chart_format.h, chart_parser.h, iparser.h, parse_config.h, parse_result.h |
| **S-57** | s57_parser.h, s57_attribute_parser.h, s57_feature_type_mapper.h, s57_geometry_converter.h |
| **S-100** | s100_parser.h, s101_parser.h, s101_gml_parser.h, s102_parser.h |
| **Cache** | parse_cache.h |
| **Error** | error_codes.h, error_handler.h |
| **Incremental** | incremental_parser.h |
| **GDAL** | gdal_initializer.h, data_converter.h, ogr_data_converter.h |
| **Performance** | performance_benchmark.h |

---

## 关键类

### ChartParser
**File**: [chart_parser.h](chart_parser/chart_parser.h)  
**Description**: 解析器门面类

```cpp
class ChartParser {
public:
    static ChartParser& Instance();
    
    bool Initialize();
    void Shutdown();
    
    IParser* CreateParser(ChartFormat format);
    std::vector<ChartFormat> GetSupportedFormats() const;
    
    bool IsInitialized() const;
};
```

### IParser
**File**: [iparser.h](chart_parser/iparser.h)  
**Description**: 解析器接口

```cpp
class IParser {
public:
    virtual ~IParser() = default;
    
    virtual bool Open(const std::string& path) = 0;
    virtual void Close() = 0;
    
    virtual ParseResult Parse() = 0;
    virtual ParseResult ParseIncremental(int batchSize) = 0;
    
    virtual std::vector<ChartFeature> GetFeatures() const = 0;
    virtual ChartMetadata GetMetadata() const = 0;
    
    virtual ChartFormat GetFormat() const = 0;
    virtual std::string GetFormatName() const = 0;
};
```

### S57Parser
**File**: [s57_parser.h](chart_parser/s57_parser.h)  
**Description**: S-57格式解析器

```cpp
class S57Parser : public IParser {
public:
    bool Open(const std::string& path) override;
    ParseResult Parse() override;
    
    void SetFeatureFilter(const std::vector<std::string>& features);
    void SetSpatialFilter(const Envelope& bounds);
    
    std::vector<S57Feature> GetFeatures() const;
    S57Metadata GetMetadata() const;
};
```

### IncrementalParser
**File**: [incremental_parser.h](chart_parser/incremental_parser.h)  
**Description**: 增量解析器

```cpp
class IncrementalParser {
public:
    void Start(const std::string& path, int batchSize = 1000);
    void Pause();
    void Resume();
    void Cancel();
    
    ParseResult ParseNext();
    bool HasMore() const;
    float GetProgress() const;
    
    void SetProgressCallback(std::function<void(float)> callback);
};
```

---

## 接口

### IParser
```cpp
virtual bool Open(const std::string& path) = 0;
virtual void Close() = 0;
virtual ParseResult Parse() = 0;
virtual ChartFormat GetFormat() const = 0;
```

### IDataConverter
```cpp
virtual Feature Convert(const void* ogrFeature) = 0;
virtual Geometry ConvertGeometry(const void* ogrGeometry) = 0;
```

---

## 类型定义

### ChartFormat (海图格式)
```cpp
enum class ChartFormat {
    Unknown = 0,
    S57 = 1,
    S100 = 2,
    S101 = 3,
    S101_GML = 4,
    S102 = 5,
    GeoPackage = 6,
    Shapefile = 7
};
```

### ErrorCode (错误码)
```cpp
enum class ErrorCode {
    Success = 0,
    FileNotFound = 1,
    InvalidFormat = 2,
    ParseError = 3,
    MemoryError = 4,
    IOError = 5,
    GDALInitError = 6,
    UnsupportedFormat = 7
};
```

### ParseResult (解析结果)
```cpp
struct ParseResult {
    ErrorCode error;
    std::string message;
    int featureCount;
    int errorCount;
    double parseTime;
    std::vector<Feature> features;
};
```

### ParseConfig (解析配置)
```cpp
struct ParseConfig {
    int batchSize;
    bool enableCache;
    bool validateGeometry;
    bool strictMode;
    std::vector<std::string> featureFilter;
    Envelope spatialFilter;
};
```

---

## 类继承关系图

```
IParser (interface)
    ├── S57Parser
    ├── S100Parser
    ├── S101Parser
    ├── S101GmlParser
    └── S102Parser

IDataConverter (interface)
    └── OgrDataConverter

ErrorHandler
    └── DefaultErrorHandler

ParseCache
    └── MemoryParseCache
```

---

## 依赖关系图

```
chart_format.h
    │
    ├──► iparser.h ──► parse_result.h
    │       │
    │       ├──► s57_parser.h
    │       │       ├──► s57_attribute_parser.h
    │       │       ├──► s57_feature_type_mapper.h
    │       │       └──► s57_geometry_converter.h
    │       │
    │       ├──► s100_parser.h
    │       │       ├──► s101_parser.h
    │       │       ├──► s101_gml_parser.h
    │       │       └──► s102_parser.h
    │       │
    │       └──► incremental_parser.h
    │
    ├──► chart_parser.h ──► gdal_initializer.h
    │
    ├──► parse_config.h ──► parse_cache.h
    │
    ├──► error_codes.h ──► error_handler.h
    │
    └──► data_converter.h ──► ogr_data_converter.h
```

---

## 使用示例

### 基本解析

```cpp
#include "chart_parser/chart_parser.h"

using namespace chart::parser;

auto& parser = ChartParser::Instance();
parser.Initialize();

// 创建S-57解析器
IParser* s57 = parser.CreateParser(ChartFormat::S57);
if (s57->Open("/path/to/chart.000")) {
    ParseResult result = s57->Parse();
    if (result.error == ErrorCode::Success) {
        for (const auto& feature : result.features) {
            // 处理要素
        }
    }
}
```

### 增量解析

```cpp
#include "chart_parser/incremental_parser.h"

IncrementalParser incParser;
incParser.Start("/path/to/large_chart.000", 1000);

while (incParser.HasMore()) {
    ParseResult batch = incParser.ParseNext();
    // 处理批次数据
    
    float progress = incParser.GetProgress();
    std::cout << "Progress: " << progress * 100 << "%" << std::endl;
}
```

### 使用缓存

```cpp
#include "chart_parser/parse_cache.h"

ParseCache cache("/path/to/cache_dir");

std::string cacheKey = "chart_001";
if (cache.Has(cacheKey)) {
    ParseResult result = cache.Get(cacheKey);
} else {
    ParseResult result = parser->Parse();
    cache.Put(cacheKey, result);
}
```

### 错误处理

```cpp
#include "chart_parser/error_handler.h"

ErrorHandler errorHandler;
errorHandler.SetLogLevel(LogLevel::Warning);

ParseResult result = parser->Parse();
if (result.error != ErrorCode::Success) {
    errorHandler.HandleError(result.error, result.message);
}
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加S-101/S-102支持 |
| v1.2 | 2026-03-10 | Team | 添加增量解析功能 |

---

**Generated**: 2026-04-06  
**Module Version**: v1.2  
**C++ Standard**: C++11
