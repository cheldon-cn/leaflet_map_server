# ogc_symbology 模块 - 头文件索引

## 模块描述

ogc_symbology 是 OGC 图表系统的**符号化库**，提供样式解析、过滤规则和符号化渲染能力。支持 SLD、Mapbox Style、S52 海图样式等多种样式规范，实现要素到可视化符号的转换。作为渲染层的前置处理模块，负责根据样式规则将地理要素转换为可视化表达。

## 核心特性

- **多样式支持**：SLD、Mapbox Style、S52 海图样式
- **过滤引擎**：属性过滤、空间过滤、逻辑组合过滤
- **符号化器**：点、线、面、文本、栅格、图标、组合符号化
- **规则引擎**：支持多规则匹配和优先级控制
- **昼夜模式**：S52 海图支持昼夜显示模式切换
- **扩展性**：支持自定义符号化器和过滤器

---

## 头文件清单

### 样式解析

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/symbology/export.h) | DLL导出宏 | `OGC_SYMBOLOGY_API` |
| [style/sld_parser.h](ogc/symbology/style/sld_parser.h) | SLD样式解析器 | `SldParser` |
| [style/mapbox_style_parser.h](ogc/symbology/style/mapbox_style_parser.h) | Mapbox样式解析器 | `MapboxStyleParser` |
| [style/s52_style_manager.h](ogc/symbology/style/s52_style_manager.h) | S52海图样式管理 | `S52StyleManager` |
| [style/s52_symbol_renderer.h](ogc/symbology/style/s52_symbol_renderer.h) | S52符号渲染器 | `S52SymbolRenderer` |

### 过滤规则

| File | Description | Core Classes |
|------|-------------|--------------|
| [filter/filter.h](ogc/symbology/filter/filter.h) | 过滤器基类 | `Filter`, `FilterType` |
| [filter/comparison_filter.h](ogc/symbology/filter/comparison_filter.h) | 比较过滤器 | `ComparisonFilter`, `ComparisonOperator` |
| [filter/logical_filter.h](ogc/symbology/filter/logical_filter.h) | 逻辑过滤器 | `LogicalFilter`, `LogicalOperator` |
| [filter/spatial_filter.h](ogc/symbology/filter/spatial_filter.h) | 空间过滤器 | `SpatialFilter`, `SpatialOperator` |
| [filter/symbolizer_rule.h](ogc/symbology/filter/symbolizer_rule.h) | 符号化规则 | `SymbolizerRule` |

### 符号化器

| File | Description | Core Classes |
|------|-------------|--------------|
| [symbolizer/symbolizer.h](ogc/symbology/symbolizer/symbolizer.h) | 符号化器基类 | `Symbolizer`, `SymbolizerType` |
| [symbolizer/point_symbolizer.h](ogc/symbology/symbolizer/point_symbolizer.h) | 点符号化器 | `PointSymbolizer` |
| [symbolizer/line_symbolizer.h](ogc/symbology/symbolizer/line_symbolizer.h) | 线符号化器 | `LineSymbolizer` |
| [symbolizer/polygon_symbolizer.h](ogc/symbology/symbolizer/polygon_symbolizer.h) | 多边形符号化器 | `PolygonSymbolizer` |
| [symbolizer/text_symbolizer.h](ogc/symbology/symbolizer/text_symbolizer.h) | 文本符号化器 | `TextSymbolizer` |
| [symbolizer/raster_symbolizer.h](ogc/symbology/symbolizer/raster_symbolizer.h) | 栅格符号化器 | `RasterSymbolizer` |
| [symbolizer/icon_symbolizer.h](ogc/symbology/symbolizer/icon_symbolizer.h) | 图标符号化器 | `IconSymbolizer` |
| [symbolizer/composite_symbolizer.h](ogc/symbology/symbolizer/composite_symbolizer.h) | 组合符号化器 | `CompositeSymbolizer` |

---

## 类继承关系图

```
Filter (抽象基类)
    ├── ComparisonFilter
    ├── LogicalFilter
    └── SpatialFilter

Symbolizer (抽象基类)
    ├── PointSymbolizer
    ├── LineSymbolizer
    ├── PolygonSymbolizer
    ├── TextSymbolizer
    ├── RasterSymbolizer
    ├── IconSymbolizer
    └── CompositeSymbolizer

SymbolizerRule
    └── 规则容器

RuleEngine
    └── 规则执行引擎
```

---

## 依赖关系图

```
ogc_symbology
    │
    ├── ogc_geometry (几何类型)
    │       ├── Geometry
    │       ├── Envelope
    │       └── GeomType
    │
    ├── ogc_feature (要素)
    │       └── CNFeature
    │
    └── ogc_draw (绘图引擎)
            ├── DrawContext
            ├── DrawStyle
            └── DrawResult
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Style Parsing** | sld_parser.h, mapbox_style_parser.h, s52_style_manager.h, s52_symbol_renderer.h |
| **Filter** | filter.h, comparison_filter.h, logical_filter.h, spatial_filter.h, symbolizer_rule.h, rule_engine.h |
| **Symbolizer** | symbolizer.h, point_symbolizer.h, line_symbolizer.h, polygon_symbolizer.h, text_symbolizer.h, raster_symbolizer.h, icon_symbolizer.h, composite_symbolizer.h |
| **Build** | export.h |

---

## 关键类

### Filter (过滤器基类)

**File**: [filter.h](ogc/symbology/filter/filter.h)  
**Description**: 过滤器抽象接口，用于要素筛选

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

---

### ComparisonFilter (比较过滤器)

**File**: [comparison_filter.h](ogc/symbology/filter/comparison_filter.h)  
**Description**: 属性值比较过滤器

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

typedef std::shared_ptr<ComparisonFilter> ComparisonFilterPtr;
```

---

### SpatialFilter (空间过滤器)

**File**: [spatial_filter.h](ogc/symbology/filter/spatial_filter.h)  
**Description**: 空间关系过滤器

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
    
    static std::string OperatorToString(SpatialOperator op);
    static SpatialOperator StringToOperator(const std::string& str);
};

typedef std::shared_ptr<SpatialFilter> SpatialFilterPtr;
```

---

### Symbolizer (符号化器基类)

**File**: [symbolizer.h](ogc/symbology/symbolizer/symbolizer.h)  
**Description**: 符号化器抽象接口

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

---

### SymbolizerRule (符号化规则)

**File**: [symbolizer_rule.h](ogc/symbology/filter/symbolizer_rule.h)  
**Description**: 符号化规则，包含过滤器和符号化器

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

typedef std::shared_ptr<SymbolizerRule> SymbolizerRulePtr;
```

---

### S52StyleManager (S52海图样式管理器)

**File**: [s52_style_manager.h](ogc/symbology/style/s52_style_manager.h)  
**Description**: S52 海图样式管理，支持昼夜模式

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

typedef std::shared_ptr<S52StyleManager> S52StyleManagerPtr;
```

---

## 类型定义

### ComparisonOperator (比较运算符)

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

### SpatialOperator (空间运算符)

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

### SymbolizerType (符号化器类型)

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

## 使用示例

### 比较过滤器使用

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
}
```

### 空间过滤器使用

```cpp
#include <ogc/symbology/filter/spatial_filter.h>

using namespace ogc::symbology;

// 创建空间过滤器
Envelope bbox(120.0, 30.0, 122.0, 32.0);
auto filter = std::make_shared<SpatialFilter>(SpatialOperator::kBbox, bbox);

// 评估几何对象
if (filter->Evaluate(geometry)) {
    // 几何对象在边界框内
}
```

### 符号化规则使用

```cpp
#include <ogc/symbology/filter/symbolizer_rule.h>
#include <ogc/symbology/symbolizer/point_symbolizer.h>

using namespace ogc::symbology;

// 创建规则
auto rule = SymbolizerRule::Create("buildings");

// 设置过滤器
auto filter = std::make_shared<ComparisonFilter>(
    ComparisonOperator::kEqual, "type", "building"
);
rule->SetFilter(filter);

// 添加符号化器
auto symbolizer = std::make_shared<PointSymbolizer>();
symbolizer->SetDefaultStyle(CreateBuildingStyle());
rule->AddSymbolizer(symbolizer);

// 设置比例尺范围
rule->SetMinScaleDenominator(1000);
rule->SetMaxScaleDenominator(50000);
```

### S52 海图样式使用

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
    // 使用符号...
}
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-06 | Team | 从 ogc_graph 拆分创建 |
| v1.1 | 2026-04-09 | index-validator | 补充export.h头文件，移除不存在的rule_engine.h |

---

**Generated**: 2026-04-07  
**Module Version**: v1.0  
**C++ Standard**: C++11
