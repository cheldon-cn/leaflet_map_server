# ogc_symbology 模块用户手册

> **版本**: v1.0  
> **更新日期**: 2026-04-07  
> **适用范围**: OGC Chart 系统符号化模块

---

## 一、模块描述

ogc_symbology 是 OGC 图表系统的**符号化库**，位于系统架构的符号化层（Layer 5）。该模块提供样式解析、过滤规则和符号化渲染能力，支持 SLD、Mapbox Style、S52 海图样式等多种样式规范，实现要素到可视化符号的转换。作为渲染层的前置处理模块，负责根据样式规则将地理要素转换为可视化表达。

### 1.1 模块定位

在 OGC Chart 系统架构中，ogc_symbology 位于渲染层之前：

```
┌─────────────────────────────────────────────────────────────┐
│                    渲染层 (graph)                           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    符号化层 (ogc_symbology) ← 当前模块      │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    缓存层 (cache)                           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    数据层 (layer, feature)                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、核心特性

| 特性 | 说明 |
|------|------|
| **多样式支持** | SLD、Mapbox Style、S52 海图样式 |
| **过滤引擎** | 属性过滤、空间过滤、逻辑组合过滤 |
| **符号化器** | 点、线、面、文本、栅格、图标、组合符号化 |
| **规则引擎** | 支持多规则匹配和优先级控制 |
| **昼夜模式** | S52 海图支持昼夜显示模式切换 |
| **扩展性** | 支持自定义符号化器和过滤器 |

---

## 三、依赖关系

### 3.1 依赖库

| 依赖类型 | 库名称 | 说明 |
|----------|--------|------|
| 内部依赖 | ogc_geometry | 几何类型（Geometry, Envelope, GeomType） |
| 内部依赖 | ogc_feature | 要素（CNFeature） |
| 内部依赖 | ogc_draw | 绘图引擎（DrawContext, DrawStyle, DrawResult） |
| 外部依赖 | C++11 STL | 标准库 |

### 3.2 被依赖关系

ogc_symbology 被以下模块依赖：
- ogc_graph（地图渲染）

---

## 四、目录结构

```
symbology/
├── include/ogc/symbology/
│   ├── export.h
│   ├── style/                      # 样式解析
│   │   ├── sld_parser.h
│   │   ├── mapbox_style_parser.h
│   │   ├── s52_style_manager.h
│   │   └── s52_symbol_renderer.h
│   ├── filter/                     # 过滤规则
│   │   ├── filter.h
│   │   ├── comparison_filter.h
│   │   ├── logical_filter.h
│   │   ├── spatial_filter.h
│   │   ├── symbolizer_rule.h
│   │   └── rule_engine.h
│   └── symbolizer/                 # 符号化器
│       ├── symbolizer.h
│       ├── point_symbolizer.h
│       ├── line_symbolizer.h
│       ├── polygon_symbolizer.h
│       ├── text_symbolizer.h
│       ├── raster_symbolizer.h
│       ├── icon_symbolizer.h
│       └── composite_symbolizer.h
├── src/
│   ├── style/
│   ├── filter/
│   └── symbolizer/
├── tests/
│   ├── CMakeLists.txt
│   ├── test_filter.cpp
│   ├── test_comparison_filter.cpp
│   ├── test_spatial_filter.cpp
│   ├── test_symbolizer.cpp
│   ├── test_point_symbolizer.cpp
│   ├── test_line_symbolizer.cpp
│   ├── test_polygon_symbolizer.cpp
│   ├── test_text_symbolizer.cpp
│   ├── test_raster_symbolizer.cpp
│   ├── test_icon_symbolizer.cpp
│   ├── test_composite_symbolizer.cpp
│   ├── test_symbolizer_rule.cpp
│   └── test_s52_style_manager.cpp
├── doc/
│   └── user_manual.md
└── CMakeLists.txt
```

---

## 五、关键类和主要方法

### 5.1 Filter（过滤器基类）

**头文件**: `<ogc/symbology/filter/filter.h>`

```cpp
namespace ogc {
namespace symbology {

enum class FilterType {
    kComparison = 0,
    kLogical = 1,
    kSpatial = 2,
    kFeatureId = 3,
    kNone = 4
};

class OGC_SYMBOLOGY_API Filter {
public:
    virtual ~Filter() = default;
    
    virtual FilterType GetType() const = 0;
    
    virtual bool Evaluate(const CNFeature* feature) const = 0;
    virtual bool Evaluate(const Geometry* geometry) const = 0;
    
    virtual std::string ToString() const = 0;
    virtual FilterPtr Clone() const = 0;
    
    static FilterPtr Create();
};

typedef std::shared_ptr<Filter> FilterPtr;

} // namespace symbology
} // namespace ogc
```

### 5.2 ComparisonFilter（比较过滤器）

**头文件**: `<ogc/symbology/filter/comparison_filter.h>`

```cpp
enum class ComparisonOperator {
    kEqual,
    kNotEqual,
    kLessThan,
    kGreaterThan,
    kLessThanOrEqual,
    kGreaterThanOrEqual,
    kLike,
    kIsNull,
    kBetween
};

class OGC_SYMBOLOGY_API ComparisonFilter : public Filter {
public:
    ComparisonFilter(ComparisonOperator op, 
                     const std::string& propertyName,
                     const std::string& literal);
    
    FilterType GetType() const override { return FilterType::kComparison; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    ComparisonOperator GetOperator() const;
    const std::string& GetPropertyName() const;
    const std::string& GetLiteral() const;
    
    static std::string OperatorToString(ComparisonOperator op);
    static ComparisonOperator StringToOperator(const std::string& str);
};
```

### 5.3 SpatialFilter（空间过滤器）

**头文件**: `<ogc/symbology/filter/spatial_filter.h>`

```cpp
enum class SpatialOperator {
    kBbox,
    kIntersects,
    kWithin,
    kContains,
    kEquals,
    kOverlaps,
    kTouches,
    kCrosses,
    kDisjoint
};

class OGC_SYMBOLOGY_API SpatialFilter : public Filter {
public:
    SpatialFilter(SpatialOperator op, const Geometry* geometry);
    SpatialFilter(SpatialOperator op, const Envelope& envelope);
    
    FilterType GetType() const override { return FilterType::kSpatial; }
    
    bool Evaluate(const CNFeature* feature) const override;
    bool Evaluate(const Geometry* geometry) const override;
    
    SpatialOperator GetOperator() const;
    const Geometry* GetGeometry() const;
    const Envelope& GetEnvelope() const;
    
    bool HasGeometry() const;
    bool HasEnvelope() const;
};
```

### 5.4 Symbolizer（符号化器基类）

**头文件**: `<ogc/symbology/symbolizer/symbolizer.h>`

```cpp
enum class SymbolizerType {
    kPoint,
    kLine,
    kPolygon,
    kText,
    kRaster,
    kComposite,
    kIcon
};

class OGC_SYMBOLOGY_API Symbolizer {
public:
    virtual ~Symbolizer() = default;
    
    virtual SymbolizerType GetType() const = 0;
    virtual std::string GetName() const = 0;
    
    virtual ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, 
                                             const Geometry* geometry) = 0;
    virtual ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, 
                                             const Geometry* geometry, 
                                             const ogc::draw::DrawStyle& style) = 0;
    
    virtual bool CanSymbolize(GeomType geomType) const = 0;
    
    void SetDefaultStyle(const ogc::draw::DrawStyle& style);
    ogc::draw::DrawStyle GetDefaultStyle() const;
    
    void SetMinScale(double scale);
    void SetMaxScale(double scale);
    bool IsVisibleAtScale(double scale) const;
};

typedef std::shared_ptr<Symbolizer> SymbolizerPtr;
```

### 5.5 SymbolizerRule（符号化规则）

**头文件**: `<ogc/symbology/filter/symbolizer_rule.h>`

```cpp
class OGC_SYMBOLOGY_API SymbolizerRule {
public:
    SymbolizerRule();
    explicit SymbolizerRule(const std::string& name);
    
    const std::string& GetName() const;
    void SetName(const std::string& name);
    
    FilterPtr GetFilter() const;
    void SetFilter(FilterPtr filter);
    bool HasFilter() const;
    
    bool Evaluate(const CNFeature* feature) const;
    bool Evaluate(const Geometry* geometry) const;
    
    double GetMinScaleDenominator() const;
    double GetMaxScaleDenominator() const;
    bool IsScaleInRange(double scale) const;
    
    void AddSymbolizer(SymbolizerPtr symbolizer);
    void RemoveSymbolizer(SymbolizerPtr symbolizer);
    void ClearSymbolizers();
    
    const std::vector<SymbolizerPtr>& GetSymbolizers() const;
    size_t GetSymbolizerCount() const;
    
    bool IsElseFilter() const;
    void SetElseFilter(bool isElse);
    
    int GetPriority() const;
    void SetPriority(int priority);
    
    SymbolizerRulePtr Clone() const;
    
    static SymbolizerRulePtr Create();
    static SymbolizerRulePtr Create(const std::string& name);
};
```

### 5.6 S52StyleManager（S52海图样式管理器）

**头文件**: `<ogc/symbology/style/s52_style_manager.h>`

```cpp
enum class DayNightMode {
    kDay = 0,
    kNight = 1,
    kDusk = 2,
    kTwilight = 3
};

enum class DisplayCategory {
    kBase = 0,
    kStandard = 1,
    kOther = 2,
    kAll = 3
};

class OGC_SYMBOLOGY_API S52StyleManager {
public:
    static S52StyleManager& Instance();
    
    bool Initialize(const std::string& chartPath);
    void Shutdown();
    
    // 昼夜模式
    void SetDayNightMode(DayNightMode mode);
    DayNightMode GetDayNightMode() const;
    
    // 显示类别
    void SetDisplayCategory(DisplayCategory category);
    DisplayCategory GetDisplayCategory() const;
    
    // 符号查询
    SymbolDefinition GetSymbol(const std::string& symbolId) const;
    bool HasSymbol(const std::string& symbolId) const;
    
    // 样式获取
    ogc::draw::DrawStyle GetStyleForObjectClass(const std::string& objectClass, 
                                                  int attributeValue);
    
    // 规则生成
    std::vector<SymbolizerRulePtr> GenerateRulesForLayer(const std::string& layerName);
};
```

---

## 六、使用示例

### 6.1 比较过滤器使用

```cpp
#include <ogc/symbology/filter/comparison_filter.h>

using namespace ogc::symbology;

// 创建属性过滤器
auto filter = std::make_shared<ComparisonFilter>(
    ComparisonOperator::kEqual,
    "type",
    "building"
);

// 评估要素
if (filter->Evaluate(feature)) {
    // 要素类型为 building
    std::cout << "This is a building" << std::endl;
}

// 创建范围过滤器
auto rangeFilter = std::make_shared<ComparisonFilter>(
    ComparisonOperator::kBetween,
    "height",
    "10",
    "50"
);

if (rangeFilter->Evaluate(feature)) {
    // 高度在 10-50 之间
}
```

### 6.2 空间过滤器使用

```cpp
#include <ogc/symbology/filter/spatial_filter.h>

using namespace ogc::symbology;

// 创建边界框过滤器
Envelope bbox(120.0, 30.0, 122.0, 32.0);
auto bboxFilter = std::make_shared<SpatialFilter>(SpatialOperator::kBbox, bbox);

// 创建相交过滤器
ogc::Polygon* searchArea = CreateSearchPolygon();
auto intersectsFilter = std::make_shared<SpatialFilter>(SpatialOperator::kIntersects, searchArea);

// 评估几何对象
if (bboxFilter->Evaluate(geometry)) {
    // 几何对象在边界框内
}

if (intersectsFilter->Evaluate(feature)) {
    // 要素与搜索区域相交
}
```

### 6.3 符号化规则使用

```cpp
#include <ogc/symbology/filter/symbolizer_rule.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>

using namespace ogc::symbology;

// 创建规则
auto rule = SymbolizerRule::Create("buildings");

// 设置过滤器
auto filter = std::make_shared<ComparisonFilter>(
    ComparisonOperator::kEqual, "type", "building"
);
rule->SetFilter(filter);

// 添加点符号化器
auto pointSymbolizer = std::make_shared<PointSymbolizer>();
ogc::draw::DrawStyle style;
style.fillColor = ogc::draw::Color(255, 0, 0);  // 红色填充
pointSymbolizer->SetDefaultStyle(style);
rule->AddSymbolizer(pointSymbolizer);

// 设置比例尺范围
rule->SetMinScaleDenominator(1000);
rule->SetMaxScaleDenominator(50000);

// 评估要素
if (rule->Evaluate(feature)) {
    // 应用符号化器
    for (const auto& symbolizer : rule->GetSymbolizers()) {
        symbolizer->Symbolize(context, feature->GetGeometry());
    }
}
```

### 6.4 S52 海图样式使用

```cpp
#include <ogc/symbology/style/s52_style_manager.h>

using namespace ogc::symbology;

// 初始化
auto& manager = S52StyleManager::Instance();
manager.Initialize("E:/charts/s52");

// 设置夜间模式
manager.SetDayNightMode(DayNightMode::kNight);

// 设置显示类别
manager.SetDisplayCategory(DisplayCategory::kStandard);

// 获取符号
if (manager.HasSymbol("BOYCAR01")) {
    auto symbol = manager.GetSymbol("BOYCAR01");
    std::cout << "Symbol: " << symbol.symbolId << std::endl;
    std::cout << "Description: " << symbol.description << std::endl;
}

// 获取对象类样式
auto style = manager.GetStyleForObjectClass("BUAARE", 1);

// 生成图层规则
auto rules = manager.GenerateRulesForLayer("depth_areas");
for (const auto& rule : rules) {
    std::cout << "Rule: " << rule->GetName() << std::endl;
}
```

### 6.5 组合符号化器使用

```cpp
#include <ogc/symbology/symbolizer/composite_symbolizer.h>
#include <ogc/symbology/symbolizer/polygon_symbolizer.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>

using namespace ogc::symbology;

// 创建组合符号化器
auto composite = std::make_shared<CompositeSymbolizer>();

// 添加填充符号化器
auto fillSymbolizer = std::make_shared<PolygonSymbolizer>();
ogc::draw::DrawStyle fillStyle;
fillStyle.fillColor = ogc::draw::Color(200, 200, 255, 128);  // 半透明蓝色
fillSymbolizer->SetDefaultStyle(fillStyle);
composite->AddSymbolizer(fillSymbolizer);

// 添加边框符号化器
auto strokeSymbolizer = std::make_shared<LineSymbolizer>();
ogc::draw::DrawStyle strokeStyle;
strokeStyle.strokeColor = ogc::draw::Color(0, 0, 255);
strokeStyle.strokeWidth = 2.0;
strokeSymbolizer->SetDefaultStyle(strokeStyle);
composite->AddSymbolizer(strokeSymbolizer);

// 应用组合符号化
composite->Symbolize(context, geometry);
```

---

## 七、类型定义

### 7.1 ComparisonOperator（比较运算符）

```cpp
enum class ComparisonOperator {
    kEqual,              // 等于
    kNotEqual,           // 不等于
    kLessThan,           // 小于
    kGreaterThan,        // 大于
    kLessThanOrEqual,    // 小于等于
    kGreaterThanOrEqual, // 大于等于
    kLike,               // 模糊匹配
    kIsNull,             // 为空
    kBetween             // 区间
};
```

### 7.2 SpatialOperator（空间运算符）

```cpp
enum class SpatialOperator {
    kBbox,        // 边界框
    kIntersects,  // 相交
    kWithin,      // 在内部
    kContains,    // 包含
    kEquals,      // 相等
    kOverlaps,    // 重叠
    kTouches,     // 接触
    kCrosses,     // 穿越
    kDisjoint     // 分离
};
```

### 7.3 SymbolizerType（符号化器类型）

```cpp
enum class SymbolizerType {
    kPoint,      // 点符号化
    kLine,       // 线符号化
    kPolygon,    // 多边形符号化
    kText,       // 文本符号化
    kRaster,     // 栅格符号化
    kComposite,  // 组合符号化
    kIcon        // 图标符号化
};
```

---

## 八、编译和集成

### 8.1 CMake 配置

```cmake
# 添加 symbology 模块
add_subdirectory(symbology)

# 链接 symbology 库
target_link_libraries(your_target PRIVATE ogc_symbology)
```

### 8.2 头文件包含

```cpp
// 过滤器
#include <ogc/symbology/filter/filter.h>
#include <ogc/symbology/filter/comparison_filter.h>
#include <ogc/symbology/filter/logical_filter.h>
#include <ogc/symbology/filter/spatial_filter.h>
#include <ogc/symbology/filter/symbolizer_rule.h>

// 符号化器
#include <ogc/symbology/symbolizer/symbolizer.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>
#include <ogc/symbology/symbolizer/line_symbolizer.h>
#include <ogc/symbology/symbolizer/polygon_symbolizer.h>
#include <ogc/symbology/symbolizer/text_symbolizer.h>

// 样式解析
#include <ogc/symbology/style/sld_parser.h>
#include <ogc/symbology/style/mapbox_style_parser.h>
#include <ogc/symbology/style/s52_style_manager.h>
```

---

## 九、注意事项

1. **过滤器组合**：使用 LogicalFilter 组合多个过滤器实现复杂条件
2. **比例尺控制**：设置 SymbolizerRule 的比例尺范围，避免不必要的渲染
3. **S52 初始化**：使用 S52StyleManager 前需调用 Initialize 加载符号库
4. **线程安全**：过滤器评估是线程安全的，可在多线程环境中使用
5. **内存管理**：使用智能指针管理 Symbolizer 和 Filter 对象

---

## 十、版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | 2026-04-06 | 从 ogc_graph 拆分创建，包含样式解析、过滤规则和符号化器功能 |

---

**文档维护**: OGC Chart 开发团队  
**技术支持**: 参见项目 README.md
