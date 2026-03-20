# 数据库存储模型任务规划 v2.0

**版本**: 2.0  
**日期**: 2026年3月20日  
**基于设计文档**: database_model_design.md v1.3

---

## Overview

| 项目 | 数值 |
|------|------|
| Total Tasks | 24 |
| Total Estimated Hours | 约540小时 (PERT expected) |
| Critical Path Duration | 约360小时 |
| Target Completion | 14周 (3.5个月) |
| Team Size | 2-3 developers |

## 技术约束

- **编程语言**: C++11
- **核心框架**: 无外部框架依赖，纯C++11标准库
- **代码规范**: Google C++ Style
- **现代C++11特性**: 智能指针、移动语义、RAII、原子操作、强类型枚举
- **外部依赖**: libpq (PostgreSQL), sqlite3, libspatialite

## 依赖关系

```
geom (几何类库) - 已完成基础类型
    ↓
database (数据库存储) - 依赖 geom
    ↓
layer (图层模型) - 依赖 database
```

---

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1.1 | 枚举和错误码定义 | P0 | M1 | 6h | 📋 Todo | - |
| T1.2 | DbResult结果类 | P0 | M1 | 8h | 📋 Todo | T1.1 |
| T1.3 | DbConnection抽象接口 | P0 | M1 | 16h | 📋 Todo | T1.2 |
| T1.4 | DbStatement预编译语句接口 | P0 | M1 | 12h | 📋 Todo | T1.3 |
| T1.5 | DbResultSet结果集接口 | P0 | M1 | 12h | 📋 Todo | T1.3 |
| T2.1 | WkbConverter转换器 | P0 | M2 | 24h | 📋 Todo | T1.3 |
| T2.2 | PostGIS连接实现 | P0 | M2 | 32h | 📋 Todo | T2.1 |
| T2.3 | PostGIS批量插入 | P1 | M2 | 20h | 📋 Todo | T2.2 |
| T2.4 | PostGIS空间查询 | P0 | M2 | 24h | 📋 Todo | T2.2 |
| T3.1 | SpatiaLite连接实现 | P0 | M3 | 28h | 📋 Todo | T2.1 |
| T3.2 | SpatiaLite空间索引 | P1 | M3 | 16h | 📋 Todo | T3.1 |
| T3.3 | SpatiaLite事务支持 | P1 | M3 | 12h | 📋 Todo | T3.1 |
| T4.1 | DbConnectionPool连接池 | P0 | M4 | 24h | 📋 Todo | T2.2 |
| T4.2 | 连接池健康检查 | P1 | M4 | 12h | 📋 Todo | T4.1 |
| T4.3 | 连接池耗尽处理 | P1 | M4 | 8h | 📋 Todo | T4.1 |
| T5.1 | 空间索引管理 | P1 | M4 | 20h | 📋 Todo | T2.4 |
| T5.2 | SRID验证与转换 | P1 | M4 | 16h | 📋 Todo | T5.1 |
| T6.1 | GeoJSON序列化 | P1 | M5 | 16h | 📋 Todo | T2.1 |
| T6.2 | 异步API设计 | P2 | M5 | 24h | 📋 Todo | T4.1 |
| T7.1 | 单元测试完善 | P0 | M5 | 32h | 📋 Todo | T5.1 |
| T7.2 | 集成测试 | P0 | M5 | 24h | 📋 Todo | T7.1 |
| T7.3 | 性能测试 | P1 | M5 | 16h | 📋 Todo | T7.2 |

---

## Critical Path

```
T1.1 → T1.3 → T2.1 → T2.2 → T2.4 → T4.1 → T7.1
(6h + 16h + 24h + 32h + 24h + 24h + 32h = 158h 核心路径)
```

⚠️ **关键路径任务延迟将直接影响项目交付**

---

## Milestones

### M1: 基础设施 (Week 1-2)

#### T1.1 - 枚举和错误码定义

**Description**
定义数据库模块使用的所有枚举类型，包括DatabaseType、SpatialOperator、DbResult、ColumnType等。

**Priority**: P0

**Dependencies**: 无

**Acceptance Criteria**
- [ ] **Functional**: DatabaseType枚举（PostGIS/SpatiaLite等）
- [ ] **Functional**: SpatialOperator枚举（Intersects/Contains/Within等9种操作）
- [ ] **Functional**: DbResult错误码（连接/SQL/几何/事务/资源错误）
- [ ] **Functional**: ColumnType枚举
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

#### T1.2 - DbResult结果类

**Description**
实现数据库操作结果类，支持错误信息和上下文。

**Priority**: P0

**Dependencies**: T1.1

**Acceptance Criteria**
- [ ] **Functional**: DbResult类封装操作结果
- [ ] **Functional**: 支持错误码和错误消息
- [ ] **Functional**: 支持上下文信息
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

#### T1.3 - DbConnection抽象接口

**Description**
定义数据库连接抽象接口，支持连接管理、事务管理、SQL执行、几何对象操作。

**Priority**: P0

**Dependencies**: T1.2

**Acceptance Criteria**
- [ ] **Functional**: Connect/Disconnect连接管理
- [ ] **Functional**: BeginTransaction/Commit/Rollback事务管理
- [ ] **Functional**: Execute/ExecuteQuery SQL执行
- [ ] **Functional**: InsertGeometry/SelectGeometries几何操作
- [ ] **Functional**: SpatialQuery空间查询
- [ ] **Functional**: CreateSpatialTable/CreateSpatialIndex元数据操作
- [ ] **Thread-Safe**: 所有方法线程安全
- [ ] **Coverage**: 接口设计文档完整

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

---

#### T1.4 - DbStatement预编译语句接口

**Description**
定义预编译语句接口，支持参数绑定和执行。

**Priority**: P0

**Dependencies**: T1.3

**Acceptance Criteria**
- [ ] **Functional**: Prepare预编译
- [ ] **Functional**: BindInt/BindDouble/BindString参数绑定
- [ ] **Functional**: BindGeometry几何绑定
- [ ] **Functional**: Execute/ExecuteQuery执行
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

#### T1.5 - DbResultSet结果集接口

**Description**
定义查询结果集接口，支持数据读取和遍历。

**Priority**: P0

**Dependencies**: T1.3

**Acceptance Criteria**
- [ ] **Functional**: Next/IsEOF遍历
- [ ] **Functional**: GetInt/GetDouble/GetString数据读取
- [ ] **Functional**: GetGeometry几何读取
- [ ] **Functional**: GetColumnCount/GetColumnName元数据
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

### M2: PostGIS适配器 (Week 3-5)

#### T2.1 - WkbConverter转换器

**Description**
实现WKB/EWKB格式转换器，支持几何对象与二进制格式互转。

**Priority**: P0

**Dependencies**: T1.3

**Acceptance Criteria**
- [ ] **Functional**: GeometryToWkb/WkbToGeometry转换
- [ ] **Functional**: GeometryToHexWkb/HexWkbToGeometry十六进制
- [ ] **Functional**: IsEwkb/ExtractSrid EWKB支持
- [ ] **Functional**: 支持大端/小端字节序
- [ ] **Functional**: 支持Z/M维度
- [ ] **Performance**: SIMD优化路径（可选）
- [ ] **Coverage**: 单元测试覆盖率 ≥95%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Medium
- Complexity: High
- Story Points: 5

---

#### T2.2 - PostGIS连接实现

**Description**
实现PostGIS数据库连接，基于libpq。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 连接字符串解析
- [ ] **Functional**: 连接/断开/重连
- [ ] **Functional**: Ping健康检查
- [ ] **Functional**: 事务管理
- [ ] **Functional**: 预编译语句支持
- [ ] **Functional**: 错误处理和重连机制
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 24h
- Most Likely (M): 32h
- Pessimistic (P): 48h
- Expected: 32h
- Confidence: Medium
- Complexity: High
- Story Points: 6

---

#### T2.3 - PostGIS批量插入

**Description**
实现PostGIS批量插入，使用COPY命令优化。

**Priority**: P1

**Dependencies**: T2.2

**Acceptance Criteria**
- [ ] **Functional**: COPY命令批量插入
- [ ] **Functional**: 错误恢复机制
- [ ] **Functional**: 进度回调支持
- [ ] **Performance**: 批量插入性能达标
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

#### T2.4 - PostGIS空间查询

**Description**
实现PostGIS空间查询，支持所有空间操作符。

**Priority**: P0

**Dependencies**: T2.2

**Acceptance Criteria**
- [ ] **Functional**: Intersects/Contains/Within等9种操作
- [ ] **Functional**: DWithin距离查询
- [ ] **Functional**: 空间索引使用（GiST）
- [ ] **Functional**: 查询计划分析
- [ ] **Performance**: 空间查询性能达标
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

### M3: SpatiaLite适配器 (Week 6-8)

#### T3.1 - SpatiaLite连接实现

**Description**
实现SpatiaLite数据库连接，基于sqlite3和libspatialite。

**Priority**: P0

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: 文件数据库连接
- [ ] **Functional**: 内存数据库支持
- [ ] **Functional**: 连接/断开
- [ ] **Functional**: 事务管理
- [ ] **Functional**: 预编译语句支持
- [ ] **Coverage**: 单元测试覆盖率 ≥90%

**Estimated Effort (PERT Method)**
- Optimistic (O): 20h
- Most Likely (M): 28h
- Pessimistic (P): 40h
- Expected: 28h
- Confidence: Medium
- Complexity: High
- Story Points: 5

---

#### T3.2 - SpatiaLite空间索引

**Description**
实现SpatiaLite R-Tree空间索引管理。

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: 创建R-Tree索引
- [ ] **Functional**: 索引查询优化
- [ ] **Functional**: 索引更新
- [ ] **Performance**: 索引查询性能达标
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

#### T3.3 - SpatiaLite事务支持

**Description**
实现SpatiaLite事务支持，包括MVCC和隔离级别。

**Priority**: P1

**Dependencies**: T3.1

**Acceptance Criteria**
- [ ] **Functional**: 事务开始/提交/回滚
- [ ] **Functional**: 事务隔离级别
- [ ] **Functional**: 嵌套事务（SAVEPOINT）
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

### M4: 连接池与高级特性 (Week 9-11)

#### T4.1 - DbConnectionPool连接池

**Description**
实现数据库连接池，支持连接复用和管理。

**Priority**: P0

**Dependencies**: T2.2

**Acceptance Criteria**
- [ ] **Functional**: 连接获取/释放
- [ ] **Functional**: 最小/最大连接数配置
- [ ] **Functional**: 连接超时处理
- [ ] **Functional**: 连接预热
- [ ] **Thread-Safe**: 线程安全
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

#### T4.2 - 连接池健康检查

**Description**
实现连接池健康检查机制。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 定时健康检查
- [ ] **Functional**: 无效连接清理
- [ ] **Functional**: 连接有效性验证
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 8h
- Most Likely (M): 12h
- Pessimistic (P): 16h
- Expected: 12h
- Confidence: High
- Complexity: Medium
- Story Points: 2

---

#### T4.3 - 连接池耗尽处理

**Description**
实现连接池耗尽时的处理策略。

**Priority**: P1

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 等待策略
- [ ] **Functional**: 超时处理
- [ ] **Functional**: 拒绝策略
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 6h
- Most Likely (M): 8h
- Pessimistic (P): 12h
- Expected: 8h
- Confidence: High
- Complexity: Low
- Story Points: 1

---

#### T5.1 - 空间索引管理

**Description**
实现空间索引的创建、管理和优化。

**Priority**: P1

**Dependencies**: T2.4

**Acceptance Criteria**
- [ ] **Functional**: 创建空间索引
- [ ] **Functional**: 索引统计信息
- [ ] **Functional**: 索引重建
- [ ] **Functional**: 索引删除
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

#### T5.2 - SRID验证与转换

**Description**
实现SRID验证和坐标转换流水线。

**Priority**: P1

**Dependencies**: T5.1

**Acceptance Criteria**
- [ ] **Functional**: SRID验证
- [ ] **Functional**: 坐标转换流水线
- [ ] **Functional**: 严格验证选项
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

### M5: 高级特性与测试 (Week 12-14)

#### T6.1 - GeoJSON序列化

**Description**
实现GeoJSON格式的序列化和反序列化。

**Priority**: P1

**Dependencies**: T2.1

**Acceptance Criteria**
- [ ] **Functional**: Geometry转GeoJSON
- [ ] **Functional**: GeoJSON转Geometry
- [ ] **Functional**: Feature支持
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

#### T6.2 - 异步API设计

**Description**
实现异步数据库操作API。

**Priority**: P2

**Dependencies**: T4.1

**Acceptance Criteria**
- [ ] **Functional**: 异步查询接口
- [ ] **Functional**: 回调机制
- [ ] **Functional**: 取消操作
- [ ] **Coverage**: 单元测试覆盖率 ≥85%

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Low
- Complexity: High
- Story Points: 5

---

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

#### T7.2 - 集成测试

**Description**
进行数据库集成测试。

**Priority**: P0

**Dependencies**: T7.1

**Acceptance Criteria**
- [ ] **Functional**: PostGIS集成测试通过
- [ ] **Functional**: SpatiaLite集成测试通过
- [ ] **Functional**: 性能基准测试

**Estimated Effort (PERT Method)**
- Optimistic (O): 16h
- Most Likely (M): 24h
- Pessimistic (P): 36h
- Expected: 24h
- Confidence: Medium
- Complexity: Medium
- Story Points: 5

---

#### T7.3 - 性能测试

**Description**
进行性能测试和优化。

**Priority**: P1

**Dependencies**: T7.2

**Acceptance Criteria**
- [ ] **Performance**: 批量插入性能达标
- [ ] **Performance**: 空间查询性能达标
- [ ] **Documentation**: 性能测试报告

**Estimated Effort (PERT Method)**
- Optimistic (O): 12h
- Most Likely (M): 16h
- Pessimistic (P): 24h
- Expected: 16h
- Confidence: Medium
- Complexity: Medium
- Story Points: 3

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| libpq/libspatialite版本兼容性 | Medium | Medium | 明确最低版本要求 | Dev Team |
| 空间索引性能不达标 | Medium | High | 提前进行性能测试 | Tech Lead |
| 连接池并发问题 | Low | High | 充分的并发测试 | Dev Team |
| 事务隔离级别问题 | Medium | Medium | 详细测试隔离级别 | Dev Team |

---

## Change Log

| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v2.0 | 2026-03-20 | 基于database_model_design.md v1.3重新规划 | 完整重构 |
| v1.0 | 2026-03-18 | 初始版本 | - |
