# 图层模型任务规划 v2.0

**版本**: 2.0  
**日期**: 2026年3月20日  
**基于设计文档**: layer_model_design.md v1.1

---

## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 20 |
| Total Estimated Hours | 约380小时 (PERT expected) |
| Critical Path Duration | 约260小时 |
| Target Completion | 10周 (2.5个月) |
| Team Size | 2 developers |

## 技术约束

- **编程语言**: C++11
- **核心框架**: 无外部框架依赖，纯C++11标准库
- **代码规范**: Google C++ Style
- **现代C++11特性**: 智能指针、移动语义、RAII、原子操作、强类型枚举
- **外部依赖**: geom, feature, database模块

## 依赖关系

```
geom (几何类库)
    ↓
feature (要素模型)
    ↓
database (数据库存储)
    ↓
layer (图层模型) - 依赖 geom, feature, database
    ↓
graph (图形绘制) - 依赖 layer
```

---

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和错误码定义 | P0 | M1 | 6h | 📋 Todo | - |
| T1.2 | LayerDefinition图层定义 | P0 | M1 | 16h | 📋 Todo | T1.1 |
| T1.3 | LayerMetadata元数据 | P1 | M1 | 12h | 📋 Todo | T1.2 |
| T2.1 | Layer抽象基类 | P0 | M2 | 20h | 📋 Todo | T1.2 |
| T2.2 | MemoryLayer内存图层 | P0 | M2 | 16h | 📋 Todo | T2.1 |
| T2.3 | DatabaseLayer数据库图层 | P0 | M2 | 24h | 📋 Todo | T2.1 |
| T2.4 | FileLayer文件图层 | P1 | M2 | 20h | 📋 Todo | T2.1 |
| T3.1 | LayerCollection图层集合 | P0 | M3 | 16h | 📋 Todo | T2.1 |
| T3.2 | LayerGroup图层组 | P1 | M3 | 12h | 📋 Todo | T3.1 |
| T3.3 | LayerTree图层树 | P1 | M3 | 16h | 📋 Todo | T3.2 |
| T4.1 | LayerRenderer渲染器 | P0 | M4 | 24h | 📋 Todo | T2.1 |
| T4.2 | Symbol符号系统 | P1 | M4 | 20h | 📋 Todo | T4.1 |
| T4.3 | Label标注系统 | P1 | M4 | 16h | 📋 Todo | T4.1 |
| T5.1 | LayerSelection选择管理 | P1 | M5 | 16h | 📋 Todo | T3.1 |
| T5.2 | LayerEdit编辑操作 | P1 | M5 | 20h | 📋 Todo | T5.1 |
| T5.3 | LayerUndo撤销重做 | P2 | M5 | 16h | 📋 Todo | T5.2 |
| T6.1 | 单元测试完善 | P0 | M5 | 32h | 📋 Todo | T5.1 |
| T6.2 | 集成测试 | P0 | M5 | 20h | 📋 Todo | T6.1 |

---

## Critical Path

```
T1.1 → T1.2 → T2.1 → T2.3 → T3.1 → T4.1 → T6.1
(6h + 16h + 20h + 24h + 16h + 24h + 32h = 138h 核心路径)
```

⚠️ **关键路径任务延迟将直接影响项目交付**

---

## Milestones

### M1: 基础设施 (Week 1-2)

#### T1.1 - 枚举和错误码定义

**Description**
定义图层模块使用的所有枚举类型，包括LayerType、LayerState、RenderMode等。

**Priority**: P0

**Dependencies**: 无

**Acceptance Criteria**
- [ ] **Functional**: LayerType枚举（Memory/Database/File/WMS/WFS等）
- [ ] **Functional**: LayerState枚举（Normal/Editing/Loading/Error等）
- [ ] **Functional**: RenderMode枚举（Point/Line/Polygon/Raster等）
- [ ] **Functional**: LayerResult错误码
- [ ] **Quality**: 所有枚举使用enum class
- [ ] **Coverage**: 枚举相关单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 4h
- Most Likely (M): 6h
- Pessimistic (P): 8h
- Expected: 6h
- Confidence: High
- Complexity: Low
- Story Points: 1

---

#### T1.2 - LayerDefinition图层定义

**Description**
实现图层定义类，描述图层的元数据和配置。

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: 图层名称、类型、状态
- [ ] **Functional**: 可见性、可选择、可编辑属性
- [ ] **Functional**: 最小/最大显示比例
- [ ] **Functional**: 坐标系统定义
- [ ] **Functional**: 要素模式引用
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

---

#### T1.3 - LayerMetadata元数据

**Description**
实现图层元数据类，存储图层的描述信息。

**Priority**: P1

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 标题、描述、关键字
- [ ] **Functional**: 创建时间、修改时间
- [ ] **Functional**: 数据源信息
- [ ] **Functional**: 扩展属性
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Low
- Story Points: 2

---

### M2: 核心图层类 (Week 3-5)

#### T2.1 - Layer抽象基类

**Description**
实现图层抽象基类，定义所有图层类型的公共接口。

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: GetFeature/GetFeatureCount要素访问
- [ ] **Functional**: GetEnvelope获取外包
- [ ] **Functional**: Select/Query空间查询
- [ ] **Functional**: AddFeature/UpdateFeature/DeleteFeature编辑操作
- [ ] **Functional**: BeginEdit/CommitEdit/RollbackEdit事务
- [ ] **Functional**: Refresh刷新数据
- [ ] **Thread-Safe**: 所有方法线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Medium
- Complexity: High
- Story Points: 4

---

#### T2.2 - MemoryLayer内存图层

**Description**
实现内存图层，数据存储在内存中。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 内存存储要素
- [ ] **Functional**: 快速读写操作
- [ ] **Functional**: 空间索引支持
- [ ] **Functional**: 序列化/反序列化
- [ ] **Performance**: 读写性能达标
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

---

#### T2.3 - DatabaseLayer数据库图层

**Description**
实现数据库图层，数据存储在数据库中。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 数据库连接管理
- [ ] **Functional**: 分页加载
- [ ] **Functional**: 延迟加载
- [ ] **Functional**: 缓存机制
- [ ] **Functional**: 事务支持
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Medium
- Complexity: High
- Story Points: 5

---

#### T2.4 - FileLayer文件图层

**Description**
实现文件图层，数据存储在文件中。

**Priority**: P1

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 文件读写
- [ ] **Functional**: 格式自动识别
- [ ] **Functional**: 大文件流式处理
- [ ] **Functional**: 增量更新
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Medium
- Complexity: Medium
- Story Points: 4

---

### M3: 图层集合与组织 (Week 6-7)

#### T3.1 - LayerCollection图层集合

**Description**
实现图层集合类，管理多个图层。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: Add/Remove图层操作
- [ ] **Functional**: GetLayerByName/GetLayerByIndex访问
- [ ] **Functional**: 图层顺序管理
- [ ] **Functional**: 图层可见性控制
- [ ] **Functional**: 统一空间查询
- [ ] **Thread-Safe**: 线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

---

#### T3.2 - LayerGroup图层组

**Description**
实现图层组，支持图层分组管理。

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: 图层分组
- [ ] **Functional**: 组级可见性控制
- [ ] **Functional**: 嵌套组支持
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Low
- Story Points: 2

---

#### T3.3 - LayerTree图层树

**Description**
实现图层树结构，支持复杂的图层组织。

**Priority**: P1

**Dependencies**: T3.2

**Acceptance Criteria**
- [ ] **Functional**: 树形结构管理
- [ ] **Functional**: 遍历和搜索
- [ ] **Functional**: 序列化/反序列化
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

---

### M4: 渲染与符号 (Week 8-9)

#### T4.1 - LayerRenderer渲染器

**Description**
实现图层渲染器，支持要素的可视化渲染。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 点/线/面渲染
- [ ] **Functional**: 比例尺控制
- [ ] **Functional**: 渲染优化
- [ ] **Functional**: 自定义渲染器注册
- [ ] **Performance**: 渲染性能达标
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Medium
- Complexity: High
- Story Points: 5

---

#### T4.2 - Symbol符号系统

**Description**
实现符号系统，支持多种符号类型。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 点符号（圆形、方形、图片等）
- [ ] **Functional**: 线符号（实线、虚线、箭头等）
- [ ] **Functional**: 面符号（填充、图案等）
- [ ] **Functional**: 符号库管理
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Medium
- Complexity: Medium
- Story Points: 4

---

#### T4.3 - Label标注系统

**Description**
实现标注系统，支持要素标注显示。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 文本标注
- [ ] **Functional**: 标注位置优化
- [ ] **Functional**: 标注冲突处理
- [ ] **Functional**: 标注样式设置
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

---

### M5: 编辑与测试 (Week 10)

#### T5.1 - LayerSelection选择管理

**Description**
实现图层选择管理，支持要素选择操作。

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: 单选/多选/框选
- [ ] **Functional**: 选择集管理
- [ ] **Functional**: 选择事件通知
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

---

#### T5.2 - LayerEdit编辑操作

**Description**
实现图层编辑操作，支持要素编辑。

**Priority**: P1

**Dependencies**: T5.1

**Acceptance Criteria**
- [ ] **Functional**: 添加/修改/删除要素
- [ ] **Functional**: 几何编辑（移动、旋转、缩放）
- [ ] **Functional**: 属性编辑
- [ ] **Functional**: 编辑事件通知
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Medium
- Complexity: Medium
- Story Points: 4

---

#### T5.3 - LayerUndo撤销重做

**Description**
实现图层撤销重做功能。

**Priority**: P2

**Dependencies**: T5.2

**Acceptance Criteria**
- [ ] **Functional**: 撤销/重做操作
- [ ] **Functional**: 操作历史管理
- [ ] **Functional**: 操作合并
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

---

#### T6.1 - 单元测试完善

**Description**
完善所有模块的单元测试。

**Priority**: P0

**Dependencies**: T5.1

**Acceptance Criteria**
- [ ] **Coverage**: 整体代码覆盖率 ≥90%
- [ ] **Quality**: 所有测试用例通过
- [ ] **Quality**: 无内存泄漏

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 48h
- Expected: 32h
- Confidence: Medium
- Complexity: Medium
- Story Points: 6

---

#### T6.2 - 集成测试

**Description**
进行集成测试。

**Priority**: P0

**Dependencies**: T6.1

**Acceptance Criteria**
- [ ] **Functional**: 与geom模块集成测试
- [ ] **Functional**: 与feature模块集成测试
- [ ] **Functional**: 与database模块集成测试
- [ ] **Performance**: 性能基准测试

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Medium
- Complexity: Medium
- Story Points: 4

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| 大数据量渲染性能 | Medium | High | 实现LOD和分块渲染 | Tech Lead |
| 数据库图层延迟加载复杂度 | Medium | Medium | 详细设计缓存策略 | Dev Team |
| 编辑操作并发问题 | Low | High | 实现乐观锁机制 | Dev Team |
| 符号系统扩展性 | Low | Medium | 设计可扩展的符号接口 | Dev Team |

---

## Change Log

| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v2.0 | 2026-03-20 | 基于layer_model_design.md v1.1重新规划 | 完整重构 |
| v1.0 | 2026-03-18 | 初始版本 | - |
