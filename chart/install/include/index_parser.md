# Chart Parser Module - Header Index

## 模块描述

Chart Parser模块提供海图数据解析功能，支持多种海图格式（S-57、S-100、S-101、S-102等），提供增量解析、缓存管理、错误处理、性能监控等功能。基于GDAL/OGR库实现多格式支持。

## 核心特性

- 多格式支持：S-57、S-100、S-101、S-102、GML
- 增量解析和文件变化检测
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
| [chart_format.h](parser/chart_format.h) | 海图格式定义 | `ChartFormat` |
| [chart_parser.h](parser/chart_parser.h) | 解析器门面 | `ChartParser` |
| [iparser.h](parser/iparser.h) | 解析器接口 | `IParser` |
| [parse_config.h](parser/parse_config.h) | 解析配置 | `ParseConfig` |
| [parse_result.h](parser/parse_result.h) | 解析结果 | `ParseResult`, `Feature`, `Geometry` |
| [parse_cache.h](parser/parse_cache.h) | 解析缓存 | `ParseCache` |
| [error_codes.h](parser/error_codes.h) | 错误码定义 | `ErrorCode` |
| [error_handler.h](parser/error_handler.h) | 错误处理器 | `ErrorHandler` |
| [incremental_parser.h](parser/incremental_parser.h) | 增量解析器 | `IncrementalParser`, `IncrementalParseResult` |
| [gdal_initializer.h](parser/gdal_initializer.h) | GDAL初始化 | `GdalInitializer` |
| [data_converter.h](parser/data_converter.h) | 数据转换器 | `DataConverter` |
| [ogr_data_converter.h](parser/ogr_data_converter.h) | OGR转换器 | `OgrDataConverter` |
| [s57_parser.h](parser/s57_parser.h) | S-57解析器 | `S57Parser` |
| [s57_attribute_parser.h](parser/s57_attribute_parser.h) | S-57属性解析 | `S57AttributeParser` |
| [s57_feature_type_mapper.h](parser/s57_feature_type_mapper.h) | S-57要素映射 | `S57FeatureTypeMapper` |
| [s57_geometry_converter.h](parser/s57_geometry_converter.h) | S-57几何转换 | `S57GeometryConverter` |
| [s100_parser.h](parser/s100_parser.h) | S-100解析器 | `S100Parser` |
| [s101_parser.h](parser/s101_parser.h) | S-101解析器 | `S101Parser` |
| [s101_gml_parser.h](parser/s101_gml_parser.h) | S-101 GML解析 | `S101GmlParser` |
| [s102_parser.h](parser/s102_parser.h) | S-102解析器 | `S102Parser` |
| [performance_benchmark.h](parser/performance_benchmark.h) | 性能基准 | `PerformanceBenchmark` |

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
**File**: [chart_parser.h](parser/chart_parser.h)  
**Description**: 解析器门面类（单例模式）

```cpp
namespace chart {
namespace parser {

class ChartParser {
public:
    static ChartParser& Instance();
    
    bool Initialize();
    void Shutdown();
    
    IParser* CreateParser(ChartFormat format);
    std::vector<ChartFormat> GetSupportedFormats() const;
    
    bool IsInitialized() const { return m_initialized; }

private:
    ChartParser();
    ~ChartParser();
    
    ChartParser(const ChartParser&) = delete;
    ChartParser& operator=(const ChartParser&) = delete;
    
    bool m_initialized;
};

} // namespace parser
} // namespace chart
```

### IParser
**File**: [iparser.h](parser/iparser.h)  
**Description**: 解析器接口

```cpp
namespace chart {
namespace parser {

class IParser {
public:
    virtual ~IParser() = default;
    
    virtual ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) = 0;
    
    virtual bool ParseFeature(const std::string& data, Feature& feature) = 0;
    
    virtual std::vector<ChartFormat> GetSupportedFormats() const = 0;
    
    virtual bool SupportsFormat(ChartFormat format) const;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
};

} // namespace parser
} // namespace chart
```

**Note**: IParser does NOT have `Open()` or `Close()` methods. Use `ParseChart()` directly with file path.

### S57Parser
**File**: [s57_parser.h](parser/s57_parser.h)  
**Description**: S-57格式解析器

```cpp
namespace chart {
namespace parser {

class S57Parser : public IParser {
public:
    S57Parser();
    virtual ~S57Parser();
    
    ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) override;
    
    bool ParseFeature(const std::string& data, Feature& feature) override;
    
    std::vector<ChartFormat> GetSupportedFormats() const override;
    
    std::string GetName() const override { return "S57Parser"; }
    std::string GetVersion() const override { return "1.0.0"; }

private:
    bool OpenDataset(const std::string& filePath, void** dataset);
    void CloseDataset(void* dataset);
    
    bool ParseLayer(void* dataset, const std::string& layerName, std::vector<Feature>& features, const ParseConfig& config);
    bool ParseOGRFeature(void* feature, Feature& outFeature, const ParseConfig& config);
    
    std::unique_ptr<S57GeometryConverter> m_geometryConverter;
    std::unique_ptr<S57AttributeParser> m_attributeParser;
    std::unique_ptr<S57FeatureTypeMapper> m_featureTypeMapper;
};

} // namespace parser
} // namespace chart
```

**Note**: S57Parser does NOT have `SetFeatureFilter()` or `SetSpatialFilter()` methods. Use `ParseConfig` for configuration.

### IncrementalParser
**File**: [incremental_parser.h](parser/incremental_parser.h)  
**Description**: 增量解析器（单例模式）

```cpp
namespace chart {
namespace parser {

struct FileChangeInfo {
    std::string filePath;
    uint64_t lastModifiedTime;
    uint64_t fileSize;
    std::string fileHash;
    
    bool HasChanged(const FileChangeInfo& other) const;
};

struct IncrementalParseResult {
    ParseResult addedFeatures;
    ParseResult modifiedFeatures;
    ParseResult deletedFeatures;
    ParseResult unchangedFeatures;
    
    std::vector<std::string> addedFeatureIds;
    std::vector<std::string> modifiedFeatureIds;
    std::vector<std::string> deletedFeatureIds;
    
    bool hasChanges;
    double parseTimeMs;
};

class IncrementalParser {
public:
    using ParserFunc = std::function<ParseResult(const std::string&, const ParseConfig&)>;
    
    static IncrementalParser& Instance();
    
    void SetParser(ParserFunc parser);
    
    IncrementalParseResult ParseIncremental(
        const std::string& filePath,
        const ParseConfig& config = ParseConfig()
    );
    
    bool HasFileChanged(const std::string& filePath) const;
    
    void MarkFileProcessed(const std::string& filePath);
    void ClearFileState(const std::string& filePath);
    void ClearAllStates();
    
    void SetChangeCallback(std::function<void(const std::string&, const IncrementalParseResult&)> callback);
    
    const State* GetFileState(const std::string& filePath) const;

private:
    IncrementalParser();
    ~IncrementalParser();
    
    IncrementalParser(const IncrementalParser&) = delete;
    IncrementalParser& operator=(const IncrementalParser&) = delete;
};

} // namespace parser
} // namespace chart
```

**Note**: IncrementalParser does NOT have `Pause()`, `Resume()`, `ParseNext()`, `HasMore()`, or `GetProgress()` methods. Use `ParseIncremental()` for incremental parsing with change detection.

---

## 接口

### IParser
```cpp
virtual ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) = 0;
virtual bool ParseFeature(const std::string& data, Feature& feature) = 0;
virtual std::vector<ChartFormat> GetSupportedFormats() const = 0;
virtual std::string GetName() const = 0;
virtual std::string GetVersion() const = 0;
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
    Shapefile = 7,
    S111 = 8,
    S112 = 9
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
    bool success;
    ErrorCode errorCode;
    std::string errorMessage;
    std::string filePath;
    
    std::vector<Feature> features;
    ParseStatistics statistics;
    
    std::map<std::string, std::string> metadata;
    
    bool HasError() const;
    void SetError(ErrorCode code, const std::string& message);
    void ClearError();
};
```

### ParseConfig (解析配置)
```cpp
struct ParseConfig {
    bool validateGeometry;
    bool validateAttributes;
    bool strictMode;
    bool includeMetadata;
    int32_t maxFeatureCount;
    std::string coordinateSystem;
    double tolerance;
    
    ParseConfig();
};
```

### Feature (要素)
```cpp
struct Feature {
    std::string id;
    FeatureType type;
    Geometry geometry;
    AttributeMap attributes;
    std::string className;
    int32_t rcid;
};
```

### Geometry (几何)
```cpp
struct Geometry {
    GeometryType type;
    std::vector<Point> points;
    std::vector<std::vector<Point>> rings;
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
#include "parser/chart_parser.h"

using namespace chart::parser;

auto& parser = ChartParser::Instance();
parser.Initialize();

// 创建S-57解析器
IParser* s57 = parser.CreateParser(ChartFormat::S57);
if (s57) {
    ParseResult result = s57->ParseChart("/path/to/chart.000");
    if (result.success) {
        for (const auto& feature : result.features) {
            // 处理要素
        }
    }
    delete s57;
}
```

### 使用配置解析

```cpp
#include "parser/s57_parser.h"

using namespace chart::parser;

S57Parser parser;
ParseConfig config;
config.validateGeometry = true;
config.strictMode = false;
config.coordinateSystem = "EPSG:4326";

ParseResult result = parser.ParseChart("/path/to/chart.000", config);
if (result.success) {
    std::cout << "Parsed " << result.features.size() << " features" << std::endl;
}
```

### 增量解析

```cpp
#include "parser/incremental_parser.h"

using namespace chart::parser;

auto& incParser = IncrementalParser::Instance();

// 检查文件是否变化
if (incParser.HasFileChanged("/path/to/chart.000")) {
    IncrementalParseResult result = incParser.ParseIncremental("/path/to/chart.000");
    
    if (result.hasChanges) {
        std::cout << "Added: " << result.addedFeatureIds.size() << std::endl;
        std::cout << "Modified: " << result.modifiedFeatureIds.size() << std::endl;
        std::cout << "Deleted: " << result.deletedFeatureIds.size() << std::endl;
    }
    
    // 标记文件已处理
    incParser.MarkFileProcessed("/path/to/chart.000");
}
```

### 使用缓存

```cpp
#include "parser/parse_cache.h"

ParseCache cache("/path/to/cache_dir");

std::string cacheKey = "chart_001";
if (cache.Has(cacheKey)) {
    ParseResult result = cache.Get(cacheKey);
} else {
    ParseResult result = parser->ParseChart("/path/to/chart.000");
    cache.Put(cacheKey, result);
}
```

### 错误处理

```cpp
#include "parser/error_handler.h"

ErrorHandler errorHandler;
errorHandler.SetLogLevel(LogLevel::Warning);

ParseResult result = parser->ParseChart("/path/to/chart.000");
if (result.HasError()) {
    errorHandler.HandleError(result.errorCode, result.errorMessage);
}
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-01-01 | Team | 初始版本 |
| v1.1 | 2026-02-15 | Team | 添加S-101/S-102支持 |
| v1.2 | 2026-03-10 | Team | 添加增量解析功能 |
| v1.3 | 2026-04-09 | Team | 更新接口定义，修正方法签名 |

---

**Generated**: 2026-04-09  
**Module Version**: v1.3  
**C++ Standard**: C++11
