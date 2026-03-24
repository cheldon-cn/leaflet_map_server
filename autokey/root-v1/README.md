# Trae IDE 自动继续工具

基于 AutoHotkey V1 开发的 Trae IDE 自动继续工具，用于自动检测并点击"继续"按钮。

## 功能特性

- **OCR 文字识别**: 使用 RapidOCR 进行屏幕文字识别
- **关键词检测**: 自动检测"模型思考次数已达上限"等关键词
- **自动执行**: 自动点击输入框、输入文本、点击发送按钮
- **重试机制**: 支持指数退避重试策略
- **心跳检测**: 监控脚本运行状态
- **看门狗**: 防止脚本卡死
- **异常处理**: 完善的异常处理和恢复机制

## 系统要求

- Windows 10/11
- AutoHotkey v1.1.33+ (已安装于 `D:\Program Files\AutoHotKey`)

## 目录结构

```
root-v1/
├── src/                          ; 源代码目录
│   ├── ConfigManager.ahk         ; 配置管理模块
│   ├── Logger.ahk                ; 日志系统模块
│   ├── Utils.ahk                 ; 辅助函数库
│   ├── WindowDetector.ahk        ; 窗口检测模块
│   ├── OCRDetector.ahk           ; OCR 识别模块
│   ├── KeywordDetector.ahk       ; 关键词检测模块
│   ├── AutoContinueExecutor.ahk  ; 自动继续执行模块
│   ├── RetryManager.ahk          ; 重试管理模块
│   ├── HeartbeatMonitor.ahk      ; 心跳检测模块
│   ├── Watchdog.ahk              ; 看门狗模块
│   ├── Main.ahk                  ; 主程序入口
│   ├── ExceptionHandler.ahk      ; 异常处理模块
│   ├── TestRunner.ahk            ; 集成测试脚本
│   └── Compile.ahk               ; 编译打包脚本
├── lib/                          ; 第三方库目录
│   └── RapidOCR/                 ; RapidOCR-AutoHotkey 库
├── log/                          ; 日志目录 (运行时生成)
│   ├── trae_auto.log             ; 日志文件
│   └── watchdog.txt              ; 看门狗文件
├── config.ini                    ; 配置文件
└── README.md                     ; 使用说明
```

## 安装

1. 确保已安装 AutoHotkey v1.1.33+
2. 克隆或下载本项目
3. 运行 `src\Main.ahk`

## 配置

配置文件 `config.ini` 包含以下设置:

### [General] - 通用设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| DetectInterval | 检测间隔 (毫秒) | 10000 |
| CooldownTime | 冷却时间 (毫秒) | 900000 (15分钟) |
| PostExecutionWait | 执行后等待时间 (毫秒) | 30000 |
| EnableAutoContinue | 是否启用自动继续 | 1 |
| DebugMode | 是否启用调试模式 | 0 |

### [Window] - 窗口设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| WindowTitle | Trae IDE 窗口标题匹配字符串 | Trae |

### [OCR] - OCR 设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| OCREngine | OCR 引擎类型 | RapidOCR |
| OCRTimeout | OCR 识别超时 (毫秒) | 3000 |
| ConfidenceThreshold | OCR 置信度阈值 | 60 |
| EnableOCRCache | 是否启用 OCR 缓存 | 1 |
| OCRCacheExpiry | OCR 缓存有效期 (毫秒) | 60000 |

### [Keywords] - 关键词设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| PromptKeywords | 检测关键词 (逗号分隔) | 模型思考次数已达上限,思考次数已达上限,已达上限 |
| InputText | 输入文本 | 继续 |

### [Retry] - 重试设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| MaxRetryCount | 最大重试次数 | 3 |
| RetryBaseDelay | 重试基础延迟 (毫秒) | 1000 |
| RetryMaxDelay | 重试最大延迟 (毫秒) | 30000 |
| ConsecutiveFailureThreshold | 连续失败阈值 | 5 |

### [Log] - 日志设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| LogLevel | 日志级别 (0=DEBUG, 1=INFO, 2=WARN, 3=ERROR) | 1 |
| MaxLogSize | 最大日志文件大小 (字节) | 10485760 |
| LogBackupCount | 日志备份数量 | 5 |

### [Watchdog] - 看门狗设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| WatchdogTimeout | 看门狗超时 (毫秒) | 60000 |

### [Heartbeat] - 心跳设置

| 配置项 | 说明 | 默认值 |
|--------|------|--------|
| HeartbeatInterval | 心跳间隔 (毫秒) | 5000 |
| HeartbeatTimeout | 心跳超时 (毫秒) | 30000 |

## 使用方法

### 运行脚本

双击 `src\Main.ahk` 或在命令行运行:

```bash
"D:\Program Files\AutoHotKey\AutoHotkey.exe" "path\to\root-v1\src\Main.ahk"
```

### 托盘菜单

右键托盘图标可访问以下功能:

- **显示状态**: 显示当前运行状态
- **暂停/继续**: 暂停或恢复脚本
- **重置计数**: 重置所有计数器
- **查看日志**: 打开日志文件
- **退出**: 退出脚本

### 快捷键

| 快捷键 | 功能 |
|--------|------|
| Ctrl+Alt+P | 暂停/继续 |
| Ctrl+Alt+S | 显示状态 |
| Ctrl+Alt+R | 重置计数器 |

### 运行测试

运行集成测试:

```bash
"D:\Program Files\AutoHotKey\AutoHotkey.exe" "path\to\root-v1\src\TestRunner.ahk"
```

### 编译打包

运行编译脚本:

```bash
"D:\Program Files\AutoHotKey\AutoHotkey.exe" "path\to\root-v1\src\Compile.ahk"
```

编译完成后，将在 `root-v1` 目录下生成 `TraeAutoContinue.exe`。

## 状态机

脚本使用状态机管理运行流程:

```
COOLDOWN (冷却期) -> DETECTING (检测期) -> POST_EXECUTION (执行后等待) -> COOLDOWN
```

1. **COOLDOWN**: 冷却期，等待配置的冷却时间
2. **DETECTING**: 检测期，执行 OCR 识别和关键词检测
3. **POST_EXECUTION**: 执行后等待，等待配置的等待时间

## 异常处理

脚本支持以下异常类型的处理:

| 异常类型 | 说明 | 恢复策略 |
|----------|------|----------|
| OCR_ERROR | OCR 识别错误 | 重新初始化或使用像素检测备选 |
| WINDOW_ERROR | 窗口错误 | 等待窗口或重新检测 |
| EXECUTION_ERROR | 执行错误 | 重试 |
| CONFIG_ERROR | 配置错误 | 使用默认配置 |
| UNKNOWN_ERROR | 未知错误 | 记录日志 |

## 日志

日志文件位于 `log\trae_auto.log`，支持以下级别:

- **DEBUG** (0): 调试信息
- **INFO** (1): 一般信息
- **WARN** (2): 警告信息
- **ERROR** (3): 错误信息

## 注意事项

1. 确保 Trae IDE 窗口标题包含配置的 `WindowTitle` 字符串
2. 首次运行时建议启用 `DebugMode` 以查看详细日志
3. 如遇到 OCR 识别问题，可调整 `ConfidenceThreshold` 参数
4. 连续失败达到阈值时，脚本会自动重启

## 许可证

MIT License

## 更新日志

### v1.0.0 (2024-01-01)

- 初始版本
- 支持 OCR 文字识别
- 支持关键词检测
- 支持自动继续执行
- 支持重试机制
- 支持心跳检测和看门狗
- 支持异常处理
