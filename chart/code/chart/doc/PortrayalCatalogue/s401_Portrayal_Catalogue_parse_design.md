# S-401 Portrayal Catalogue 解析设计文档

**版本**: v1.2
**日期**: 2026-05-06
**状态**: 设计中
**需求来源**: S-401 portrayal_catalogue.xml 解析需求
**变更记录**: v1.2 - 采用方案B（集成到 symbology），目录结构改为 include/ogc/portrayal，命名空间改为 ogc::portrayal；v1.1 - 根据技术评审意见，补充 SAX 模式、内存池、线程安全、缓存层、集成说明、CMake 配置、时序图

---

## 1. 概述

### 1.1 文档目的
本文档描述 S-401 Portrayal Catalogue XML 文件的解析系统设计，实现模块化、可复用的解析框架。

### 1.2 范围
- 解析 `portrayal_catalogue.xml` 文件
- 支持所有子元素的解析和存储
- 提供可复用的解析组件

### 1.3 XML 文件结构分析

**文件信息**:
- 文件路径: `code/chart/doc/PortrayalCatalogue/s401/portrayal_catalogue.xml`
- 文件大小: 16650 行
- 编码: ISO-8859-1 或 UTF-8
- 根元素: `portrayalCatalog`

**主要元素结构**:

```
portrayalCatalog (productId, version)
├── pixmaps (空)
├── alertCatalog
│   └── description (name, description, language)
│   └── fileName, fileType, fileFormat
├── colorProfiles
│   └── colorProfile (id)
│       └── description
│       └── fileName, fileType, fileFormat
├── symbols
│   └── symbol (id) × N
│       └── description
│       └── fileName, fileType, fileFormat
├── styleSheets
│   └── styleSheet (id)
│       └── description
│       └── fileName, fileType, fileFormat
├── lineStyles
│   └── lineStyle (id) × N
│       └── description
│       └── fileName, fileType, fileFormat
├── areaFills
│   └── areaFill (id) × N
│       └── description
│       └── fileName, fileType, fileFormat
├── fonts (空)
├── viewingGroups
│   └── viewingGroup (id) × N
│       └── description
└── rules
    └── ruleFile (id) × N
        └── description
        └── fileName, fileType, fileFormat, ruleType
```

### 1.4 三方库依赖

| 依赖库 | 版本 | 用途 | 配置路径 |
|--------|------|------|----------|
| libxml2 | - | XML 解析 | LIBXML2_ROOT |
| GDAL | 3.9.3 | 地理数据处理（可选） | GDAL_ROOT |
| GoogleTest | - | 单元测试 | GTEST_ROOT |

---

## 2. 设计目标

### 2.1 核心目标

1. **模块化**: 按功能划分模块，每个模块职责单一
2. **可复用**: 解析组件可复用于其他 XML 解析场景
3. **可扩展**: 易于添加新的元素类型支持
4. **高性能**: 支持大文件解析（16650 行）

### 2.2 设计原则

| 原则 | 说明 |
|------|------|
| 单一职责原则 | 每个类只负责一个功能 |
| 开闭原则 | 对扩展开放，对修改关闭 |
| 依赖倒置原则 | 依赖抽象而非具体实现 |
| 接口隔离原则 | 接口小而专一 |

### 2.3 约束条件

- **语言标准**: C++11
- **编码格式**: 支持 ISO-8859-1 和 UTF-8
- **内存管理**: 使用智能指针，避免内存泄漏
- **异常安全**: 提供基本异常安全保证

---

## 3. 模块归属方案比选

### 3.1 方案概述

根据项目现有模块结构，Portrayal Catalogue 解析功能有以下三种归属方案：

| 方案 | 归属模块 | 目录位置 | 说明 |
|------|----------|----------|------|
| 方案A | chart/parser | code/chart/parser | 集成到现有解析器模块 |
| 方案B | symbology | code/symbology | 集成到符号化模块，独立命名空间 ogc::portrayal |
| 方案C | chart/portrayal | code/chart/portrayal | 新建独立模块 |

### 3.2 现有模块分析

#### 3.2.1 chart/parser 模块

**现有功能**:
- S57Parser: 解析 S-57 格式海图数据
- S100Parser: 解析 S-100 格式海图数据
- S102Parser: 解析 S-102 格式海图数据

**命名空间**: `chart::parser`

**特点**:
- 专注于海图数据解析
- 输出为 Feature 集合
- 与地理数据处理紧密相关

#### 3.2.2 symbology 模块

**现有功能**:
- S52StyleManager: S-52 样式管理
- Symbolizer: 符号化接口
- 各种 Symbolizer 实现（Point, Line, Polygon, Text 等）

**命名空间**: `ogc::symbology`

**特点**:
- 专注于符号化渲染
- 使用 PortrayalCatalog 进行样式查找
- 与绘图系统紧密相关

**关键类定义**:
```cpp
// code/symbology/include/ogc/symbology/style/s52_style_manager.h
struct SymbolDefinition {
    std::string symbolId;
    SymbolType type;
    std::string description;
    std::vector<VectorCommand> vectorCommands;
    std::string rasterImagePath;
    // ...
};

// code/symbology/include/ogc/symbology/symbolizer/symbolizer.h
class Symbolizer {
public:
    virtual SymbolizerType GetType() const = 0;
    virtual ogc::draw::DrawResult Symbolize(
        ogc::draw::DrawContextPtr context, 
        const Geometry* geometry) = 0;
    // ...
};
```

### 3.3 方案详细比选

#### 3.3.1 方案A: 集成到 chart/parser

**目录结构**:
```
code/chart/parser/
├── include/parser/
│   ├── chart_parser.h
│   ├── s57_parser.h
│   └── portrayal/
│       ├── portrayal_catalog_parser.h
│       ├── element_parser.h
│       └── model/
│           ├── symbol.h
│           └── ...
└── src/
    └── portrayal/
        └── ...
```

**优点**:
- 解析功能集中管理
- 复用现有 XML 解析基础设施
- 便于统一版本管理

**缺点**:
- parser 模块职责过重，已有 S57/S100/S101/S102 四种解析器
- PortrayalCatalog 不是海图数据，而是渲染配置元数据，语义不符
- 与 symbology 模块耦合度增加（parser 需引用 portrayal 数据传递给 symbology）
- 不符合单一职责原则

**适用场景**: 解析功能高度相关，需要统一管理

#### 3.3.2 方案B: 集成到 symbology

**目录结构**:
```
code/symbology/
├── include/ogc/
│   ├── symbology/
│   │   ├── style/
│   │   │   └── s52_style_manager.h
│   │   └── symbolizer/
│   │       └── symbolizer.h
│   └── portrayal/
│       ├── model/
│       │   ├── portrayal_catalog.h
│       │   ├── symbol.h
│       │   └── ...
│       ├── parser/
│       │   ├── portrayal_catalog_parser.h
│       │   └── ...
│       └── utils/
│           └── ...
├── src/
│   ├── style/
│   ├── symbolizer/
│   └── portrayal/
│       ├── model/
│       ├── parser/
│       └── utils/
└── tests/
    └── portrayal/
```

**优点**:
- PortrayalCatalog 与符号化紧密相关，功能内聚性强
- 现有 `SymbolLibrary::LoadFromFile` 和 `StyleRuleEngine::LoadRulesFromFile` 正是 PortrayalCatalog 解析的自然演进
- 减少跨模块依赖，symbology 无需额外依赖外部 portrayal 模块
- 修改 PortrayalCatalog 时无需跨模块协调
- 独立命名空间 `ogc::portrayal`，与 `ogc::symbology` 逻辑解耦，便于未来独立拆分

**缺点**:
- symbology 模块职责扩展，包含解析与渲染两种关注点
- 物理位置在 symbology 内，但命名空间独立，需注意模块边界维护
- 若其他模块也需要独立使用 PortrayalCatalog，则需反向依赖 symbology

**适用场景**: PortrayalCatalog 主要服务于符号化渲染，且项目规模中等

#### 3.3.3 方案C: 新建独立 portrayal 模块

**目录结构**:
```
code/ogc/portrayal/
├── include/ogc/portrayal/
│   ├── model/
│   │   ├── description.h
│   │   ├── symbol.h
│   │   ├── line_style.h
│   │   ├── area_fill.h
│   │   ├── rule_file.h
│   │   ├── viewing_group.h
│   │   └── portrayal_catalog.h
│   ├── parser/
│   │   ├── xml_reader.h
│   │   ├── element_parser.h
│   │   ├── parser_registry.h
│   │   └── portrayal_catalog_parser.h
│   └── utils/
│       ├── string_utils.h
│       └── file_utils.h
└── src/
    ├── model/
    ├── parser/
    └── utils/
```

**优点**:
- 职责清晰，符合单一职责原则
- 便于独立测试和维护
- 可被多个模块复用（parser、symbology、alert）
- 命名空间一致（ogc::portrayal）
- 便于未来扩展（如支持其他版本的 Portrayal Catalog）

**缺点**:
- 增加模块数量和 CMake 配置复杂度
- 跨模块依赖管理：symbology 需依赖独立 portrayal 模块，增加依赖图节点
- 修改 PortrayalCatalog 时需同时协调消费方模块
- 当前仅 symbology 为主要消费方，独立模块的复用优势尚未体现

**适用场景**: 需要独立维护和复用的通用模块，或多模块消费场景

### 3.4 方案对比矩阵

| 评估维度 | 方案A (parser) | 方案B (symbology) | 方案C (独立模块) |
|----------|----------------|-------------------|------------------|
| **职责清晰度** | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **功能内聚性** | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **复用性** | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **可维护性** | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| **扩展性** | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **实现复杂度** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| **依赖管理** | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |

> **评估说明**:
> - **功能内聚性**: 方案B 得分最高，因为 PortrayalCatalog 的核心消费者是 symbology 模块，现有 `SymbolLibrary` 和 `StyleRuleEngine` 正是其自然演进方向，功能内聚性最强。方案C 虽然结构内聚（同目录），但功能上 portrayal 与 symbology 分离，内聚性略低。
> - **可维护性**: 方案B 在当前项目规模下更易维护——修改 PortrayalCatalog 只需改一个模块；方案C 需跨模块协调，但长期来看独立模块的边界更清晰。
> - **复用性**: 方案C 在多模块消费场景下优势明显；但当前仅 symbology 为主要消费方，方案B 通过 `ogc::portrayal` 独立命名空间也能提供有限的复用能力。
> - **依赖管理**: 方案B 最优，symbology 内部自包含，无额外跨模块依赖；方案C 引入新依赖节点，symbology 需依赖独立 portrayal 模块。

### 3.5 推荐方案

**推荐方案B: 集成到 symbology 模块，使用独立命名空间 ogc::portrayal**

**理由**:
1. **功能内聚**: PortrayalCatalog 的核心消费方是 symbology 模块，现有 `SymbolLibrary` 和 `StyleRuleEngine` 正是其自然演进方向
2. **依赖简化**: symbology 内部自包含，无需引入额外跨模块依赖
3. **命名空间独立**: 使用 `ogc::portrayal` 命名空间，与 `ogc::symbology` 逻辑解耦，保持清晰的模块边界
4. **便于维护**: 修改 PortrayalCatalog 只需改一个模块，无需跨模块协调
5. **可拆分**: 若未来需要独立模块，只需将 `ogc/portrayal` 目录迁出，命名空间无需变更

**方案C 的合理性说明**:

方案C 在以下条件下是最优选择：
- 多模块消费 PortrayalCatalog（如 alert、draw 同时独立消费）
- 项目规模大，需要严格的模块隔离
- 团队规模大，跨模块协调成本高

但当前阶段：
- 仅 symbology 为主要消费方
- alert 模块可通过 symbology 间接获取 alertCatalog 数据
- 独立模块增加 CMake 配置和依赖管理复杂度

因此方案B 在当前阶段更具优势，方案C 可作为未来演进方向。

**模块依赖关系**:
```
┌─────────────────────────────────────────────────────────────┐
│                      应用层                                  │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ S57Parser   │  │ S100Parser  │  │ SymbologyRenderer   │  │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘  │
└─────────┼────────────────┼───────────────────┼─────────────┘
          │                │                   │
          └────────────────┼───────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│              code/symbology (含 ogc::portrayal)             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  ogc::portrayal                                       │  │
│  │  PortrayalCatalogParser                               │  │
│  │  - 解析 S-401 表现目录                                 │  │
│  │  - 提供 PortrayalCatalog 数据模型                      │  │
│  └───────────────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  ogc::symbology                                       │  │
│  │  S52StyleManager / Symbolizer                         │  │
│  │  - 消费 PortrayalCatalog 进行符号化渲染                │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. 系统架构

### 4.1 整体架构

采用**分层架构**模式：

```
┌─────────────────────────────────────────────────────────────┐
│                    应用层 (Application)                      │
│              PortrayalCatalogParser (门面)                   │
├─────────────────────────────────────────────────────────────┤
│                    业务层 (Business)                         │
│    ElementParser, CatalogBuilder, ValidationService         │
├─────────────────────────────────────────────────────────────┤
│                    数据层 (Data)                             │
│         XmlReader, Model Classes, Repository                │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 架构决策记录

#### ADR-001: 选择分层架构

**背景**:
系统功能明确，主要是 XML 解析和数据存储，预计用户为内部模块调用。

**决策**:
采用三层架构：应用层、业务层、数据层。

**理由**:
1. 功能需求清晰，分层架构足够
2. 便于测试和维护
3. 与现有 chart/parser 模块架构一致

**影响**:
- 模块间通过接口通信
- 数据层使用 Repository 模式
- 业务层使用策略模式处理不同元素类型

#### ADR-002: 使用 libxml2 作为 XML 解析库

**背景**:
需要解析大型 XML 文件（16650 行），支持 ISO-8859-1 和 UTF-8 编码。

**决策**:
使用 libxml2 库进行 XML 解析。

**理由**:
1. 项目已依赖 libxml2
2. 支持多种编码格式
3. 成熟稳定，性能优秀
4. 支持 SAX 和 DOM 两种解析模式

**影响**:
- 需要封装 libxml2 接口
- 提供统一的 XML 读取抽象

### 4.3 技术选型

| 层次 | 技术选型 | 版本 | 说明 |
|------|----------|------|------|
| XML解析 | libxml2 | - | 项目已有依赖 |
| 智能指针 | std::shared_ptr | C++11 | 内存管理 |
| 字符串 | std::string | C++11 | 文本处理 |
| 容器 | std::vector, std::map | C++11 | 数据存储 |
| 单元测试 | GoogleTest | - | 项目已有依赖 |

---

## 5. 核心模块设计

### 5.1 命名空间设计

```cpp
namespace ogc {
namespace portrayal {

// 核心模型类
namespace model { }

// 解析器
namespace parser { }

// 工具类
namespace utils { }

} // namespace portrayal
} // namespace ogc
```

### 5.2 数据模型模块 (model)

**命名空间**: `ogc::portrayal::model`

#### 4.2.1 基础描述类 (Description)

```cpp
namespace ogc {
namespace portrayal {
namespace model {

class Description {
public:
    Description() = default;
    Description(const std::string& name, 
                const std::string& desc,
                const std::string& lang = "eng");
    
    const std::string& GetName() const;
    const std::string& GetDescription() const;
    const std::string& GetLanguage() const;
    
    void SetName(const std::string& name);
    void SetDescription(const std::string& desc);
    void SetLanguage(const std::string& lang);

private:
    std::string name_;
    std::string description_;
    std::string language_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc
```

#### 4.2.2 文件引用基类 (FileReference)

```cpp
namespace ogc {
namespace portrayal {
namespace model {

class FileReference {
public:
    FileReference() = default;
    virtual ~FileReference() = default;
    
    const std::string& GetId() const;
    const Description& GetDescription() const;
    const std::string& GetFileName() const;
    const std::string& GetFileType() const;
    const std::string& GetFileFormat() const;
    
    void SetId(const std::string& id);
    void SetDescription(const Description& desc);
    void SetFileName(const std::string& fileName);
    void SetFileType(const std::string& fileType);
    void SetFileFormat(const std::string& fileFormat);

protected:
    std::string id_;
    Description description_;
    std::string fileName_;
    std::string fileType_;
    std::string fileFormat_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc
```

#### 4.2.3 具体模型类

| 类名 | 继承 | 说明 | 特有属性 |
|------|------|------|----------|
| Symbol | FileReference | 符号定义 | 无 |
| LineStyle | FileReference | 线型样式 | 无 |
| AreaFill | FileReference | 区域填充 | 无 |
| StyleSheet | FileReference | 样式表 | 无 |
| RuleFile | FileReference | 规则文件 | ruleType_ |
| AlertCatalog | FileReference | 警报目录 | 无 |
| ColorProfile | FileReference | 颜色配置 | 无 |
| ViewingGroup | - | 查看组 | id_, description_ |

```cpp
namespace ogc {
namespace portrayal {
namespace model {

class Symbol : public FileReference {
public:
    Symbol() = default;
    using FileReference::FileReference;
};

class LineStyle : public FileReference {
public:
    LineStyle() = default;
    using FileReference::FileReference;
};

class AreaFill : public FileReference {
public:
    AreaFill() = default;
    using FileReference::FileReference;
};

class StyleSheet : public FileReference {
public:
    StyleSheet() = default;
    using FileReference::FileReference;
};

class RuleFile : public FileReference {
public:
    RuleFile() = default;
    
    const std::string& GetRuleType() const;
    void SetRuleType(const std::string& ruleType);

private:
    std::string ruleType_;
};

class ViewingGroup {
public:
    ViewingGroup() = default;
    
    const std::string& GetId() const;
    const Description& GetDescription() const;
    
    void SetId(const std::string& id);
    void SetDescription(const Description& desc);

private:
    std::string id_;
    Description description_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc
```

#### 4.2.4 PortrayalCatalog 聚合类

```cpp
namespace ogc {
namespace portrayal {
namespace model {

class PortrayalCatalog {
public:
    PortrayalCatalog() = default;
    
    // 基本信息
    const std::string& GetProductId() const;
    const std::string& GetVersion() const;
    void SetProductId(const std::string& productId);
    void SetVersion(const std::string& version);
    
    // 元素访问
    const AlertCatalog& GetAlertCatalog() const;
    const std::vector<ColorProfile>& GetColorProfiles() const;
    const std::vector<Symbol>& GetSymbols() const;
    const std::vector<StyleSheet>& GetStyleSheets() const;
    const std::vector<LineStyle>& GetLineStyles() const;
    const std::vector<AreaFill>& GetAreaFills() const;
    const std::vector<ViewingGroup>& GetViewingGroups() const;
    const std::vector<RuleFile>& GetRuleFiles() const;
    
    // 元素添加
    void SetAlertCatalog(const AlertCatalog& catalog);
    void AddColorProfile(const ColorProfile& profile);
    void AddSymbol(const Symbol& symbol);
    void AddStyleSheet(const StyleSheet& sheet);
    void AddLineStyle(const LineStyle& style);
    void AddAreaFill(const AreaFill& fill);
    void AddViewingGroup(const ViewingGroup& group);
    void AddRuleFile(const RuleFile& rule);
    
    // 查询接口
    const Symbol* FindSymbol(const std::string& id) const;
    const LineStyle* FindLineStyle(const std::string& id) const;
    const AreaFill* FindAreaFill(const std::string& id) const;
    const RuleFile* FindRuleFile(const std::string& id) const;
    const ViewingGroup* FindViewingGroup(const std::string& id) const;
    
    // 内存管理
    void SetStringPool(std::shared_ptr<utils::StringPool> pool);

private:
    std::string productId_;
    std::string version_;
    
    AlertCatalog alertCatalog_;
    std::vector<ColorProfile> colorProfiles_;
    std::vector<Symbol> symbols_;
    std::vector<StyleSheet> styleSheets_;
    std::vector<LineStyle> lineStyles_;
    std::vector<AreaFill> areaFills_;
    std::vector<ViewingGroup> viewingGroups_;
    std::vector<RuleFile> ruleFiles_;
    
    // 索引映射（用于快速查询，Add 方法需同步维护）
    std::map<std::string, size_t> symbolIndex_;
    std::map<std::string, size_t> lineStyleIndex_;
    std::map<std::string, size_t> areaFillIndex_;
    std::map<std::string, size_t> ruleFileIndex_;
    std::map<std::string, size_t> viewingGroupIndex_;
    
    // 字符串池（可选，用于内存优化）
    std::shared_ptr<utils::StringPool> stringPool_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc
```

### 5.3 解析器模块 (parser)

**命名空间**: `ogc::portrayal::parser`

#### 4.3.1 XML 读取器接口 (XmlReader)

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class XmlReader {
public:
    virtual ~XmlReader() = default;
    
    virtual bool Open(const std::string& filePath) = 0;
    virtual void Close() = 0;
    virtual bool IsOpen() const = 0;
    virtual std::string GetEncoding() const = 0;
    
    virtual bool ReadNextElement() = 0;
    virtual std::string GetCurrentElementName() const = 0;
    virtual std::string GetCurrentElementText() const = 0;
    virtual std::string GetAttribute(const std::string& name) const = 0;
    virtual bool HasAttribute(const std::string& name) const = 0;
    
    virtual bool MoveToFirstChild() = 0;
    virtual bool MoveToNextSibling() = 0;
    virtual bool MoveToParent() = 0;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

#### 4.3.2 libxml2 实现 (LibXml2Reader)

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class LibXml2Reader : public XmlReader {
public:
    LibXml2Reader();
    ~LibXml2Reader() override;
    
    bool Open(const std::string& filePath) override;
    void Close() override;
    bool IsOpen() const override;
    std::string GetEncoding() const override;
    
    bool ReadNextElement() override;
    std::string GetCurrentElementName() const override;
    std::string GetCurrentElementText() const override;
    std::string GetAttribute(const std::string& name) const override;
    bool HasAttribute(const std::string& name) const override;
    
    bool MoveToFirstChild() override;
    bool MoveToNextSibling() override;
    bool MoveToParent() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

#### 4.3.3 元素解析器接口 (ElementParser)

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class ElementParser {
public:
    virtual ~ElementParser() = default;
    
    virtual std::string GetElementName() const = 0;
    
    // 判断是否能解析指定元素，默认实现比较 GetElementName()，
    // 子类可重写以支持通配符/正则匹配等高级场景
    virtual bool CanParse(const std::string& elementName) const = 0;
    
    virtual bool Parse(XmlReader& reader, 
                       model::PortrayalCatalog& catalog) = 0;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

#### 4.3.4 具体元素解析器

**符号解析器 (SymbolParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class SymbolParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader, 
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**线型解析器 (LineStyleParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class LineStyleParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader, 
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**区域填充解析器 (AreaFillParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class AreaFillParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader, 
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**规则文件解析器 (RuleFileParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class RuleFileParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader, 
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**查看组解析器 (ViewingGroupParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class ViewingGroupParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader, 
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**告警目录解析器 (AlertCatalogParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class AlertCatalogParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader,
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**颜色配置文件解析器 (ColorProfileParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class ColorProfileParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader,
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

**样式表解析器 (StyleSheetParser)**:

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class StyleSheetParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader,
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

#### 4.3.5 解析器注册表 (ParserRegistry)

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class ParserRegistry {
public:
    ParserRegistry();
    
    void RegisterParser(std::shared_ptr<ElementParser> parser);
    void UnregisterParser(const std::string& elementName);
    
    ElementParser* GetParser(const std::string& elementName) const;
    bool HasParser(const std::string& elementName) const;
    
    std::vector<std::string> GetRegisteredElements() const;

private:
    std::map<std::string, std::shared_ptr<ElementParser>> parsers_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

#### 4.3.6 门面解析器 (PortrayalCatalogParser)

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class PortrayalCatalogParser {
public:
    PortrayalCatalogParser();
    explicit PortrayalCatalogParser(std::shared_ptr<XmlReader> reader);
    
    void SetXmlReader(std::shared_ptr<XmlReader> reader);
    void RegisterParser(std::shared_ptr<ElementParser> parser);
    
    std::shared_ptr<model::PortrayalCatalog> Parse(const std::string& filePath);
    
    // 解析选项
    void SetValidateEnabled(bool enabled);
    void SetIgnoreComments(bool ignore);

private:
    bool ParseRootElement();
    bool ParseChildElements();
    bool ParseAttributes();
    
    std::shared_ptr<XmlReader> reader_;
    ParserRegistry registry_;
    std::shared_ptr<model::PortrayalCatalog> catalog_;
    
    bool validateEnabled_ = true;
    bool ignoreComments_ = true;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

### 5.4 工具模块 (utils)

**命名空间**: `ogc::portrayal::utils`

#### 4.4.1 字符串工具 (StringUtils)

```cpp
namespace ogc {
namespace portrayal {
namespace utils {

class StringUtils {
public:
    static std::string Trim(const std::string& str);
    static std::string ToLower(const std::string& str);
    static std::string ToUpper(const std::string& str);
    static std::vector<std::string> Split(const std::string& str, 
                                          char delimiter);
    static std::string Join(const std::vector<std::string>& parts, 
                           const std::string& delimiter);
    static bool StartsWith(const std::string& str, 
                          const std::string& prefix);
    static bool EndsWith(const std::string& str, 
                        const std::string& suffix);
    static std::string ConvertEncoding(const std::string& input,
                                       const std::string& fromEncoding,
                                       const std::string& toEncoding);
};

} // namespace utils
} // namespace portrayal
} // namespace ogc
```

#### 4.4.2 文件工具 (FileUtils)

```cpp
namespace ogc {
namespace portrayal {
namespace utils {

class FileUtils {
public:
    static bool Exists(const std::string& path);
    static bool IsFile(const std::string& path);
    static bool IsDirectory(const std::string& path);
    static std::string GetDirectory(const std::string& filePath);
    static std::string GetFileName(const std::string& filePath);
    static std::string GetFileExtension(const std::string& filePath);
    static std::string CombinePath(const std::string& dir, 
                                   const std::string& file);
};

} // namespace utils
} // namespace portrayal
} // namespace ogc
```

---

## 6. 数据模型设计

### 6.1 类关系图

```
┌─────────────────────────────────────────────────────────────┐
│                      Model Classes                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────┐         ┌──────────────┐                  │
│  │ Description  │         │ FileReference│ (abstract)       │
│  ├──────────────┤         ├──────────────┤                  │
│  │ - name_      │◄────────│ - id_        │                  │
│  │ - description_│         │ - description_│                 │
│  │ - language_  │         │ - fileName_  │                  │
│  └──────────────┘         │ - fileType_  │                  │
│                           │ - fileFormat_ │                  │
│                           └──────┬───────┘                  │
│                                  │                           │
│           ┌──────────────────────┼──────────────────────┐   │
│           │                      │                      │   │
│           ▼                      ▼                      ▼   │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│  │    Symbol    │      │  LineStyle   │      │  RuleFile    │
│  └──────────────┘      └──────────────┘      │ - ruleType_  │
│                                               └──────────────┘
│                                                              │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│  │  AreaFill    │      │ StyleSheet   │      │ ViewingGroup │
│  └──────────────┘      └──────────────┘      │ - id_        │
│                                               │ - description_│
│                                               └──────────────┘
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │              PortrayalCatalog (聚合根)                  │ │
│  ├────────────────────────────────────────────────────────┤ │
│  │ - productId_                                           │ │
│  │ - version_                                             │ │
│  │ - alertCatalog_                                        │ │
│  │ - colorProfiles_: vector<ColorProfile>                 │ │
│  │ - symbols_: vector<Symbol>                             │ │
│  │ - styleSheets_: vector<StyleSheet>                     │ │
│  │ - lineStyles_: vector<LineStyle>                       │ │
│  │ - areaFills_: vector<AreaFill>                         │ │
│  │ - viewingGroups_: vector<ViewingGroup>                 │ │
│  │ - ruleFiles_: vector<RuleFile>                         │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 解析器类关系图

```
┌─────────────────────────────────────────────────────────────┐
│                      Parser Classes                          │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────┐                                           │
│  │  XmlReader   │ (interface)                               │
│  ├──────────────┤                                           │
│  │ + Open()     │                                           │
│  │ + Close()    │                                           │
│  │ + ReadNext() │                                           │
│  └──────┬───────┘                                           │
│         │                                                    │
│         ▼                                                    │
│  ┌──────────────┐                                           │
│  │LibXml2Reader │                                           │
│  └──────────────┘                                           │
│                                                              │
│  ┌──────────────┐                                           │
│  │ElementParser │ (interface)                               │
│  ├──────────────┤                                           │
│  │ + Parse()    │                                           │
│  └──────┬───────┘                                           │
│         │                                                    │
│    ┌────┴────┬────────────┬────────────┐                    │
│    ▼         ▼            ▼            ▼                    │
│ ┌──────┐ ┌──────┐   ┌──────┐   ┌──────┐                    │
│ │Symbol│ │LineSt│   │AreaFi│   │RuleFi│ ...                │
│ │Parser│ │Parser│   │Parser│   │Parser│                    │
│ └──────┘ └──────┘   └──────┘   └──────┘                    │
│                                                              │
│  ┌──────────────┐      ┌──────────────┐                     │
│  │ParserRegistry│◄─────│PortrayalCatalog│                   │
│  └──────────────┘      │Parser (门面)  │                     │
│                        └──────────────┘                     │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 7. 接口设计

### 7.1 公共接口列表

| 模块 | 接口 | 方法 | 说明 |
|------|------|------|------|
| parser | PortrayalCatalogParser | Parse(filePath) | 解析 XML 文件 |
| model | PortrayalCatalog | GetSymbols() | 获取符号列表 |
| model | PortrayalCatalog | FindSymbol(id) | 查找符号 |
| model | PortrayalCatalog | GetLineStyles() | 获取线型列表 |
| model | PortrayalCatalog | FindLineStyle(id) | 查找线型 |
| model | PortrayalCatalog | GetAreaFills() | 获取区域填充列表 |
| model | PortrayalCatalog | FindAreaFill(id) | 查找区域填充 |
| model | PortrayalCatalog | GetRuleFiles() | 获取规则文件列表 |
| model | PortrayalCatalog | FindRuleFile(id) | 查找规则文件 |
| model | PortrayalCatalog | GetViewingGroups() | 获取查看组列表 |

### 7.2 核心接口详细设计

#### 7.2.1 PortrayalCatalogParser::Parse

```cpp
std::shared_ptr<model::PortrayalCatalog> Parse(const std::string& filePath);
```

**参数**:
| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| filePath | string | 是 | XML 文件路径 |

**返回值**:
| 类型 | 说明 |
|------|------|
| shared_ptr<PortrayalCatalog> | 解析结果，失败返回 nullptr |

**异常**:
| 异常类型 | 说明 |
|----------|------|
| std::invalid_argument | 文件路径为空 |
| std::runtime_error | 文件打开失败 |
| std::runtime_error | XML 格式错误 |

**使用示例**:
```cpp
using namespace ogc::portrayal;

parser::PortrayalCatalogParser parser;
auto catalog = parser.Parse("portrayal_catalogue.xml");

if (catalog) {
    auto symbols = catalog->GetSymbols();
    for (const auto& symbol : symbols) {
        std::cout << symbol.GetId() << std::endl;
    }
}
```

#### 7.2.2 PortrayalCatalog::FindSymbol

```cpp
const Symbol* FindSymbol(const std::string& id) const;
```

**参数**:
| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| id | string | 是 | 符号 ID |

**返回值**:
| 类型 | 说明 |
|------|------|
| const Symbol* | 符号指针，未找到返回 nullptr |

**使用示例**:
```cpp
auto catalog = parser.Parse("portrayal_catalogue.xml");
if (catalog) {
    if (auto* symbol = catalog->FindSymbol("ACHARE02")) {
        std::cout << "Found: " << symbol->GetFileName() << std::endl;
    }
}
```

---

## 8. 模块依赖关系

### 8.1 模块依赖图

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层                                  │
│  ┌───────────────────────────────────────────────────────┐  │
│  │              PortrayalCatalogParser                    │  │
│  └───────────────────────────┬───────────────────────────┘  │
└──────────────────────────────┼──────────────────────────────┘
                               │
┌──────────────────────────────┼──────────────────────────────┐
│                      业务层  │                              │
│  ┌──────────────┐  ┌────────┴────────┐  ┌──────────────┐   │
│  │ParserRegistry│◄─┤ ElementParser   │◄─┤SymbolParser  │   │
│  └──────────────┘  │ (interface)     │  │LineStylePars │   │
│                    └─────────────────┘  │AreaFillPars  │   │
│                                         │RuleFilePars  │   │
│                                         │ViewingGroupP │   │
│                                         └──────────────┘   │
└──────────────────────────────┬──────────────────────────────┘
                               │
┌──────────────────────────────┼──────────────────────────────┐
│                      数据层  │                              │
│  ┌──────────────┐  ┌────────┴────────┐  ┌──────────────┐   │
│  │  XmlReader   │◄─┤ LibXml2Reader   │  │PortrayalCat  │   │
│  │ (interface)  │  └─────────────────┘  │ (Model)      │   │
│  └──────────────┘                       └──────────────┘   │
│                                                             │
│  ┌──────────────┐                                          │
│  │ Model Classes│                                          │
│  │ - Description│                                          │
│  │ - FileRef    │                                          │
│  │ - Symbol     │                                          │
│  │ - LineStyle  │                                          │
│  │ - AreaFill   │                                          │
│  │ - RuleFile   │                                          │
│  │ - ViewingGrp │                                          │
│  └──────────────┘                                          │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 依赖说明

| 模块 | 依赖模块 | 依赖类型 | 说明 |
|------|----------|----------|------|
| PortrayalCatalogParser | XmlReader | 接口依赖 | 通过接口读取 XML |
| PortrayalCatalogParser | ParserRegistry | 组合 | 管理解析器 |
| PortrayalCatalogParser | PortrayalCatalog | 创建 | 创建模型对象 |
| ElementParser | XmlReader | 接口依赖 | 读取 XML 元素 |
| ElementParser | PortrayalCatalog | 参数依赖 | 填充模型数据 |
| LibXml2Reader | libxml2 | 实现依赖 | 封装 libxml2 |

---

## 9. 文件组织结构

### 9.1 目录结构

```
code/symbology/
├── include/ogc/
│   ├── symbology/
│   │   ├── style/
│   │   │   ├── s52_style_manager.h
│   │   │   └── ...
│   │   ├── symbolizer/
│   │   │   ├── symbolizer.h
│   │   │   └── ...
│   │   └── filter/
│   │       └── ...
│   └── portrayal/
│       ├── model/
│       │   ├── description.h
│       │   ├── file_reference.h
│       │   ├── symbol.h
│       │   ├── line_style.h
│       │   ├── area_fill.h
│       │   ├── style_sheet.h
│       │   ├── rule_file.h
│       │   ├── viewing_group.h
│       │   ├── alert_catalog.h
│       │   ├── color_profile.h
│       │   └── portrayal_catalog.h
│       ├── parser/
│       │   ├── xml_reader.h
│       │   ├── libxml2_reader.h
│       │   ├── element_parser.h
│       │   ├── symbol_parser.h
│       │   ├── line_style_parser.h
│       │   ├── area_fill_parser.h
│       │   ├── rule_file_parser.h
│       │   ├── viewing_group_parser.h
│       │   ├── parser_registry.h
│       │   └── portrayal_catalog_parser.h
│       └── utils/
│           ├── string_utils.h
│           └── file_utils.h
├── src/
│   ├── style/
│   ├── symbolizer/
│   ├── filter/
│   └── portrayal/
│       ├── model/
│       │   ├── description.cpp
│       │   ├── file_reference.cpp
│       │   ├── symbol.cpp
│       │   ├── line_style.cpp
│       │   ├── area_fill.cpp
│       │   ├── style_sheet.cpp
│       │   ├── rule_file.cpp
│       │   ├── viewing_group.cpp
│       │   ├── alert_catalog.cpp
│       │   ├── color_profile.cpp
│       │   └── portrayal_catalog.cpp
│       ├── parser/
│       │   ├── libxml2_reader.cpp
│       │   ├── symbol_parser.cpp
│       │   ├── line_style_parser.cpp
│       │   ├── area_fill_parser.cpp
│       │   ├── rule_file_parser.cpp
│       │   ├── viewing_group_parser.cpp
│       │   ├── parser_registry.cpp
│       │   └── portrayal_catalog_parser.cpp
│       └── utils/
│           ├── string_utils.cpp
│           └── file_utils.cpp
└── tests/
    ├── test_s52_style_manager.cpp
    ├── test_symbolizer.cpp
    └── portrayal/
        ├── test_description.cpp
        ├── test_symbol_parser.cpp
        ├── test_portrayal_catalog_parser.cpp
        └── ...
```

### 9.2 头文件设计原则

1. **最小化依赖**: 头文件只包含必要的声明
2. **前向声明**: 优先使用前向声明减少编译依赖
3. **Pimpl 模式**: 对于实现复杂的类使用 Pimpl 模式
4. **命名空间**: 所有代码在 `ogc::portrayal` 命名空间内

---

## 10. 错误处理设计

### 10.1 异常类型

```cpp
namespace ogc {
namespace portrayal {

class PortrayalException : public std::runtime_error {
public:
    explicit PortrayalException(const std::string& message);
};

class FileNotFoundException : public PortrayalException {
public:
    explicit FileNotFoundException(const std::string& filePath);
};

class XmlParseException : public PortrayalException {
public:
    XmlParseException(const std::string& message, int line = -1);
    int GetLine() const;

private:
    int line_;
};

class ValidationException : public PortrayalException {
public:
    explicit ValidationException(const std::string& message);
};

} // namespace portrayal
} // namespace ogc
```

### 10.2 错误处理策略

| 错误类型 | 处理策略 | 恢复方式 |
|----------|----------|----------|
| 文件不存在 | 抛出 FileNotFoundException | 检查文件路径 |
| XML 格式错误 | 抛出 XmlParseException | 修复 XML 文件 |
| 编码转换失败 | 记录日志，使用默认编码 | 继续解析 |
| 必需属性缺失 | 抛出 ValidationException | 补充属性 |
| 可选属性缺失 | 使用默认值 | 继续解析 |

---

## 11. 性能设计

### 11.1 性能目标

| 指标 | 目标值 | 说明 |
|------|--------|------|
| 解析时间 | < 1秒 | 16650 行文件 |
| 内存占用 | < 50MB | 加载后模型 |
| 查询响应 | < 1ms | ID 查询 |

### 11.2 性能优化策略

1. **索引优化**: 使用 map 存储元素索引，支持 O(1) 查询
2. **延迟加载**: 大型子元素可延迟加载
3. **内存池**: 复用字符串内存
4. **SAX 模式**: 对于超大文件使用 SAX 模式

### 11.3 内存管理

```cpp
// 使用智能指针管理模型对象
std::shared_ptr<model::PortrayalCatalog> catalog_;

// 使用 vector 存储元素，避免频繁内存分配
std::vector<model::Symbol> symbols_;

// 使用 map 建立索引
std::map<std::string, size_t> symbolIndex_;
```

---

## 12. 测试策略

### 12.1 单元测试

| 测试类 | 测试内容 | 覆盖目标 |
|--------|----------|----------|
| DescriptionTest | 描述类基本功能 | 100% |
| FileReferenceTest | 文件引用类功能 | 100% |
| SymbolParserTest | 符号解析 | 100% |
| LineStyleParserTest | 线型解析 | 100% |
| AreaFillParserTest | 区域填充解析 | 100% |
| RuleFileParserTest | 规则文件解析 | 100% |
| ViewingGroupParserTest | 查看组解析 | 100% |
| PortrayalCatalogParserTest | 整体解析 | 100% |
| PortrayalCatalogTest | 模型操作 | 100% |

### 12.2 集成测试

| 测试场景 | 测试内容 |
|----------|----------|
| 完整文件解析 | 解析完整的 portrayal_catalogue.xml |
| 大文件测试 | 测试 16650 行文件解析性能 |
| 编码测试 | 测试 ISO-8859-1 和 UTF-8 编码处理 |
| 错误恢复 | 测试错误 XML 文件处理 |

### 11.3 测试框架

使用 GoogleTest 框架：

```cpp
#include <gtest/gtest.h>

TEST(SymbolParserTest, ParseValidSymbol) {
    parser::SymbolParser parser;
    model::PortrayalCatalog catalog;
    
    // 创建测试 XML
    // ...
    
    EXPECT_TRUE(parser.Parse(reader, catalog));
    EXPECT_EQ(1, catalog.GetSymbols().size());
}
```

---

## 13. 扩展性设计

### 13.1 添加新元素类型

1. 创建新的模型类（继承 FileReference 或独立类）
2. 创建新的解析器类（实现 ElementParser 接口）
3. 注册解析器到 ParserRegistry

**示例：添加 Pixmap 元素**

```cpp
// 1. 模型类
class Pixmap : public FileReference {
    // ...
};

// 2. 解析器类
class PixmapParser : public ElementParser {
public:
    std::string GetElementName() const override {
        return "pixmap";
    }
    bool Parse(XmlReader& reader, 
               model::PortrayalCatalog& catalog) override {
        // ...
    }
};

// 3. 注册解析器
parser.RegisterParser(std::make_shared<PixmapParser>());
```

### 13.2 支持新的 XML 解析库

实现 XmlReader 接口：

```cpp
class RapidXmlReader : public XmlReader {
    // 实现 interface 方法
};
```

---

## 14. 使用示例

### 14.1 基本使用

```cpp
#include <ogc/portrayal/parser/portrayal_catalog_parser.h>
#include <iostream>

using namespace ogc::portrayal;

int main() {
    // 创建解析器
    parser::PortrayalCatalogParser parser;
    
    // 解析文件
    auto catalog = parser.Parse("portrayal_catalogue.xml");
    
    if (!catalog) {
        std::cerr << "解析失败" << std::endl;
        return 1;
    }
    
    // 访问数据
    std::cout << "Product ID: " << catalog->GetProductId() << std::endl;
    std::cout << "Version: " << catalog->GetVersion() << std::endl;
    std::cout << "Symbols: " << catalog->GetSymbols().size() << std::endl;
    
    // 查找特定符号
    if (auto* symbol = catalog->FindSymbol("ACHARE02")) {
        std::cout << "Symbol: " << symbol->GetFileName() << std::endl;
    }
    
    return 0;
}
```

### 14.2 自定义解析器

```cpp
#include <ogc/portrayal/parser/portrayal_catalog_parser.h>
#include <ogc/portrayal/parser/symbol_parser.h>

using namespace ogc::portrayal;

int main() {
    parser::PortrayalCatalogParser parser;
    
    // 注册自定义解析器
    parser.RegisterParser(std::make_shared<parser::SymbolParser>());
    
    // 解析
    auto catalog = parser.Parse("portrayal_catalogue.xml");
    
    // ...
}
```

---

## 15. 需求追溯矩阵

| 需求 | 设计章节 | 实现模块 | 状态 |
|------|----------|----------|------|
| 模块化 | 4. 核心模块设计 | model, parser, utils | 📋 待实现 |
| 函数可复用 | 4.3 解析器模块 | ElementParser 接口 | 📋 待实现 |
| 功能可复用 | 4.4 工具模块 | StringUtils, FileUtils | 📋 待实现 |
| 类可复用 | 4.2 数据模型模块 | FileReference 基类 | 📋 待实现 |
| 模块可复用 | 3. 系统架构 | 分层架构 | 📋 待实现 |
| C++11 支持 | 2.3 约束条件 | 所有模块 | 📋 待实现 |
| 命名空间 | 4.1 命名空间设计 | ogc::portrayal | 📋 待实现 |

---

## 16. 附录

### 16.1 术语表

| 术语 | 说明 |
|------|------|
| Portrayal Catalogue | 表现目录，定义海图符号化规则 |
| Symbol | 符号，用于点状要素表示 |
| LineStyle | 线型，用于线状要素表示 |
| AreaFill | 区域填充，用于面状要素表示 |
| RuleFile | 规则文件，Lua 脚本定义符号化规则 |
| ViewingGroup | 查看组，控制要素显示级别 |

### 16.2 参考资源

- S-401 IHO 标准
- libxml2 文档: http://www.xmlsoft.org/
- C++11 参考: https://en.cppreference.com/

### 16.3 XML 元素统计

| 元素类型 | 数量 | 说明 |
|----------|------|------|
| symbol | ~500+ | 符号定义 |
| lineStyle | ~100+ | 线型样式 |
| areaFill | ~20+ | 区域填充 |
| viewingGroup | ~100+ | 查看组 |
| ruleFile | ~200+ | 规则文件 |

---

## 17. SAX 模式实现设计

### 17.1 SAX 解析器接口

对于超大 XML 文件，提供基于事件的 SAX（Simple API for XML）解析模式，避免一次性加载整个文档到内存。

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

struct XmlAttributes {
    std::map<std::string, std::string> attrs;
    
    bool Has(const std::string& name) const;
    std::string Get(const std::string& name, const std::string& defaultValue = "") const;
};

class XmlSaxHandler {
public:
    virtual ~XmlSaxHandler() = default;
    
    virtual void StartDocument() {}
    virtual void EndDocument() {}
    virtual void StartElement(const std::string& name, 
                              const XmlAttributes& attrs) {}
    virtual void EndElement(const std::string& name) {}
    virtual void Characters(const std::string& data) {}
    virtual void Warning(const std::string& msg) {}
    virtual void Error(const std::string& msg) {}
};

class XmlSaxReader {
public:
    virtual ~XmlSaxReader() = default;
    
    virtual bool Parse(const std::string& filePath, 
                       XmlSaxHandler& handler) = 0;
    virtual std::string GetEncoding() const = 0;
    virtual int GetCurrentLineNumber() const = 0;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

### 17.2 SAX 元素解析器

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class SaxElementParser {
public:
    virtual ~SaxElementParser() = default;
    
    virtual std::string GetElementName() const = 0;
    
    virtual bool CanParse(const std::string& elementName) const {
        return elementName == GetElementName();
    }
    
    virtual void StartElement(const std::string& name,
                              const XmlAttributes& attrs,
                              model::PortrayalCatalog& catalog) = 0;
    virtual void EndElement(const std::string& name,
                            model::PortrayalCatalog& catalog) = 0;
    virtual void Characters(const std::string& data) = 0;
};

class SaxSymbolParser : public SaxElementParser {
public:
    std::string GetElementName() const override { return "symbol"; }
    
    void StartElement(const std::string& name,
                      const XmlAttributes& attrs,
                      model::PortrayalCatalog& catalog) override {
        currentSymbol_.SetId(attrs.Get("id"));
        currentText_.clear();
    }
    
    void EndElement(const std::string& name,
                    model::PortrayalCatalog& catalog) override {
        if (name == "symbol") {
            catalog.AddSymbol(currentSymbol_);
            currentSymbol_ = model::Symbol();
        } else if (name == "name") {
            currentSymbol_.GetDescription().SetName(currentText_);
        } else if (name == "description") {
            currentSymbol_.GetDescription().SetDescription(currentText_);
        }
    }
    
    void Characters(const std::string& data) override {
        currentText_ += data;
    }

private:
    model::Symbol currentSymbol_;
    std::string currentText_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

### 17.3 SAX 解析器工厂

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

enum class ParseMode {
    DOM,    // 适合小文件，支持随机访问
    SAX     // 适合大文件，流式处理
};

class ParserFactory {
public:
    static std::unique_ptr<XmlReader> CreateDomReader();
    static std::unique_ptr<XmlSaxReader> CreateSaxReader();
    
    static std::shared_ptr<PortrayalCatalog> Parse(
        const std::string& filePath,
        ParseMode mode = ParseMode::DOM);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

### 17.4 解析模式选择策略

| 文件大小 | 推荐模式 | 内存占用 | 解析速度 |
|----------|----------|----------|----------|
| < 1MB | DOM | 低 | 快 |
| 1MB - 10MB | DOM/SAX | 中 | 中 |
| > 10MB | SAX | 极低 | 稳定 |

---

## 18. 内存池实现设计

### 18.1 字符串池 (StringPool)

对于大量重复的字符串（如符号 ID、文件名），使用字符串池避免重复存储。

```cpp
namespace ogc {
namespace portrayal {
namespace utils {

class StringPool {
public:
    StringPool() = default;
    explicit StringPool(size_t initialCapacity);
    
    // 获取字符串的内部引用（自动去重）
    const std::string& Intern(const std::string& str);
    const std::string& Intern(std::string&& str);
    
    // 查找是否已存在
    bool Contains(const std::string& str) const;
    
    // 统计信息
    size_t Size() const;
    size_t MemoryUsage() const;
    
    // 预分配
    void Reserve(size_t count);
    
    // 清空
    void Clear();

private:
    std::unordered_set<std::string> pool_;
    static const std::string empty_;
};

} // namespace utils
} // namespace portrayal
} // namespace ogc
```

### 18.2 对象池 (ObjectPool)

对于频繁创建的解析器对象，使用对象池复用。

```cpp
namespace ogc {
namespace portrayal {
namespace utils {

template<typename T>
class ObjectPool {
public:
    ObjectPool() = default;
    explicit ObjectPool(size_t initialSize);
    
    std::shared_ptr<T> Acquire();
    void Release(std::shared_ptr<T> obj);
    
    size_t Available() const;
    size_t InUse() const;
    void Shrink(size_t targetSize);

private:
    std::vector<std::shared_ptr<T>> available_;
    std::atomic<size_t> inUseCount_{0};
    std::mutex mutex_;
};

using SymbolParserPool = ObjectPool<parser::SymbolParser>;
using LineStyleParserPool = ObjectPool<parser::LineStyleParser>;

} // namespace utils
} // namespace portrayal
} // namespace ogc
```

### 18.3 内存管理集成

```cpp
namespace ogc {
namespace portrayal {
namespace model {

class PortrayalCatalog {
public:
    // 使用 StringPool 管理字符串
    void SetStringPool(std::shared_ptr<utils::StringPool> pool) {
        stringPool_ = pool;
    }
    
    // 添加元素时使用字符串池
    void AddSymbol(const Symbol& symbol) {
        Symbol pooledSymbol;
        pooledSymbol.SetId(stringPool_->Intern(symbol.GetId()));
        pooledSymbol.SetFileName(stringPool_->Intern(symbol.GetFileName()));
        // ...
        symbols_.push_back(pooledSymbol);
    }

private:
    std::shared_ptr<utils::StringPool> stringPool_;
    // ...
};

} // namespace model
} // namespace portrayal
} // namespace ogc
```

### 18.4 内存使用估算

| 数据类型 | 数量 | 单项大小 | 总大小（无池） | 总大小（有池） |
|----------|------|----------|----------------|----------------|
| Symbol | 500 | ~200B | 100KB | ~40KB |
| LineStyle | 100 | ~150B | 15KB | ~8KB |
| AreaFill | 20 | ~150B | 3KB | ~2KB |
| RuleFile | 200 | ~180B | 36KB | ~15KB |
| ViewingGroup | 100 | ~120B | 12KB | ~6KB |
| **总计** | - | - | **~166KB** | **~71KB** |

**内存节省**: 约 57%

---

## 19. 线程安全设计

### 19.1 线程安全级别定义

| 类 | 线程安全级别 | 说明 |
|------|--------------|------|
| PortrayalCatalog | 读安全 | 多线程可并发读取，写入需同步 |
| PortrayalCatalogParser | 不安全 | 每个线程应使用独立实例 |
| ParserRegistry | 写安全 | 内部使用互斥锁保护 |
| StringPool | 完全安全 | 内部使用读写锁 |

### 19.2 线程安全包装器

```cpp
namespace ogc {
namespace portrayal {
namespace model {

class ThreadSafePortrayalCatalog {
public:
    explicit ThreadSafePortrayalCatalog(
        std::shared_ptr<PortrayalCatalog> catalog);
    
    // 读操作（线程安全）
    const Symbol* FindSymbol(const std::string& id) const;
    const LineStyle* FindLineStyle(const std::string& id) const;
    const AreaFill* FindAreaFill(const std::string& id) const;
    const RuleFile* FindRuleFile(const std::string& id) const;
    
    // 批量读操作
    std::vector<Symbol> GetSymbols() const;
    std::vector<LineStyle> GetLineStyles() const;
    
    // 写操作（需要独占锁）
    void AddSymbol(const Symbol& symbol);
    void AddLineStyle(const LineStyle& style);
    
    // 统计信息
    size_t SymbolCount() const;
    size_t LineStyleCount() const;

private:
    std::shared_ptr<PortrayalCatalog> catalog_;
    mutable std::shared_mutex mutex_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc
```

### 19.3 并发解析支持

#### 19.3.1 简易线程池

```cpp
namespace ogc {
namespace portrayal {
namespace utils {

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount);
    ~ThreadPool();
    
    template<typename Func, typename... Args>
    auto Enqueue(Func&& func, Args&&... args)
        -> std::future<typename std::result_of<Func(Args...)>::type>;
    
    size_t ThreadCount() const;
    void WaitAll();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_ = false;
};

} // namespace utils
} // namespace portrayal
} // namespace ogc
```

#### 19.3.2 并发解析器

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class ConcurrentParser {
public:
    explicit ConcurrentParser(size_t threadCount = 
        std::thread::hardware_concurrency());
    
    // 并发解析多个文件
    std::map<std::string, std::shared_ptr<model::PortrayalCatalog>>
    ParseFiles(const std::vector<std::string>& filePaths);
    
    // 设置回调
    void SetProgressCallback(
        std::function<void(const std::string&, double)> callback);

private:
    utils::ThreadPool pool_;
    std::mutex resultsMutex_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

### 19.4 使用指南

```cpp
// 场景1: 单线程解析
auto catalog = parser::ParserFactory::Parse("file.xml");

// 场景2: 多线程读取
auto safeCatalog = std::make_shared<model::ThreadSafePortrayalCatalog>(catalog);
std::thread t1([&]() {
    auto* symbol = safeCatalog->FindSymbol("ID1");  // 安全
});
std::thread t2([&]() {
    auto* symbol = safeCatalog->FindSymbol("ID2");  // 安全
});

// 场景3: 并发解析多文件
parser::ConcurrentParser concurrentParser;
auto results = concurrentParser.ParseFiles({"file1.xml", "file2.xml"});
```

---

## 20. 缓存层设计

### 20.1 解析结果缓存

```cpp
namespace ogc {
namespace portrayal {
namespace cache {

struct CacheEntry {
    std::shared_ptr<model::PortrayalCatalog> catalog;
    std::time_t lastModified;
    std::time_t loadTime;
    size_t memorySize;
};

class CatalogCache {
public:
    CatalogCache() = default;
    explicit CatalogCache(size_t maxMemoryMB);
    
    // 获取缓存
    std::shared_ptr<model::PortrayalCatalog> Get(
        const std::string& filePath);
    
    // 放入缓存
    void Put(const std::string& filePath,
             std::shared_ptr<model::PortrayalCatalog> catalog);
    
    // 失效
    void Invalidate(const std::string& filePath);
    void InvalidateAll();
    
    // 统计
    size_t Size() const;
    size_t MemoryUsage() const;
    double HitRate() const;

private:
    std::map<std::string, CacheEntry> cache_;
    mutable std::mutex mutex_;
    size_t maxMemoryBytes_;
    std::atomic<size_t> hits_{0};
    std::atomic<size_t> misses_{0};
    
    // LRU 访问顺序追踪
    std::list<std::string> accessOrder_;
    std::map<std::string, std::list<std::string>::iterator> accessIterators_;
    
    void EvictIfNeeded();
    void UpdateAccessOrder(const std::string& filePath);
};

} // namespace cache
} // namespace portrayal
} // namespace ogc
```

### 20.2 缓存策略

| 策略 | 说明 | 适用场景 |
|------|------|----------|
| 基于时间失效 | 文件修改时间变化时失效 | 开发环境 |
| 基于大小淘汰 | 超过内存限制时 LRU 淘汰 | 生产环境 |
| 手动失效 | 调用 invalidate() | 热加载场景 |

### 20.3 缓存集成

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class CachedPortrayalCatalogParser {
public:
    CachedPortrayalCatalogParser();
    
    std::shared_ptr<model::PortrayalCatalog> Parse(
        const std::string& filePath);
    
    void SetCacheEnabled(bool enabled);
    void SetMaxCacheSize(size_t maxMemoryMB);
    void ClearCache();

private:
    std::unique_ptr<cache::CatalogCache> cache_;
    PortrayalCatalogParser parser_;
    bool cacheEnabled_ = true;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

---

## 21. 与现有系统集成

### 21.1 与 chart/parser 模块的关系

```
┌─────────────────────────────────────────────────────────────┐
│                    模块架构                                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                  chart/parser                        │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │   │
│  │  │ S57Parser   │  │ S100Parser  │  │ S102Parser  │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│                          │                                  │
│                          ▼                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              code/symbology                          │   │
│  │  ┌──────────────────────────────────────────────┐   │   │
│  │  │ ogc::portrayal                               │   │   │
│  │  │ PortrayalCatalogParser                       │   │   │
│  │  │ - 解析 S-401 表现目录                         │   │   │
│  │  │ - 提供符号化规则                              │   │   │
│  │  └──────────────────────────────────────────────┘   │   │
│  │  ┌──────────────────────────────────────────────┐   │   │
│  │  │ ogc::symbology                               │   │   │
│  │  │ S52StyleManager / Symbolizer                  │   │   │
│  │  │ - 消费 PortrayalCatalog 进行符号化            │   │   │
│  │  └──────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 21.2 数据流关系

```
S-57/S-100 数据文件
        │
        ▼
┌───────────────┐
│ S57Parser/    │
│ S100Parser    │
└───────┬───────┘
        │
        ▼
┌───────────────┐     ┌───────────────────┐
│ Feature       │────►│ PortrayalCatalog  │
│ (地理要素)    │     │ (符号化规则)       │
└───────────────┘     └─────────┬─────────┘
                                │
        ┌───────────────────────┘
        ▼
┌───────────────┐
│ Symbology     │
│ (符号化渲染)  │
└───────────────┘
```

### 21.3 PortrayalCatalog 与 S52StyleManager 的集成

PortrayalCatalog 解析后，其数据将被 S52StyleManager 消费。集成方式如下：

```cpp
namespace ogc {
namespace symbology {

class S52StyleManager {
public:
    bool Initialize();
    
    // 加载 PortrayalCatalog 并集成到样式管理
    bool LoadPortrayalCatalog(const std::string& filePath);
    
    // 内部使用 portrayal::parser 解析
    // 解析结果填充到 SymbolLibrary、ColorSchemeManager、StyleRuleEngine
private:
    portrayal::parser::PortrayalCatalogParser catalogParser_;
    std::shared_ptr<portrayal::model::PortrayalCatalog> catalog_;
};

} // namespace symbology
} // namespace ogc
```

### 21.4 符号化集成示例

```cpp
#include <parser/chart_parser.h>
#include <ogc/portrayal/parser/portrayal_catalog_parser.h>
#include <ogc/symbology/symbolizer.h>

using namespace ogc;

// 1. 解析海图数据
parser::ChartParser chartParser;
auto features = chartParser.Parse("chart.000");

// 2. 解析表现目录
portrayal::parser::PortrayalCatalogParser catalogParser;
auto catalog = catalogParser.Parse("portrayal_catalogue.xml");

// 3. 符号化
symbology::Symbolizer symbolizer(catalog);
for (const auto& feature : features) {
    auto drawingInstructions = symbolizer.Symbolize(feature);
    // 渲染...
}
```

---

## 22. CMake 配置

### 22.1 模块 CMakeLists.txt

Portrayal 代码集成到 symbology 模块，在 `code/symbology/CMakeLists.txt` 中追加 portrayal 相关源文件：

```cmake
# code/symbology/CMakeLists.txt (追加 portrayal 部分)

# Portrayal 源文件
set(PORTRAYAL_SOURCES
    src/portrayal/model/description.cpp
    src/portrayal/model/file_reference.cpp
    src/portrayal/model/symbol.cpp
    src/portrayal/model/line_style.cpp
    src/portrayal/model/area_fill.cpp
    src/portrayal/model/style_sheet.cpp
    src/portrayal/model/rule_file.cpp
    src/portrayal/model/viewing_group.cpp
    src/portrayal/model/alert_catalog.cpp
    src/portrayal/model/color_profile.cpp
    src/portrayal/model/portrayal_catalog.cpp
    src/portrayal/parser/libxml2_reader.cpp
    src/portrayal/parser/libxml2_sax_reader.cpp
    src/portrayal/parser/symbol_parser.cpp
    src/portrayal/parser/line_style_parser.cpp
    src/portrayal/parser/area_fill_parser.cpp
    src/portrayal/parser/rule_file_parser.cpp
    src/portrayal/parser/viewing_group_parser.cpp
    src/portrayal/parser/parser_registry.cpp
    src/portrayal/parser/portrayal_catalog_parser.cpp
    src/portrayal/parser/parser_factory.cpp
    src/portrayal/utils/string_utils.cpp
    src/portrayal/utils/file_utils.cpp
    src/portrayal/utils/string_pool.cpp
    src/portrayal/cache/catalog_cache.cpp
)

# Portrayal 头文件
set(PORTRAYAL_HEADERS
    include/ogc/portrayal/model/description.h
    include/ogc/portrayal/model/file_reference.h
    include/ogc/portrayal/model/symbol.h
    include/ogc/portrayal/model/line_style.h
    include/ogc/portrayal/model/area_fill.h
    include/ogc/portrayal/model/style_sheet.h
    include/ogc/portrayal/model/rule_file.h
    include/ogc/portrayal/model/viewing_group.h
    include/ogc/portrayal/model/alert_catalog.h
    include/ogc/portrayal/model/color_profile.h
    include/ogc/portrayal/model/portrayal_catalog.h
    include/ogc/portrayal/parser/xml_reader.h
    include/ogc/portrayal/parser/xml_sax_reader.h
    include/ogc/portrayal/parser/libxml2_reader.h
    include/ogc/portrayal/parser/element_parser.h
    include/ogc/portrayal/parser/symbol_parser.h
    include/ogc/portrayal/parser/line_style_parser.h
    include/ogc/portrayal/parser/area_fill_parser.h
    include/ogc/portrayal/parser/rule_file_parser.h
    include/ogc/portrayal/parser/viewing_group_parser.h
    include/ogc/portrayal/parser/parser_registry.h
    include/ogc/portrayal/parser/portrayal_catalog_parser.h
    include/ogc/portrayal/parser/parser_factory.h
    include/ogc/portrayal/utils/string_utils.h
    include/ogc/portrayal/utils/file_utils.h
    include/ogc/portrayal/utils/string_pool.h
    include/ogc/portrayal/cache/catalog_cache.h
)

# 追加到 symbology 库
target_sources(ogc_symbology
    PRIVATE
        ${PORTRAYAL_SOURCES}
        ${PORTRAYAL_HEADERS}
)

# 链接 libxml2（若 symbology 尚未链接）
find_package(LibXml2 REQUIRED)
target_link_libraries(ogc_symbology
    PUBLIC
        ${LIBXML2_LIBRARIES}
)
target_include_directories(ogc_symbology
    PRIVATE
        ${LIBXML2_INCLUDE_DIRS}
)
target_compile_definitions(ogc_symbology
    PRIVATE
        LIBXML_STATIC
)
```

### 22.2 测试 CMakeLists.txt

```cmake
# code/symbology/tests/CMakeLists.txt (追加 portrayal 测试)

# Portrayal 测试源文件
set(PORTRAYAL_TEST_SOURCES
    portrayal/test_description.cpp
    portrayal/test_file_reference.cpp
    portrayal/test_symbol_parser.cpp
    portrayal/test_line_style_parser.cpp
    portrayal/test_area_fill_parser.cpp
    portrayal/test_rule_file_parser.cpp
    portrayal/test_viewing_group_parser.cpp
    portrayal/test_portrayal_catalog_parser.cpp
    portrayal/test_portrayal_catalog.cpp
    portrayal/test_string_pool.cpp
    portrayal/test_catalog_cache.cpp
    portrayal/test_thread_safety.cpp
    portrayal/test_integration.cpp
)

# 追加到 symbology 测试
target_sources(ogc_symbology_tests
    PRIVATE
        ${PORTRAYAL_TEST_SOURCES}
)
```

### 22.3 使用示例

```cmake
# 在其他模块中使用 symbology（含 portrayal）

find_package(ogc_symbology REQUIRED)

add_executable(my_app main.cpp)

target_link_libraries(my_app
    PRIVATE
        ogc_symbology
)
```

---

## 23. 解析流程时序图

### 23.1 DOM 模式解析流程

```
┌────────┐     ┌────────────────────────┐     ┌───────────┐     ┌──────────────┐     ┌─────────────────┐
│ Client │     │PortrayalCatalogParser  │     │ XmlReader │     │ParserRegistry│     │ ElementParser  │
└───┬────┘     └───────────┬────────────┘     └─────┬─────┘     └──────┬───────┘     └────────┬────────┘
    │                      │                        │                  │                      │
    │ Parse(filePath)      │                        │                  │                      │
    │─────────────────────►│                        │                  │                      │
    │                      │                        │                  │                      │
    │                      │ Open(filePath)         │                  │                      │
    │                      │───────────────────────►│                  │                      │
    │                      │                        │                  │                      │
    │                      │ success                │                  │                      │
    │                      │◄───────────────────────│                  │                      │
    │                      │                        │                  │                      │
    │                      │ ReadNextElement()      │                  │                      │
    │                      │───────────────────────►│                  │                      │
    │                      │                        │                  │                      │
    │                      │ elementName            │                  │                      │
    │                      │◄───────────────────────│                  │                      │
    │                      │                        │                  │                      │
    │                      │ GetParser(elementName) │                  │                      │
    │                      │────────────────────────────────────────────►│                      │
    │                      │                        │                  │                      │
    │                      │ parser                 │                  │                      │
    │                      │◄────────────────────────────────────────────│                      │
    │                      │                        │                  │                      │
    │                      │ Parse(reader, catalog) │                  │                      │
    │                      │─────────────────────────────────────────────────────────────────────►│
    │                      │                        │                  │                      │
    │                      │                        │                  │                      │
    │                      │                        │  ┌───────────────────────────────────────┐│
    │                      │                        │  │ loop: read child elements             ││
    │                      │                        │  │  - MoveToFirstChild()                 ││
    │                      │                        │  │  - GetCurrentElementName()            ││
    │                      │                        │  │  - GetCurrentElementText()            ││
    │                      │                        │  │  - MoveToParent()                    ││
    │                      │                        │  └───────────────────────────────────────┘│
    │                      │                        │                  │                      │
    │                      │ success                │                  │                      │
    │                      │◄─────────────────────────────────────────────────────────────────────│
    │                      │                        │                  │                      │
    │                      │    ┌───────────────────────────────────────┐                  │
    │                      │    │ repeat for each element type          │                  │
    │                      │    └───────────────────────────────────────┘                  │
    │                      │                        │                  │                      │
    │                      │ Close()                │                  │                      │
    │                      │───────────────────────►│                  │                      │
    │                      │                        │                  │                      │
    │                      │                        │                  │                      │
    │ catalog              │                        │                  │                      │
    │◄─────────────────────│                        │                  │                      │
    │                      │                        │                  │                      │
```

### 23.2 SAX 模式解析流程

```
┌────────┐     ┌────────────────────────┐     ┌──────────────┐     ┌────────────────┐
│ Client │     │   SaxPortrayalParser   │     │XmlSaxReader  │     │ SaxElementPars │
└───┬────┘     └───────────┬────────────┘     └──────┬───────┘     └───────┬────────┘
    │                      │                         │                     │
    │ Parse(filePath)      │                         │                     │
    │─────────────────────►│                         │                     │
    │                      │                         │                     │
    │                      │ Parse(filePath, handler)│                     │
    │                      │────────────────────────►│                     │
    │                      │                         │                     │
    │                      │                         │ startDocument()     │
    │                      │◄────────────────────────│                     │
    │                      │                         │                     │
    │                      │                         │ ┌─────────────────────────────────┐
    │                      │                         │ │ loop: stream elements           │
    │                      │                         │ └─────────────────────────────────┘
    │                      │                         │                     │
    │                      │                         │ startElement(name)  │
    │                      │◄────────────────────────│                     │
    │                      │                         │                     │
    │                      │ GetParser(name)         │                     │
    │                      │────────────────────────────────────────────────►
    │                      │                         │                     │
    │                      │ parser                  │                     │
    │                      │◄────────────────────────────────────────────────
    │                      │                         │                     │
    │                      │ startElement(attrs)     │                     │
    │                      │────────────────────────────────────────────────►
    │                      │                         │                     │
    │                      │                         │ characters(data)    │
    │                      │◄────────────────────────│                     │
    │                      │                         │                     │
    │                      │ characters(data)        │                     │
    │                      │────────────────────────────────────────────────►
    │                      │                         │                     │
    │                      │                         │ endElement(name)    │
    │                      │◄────────────────────────│                     │
    │                      │                         │                     │
    │                      │ endElement()            │                     │
    │                      │────────────────────────────────────────────────►
    │                      │                         │                     │
    │                      │                         │ endDocument()       │
    │                      │◄────────────────────────│                     │
    │                      │                         │                     │
    │ catalog              │                         │                     │
    │◄─────────────────────│                         │                     │
    │                      │                         │                     │
```

### 23.3 缓存解析流程

```
┌────────┐     ┌────────────────────────────┐     ┌──────────────┐     ┌─────────────────────┐
│ Client │     │CachedPortrayalCatalogParser│     │CatalogCache  │     │PortrayalCatalogPars │
└───┬────┘     └─────────────┬──────────────┘     └──────┬───────┘     └──────────┬──────────┘
    │                        │                           │                        │
    │ Parse(filePath)        │                           │                        │
    │───────────────────────►│                           │                        │
    │                        │                           │                        │
    │                        │ Get(filePath)             │                        │
    │                        │──────────────────────────►│                        │
    │                        │                           │                        │
    │                        │           ┌───────────────┤ check lastModified     │
    │                        │           │               │                        │
    │                        │           │ cache hit?    │                        │
    │                        │◄──────────┼───────────────│                        │
    │                        │           │               │                        │
    │                        │     ┌─────┴─────┐         │                        │
    │                        │     │           │         │                        │
    │                        │   Yes          No         │                        │
    │                        │     │           │         │                        │
    │                        │     │           │ Parse(filePath)                  │
    │                        │     │           │────────────────────────────────►│
    │                        │     │           │                                │
    │                        │     │           │ catalog                        │
    │                        │     │           │◄───────────────────────────────│
    │                        │     │           │                                │
    │                        │     │           │ Put(filePath, catalog)          │
    │                        │     │           │───────────────────────────────►│
    │                        │     │           │                                │
    │                        │     │           │                                │
    │ catalog                │     │           │                                │
    │◄───────────────────────│◄────┴───────────┘                                │
    │                        │                           │                        │
```

---

## 24. 版本兼容性设计

### 24.1 版本检测

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class VersionDetector {
public:
    struct VersionInfo {
        std::string productId;
        std::string version;
    };
    
    static VersionInfo Detect(const std::string& filePath);
    static bool IsSupported(const VersionInfo& info);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

### 24.2 向后兼容策略

| 版本变化 | 兼容策略 | 说明 |
|----------|----------|------|
| 新增元素 | 自动忽略 | 解析器跳过未知元素 |
| 新增属性 | 使用默认值 | 不影响现有功能 |
| 元素重命名 | 别名支持 | 注册别名映射 |
| 结构变化 | 版本适配器 | 提供转换函数 |

### 23.3 版本适配器

```cpp
namespace ogc {
namespace portrayal {
namespace parser {

class VersionAdapter {
public:
    virtual ~VersionAdapter() = default;
    
    virtual void Adapt(model::PortrayalCatalog& catalog) = 0;
    virtual std::string GetTargetVersion() const = 0;
};

class S401_V1_0_to_V1_1_Adapter : public VersionAdapter {
public:
    void Adapt(model::PortrayalCatalog& catalog) override {
        // 转换逻辑
    }
    
    std::string GetTargetVersion() const override {
        return "1.1";
    }
};

} // namespace parser
} // namespace portrayal
} // namespace ogc
```

---

## 24. 跨平台兼容性

### 24.1 平台差异处理

| 平台 | 差异项 | 处理方式 |
|------|--------|----------|
| Windows | 路径分隔符 `\` | 统一转换为 `/` |
| Windows | 文件编码 | 自动检测 BOM |
| Linux | 文件权限 | 检查读取权限 |
| macOS | 资源路径 | 支持 Bundle 路径 |

### 24.2 跨平台工具类

```cpp
namespace ogc {
namespace portrayal {
namespace utils {

class PlatformUtils {
public:
    static std::string NormalizePath(const std::string& path);
    static std::string GetCurrentEncoding();
    static bool IsAbsolutePath(const std::string& path);
    static std::string GetExecutableDirectory();
};

} // namespace utils
} // namespace portrayal
} // namespace ogc
```

---

**文档结束**
