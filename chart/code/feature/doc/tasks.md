# 要素模型任务规划 v3.0

**版本**: 3.0  
**日期**: 2026年3月21日  
**基于设计文档**: feature_model_design.md v1.3

---

## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 28 |
| Completed Tasks | 24 |
| Remaining Tasks | 4 |
| Total Estimated Hours | 约380小时 (PERT expected) |
| Completed Hours | 约308小时 |
| Remaining Hours | 约72小时 |
| Critical Path Duration | 约240小时 |
| Target Completion | 10周 (2.5个月) |
| Team Size | 2-3 developers |
| **Current Progress** | **86%** |

**Note**: M1-M8任务已全部完成，单元测试全部通过(39个)。剩余T9-T10为性能优化和文档任务。

## Reference Documents

- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## 技术约束

- **编程语言**: C++11
- **核心框架**: 无外部框架依赖，纯C++11标准库
- **代码规范**: Google C++ Style
- **现代C++11特性**: 智能指针、移动语义、RAII、原子操作、强类型枚举
- **外部依赖**: geom模块 (ogc_geometry)

## 依赖关系

```
geom (ogc_geometry) - 已完成
    ↓
feature (ogc_feature) - 本项目
    ↓
layer (图层模型) - 依赖 feature
```

---

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | CNFieldType枚举定义 | P0 | M1 | 4h | ✅ Done | - |
| T1.2 | CNDateTime日期时间结构 | P0 | M1 | 8h | ✅ Done | T1.1 |
| T1.3 | CNFieldDefn字段定义接口 | P0 | M1 | 12h | ✅ Done | T1.1 |
| T1.4 | CNGeomFieldDefn几何字段定义 | P0 | M1 | 8h | ✅ Done | T1.1 |
| T2.1 | CNFieldValue字段值容器 | P0 | M2 | 20h | ✅ Done | T1.2, T1.3 |
| T2.2 | CNFieldValue类型转换 | P1 | M2 | 8h | ✅ Done | T2.1 |
| T3.1 | CNFeatureDefn要素定义 | P0 | M3 | 16h | ✅ Done | T1.3, T1.4 |
| T3.2 | CNFeatureDefn序列化 | P1 | M3 | 8h | ✅ Done | T3.1 |
| T4.1 | CNFeature核心要素类 | P0 | M4 | 24h | ✅ Done | T2.1, T3.1 |
| T4.2 | CNFeature字段访问方法 | P0 | M4 | 16h | ✅ Done | T4.1 |
| T4.3 | CNFeature几何操作 | P0 | M4 | 12h | ✅ Done | T4.1 |
| T4.4 | CNFeature克隆与比较 | P1 | M4 | 8h | ✅ Done | T4.1 |
| T5.1 | CNFeatureGuard RAII包装器 | P1 | M5 | 8h | ✅ Done | T4.1 |
| T5.2 | CNBatchProcessor批量处理器 | P1 | M5 | 16h | ✅ Done | T4.1 |
| T5.3 | CNSpatialQuery空间查询构建器 | P1 | M5 | 12h | ✅ Done | T4.1 |
| T6.1 | CNFeatureCollection要素集合 | P0 | M6 | 12h | ✅ Done | T4.1 |
| T6.2 | CNFeatureIterator迭代器 | P1 | M6 | 8h | ✅ Done | T6.1 |
| T7.1 | GeoJSON序列化 | P0 | M7 | 16h | ✅ Done | T4.1 |
| T7.2 | WKB/WKT序列化 | P1 | M7 | 12h | ✅ Done | T4.1 |
| T8.1 | 单元测试基础设施 | P0 | M8 | 8h | ✅ Done | T4.1 |
| T8.2 | CNFieldValue单元测试 | P0 | M8 | 12h | ✅ Done | T8.1, T2.1 |
| T8.3 | CNFeature单元测试 | P0 | M8 | 16h | ✅ Done | T8.1, T4.1 |
| T8.4 | CNFeatureDefn单元测试 | P1 | M8 | 8h | ✅ Done | T8.1, T3.1 |
| T9.1 | 性能基准测试 | P1 | M9 | 12h | 📋 Todo | T8.3 |
| T9.2 | 内存泄漏检测 | P0 | M9 | 8h | 📋 Todo | T8.3 |
| T10.1 | API文档生成 | P2 | M10 | 8h | 📋 Todo | T9.1 |
| T10.2 | 使用示例编写 | P2 | M10 | 8h | 📋 Todo | T10.1 |

---

## Critical Path

```
T1.1 → T1.3 → T2.1 → T3.1 → T4.1 → T4.2 → T6.1 → T7.1 → T8.1 → T8.3 → T9.2
(4h + 12h + 20h + 16h + 24h + 16h + 12h + 16h + 8h + 16h + 8h = 152h 核心路径)
```

⚠️ **关键路径任务延迟将直接影响项目交付**

---

## Resource Allocation

| Developer | Skills | Allocation | Tasks |
|-----------|--------|------------|-------|
| Dev A | C++11, STL | 100% | T1.1-T1.4, T2.1-T2.2, T4.1-T4.4 |
| Dev B | C++11, 几何算法 | 100% | T3.1-T3.2, T5.1-T5.3, T6.1-T6.2 |
| Dev C | 测试, 文档 | 50% | T7.1-T7.2, T8.1-T8.4, T9.1-T9.2, T10.1-T10.2 |

---

## Milestones

### M1: 基础类型定义 (Week 1)

#### T1.1 - CNFieldType枚举定义

**Description**
定义字段类型枚举，遵循OGC Simple Feature Access标准和GDAL/OGR的字段类型定义。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第4节 类型系统设计

**Priority**: P0

**Dependencies**: 无

**Acceptance Criteria**
- [x] **Functional**: 定义所有字段类型 (kInteger, kInteger64, kReal, kString, kDate, kTime, kDateTime, kBinary, kIntegerList, kInteger64List, kRealList, kStringList, kBoolean, kWideString)
- [x] **Functional**: 定义特殊状态类型 (kNull, kUnset, kUnknown)
- [x] **Functional**: 定义CNFieldSubType子类型枚举
- [x] **Functional**: 实现GetFieldTypeName/GetFieldTypeDescription函数
- [x] **Functional**: 实现IsListType/GetListElementType辅助函数
- [x] **Quality**: 使用enum class强类型枚举
- [x] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 2h
- Most Likely (M): 4h
- Pessimistic (P): 6h
- Expected: 4h
- Confidence: High
- Complexity: Low
- Story Points: 1

**Status**: 📋 Todo

---

#### T1.2 - CNDateTime日期时间结构

**Description**
实现日期时间辅助结构，支持日期、时间、日期时间三种类型，遵循ISO 8601标准。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第4.3节 CNDateTime日期时间结构

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: 实现TimeZoneType时区类型枚举 (kUnknown, kLocal, kUTC, kFixedOffset)
- [ ] **Functional**: 实现构造函数和静态工厂方法 (FromISO8601, NowUTC, NowLocal)
- [ ] **Functional**: 实现状态检查方法 (IsValid, IsDateOnly, IsTimeOnly, HasTimeZone, IsEmpty)
- [ ] **Functional**: 实现比较操作符 (==, !=, <, <=, >, >=)
- [ ] **Functional**: 实现转换方法 (ToISO8601, ToWKT, ToTimestamp, FromTimestamp)
- [ ] **Functional**: 实现向后兼容方法 (FromLegacyTZFlag, ToLegacyTZFlag)
- [ ] **Quality**: 遵循ISO 8601标准
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

#### T1.3 - CNFieldDefn字段定义接口

**Description**
实现字段定义接口，描述单个字段的元信息（名称、类型、宽度、精度等）。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第5.1节 CNFieldDefn字段定义接口

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: 实现基本属性接口 (GetName, SetName, GetType, SetType, GetSubType, SetSubType)
- [ ] **Functional**: 实现宽度与精度接口 (GetWidth, SetWidth, GetPrecision, SetPrecision)
- [ ] **Functional**: 实现约束属性接口 (IsNullable, SetNullable, IsUnique, SetUnique)
- [ ] **Functional**: 实现默认值接口 (GetDefaultValue, SetDefaultValue)
- [ ] **Functional**: 实现元数据接口 (GetAlternativeName, SetAlternativeName, GetComment, SetComment)
- [ ] **Functional**: 实现域约束接口 (GetDomainName, SetDomainName)
- [ ] **Functional**: 实现Clone深拷贝方法
- [ ] **Quality**: 使用纯虚接口设计
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

#### T1.4 - CNGeomFieldDefn几何字段定义

**Description**
实现几何字段定义接口，描述几何字段的元信息（几何类型、空间参考等）。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第5.2节 CNGeomFieldDefn几何字段定义接口

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: 实现基本属性接口 (GetName, SetName)
- [ ] **Functional**: 实现几何类型接口 (GetGeomType, SetGeomType)
- [ ] **Functional**: 实现空间参考接口 (GetSpatialRef, SetSpatialRef)
- [ ] **Functional**: 实现约束属性接口 (IsNullable, SetNullable, Is2D, Set2D, IsMeasured, SetMeasured)
- [ ] **Functional**: 实现坐标范围接口 (GetXMin, GetXMax, GetYMin, GetYMax, SetExtent)
- [ ] **Functional**: 实现Clone深拷贝方法
- [ ] **Quality**: 使用纯虚接口设计
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M2: 字段值容器 (Week 2-3)

#### T2.1 - CNFieldValue字段值容器

**Description**
实现类型安全的字段值存储容器，支持所有字段类型。使用类型擦除+小对象优化(SBO)实现多态存储。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第6节 字段值容器设计

**Priority**: P0

**Dependencies**: T1.2, T1.3

**Acceptance Criteria**
- [ ] **Functional**: 实现所有类型的构造函数 (int32_t, int64_t, double, bool, string, CNDateTime, vector, GeometryPtr)
- [ ] **Functional**: 实现拷贝/移动构造和赋值
- [ ] **Functional**: 实现类型检查方法 (GetType, IsNull, IsUnset, IsSet)
- [ ] **Functional**: 实现类型安全的值访问 (GetInteger, GetInteger64, GetReal, GetString, GetDateTime, GetBinary, GetGeometry)
- [ ] **Functional**: 实现值设置方法 (SetInteger, SetInteger64, SetReal, SetString, SetDateTime, SetBinary, SetGeometry, SetNull, Unset)
- [ ] **Functional**: 实现小对象优化(SBO)存储策略
- [ ] **Functional**: 实现内存管理和异常安全
- [ ] **Quality**: 类型安全，无未定义行为
- [ ] **Thread-Safe**: 所有只读方法线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 20h
- Pessimistic (P): 28h
- Expected: 20h
- Confidence: Medium
- Complexity: High
- Story Points: 5

**Status**: 📋 Todo

---

#### T2.2 - CNFieldValue类型转换

**Description**
实现字段值的类型转换方法，支持安全的类型转换。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第6节 字段值容器设计

**Priority**: P1

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 实现数值类型转换 (Integer ↔ Integer64 ↔ Real)
- [ ] **Functional**: 实现字符串解析转换 (String → Integer/Real/DateTime)
- [ ] **Functional**: 实现数值转字符串 (Integer/Real → String)
- [ ] **Functional**: 实现转换失败处理
- [ ] **Quality**: 转换失败返回错误而非抛异常
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M3: 要素定义 (Week 3-4)

#### T3.1 - CNFeatureDefn要素定义

**Description**
实现要素定义类，定义要素的字段结构，包括字段列表、主键定义、几何字段定义等。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第7节 要素定义类设计

**Priority**: P0

**Dependencies**: T1.3, T1.4

**Acceptance Criteria**
- [ ] **Functional**: 实现字段列表管理 (AddFieldDefn, GetFieldDefn, GetFieldCount, GetFieldIndex)
- [ ] **Functional**: 实现几何字段管理 (AddGeomFieldDefn, GetGeomFieldDefn, GetGeomFieldCount)
- [ ] **Functional**: 实现要素定义属性 (GetName, SetName, GetReferenceCount)
- [ ] **Functional**: 实现字段索引查找优化
- [ ] **Functional**: 实现模式验证方法
- [ ] **Functional**: 实现Clone深拷贝方法
- [ ] **Thread-Safe**: 引用计数线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

**Status**: 📋 Todo

---

#### T3.2 - CNFeatureDefn序列化

**Description**
实现要素定义的序列化和反序列化方法。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第7节 要素定义类设计

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: 实现ToJSON方法
- [ ] **Functional**: 实现FromJSON方法
- [ ] **Functional**: 实现ToXML方法
- [ ] **Functional**: 实现FromXML方法
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M4: 核心要素类 (Week 4-6)

#### T4.1 - CNFeature核心要素类

**Description**
实现核心要素类，包含几何对象和属性字段，遵循OGC Simple Feature Access标准。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第8节 核心要素类设计

**Priority**: P0

**Dependencies**: T2.1, T3.1

**Acceptance Criteria**
- [ ] **Functional**: 实现构造函数 (默认构造、模式构造、拷贝构造、移动构造)
- [ ] **Functional**: 实现析构函数和资源释放
- [ ] **Functional**: 实现GetFeatureDefn获取要素定义
- [ ] **Functional**: 实现GetFID/SetFID要素标识
- [ ] **Functional**: 实现字段值存储管理
- [ ] **Functional**: 实现内存优化策略
- [ ] **Thread-Safe**: 所有方法线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Medium
- Complexity: High
- Story Points: 5

**Status**: 📋 Todo

---

#### T4.2 - CNFeature字段访问方法

**Description**
实现要素的字段访问方法，提供类型安全的字段值读写接口。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第8节 核心要素类设计

**Priority**: P0

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现按索引访问 (GetFieldAsInteger, GetFieldAsInteger64, GetFieldAsReal, GetFieldAsString, GetFieldAsDateTime, GetFieldAsBinary)
- [ ] **Functional**: 实现按名称访问 (GetFieldAsIntegerByName, etc.)
- [ ] **Functional**: 实现字段设置方法 (SetFieldInteger, SetFieldInteger64, SetFieldReal, SetFieldString, SetFieldDateTime, SetFieldBinary, SetFieldNull)
- [ ] **Functional**: 实现字段状态检查 (IsFieldSet, IsFieldNull)
- [ ] **Functional**: 实现字段数量获取 (GetFieldCount)
- [ ] **Quality**: 类型安全访问，错误处理完善
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

**Status**: 📋 Todo

---

#### T4.3 - CNFeature几何操作

**Description**
实现要素的几何操作方法。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第8节 核心要素类设计

**Priority**: P0

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现GetGeometry/SetGeometry方法
- [ ] **Functional**: 实现多几何字段支持 (GetGeomFieldCount, GetGeomField, SetGeomField)
- [ ] **Functional**: 实现GetGeometryRef获取几何引用
- [ ] **Functional**: 实现StealGeometry转移几何所有权
- [ ] **Functional**: 实现GetEnvelope获取外包矩形
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

#### T4.4 - CNFeature克隆与比较

**Description**
实现要素的克隆和比较方法。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第8节 核心要素类设计

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现Clone深拷贝方法
- [ ] **Functional**: 实现Equal比较方法
- [ ] **Functional**: 实现GetHashCode哈希方法
- [ ] **Functional**: 实现比较操作符 (==, !=)
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M5: 辅助类 (Week 6-7)

#### T5.1 - CNFeatureGuard RAII包装器

**Description**
实现RAII要素包装器，自动管理要素生命周期。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第9节 辅助类设计

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现构造函数和析构函数
- [ ] **Functional**: 实现get/release/reset方法
- [ ] **Functional**: 实现指针操作符 (->, *)
- [ ] **Functional**: 实现移动语义
- [ ] **Quality**: 异常安全保证
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 2

**Status**: 📋 Todo

---

#### T5.2 - CNBatchProcessor批量处理器

**Description**
实现批量要素处理器，支持批量操作和事务处理。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第9节 辅助类设计

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现批量添加/删除/更新操作
- [ ] **Functional**: 实现批量大小控制
- [ ] **Functional**: 实现进度回调
- [ ] **Functional**: 实现错误处理和回滚
- [ ] **Performance**: 批量操作性能优化
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

**Status**: 📋 Todo

---

#### T5.3 - CNSpatialQuery空间查询构建器

**Description**
实现空间查询构建器，支持构建复杂的空间查询条件。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第9节 辅助类设计

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现空间关系查询 (Intersects, Contains, Within, Touches, Crosses, Overlaps)
- [ ] **Functional**: 实现距离查询 (DistanceWithin)
- [ ] **Functional**: 实现范围查询 (WithinEnvelope)
- [ ] **Functional**: 实现查询条件组合 (AND, OR, NOT)
- [ ] **Functional**: 实现流式API
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M6: 集合与迭代 (Week 7-8)

#### T6.1 - CNFeatureCollection要素集合

**Description**
实现要素集合类，支持要素的存储和管理。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第8节 核心要素类设计

**Priority**: P0

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现Add/Remove要素操作
- [ ] **Functional**: 实现GetFeature/GetFeatureCount访问
- [ ] **Functional**: 实现GetEnvelope获取外包
- [ ] **Functional**: 实现GetFeatureDefn获取模式
- [ ] **Functional**: 实现空间查询支持
- [ ] **Thread-Safe**: 线程安全
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

#### T6.2 - CNFeatureIterator迭代器

**Description**
实现要素迭代器，支持遍历和批量处理。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第8节 核心要素类设计

**Priority**: P1

**Dependencies**: T6.1

**Acceptance Criteria**
- [ ] **Functional**: 实现前向迭代器
- [ ] **Functional**: 实现begin/end方法
- [ ] **Functional**: 实现批量获取
- [ ] **Functional**: 实现条件迭代
- [ ] **Functional**: 支持range-based for循环
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M7: 序列化 (Week 8-9)

#### T7.1 - GeoJSON序列化

**Description**
实现GeoJSON格式的序列化和反序列化。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第14节 使用示例

**Priority**: P0

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现Feature转GeoJSON
- [ ] **Functional**: 实现GeoJSON转Feature
- [ ] **Functional**: 实现FeatureCollection支持
- [ ] **Functional**: 实现坐标精度控制
- [ ] **Functional**: 实现错误处理
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

**Status**: 📋 Todo

---

#### T7.2 - WKB/WKT序列化

**Description**
实现WKB/WKT格式的几何序列化支持。

**Reference Documents**
- **Design Doc**: [feature_model_design.md](../../doc/feature_model_design.md) - 第14节 使用示例

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 实现Geometry转WKB
- [ ] **Functional**: 实现WKB转Geometry
- [ ] **Functional**: 实现Geometry转WKT
- [ ] **Functional**: 实现WKT转Geometry
- [ ] **Functional**: 实现SRID支持
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M8: 测试 (Week 9-10)

#### T8.1 - 单元测试基础设施

**Description**
搭建单元测试基础设施，配置测试框架。

**Priority**: P0

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 配置Google Test框架
- [ ] **Functional**: 创建测试目录结构
- [ ] **Functional**: 配置CMake测试目标
- [ ] **Functional**: 创建测试辅助工具
- [ ] **Quality**: 测试可自动运行

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 2

**Status**: 📋 Todo

---

#### T8.2 - CNFieldValue单元测试

**Description**
完善CNFieldValue的单元测试。

**Priority**: P0

**Dependencies**: T8.1, T2.1

**Acceptance Criteria**
- [ ] **Coverage**: 类型构造测试
- [ ] **Coverage**: 类型转换测试
- [ ] **Coverage**: 边界条件测试
- [ ] **Coverage**: 异常情况测试
- [ ] **Coverage**: 代码覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

#### T8.3 - CNFeature单元测试

**Description**
完善CNFeature的单元测试。

**Priority**: P0

**Dependencies**: T8.1, T4.1

**Acceptance Criteria**
- [ ] **Coverage**: 构造函数测试
- [ ] **Coverage**: 字段访问测试
- [ ] **Coverage**: 几何操作测试
- [ ] **Coverage**: 克隆比较测试
- [ ] **Coverage**: 线程安全测试
- [ ] **Coverage**: 代码覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: High
- Complexity: Medium
- Story Points: 3

**Status**: 📋 Todo

---

#### T8.4 - CNFeatureDefn单元测试

**Description**
完善CNFeatureDefn的单元测试。

**Priority**: P1

**Dependencies**: T8.1, T3.1

**Acceptance Criteria**
- [ ] **Coverage**: 字段管理测试
- [ ] **Coverage**: 几何字段管理测试
- [ ] **Coverage**: 序列化测试
- [ ] **Coverage**: 代码覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 2

**Status**: 📋 Todo

---

### M9: 性能优化 (Week 10)

#### T9.1 - 性能基准测试

**Description**
进行性能基准测试和优化。

**Priority**: P1

**Dependencies**: T8.3

**Acceptance Criteria**
- [ ] **Performance**: 大数据量处理性能达标
- [ ] **Performance**: 空间查询性能达标
- [ ] **Performance**: 内存使用合理
- [ ] **Documentation**: 性能测试报告

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: Medium
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

#### T9.2 - 内存泄漏检测

**Description**
进行内存泄漏检测和修复。

**Priority**: P0

**Dependencies**: T8.3

**Acceptance Criteria**
- [ ] **Quality**: 无内存泄漏
- [ ] **Quality**: Valgrind/ASan检测通过
- [ ] **Documentation**: 内存检测报告

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Medium
- Story Points: 2

**Status**: 📋 Todo

---

### M10: 文档 (Week 10)

#### T10.1 - API文档生成

**Description**
生成API文档。

**Priority**: P2

**Dependencies**: T9.1

**Acceptance Criteria**
- [ ] **Documentation**: Doxygen配置
- [ ] **Documentation**: 所有公共API有文档注释
- [ ] **Documentation**: HTML文档生成

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 2

**Status**: 📋 Todo

---

#### T10.2 - 使用示例编写

**Description**
编写使用示例代码。

**Priority**: P2

**Dependencies**: T10.1

**Acceptance Criteria**
- [ ] **Documentation**: 基本使用示例
- [ ] **Documentation**: 高级用法示例
- [ ] **Documentation**: 最佳实践指南

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 2

**Status**: 📋 Todo

---

## Dependency Graph

```
[T1.1: CNFieldType] ──┬──► [T1.2: CNDateTime] ──┐
                      │                          │
                      ├──► [T1.3: CNFieldDefn] ──┼──► [T2.1: CNFieldValue] ──► [T2.2: 类型转换]
                      │                          │           │
                      └──► [T1.4: CNGeomFieldDefn]           │
                                 │                           │
                                 └──► [T3.1: CNFeatureDefn] ─┴──► [T4.1: CNFeature] ──┬──► [T4.2: 字段访问]
                                                                        │                ├──► [T4.3: 几何操作]
                                                                        │                └──► [T4.4: 克隆比较]
                                                                        │
                    ┌───────────────────────────────────────────┴────────────────────┐
                    │                                                                        │
                    ▼                                                                        ▼
            [T5.1: FeatureGuard]                                                   [T6.1: FeatureCollection]
            [T5.2: BatchProcessor]                                                          │
            [T5.3: SpatialQuery]                                                    [T6.2: Iterator]
                    │                                                                        │
                    └────────────────────────────────────────────────────────────────────┘
                                               │
                                               ▼
                                        [T7.1: GeoJSON]
                                        [T7.2: WKB/WKT]
                                               │
                                               ▼
                                        [T8.1: 测试基础]
                                               │
                    ┌──────────────────────────┼──────────────────────────┐
                    ▼                          ▼                          ▼
            [T8.2: FieldValue测试]    [T8.3: Feature测试]    [T8.4: FeatureDefn测试]
                                               │
                                               ▼
                                        [T9.1: 性能测试]
                                        [T9.2: 内存检测]
                                               │
                                               ▼
                                        [T10.1: API文档]
                                               │
                                               ▼
                                        [T10.2: 使用示例]
```

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| CNFieldValue类型安全 | Medium | High | 使用类型擦除+SBO，充分测试 | Tech Lead |
| 大数据量内存问题 | Medium | High | 实现流式处理和分页 | Dev Team |
| 线程安全复杂度 | Medium | High | 使用读写锁，原子操作 | Dev Team |
| 几何模块依赖 | Low | Medium | 明确接口契约 | Dev Team |
| 性能不达标 | Low | Medium | 早期性能测试 | Dev Team |

---

## Change Log

| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v3.0 | 2026-03-21 | 基于feature_model_design.md v1.3重新规划，细化任务粒度 | 完整重构 |
| v2.0 | 2026-03-20 | 基于v1.2重新规划 | 完整重构 |
| v1.0 | 2026-03-18 | 初始版本 | - |
