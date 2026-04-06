# Alert 模块增强实施任务清单

## 概述

- 目标: 完善 alert 模块的 UKC 和 CPA 计算器，添加蹲底效应和地球曲率修正
- 范围: alert 模块（不涉及 alarm 模块）
- 优先级: P0
- 状态: ✅ 已完成

## 任务清单

### Phase 1: 核心计算器增强

#### T1.1: 检查现有 UKCCalculator 实现

| 任务ID | 任务描述 | 状态 | 备注 |
|--------|----------|------|------|
| T1.1.1 | 检查 UKCCalculator 是否已有蹲底效应计算 | ✅ Done | 已有 Barrass 和 I.Corell 公式 |
| T1.1.2 | 检查 SquatParams 结构是否完整 | ✅ Done | 包含船舶参数 |
| T1.1.3 | 验证 UKC 计算逻辑是否正确 | ✅ Done | 测试通过 |

#### T1.2: 增强 UKCCalculator

| 任务ID | 任务描述 | 状态 | 备注 |
|--------|----------|------|------|
| T1.2.1 | 添加船舶参数输入支持 | ✅ Done | block_coefficient, ship_length 等 |
| T1.2.2 | 添加完整蹲底效应计算 | ✅ Done | 已实现 Barrass 和 I.Corell |
| T1.2.3 | 添加吃水、横倾、波浪修正 | ✅ Done | 测试通过 |

#### T1.3: 检查现有 CPACalculator 实现

| 任务ID | 任务描述 | 状态 | 备注 |
|--------|----------|------|------|
| T1.3.1 | 检查 CPACalculator 是否已有地球曲率修正 | ✅ Done | 已添加 |
| T1.3.2 | 验证距离计算精度 | ✅ Done | 使用地球曲率修正 |

#### T1.4: 增强 CPACalculator

| 任务ID | 任务描述 | 状态 | 备注 |
|--------|----------|------|------|
| T1.4.1 | 添加地球曲率修正到距离计算 | ✅ Done | 使用 AlertConstants 中的常量 |
| T1.4.2 | 验证 CPA/TCPA 计算精度 | ✅ Done | 测试通过 |

#### T1.5: 统一枚举定义

| 任务ID | 任务描述 | 状态 | 备注 |
|--------|----------|------|------|
| T1.5.1 | 验证 alert 模块 AlertType 枚举 | ✅ Done | 已完整 |
| T1.5.2 | 验证 alert 模块 AlertLevel 枚举 | ✅ Done | 已完整 |
| T1.5.3 | 验证 alert 模块 AlertStatus 枚举 | ✅ Done | 已完整 |

## 里程碑

### M1: 计算器增强完成

- [x] UKCCalculator 支持蹲底效应计算
- [x] CPACalculator 支持地球曲率修正
- [x] 所有单元测试通过 (222 tests)

## 验收标准

- [x] UKCCalculator 可计算船舶蹲底效应
- [x] CPACalculator 使用地球曲率修正计算距离
- [x] 代码无编译错误
- [x] 现有测试通过

## 实施记录

### 2026-04-06

1. **UKCCalculator 验证**
   - 确认已实现 Barrass 和 I.Corell 蹲底效应公式
   - SquatParams 结构包含所有必要参数

2. **CPACalculator 增强**
   - 在 types.h 中添加 AlertConstants 结构体
   - 修改 CalculateDistance 方法实现地球曲率修正
   - 使用纬度平均值计算经度每度米数

3. **测试验证**
   - 编译成功
   - 222 个测试全部通过

4. **DepthAlertChecker 融合蹲底效应**
   - 添加 SetShipSpeed 和 SetSquatParams 方法
   - 添加 CalculateUKCWithSquat 方法
   - Check 方法中根据 use_dynamic_ukc 配置自动使用蹲底效应计算
   - 预警消息中包含蹲底效应信息

5. **CollisionAlertChecker 融合地球曲率修正**
   - 使用 CPACalculator 替代平面坐标近似
   - 添加 CalculateCPAWithDetails 方法返回完整 CPAResult
   - Check 方法使用 CPACalculator::Calculate 获取完整碰撞风险信息
   - 预警消息使用 CPAResult.warning_message

6. **const 正确性修复**
   - UKCCalculator::CalculateSquat 等方法添加 const 修饰符
   - CPACalculator::Calculate 等方法添加 const 修饰符
