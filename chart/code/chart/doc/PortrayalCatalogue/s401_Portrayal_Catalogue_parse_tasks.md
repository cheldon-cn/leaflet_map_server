# S-401 Portrayal Catalogue 解析 - 任务计划

## 概述
- 总任务数: 30
- 预估总工时: 136h (PERT 期望值)
- 关键路径工时: 72h
- 目标完成: 4 周
- 团队规模: 1 开发者

## 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## 任务摘要

| Task ID | 任务名称 | 优先级 | 里程碑 | 工时 | 状态 | 设计章节 | 依赖 |
|---------|----------|--------|--------|------|------|----------|------|
| T1 | 项目结构与CMake配置 | P0 | M1 | 4h | 📋 Todo | §3,§22 | - |
| T2 | Description模型类 | P0 | M1 | 2h | 📋 Todo | §5.2 | T1 |
| T3 | FileReference基类 | P0 | M1 | 3h | 📋 Todo | §5.2 | T2 |
| T4 | 具体模型类(Symbol等7个) | P0 | M1 | 4h | 📋 Todo | §5.2 | T3 |
| T5 | ViewingGroup模型类 | P0 | M1 | 2h | 📋 Todo | §5.2 | T2 |
| T6 | PortrayalCatalog聚合类 | P0 | M1 | 6h | 📋 Todo | §5.2 | T4,T5 |
| T7 | StringUtils工具类 | P0 | M1 | 3h | 📋 Todo | §5.4 | T1 |
| T8 | FileUtils工具类 | P1 | M1 | 2h | 📋 Todo | §5.4 | T1 |
| T9 | XmlReader接口定义 | P0 | M2 | 2h | 📋 Todo | §5.3 | T1 |
| T10 | LibXml2Reader实现 | P0 | M2 | 8h | 📋 Todo | §5.3 | T9 |
| T11 | ElementParser接口定义 | P0 | M2 | 2h | 📋 Todo | §5.3 | T9 |
| T12 | SymbolParser实现 | P0 | M2 | 4h | 📋 Todo | §5.3 | T11,T4 |
| T13 | LineStyleParser实现 | P1 | M2 | 3h | 📋 Todo | §5.3 | T11,T4 |
| T14 | AreaFillParser实现 | P1 | M2 | 3h | 📋 Todo | §5.3 | T11,T4 |
| T15 | RuleFileParser实现 | P1 | M2 | 4h | 📋 Todo | §5.3 | T11,T4 |
| T16 | ViewingGroupParser实现 | P1 | M2 | 3h | 📋 Todo | §5.3 | T11,T5 |
| T17 | AlertCatalogParser实现 | P1 | M2 | 3h | 📋 Todo | §5.3 | T11,T4 |
| T18 | ColorProfileParser实现 | P1 | M2 | 3h | 📋 Todo | §5.3 | T11,T4 |
| T19 | StyleSheetParser实现 | P1 | M2 | 3h | 📋 Todo | §5.3 | T11,T4 |
| T20 | ParserRegistry实现 | P0 | M2 | 4h | 📋 Todo | §5.3 | T11 |
| T21 | PortrayalCatalogParser门面 | P0 | M2 | 6h | 📋 Todo | §5.3 | T10,T20 |
| T22 | SAX解析器(XmlSaxHandler/Reader) | P2 | M3 | 6h | 📋 Todo | §17 | T9 |
| T23 | SAX元素解析器(SaxSymbolParser等) | P2 | M3 | 6h | 📋 Todo | §17 | T22,T4 |
| T24 | ParserFactory与模式选择 | P2 | M3 | 3h | 📋 Todo | §17 | T22 |
| T25 | StringPool与ObjectPool | P2 | M3 | 4h | 📋 Todo | §18 | T7 |
| T26 | ThreadSafePortrayalCatalog | P2 | M4 | 4h | 📋 Todo | §19 | T6 |
| T27 | CatalogCache与CachedParser | P2 | M4 | 4h | 📋 Todo | §20 | T21 |
| T28 | VersionDetector与VersionAdapter | P3 | M4 | 3h | 📋 Todo | §24 | T21 |
| T29 | PlatformUtils跨平台工具 | P3 | M4 | 2h | 📋 Todo | §24 | T8 |
| T30 | 单元测试与集成测试 | P0 | M4 | 16h | 📋 Todo | §12 | T21 |

## 资源分配
| 开发者 | 技能 | 分配 | 任务 |
|--------|------|------|------|
| Dev A | C++, libxml2, CMake | 100% | T1-T30 |

## 关键路径
T1 → T2 → T3 → T4 → T6 → T9 → T10 → T11 → T20 → T21 → T30 (72h)
⚠️ 关键路径上的任何延迟将延迟整个项目

## 里程碑

### M1: 基础框架 (第1周)
| 任务 | 优先级 | 工时 | 状态 | 浮动时间 |
|------|----------|------|------|----------|
| T1: 项目结构与CMake配置 | P0 | 4h | 📋 Todo | 0h ⚠️ |
| T2: Description模型类 | P0 | 2h | 📋 Todo | 0h ⚠️ |
| T3: FileReference基类 | P0 | 3h | 📋 Todo | 0h ⚠️ |
| T4: 具体模型类(Symbol等7个) | P0 | 4h | 📋 Todo | 0h ⚠️ |
| T5: ViewingGroup模型类 | P0 | 2h | 📋 Todo | 2h |
| T6: PortrayalCatalog聚合类 | P0 | 6h | 📋 Todo | 0h ⚠️ |
| T7: StringUtils工具类 | P0 | 3h | 📋 Todo | 4h |
| T8: FileUtils工具类 | P1 | 2h | 📋 Todo | 5h |

### M2: 核心解析功能 (第2-3周)
| 任务 | 优先级 | 工时 | 状态 | 浮动时间 |
|------|----------|------|------|----------|
| T9: XmlReader接口定义 | P0 | 2h | 📋 Todo | 0h ⚠️ |
| T10: LibXml2Reader实现 | P0 | 8h | 📋 Todo | 0h ⚠️ |
| T11: ElementParser接口定义 | P0 | 2h | 📋 Todo | 0h ⚠️ |
| T12: SymbolParser实现 | P0 | 4h | 📋 Todo | 4h |
| T13: LineStyleParser实现 | P1 | 3h | 📋 Todo | 5h |
| T14: AreaFillParser实现 | P1 | 3h | 📋 Todo | 5h |
| T15: RuleFileParser实现 | P1 | 4h | 📋 Todo | 4h |
| T16: ViewingGroupParser实现 | P1 | 3h | 📋 Todo | 5h |
| T17: AlertCatalogParser实现 | P1 | 3h | 📋 Todo | 5h |
| T18: ColorProfileParser实现 | P1 | 3h | 📋 Todo | 5h |
| T19: StyleSheetParser实现 | P1 | 3h | 📋 Todo | 5h |
| T20: ParserRegistry实现 | P0 | 4h | 📋 Todo | 0h ⚠️ |
| T21: PortrayalCatalogParser门面 | P0 | 6h | 📋 Todo | 0h ⚠️ |

### M3: 高级特性 (第3-4周)
| 任务 | 优先级 | 工时 | 状态 | 浮动时间 |
|------|----------|------|------|----------|
| T22: SAX解析器 | P2 | 6h | 📋 Todo | 8h |
| T23: SAX元素解析器 | P2 | 6h | 📋 Todo | 8h |
| T24: ParserFactory与模式选择 | P2 | 3h | 📋 Todo | 8h |
| T25: StringPool与ObjectPool | P2 | 4h | 📋 Todo | 10h |

### M4: 优化完善 (第4周)
| 任务 | 优先级 | 工时 | 状态 | 浮动时间 |
|------|----------|------|------|----------|
| T26: ThreadSafePortrayalCatalog | P2 | 4h | 📋 Todo | 12h |
| T27: CatalogCache与CachedParser | P2 | 4h | 📋 Todo | 12h |
| T28: VersionDetector与VersionAdapter | P3 | 3h | 📋 Todo | 16h |
| T29: PlatformUtils跨平台工具 | P3 | 2h | 📋 Todo | 18h |
| T30: 单元测试与集成测试 | P0 | 16h | 📋 Todo | 0h ⚠️ |

## 依赖图

```
[T1: 项目结构] ──► [T2: Description] ──► [T3: FileReference] ──► [T4: 具体模型类] ──► [T6: PortrayalCatalog]
       │                   │                                         │                      │
       │                   └──► [T5: ViewingGroup] ──────────────────┘                      │
       │                                                                                │
       ├──► [T7: StringUtils] ──► [T25: StringPool/ObjectPool]                            │
       ├──► [T8: FileUtils] ──► [T29: PlatformUtils]                                     │
       │                                                                                │
       └──► [T9: XmlReader接口] ──► [T10: LibXml2Reader] ──┐                             │
                    │                                      │                             │
                    ├──► [T22: SAX解析器] ──► [T23: SAX元素解析器]                        │
                    │                    └──► [T24: ParserFactory]                        │
                    │                                                                    │
                    └──► [T11: ElementParser接口] ──► [T12-T19: 具体解析器]               │
                                         │                                              │
                                         └──► [T20: ParserRegistry] ──► [T21: 门面解析器]
                                                                            │              │
                                                              [T26: 线程安全]◄┘              │
                                                              [T27: 缓存]◄─────────────────┘
                                                              [T28: 版本兼容]◄──────────────┘
                                                                                           │
                                                              [T30: 测试]◄────────────────┘
```

## 风险登记

| 风险 | 概率 | 影响 | 缓解措施 | 负责人 |
|------|------|------|----------|--------|
| libxml2 API复杂度高 | 中 | 高 | 先实现DOM模式，SAX模式延后 | Dev A |
| XML编码处理(ISO-8859-1) | 中 | 中 | 使用libxml2内置编码转换 | Dev A |
| symbology模块集成冲突 | 低 | 高 | 独立命名空间ogc::portrayal | Dev A |
| 大文件解析性能 | 低 | 中 | SAX模式兜底，StringPool优化 | Dev A |
| C++11限制(无optional等) | 低 | 低 | 使用原始指针+nullptr | Dev A |

## 详细任务描述

### T1 - 项目结构与CMake配置

#### 描述
- 在 code/symbology 下创建 portrayal 子目录结构
- 创建 include/ogc/portrayal/ 目录结构 (model/, parser/, utils/)
- 创建 src/portrayal/ 目录结构 (model/, parser/, utils/)
- 创建 tests/portrayal/ 目录
- 修改 symbology/CMakeLists.txt，添加 portrayal 源文件和头文件
- 配置 libxml2 依赖 (find_package, target_link_libraries)
- 确保编译通过（空实现）

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §3, §22

#### 优先级
P0: 关键阻塞

#### 依赖
无

#### 验收标准
- [ ] **功能**: 目录结构符合方案B设计（include/ogc/portrayal/, src/portrayal/）
- [ ] **功能**: CMakeLists.txt 包含 portrayal 子目录的所有源文件
- [ ] **功能**: libxml2 依赖正确配置（find_package + target_link_libraries）
- [ ] **质量**: 编译通过，无警告
- [ ] **集成**: ogc_symbology 目标可正常构建

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 8h
- 期望: 4.33h
- 信心: 高 (>80%)
- 复杂度: 中

#### 里程碑
M1

#### 状态
📋 Todo

---

### T2 - Description模型类

#### 描述
- 实现 ogc::portrayal::model::Description 类
- 包含 name_, description_, language_ 成员
- 实现 getter/setter 方法
- 头文件: include/ogc/portrayal/model/description.h
- 源文件: src/portrayal/model/description.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.2

#### 优先级
P0: 关键阻塞

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 构造函数（默认/带参）正确实现
- [ ] **功能**: getter/setter 方法正确实现
- [ ] **覆盖**: 行覆盖 ≥80%
- [ ] **质量**: 无内存泄漏

#### 预估工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T3 - FileReference基类

#### 描述
- 实现 ogc::portrayal::model::FileReference 抽象基类
- 包含 id_, description_, fileName_, fileType_, fileFormat_ 成员
- 虚析构函数
- 实现 getter/setter 方法
- 头文件: include/ogc/portrayal/model/file_reference.h
- 源文件: src/portrayal/model/file_reference.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.2

#### 优先级
P0: 关键阻塞

#### 依赖
T2

#### 验收标准
- [ ] **功能**: 虚析构函数正确实现
- [ ] **功能**: 所有 getter/setter 正确实现
- [ ] **功能**: Description 组合关系正确
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T4 - 具体模型类(Symbol等7个)

#### 描述
- 实现 7 个 FileReference 子类: Symbol, LineStyle, AreaFill, StyleSheet, AlertCatalog, ColorProfile, RuleFile
- RuleFile 额外包含 ruleType_ 属性和 getter/setter
- 其他子类使用 using FileReference::FileReference 继承构造函数
- 头文件: include/ogc/portrayal/model/symbol.h, line_style.h, area_fill.h, style_sheet.h, alert_catalog.h, color_profile.h, rule_file.h
- 源文件: src/portrayal/model/ 对应 .cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.2

#### 优先级
P0: 关键阻塞

#### 依赖
T3

#### 验收标准
- [ ] **功能**: 7 个模型类全部实现
- [ ] **功能**: RuleFile 包含 ruleType_ 属性
- [ ] **功能**: 继承关系正确（public FileReference）
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 6h
- 期望: 4h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T5 - ViewingGroup模型类

#### 描述
- 实现 ogc::portrayal::model::ViewingGroup 类
- 不继承 FileReference，独立类
- 包含 id_, description_ 成员
- 头文件: include/ogc/portrayal/model/viewing_group.h
- 源文件: src/portrayal/model/viewing_group.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.2

#### 优先级
P0

#### 依赖
T2

#### 验收标准
- [ ] **功能**: id_ 和 description_ getter/setter 正确
- [ ] **功能**: 不继承 FileReference
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 3h
- 期望: 2h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T6 - PortrayalCatalog聚合类

#### 描述
- 实现 ogc::portrayal::model::PortrayalCatalog 聚合根类
- 包含 productId_, version_ 基本信息
- 包含所有子元素集合（alertCatalog_, colorProfiles_, symbols_, styleSheets_, lineStyles_, areaFills_, viewingGroups_, ruleFiles_）
- 实现索引映射（symbolIndex_, lineStyleIndex_ 等）用于快速查询
- 实现 Add* 方法和 Find* 查询方法
- 可选的 StringPool 集成
- 头文件: include/ogc/portrayal/model/portrayal_catalog.h
- 源文件: src/portrayal/model/portrayal_catalog.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.2

#### 优先级
P0: 关键阻塞

#### 依赖
T4, T5

#### 验收标准
- [ ] **功能**: 所有集合的 getter 方法正确返回
- [ ] **功能**: Add* 方法正确添加元素并维护索引
- [ ] **功能**: Find* 方法通过索引快速查找
- [ ] **功能**: SetStringPool 可选集成
- [ ] **性能**: Find* 方法 O(log n) 查找
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 4h
- 最可能: 6h
- 悲观: 10h
- 期望: 6.33h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T7 - StringUtils工具类

#### 描述
- 实现 ogc::portrayal::utils::StringUtils 静态工具类
- 方法: Trim, ToLower, ToUpper, Split, Join, StartsWith, EndsWith, ConvertEncoding
- 头文件: include/ogc/portrayal/utils/string_utils.h
- 源文件: src/portrayal/utils/string_utils.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.4

#### 优先级
P0

#### 依赖
T1

#### 验收标准
- [ ] **功能**: Trim 正确处理首尾空白
- [ ] **功能**: ConvertEncoding 支持 ISO-8859-1 → UTF-8
- [ ] **功能**: Split/Join 互为逆操作
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T8 - FileUtils工具类

#### 描述
- 实现 ogc::portrayal::utils::FileUtils 静态工具类
- 方法: Exists, IsFile, IsDirectory, GetDirectory, GetFileName, GetFileExtension, CombinePath
- 头文件: include/ogc/portrayal/utils/file_utils.h
- 源文件: src/portrayal/utils/file_utils.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.4

#### 优先级
P1

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 跨平台路径处理（Windows/Linux）
- [ ] **功能**: CombinePath 正确拼接路径
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h

#### 里程碑
M1

#### 状态
📋 Todo

---

### T9 - XmlReader接口定义

#### 描述
- 定义 ogc::portrayal::parser::XmlReader 抽象接口
- 方法: Open, Close, IsOpen, GetEncoding, ReadNextElement, GetCurrentElementName, GetCurrentElementText, GetAttribute, HasAttribute, MoveToFirstChild, MoveToNextSibling, MoveToParent
- 纯虚析构函数
- 头文件: include/ogc/portrayal/parser/xml_reader.h

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P0: 关键阻塞

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 接口方法完整定义
- [ ] **功能**: 虚析构函数
- [ ] **质量**: 纯头文件接口，无实现

#### 预估工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 3h
- 期望: 2h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T10 - LibXml2Reader实现

#### 描述
- 实现 ogc::portrayal::parser::LibXml2Reader 类
- 使用 PIMPL 模式封装 libxml2 的 xmlTextReader API
- 实现 XmlReader 接口的所有方法
- 支持 ISO-8859-1 和 UTF-8 编码自动检测
- 头文件: include/ogc/portrayal/parser/libxml2_reader.h
- 源文件: src/portrayal/parser/libxml2_reader.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P0: 关键阻塞

#### 依赖
T9

#### 验收标准
- [ ] **功能**: Open/Close 正确管理 libxml2 资源
- [ ] **功能**: ReadNextElement 正确遍历 XML 元素
- [ ] **功能**: GetAttribute 正确读取属性值
- [ ] **功能**: MoveToFirstChild/MoveToNextSibling/MoveToParent 导航正确
- [ ] **功能**: 支持 ISO-8859-1 和 UTF-8 编码
- [ ] **质量**: PIMPL 模式隔离 libxml2 头文件依赖
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 5h
- 最可能: 8h
- 悲观: 14h
- 期望: 8.5h
- 信心: 中 (50-80%)
- 复杂度: 高

#### 里程碑
M2

#### 状态
📋 Todo

---

### T11 - ElementParser接口定义

#### 描述
- 定义 ogc::portrayal::parser::ElementParser 抽象接口
- 方法: GetElementName, CanParse, Parse
- 头文件: include/ogc/portrayal/parser/element_parser.h

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P0: 关键阻塞

#### 依赖
T9

#### 验收标准
- [ ] **功能**: 接口方法完整定义
- [ ] **功能**: CanParse 默认实现比较 GetElementName()
- [ ] **质量**: 纯头文件接口

#### 预估工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 3h
- 期望: 2h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T12 - SymbolParser实现

#### 描述
- 实现 ogc::portrayal::parser::SymbolParser 类
- 解析 <symbol> 元素，提取 id, description, fileName, fileType, fileFormat
- 内部 ParseDescription 辅助方法
- 头文件: include/ogc/portrayal/parser/symbol_parser.h
- 源文件: src/portrayal/parser/symbol_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P0

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 symbol 元素的所有属性和子元素
- [ ] **功能**: 解析结果通过 catalog.AddSymbol 添加
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 6h
- 期望: 4h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T13 - LineStyleParser实现

#### 描述
- 实现 ogc::portrayal::parser::LineStyleParser 类
- 解析 <lineStyle> 元素
- 头文件: include/ogc/portrayal/parser/line_style_parser.h
- 源文件: src/portrayal/parser/line_style_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 lineStyle 元素
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T14 - AreaFillParser实现

#### 描述
- 实现 ogc::portrayal::parser::AreaFillParser 类
- 解析 <areaFill> 元素
- 头文件: include/ogc/portrayal/parser/area_fill_parser.h
- 源文件: src/portrayal/parser/area_fill_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 areaFill 元素
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T15 - RuleFileParser实现

#### 描述
- 实现 ogc::portrayal::parser::RuleFileParser 类
- 解析 <ruleFile> 元素，额外提取 ruleType 属性
- 头文件: include/ogc/portrayal/parser/rule_file_parser.h
- 源文件: src/portrayal/parser/rule_file_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 ruleFile 元素及 ruleType 属性
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 6h
- 期望: 4h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T16 - ViewingGroupParser实现

#### 描述
- 实现 ogc::portrayal::parser::ViewingGroupParser 类
- 解析 <viewingGroup> 元素（非 FileReference 结构）
- 头文件: include/ogc/portrayal/parser/viewing_group_parser.h
- 源文件: src/portrayal/parser/viewing_group_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T5

#### 验收标准
- [ ] **功能**: 正确解析 viewingGroup 元素
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T17 - AlertCatalogParser实现

#### 描述
- 实现 ogc::portrayal::parser::AlertCatalogParser 类
- 解析 <alertCatalog> 元素
- 头文件: include/ogc/portrayal/parser/alert_catalog_parser.h
- 源文件: src/portrayal/parser/alert_catalog_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 alertCatalog 元素
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T18 - ColorProfileParser实现

#### 描述
- 实现 ogc::portrayal::parser::ColorProfileParser 类
- 解析 <colorProfile> 元素
- 头文件: include/ogc/portrayal/parser/color_profile_parser.h
- 源文件: src/portrayal/parser/color_profile_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 colorProfile 元素
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T19 - StyleSheetParser实现

#### 描述
- 实现 ogc::portrayal::parser::StyleSheetParser 类
- 解析 <styleSheet> 元素
- 头文件: include/ogc/portrayal/parser/style_sheet_parser.h
- 源文件: src/portrayal/parser/style_sheet_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P1

#### 依赖
T11, T4

#### 验收标准
- [ ] **功能**: 正确解析 styleSheet 元素
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T20 - ParserRegistry实现

#### 描述
- 实现 ogc::portrayal::parser::ParserRegistry 类
- 方法: RegisterParser, UnregisterParser, GetParser, HasParser, GetRegisteredElements
- 内部使用 std::map<std::string, std::shared_ptr<ElementParser>> 存储
- 头文件: include/ogc/portrayal/parser/parser_registry.h
- 源文件: src/portrayal/parser/parser_registry.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3

#### 优先级
P0: 关键阻塞

#### 依赖
T11

#### 验收标准
- [ ] **功能**: 注册/注销解析器正确
- [ ] **功能**: GetParser 返回已注册的解析器
- [ ] **功能**: 未注册元素返回 nullptr
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 6h
- 期望: 4h

#### 里程碑
M2

#### 状态
📋 Todo

---

### T21 - PortrayalCatalogParser门面

#### 描述
- 实现 ogc::portrayal::parser::PortrayalCatalogParser 门面类
- 组合 XmlReader + ParserRegistry
- 方法: SetXmlReader, RegisterParser, Parse, SetValidateEnabled, SetIgnoreComments
- Parse 方法流程: 打开文件 → 解析根元素属性 → 遍历子元素 → 分派到对应解析器 → 返回 PortrayalCatalog
- 默认注册所有内置解析器
- 头文件: include/ogc/portrayal/parser/portrayal_catalog_parser.h
- 源文件: src/portrayal/parser/portrayal_catalog_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §5.3, §14

#### 优先级
P0: 关键阻塞

#### 依赖
T10, T20

#### 验收标准
- [ ] **功能**: Parse 方法正确解析完整 portrayal_catalogue.xml
- [ ] **功能**: 默认注册所有8个内置解析器
- [ ] **功能**: 支持自定义 XmlReader 注入
- [ ] **功能**: 支持自定义解析器注册
- [ ] **集成**: 解析结果 PortrayalCatalog 数据完整
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 4h
- 最可能: 6h
- 悲观: 10h
- 期望: 6.33h
- 信心: 中 (50-80%)
- 复杂度: 高

#### 里程碑
M2

#### 状态
📋 Todo

---

### T22 - SAX解析器(XmlSaxHandler/Reader)

#### 描述
- 定义 ogc::portrayal::parser::XmlAttributes 结构体
- 定义 ogc::portrayal::parser::XmlSaxHandler 接口
- 定义 ogc::portrayal::parser::XmlSaxReader 接口
- 实现 LibXml2SaxReader（基于 libxml2 SAX API）
- 头文件: include/ogc/portrayal/parser/xml_sax_handler.h, xml_sax_reader.h, libxml2_sax_reader.h
- 源文件: src/portrayal/parser/libxml2_sax_reader.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §17

#### 优先级
P2

#### 依赖
T9

#### 验收标准
- [ ] **功能**: SAX 事件正确触发（startElement/endElement/characters）
- [ ] **功能**: XmlAttributes 正确传递属性
- [ ] **功能**: 支持流式解析大文件
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 4h
- 最可能: 6h
- 悲观: 10h
- 期望: 6.33h
- 复杂度: 高

#### 里程碑
M3

#### 状态
📋 Todo

---

### T23 - SAX元素解析器(SaxSymbolParser等)

#### 描述
- 定义 ogc::portrayal::parser::SaxElementParser 接口
- 实现 SaxSymbolParser 作为示例
- 实现 SaxPortrayalCatalogParser（SAX 模式门面）
- 头文件: include/ogc/portrayal/parser/sax_element_parser.h, sax_symbol_parser.h, sax_portrayal_catalog_parser.h
- 源文件: src/portrayal/parser/ 对应 .cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §17

#### 优先级
P2

#### 依赖
T22, T4

#### 验收标准
- [ ] **功能**: SaxSymbolParser 正确解析 symbol 元素
- [ ] **功能**: SAX 模式解析结果与 DOM 模式一致
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 4h
- 最可能: 6h
- 悲观: 10h
- 期望: 6.33h

#### 里程碑
M3

#### 状态
📋 Todo

---

### T24 - ParserFactory与模式选择

#### 描述
- 实现 ogc::portrayal::parser::ParserFactory 工厂类
- 定义 ParseMode 枚举（DOM, SAX）
- CreateDomReader/CreateSaxReader 静态方法
- Parse 便捷方法根据文件大小自动选择模式
- 头文件: include/ogc/portrayal/parser/parser_factory.h
- 源文件: src/portrayal/parser/parser_factory.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §17

#### 优先级
P2

#### 依赖
T22

#### 验收标准
- [ ] **功能**: DOM/SAX 模式正确切换
- [ ] **功能**: 自动模式根据文件大小选择策略
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M3

#### 状态
📋 Todo

---

### T25 - StringPool与ObjectPool

#### 描述
- 实现 ogc::portrayal::utils::StringPool 类
  - Intern 方法自动去重
  - Contains, Size, MemoryUsage, Reserve, Clear
- 实现 ogc::portrayal::utils::ObjectPool<T> 模板类
  - Acquire/Release 方法
  - 线程安全（mutex 保护）
- 头文件: include/ogc/portrayal/utils/string_pool.h, object_pool.h
- 源文件: src/portrayal/utils/string_pool.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §18

#### 优先级
P2

#### 依赖
T7

#### 验收标准
- [ ] **功能**: StringPool Intern 去重正确
- [ ] **功能**: ObjectPool Acquire/Release 正确复用
- [ ] **性能**: 字符串池内存节省 ≥50%
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 7h
- 期望: 4.17h

#### 里程碑
M3

#### 状态
📋 Todo

---

### T26 - ThreadSafePortrayalCatalog

#### 描述
- 实现 ogc::portrayal::model::ThreadSafePortrayalCatalog 包装器
- 读操作使用 shared_mutex 共享锁
- 写操作使用独占锁
- 实现 Find*, Get*, Add*, Count 方法
- 头文件: include/ogc/portrayal/model/thread_safe_portrayal_catalog.h
- 源文件: src/portrayal/model/thread_safe_portrayal_catalog.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §19

#### 优先级
P2

#### 依赖
T6

#### 验收标准
- [ ] **功能**: 多线程并发读安全
- [ ] **功能**: 写操作独占锁保护
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 7h
- 期望: 4.17h

#### 里程碑
M4

#### 状态
📋 Todo

---

### T27 - CatalogCache与CachedParser

#### 描述
- 实现 ogc::portrayal::cache::CatalogCache 类
  - Get/Put/Invalidate/InvalidateAll
  - LRU 淘汰策略
  - 基于文件修改时间的自动失效
  - 内存限制和统计
- 实现 ogc::portrayal::parser::CachedPortrayalCatalogParser
- 头文件: include/ogc/portrayal/cache/catalog_cache.h, include/ogc/portrayal/parser/cached_portrayal_catalog_parser.h
- 源文件: src/portrayal/cache/catalog_cache.cpp, src/portrayal/parser/cached_portrayal_catalog_parser.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §20

#### 优先级
P2

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 缓存命中时跳过解析
- [ ] **功能**: 文件修改后自动失效
- [ ] **功能**: LRU 淘汰超过内存限制的条目
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 7h
- 期望: 4.17h

#### 里程碑
M4

#### 状态
📋 Todo

---

### T28 - VersionDetector与VersionAdapter

#### 描述
- 实现 ogc::portrayal::parser::VersionDetector 类
  - Detect 静态方法读取 productId 和 version
  - IsSupported 检查版本支持
- 定义 ogc::portrayal::parser::VersionAdapter 接口
- 实现 S401_V1_0_to_V1_1_Adapter 示例
- 头文件: include/ogc/portrayal/parser/version_detector.h, version_adapter.h
- 源文件: src/portrayal/parser/version_detector.cpp, version_adapter.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §24

#### 优先级
P3

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 正确检测 XML 文件版本
- [ ] **功能**: 版本适配器接口定义
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h

#### 里程碑
M4

#### 状态
📋 Todo

---

### T29 - PlatformUtils跨平台工具

#### 描述
- 实现 ogc::portrayal::utils::PlatformUtils 静态工具类
- 方法: NormalizePath, GetCurrentEncoding, IsAbsolutePath, GetExecutableDirectory
- 头文件: include/ogc/portrayal/utils/platform_utils.h
- 源文件: src/portrayal/utils/platform_utils.cpp

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §24

#### 优先级
P3

#### 依赖
T8

#### 验收标准
- [ ] **功能**: NormalizePath 统一路径分隔符
- [ ] **功能**: 跨平台编译通过
- [ ] **覆盖**: 行覆盖 ≥80%

#### 预估工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h

#### 里程碑
M4

#### 状态
📋 Todo

---

### T30 - 单元测试与集成测试

#### 描述
- 为所有模型类编写单元测试（Description, FileReference, Symbol, PortrayalCatalog 等）
- 为所有解析器编写单元测试（SymbolParser, LineStyleParser 等）
- 为 PortrayalCatalogParser 编写集成测试
- 使用真实 portrayal_catalogue.xml 进行端到端测试
- 测试文件: tests/portrayal/ 目录下
- 修改 tests/CMakeLists.txt 添加 portrayal 测试

#### 参考文档
- **设计文档**: [s401_Portrayal_Catalogue_parse_design.md](s401_Portrayal_Catalogue_parse_design.md) §12

#### 优先级
P0: 关键阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有模型类测试通过
- [ ] **功能**: 所有解析器测试通过
- [ ] **功能**: 端到端解析 portrayal_catalogue.xml 成功
- [ ] **功能**: 编码测试（ISO-8859-1/UTF-8）通过
- [ ] **功能**: 错误恢复测试通过
- [ ] **覆盖**: 总体行覆盖 ≥80%, 分支覆盖 ≥70%

#### 预估工时 (PERT)
- 乐观: 10h
- 最可能: 16h
- 悲观: 24h
- 期望: 16.33h
- 复杂度: 高

#### 里程碑
M4

#### 状态
📋 Todo

---

## 变更日志
| 版本 | 日期 | 变更 | 影响 |
|------|------|------|------|
| v1.0 | 2026-05-06 | 初始任务计划 | - |
