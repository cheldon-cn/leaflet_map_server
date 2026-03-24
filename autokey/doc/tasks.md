# Trae IDE 自动继续工具 - 任务计划

## Overview

- **Total Tasks**: 24
- **Completed Tasks**: 24 (100%)
- **Remaining Tasks**: 0 (0%)
- **Total Estimated Hours**: 74h (PERT expected)
- **Critical Path Duration**: 46h
- **Target Completion**: 2 weeks
- **Team Size**: 1-2 developers

## Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## External Resources

- **RapidOCR-AutoHotkey**: 已集成在 `RapidOCR-AutoHotkey/` 目录
  - 库文件: `RapidOCR/RapidOCR.ahk`
  - 依赖库: `RapidOCR/Lib/` (ImagePut.ahk, NonNull.ahk, cJson.ahk)
  - 模型文件: `RapidOCR/Exe/models/` (中英文、日韩文模型)
  - 说明文档: `README.md`, 示例脚本 (0-10.ahk)

## Task Summary

| Task ID | Task Name | Priority | Milestone | Effort | Status | Dependencies |
|---------|-----------|----------|-----------|--------|--------|--------------|
| T1 | 项目初始化与目录结构 | P0 | M1 | 2h | ✅ Done | - |
| T2 | 配置管理模块 (ConfigManager) | P0 | M1 | 4h | ✅ Done | T1 |
| T3 | 日志系统模块 (Logger) | P0 | M1 | 3h | ✅ Done | T1 |
| T4 | 辅助函数库 (Utils) | P0 | M1 | 3h | ✅ Done | T1 |
| T5 | 窗口检测模块 (WindowDetector) | P0 | M2 | 3h | ✅ Done | T2, T3 |
| T6 | RapidOCR 集成 | P0 | M2 | 3h | ✅ Done | T4 |
| T7 | OCR 识别模块 (OCRDetector) | P0 | M2 | 5h | ✅ Done | T5, T6 |
| T8 | 九宫格区域计算 | P1 | M2 | 2h | ✅ Done | T7 |
| T9 | 关键词检测模块 (KeywordDetector) | P1 | M2 | 2h | ✅ Done | T2 |
| T10 | OCR 缓存机制 (OCRCache) | P1 | M2 | 3h | ✅ Done | T7 |
| T11 | 执行模块 (AutoContinueExecutor) | P0 | M3 | 4h | ✅ Done | T5 |
| T12 | 执行模块 - 位置计算 | P1 | M3 | 2h | ✅ Done | T11 |
| T13 | 执行模块 - 结果验证 | P1 | M3 | 2h | ✅ Done | T7, T11 |
| T14 | 重试管理模块 (RetryManager) | P0 | M3 | 3h | ✅ Done | T11 |
| T15 | 异常处理器 (ExceptionHandler) | P1 | M3 | 2h | ✅ Done | T3 |
| T16 | 状态机实现 (StateMachine) | P0 | M4 | 4h | ✅ Done | T5, T7, T11 |
| T17 | 主程序框架 (Main) | P0 | M4 | 3h | ✅ Done | T2, T3, T16 |
| T18 | 托盘图标与热键 | P1 | M4 | 2h | ✅ Done | T17 |
| T19 | 心跳检测模块 (HeartbeatMonitor) | P1 | M4 | 2h | ✅ Done | T3 |
| T20 | 看门狗机制 (Watchdog) | P1 | M4 | 2h | ✅ Done | T3 |
| T21 | 单元测试 | P1 | M5 | 5h | ✅ Done | T1-T20 |
| T22 | 集成测试 | P1 | M5 | 3h | ✅ Done | T21 |
| T23 | EXE 编译与发布包 | P2 | M5 | 2h | ✅ Done | T22 |
| T24 | 文档与使用说明 | P2 | M5 | 2h | ✅ Done | T23 |

## Resource Allocation

| Developer | Skills | Allocation | Tasks |
|-----------|--------|------------|-------|
| Dev A | AutoHotkey v2, Windows API | 100% | T1-T10, T16-T17, T21-T22 |
| Dev B | AutoHotkey v2, OCR | 50% | T11-T15, T18-T20, T23-T24 |

## Critical Path

```
T1 → T2 → T5 → T7 → T11 → T14 → T16 → T17 → T21 → T22 (46h)
```

⚠️ Any delay on these tasks will delay the project

## Milestones

### M1: Foundation (Week 1, Days 1-2) ✅

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T1: 项目初始化与目录结构 | P0 | 2h | ✅ Done | Dev A | 0h ⚠️ |
| T2: 配置管理模块 | P0 | 4h | ✅ Done | Dev A | 0h ⚠️ |
| T3: 日志系统模块 | P0 | 3h | ✅ Done | Dev A | 2h |
| T4: 辅助函数库 | P0 | 3h | ✅ Done | Dev A | 2h |

### M2: Core Detection (Week 1, Days 3-5) ✅

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T5: 窗口检测模块 | P0 | 3h | ✅ Done | Dev A | 0h ⚠️ |
| T6: RapidOCR 集成 | P0 | 3h | ✅ Done | Dev A | 2h |
| T7: OCR 识别模块 | P0 | 5h | ✅ Done | Dev A | 0h ⚠️ |
| T8: 九宫格区域计算 | P1 | 2h | ✅ Done | Dev A | 4h |
| T9: 关键词检测模块 | P1 | 2h | ✅ Done | Dev B | 6h |
| T10: OCR 缓存机制 | P1 | 3h | ✅ Done | Dev A | 2h |

### M3: Execution (Week 2, Days 1-2) ✅

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T11: 执行模块 | P0 | 4h | ✅ Done | Dev B | 0h ⚠️ |
| T12: 执行模块 - 位置计算 | P1 | 2h | ✅ Done | Dev B | 2h |
| T13: 执行模块 - 结果验证 | P1 | 2h | ✅ Done | Dev B | 2h |
| T14: 重试管理模块 | P0 | 3h | ✅ Done | Dev B | 0h ⚠️ |
| T15: 异常处理器 | P1 | 2h | ✅ Done | Dev B | 3h |

### M4: Integration (Week 2, Days 3-4) ✅

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T16: 状态机实现 | P0 | 4h | ✅ Done | Dev A | 0h ⚠️ |
| T17: 主程序框架 | P0 | 3h | ✅ Done | Dev A | 0h ⚠️ |
| T18: 托盘图标与热键 | P1 | 2h | ✅ Done | Dev B | 2h |
| T19: 心跳检测模块 | P1 | 2h | ✅ Done | Dev B | 2h |
| T20: 看门狗机制 | P1 | 2h | ✅ Done | Dev B | 2h |

### M5: Testing & Release (Week 2, Day 5) ✅

| Task | Priority | Effort | Status | Assignee | Float |
|------|----------|--------|--------|----------|-------|
| T21: 单元测试 | P1 | 5h | ✅ Done | Dev A | 0h ⚠️ |
| T22: 集成测试 | P1 | 3h | ✅ Done | Dev A | 0h ⚠️ |
| T23: EXE 编译与发布包 | P2 | 2h | ✅ Done | Dev B | 2h |
| T24: 文档与使用说明 | P2 | 2h | ✅ Done | Dev B | 3h |

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

**目录结构** (所有文件生成到 `root/` 目录下):
```
root/
├── src/           # 源代码目录
│   ├── ConfigManager.ahk
│   ├── Logger.ahk
│   ├── Utils.ahk
│   ├── WindowDetector.ahk
│   ├── OCRWrapper.ahk
│   ├── OCRDetector.ahk
│   ├── GridRegion.ahk
│   ├── KeywordDetector.ahk
│   ├── AutoContinueExecutor.ahk
│   ├── RetryManager.ahk
│   ├── ExceptionHandler.ahk
│   ├── StateMachine.ahk
│   ├── HeartbeatMonitor.ahk
│   ├── Watchdog.ahk
│   ├── ResultVerifier.ahk
│   └── Main.ahk
├── lib/           # 第三方库目录
│   └── RapidOCR/  # RapidOCR-AutoHotkey 库
│       ├── RapidOCR.ahk
│       ├── exe/
│       │   ├── RapidOCR-json.exe
│       │   └── models/
│       └── Lib/
│           ├── ImagePut.ahk
│           ├── NonNull.ahk
│           └── cJson.ahk
├── config/        # 配置文件目录
│   └── config.ini
├── log/           # 日志文件目录
├── tests/         # 测试文件目录
│   ├── UnitTests.ahk
│   └── IntegrationTests.ahk
└── TraeAutoContinue.ahk  # 主入口脚本
```

**具体工作**:
- 创建 root/ 目录结构 (src/, lib/, config/, log/)
- 初始化 Git 仓库
- 创建 .gitignore 文件
- 创建 README.md 基础框架
- 复制 RapidOCR-AutoHotkey 库到 root/lib/

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 部署结构

#### Priority

P0: Critical/Blocking

#### Dependencies

None

#### Acceptance Criteria

- [ ] **Functional**: 目录结构符合设计文档要求
- [ ] **Functional**: RapidOCR 库正确放置
- [ ] **Quality**: Git 仓库初始化成功
- [ ] **Documentation**: README.md 包含项目简介

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

✅ Done

---

### T2 - 配置管理模块 (ConfigManager)

#### Description

实现 ConfigManager 类，支持 INI 格式配置文件的读取、写入和默认值处理。

**具体工作**:
- 实现 ConfigManager 类
- 支持 INI 文件读取 (IniRead)
- 支持配置写入 (IniWrite)
- 实现默认值机制
- 实现配置验证
- 创建默认 config.ini 模板

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 配置系统 (3.1节)

#### Priority

P0: Critical/Blocking

#### Dependencies

T1

#### Acceptance Criteria

- [ ] **Functional**: 正确读取所有配置项 (General/Detection/OCR/Execution/UI/Position)
- [ ] **Functional**: 配置文件不存在时使用默认值
- [ ] **Functional**: 支持配置热重载
- [ ] **Coverage**: 核心函数覆盖率 ≥80%
- [ ] **Documentation**: 函数注释完整

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4h

#### Status

✅ Done

---

### T3 - 日志系统模块 (Logger)

#### Description

实现 Logger 类，支持多级别日志记录、日志轮转和错误通知。

**具体工作**:
- 实现 Logger 类
- 支持 DEBUG/INFO/WARN/ERROR 级别
- 实现日志轮转 (maxLogSize)
- 实现 TrayTip 错误通知
- 支持日志文件配置
- 支持日志格式化输出

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 日志系统

#### Priority

P0: Critical/Blocking

#### Dependencies

T1

#### Acceptance Criteria

- [ ] **Functional**: 日志正确写入文件
- [ ] **Functional**: 日志轮转正常工作
- [ ] **Functional**: ERROR 级别触发 TrayTip
- [ ] **Functional**: 日志格式包含时间戳和级别
- [ ] **Quality**: 无日志写入异常

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

✅ Done

---

### T4 - 辅助函数库 (Utils)

#### Description

实现通用辅助函数，包括 DPI 缩放、屏幕截图、Bitmap 管理等。

**具体工作**:
- GetDpiScalingFactor() - DPI 缩放因子计算
- CaptureScreen() - 屏幕截图 (GDI+)
- DeleteBitmap() - Bitmap 资源释放
- StrJoin() - 字符串连接工具
- IsProcessRunning() - 进程检测
- EnsureDirectory() - 目录创建

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 辅助函数

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
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

✅ Done

---

### T5 - 窗口检测模块 (WindowDetector)

#### Description

实现 WindowDetector 类，检测和激活 Trae IDE 窗口。

**具体工作**:
- DetectTraeWindow() - 检测窗口存在
- GetWindowRect() - 获取窗口位置和大小
- 处理窗口最小化状态
- 处理 DPI 缩放
- 窗口激活逻辑

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 窗口检测 (3.2.1节)

#### Priority

P0: Critical/Blocking

#### Dependencies

T2, T3

#### Acceptance Criteria

- [ ] **Functional**: 正确检测 Trae 窗口 (ahk_exe Trae.exe)
- [ ] **Functional**: 正确获取窗口坐标
- [ ] **Functional**: 自动恢复最小化窗口
- [ ] **Performance**: 检测延迟 <50ms

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T6 - RapidOCR 集成

#### Description

集成 RapidOCR-AutoHotkey 库，实现 OCR 引擎初始化和基本调用。

**已有资源** (位于 `RapidOCR-AutoHotkey/` 目录):
- 库文件: `RapidOCR/RapidOCR.ahk` - 主库文件
- 依赖库: `RapidOCR/Lib/` - ImagePut.ahk, NonNull.ahk, cJson.ahk
- 模型文件: `RapidOCR/Exe/models/` - 中英文、日韩文 ONNX 模型
- 示例脚本: `0-10.ahk` - 各种使用场景示例

**具体工作**:
- 引用现有 RapidOCR.ahk 库
- 实现 RapidOCR() 单例
- 测试 OCR 基本功能 (参考示例脚本)
- 配置中文模型路径

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - OCR 初始化
- **Local**: `RapidOCR-AutoHotkey/README.md`
- **External**: https://github.com/telppa/RapidOCR-AutoHotkey

#### Priority

P0: Critical/Blocking

#### Dependencies

T4

#### Acceptance Criteria

- [ ] **Functional**: RapidOCR 初始化成功
- [ ] **Functional**: OCR 调用返回结果
- [ ] **Functional**: 中文识别正常工作
- [ ] **Documentation**: 库使用说明完整

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T7 - OCR 识别模块 (OCRDetector)

#### Description

实现 OCRDetector 类，封装 OCR 识别流程。

**具体工作**:
- OCRDetector.Initialize() - 初始化
- OCRDetector.PerformOCR() - 执行识别
- OCRDetector.FilterResults() - 结果过滤 (置信度)
- OCRDetector.ExtractKeywords() - 提取文本
- 错误处理和重试

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - OCR 识别 (3.2.2节)

#### Priority

P0: Critical/Blocking

#### Dependencies

T5, T6

#### Acceptance Criteria

- [ ] **Functional**: OCR 识别返回文本结果
- [ ] **Functional**: 置信度过滤正常工作
- [ ] **Performance**: 识别延迟 <3s
- [ ] **Quality**: 资源正确释放 (Bitmap)

#### Estimated Effort

- Optimistic: 3h
- Most Likely: 5h
- Pessimistic: 8h
- Expected: 5.17h

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
- 区域边界验证

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 九宫格区域

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

### T9 - 关键词检测模块 (KeywordDetector)

#### Description

实现 KeywordDetector 类，检测文本中的关键词。

**具体工作**:
- KeywordDetector.Initialize() - 加载关键词
- KeywordDetector.DetectKeywords() - 检测关键词
- 支持配置化关键词列表
- 支持多关键词匹配

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 关键词检测

#### Priority

P1: High

#### Dependencies

T2

#### Acceptance Criteria

- [ ] **Functional**: 正确检测包含关键词的文本
- [ ] **Functional**: 支持多关键词检测 (模型思考次数已达上限, 请输入继续)
- [ ] **Performance**: 检测延迟 <10ms

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 3h
- Expected: 2h

#### Status

📋 Todo

---

### T10 - OCR 缓存机制 (OCRCache)

#### Description

实现 OCRCache 类，缓存 OCR 结果以提升性能。

**具体工作**:
- OCRCache.GetCachedResult() - 获取缓存
- OCRCache.SetCache() - 设置缓存
- OCRCache.CalculateHash() - 计算图像哈希
- 实现缓存超时机制 (默认 60 秒)
- 缓存命中率统计

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - OCR 缓存

#### Priority

P1: High

#### Dependencies

T7

#### Acceptance Criteria

- [ ] **Functional**: 相同图像使用缓存结果
- [ ] **Functional**: 缓存超时后重新识别
- [ ] **Performance**: 缓存命中时延迟 <10ms
- [ ] **Quality**: 缓存大小可控

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T11 - 执行模块 (AutoContinueExecutor)

#### Description

实现 AutoContinueExecutor 类的基础操作功能。

**具体工作**:
- AutoContinueExecutor.Execute() - 主执行函数
- ClickAt() - 点击操作
- InputText() - 文本输入
- 使用剪贴板输入中文 "继续"
- 执行延迟控制

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 执行模块 (3.3节)

#### Priority

P0: Critical/Blocking

#### Dependencies

T5

#### Acceptance Criteria

- [ ] **Functional**: 点击操作正确执行
- [ ] **Functional**: 文本输入正确 ("继续")
- [ ] **Functional**: 中文输入无乱码
- [ ] **Performance**: 操作延迟符合配置 (InputDelay/ClickDelay)

#### Estimated Effort

- Optimistic: 3h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4.17h

#### Status

📋 Todo

---

### T12 - 执行模块 - 位置计算

#### Description

实现输入框和发送按钮的位置计算逻辑。

**具体工作**:
- CalculateInputPosition() - 计算输入框位置 (默认 50%, 92%)
- CalculateSendButtonPosition() - 计算发送按钮位置 (默认 85%, 92%)
- 支持配置化坐标比例
- DPI 缩放处理

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 位置计算

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
- 检查提示是否消失 (OCR 二次检测)
- 处理验证失败情况
- 验证超时处理

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 结果验证

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

### T14 - 重试管理模块 (RetryManager)

#### Description

实现 RetryManager 类，管理操作重试逻辑。

**具体工作**:
- RetryManager.Initialize() - 初始化
- RetryManager.ExecuteWithRetry() - 带重试执行
- 实现指数退避 (可选)
- 实现连续失败计数
- 触发脚本重启机制

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 重试机制

#### Priority

P0: Critical/Blocking

#### Dependencies

T11

#### Acceptance Criteria

- [ ] **Functional**: 重试逻辑正确执行 (MaxRetries=3)
- [ ] **Functional**: 连续失败触发重启 (ConsecutiveFailures >= 5)
- [ ] **Quality**: 无无限重试

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T15 - 异常处理器 (ExceptionHandler)

#### Description

实现 ExceptionHandler 类，统一处理各类异常。

**具体工作**:
- ExceptionHandler.Handle() - 异常处理入口
- HandleOCRError() - OCR 异常处理
- HandleWindowError() - 窗口异常处理
- HandleGenericError() - 通用异常处理
- 异常日志记录

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 错误处理

#### Priority

P1: High

#### Dependencies

T3

#### Acceptance Criteria

- [ ] **Functional**: 异常正确分类处理
- [ ] **Functional**: OCR 异常触发重新初始化
- [ ] **Quality**: 无未捕获异常

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

### T16 - 状态机实现 (StateMachine)

#### Description

实现三状态状态机 (COOLDOWN → DETECTING → POST_EXECUTION)。

**具体工作**:
- 实现 COOLDOWN 状态逻辑 (15分钟)
- 实现 DETECTING 状态逻辑 (每10秒检测)
- 实现 POST_EXECUTION 状态逻辑 (30秒等待)
- 状态转换计时
- 状态日志记录

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 状态机设计 (2.2节)

#### Priority

P0: Critical/Blocking

#### Dependencies

T5, T7, T11

#### Acceptance Criteria

- [ ] **Functional**: 状态转换正确
- [ ] **Functional**: 冷却期 15 分钟 (CooldownPeriod)
- [ ] **Functional**: 检测间隔 10 秒 (CheckInterval)
- [ ] **Functional**: 执行后等待 30 秒 (PostExecutionDelay)
- [ ] **Quality**: 无状态混乱

#### Estimated Effort

- Optimistic: 3h
- Most Likely: 4h
- Pessimistic: 6h
- Expected: 4.17h

#### Status

📋 Todo

---

### T17 - 主程序框架 (Main)

#### Description

实现主程序框架，整合所有模块。

**具体工作**:
- 主程序入口 (Main())
- 模块初始化顺序
- 主循环 MainLoop()
- 资源清理 CleanupResources()
- 退出处理 (OnExit)

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 主程序

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
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T18 - 托盘图标与热键

#### Description

实现系统托盘图标和快捷键绑定。

**具体工作**:
- 创建托盘菜单 (启动/暂停/退出)
- 实现启动/暂停切换
- 实现快捷键绑定 (Ctrl+Alt+P 暂停/继续)
- 托盘图标状态指示 (运行中/已暂停)
- 托盘提示信息

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 托盘图标

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

### T19 - 心跳检测模块 (HeartbeatMonitor)

#### Description

实现心跳检测模块，监控脚本运行状态。

**具体工作**:
- HeartbeatMonitor.Initialize() - 初始化
- HeartbeatMonitor.Beat() - 心跳记录
- HeartbeatMonitor.CheckHealth() - 健康检查
- 心跳日志记录

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 心跳检测

#### Priority

P1: High

#### Dependencies

T3

#### Acceptance Criteria

- [ ] **Functional**: 心跳正确记录
- [ ] **Functional**: 健康检查返回正确状态
- [ ] **Performance**: 心跳开销 <1ms

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 3h
- Expected: 2h

#### Status

📋 Todo

---

### T20 - 看门狗机制 (Watchdog)

#### Description

实现看门狗机制，检测脚本假死并自动重启。

**具体工作**:
- Watchdog.Initialize() - 初始化
- Watchdog.Check() - 检查心跳
- Watchdog.Restart() - 重启脚本
- 配置化超时阈值

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 看门狗机制

#### Priority

P1: High

#### Dependencies

T3

#### Acceptance Criteria

- [ ] **Functional**: 检测到假死时自动重启
- [ ] **Functional**: 重启后状态正确恢复
- [ ] **Quality**: 无误重启

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
- WindowDetector 测试
- OCRDetector 测试
- KeywordDetector 测试
- OCRCache 测试
- AutoContinueExecutor 测试
- StateMachine 测试

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 测试

#### Priority

P1: High

#### Dependencies

T1-T20

#### Acceptance Criteria

- [ ] **Coverage**: 核心模块覆盖率 ≥80%
- [ ] **Quality**: 所有测试通过
- [ ] **Documentation**: 测试用例文档完整

#### Estimated Effort

- Optimistic: 3h
- Most Likely: 5h
- Pessimistic: 8h
- Expected: 5.17h

#### Status

📋 Todo

---

### T22 - 集成测试

#### Description

执行端到端集成测试。

**具体工作**:
- 完整流程测试 (启动→检测→执行→验证)
- 状态转换测试
- 异常恢复测试
- 长时间运行测试 (稳定性)
- 性能基准测试

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 测试

#### Priority

P1: High

#### Dependencies

T21

#### Acceptance Criteria

- [ ] **Functional**: 完整流程测试通过
- [ ] **Functional**: 状态转换正确
- [ ] **Performance**: 检测延迟 <100ms, OCR 延迟 <3s
- [ ] **Stability**: 连续运行 8 小时无异常

#### Estimated Effort

- Optimistic: 2h
- Most Likely: 3h
- Pessimistic: 5h
- Expected: 3.17h

#### Status

📋 Todo

---

### T23 - EXE 编译与发布包

#### Description

编译为独立 EXE 并创建发布包。

**具体工作**:
- 使用 Ahk2Exe 编译脚本
- 打包 RapidOCR 相关文件 (已有资源)
- 创建 config.ini 模板
- 创建发布 ZIP 包

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md) - 部署
- **External**: `RapidOCR-AutoHotkey/` - 已有库和模型

#### Priority

P2: Medium

#### Dependencies

T22

#### Acceptance Criteria

- [ ] **Functional**: EXE 可独立运行
- [ ] **Functional**: 发布包完整 (含 RapidOCR)
- [ ] **Documentation**: 配置模板清晰

#### Estimated Effort

- Optimistic: 1h
- Most Likely: 2h
- Pessimistic: 4h
- Expected: 2.17h

#### Status

📋 Todo

---

### T24 - 文档与使用说明

#### Description

编写用户文档和开发文档。

**具体工作**:
- README.md - 项目简介和快速开始
- USER_GUIDE.md - 用户使用指南
  - 安装步骤
  - 配置说明
  - 常见问题
- DEVELOPER.md - 开发者文档
  - 架构说明
  - 模块接口
  - 扩展指南
- CHANGELOG.md - 版本变更记录

#### Reference Documents

- **Design Doc**: [auto_design.md](auto_design.md)
- **External**: `RapidOCR-AutoHotkey/README.md`

#### Priority

P2: Medium

#### Dependencies

T23

#### Acceptance Criteria

- [ ] **Documentation**: README 包含安装和使用说明
- [ ] **Documentation**: 配置项说明完整
- [ ] **Documentation**: 常见问题解答
- [ ] **Quality**: 文档格式规范

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
| v1.1 | 2026-03-23 | Re-generated from auto_design.md v1.6 | Task count reduced from 24 to 22, effort optimized |
| v1.2 | 2026-03-23 | Added T10 OCR缓存, T24 文档任务; Updated RapidOCR info | Task count 24, added External Resources section |
