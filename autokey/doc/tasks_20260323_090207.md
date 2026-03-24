# Trae IDE 自动继续工具 - 任务计划

## Overview

- **Total Tasks**: 24
- **Total Estimated Hours**: 72h (PERT expected)
- **Critical Path Duration**: 48h
- **Target Completion**: 2 weeks
- **Team Size**: 1-2 developers

## Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1 | 项目初始化与目录结构 | P0 | M1 | 2h | 📋 Todo | - |
| T2 | 配置管理模块 | P0 | M1 | 4h | 📋 Todo | T1 |
| T3 | 日志系统模块 | P0 | M1 | 4h | 📋 Todo | T1 |
| T4 | 辅助函数库 | P0 | M1 | 4h | 📋 Todo | T1 |
| T5 | 窗口检测模块 | P0 | M2 | 4h | 📋 Todo | T2, T3 |
| T6 | RapidOCR 集成 | P0 | M2 | 6h | 📋 Todo | T4 |
| T7 | OCR 识别模块 | P0 | M2 | 6h | 📋 Todo | T5, T6 |
| T8 | 九宫格区域计算 | P1 | M2 | 2h | 📋 Todo | T7 |
| T9 | 关键词检测模块 | P1 | M2 | 2h | 📋 Todo | T2 |
| T10 | OCR 缓存机制 | P1 | M2 | 4h | 📋 Todo | T7 |
| T11 | 执行模块 - 基础操作 | P0 | M3 | 4h | 📋 Todo | T5 |
| T12 | 执行模块 - 位置计算 | P1 | M3 | 2h | 📋 Todo | T11 |
| T13 | 执行模块 - 结果验证 | P1 | M3 | 2h | 📋 Todo | T7, T11 |
| T14 | 重试管理模块 | P0 | M3 | 4h | 📋 Todo | T11 |
| T15 | 异常处理器 | P1 | M3 | 3h | 📋 Todo | T3 |
| T16 | 状态机实现 | P0 | M4 | 4h | 📋 Todo | T5, T7, T11 |
| T17 | 主程序框架 | P0 | M4 | 4h | 📋 Todo | T2, T3, T16 |
| T18 | 托盘图标与热键 | P1 | M4 | 2h | 📋 Todo | T17 |
| T19 | 心跳检测模块 | P1 | M4 | 2h | 📋 Todo | T3 |
| T20 | 看门狗机制 | P1 | M4 | 2h | 📋 Todo | T3 |
| T21 | 单元测试 | P1 | M5 | 6h | 📋 Todo | T1-T20 |
| T22 | 集成测试 | P1 | M5 | 4h | 📋 Todo | T21 |
| T23 | EXE 编译与发布包 | P2 | M5 | 3h | 📋 Todo | T22 |
| T24 | 文档与使用说明 | P2 | M5 | 2h | 📋 Todo | T23 |

## Resource Allocation

| Developer | Skills | Allocation | Tasks |
|-----------|--------|------------|-------|
| Dev A | AutoHotkey v2, Windows API | 100% | T1-T10, T16-T17, T21-T22 |
| Dev B | AutoHotkey v2, OCR | 50% | T11-T15, T18-T20, T23-T24 |

## Critical Path

```
T1 → T2 → T5 → T7 → T11 → T14 → T16 → T17 → T21 → T22 (48h)
```

⚠️ Any delay on these tasks will delay the project

## Milestones

### M1: Foundation (Week 1, Days 1-2)

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T1: 项目初始化与目录结构 | P0 | 2h | 📋 Todo | Dev A | 0h ⚠️ |
| T2: 配置管理模块 | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T3: 日志系统模块 | P0 | 4h | 📋 Todo | Dev A | 2h |
| T4: 辅助函数库 | P0 | 4h | 📋 Todo | Dev A | 2h |

### M2: Core Detection (Week 1, Days 3-5)

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T5: 窗口检测模块 | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T6: RapidOCR 集成 | P0 | 6h | 📋 Todo | Dev A | 2h |
| T7: OCR 识别模块 | P0 | 6h | 📋 Todo | Dev A | 0h ⚠️ |
| T8: 九宫格区域计算 | P1 | 2h | 📋 Todo | Dev A | 4h |
| T9: 关键词检测模块 | P1 | 2h | 📋 Todo | Dev B | 6h |
| T10: OCR 缓存机制 | P1 | 4h | 📋 Todo | Dev A | 2h |

### M3: Execution (Week 2, Days 1-2)

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T11: 执行模块 - 基础操作 | P0 | 4h | 📋 Todo | Dev B | 0h ⚠️ |
| T12: 执行模块 - 位置计算 | P1 | 2h | 📋 Todo | Dev B | 2h |
| T13: 执行模块 - 结果验证 | P1 | 2h | 📋 Todo | Dev B | 2h |
| T14: 重试管理模块 | P0 | 4h | 📋 Todo | Dev B | 0h ⚠️ |
| T15: 异常处理器 | P1 | 3h | 📋 Todo | Dev B | 3h |

### M4: Integration (Week 2, Days 3-4)

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T16: 状态机实现 | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T17: 主程序框架 | P0 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T18: 托盘图标与热键 | P1 | 2h | 📋 Todo | Dev B | 2h |
| T19: 心跳检测模块 | P1 | 2h | 📋 Todo | Dev B | 2h |
| T20: 看门狗机制 | P1 | 2h | 📋 Todo | Dev B | 2h |

### M5: Testing & Release (Week 2, Days 5)

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T21: 单元测试 | P1 | 6h | 📋 Todo | Dev A | 0h ⚠️ |
| T22: 集成测试 | P1 | 4h | 📋 Todo | Dev A | 0h ⚠️ |
| T23: EXE 编译与发布包 | P2 | 3h | 📋 Todo | Dev B | 2h |
| T24: 文档与使用说明 | P2 | 2h | 📋 Todo | Dev B | 3h |

## Dependency Graph

```
[T1: 项目初始化]
    │
    ├──► [T2: 配置管理] ──► [T5: 窗口检测] ──► [T7: OCR识别] ──► [T11: 执行模块] ──► [T14: 重试管理] ──► [T16: 状态机] ──► [T17: 主程序] ──► [T21: 单元测试] ──► [T22: 集成测试]
    │         │                    │                  │                  │
    │         │                    │                  │                  └──► [T13: 结果验证]
    │         │                    │                  │
    │         │                    │                  └──► [T12: 位置计算]
    │         │                    │
    │         │                    └──► [T8: 九宫格]
    │         │                    └──► [T10: OCR缓存]
    │         │
    │         └──► [T9: 关键词检测]
    │
    ├──► [T3: 日志系统] ──► [T15: 异常处理]
    │         │
    │         └──► [T19: 心跳检测]
    │         └──► [T20: 看门狗]
    │
    └──► [T4: 辅助函数] ──► [T6: RapidOCR集成]
                                  │
                                  └──► [T7: OCR识别]

[T17: 主程序] ──► [T18: 托盘图标]
[T22: 集成测试] ──► [T23: EXE编译] ──► [T24: 文档]
```

## Risk Register

| Risk | Probability | Impact | Mitigation | Owner |
|------|-------------|--------|------------|-------|
| RapidOCR DLL 兼容性问题 | Medium | High | 提前测试 DLL 加载 | Dev A |
| DPI 缩放导致坐标不准 | Medium | Medium | 添加 DPI 缩放因子计算 | Dev B |
| 内存泄漏 | Medium | Medium | 定期重启机制 + 资源释放检查 | Dev A |
| Trae IDE 界面变化 | Low | High | 使用配置化坐标，易于调整 | Dev B |
| OCR 识别准确率不足 | Medium | Medium | 调整置信度阈值 + 区域优化 | Dev A |

---

## Detailed Task Descriptions

### T1 - 项目初始化与目录结构

#### Description

创建项目基础目录结构，初始化 Git 仓库，配置开发环境。

**具体工作**:
- 创建项目目录结构
- 初始化 Git 仓库
- 创建 .gitignore 文件
- 创建 README.md 基础框架

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1500-L1530) - 部署结构

#### Priority

P0: Critical/Blocking

#### Dependencies

None

#### Acceptance Criteria

- [ ] **Functional**: 目录结构符合设计文档要求
- [ ] **Quality**: Git 仓库初始化成功
- [ ] **Documentation**: README.md 包含项目简介

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

### T2 - 配置管理模块

#### Description

实现 ConfigManager 类，支持 INI 格式配置文件的读取、写入和默认值处理。

**具体工作**:
- 实现 ConfigManager 类
- 支持 INI 文件读取 (IniRead)
- 支持配置写入 (IniWrite)
- 实现默认值机制
- 实现配置验证

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L296-L410) - 配置系统

#### Priority

P0: Critical/Blocking

#### Dependencies

T1

#### Acceptance Criteria

- [ ] **Functional**: 正确读取所有配置项
- [ ] **Functional**: 配置文件不存在时使用默认值
- [ ] **Functional**: 支持配置热重载
- [ ] **Coverage**: 核心函数覆盖率 ≥80%
- [ ] **Documentation**: 函数注释完整

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 8h
- Expected: 4.33h

#### Status

📋 Todo

---

### T3 - 日志系统模块

#### Description

实现 Logger 类，支持多级别日志记录、日志轮转和错误通知。

**具体工作**:
- 实现 Logger 类
- 支持 DEBUG/INFO/WARN/ERROR 级别
- 实现日志轮转 (maxLogSize)
- 实现 TrayTip 错误通知
- 支持日志文件配置

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L946-L1000) - 日志系统

#### Priority

P0: Critical/Blocking

#### Dependencies

T1

#### Acceptance Criteria

- [ ] **Functional**: 日志正确写入文件
- [ ] **Functional**: 日志轮转正常工作
- [ ] **Functional**: ERROR 级别触发 TrayTip
- [ ] **Quality**: 无日志写入异常

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T4 - 辅助函数库

#### Description

实现通用辅助函数，包括 DPI 缩放、屏幕截图、Bitmap 管理等。

**具体工作**:
- GetDpiScalingFactor() - DPI 缩放因子
- CaptureScreen() - 屏幕截图
- DeleteBitmap() - Bitmap 资源释放
- StrJoin() 等字符串工具函数

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L595-L660) - 辅助函数

#### Priority

P0: Critical/Blocking

#### Dependencies

T1

#### Acceptance Criteria

- [ ] **Functional**: DPI 缩放因子计算正确
- [ ] **Functional**: 屏幕截图返回有效 Bitmap
- [ ] **Functional**: DeleteBitmap 正确释放资源
- [ ] **Performance**: 截图延迟 <100ms
- [ ] **Coverage**: 函数覆盖率 ≥80%

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T5 - 窗口检测模块

#### Description

实现 WindowDetector 类，检测和激活 Trae IDE 窗口。

**具体工作**:
- DetectTraeWindow() - 检测窗口存在
- GetWindowRect() - 获取窗口位置和大小
- 处理窗口最小化状态
- 处理 DPI 缩放

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L460-L498) - 窗口检测

#### Priority

P0: Critical/Blocking

#### Dependencies

T2, T3

#### Acceptance Criteria

- [ ] **Functional**: 正确检测 Trae 窗口
- [ ] **Functional**: 正确获取窗口坐标
- [ ] **Functional**: 自动恢复最小化窗口
- [ ] **Performance**: 检测延迟 <50ms

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T6 - RapidOCR 集成

#### Description

集成 RapidOCR-AutoHotkey 库，实现 OCR 引擎初始化和基本调用。

**具体工作**:
- 下载并集成 RapidOCR-AutoHotkey 库
- 实现 RapidOCR() 单例
- 实现 RapidOCREngine 类
- 处理 DLL 加载和释放

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L500-L520) - OCR 初始化
- **External**: https://github.com/telppa/RapidOCR-AutoHotkey

#### Priority

P0: Critical/Blocking

#### Dependencies

T4

#### Acceptance Criteria

- [ ] **Functional**: RapidOCR 初始化成功
- [ ] **Functional**: OCR 调用返回结果
- [ ] **Quality**: DLL 正确加载和释放
- [ ] **Documentation**: 库使用说明完整

#### Estimated Effort

- Optimistic: 4h
- Most Likely: 6h
- Pessimistic: 10h
- Expected: 6.33h

#### Status

📋 Todo

---

### T7 - OCR 识别模块

#### Description

实现 OCRDetector 类，封装 OCR 识别流程。

**具体工作**:
- OCRDetector.Initialize() - 初始化
- OCRDetector.PerformOCR() - 执行识别
- OCRDetector.FilterResults() - 结果过滤
- OCRDetector.ExtractKeywords() - 提取文本

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L500-L595) - OCR 识别

#### Priority

P0: Critical/Blocking

#### Dependencies

T5, T6

#### Acceptance Criteria

- [ ] **Functional**: OCR 识别返回文本结果
- [ ] **Functional**: 置信度过滤正常工作
- [ ] **Performance**: 识别延迟 <3s
- [ ] **Quality**: 资源正确释放

#### Estimated Effort

- Optimistic: 4h
- Most Likely: 6h
- Pessimistic: 10h
- Expected: 6.33h

#### Status

📋 Todo

---

### T8 - 九宫格区域计算

#### Description

实现 OCR 区域的九宫格划分和坐标计算。

**具体工作**:
- CalculateOCRRegion() - 计算区域坐标
- 支持 1-9 九个区域
- 支持全屏模式 (regionCode=0)

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L545-L595) - 九宫格区域

#### Priority

P1: High

#### Dependencies

T7

#### Acceptance Criteria

- [ ] **Functional**: 九宫格坐标计算正确
- [ ] **Functional**: 全屏模式正常工作
- [ ] **Quality**: 边界情况处理完善

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

### T9 - 关键词检测模块

#### Description

实现 KeywordDetector 类，检测文本中的关键词。

**具体工作**:
- KeywordDetector.Initialize() - 加载关键词
- KeywordDetector.DetectKeywords() - 检测关键词
- 支持配置化关键词列表

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L660-L700) - 关键词检测

#### Priority

P1: High

#### Dependencies

T2

#### Acceptance Criteria

- [ ] **Functional**: 正确检测包含关键词的文本
- [ ] **Functional**: 支持多关键词检测
- [ ] **Performance**: 检测延迟 <10ms

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 3h
- Expected: 2h

#### Status

📋 Todo

---

### T10 - OCR 缓存机制

#### Description

实现 OCRCache 类，缓存 OCR 结果以提升性能。

**具体工作**:
- OCRCache.GetCachedResult() - 获取缓存
- OCRCache.SetCache() - 设置缓存
- OCRCache.CalculateHash() - 计算图像哈希
- 实现缓存超时机制

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1285-L1330) - OCR 缓存

#### Priority

P1: High

#### Dependencies

T7

#### Acceptance Criteria

- [ ] **Functional**: 相同图像使用缓存结果
- [ ] **Functional**: 缓存超时后重新识别
- [ ] **Performance**: 缓存命中时延迟 <10ms

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T11 - 执行模块 - 基础操作

#### Description

实现 AutoContinueExecutor 类的基础操作功能。

**具体工作**:
- AutoContinueExecutor.Execute() - 主执行函数
- ClickAt() - 点击操作
- InputText() - 文本输入
- 使用剪贴板输入中文

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L700-L800) - 执行模块

#### Priority

P0: Critical/Blocking

#### Dependencies

T5

#### Acceptance Criteria

- [ ] **Functional**: 点击操作正确执行
- [ ] **Functional**: 文本输入正确
- [ ] **Functional**: 中文输入无乱码
- [ ] **Performance**: 操作延迟符合配置

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T12 - 执行模块 - 位置计算

#### Description

实现输入框和发送按钮的位置计算逻辑。

**具体工作**:
- CalculateInputPosition() - 计算输入框位置
- CalculateSendButtonPosition() - 计算发送按钮位置
- 支持配置化坐标比例

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L750-L800) - 位置计算

#### Priority

P1: High

#### Dependencies

T11

#### Acceptance Criteria

- [ ] **Functional**: 位置计算正确
- [ ] **Functional**: 支持窗口缩放
- [ ] **Quality**: DPI 缩放处理正确

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

### T13 - 执行模块 - 结果验证

#### Description

实现执行结果的验证逻辑。

**具体工作**:
- VerifyResult() - 验证操作结果
- 检查提示是否消失
- 处理验证失败情况

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L800-L830) - 结果验证

#### Priority

P1: High

#### Dependencies

T7, T11

#### Acceptance Criteria

- [ ] **Functional**: 正确验证操作结果
- [ ] **Functional**: 提示消失时返回 true
- [ ] **Performance**: 验证延迟 <2s

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

### T14 - 重试管理模块

#### Description

实现 RetryManager 类，管理操作重试逻辑。

**具体工作**:
- RetryManager.Initialize() - 初始化
- RetryManager.ExecuteWithRetry() - 带重试执行
- 实现指数退避
- 实现连续失败计数

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L830-L900) - 重试机制

#### Priority

P0: Critical/Blocking

#### Dependencies

T11

#### Acceptance Criteria

- [ ] **Functional**: 重试逻辑正确执行
- [ ] **Functional**: 指数退避正常工作
- [ ] **Functional**: 连续失败触发重启
- [ ] **Quality**: 无无限重试

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T15 - 异常处理器

#### Description

实现 ExceptionHandler 类，统一处理各类异常。

**具体工作**:
- ExceptionHandler.Handle() - 异常处理入口
- HandleOCRError() - OCR 异常处理
- HandleWindowError() - 窗口异常处理
- HandleGenericError() - 通用异常处理

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1040-L1100) - 错误处理

#### Priority

P1: High

#### Dependencies

T3

#### Acceptance Criteria

- [ ] **Functional**: 异常正确分类处理
- [ ] **Functional**: OCR 异常触发重新初始化
- [ ] **Quality**: 无未捕获异常

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T16 - 状态机实现

#### Description

实现三状态状态机 (COOLDOWN → DETECTING → POST_EXECUTION)。

**具体工作**:
- 实现 COOLDOWN 状态逻辑
- 实现 DETECTING 状态逻辑
- 实现 POST_EXECUTION 状态逻辑
- 状态转换计时

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L70-L150) - 状态机设计

#### Priority

P0: Critical/Blocking

#### Dependencies

T5, T7, T11

#### Acceptance Criteria

- [ ] **Functional**: 状态转换正确
- [ ] **Functional**: 冷却期 15 分钟
- [ ] **Functional**: 检测间隔 10 秒
- [ ] **Functional**: 执行后等待 30 秒
- [ ] **Quality**: 无状态混乱

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T17 - 主程序框架

#### Description

实现主程序框架，整合所有模块。

**具体工作**:
- 主程序入口
- 模块初始化顺序
- 主循环 MainLoop()
- 资源清理 CleanupResources()

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1000-L1100) - 主程序

#### Priority

P0: Critical/Blocking

#### Dependencies

T2, T3, T16

#### Acceptance Criteria

- [ ] **Functional**: 所有模块正确初始化
- [ ] **Functional**: 主循环正常运行
- [ ] **Functional**: 退出时资源正确释放
- [ ] **Quality**: 无启动异常

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T18 - 托盘图标与热键

#### Description

实现系统托盘图标和快捷键绑定。

**具体工作**:
- 创建托盘菜单
- 实现启动/暂停切换
- 实现快捷键绑定 (Ctrl+Alt+P/L/R)
- 托盘图标状态指示

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1100-L1150) - 托盘图标

#### Priority

P1: High

#### Dependencies

T17

#### Acceptance Criteria

- [ ] **Functional**: 托盘菜单正常工作
- [ ] **Functional**: 快捷键响应正确
- [ ] **Quality**: 图标状态正确显示

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 3h
- Expected: 2h

#### Status

📋 Todo

---

### T19 - 心跳检测模块

#### Description

实现 HeartbeatMonitor 类，检测脚本是否卡死。

**具体工作**:
- HeartbeatMonitor.Initialize() - 初始化
- HeartbeatMonitor.Check() - 心跳检查
- 超时触发重启

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1375-L1420) - 心跳检测

#### Priority

P1: High

#### Dependencies

T3

#### Acceptance Criteria

- [ ] **Functional**: 心跳检测正常工作
- [ ] **Functional**: 超时触发重启
- [ ] **Quality**: 首次检查不误报

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 3h
- Expected: 2h

#### Status

📋 Todo

---

### T20 - 看门狗机制

#### Description

实现 Watchdog 类，通过文件监控脚本状态。

**具体工作**:
- Watchdog.Initialize() - 初始化
- Watchdog.Update() - 更新看门狗文件
- Watchdog.Check() - 检查看门狗状态

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1403-L1460) - 看门狗机制

#### Priority

P1: High

#### Dependencies

T3

#### Acceptance Criteria

- [ ] **Functional**: 看门狗文件正确更新
- [ ] **Functional**: 超时检测正常工作
- [ ] **Quality**: 文件操作异常处理完善

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 3h
- Expected: 2h

#### Status

📋 Todo

---

### T21 - 单元测试

#### Description

编写各模块的单元测试。

**具体工作**:
- ConfigManager 测试
- Logger 测试
- KeywordDetector 测试
- RetryManager 测试
- 状态机测试

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1800-L1850) - 测试策略

#### Priority

P1: High

#### Dependencies

T1-T20

#### Acceptance Criteria

- [ ] **Coverage**: 代码覆盖率 ≥80%
- [ ] **Functional**: 所有测试用例通过
- [ ] **Quality**: 无测试警告

#### Estimated Effort

- Optimistic: 4h
- Most Likely: 6h
- Pessimistic: 10h
- Expected: 6.33h

#### Status

📋 Todo

---

### T22 - 集成测试

#### Description

编写端到端集成测试。

**具体工作**:
- 完整流程测试
- 错误恢复测试
- 性能测试
- 稳定性测试

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1850-L1890) - 集成测试

#### Priority

P1: High

#### Dependencies

T21

#### Acceptance Criteria

- [ ] **Functional**: 完整流程测试通过
- [ ] **Performance**: 性能指标达标
- [ ] **Quality**: 24小时稳定性测试通过

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

📋 Todo

---

### T23 - EXE 编译与发布包

#### Description

编译 EXE 并创建发布包。

**具体工作**:
- 使用 Ahk2Exe 编译
- 创建发布目录结构
- 编写发布脚本
- 测试独立运行

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1530-L1620) - EXE 编译

#### Priority

P2: Medium

#### Dependencies

T22

#### Acceptance Criteria

- [ ] **Functional**: EXE 可独立运行
- [ ] **Functional**: 发布包结构完整
- [ ] **Quality**: 无编译警告

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T24 - 文档与使用说明

#### Description

编写用户文档和使用说明。

**具体工作**:
- 更新 README.md
- 编写使用说明
- 编写常见问题解答
- 编写配置说明

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md#L1620-L1800) - 使用说明

#### Priority

P2: Medium

#### Dependencies

T23

#### Acceptance Criteria

- [ ] **Documentation**: README 完整
- [ ] **Documentation**: 使用说明清晰
- [ ] **Documentation**: FAQ 覆盖常见问题

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

## Change Log

| Version | Date | Changes | Impact |
|---------|------|---------|--------|
| v1.0 | 2026-03-22 | Initial task plan | - |
