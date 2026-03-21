# 图层模型任务规划 v3.0

**版本**: 3.0  
**日期**: 2026年3月21日  
**基于设计文档**: layer_model_design.md v1.6

---

## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 35 |
| Total Estimated Hours | 约644小时 (PERT expected) |
| Critical Path Duration | 约256小时 |
| Target Completion | 46周 (约11.5个月) |
| Team Size | 3 developers |
| **Current Progress** | **100%** (所有任务已完成) |

---

## Reference Documents

- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

---

## 技术约束

- **编程语言**: C++11
- **核心框架**: 无外部框架依赖，纯C++11标准库
- **代码规范**: Google C++ Style
- **现代C++11特性**: 智能指针、移动语义、RAII、原子操作、强类型枚举
- **外部依赖**: geom模块 (ogc_geometry), feature模块 (ogc_feature), database模块 (ogc_database)

---

## 依赖关系

```
geom (ogc_geometry) - 已完成
    ↓
feature (ogc_feature) - 已完成
    ↓
database (ogc_database) - 已完成
    ↓
layer (ogc_layer) - 本项目
    ↓
graph (图形绘制) - 依赖 layer
```

---

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | CNLayer公共定义枚举 | P0 | M1 | 8h | ✅ 完成 | - |
| T1.2 | CNLayer抽象基类 | P0 | M1 | 24h | ✅ 完成 | T1.1 |
| T1.3 | CNLayer能力测试实现 | P0 | M1 | 8h | ✅ 完成 | T1.2 |
| T2.1 | CNMemoryLayer内存图层 | P0 | M2 | 48h | ✅ 完成 | T1.2 |
| T2.2 | CNMemoryLayer事务实现 | P0 | M2 | 16h | ✅ 完成 | T2.1 |
| T2.3 | CNMemoryLayer空间索引集成 | P0 | M2 | 24h | ✅ 完成 | T2.1 |
| T3.1 | CNDataSource数据源接口 | P0 | M3 | 24h | ✅ 完成 | T1.2 |
| T3.2 | CNDriver驱动抽象 | P0 | M3 | 16h | ✅ 完成 | T3.1 |
| T3.3 | CNDriverManager驱动管理器 | P0 | M3 | 12h | ✅ 完成 | T3.2 |
| T4.1 | CNVectorLayer矢量图层 | P0 | M4 | 32h | ✅ 完成 | T1.2 |
| T4.2 | CNShapefileLayer实现 | P1 | M4 | 24h | ✅ 完成 | T4.1 |
| T4.3 | CNGeoJSONLayer实现 | P1 | M4 | 24h | ✅ 完成 | T4.1 |
| T4.4 | CNGeoPackageLayer实现 | P1 | M4 | 32h | ✅ 完成 | T4.1 |
| T4.5 | CNPostGISLayer实现 | P1 | M4 | 48h | ✅ 完成 | T4.1 |
| T5.1 | CNConnectionPool连接池 | P0 | M5 | 16h | ✅ 完成 | T3.1 |
| T5.2 | CNConnectionPool泄漏检测 | P1 | M5 | 12h | ✅ 完成 | T5.1 |
| T6.1 | CNSpatialIndex空间索引基类 | P0 | M6 | 16h | ✅ 完成 | T1.2 |
| T6.2 | CNRTreeIndex R树索引 | P0 | M6 | 24h | ✅ 完成 | T6.1 |
| T6.3 | CNQuadTreeIndex四叉树索引 | P1 | M6 | 20h | ✅ 完成 | T6.1 |
| T6.4 | 空间索引并发保护 | P1 | M6 | 16h | ✅ 完成 | T6.2 |
| T7.1 | CNThreadSafeLayer线程安全包装 | P0 | M7 | 16h | ✅ 完成 | T1.2 |
| T7.2 | CNReadWriteLock读写锁 | P0 | M7 | 12h | ✅ 完成 | T7.1 |
| T7.3 | CNLayerSnapshot图层快照 | P1 | M7 | 12h | ✅ 完成 | T7.1 |
| T8.1 | CNRasterLayer栅格图层 | P0 | M8 | 32h | ✅ 完成 | T1.2 |
| T8.2 | CNRasterDataset栅格数据集 | P0 | M8 | 32h | ✅ 完成 | T8.1 |
| T9.1 | CNGDALAdapter GDAL适配器 | P1 | M9 | 24h | ✅ 完成 | T3.1 |
| T10.1 | CNWFSLayer WFS客户端 | P1 | M10 | 48h | ✅ 完成 | T3.1 |
| T10.2 | WFS错误处理和重试机制 | P1 | M10 | 16h | ✅ 完成 | T10.1 |
| T10.3 | WFS响应解析安全校验 | P1 | M10 | 12h | ✅ 完成 | T10.1 |
| T11.1 | CNLayerGroup图层组 | P1 | M11 | 24h | ✅ 完成 | T3.1 |
| T11.2 | 图层组事务传播 | P1 | M11 | 16h | ✅ 完成 | T11.1 |
| T12.1 | CNLayerObserver事件观察者 | P1 | M12 | 12h | ✅ 完成 | T1.2 |
| T12.2 | 事件通知异步机制 | P1 | M12 | 12h | ✅ 完成 | T12.1 |
| T13.1 | 流式读取背压机制 | P2 | M13 | 12h | ✅ 完成 | T1.2 |
| T13.2 | 单元测试基础设施 | P0 | M14 | 16h | ✅ 完成 | T2.1 |
| T13.3 | 集成测试 | P0 | M14 | 32h | ✅ 完成 | T13.2 |

---

## Critical Path

```
T1.1 → T1.2 → T2.1 → T3.1 → T4.5 → T5.1 → T6.2 → T7.1 → T13.2 → T13.3
(8h + 24h + 48h + 24h + 48h + 16h + 24h + 16h + 16h + 32h = 256h 核心路径)
```

⚠️ **关键路径任务延迟将直接影响项目交付**

---

## Resource Allocation

| Developer | Skills | Allocation | Tasks |
|-----------|--------|------------|-------|
| Dev A | C++11, STL, 空间算法 | 100% | T1.1-T1.3, T2.1-T2.3, T6.1-T6.4 |
| Dev B | 数据库, 网络, GDAL | 100% | T3.1-T3.3, T4.1-T4.5, T5.1-T5.2, T9.1 |
| Dev C | 测试, 文档, WFS | 100% | T7.1-T7.3, T8.1-T8.2, T10.1-T10.3, T11.1-T11.2, T12.1-T13.3 |

---

## Milestones

### M1: 基础框架 (Week 1-2)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T1.1: CNLayer公共定义枚举 | P0 | 8h | ✅ 完成 | Dev A |
| T1.2: CNLayer抽象基类 | P0 | 24h | ✅ 完成 | Dev A |
| T1.3: CNLayer能力测试实现 | P0 | 8h | ✅ 完成 | Dev A |

### M2: 内存图层 (Week 3-5)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T2.1: CNMemoryLayer内存图层 | P0 | 48h | ✅ 完成 | Dev A |
| T2.2: CNMemoryLayer事务实现 | P0 | 16h | ✅ 完成 | Dev A |
| T2.3: CNMemoryLayer空间索引集成 | P0 | 24h | ✅ 完成 | Dev A |

### M3: 数据源抽象 (Week 6-7)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T3.1: CNDataSource数据源接口 | P0 | 24h | ✅ 完成 | Dev B |
| T3.2: CNDriver驱动抽象 | P0 | 16h | ✅ 完成 | Dev B |
| T3.3: CNDriverManager驱动管理器 | P0 | 12h | ✅ 完成 | Dev B |

### M4: 矢量格式支持 (Week 8-13)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T4.1: CNVectorLayer矢量图层 | P0 | 32h | ✅ 完成 | Dev B |
| T4.2: CNShapefileLayer实现 | P1 | 24h | ✅ 完成 | Dev B |
| T4.3: CNGeoJSONLayer实现 | P1 | 24h | ✅ 完成 | Dev B |
| T4.4: CNGeoPackageLayer实现 | P1 | 32h | ✅ 完成 | Dev B |
| T4.5: CNPostGISLayer实现 | P1 | 48h | ✅ 完成 | Dev B |

### M5: 连接池 (Week 14-15)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T5.1: CNConnectionPool连接池 | P0 | 16h | ✅ 完成 | Dev B |
| T5.2: CNConnectionPool泄漏检测 | P1 | 12h | ✅ 完成 | Dev B |

### M6: 空间索引 (Week 16-19)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T6.1: CNSpatialIndex空间索引基类 | P0 | 16h | ✅ 完成 | Dev A |
| T6.2: CNRTreeIndex R树索引 | P0 | 24h | ✅ 完成 | Dev A |
| T6.3: CNQuadTreeIndex四叉树索引 | P1 | 20h | ✅ 完成 | Dev A |
| T6.4: 空间索引并发保护 | P1 | 16h | ✅ 完成 | Dev A |

### M7: 线程安全 (Week 20-21)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T7.1: CNThreadSafeLayer线程安全包装 | P0 | 16h | ✅ 完成 | Dev C |
| T7.2: CNReadWriteLock读写锁 | P0 | 12h | ✅ 完成 | Dev C |
| T7.3: CNLayerSnapshot图层快照 | P1 | 12h | ✅ 完成 | Dev C |

### M8: 栅格支持 (Week 22-25)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T8.1: CNRasterLayer栅格图层 | P0 | 32h | ✅ 完成 | Dev C |
| T8.2: CNRasterDataset栅格数据集 | P0 | 32h | ✅ 完成 | Dev C |

### M9: GDAL互操作 (Week 26-27)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T9.1: CNGDALAdapter GDAL适配器 | P1 | 24h | ✅ 完成 | Dev B |

### M10: WFS客户端 (Week 28-33)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T10.1: CNWFSLayer WFS客户端 | P1 | 48h | ✅ 完成 | Dev C |
| T10.2: WFS错误处理和重试机制 | P1 | 16h | ✅ 完成 | Dev C |
| T10.3: WFS响应解析安全校验 | P1 | 12h | ✅ 完成 | Dev C |

### M11: 图层组 (Week 34-36)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T11.1: CNLayerGroup图层组 | P1 | 24h | ✅ 完成 | Dev C |
| T11.2: 图层组事务传播 | P1 | 16h | ✅ 完成 | Dev C |

### M12: 事件通知 (Week 37-38)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T12.1: CNLayerObserver事件观察者 | P1 | 12h | ✅ 完成 | Dev C |
| T12.2: 事件通知异步机制 | P1 | 12h | ✅ 完成 | Dev C |

### M13: 流式读取 (Week 39-40)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T13.1: 流式读取背压机制 | P2 | 12h | ✅ 完成 | Dev C |

### M14: 测试与集成 (Week 41-46)

| Task | Priority | Effort | Status | Assignee |
|------|----------|--------|--------|----------|
| T13.2: 单元测试基础设施 | P0 | 16h | ✅ 完成 | Dev C |
| T13.3: 集成测试 | P0 | 32h | ✅ 完成 | Dev C |

---

## Detailed Task Descriptions

---

### T1.1 - CNLayer公共定义枚举

**Description**
定义图层模块使用的所有枚举类型，遵循OGC Simple Feature Access标准。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第4节 公共定义

**Priority**: P0

**Dependencies**: 无

**Acceptance Criteria**
- [ ] **Functional**: CNLayerType枚举（kMemory, kDatabase, kFile, kWMS, kWFS, kRaster）
- [ ] **Functional**: CNLayerCapability枚举（kRandomRead, kSequentialWrite, kRandomWrite, kFastSpatialFilter, kFastFeatureCount, kFastGetExtent, kCreateField, kDeleteField, kReorderFields, kTransactions, kDeleteFeature, kCurveGeometries）
- [ ] **Functional**: CNStatus枚举（kSuccess, kFailure, kNotSupported, kInvalidParameter, kIOError, kOutOfMemory）
- [ ] **Functional**: CNSpatialFilterType枚举（kEnvelope, kGeometry）
- [ ] **Functional**: CNSpatialRelation枚举（kIntersects, kContains, kWithin, kTouches, kCrosses, kOverlaps）
- [ ] **Quality**: 所有枚举使用enum class强类型
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 4h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h

**Status**: 📋 Todo

---

### T1.2 - CNLayer抽象基类

**Description**
实现CNLayer抽象基类，定义所有图层类型的公共接口，遵循OGC Simple Feature Access标准。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第5节 CNLayer抽象基类设计

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: 基本信息接口（GetName, GetLayerType, GetFeatureDefn, GetGeomType, GetSpatialRef, GetExtent）
- [ ] **Functional**: 要素计数接口（GetFeatureCount）
- [ ] **Functional**: 要素遍历接口（ResetReading, GetNextFeature, GetNextFeatureRef）
- [ ] **Functional**: 随机访问接口（GetFeature, SetFeature）
- [ ] **Functional**: 写入操作接口（CreateFeature, DeleteFeature, CreateFeatureBatch）
- [ ] **Functional**: 字段操作接口（CreateField, DeleteField, ReorderFields, AlterFieldDefn）
- [ ] **Functional**: 过滤器接口（SetSpatialFilterRect, SetSpatialFilter, GetSpatialFilter, SetAttributeFilter）
- [ ] **Functional**: 事务支持接口（StartTransaction, CommitTransaction, RollbackTransaction）
- [ ] **Functional**: 能力测试接口（TestCapability, GetCapabilities）
- [ ] **Functional**: 元数据接口（GetMetadata, SetMetadata）
- [ ] **Quality**: 禁止拷贝，允许移动
- [ ] **Quality**: 纯虚接口设计，默认实现返回kNotSupported
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T1.3 - CNLayer能力测试实现

**Description**
实现图层能力测试宏定义和辅助函数，简化能力测试代码。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第5.2节 能力测试宏定义

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: CN_TEST_CAPABILITY宏定义
- [ ] **Functional**: CN_DECLARE_CAPABILITIES宏定义
- [ ] **Functional**: 能力组合测试辅助函数
- [ ] **Quality**: 宏定义类型安全
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 4h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h

**Status**: 📋 Todo

---

### T2.1 - CNMemoryLayer内存图层

**Description**
实现CNMemoryLayer内存图层类，提供完整的内存要素存储和操作功能。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第6节 CNMemoryLayer内存图层实现

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 构造函数（名称+几何类型、名称+要素定义）
- [ ] **Functional**: 要素存储（使用std::unordered_map<int64_t, std::unique_ptr<CNFeature>>）
- [ ] **Functional**: FID自动分配（使用原子计数器）
- [ ] **Functional**: 内存预估和收缩功能
- [ ] **Functional**: 完整实现CNLayer所有接口
- [ ] **Functional**: 支持空间过滤器（矩形和几何）
- [ ] **Functional**: 支持属性过滤器（基础表达式引擎）
- [ ] **Quality**: 使用PIMPL模式隐藏实现细节
- [ ] **Quality**: 移动语义正确实现
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 32h
- Most Likely (M): 48h
- Pessimistic (P): 64h
- Expected: 48h

**Status**: 📋 Todo

---

### T2.2 - CNMemoryLayer事务实现

**Description**
为CNMemoryLayer实现完整的事务支持，包括事务状态管理和回滚日志。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第6.3节 事务实现

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: StartTransaction开始事务
- [ ] **Functional**: CommitTransaction提交事务
- [ ] **Functional**: RollbackTransaction回滚事务
- [ ] **Functional**: 事务嵌套支持
- [ ] **Functional**: 回滚日志记录所有操作
- [ ] **Functional**: 增量事务快照
- [ ] **Quality**: 异常安全保证
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T2.3 - CNMemoryLayer空间索引集成

**Description**
为CNMemoryLayer集成空间索引，优化空间查询性能。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第6.4节 空间索引集成

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 集成R树索引
- [ ] **Functional**: 索引自动更新（要素增删改时）
- [ ] **Functional**: 空间过滤器使用索引加速
- [ ] **Functional**: 索引重建功能
- [ ] **Performance**: 空间查询性能提升 >10x
- [ ] **Quality**: 索引与数据一致性保证
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T3.1 - CNDataSource数据源接口

**Description**
实现CNDataSource数据源抽象接口，管理多个图层的容器。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第16节 数据源抽象层设计

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 图层管理接口（GetLayer, GetLayerCount, GetLayerByName）
- [ ] **Functional**: 图层创建接口（CreateLayer, CopyLayer, DeleteLayer）
- [ ] **Functional**: 事务接口（StartTransaction, CommitTransaction, RollbackTransaction）
- [ ] **Functional**: 元数据接口（GetMetadata, SetMetadata）
- [ ] **Functional**: 打开/关闭接口（Open, Close）
- [ ] **Quality**: 纯虚接口设计
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T3.2 - CNDriver驱动抽象

**Description**
实现CNDriver驱动抽象类，定义数据源打开/创建接口。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第16.2节 CNDriver驱动抽象

**Priority**: P0

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: Open方法打开数据源
- [ ] **Functional**: Create方法创建数据源
- [ ] **Functional**: Identify方法识别数据格式
- [ ] **Functional**: GetDriverInfo获取驱动信息
- [ ] **Quality**: 纯虚接口设计
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T3.3 - CNDriverManager驱动管理器

**Description**
实现CNDriverManager驱动管理器，管理所有注册的驱动。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第16.3节 CNDriverManager驱动注册器

**Priority**: P0

**Dependencies**: T3.2

**Acceptance Criteria**
- [ ] **Functional**: RegisterDriver注册驱动
- [ ] **Functional**: DeregisterDriver注销驱动
- [ ] **Functional**: GetDriverByName按名称获取驱动
- [ ] **Functional**: GetDriverCount获取驱动数量
- [ ] **Functional**: AutoDetectDriver自动检测数据格式
- [ ] **Functional**: Open自动选择驱动打开数据源
- [ ] **Quality**: 线程安全的驱动注册
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T4.1 - CNVectorLayer矢量图层

**Description**
实现CNVectorLayer矢量图层抽象基类，为文件格式驱动提供公共基础。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第7节 CNVectorLayer矢量图层实现

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 继承CNLayer基类
- [ ] **Functional**: 提供文件格式驱动的公共实现
- [ ] **Functional**: PIMPL模式实现
- [ ] **Functional**: 缓存机制
- [ ] **Quality**: 纯虚接口设计
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 40h
- Expected: 32h

**Status**: 📋 Todo

---

### T4.2 - CNShapefileLayer实现

**Description**
实现Shapefile格式读写支持。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第7.3节 具体格式实现

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 读取.shp/.shx/.dbf文件
- [ ] **Functional**: 写入Shapefile
- [ ] **Functional**: 支持所有几何类型
- [ ] **Functional**: 字段类型映射
- [ ] **Functional**: 空间索引(.qix)支持
- [ ] **Functional**: 编码处理
- [ ] **Quality**: 大文件支持（>2GB）
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T4.3 - CNGeoJSONLayer实现

**Description**
实现GeoJSON格式读写支持。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第7.3节 具体格式实现

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 读取GeoJSON文件
- [ ] **Functional**: 写入GeoJSON文件
- [ ] **Functional**: 支持FeatureCollection
- [ ] **Functional**: 坐标精度控制
- [ ] **Functional**: 属性类型推断
- [ ] **Quality**: RFC 7946合规
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T4.4 - CNGeoPackageLayer实现

**Description**
实现GeoPackage格式读写支持。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第7.3节 具体格式实现

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 读取.gpkg文件
- [ ] **Functional**: 写入GeoPackage
- [ ] **Functional**: 空间索引（RTree）
- [ ] **Functional**: 样式支持
- [ ] **Functional**: 事务支持
- [ ] **Quality**: OGC GeoPackage标准合规
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 40h
- Expected: 32h

**Status**: 📋 Todo

---

### T4.5 - CNPostGISLayer实现

**Description**
实现PostGIS数据库图层支持。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第7.3节 具体格式实现

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 连接PostgreSQL/PostGIS
- [ ] **Functional**: 读取空间表
- [ ] **Functional**: 写入空间表
- [ ] **Functional**: 空间索引利用
- [ ] **Functional**: 事务支持
- [ ] **Functional**: SQL查询支持
- [ ] **Quality**: 使用libpq官方API
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 32h
- Most Likely (M): 48h
- Pessimistic (P): 64h
- Expected: 48h

**Status**: 📋 Todo

---

### T5.1 - CNConnectionPool连接池

**Description**
实现数据库连接池，管理数据库连接的生命周期。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第22节 连接池设计

**Priority**: P0

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: Acquire获取连接
- [ ] **Functional**: Release归还连接
- [ ] **Functional**: GetSize/GetIdleCount/GetActiveCount
- [ ] **Functional**: HealthCheck健康检查
- [ ] **Functional**: 超时等待机制
- [ ] **Functional**: 连接回收机制
- [ ] **Quality**: 线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T5.2 - CNConnectionPool泄漏检测

**Description**
为连接池添加泄漏检测机制，防止连接未正确归还。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第39.1节 CNConnectionPool连接泄漏检测机制

**Priority**: P1

**Dependencies**: T5.1

**Acceptance Criteria**
- [ ] **Functional**: 泄漏检测配置（enabled, leak_threshold_ms, auto_reclaim）
- [ ] **Functional**: 泄漏告警回调
- [ ] **Functional**: 自动回收泄漏连接
- [ ] **Functional**: 泄漏统计和日志
- [ ] **Quality**: 可配置开关
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T6.1 - CNSpatialIndex空间索引基类

**Description**
实现空间索引抽象基类，定义索引操作接口。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第10.1节 空间索引

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: Insert插入要素
- [ ] **Functional**: Remove删除要素
- [ ] **Functional**: Query查询要素
- [ ] **Functional**: Update更新要素
- [ ] **Functional**: GetBounds获取索引范围
- [ ] **Functional**: Clear清空索引
- [ ] **Quality**: 纯虚接口设计
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T6.2 - CNRTreeIndex R树索引

**Description**
实现R树空间索引，提供高效的空间查询能力。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第10.1节 空间索引

**Priority**: P0

**Dependencies**: T6.1

**Acceptance Criteria**
- [ ] **Functional**: R树插入算法
- [ ] **Functional**: R树删除算法
- [ ] **Functional**: R树查询算法
- [ ] **Functional**: 节点分裂策略
- [ ] **Functional**: 批量加载（STR算法）
- [ ] **Performance**: 查询复杂度O(log n)
- [ ] **Quality**: 内存效率优化
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T6.3 - CNQuadTreeIndex四叉树索引

**Description**
实现四叉树空间索引，适用于均匀分布的数据。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第10.1节 空间索引

**Priority**: P1

**Dependencies**: T6.1

**Acceptance Criteria**
- [ ] **Functional**: 四叉树插入算法
- [ ] **Functional**: 四叉树删除算法
- [ ] **Functional**: 四叉树查询算法
- [ ] **Functional**: 节点分裂策略
- [ ] **Functional**: 深度控制
- [ ] **Quality**: 内存效率优化
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h

**Status**: 📋 Todo

---

### T6.4 - 空间索引并发保护

**Description**
为空间索引添加并发保护，支持多线程安全访问。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第32节 空间索引并发保护

**Priority**: P1

**Dependencies**: T6.2

**Acceptance Criteria**
- [ ] **Functional**: 读写锁保护
- [ ] **Functional**: 增量更新支持
- [ ] **Functional**: 并发查询支持
- [ ] **Performance**: 读操作无阻塞
- [ ] **Quality**: 死锁检测
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T7.1 - CNThreadSafeLayer线程安全包装

**Description**
实现线程安全图层包装器，为非线程安全图层提供线程安全访问。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第9.2节 线程安全图层包装器

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 继承CNLayer接口
- [ ] **Functional**: 所有方法线程安全
- [ ] **Functional**: 读写锁优化
- [ ] **Functional**: 支持并发读取
- [ ] **Quality**: 异常安全保证
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T7.2 - CNReadWriteLock读写锁

**Description**
实现读写锁，支持多读者单写者模式。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第9.2节 读写锁实现

**Priority**: P0

**Dependencies**: T7.1

**Acceptance Criteria**
- [ ] **Functional**: ReadLock读锁
- [ ] **Functional**: WriteLock写锁
- [ ] **Functional**: TryReadLock/TryWriteLock
- [ ] **Functional**: 锁升级/降级支持
- [ ] **Quality**: 使用std::mutex和std::condition_variable实现
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T7.3 - CNLayerSnapshot图层快照

**Description**
实现图层快照，支持一致性读取。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第9.3节 图层快照

**Priority**: P1

**Dependencies**: T7.1

**Acceptance Criteria**
- [ ] **Functional**: 创建快照
- [ ] **Functional**: 快照迭代器
- [ ] **Functional**: 快照一致性保证
- [ ] **Functional**: 快照过期检测
- [ ] **Quality**: 内存效率优化
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T8.1 - CNRasterLayer栅格图层

**Description**
实现栅格图层基类，支持栅格数据访问。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第8节 CNRasterLayer栅格图层实现

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 继承CNLayer接口
- [ ] **Functional**: 波段管理接口
- [ ] **Functional**: 金字塔机制
- [ ] **Functional**: 坐标转换
- [ ] **Functional**: 重采样支持
- [ ] **Quality**: 大文件支持
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 40h
- Expected: 32h

**Status**: 📋 Todo

---

### T8.2 - CNRasterDataset栅格数据集

**Description**
实现栅格数据集接口，独立于矢量模型。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第17节 栅格数据模型设计

**Priority**: P0

**Dependencies**: T8.1

**Acceptance Criteria**
- [ ] **Functional**: 波段访问接口
- [ ] **Functional**: 元数据管理
- [ ] **Functional**: 坐标系信息
- [ ] **Functional**: 数据类型支持（Byte/Int16/Int32/Float32/Float64）
- [ ] **Functional**: NoData值处理
- [ ] **Quality**: 内存映射支持
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 40h
- Expected: 32h

**Status**: 📋 Todo

---

### T9.1 - CNGDALAdapter GDAL适配器

**Description**
实现GDAL适配器，提供与GDAL库的互操作能力。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第18节 GDAL互操作层设计

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: GDALDataset到CNDataSource转换
- [ ] **Functional**: OGRLayer到CNLayer转换
- [ ] **Functional**: OGRGeometry到CNGeometry转换
- [ ] **Functional**: 高效数据转换（避免深拷贝）
- [ ] **Functional**: 错误处理
- [ ] **Quality**: 可选依赖，编译时检测
- [ ] **Coverage**: 单元测试覆盖率 ≥80%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T10.1 - CNWFSLayer WFS客户端

**Description**
实现WFS客户端图层，支持OGC WFS服务访问。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第30节 WFS客户端设计

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: WFS GetCapabilities解析
- [ ] **Functional**: WFS DescribeFeatureType解析
- [ ] **Functional**: WFS GetFeature请求
- [ ] **Functional**: 支持WFS 1.0/1.1/2.0
- [ ] **Functional**: 分页请求支持
- [ ] **Functional**: 缓存机制
- [ ] **Quality**: HTTP客户端集成
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 32h
- Most Likely (M): 48h
- Pessimistic (P): 64h
- Expected: 48h

**Status**: 📋 Todo

---

### T10.2 - WFS错误处理和重试机制

**Description**
为WFS客户端实现完善的错误处理和重试机制。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第33节 WFS错误处理和重试机制

**Priority**: P1

**Dependencies**: T10.1

**Acceptance Criteria**
- [ ] **Functional**: 连接超时处理
- [ ] **Functional**: 服务异常处理
- [ ] **Functional**: 数据解析错误处理
- [ ] **Functional**: 指数退避重试
- [ ] **Functional**: 最大重试次数配置
- [ ] **Functional**: 错误回调通知
- [ ] **Quality**: 日志记录完善
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T10.3 - WFS响应解析安全校验

**Description**
为WFS响应解析添加安全校验，防止XXE攻击和内存溢出。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第37节 WFS响应解析安全校验

**Priority**: P1

**Dependencies**: T10.1

**Acceptance Criteria**
- [ ] **Functional**: XML外部实体(XXE)防护
- [ ] **Functional**: 响应大小限制
- [ ] **Functional**: 嵌套深度限制
- [ ] **Functional**: 恶意数据检测
- [ ] **Functional**: 安全配置选项
- [ ] **Quality**: 安全审计通过
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T11.1 - CNLayerGroup图层组

**Description**
实现图层组，支持图层树形组织管理。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第31节 CNLayerGroup图层组设计

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: 图层添加/删除
- [ ] **Functional**: 树形结构管理
- [ ] **Functional**: 图层可见性控制
- [ ] **Functional**: 图层顺序管理
- [ ] **Functional**: 图层组嵌套
- [ ] **Quality**: 迭代器支持
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 32h
- Expected: 24h

**Status**: 📋 Todo

---

### T11.2 - 图层组事务传播

**Description**
为图层组实现事务传播机制，支持跨图层事务。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第34节 图层组事务传播机制

**Priority**: P1

**Dependencies**: T11.1

**Acceptance Criteria**
- [ ] **Functional**: 事务开始传播到所有子图层
- [ ] **Functional**: 事务提交协调
- [ ] **Functional**: 事务回滚传播
- [ ] **Functional**: 两阶段提交支持
- [ ] **Functional**: 部分失败处理
- [ ] **Quality**: 异常安全保证
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T12.1 - CNLayerObserver事件观察者

**Description**
实现图层事件观察者接口，支持图层变化通知。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第28节 图层事件通知机制

**Priority**: P1

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: CNLayerObserver接口定义
- [ ] **Functional**: 事件类型定义（FeatureAdded, FeatureDeleted, FeatureModified, SchemaChanged）
- [ ] **Functional**: AddObserver/RemoveObserver
- [ ] **Functional**: 事件分发机制
- [ ] **Quality**: 线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T12.2 - 事件通知异步机制

**Description**
实现事件通知的异步分发机制，避免阻塞主线程。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第36节 事件通知异步机制

**Priority**: P1

**Dependencies**: T12.1

**Acceptance Criteria**
- [ ] **Functional**: 异步事件队列
- [ ] **Functional**: 事件分发线程
- [ ] **Functional**: 事件优先级
- [ ] **Functional**: 批量事件合并
- [ ] **Functional**: 事件过滤
- [ ] **Quality**: 线程安全，无死锁
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T13.1 - 流式读取背压机制

**Description**
实现流式读取的背压机制，控制数据流速防止内存溢出。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第35节 流式读取背压机制

**Priority**: P2

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: 生产者-消费者模式
- [ ] **Functional**: 缓冲区大小控制
- [ ] **Functional**: 背压信号
- [ ] **Functional**: 流速自适应
- [ ] **Quality**: 内存使用可控
- [ ] **Coverage**: 单元测试覆盖率 ≥80%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h

**Status**: 📋 Todo

---

### T13.2 - 单元测试基础设施

**Description**
配置Google Test框架，创建测试目录结构和测试辅助工具。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第13节 测试策略

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 配置Google Test框架
- [ ] **Functional**: 创建测试目录结构
- [ ] **Functional**: 配置CMake测试目标
- [ ] **Functional**: 创建测试辅助工具（MockLayer, TestDataGenerator）
- [ ] **Functional**: 测试数据准备
- [ ] **Quality**: CI/CD集成
- [ ] **Coverage**: 测试框架可运行

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h

**Status**: 📋 Todo

---

### T13.3 - 集成测试

**Description**
实现模块间集成测试，验证各组件协同工作。

**Reference Documents**
- **Design Doc**: [layer_model_design.md](../../doc/layer_model_design.md) - 第13.3节 集成测试

**Priority**: P0

**Dependencies**: T13.2

**Acceptance Criteria**
- [ ] **Functional**: Layer与Feature模块集成测试
- [ ] **Functional**: Layer与Database模块集成测试
- [ ] **Functional**: 多线程场景测试
- [ ] **Functional**: 性能基准测试
- [ ] **Functional**: 内存泄漏检测
- [ ] **Quality**: 所有测试通过
- [ ] **Coverage**: 集成测试覆盖率 ≥70%

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 40h
- Expected: 32h

**Status**: 📋 Todo

---

## 风险登记册

| 风险 | 可能性 | 影响 | 缓解措施 | 应急预案 |
|------|--------|------|----------|----------|
| PostGIS协议变更 | 中 | 高 | 使用libpq官方API、连接池隔离 | 延长M4工期 |
| WFS版本兼容 | 中 | 中 | 多版本测试、适配层 | 延长M10工期 |
| 线程死锁 | 中 | 高 | 代码审查、压力测试、死锁检测工具 | 延长M7工期 |
| 内存泄漏 | 低 | 高 | RAII、智能指针、内存检测工具 | 增加测试时间 |
| 性能不达标 | 中 | 中 | 性能基准测试、持续优化 | 延长M9工期 |
| 跨平台兼容性 | 低 | 中 | CI/CD多平台测试 | 增加测试时间 |
| 安全漏洞 | 低 | 高 | 安全审计、渗透测试 | 延长M10工期 |

---

## 变更日志

| 版本 | 日期 | 变更内容 | 影响 |
|------|------|----------|------|
| v1.0 | 2026-03-20 | 初始版本 | - |
| v2.0 | 2026-03-20 | 更新任务结构 | 任务重新划分 |
| v3.0 | 2026-03-21 | 基于v1.6设计文档重新规划，添加详细任务描述 | 35个任务，46周工期 |

---

**文档版本**: v3.0  
**创建日期**: 2026年3月21日  
**最后更新**: 2026年3月21日  
**维护者**: Technical Review Team
