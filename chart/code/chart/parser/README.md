# Chart Parser Library

海图数据解析库，支持S57、S101等海图数据格式的解析。

## 功能特性

- 支持S57格式海图数据解析（.000文件）
- 支持S101格式海图数据解析（GML/XML格式）
- 支持S100系列扩展格式（S100基础、S102水深）
- 统一的解析器接口
- 完整的几何数据转换
- 属性数据解析
- 错误处理和日志系统
- 解析缓存机制
- 增量解析支持
- 性能基准测试工具
- 数据转换器工厂

## 依赖项

- **GDAL 3.9.3**: 地理数据抽象库
- **libxml2**: XML解析库（用于S101格式）
- **Google Test**: 单元测试框架

## 目录结构

```
chart_parser/
├── CMakeLists.txt           # CMake配置文件
├── README.md                # 本文件
├── include/
│   └── chart_parser/        # 头文件目录
│       ├── chart_parser.h
│       ├── iparser.h
│       ├── parse_result.h
│       ├── chart_format.h
│       ├── parse_config.h
│       ├── error_codes.h
│       ├── error_handler.h
│       ├── gdal_initializer.h
│       ├── s57_parser.h
│       ├── s57_geometry_converter.h
│       ├── s57_attribute_parser.h
│       ├── s57_feature_type_mapper.h
│       ├── s101_parser.h
│       ├── s101_gml_parser.h
│       ├── s100_parser.h
│       ├── s102_parser.h
│       ├── data_converter.h
│       ├── ogr_data_converter.h
│       ├── parse_cache.h
│       ├── incremental_parser.h
│       └── performance_benchmark.h
├── src/                     # 源文件目录
│   ├── chart_parser.cpp
│   ├── iparser.cpp
│   ├── parse_result.cpp
│   ├── gdal_initializer.cpp
│   ├── error_handler.cpp
│   ├── s57_parser.cpp
│   ├── s57_geometry_converter.cpp
│   ├── s57_attribute_parser.cpp
│   ├── s57_feature_type_mapper.cpp
│   ├── s101_parser.cpp
│   └── s101_gml_parser.cpp
│   ├── s100_parser.cpp
│   ├── s102_parser.cpp
│   ├── data_converter.cpp
│   ├── ogr_data_converter.cpp
│   ├── parse_cache.cpp
│   ├── incremental_parser.cpp
│   └── performance_benchmark.cpp
├── tests/                   # 测试文件目录
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── test_s57_parser.cpp
│   ├── test_s57_geometry_converter.cpp
│   ├── test_s57_attribute_parser.cpp
│   ├── test_parse_result.cpp
│   └── test_s101_parser.cpp
├── doc/                     # 文档和测试数据
│   ├── chart_parser_tasks.md
│   ├── C1104001.000         # S57测试数据
│   └── 101KR004X0000.000    # S101测试数据
└── temp/                    # 编译临时文件
    ├── bin/
    └── lib/
```

## 构建方法

### Windows (MSVC)

```powershell
# 配置项目
cmake -B build -G "Visual Studio 17 2022" -A x64 ^
    -DGDAL_ROOT=E:/xspace/3rd/gdal-3.9.3 ^
    -DLIBXML2_ROOT=E:/xspace/3rd/libxml2

# 构建项目
cmake --build build --config Release

# 运行测试
cd build
ctest -C Release --output-on-failure
```

### Linux (GCC)

```bash
# 配置项目
cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DGDAL_ROOT=/path/to/gdal-3.9.3 \
    -DLIBXML2_ROOT=/path/to/libxml2

# 构建项目
cmake --build build

# 运行测试
cd build && ctest --output-on-failure
```

## 使用示例

### 基本用法

```cpp
#include <chart_parser/chart_parser.h>
#include <iostream>

using namespace chart::parser;

int main() {
    // 初始化解析器模块
    ChartParser::Instance().Initialize();
    
    // 创建S57解析器
    IParser* parser = ChartParser::Instance().CreateParser(ChartFormat::S57);
    
    if (parser) {
        // 解析海图文件
        ParseResult result = parser->ParseChart("path/to/chart.000");
        
        if (result.success) {
            std::cout << "解析成功！" << std::endl;
            std::cout << "要素数量: " << result.statistics.totalFeatureCount << std::endl;
            std::cout << "解析时间: " << result.statistics.parseTimeMs << " ms" << std::endl;
            
            // 遍历要素
            for (const auto& feature : result.features) {
                std::cout << "要素ID: " << feature.id 
                          << ", 类型: " << FeatureTypeToString(feature.type)
                          << std::endl;
            }
        } else {
            std::cerr << "解析失败: " << result.errorMessage << std::endl;
        }
        
        delete parser;
    }
    
    // 关闭解析器模块
    ChartParser::Instance().Shutdown();
    
    return 0;
}
```

### 使用解析配置

```cpp
ParseConfig config;
config.validateGeometry = true;
config.validateAttributes = true;
config.strictMode = false;
config.includeMetadata = true;
config.coordinateSystem = "EPSG:4326";

ParseResult result = parser->ParseChart("chart.000", config);
```

### 访问属性数据

```cpp
S57AttributeParser attrParser;

for (const auto& feature : result.features) {
    // 获取字符串属性
    std::string name = attrParser.GetStringValue(feature.attributes, "OBJNAM", "Unknown");
    
    // 获取整数属性
    int scamin = attrParser.GetIntValue(feature.attributes, "SCAMIN", 0);
    
    // 获取浮点数属性
    double depth = attrParser.GetDoubleValue(feature.attributes, "DEPTH", 0.0);
    
    std::cout << "名称: " << name << ", 比例尺: " << scamin << ", 深度: " << depth << std::endl;
}
```

### 使用解析缓存

```cpp
#include <chart_parser/parse_cache.h>

// 启用缓存
ParseCache& cache = ParseCache::Instance();
cache.Enable(true);
cache.SetMaxSize(100);
cache.SetMaxAge(std::chrono::seconds(3600));

// 第一次解析会缓存结果
ParseResult result1 = parser->ParseChart("chart.000");

// 第二次解析会从缓存读取
ParseResult result2;
if (cache.Get("chart.000", result2)) {
    std::cout << "从缓存加载成功" << std::endl;
}

// 查看缓存统计
auto stats = cache.GetStatistics();
std::cout << "命中率: " << stats.hitRate * 100 << "%" << std::endl;
```

### 使用增量解析

```cpp
#include <chart_parser/incremental_parser.h>
#include <chart_parser/s57_parser.h>

// 设置解析器
IncrementalParser& incParser = IncrementalParser::Instance();
incParser.SetParser([](const std::string& path, const ParseConfig& cfg) {
    S57Parser parser;
    return parser.ParseChart(path, cfg);
});

// 增量解析
IncrementalParseResult result = incParser.ParseIncremental("chart.000");

std::cout << "新增: " << result.addedFeatureIds.size() << std::endl;
std::cout << "修改: " << result.modifiedFeatureIds.size() << std::endl;
std::cout << "删除: " << result.deletedFeatureIds.size() << std::endl;
```

### 使用性能基准测试

```cpp
#include <chart_parser/performance_benchmark.h>

PerformanceBenchmark& bench = PerformanceBenchmark::Instance();

// 运行基准测试
bench.RunBenchmark("TestFunction", []() {
    // 测试代码
});

// 运行解析基准测试
bench.RunParsingBenchmark("S57Parse", "chart.000", 10);

// 生成报告
bench.GenerateReport("benchmark_report.md");
```

### 使用S102水深解析器

```cpp
#include <chart_parser/s102_parser.h>

S102Parser parser;
BathymetryGrid grid;

if (parser.ParseBathymetryGrid("bathymetry.h5", grid)) {
    std::cout << "网格大小: " << grid.rows << " x " << grid.cols << std::endl;
    std::cout << "水深点数: " << grid.points.size() << std::endl;
    
    // 计算统计信息
    auto stats = parser.CalculateStatistics(grid);
    std::cout << "最小深度: " << stats.minDepth << std::endl;
    std::cout << "最大深度: " << stats.maxDepth << std::endl;
    std::cout << "平均深度: " << stats.avgDepth << std::endl;
}
```

## API文档

### 主要类

#### ChartParser
单例类，管理解析器模块的生命周期。

```cpp
// 获取实例
ChartParser& Instance();

// 初始化/关闭
bool Initialize();
void Shutdown();

// 创建解析器
IParser* CreateParser(ChartFormat format);

// 获取支持的格式
std::vector<ChartFormat> GetSupportedFormats() const;
```

#### IParser
解析器接口，所有解析器必须实现此接口。

```cpp
// 解析海图文件
ParseResult ParseChart(const std::string& filePath, const ParseConfig& config);

// 解析单个要素
bool ParseFeature(const std::string& data, Feature& feature);

// 获取支持的格式
std::vector<ChartFormat> GetSupportedFormats() const;

// 获取解析器信息
std::string GetName() const;
std::string GetVersion() const;
```

#### ParseResult
解析结果数据结构。

```cpp
struct ParseResult {
    bool success;                          // 是否成功
    ErrorCode errorCode;                   // 错误码
    std::string errorMessage;              // 错误信息
    std::string filePath;                  // 文件路径
    std::vector<Feature> features;         // 要素列表
    ParseStatistics statistics;            // 统计信息
    std::map<std::string, std::string> metadata; // 元数据
};
```

## 测试

运行单元测试：

```bash
# 构建测试
cmake --build build --target chart_parser_tests

# 运行所有测试
cd build && ctest --output-on-failure

# 运行特定测试
./build/tests/chart_parser_tests --gtest_filter=S57ParserTest.*
```

## 许可证

MIT License

## 版本历史

- v1.0.0 (2026-04-01): 初始版本
  - 支持S57格式解析
  - 支持S101格式解析
  - 完整的几何数据转换
  - 属性数据解析
  - 错误处理和日志系统
