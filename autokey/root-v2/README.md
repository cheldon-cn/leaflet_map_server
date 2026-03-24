# Trae IDE 自动继续工具

自动检测 Trae IDE 中的"模型思考次数已达上限"提示，并自动输入"继续"继续对话。

## 功能特性

- **智能检测**: 定期检测 Trae IDE 窗口中的提示文本
- **自动响应**: 自动输入"继续"并提交
- **可靠性保障**: 完善的错误处理和重试机制
- **状态监控**: 实时监控运行状态和日志记录
- **配置灵活**: 支持用户自定义配置
- **OCR 缓存**: 内置缓存机制提升性能
- **多语言支持**: 支持中英日韩多语言 OCR 模型

## 目录结构

```
root/
├── src/                    # 源代码目录
│   ├── ConfigManager.ahk   # 配置管理
│   ├── Logger.ahk          # 日志系统 (UTF-8)
│   ├── Utils.ahk           # 辅助函数 (截图、DPI等)
│   ├── WindowDetector.ahk  # 窗口检测
│   ├── OCRWrapper.ahk      # OCR 封装 (RapidOCR-json.exe)
│   ├── OCRDetector.ahk     # OCR 识别与缓存
│   ├── GridRegion.ahk      # 九宫格区域
│   ├── KeywordDetector.ahk # 关键词检测
│   ├── AutoContinueExecutor.ahk # 执行模块
│   ├── RetryManager.ahk    # 重试管理
│   ├── ExceptionHandler.ahk # 异常处理
│   ├── StateMachine.ahk    # 状态机
│   ├── HeartbeatMonitor.ahk # 心跳检测
│   ├── Watchdog.ahk        # 看门狗
│   ├── ResultVerifier.ahk  # 结果验证
│   └── Main.ahk            # 主程序
├── lib/                    # 第三方库
│   └── RapidOCR/           # RapidOCR-AutoHotkey 库
│       ├── RapidOCR.ahk    # 主库文件
│       ├── exe/            # 可执行文件
│       │   ├── RapidOCR-json.exe
│       │   └── models/     # ONNX 模型文件
│       │       ├── ch_PP-OCRv4_det_infer.onnx  # 文本检测模型
│       │       ├── ch_PP-OCRv4_rec_infer.onnx  # 文本识别模型
│       │       ├── ch_ppocr_mobile_v2.0_cls_infer.onnx  # 方向分类模型
│       │       └── dict_chinese.txt  # 中文字典
│       └── Lib/            # 依赖库
│           ├── ImagePut.ahk
│           ├── NonNull.ahk
│           └── cJson.ahk
├── config/                 # 配置文件
│   └── config.ini          # 主配置文件
├── log/                    # 日志目录
│   ├── trae_auto.log       # 运行日志
│   ├── test_unit.log       # 单元测试日志
│   └── test_integration.log # 集成测试日志
├── tests/                  # 测试文件
│   ├── UnitTests.ahk       # 单元测试 (28 项)
│   └── IntegrationTests.ahk # 集成测试 (19 项)
└── TraeAutoContinue.ahk    # 主入口脚本
```

## 快速开始

### 前置要求

- Windows 10/11
- AutoHotkey v2.0+
- Trae IDE (Trae CN.exe)

### 安装

1. 确保已安装 AutoHotkey v2.0
2. 将整个 `root/` 目录复制到任意位置
3. 运行 `TraeAutoContinue.ahk`

### 配置

编辑 `config/config.ini` 文件：

```ini
[General]
CooldownPeriod=900000      ; 冷却期时长 (毫秒), 默认 15 分钟
CheckInterval=10000        ; 检测间隔 (毫秒), 默认 10 秒
PostExecutionDelay=30000   ; 执行后等待 (毫秒), 默认 30 秒

[Detection]
TargetWindow=ahk_exe Trae CN.exe  ; 目标窗口
Keywords=模型思考次数已达上限,请输入继续,获得更多结果  ; 关键词
OCRRegion=9                ; OCR 区域 (1-9 九宫格, 0=全屏)

[OCR]
Engine=RapidOCR            ; OCR 引擎
ConfidenceThreshold=0.9    ; 置信度阈值

[Execution]
InputDelay=100             ; 输入延迟 (毫秒)
ClickDelay=200             ; 点击延迟 (毫秒)
MaxRetries=3               ; 最大重试次数
ConsecutiveFailuresThreshold=5  ; 连续失败阈值

[Position]
InputX=0.5                 ; 输入框 X 位置比例
InputY=0.92                ; 输入框 Y 位置比例
SendBtnX=0.85              ; 发送按钮 X 位置比例
SendBtnY=0.92              ; 发送按钮 Y 位置比例
```

## 状态机说明

脚本采用三状态状态机设计：

```
COOLDOWN (冷却期) → DETECTING (检测期) → POST_EXECUTION (执行后等待) → COOLDOWN
```

| 状态 | 时长 | 行为 |
|------|------|------|
| COOLDOWN | 15 分钟 | 不做任何检测 |
| DETECTING | 持续 | 每 10 秒检测一次 |
| POST_EXECUTION | 30 秒 | 等待界面稳定 |

## 九宫格区域

```
┌─────────────┬─────────────┬─────────────┐
│      1      │      2      │      3      │
│    左上     │    中上     │    右上     │
├─────────────┼─────────────┼─────────────┤
│      4      │      5      │      6      │
│    左中     │    中中     │    右中     │
├─────────────┼─────────────┼─────────────┤
│      7      │      8      │      9      │
│    左下     │    中下     │    右下     │ ← 默认区域
└─────────────┴─────────────┴─────────────┘
```

## OCR 模块

### 支持的语言模型

| 模型代码 | 语言 | 说明 |
|----------|------|------|
| zh_hans | 简体中文 | 默认模型 |
| zh_hant | 繁体中文 | |
| en | 英文 | |
| ja | 日文 | |
| ko | 韩文 | |

### OCR 缓存机制

- 默认缓存超时: 5 秒
- 相同窗口区域复用 OCR 结果
- 可通过配置禁用缓存

## 运行测试

```bash
# 运行单元测试 (28 项测试)
AutoHotkey.exe tests\UnitTests.ahk

# 运行集成测试 (19 项测试)
AutoHotkey.exe tests\IntegrationTests.ahk
```

### 测试结果

| 测试类型 | 通过 | 失败 | 总计 |
|----------|------|------|------|
| 单元测试 | 28 | 0 | 28 |
| 集成测试 | 19 | 0 | 19 |

## 使用说明

### 基本使用

1. **启动程序**: 双击 `TraeAutoContinue.ahk` 启动脚本
2. **打开 Trae IDE**: 确保 Trae IDE 窗口已打开
3. **正常使用**: 当检测到"模型思考次数已达上限"提示时，脚本会自动输入"继续"

### 使用示例

#### 示例 1: 默认配置运行

```bash
# 直接运行，使用默认配置
双击 TraeAutoContinue.ahk
```

程序将：
- 每 15 分钟进入检测期
- 检测期内每 10 秒扫描一次右下角区域
- 发现关键词后自动输入"继续"并点击发送

#### 示例 2: 自定义检测间隔

编辑 `config/config.ini`:

```ini
[General]
CheckInterval=5000    ; 改为 5 秒检测一次

[Detection]
OCRRegion=8           ; 改为检测中下区域
```

#### 示例 3: 调整 OCR 置信度

如果 OCR 识别不准确，可以降低置信度阈值：

```ini
[OCR]
ConfidenceThreshold=0.8  ; 从 0.9 降到 0.8
```

#### 示例 4: 添加自定义关键词

```ini
[Detection]
Keywords=模型思考次数已达上限,请输入继续,获得更多结果,继续生成,请点击继续
```

### 托盘菜单

- **状态**: 显示当前运行状态 (冷却期/检测期/执行后等待)
- **重启**: 重启程序
- **退出**: 退出程序

### 日志

日志文件位于 `log/trae_auto.log`，包含：
- 时间戳
- 日志级别 (DEBUG/INFO/WARN/ERROR)
- 详细信息

日志文件使用 UTF-8 编码，支持中文显示。

## 技术实现

### OCRWrapper

直接调用 RapidOCR-json.exe 可执行文件，通过 stdin/stdout 进行 JSON 通信：
- 输入: `{"image_base64": "..."}`
- 输出: `{"code": 100, "data": [{"text": "...", "score": 0.95, "box": [...]}]}`

### OCRDetector

封装 OCR 识别流程，包含：
- 区域截图
- 结果过滤 (置信度阈值)
- 缓存管理
- 文本提取

## 依赖

- [RapidOCR-AutoHotkey](https://github.com/telppa/RapidOCR-AutoHotkey) - OCR 识别库
  - 基于 PaddleOCR 的 ONNX 模型
  - 支持多语言识别

## 许可证

MIT License

## 更新日志

### v1.0.0 (2026-03-23)

**功能完成**
- ✅ 完成所有 24 项任务
- ✅ 单元测试 28/28 通过
- ✅ 集成测试 19/19 通过

**核心功能**
- ✅ OCR 集成完成 (RapidOCR-json.exe)
- ✅ 目标窗口更新为 Trae CN.exe
- ✅ 日志文件 UTF-8 编码支持
- ✅ OCR 缓存机制 (5 秒超时)
- ✅ 九宫格区域检测
- ✅ 状态机 (COOLDOWN → DETECTING → POST_EXECUTION)

**配置文件**
- ✅ config.ini 完整中文注释
- ✅ 支持自定义关键词、检测区域、置信度阈值

**文档**
- ✅ README.md 完整使用说明
- ✅ 目录结构详细说明 (含 RapidOCR 子目录)
- ✅ tasks.md 任务计划文档
