# ogc_graph 模块拆分任务计划

## 概述

- **总任务数**: 36
- **预计工时**: 85h (PERT期望值)
- **关键路径**: 48h
- **目标完成**: 3周
- **团队规模**: 2-3人

## 参考文档

- **设计文档**: [split_graph.md](split_graph.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

---

## 任务摘要

| 任务ID | 任务名称 | 优先级 | 里程碑 | 工时 | 状态 | 依赖 |
|--------|----------|--------|--------|------|------|------|
| T1 | 创建代码分支和回滚准备 | P0 | M1 | 2h | 📋 Todo | - |
| T2 | 创建ogc_base目录结构 | P0 | M1 | 1h | ✅ Done | T1 |
| T3 | 创建ogc_proj目录结构 | P0 | M1 | 1h | ✅ Done | T1 |
| T4 | 创建ogc_cache目录结构 | P0 | M1 | 1h | ✅ Done | T1 |
| T5 | 创建ogc_symbology目录结构 | P0 | M1 | 1h | ✅ Done | T1 |
| T6 | 迁移ogc_base文件 | P0 | M2 | 2h | ✅ Done | T2 |
| T7 | 配置ogc_base CMake | P0 | M2 | 1h | ✅ Done | T6 |
| T8 | 编写ogc_base单元测试 | P1 | M2 | 2h | ✅ Done | T7 |
| T9 | 迁移ogc_proj文件 | P0 | M2 | 2h | ✅ Done | T3 |
| T10 | 配置ogc_proj CMake | P0 | M2 | 1h | ✅ Done | T9 |
| T11 | 编写ogc_proj单元测试 | P1 | M2 | 2h | ✅ Done | T10 |
| T12 | 迁移ogc_cache瓦片缓存文件 | P0 | M3 | 3h | ✅ Done | T4 |
| T13 | 迁移ogc_cache离线存储文件 | P0 | M3 | 2h | ✅ Done | T4 |
| T14 | 配置ogc_cache CMake | P0 | M3 | 1h | ✅ Done | T12,T13 |
| T15 | 编写ogc_cache单元测试 | P1 | M3 | 3h | ✅ Done | T14 |
| T16 | 迁移ogc_symbology样式解析文件 | P0 | M4 | 2h | ✅ Done | T5 |
| T17 | 迁移ogc_symbology过滤规则文件 | P0 | M4 | 2h | ✅ Done | T5 |
| T18 | 迁移ogc_symbology符号化器文件 | P0 | M4 | 3h | ✅ Done | T5 |
| T19 | 配置ogc_symbology CMake | P0 | M4 | 1h | ✅ Done | T16,T17,T18 |
| T20 | 编写ogc_symbology单元测试 | P1 | M4 | 4h | ✅ Done | T19 |
| T21 | 重组ogc_graph目录结构 | P0 | M5 | 2h | ✅ Done | T6,T9,T12,T16 |
| T22 | 迁移ogc_graph渲染核心文件 | P0 | M5 | 1h | ✅ Done | T21 |
| T23 | 迁移ogc_graph标签引擎文件 | P0 | M5 | 1h | ✅ Done | T21 |
| T24 | 迁移ogc_graph交互处理文件 | P0 | M5 | 2h | ✅ Done | T21 |
| T25 | 迁移ogc_graph图层管理文件 | P0 | M5 | 1h | ✅ Done | T21 |
| T26 | 迁移ogc_graph LOD文件 | P0 | M5 | 1h | ✅ Done | T21 |
| T27 | 迁移ogc_graph工具文件 | P0 | M5 | 1h | ✅ Done | T21 |
| T28 | 更新ogc_graph CMake配置 | P0 | M5 | 2h | ✅ Done | T22-T27 |
| T29 | 迁移clipper.h到ogc_draw | P1 | M5 | 1h | ✅ Done | T21 |
| T30 | 迁移track_recorder.h到ogc_navi | P1 | M5 | 1h | ✅ Done | T21 |
| T31 | 更新所有#include路径 | P0 | M6 | 4h | ✅ Done | T28 |
| T32 | 更新命名空间引用 | P0 | M6 | 2h | ✅ Done | T31 |
| T33 | 循环依赖检测验证 | P0 | M6 | 2h | ✅ Done | T32 |
| T34 | 编译验证所有模块 | P0 | M7 | 4h | ✅ Done | T33 |
| T35 | 运行完整测试套件 | P0 | M7 | 3h | ✅ Done | T34 |
| T36 | 性能基准测试验证 | P1 | M7 | 2h | ✅ Done | T35 |

---

## 资源分配

| 开发者 | 技能 | 分配 | 任务 |
|--------|------|------|------|
| Dev A | C++, CMake, 架构 | 100% | T1-T3, T6-T11, T21, T28, T31-T34 |
| Dev B | C++, 测试, GIS | 100% | T4-T5, T12-T20, T22-T27, T35-T36 |
| Dev C | C++, 文档 | 50% | T29-T30, 协助T31-T32 |

---

## 关键路径

```
T1 → T2 → T6 → T7 → T21 → T28 → T31 → T32 → T33 → T34 → T35
 2h   1h    2h   1h    2h    2h    4h    2h    2h    4h    3h
```

**关键路径总工时**: 21h (串行)

⚠️ **关键任务**: T1, T2, T6, T7, T21, T28, T31, T32, T33, T34, T35

任何关键路径任务的延迟都会影响项目整体进度。

---

## 里程碑

### M1: 准备阶段 (第1天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T1: 创建代码分支和回滚准备 | P0 | 2h | 📋 Todo | Dev A | 0h ⚠️ |
| T2: 创建ogc_base目录结构 | P0 | 1h | ✅ Done | Dev A | 0h ⚠️ |
| T3: 创建ogc_proj目录结构 | P0 | 1h | ✅ Done | Dev A | 1h |
| T4: 创建ogc_cache目录结构 | P0 | 1h | ✅ Done | Dev B | 2h |
| T5: 创建ogc_symbology目录结构 | P0 | 1h | ✅ Done | Dev B | 2h |

### M2: 基础模块迁移 (第2-3天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T6: 迁移ogc_base文件 | P0 | 2h | ✅ Done | Dev A | 0h ⚠️ |
| T7: 配置ogc_base CMake | P0 | 1h | ✅ Done | Dev A | 0h ⚠️ |
| T8: 编写ogc_base单元测试 | P1 | 2h | 📋 Todo | Dev A | 1h |
| T9: 迁移ogc_proj文件 | P0 | 2h | ✅ Done | Dev A | 0h |
| T10: 配置ogc_proj CMake | P0 | 1h | ✅ Done | Dev A | 0h |
| T11: 编写ogc_proj单元测试 | P1 | 2h | 📋 Todo | Dev A | 1h |

### M3: 缓存模块迁移 (第4-5天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T12: 迁移ogc_cache瓦片缓存文件 | P0 | 3h | ✅ Done | Dev B | 1h |
| T13: 迁移ogc_cache离线存储文件 | P0 | 2h | ✅ Done | Dev B | 2h |
| T14: 配置ogc_cache CMake | P0 | 1h | ✅ Done | Dev B | 0h |
| T15: 编写ogc_cache单元测试 | P1 | 3h | 📋 Todo | Dev B | 1h |

### M4: 符号化模块迁移 (第6-7天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T16: 迁移ogc_symbology样式解析文件 | P0 | 2h | ✅ Done | Dev B | 1h |
| T17: 迁移ogc_symbology过滤规则文件 | P0 | 2h | ✅ Done | Dev B | 1h |
| T18: 迁移ogc_symbology符号化器文件 | P0 | 3h | ✅ Done | Dev B | 0h |
| T19: 配置ogc_symbology CMake | P0 | 1h | ✅ Done | Dev B | 0h |
| T20: 编写ogc_symbology单元测试 | P1 | 4h | 📋 Todo | Dev B | 1h |

### M5: ogc_graph重组 (第8-9天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T21: 重组ogc_graph目录结构 | P0 | 2h | ✅ Done | Dev A | 0h ⚠️ |
| T22: 迁移ogc_graph渲染核心文件 | P0 | 1h | ✅ Done | Dev B | 1h |
| T23: 迁移ogc_graph标签引擎文件 | P0 | 1h | ✅ Done | Dev B | 1h |
| T24: 迁移ogc_graph交互处理文件 | P0 | 2h | ✅ Done | Dev B | 1h |
| T25: 迁移ogc_graph图层管理文件 | P0 | 1h | ✅ Done | Dev B | 1h |
| T26: 迁移ogc_graph LOD文件 | P0 | 1h | ✅ Done | Dev B | 1h |
| T27: 迁移ogc_graph工具文件 | P0 | 1h | ✅ Done | Dev B | 1h |
| T28: 更新ogc_graph CMake配置 | P0 | 2h | ✅ Done | Dev A | 0h ⚠️ |
| T29: 迁移clipper.h到ogc_draw | P1 | 1h | ✅ Done | Dev C | 2h |
| T30: 迁移track_recorder.h到ogc_navi | P1 | 1h | ✅ Done | Dev C | 2h |

### M6: 路径和命名空间更新 (第10-11天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T31: 更新所有#include路径 | P0 | 4h | ✅ Done | Dev A | 0h ⚠️ |
| T32: 更新命名空间引用 | P0 | 2h | ✅ Done | Dev A | 0h ⚠️ |
| T33: 循环依赖检测验证 | P0 | 2h | ✅ Done | Dev A | 0h ⚠️ |

### M7: 验证与测试 (第12-14天)

| 任务 | 优先级 | 工时 | 状态 | 负责人 | 浮动时间 |
|------|--------|------|------|--------|----------|
| T34: 编译验证所有模块 | P0 | 4h | ✅ Done | Dev A | 0h ⚠️ |
| T35: 运行完整测试套件 | P0 | 3h | 📋 Todo | Dev B | 0h ⚠️ |
| T36: 性能基准测试验证 | P1 | 2h | 📋 Todo | Dev B | 1h |

---

## 依赖关系图

```
[T1: 准备]
    │
    ├── [T2: ogc_base目录] ──► [T6: 迁移文件] ──► [T7: CMake] ──┐
    │                                                           │
    ├── [T3: ogc_proj目录] ──► [T9: 迁移文件] ──► [T10: CMake] ──┤
    │                                                           │
    ├── [T4: ogc_cache目录] ──► [T12: 瓦片缓存] ──┐              │
    │                      └──► [T13: 离线存储] ──┼──► [T14] ───┤
    │                                            │              │
    └── [T5: ogc_symbology目录] ──► [T16: 样式] ─┤              │
                                 └──► [T17: 过滤] ┼──► [T19] ───┤
                                 └──► [T18: 符号] ┘              │
                                                                │
                            ┌───────────────────────────────────┘
                            ▼
                    [T21: 重组ogc_graph]
                            │
        ┌───────────────────┼───────────────────┐
        ▼                   ▼                   ▼
    [T22-T27]          [T29: clipper]      [T30: track]
        │                   │                   │
        └───────────────────┼───────────────────┘
                            ▼
                    [T28: 更新CMake]
                            │
                            ▼
                    [T31: 更新#include]
                            │
                            ▼
                    [T32: 更新命名空间]
                            │
                            ▼
                    [T33: 循环依赖检测]
                            │
                            ▼
                    [T34: 编译验证]
                            │
                            ▼
                    [T35: 测试验证]
                            │
                            ▼
                    [T36: 性能测试]
```

---

## 风险登记

| 风险 | 概率 | 影响 | 缓解措施 | 负责人 |
|------|------|------|----------|--------|
| 循环依赖问题 | 中 | 高 | T33专项检测，提前验证 | Dev A |
| #include路径遗漏 | 高 | 中 | 全局搜索，逐模块验证 | Dev A |
| 命名空间冲突 | 低 | 中 | 使用命名空间别名过渡 | Dev A |
| 编译时间增加 | 高 | 低 | 已评估，增量编译改善 | Dev A |
| 测试覆盖率下降 | 中 | 中 | 每模块迁移后立即补充测试 | Dev B |
| 性能下降 | 低 | 中 | T36性能基准测试验证 | Dev B |

---

## 详细任务描述

### T1 - 创建代码分支和回滚准备

#### 描述
- 创建Git开发分支 `feature/module-split`
- 创建主分支标签 `pre-split-v1.0`
- 准备回滚脚本和检查点机制

#### 参考文档
- [split_graph.md](split_graph.md) 7.3 回滚方案

#### 优先级
P0: 关键/阻塞

#### 依赖
无

#### 验收标准
- [ ] **功能**: Git分支创建成功
- [ ] **功能**: 标签 `pre-split-v1.0` 创建成功
- [ ] **功能**: 回滚脚本 `rollback_split.sh` 可执行
- [ ] **文档**: 检查点标签列表文档化

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T2 - 创建ogc_base目录结构

#### 描述
- 创建 `base/` 目录
- 创建子目录 `include/ogc/base/` 和 `src/`
- 创建 `export.h` 导出宏定义文件

#### 参考文档
- [split_graph.md](split_graph.md) 3.1 base

#### 优先级
P0: 关键/阻塞

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 目录结构符合设计文档
- [ ] **功能**: export.h 包含 `OGC_BASE_API` 宏定义
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T3 - 创建ogc_proj目录结构

#### 描述
- 创建 `proj/` 目录
- 创建子目录 `include/ogc/proj/` 和 `src/`
- 创建 `export.h` 导出宏定义文件

#### 参考文档
- [split_graph.md](split_graph.md) 3.2 proj

#### 优先级
P0: 关键/阻塞

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 目录结构符合设计文档
- [ ] **功能**: export.h 包含 `OGC_PROJ_API` 宏定义
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T4 - 创建ogc_cache目录结构

#### 描述
- 创建 `cache/` 目录
- 创建子目录 `include/ogc/cache/tile/` 和 `include/ogc/cache/offline/`
- 创建 `src/tile/` 和 `src/offline/`
- 创建 `export.h` 导出宏定义文件

#### 参考文档
- [split_graph.md](split_graph.md) 3.3 ogc_cache

#### 优先级
P0: 关键/阻塞

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 目录结构符合设计文档
- [ ] **功能**: export.h 包含 `OGC_CACHE_API` 宏定义
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T5 - 创建ogc_symbology目录结构

#### 描述
- 创建 `symbology/` 目录
- 创建子目录 `include/ogc/symbology/style/`、`filter/`、`symbolizer/`
- 创建对应的 `src/` 子目录
- 创建 `export.h` 导出宏定义文件

#### 参考文档
- [split_graph.md](split_graph.md) 3.4 symbology

#### 优先级
P0: 关键/阻塞

#### 依赖
T1

#### 验收标准
- [ ] **功能**: 目录结构符合设计文档
- [ ] **功能**: export.h 包含 `OGC_SYMBOLOGY_API` 宏定义
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T6 - 迁移ogc_base文件

#### 描述
- 迁移 `log.h` 到 `ogc/base/log.h`
- 迁移 `thread_safe.h` 到 `ogc/base/thread_safe.h`
- 迁移 `performance_metrics.h` 到 `ogc/base/performance_metrics.h`
- 迁移 `performance_monitor.h` 到 `ogc/base/performance_monitor.h`
- 迁移对应的 `.cpp` 文件

#### 参考文档
- [split_graph.md](split_graph.md) 4.1 迁移到 ogc_base

#### 优先级
P0: 关键/阻塞

#### 依赖
T2

#### 验收标准
- [ ] **功能**: 所有文件迁移到正确位置
- [ ] **功能**: 更新命名空间为 `ogc::base`
- [ ] **功能**: 更新导出宏为 `OGC_BASE_API`
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T7 - 配置ogc_base CMake

#### 描述
- 创建 `ogc_base/CMakeLists.txt`
- 配置源文件和头文件列表
- 配置依赖关系（无外部依赖）
- 配置导出宏定义

#### 参考文档
- [split_graph.md](split_graph.md) 6.1 ogc_base CMakeLists.txt

#### 优先级
P0: 关键/阻塞

#### 依赖
T6

#### 验收标准
- [ ] **功能**: CMake配置正确
- [ ] **功能**: 模块可独立编译
- [ ] **功能**: 导出宏配置正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T8 - 编写ogc_base单元测试

#### 描述
- 迁移现有测试文件到 `ogc_base/tests/`
- 更新测试中的命名空间引用
- 确保所有测试通过

#### 参考文档
- [split_graph.md](split_graph.md) 5.1 迁移到 ogc_base/tests

#### 优先级
P1: 高

#### 依赖
T7

#### 验收标准
- [ ] **功能**: 所有测试文件迁移完成
- [ ] **功能**: 所有测试通过
- [ ] **覆盖率**: 行覆盖率 ≥80%

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 中 (50-80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T9 - 迁移ogc_proj文件

#### 描述
- 迁移 `coordinate_transform.h` 到 `ogc/proj/coordinate_transform.h`
- 迁移 `coordinate_transformer.h` 到 `ogc/proj/coordinate_transformer.h`
- 迁移 `proj_transformer.h` 到 `ogc/proj/proj_transformer.h`
- 迁移 `coord_system_preset.h` 到 `ogc/proj/coord_system_preset.h`
- 迁移对应的 `.cpp` 文件

#### 参考文档
- [split_graph.md](split_graph.md) 4.2 迁移到 ogc_proj

#### 优先级
P0: 关键/阻塞

#### 依赖
T3

#### 验收标准
- [ ] **功能**: 所有文件迁移到正确位置
- [ ] **功能**: 更新命名空间为 `ogc::proj`
- [ ] **功能**: 更新导出宏为 `OGC_PROJ_API`
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T10 - 配置ogc_proj CMake

#### 描述
- 创建 `ogc_proj/CMakeLists.txt`
- 配置源文件和头文件列表
- 配置依赖关系（ogc_geometry, PROJ）
- 配置导出宏定义

#### 参考文档
- [split_graph.md](split_graph.md) 6.1 ogc_proj CMakeLists.txt

#### 优先级
P0: 关键/阻塞

#### 依赖
T9

#### 验收标准
- [ ] **功能**: CMake配置正确
- [ ] **功能**: 模块可独立编译
- [ ] **功能**: PROJ库链接正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T11 - 编写ogc_proj单元测试

#### 描述
- 迁移现有测试文件到 `ogc_proj/tests/`
- 更新测试中的命名空间引用
- 确保所有测试通过

#### 参考文档
- [split_graph.md](split_graph.md) 5.2 迁移到 ogc_proj/tests

#### 优先级
P1: 高

#### 依赖
T10

#### 验收标准
- [ ] **功能**: 所有测试文件迁移完成
- [ ] **功能**: 所有测试通过
- [ ] **覆盖率**: 行覆盖率 ≥80%

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 中 (50-80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T12 - 迁移ogc_cache瓦片缓存文件

#### 描述
- 迁移瓦片缓存相关文件（6个）到 `ogc/cache/tile/`
- 更新命名空间为 `ogc::cache`
- 更新导出宏为 `OGC_CACHE_API`

#### 参考文档
- [split_graph.md](split_graph.md) 4.3 迁移到 ogc_cache

#### 优先级
P0: 关键/阻塞

#### 依赖
T4

#### 验收标准
- [ ] **功能**: 所有瓦片缓存文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T13 - 迁移ogc_cache离线存储文件

#### 描述
- 迁移离线存储相关文件（3个）到 `ogc/cache/offline/`
- 更新命名空间为 `ogc::cache`
- 更新导出宏为 `OGC_CACHE_API`

#### 参考文档
- [split_graph.md](split_graph.md) 4.3 迁移到 ogc_cache

#### 优先级
P0: 关键/阻塞

#### 依赖
T4

#### 验收标准
- [ ] **功能**: 所有离线存储文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T14 - 配置ogc_cache CMake

#### 描述
- 创建 `ogc_cache/CMakeLists.txt`
- 配置tile和offline子目录
- 配置依赖关系（ogc_geometry, ogc_draw, ogc_layer）

#### 参考文档
- [split_graph.md](split_graph.md) 6.1 ogc_cache CMakeLists.txt

#### 优先级
P0: 关键/阻塞

#### 依赖
T12, T13

#### 验收标准
- [ ] **功能**: CMake配置正确
- [ ] **功能**: 模块可独立编译
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T15 - 编写ogc_cache单元测试

#### 描述
- 迁移现有测试文件到 `ogc_cache/tests/`
- 更新测试中的命名空间引用
- 确保所有测试通过

#### 参考文档
- [split_graph.md](split_graph.md) 5.3 迁移到 ogc_cache/tests

#### 优先级
P1: 高

#### 依赖
T14

#### 验收标准
- [ ] **功能**: 所有测试文件迁移完成
- [ ] **功能**: 所有测试通过
- [ ] **覆盖率**: 行覆盖率 ≥80%

#### 预计工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T16 - 迁移ogc_symbology样式解析文件

#### 描述
- 迁移样式解析文件（4个）到 `ogc/symbology/style/`
- 更新命名空间为 `ogc::symbology`
- 更新导出宏为 `OGC_SYMBOLOGY_API`

#### 参考文档
- [split_graph.md](split_graph.md) 4.4 迁移到 ogc_symbology

#### 优先级
P0: 关键/阻塞

#### 依赖
T5

#### 验收标准
- [ ] **功能**: 所有样式解析文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T17 - 迁移ogc_symbology过滤规则文件

#### 描述
- 迁移过滤规则文件（6个）到 `ogc/symbology/filter/`
- 更新命名空间为 `ogc::symbology`
- 更新导出宏为 `OGC_SYMBOLOGY_API`

#### 参考文档
- [split_graph.md](split_graph.md) 4.4 迁移到 ogc_symbology

#### 优先级
P0: 关键/阻塞

#### 依赖
T5

#### 验收标准
- [ ] **功能**: 所有过滤规则文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T18 - 迁移ogc_symbology符号化器文件

#### 描述
- 迁移符号化器文件（8个）到 `ogc/symbology/symbolizer/`
- 更新命名空间为 `ogc::symbology`
- 更新导出宏为 `OGC_SYMBOLOGY_API`

#### 参考文档
- [split_graph.md](split_graph.md) 4.4 迁移到 ogc_symbology

#### 优先级
P0: 关键/阻塞

#### 依赖
T5

#### 验收标准
- [ ] **功能**: 所有符号化器文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 5h
- 期望: 3.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T19 - 配置ogc_symbology CMake

#### 描述
- 创建 `ogc_symbology/CMakeLists.txt`
- 配置style、filter、symbolizer子目录
- 配置依赖关系（ogc_geometry, ogc_draw, ogc_feature）

#### 参考文档
- [split_graph.md](split_graph.md) 6.1 ogc_symbology CMakeLists.txt

#### 优先级
P0: 关键/阻塞

#### 依赖
T16, T17, T18

#### 验收标准
- [ ] **功能**: CMake配置正确
- [ ] **功能**: 模块可独立编译
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T20 - 编写ogc_symbology单元测试

#### 描述
- 迁移现有测试文件到 `ogc_symbology/tests/`
- 更新测试中的命名空间引用
- 确保所有测试通过

#### 参考文档
- [split_graph.md](split_graph.md) 5.4 迁移到 ogc_symbology/tests

#### 优先级
P1: 高

#### 依赖
T19

#### 验收标准
- [ ] **功能**: 所有测试文件迁移完成
- [ ] **功能**: 所有测试通过
- [ ] **覆盖率**: 行覆盖率 ≥80%

#### 预计工时 (PERT)
- 乐观: 3h
- 最可能: 4h
- 悲观: 6h
- 期望: 4.17h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T21 - 重组ogc_graph目录结构

#### 描述
- 创建新的子目录结构：render/, label/, interaction/, layer/, lod/, util/
- 规划文件迁移路径
- 创建检查点标签 `split-cp6-graph`

#### 参考文档
- [split_graph.md](split_graph.md) 3.5 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T6, T9, T12, T16 (所有模块迁移完成)

#### 验收标准
- [ ] **功能**: 目录结构符合设计文档
- [ ] **功能**: 检查点标签创建成功
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T22 - 迁移ogc_graph渲染核心文件

#### 描述
- 迁移渲染核心文件（6个）到 `ogc/graph/render/`
- 更新命名空间为 `ogc::graph`

#### 参考文档
- [split_graph.md](split_graph.md) 4.7 保留在 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有渲染核心文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T23 - 迁移ogc_graph标签引擎文件

#### 描述
- 迁移标签引擎文件（3个）到 `ogc/graph/label/`
- 更新命名空间为 `ogc::graph`

#### 参考文档
- [split_graph.md](split_graph.md) 4.7 保留在 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有标签引擎文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T24 - 迁移ogc_graph交互处理文件

#### 描述
- 迁移交互处理文件（6个）到 `ogc/graph/interaction/`
- 更新命名空间为 `ogc::graph`

#### 参考文档
- [split_graph.md](split_graph.md) 4.7 保留在 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有交互处理文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T25 - 迁移ogc_graph图层管理文件

#### 描述
- 迁移图层管理文件（2个）到 `ogc/graph/layer/`
- 更新命名空间为 `ogc::graph`

#### 参考文档
- [split_graph.md](split_graph.md) 4.7 保留在 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有图层管理文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T26 - 迁移ogc_graph LOD文件

#### 描述
- 迁移LOD文件（2个）到 `ogc/graph/lod/`
- 更新命名空间为 `ogc::graph`

#### 参考文档
- [split_graph.md](split_graph.md) 4.7 保留在 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有LOD文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T27 - 迁移ogc_graph工具文件

#### 描述
- 迁移工具文件（3个）到 `ogc/graph/util/`
- 更新命名空间为 `ogc::graph`

#### 参考文档
- [split_graph.md](split_graph.md) 4.7 保留在 ogc_graph

#### 优先级
P0: 关键/阻塞

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 所有工具文件迁移完成
- [ ] **功能**: 命名空间更新正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T28 - 更新ogc_graph CMake配置

#### 描述
- 更新 `graph/CMakeLists.txt`
- 配置新的子目录结构
- 更新依赖关系（ogc_symbology, ogc_cache等）

#### 参考文档
- [split_graph.md](split_graph.md) 6.2 更新 graph CMakeLists.txt

#### 优先级
P0: 关键/阻塞

#### 依赖
T22, T23, T24, T25, T26, T27

#### 验收标准
- [ ] **功能**: CMake配置正确
- [ ] **功能**: 模块可独立编译
- [ ] **功能**: 依赖关系正确
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 高 (>80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T29 - 迁移clipper.h到ogc_draw

#### 描述
- 迁移 `clipper.h` 到 `ogc/draw/clipper.h`
- 更新ogc_draw的CMake配置

#### 参考文档
- [split_graph.md](split_graph.md) 4.5 迁移到 ogc_draw

#### 优先级
P1: 高

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 文件迁移完成
- [ ] **功能**: ogc_draw编译通过
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
📋 Todo

---

### T30 - 迁移track_recorder.h到ogc_navi

#### 描述
- 迁移 `track_recorder.h` 到 `ogc/navi/track_recorder.h`
- 更新ogc_navi的CMake配置

#### 参考文档
- [split_graph.md](split_graph.md) 4.6 迁移到 ogc_navi

#### 优先级
P1: 高

#### 依赖
T21

#### 验收标准
- [ ] **功能**: 文件迁移完成
- [ ] **功能**: ogc_navi编译通过
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 0.5h
- 最可能: 1h
- 悲观: 2h
- 期望: 1.08h
- 置信度: 高 (>80%)
- 复杂度: 低

#### 状态
✅ Done

---

### T31 - 更新所有#include路径

#### 描述
- 全局搜索并替换所有 `#include <ogc/draw/xxx.h>`
- 按照映射表更新到新路径
- 验证所有文件编译通过

#### 参考文档
- [split_graph.md](split_graph.md) 8.1 头文件路径变更

#### 优先级
P0: 关键/阻塞

#### 依赖
T28

#### 验收标准
- [ ] **功能**: 所有#include路径更新完成
- [ ] **功能**: 无遗漏的头文件引用
- [ ] **质量**: 无编译警告
- [ ] **覆盖率**: 所有受影响文件已更新

#### 预计工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 8h
- 期望: 4.33h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
✅ Done

---

### T32 - 更新命名空间引用

#### 描述
- 全局搜索并替换命名空间引用
- 更新 `using` 声明
- 更新类名引用

#### 参考文档
- [split_graph.md](split_graph.md) 11.3 命名空间迁移指南

#### 优先级
P0: 关键/阻塞

#### 依赖
T31

#### 验收标准
- [ ] **功能**: 所有命名空间引用更新完成
- [ ] **功能**: 无遗漏的命名空间引用
- [ ] **质量**: 无编译警告

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
✅ Done

---

### T33 - 循环依赖检测验证

#### 描述
- 使用cpp-dependencies工具检测循环依赖
- 验证依赖矩阵为下三角矩阵
- 创建检查点标签 `split-cp10-deps`

#### 参考文档
- [split_graph.md](split_graph.md) 2.4 循环依赖检测

#### 优先级
P0: 关键/阻塞

#### 依赖
T32

#### 验收标准
- [ ] **功能**: 无循环依赖警告
- [ ] **功能**: 依赖矩阵为下三角矩阵
- [ ] **功能**: 检查点标签创建成功
- [ ] **文档**: 依赖关系图已生成

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T34 - 编译验证所有模块

#### 描述
- 清理并重新编译所有模块
- 验证无编译错误和警告
- 验证无链接错误
- 创建检查点标签 `split-cp11-compile`

#### 参考文档
- [split_graph.md](split_graph.md) 9.1 编译验收

#### 优先级
P0: 关键/阻塞

#### 依赖
T33

#### 验收标准
- [ ] **功能**: 所有模块独立编译通过
- [ ] **功能**: 无编译警告
- [ ] **功能**: 无链接错误
- [ ] **功能**: 检查点标签创建成功

#### 预计工时 (PERT)
- 乐观: 2h
- 最可能: 4h
- 悲观: 8h
- 期望: 4.33h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T35 - 运行完整测试套件

#### 描述
- 运行所有单元测试
- 运行所有集成测试
- 验证测试覆盖率不低于拆分前
- 创建检查点标签 `split-cp12-final`

#### 参考文档
- [split_graph.md](split_graph.md) 9.2 测试验收

#### 优先级
P0: 关键/阻塞

#### 依赖
T34

#### 验收标准
- [ ] **功能**: 所有单元测试通过
- [ ] **功能**: 所有集成测试通过
- [ ] **功能**: 测试覆盖率不低于拆分前
- [ ] **功能**: 检查点标签创建成功
- [ ] **覆盖率**: 行覆盖率 ≥80%, 分支覆盖率 ≥70%

#### 预计工时 (PERT)
- 乐观: 2h
- 最可能: 3h
- 悲观: 6h
- 期望: 3.33h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
📋 Todo

---

### T36 - 性能基准测试验证

#### 描述
- 运行瓦片渲染性能测试
- 运行样式解析性能测试
- 对比拆分前后性能指标
- 验证性能无明显下降（<5%）

#### 参考文档
- [split_graph.md](split_graph.md) 10.3 性能基准测试

#### 优先级
P1: 高

#### 依赖
T35

#### 验收标准
- [ ] **功能**: 瓦片渲染性能变化 < 5%
- [ ] **功能**: 样式解析性能变化 < 5%
- [ ] **功能**: 内存占用变化 < 5%
- [ ] **功能**: 启动时间变化 < 10%
- [ ] **文档**: 性能测试报告已生成

#### 预计工时 (PERT)
- 乐观: 1h
- 最可能: 2h
- 悲观: 4h
- 期望: 2.17h
- 置信度: 中 (50-80%)
- 复杂度: 中

#### 状态
📋 Todo

---

## 变更日志

| 版本 | 日期 | 变更内容 | 影响范围 |
|------|------|----------|----------|
| v1.0 | 2026-04-06 | 初始任务计划创建 | - |

---

**文档生成时间**: 2026-04-06
