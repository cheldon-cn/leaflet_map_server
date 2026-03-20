# 图形绘制任务规划 v2.0

**版本**: 2.0  
**日期**: 2026年3月20日  
**基于设计文档**: graph_draw_design_glm.md v1.2

---

## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 26 |
| Total Estimated Hours | 约480小时 (PERT expected) |
| Critical Path Duration | 约320小时 |
| Target Completion | 12周 (3个月) |
| Team Size | 2-3 developers |

## 技术约束

- **编程语言**: C++11
- **核心框架**: 无外部框架依赖，纯C++11标准库
- **代码规范**: Google C++ Style
- **现代C++11特性**: 智能指针、移动语义、RAII、原子操作、强类型枚举
- **外部依赖**: geom, feature, layer模块

## 依赖关系

```
geom (几何类库)
    ↓
feature (要素模型)
    ↓
layer (图层模型)
    ↓
graph (图形绘制) - 依赖 geom, feature, layer
```

---

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和错误码定义 | P0 | M1 | 6h | 📋 Todo | - |
| T1.2 | Color颜色类 | P0 | M1 | 12h | 📋 Todo | T1.1 |
| T1.3 | Font字体类 | P1 | M1 | 8h | 📋 Todo | T1.1 |
| T1.4 | Transform变换矩阵 | P0 | M1 | 16h | 📋 Todo | T1.2 |
| T2.1 | GraphContext绘图上下文 | P0 | M2 | 20h | 📋 Todo | T1.4 |
| T2.2 | GraphDevice设备抽象 | P0 | M2 | 16h | 📋 Todo | T2.1 |
| T2.3 | GraphPath路径类 | P0 | M2 | 24h | 📋 Todo | T2.1 |
| T3.1 | GraphRenderer渲染器 | P0 | M3 | 24h | 📋 Todo | T2.3 |
| T3.2 | GeometryRenderer几何渲染器 | P0 | M3 | 20h | 📋 Todo | T3.1 |
| T3.3 | FeatureRenderer要素渲染器 | P0 | M3 | 16h | 📋 Todo | T3.2 |
| T3.4 | LayerRenderer图层渲染器 | P0 | M3 | 16h | 📋 Todo | T3.3 |
| T4.1 | Symbol符号系统 | P0 | M4 | 24h | 📋 Todo | T3.1 |
| T4.2 | PointSymbol点符号 | P1 | M4 | 12h | 📋 Todo | T4.1 |
| T4.3 | LineSymbol线符号 | P1 | M4 | 12h | 📋 Todo | T4.1 |
| T4.4 | FillSymbol填充符号 | P1 | M4 | 12h | 📋 Todo | T4.1 |
| T4.5 | TextSymbol文本符号 | P1 | M4 | 16h | 📋 Todo | T4.1 |
| T5.1 | LabelEngine标注引擎 | P1 | M5 | 24h | 📋 Todo | T4.5 |
| T5.2 | LabelPlacement标注放置 | P1 | M5 | 16h | 📋 Todo | T5.1 |
| T5.3 | LabelConflict标注冲突 | P1 | M5 | 12h | 📋 Todo | T5.1 |
| T6.1 | GraphExporter导出器 | P1 | M5 | 20h | 📋 Todo | T3.4 |
| T6.2 | ImageExporter图像导出 | P1 | M5 | 16h | 📋 Todo | T6.1 |
| T6.3 | PDFExporter PDF导出 | P2 | M5 | 20h | 📋 Todo | T6.1 |
| T7.1 | 单元测试完善 | P0 | M6 | 32h | 📋 Todo | T5.1 |
| T7.2 | 性能测试与优化 | P1 | M6 | 24h | 📋 Todo | T7.1 |

---

## Critical Path

```
T1.1 → T1.2 → T1.4 → T2.1 → T2.3 → T3.1 → T3.4 → T7.1
(6h + 12h + 16h + 20h + 24h + 24h + 16h + 32h = 150h 核心路径)
```

⚠️ **关键路径任务延迟将直接影响项目交付**

---

## Milestones

### M1: 基础设施 (Week 1-2)

#### T1.1 - 枚举和错误码定义

**Description**
定义图形绘制模块使用的所有枚举类型，包括GraphType、RenderMode、SymbolType等。

**Priority**: P0

**Dependencies**: 无

**Acceptance Criteria**
- [ ] **Functional**: GraphType枚举（Point/Line/Polygon/Text/Image等）
- [ ] **Functional**: RenderMode枚举（Normal/Selected/Highlight等）
- [ ] **Functional**: SymbolType枚举（Simple/Character/Picture等）
- [ ] **Functional**: GraphResult错误码
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

#### T1.2 - Color颜色类

**Description**
实现颜色类，支持RGB/RGBA/HSV等颜色模型。

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: RGB/RGBA构造
- [ ] **Functional**: HSV/HSL转换
- [ ] **Functional**: 颜色混合
- [ ] **Functional**: 预定义颜色常量
- [ ] **Functional**: 颜色解析（#RRGGBB格式）
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

---

#### T1.3 - Font字体类

**Description**
实现字体类，支持字体属性设置。

**Priority**: P1

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: 字体名称、大小、样式
- [ ] **Functional**: 粗体、斜体、下划线
- [ ] **Functional**: 字体度量
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 1

---

#### T1.4 - Transform变换矩阵

**Description**
实现2D变换矩阵，支持平移、旋转、缩放。

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 平移变换
- [ ] **Functional**: 旋转变换
- [ ] **Functional**: 缩放变换
- [ ] **Functional**: 组合变换
- [ ] **Functional**: 矩阵求逆
- [ ] **Functional**: 点变换
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

### M2: 绘图核心 (Week 3-4)

#### T2.1 - GraphContext绘图上下文

**Description**
实现绘图上下文，管理绘图状态和属性。

**Priority**: P0

**Dependencies**: T1.4

**Acceptance Criteria**
- [ ] **Functional**: 绘图状态管理
- [ ] **Functional**: 变换矩阵栈
- [ ] **Functional**: 裁剪区域
- [ ] **Functional**: 绘图属性（颜色、线宽、填充等）
- [ ] **Functional**: Save/Restore状态
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

#### T2.2 - GraphDevice设备抽象

**Description**
实现绘图设备抽象接口，支持多种输出设备。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 设备初始化
- [ ] **Functional**: 绘图表面
- [ ] **Functional**: 设备能力查询
- [ ] **Functional**: 像素格式支持
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

#### T2.3 - GraphPath路径类

**Description**
实现路径类，支持复杂图形绘制。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: MoveTo/LineTo路径命令
- [ ] **Functional**: CurveTo/ArcTo曲线命令
- [ ] **Functional**: ClosePath闭合路径
- [ ] **Functional**: 路径填充规则
- [ ] **Functional**: 路径变换
- [ ] **Functional**: 路径布尔运算
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

### M3: 渲染器 (Week 5-7)

#### T3.1 - GraphRenderer渲染器

**Description**
实现基础渲染器，提供渲染框架。

**Priority**: P0

**Dependencies**: T2.3

**Acceptance Criteria**
- [ ] **Functional**: 渲染管线
- [ ] **Functional**: 渲染状态管理
- [ ] **Functional**: 渲染优化
- [ ] **Functional**: 批量渲染
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

#### T3.2 - GeometryRenderer几何渲染器

**Description**
实现几何对象渲染器。

**Priority**: P0

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: Point渲染
- [ ] **Functional**: LineString渲染
- [ ] **Functional**: Polygon渲染
- [ ] **Functional**: 集合类型渲染
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

#### T3.3 - FeatureRenderer要素渲染器

**Description**
实现要素渲染器。

**Priority**: P0

**Dependencies**: T3.2

**Acceptance Criteria**
- [ ] **Functional**: Feature渲染
- [ ] **Functional**: 属性驱动样式
- [ ] **Functional**: 选择高亮
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

#### T3.4 - LayerRenderer图层渲染器

**Description**
实现图层渲染器。

**Priority**: P0

**Dependencies**: T3.3

**Acceptance Criteria**
- [ ] **Functional**: Layer渲染
- [ ] **Functional**: 图层顺序
- [ ] **Functional**: 可见性控制
- [ ] **Functional**: 比例尺控制
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

### M4: 符号系统 (Week 8-9)

#### T4.1 - Symbol符号系统

**Description**
实现符号系统基础架构。

**Priority**: P0

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: Symbol抽象基类
- [ ] **Functional**: 符号属性管理
- [ ] **Functional**: 符号序列化
- [ ] **Functional**: 符号库管理
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

#### T4.2 - PointSymbol点符号

**Description**
实现点符号类型。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 简单点符号（圆形、方形、三角形等）
- [ ] **Functional**: 字符符号
- [ ] **Functional**: 图片符号
- [ ] **Functional**: 符号旋转和缩放
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

---

#### T4.3 - LineSymbol线符号

**Description**
实现线符号类型。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 简单线符号
- [ ] **Functional**: 虚线模式
- [ ] **Functional**: 箭头符号
- [ ] **Functional**: 线端点和连接样式
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

---

#### T4.4 - FillSymbol填充符号

**Description**
实现填充符号类型。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 简单填充
- [ ] **Functional**: 图案填充
- [ ] **Functional**: 渐变填充
- [ ] **Functional**: 图片填充
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

---

#### T4.5 - TextSymbol文本符号

**Description**
实现文本符号类型。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 文本样式
- [ ] **Functional**: 文本对齐
- [ ] **Functional**: 文本效果（阴影、描边等）
- [ ] **Functional**: 文本背景
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

### M5: 标注与导出 (Week 10-11)

#### T5.1 - LabelEngine标注引擎

**Description**
实现标注引擎，管理标注生成和放置。

**Priority**: P1

**Dependencies**: T4.5

**Acceptance Criteria**
- [ ] **Functional**: 标注生成
- [ ] **Functional**: 标注优先级
- [ ] **Functional**: 标注过滤
- [ ] **Performance**: 标注性能优化
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

#### T5.2 - LabelPlacement标注放置

**Description**
实现标注放置算法。

**Priority**: P1

**Dependencies**: T5.1

**Acceptance Criteria**
- [ ] **Functional**: 点标注放置
- [ ] **Functional**: 线标注放置（沿线标注）
- [ ] **Functional**: 面标注放置
- [ ] **Functional**: 放置位置优化
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

#### T5.3 - LabelConflict标注冲突

**Description**
实现标注冲突检测和解决。

**Priority**: P1

**Dependencies**: T5.1

**Acceptance Criteria**
- [ ] **Functional**: 冲突检测
- [ ] **Functional**: 冲突解决策略
- [ ] **Functional**: 四叉树索引优化
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

---

#### T6.1 - GraphExporter导出器

**Description**
实现图形导出器基础架构。

**Priority**: P1

**Dependencies**: T3.4

**Acceptance Criteria**
- [ ] **Functional**: 导出器抽象接口
- [ ] **Functional**: 导出配置
- [ ] **Functional**: 导出进度
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

#### T6.2 - ImageExporter图像导出

**Description**
实现图像格式导出。

**Priority**: P1

**Dependencies**: T6.1

**Acceptance Criteria**
- [ ] **Functional**: PNG导出
- [ ] **Functional**: JPEG导出
- [ ] **Functional**: BMP导出
- [ ] **Functional**: 分辨率和质量设置
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

#### T6.3 - PDFExporter PDF导出

**Description**
实现PDF格式导出。

**Priority**: P2

**Dependencies**: T6.1

**Acceptance Criteria**
- [ ] **Functional**: PDF页面设置
- [ ] **Functional**: 矢量图形导出
- [ ] **Functional**: 字体嵌入
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Low
- Complexity: High
- Story Points: 4

---

### M6: 测试与优化 (Week 12)

#### T7.1 - 单元测试完善

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

#### T7.2 - 性能测试与优化

**Description**
进行性能测试和优化。

**Priority**: P1

**Dependencies**: T7.1

**Acceptance Criteria**
- [ ] **Performance**: 渲染性能达标
- [ ] **Performance**: 大数据量渲染优化
- [ ] **Performance**: 内存使用优化
- [ ] **Documentation**: 性能测试报告

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Medium
- Complexity: Medium
- Story Points: 5

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| 渲染性能不达标 | Medium | High | 实现LOD和分块渲染 | Tech Lead |
| 标注冲突算法复杂 | Medium | Medium | 参考成熟算法实现 | Dev Team |
| PDF导出兼容性 | Low | Medium | 使用标准PDF规范 | Dev Team |
| 跨平台渲染差异 | Low | Medium | 使用平台无关的渲染接口 | Dev Team |

---

## Change Log

| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v2.0 | 2026-03-20 | 基于graph_draw_design_glm.md v1.2重新规划 | 完整重构 |
| v1.0 | 2026-03-18 | 初始版本 | - |
