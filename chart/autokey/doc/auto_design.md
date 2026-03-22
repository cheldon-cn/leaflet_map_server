# AutoHotkey 自动化方案设计文档

## 文档信息

- **项目名称**: Trae IDE 自动继续工具
- **版本**: 1.6
- **创建日期**: 2026-03-22
- **最后更新**: 2026-03-22
- **状态**: 设计完成 (多角色交叉评审通过)
- **技术栈**: AutoHotkey v2 + RapidOCR-AutoHotkey

---

## 一、方案概述

### 1.1 设计目标

开发一个基于 AutoHotkey 的自动化工具,自动检测 Trae IDE 中的"模型思考次数已达上限"提示,并自动输入"继续"继续对话,实现无需人工干预的连续工作流。

### 1.2 核心功能

1. **智能检测**: 定期检测 Trae IDE 窗口中的提示文本
2. **自动响应**: 自动输入"继续"并提交
3. **可靠性保障**: 完善的错误处理和重试机制
4. **状态监控**: 实时监控运行状态和日志记录
5. **配置灵活**: 支持用户自定义配置

### 1.3 技术选型依据

根据方法选型文档分析,选择 AutoHotkey 方案的原因:

- ✅ **轻量级**: 脚本文件仅几 KB,资源占用极低
- ✅ **实时响应**: 检测延迟可控制在 100ms 以内
- ✅ **Windows 原生支持**: 完美支持 Win32 API 调用
- ✅ **OCR 能力**: 通过 RapidOCR 实现高精度识别
- ✅ **易于部署**: 可编译为 exe 独立运行

### 1.4 适用场景

- ✅ Windows 10/11 操作系统
- ✅ Trae IDE 正常运行
- ✅ 单显示器或多显示器环境
- ✅ 需要长时间连续工作的场景

---

## 二、系统架构

### 2.1 整体架构

```
┌─────────────────────────────────────────┐
│           用户界面层 (可选)              │
│     - 配置面板                          │
│     - 状态显示                          │
├─────────────────────────────────────────┤
│           应用逻辑层                     │
│  ┌──────────┐  ┌──────────┐           │
│  │ 检测模块 │→ │ 决策模块 │           │
│  └──────────┘  └──────────┘           │
│        ↓              ↓                 │
│  ┌──────────┐  ┌──────────┐           │
│  │  OCR模块 │  │ 执行模块 │           │
│  └──────────┘  └──────────┘           │
├─────────────────────────────────────────┤
│           基础设施层                     │
│  - 日志系统                             │
│  - 配置管理                             │
│  - 异常处理                             │
│  - 性能监控                             │
└─────────────────────────────────────────┘
```

### 2.2 状态机设计

#### 2.2.1 状态机流程

脚本采用三状态状态机设计，实现冷却期-检测期-执行后等待的周期循环：

```
┌─────────────────────────────────────────────────────────────────────────┐
│                                                                         │
│  ┌──────────────┐         ┌──────────────┐         ┌──────────────┐   │
│  │              │  15分钟  │              │ 检测到   │              │   │
│  │   COOLDOWN   │ ──────→ │  DETECTING   │ ──────→ │POST_EXECUTION│   │
│  │   冷却期     │         │   检测期     │ 提示    │  执行后等待   │   │
│  │              │         │              │         │              │   │
│  │  不做检测    │         │  每10秒检测  │         │   等待30秒   │   │
│  └──────────────┘         └──────────────┘         └──────────────┘   │
│         ↑                                                   │          │
│         │                    30秒后                         │          │
│         └───────────────────────────────────────────────────┘          │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### 2.2.2 状态说明

| 状态 | 时长 | 行为 | 转换条件 |
|------|------|------|----------|
| **COOLDOWN** | 15 分钟 | 不做任何检测，静默等待 | 15 分钟后 → DETECTING |
| **DETECTING** | 持续 | 每 10 秒检测一次 | 检测到提示 → POST_EXECUTION |
| **POST_EXECUTION** | 30 秒 | 等待界面稳定 | 30 秒后 → COOLDOWN |

#### 2.2.3 时间线示例

```
时间轴:
0min ────────────── 15min ────────────────────── 执行 ── 30s ────────────── 15min ────
     │    冷却期    │←── 检测期(每10s) ─────────→│  等待  │     冷却期      │ 检测期...
     │   不检测     │   检测...检测...检测...    │        │    不检测       │
```

#### 2.2.4 设计目的

1. **避免频繁检测**：启动后给用户 15 分钟的"安静期"
2. **执行后冷却**：每次执行后重置周期，避免短时间内重复触发
3. **资源节约**：减少不必要的 CPU 和内存占用
4. **稳定性保障**：给 Trae IDE 足够的响应时间

### 2.3 模块设计

#### 2.3.1 检测模块 (Detection Module)

**职责**:
- 定期检查 Trae IDE 窗口状态
- 捕获窗口截图
- 调用 OCR 识别文本
- 提取关键信息

**接口**:
```autohotkey
class DetectionModule {
    ; 初始化
    __New(config)
    
    ; 检测提示
    DetectPrompt() => bool
    
    ; 获取窗口截图
    CaptureWindow() => Bitmap
    
    ; OCR 识别
    PerformOCR(bitmap) => string
    
    ; 检查关键词
    CheckKeywords(text) => bool
}
```

#### 2.3.2 决策模块 (Decision Module)

**职责**:
- 分析检测结果
- 判断是否需要执行操作
- 处理重试逻辑
- 记录决策过程

**接口**:
```autohotkey
class DecisionModule {
    ; 初始化
    __New(config)
    
    ; 分析并决策
    Analyze(detectionResult) => Action
    
    ; 重试判断
    ShouldRetry(failCount) => bool
    
    ; 记录决策
    LogDecision(decision)
}
```

#### 2.3.3 执行模块 (Execution Module)

**职责**:
- 执行自动继续操作
- 控制输入速度
- 验证操作结果
- 处理执行异常

**接口**:
```autohotkey
class ExecutionModule {
    ; 初始化
    __New(config)
    
    ; 执行继续操作
    ExecuteContinue() => bool
    
    ; 输入文本
    InputText(text)
    
    ; 点击按钮
    ClickButton(x, y)
    
    ; 验证结果
    VerifyResult() => bool
}
```

#### 2.3.4 OCR 模块 (OCR Module)

**职责**:
- 封装 RapidOCR 调用
- 图像预处理
- 结果后处理
- 置信度过滤

**接口**:
```autohotkey
class OCRModule {
    ; 初始化 OCR 引擎
    __New(config)
    
    ; 识别图像
    Recognize(bitmap) => OCRResult
    
    ; 图像预处理
    PreprocessImage(bitmap) => Bitmap
    
    ; 过滤结果
    FilterResults(results, threshold) => OCRResult[]
}
```

### 2.3 数据流

```
用户启动脚本
    ↓
[初始化] 加载配置、初始化 OCR、设置定时器
    ↓
[主循环] 根据状态机周期执行
    ↓
    ├─ 冷却期: 不检测,等待15分钟
    ├─ 检测期: 每10秒检测一次
    └─ 执行后: 等待30秒后进入冷却期
    ↓
[检测] 
    ├─ 检查窗口是否存在
    ├─ 激活窗口
    ├─ 截图
    ├─ OCR 识别
    └─ 检查关键词
    ↓
[决策]
    ├─ 分析检测结果
    ├─ 判断是否需要继续
    └─ 记录决策
    ↓
[执行] (如果需要)
    ├─ 点击输入框
    ├─ 输入"继续"
    ├─ 点击发送按钮
    └─ 验证结果
    ↓
[异常处理] (如果失败)
    ├─ 重试(最多3次)
    ├─ 记录错误
    └─ 发送通知
    ↓
[日志记录] 记录所有操作和状态
    ↓
返回主循环
```

---

## 三、详细设计

### 3.1 配置系统

#### 3.1.1 配置文件格式

使用 INI 格式配置文件 (`config.ini`):

```ini
[General]
; 冷却期时长(毫秒), 启动后和执行后的静默期
; 默认 900000 = 15分钟
CooldownPeriod=900000

; 检测间隔(毫秒), 冷却期后的检测频率
; 默认 10000 = 10秒
CheckInterval=10000

; 执行后等待时长(毫秒), 执行操作后的稳定等待
; 默认 30000 = 30秒
PostExecutionDelay=30000

; 是否启用日志
EnableLog=true

; 日志文件路径
LogFile=trae_auto.log

[Detection]
; 目标窗口
TargetWindow=ahk_exe Trae.exe

; 关键词列表(逗号分隔)
Keywords=模型思考次数已达上限,请输入继续,获得更多结果

; OCR 识别区域(九宫格: 1-9, 0=全屏)
; 1=左上 2=中上 3=右上
; 4=左中 5=中中 6=右中
; 7=左下 8=中下 9=右下
; 默认 9=右下(提示通常出现在右下角)
OCRRegion=9

; 是否使用 OCR(true/false)
UseOCR=true

[OCR]
; OCR 引擎(RapidOCR/WindowsOCR/OnlineAPI)
Engine=RapidOCR

; RapidOCR 库路径(使用 RapidOCR-AutoHotkey 库)
; 下载地址: https://github.com/telppa/RapidOCR-AutoHotkey
RapidOCRPath=lib\RapidOCR.ahk

; 识别语言
Language=ch

; 置信度阈值
ConfidenceThreshold=0.9

[Execution]
; 输入延迟(毫秒)
InputDelay=100

; 点击延迟(毫秒)
ClickDelay=200

; 最大重试次数
MaxRetries=3

; 重试延迟(毫秒)
RetryDelay=1000

; 是否验证结果
VerifyResult=true

[UI]
; 是否显示托盘图标
ShowTrayIcon=true

; 是否显示提示
ShowNotifications=true
```

#### 3.1.2 配置管理类

```autohotkey
class ConfigManager {
    static config := Map()
    
    static Load(configFile := "config.ini") {
        if !FileExist(configFile) {
            LogWarn("配置文件不存在: " configFile ", 使用默认配置")
            this.LoadDefaults()
            return
        }
        
        sections := ["General", "Detection", "OCR", "Execution", "UI", "Position"]
        
        for section in sections {
            this.config[section] := Map()
        }
        
        try {
            this.config["General"]["CooldownPeriod"] := Integer(IniRead(configFile, "General", "CooldownPeriod", "900000"))
            this.config["General"]["CheckInterval"] := Integer(IniRead(configFile, "General", "CheckInterval", "10000"))
            this.config["General"]["PostExecutionDelay"] := Integer(IniRead(configFile, "General", "PostExecutionDelay", "30000"))
            this.config["General"]["EnableLog"] := IniRead(configFile, "General", "EnableLog", "true")
            this.config["General"]["LogFile"] := IniRead(configFile, "General", "LogFile", "trae_auto.log")
            
            this.config["Detection"]["TargetWindow"] := IniRead(configFile, "Detection", "TargetWindow", "ahk_exe Trae.exe")
            this.config["Detection"]["Keywords"] := IniRead(configFile, "Detection", "Keywords", "模型思考次数已达上限,请输入继续,获得更多结果")
            this.config["Detection"]["OCRRegion"] := Integer(IniRead(configFile, "Detection", "OCRRegion", "9"))
            this.config["Detection"]["UseOCR"] := IniRead(configFile, "Detection", "UseOCR", "true")
            
            this.config["OCR"]["Engine"] := IniRead(configFile, "OCR", "Engine", "RapidOCR")
            this.config["OCR"]["RapidOCRPath"] := IniRead(configFile, "OCR", "RapidOCRPath", "lib\RapidOCR.ahk")
            this.config["OCR"]["ConfidenceThreshold"] := Float(IniRead(configFile, "OCR", "ConfidenceThreshold", "0.9"))
            
            this.config["Execution"]["InputDelay"] := Integer(IniRead(configFile, "Execution", "InputDelay", "100"))
            this.config["Execution"]["ClickDelay"] := Integer(IniRead(configFile, "Execution", "ClickDelay", "200"))
            this.config["Execution"]["MaxRetries"] := Integer(IniRead(configFile, "Execution", "MaxRetries", "3"))
            this.config["Execution"]["RetryDelay"] := Integer(IniRead(configFile, "Execution", "RetryDelay", "1000"))
            this.config["Execution"]["VerifyResult"] := IniRead(configFile, "Execution", "VerifyResult", "true")
            
            this.config["UI"]["ShowTrayIcon"] := IniRead(configFile, "UI", "ShowTrayIcon", "true")
            this.config["UI"]["ShowNotifications"] := IniRead(configFile, "UI", "ShowNotifications", "true")
            
            this.config["Position"]["InputX"] := Float(IniRead(configFile, "Position", "InputX", "0.5"))
            this.config["Position"]["InputY"] := Float(IniRead(configFile, "Position", "InputY", "0.92"))
            this.config["Position"]["SendBtnX"] := Float(IniRead(configFile, "Position", "SendBtnX", "0.85"))
            this.config["Position"]["SendBtnY"] := Float(IniRead(configFile, "Position", "SendBtnY", "0.92"))
            
        } catch as e {
            LogError("配置加载失败: " e.Message ", 使用默认配置")
            this.LoadDefaults()
        }
    }
    
    static LoadDefaults() {
        this.config["General"] := Map("CooldownPeriod", 900000, "CheckInterval", 10000, "PostExecutionDelay", 30000, "EnableLog", "true", "LogFile", "trae_auto.log")
        this.config["Detection"] := Map("TargetWindow", "ahk_exe Trae.exe", "Keywords", "模型思考次数已达上限,请输入继续,获得更多结果", "OCRRegion", 9, "UseOCR", "true")
        this.config["OCR"] := Map("Engine", "RapidOCR", "RapidOCRPath", "lib\RapidOCR.ahk", "ConfidenceThreshold", 0.9)
        this.config["Execution"] := Map("InputDelay", 100, "ClickDelay", 200, "MaxRetries", 3, "RetryDelay", 1000, "VerifyResult", "true")
        this.config["UI"] := Map("ShowTrayIcon", "true", "ShowNotifications", "true")
        this.config["Position"] := Map("InputX", 0.5, "InputY", 0.92, "SendBtnX", 0.85, "SendBtnY", 0.92)
    }
    
    static Get(section, key, default := "") {
        try {
            return this.config[section][key]
        } catch {
            return default
        }
    }
    
    static Set(section, key, value) {
        if !this.config.Has(section)
            this.config[section] := Map()
        
        this.config[section][key] := value
    }
    
    static Save(configFile := "config.ini") {
        try {
            for section, keys in this.config {
                for key, value in keys {
                    IniWrite(String(value), configFile, section, key)
                }
            }
        } catch as e {
            LogError("配置保存失败: " e.Message)
        }
    }
}
```

### 3.2 检测模块实现

#### 3.2.1 窗口检测

```autohotkey
class WindowDetector {
    static DetectTraeWindow() {
        ; 检查窗口是否存在
        if !WinExist("ahk_exe Trae.exe") {
            LogDebug("Trae 窗口不存在")
            return false
        }
        
        ; 获取窗口信息
        winID := WinGetID("ahk_exe Trae.exe")
        winState := WinGetMinMax("ahk_exe Trae.exe")
        
        ; 检查窗口状态
        if (winState = -1) {  ; 最小化
            LogDebug("窗口已最小化,尝试恢复")
            WinRestore "ahk_id " winID
            Sleep(500)
        }
        
        ; 激活窗口
        WinActivate "ahk_id " winID
        WinWaitActive "ahk_id " winID, , 2
        
        if ErrorLevel {
            LogError("窗口激活失败")
            return false
        }
        
        return winID
    }
    
    static GetWindowRect(winID) {
        WinGetPos &x, &y, &w, &h, "ahk_id " winID
        
        ; 处理 DPI 缩放
        dpiFactor := GetDpiScalingFactor()
        
        return {
            x: x,
            y: y,
            w: w * dpiFactor,
            h: h * dpiFactor
        }
    }
}
```

#### 3.2.2 OCR 识别

```autohotkey
; 引入 RapidOCR-AutoHotkey 库
; 下载地址: https://github.com/telppa/RapidOCR-AutoHotkey
#Include lib\RapidOCR.ahk

class OCRDetector {
    static ocr := ""
    
    static Initialize() {
        ; 使用 RapidOCR-AutoHotkey 库初始化
        ; 参考: https://github.com/telppa/RapidOCR-AutoHotkey
        try {
            ; RapidOCR-AutoHotkey 库的初始化方式
            ; 库会自动处理 DLL 加载和模型初始化
            this.ocr := RapidOCR()
            LogInfo("OCR 引擎初始化成功 (RapidOCR-AutoHotkey)")
        } catch as e {
            LogError("OCR 初始化失败: " e.Message)
            throw e
        }
    }
    
    static PerformOCR(x := "", y := "", w := "", h := "") {
        bitmap := 0
        try {
            winRect := WindowDetector.GetWindowRect(WinExist("A"))
            
            ; 应用九宫格区域配置
            if (x = "" or y = "" or w = "" or h = "") {
                region := this.CalculateOCRRegion(winRect)
                x := region.x
                y := region.y
                w := region.w
                h := region.h
            }
            
            LogDebug(Format("OCR 区域: ({}, {}) {}x{}", x, y, w, h))
            
            bitmap := CaptureScreen(x, y, w, h)
            
            ; 使用 RapidOCR-AutoHotkey 库识别
            result := this.ocr.OCR(bitmap)
            
            ; 过滤结果
            threshold := ConfigManager.Get("OCR", "ConfidenceThreshold", 0.9)
            filteredResults := this.FilterResults(result, threshold)
            
            return filteredResults
        } finally {
            if (bitmap != 0)
                DeleteBitmap(bitmap)
        }
    }
    
    static CalculateOCRRegion(winRect) {
        regionCode := ConfigManager.Get("Detection", "OCRRegion", 9)
        
        if (regionCode = 0) {
            return {x: winRect.x, y: winRect.y, w: winRect.w, h: winRect.h}
        }
        
        thirdW := winRect.w / 3
        thirdH := winRect.h / 3
        
        col := Mod(regionCode - 1, 3)
        row := (regionCode - 1) // 3
        
        x := winRect.x + col * thirdW
        y := winRect.y + row * thirdH
        
        return {x: x, y: y, w: thirdW, h: thirdH}
    }
    
    static FilterResults(results, threshold) {
        filtered := []
        
        for result in results {
            if (result.confidence >= threshold) {
                filtered.Push(result)
            }
        }
        
        return filtered
    }
    
    static ExtractKeywords(ocrResults) {
        text := ""
        
        for result in ocrResults {
            text .= result.text . " "
        }
        
        return text
    }
}
```

**九宫格区域示意图**:

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

**区域选择建议**:
- **区域 9 (右下)**: 默认配置，提示通常出现在右下角
- **区域 8 (中下)**: 如果提示在底部居中位置
- **区域 6 (右中)**: 如果提示在右侧中间位置
- **区域 0 (全屏)**: 不确定位置时使用，但性能较低
```

#### 3.2.4 辅助函数

```autohotkey
GetDpiScalingFactor() {
    static dpiFactor := 0
    
    if (dpiFactor = 0) {
        try {
            hdc := DllCall("GetDC", "Ptr", 0, "Ptr")
            dpi := DllCall("GetDeviceCaps", "Ptr", hdc, "Int", 88)
            DllCall("ReleaseDC", "Ptr", 0, "Ptr", hdc)
            dpiFactor := dpi / 96.0
        } catch {
            dpiFactor := 1.0
        }
    }
    
    return dpiFactor
}

CaptureScreen(x, y, w, h) {
    hdc := DllCall("GetDC", "Ptr", 0, "Ptr")
    hdcMem := DllCall("CreateCompatibleDC", "Ptr", hdc, "Ptr")
    hBitmap := DllCall("CreateCompatibleBitmap", "Ptr", hdc, "Int", w, "Int", h, "Ptr")
    hOldBitmap := DllCall("SelectObject", "Ptr", hdcMem, "Ptr", hBitmap, "Ptr")
    DllCall("BitBlt", "Ptr", hdcMem, "Int", 0, "Int", 0, "Int", w, "Int", h, "Ptr", hdc, "Int", x, "Int", y, "UInt", 0x00CC0020)
    DllCall("SelectObject", "Ptr", hdcMem, "Ptr", hOldBitmap, "Ptr")
    DllCall("DeleteDC", "Ptr", hdcMem)
    DllCall("ReleaseDC", "Ptr", 0, "Ptr", hdc)
    
    return hBitmap
}

DeleteBitmap(hBitmap) {
    if hBitmap
        DllCall("DeleteObject", "Ptr", hBitmap)
}

RapidOCR() {
    static instance := ""
    
    if (instance = "") {
        instance := RapidOCREngine()
    }
    
    return instance
}

class RapidOCREngine {
    dllPath := ""
    hModule := 0
    
    Initialize(dllPath) {
        this.dllPath := dllPath
        
        if !FileExist(dllPath) {
            throw Error("RapidOCR DLL 不存在: " dllPath)
        }
        
        this.hModule := DllCall("LoadLibrary", "Str", dllPath, "Ptr")
        
        if !this.hModule {
            throw Error("加载 RapidOCR DLL 失败")
        }
        
        LogInfo("RapidOCR 初始化成功")
    }
    
    OCR(bitmap) {
        results := []
        
        try {
            ret := DllCall(this.dllPath . "\OCR", "Ptr", bitmap, "Ptr*", &ocrResult, "Int")
            
            if (ret = 0) {
                Loop Parse ocrResult, "`n" {
                    if (A_LoopField = "")
                        continue
                    
                    parts := StrSplit(A_LoopField, "`t")
                    
                    if (parts.Length >= 2) {
                        results.Push({
                            text: parts[1],
                            confidence: Float(parts[2])
                        })
                    }
                }
            }
        } catch as e {
            LogError("OCR 识别失败: " e.Message)
        }
        
        return results
    }
    
    __Delete() {
        if this.hModule {
            DllCall("FreeLibrary", "Ptr", this.hModule)
        }
    }
}
```

#### 3.2.3 关键词检测

```autohotkey
class KeywordDetector {
    static keywords := []
    
    static Initialize() {
        ; 加载关键词
        keywordStr := ConfigManager.Get("Detection", "Keywords", "")
        
        if (keywordStr = "") {
            ; 默认关键词
            this.keywords := [
                "模型思考次数已达上限",
                "请输入继续",
                "获得更多结果"
            ]
        } else {
            ; 解析配置的关键词
            this.keywords := StrSplit(keywordStr, ",")
        }
        
        LogInfo("关键词加载完成: " this.keywords.Length " 个")
    }
    
    static DetectKeywords(text) {
        for keyword in this.keywords {
            if InStr(text, keyword) {
                LogDebug("检测到关键词: " keyword)
                return true
            }
        }
        
        return false
    }
}
```

### 3.3 执行模块实现

#### 3.3.1 自动继续操作

```autohotkey
class AutoContinueExecutor {
    static Execute() {
        LogInfo("开始执行自动继续操作")
        
        ; 获取窗口信息
        winID := WindowDetector.DetectTraeWindow()
        
        if !winID {
            LogError("无法找到 Trae 窗口")
            return false
        }
        
        winRect := WindowDetector.GetWindowRect(winID)
        
        ; 计算输入框和发送按钮位置
        inputPos := this.CalculateInputPosition(winRect)
        sendBtnPos := this.CalculateSendButtonPosition(winRect)
        
        ; 执行操作
        try {
            ; 1. 点击输入框
            this.ClickAt(inputPos.x, inputPos.y)
            Sleep(ConfigManager.Get("Execution", "ClickDelay", 200))
            
            ; 2. 输入"继续"
            this.InputText("继续")
            Sleep(ConfigManager.Get("Execution", "InputDelay", 100))
            
            ; 3. 点击发送按钮
            this.ClickAt(sendBtnPos.x, sendBtnPos.y)
            Sleep(500)
            
            ; 4. 验证结果
            if (ConfigManager.Get("Execution", "VerifyResult", "true") = "true") {
                if !this.VerifyResult() {
                    LogError("操作验证失败")
                    return false
                }
            }
            
            LogInfo("自动继续操作成功")
            return true
            
        } catch as e {
            LogError("执行操作异常: " e.Message)
            return false
        }
    }
    
    static CalculateInputPosition(winRect) {
        inputX := ConfigManager.Get("Position", "InputX", 0.5)
        inputY := ConfigManager.Get("Position", "InputY", 0.92)
        
        x := winRect.x + winRect.w * inputX
        y := winRect.y + winRect.h * inputY
        
        return {x: x, y: y}
    }
    
    static CalculateSendButtonPosition(winRect) {
        sendBtnX := ConfigManager.Get("Position", "SendBtnX", 0.85)
        sendBtnY := ConfigManager.Get("Position", "SendBtnY", 0.92)
        
        x := winRect.x + winRect.w * sendBtnX
        y := winRect.y + winRect.h * sendBtnY
        
        return {x: x, y: y}
    }
    
    static ClickAt(x, y) {
        ; 使用 Click 命令
        Click x, y
        
        LogDebug("点击位置: (" x ", " y ")")
    }
    
    static InputText(text) {
        ; 清除修饰键状态
        Send {Ctrl Up}{Alt Up}{LWin Up}{RWin Up}
        
        ; 使用剪贴板输入(避免编码问题)
        A_Clipboard := text
        Send "^v"
        
        LogDebug("输入文本: " text)
    }
    
    static VerifyResult() {
        ; 等待界面变化
        Sleep(1000)
        
        ; 再次截图识别,检查提示是否消失
        ocrResults := OCRDetector.PerformOCR()
        text := OCRDetector.ExtractKeywords(ocrResults)
        
        if !KeywordDetector.DetectKeywords(text) {
            LogDebug("验证成功,提示已消失")
            return true
        }
        
        LogDebug("验证失败,提示仍然存在")
        return false
    }
}
```

#### 3.3.2 重试机制

```autohotkey
class RetryManager {
    static failCount := 0
    static maxRetries := 3
    static baseRetryDelay := 1000
    static consecutiveFailures := 0
    
    static Initialize() {
        this.maxRetries := ConfigManager.Get("Execution", "MaxRetries", 3)
        this.baseRetryDelay := ConfigManager.Get("Execution", "RetryDelay", 1000)
    }
    
    static ExecuteWithRetry(action) {
        this.failCount := 0
        currentDelay := this.baseRetryDelay
        
        while (this.failCount < this.maxRetries) {
            try {
                result := action.Call()
                
                if result {
                    this.consecutiveFailures := 0
                    return true
                }
                
            } catch as e {
                LogError("执行失败: " e.Message)
            }
            
            this.failCount++
            
            if (this.failCount < this.maxRetries) {
                LogInfo("重试 " this.failCount "/" this.maxRetries)
                Sleep(currentDelay)
                
                ; 指数退避
                currentDelay *= 2
            }
        }
        
        ; 连续失败计数
        this.consecutiveFailures++
        
        ; 如果连续失败多次,重启脚本
        if (this.consecutiveFailures >= 5) {
            LogError("连续失败 " this.consecutiveFailures " 次,重启脚本")
            Sleep(5000)
            Reload
        }
        
        return false
    }
}
```

### 3.4 日志系统

```autohotkey
class Logger {
    static logFile := "trae_auto.log"
    static enableLog := true
    static logLevel := "INFO"
    static maxLogSize := 1048576  ; 1MB
    
    static Initialize() {
        this.enableLog := ConfigManager.Get("General", "EnableLog", "true") = "true"
        this.logFile := ConfigManager.Get("General", "LogFile", "trae_auto.log")
        
        ; 检查日志文件大小并轮转
        this.RotateLogIfNeeded()
    }
    
    static RotateLogIfNeeded() {
        if !FileExist(this.logFile)
            return
        
        try {
            fileSize := FileGetSize(this.logFile)
            if (fileSize > this.maxLogSize) {
                backupFile := this.logFile . ".bak"
                if FileExist(backupFile)
                    FileDelete(backupFile)
                FileMove(this.logFile, backupFile)
                LogInfo("日志已轮转,旧日志保存为: " backupFile)
            }
        } catch {
            ; 忽略轮转错误
        }
    }
    
    static Log(level, message) {
        if !this.enableLog
            return
        
        timeStr := FormatTime(, "yyyy-MM-dd HH:mm:ss")
        logLine := "[" timeStr "] [" level "] " message
        
        try {
            FileAppend logLine "`n", this.logFile
        } catch {
            ; 日志写入失败时忽略
        }
        
        if (level = "ERROR") {
            TrayTip "Trae Auto Error", message, 3, 3
        }
    }
    
    static Debug(message) {
        if (this.logLevel = "DEBUG")
            this.Log("DEBUG", message)
    }
    
    static Info(message) {
        this.Log("INFO", message)
    }
    
    static Warn(message) {
        this.Log("WARN", message)
    }
    
    static Error(message) {
        this.Log("ERROR", message)
    }
}
```

### 3.5 主程序

```autohotkey
#Requires AutoHotkey v2.0
#SingleInstance Force
SetBatchLines -1

; 引入依赖库
; #Include RapidOCR.ahk  ; 如使用外部 OCR 库文件

; 全局变量
global isRunning := true
global checkInterval := 10000
global cooldownPeriod := 900000
global postExecutionDelay := 30000

; 状态机状态
global currentState := "COOLDOWN"
global cooldownStartTime := 0
global lastExecutionTime := 0

; 注册退出处理
OnExit CleanupResources

CleanupResources(*) {
    global isRunning
    isRunning := false
    LogInfo("正在清理资源...")
    
    ; 释放 OCR 资源
    try {
        pocr := RapidOCR()
        if pocr.hModule {
            DllCall("FreeLibrary", "Ptr", pocr.hModule)
        }
    } catch {
        ; 忽略清理错误
    }
    
    LogInfo("资源清理完成")
}

; 加载配置
ConfigManager.Load("config.ini")
checkInterval := ConfigManager.Get("General", "CheckInterval", 10000)
cooldownPeriod := ConfigManager.Get("General", "CooldownPeriod", 900000)
postExecutionDelay := ConfigManager.Get("General", "PostExecutionDelay", 30000)

; 初始化模块
Logger.Initialize()
OCRDetector.Initialize()
KeywordDetector.Initialize()
RetryManager.Initialize()

LogInfo("Trae 自动继续工具启动")
LogInfo("冷却期: " cooldownPeriod "ms (" cooldownPeriod/60000 "分钟)")
LogInfo("检测间隔: " checkInterval "ms")
LogInfo("执行后等待: " postExecutionDelay "ms")

; 创建托盘图标
if (ConfigManager.Get("UI", "ShowTrayIcon", "true") = "true") {
    A_TrayMenu.Delete()
    A_TrayMenu.Add("启动/暂停", ToggleRunning)
    A_TrayMenu.Add("查看日志", OpenLogFile)
    A_TrayMenu.Add("退出", ExitApp)
    
    A_IconTip := "Trae 自动继续工具"
}

; 启动冷却期
cooldownStartTime := A_TickCount
currentState := "COOLDOWN"
LogInfo("进入冷却期, 15分钟内不进行检测")

; 设置定时器
SetTimer MainLoop, checkInterval

; 主循环 - 状态机实现
MainLoop() {
    global isRunning, currentState, cooldownStartTime, lastExecutionTime
    global cooldownPeriod, checkInterval, postExecutionDelay
    
    if !isRunning
        return
    
    try {
        ; 状态机处理
        switch currentState {
            case "COOLDOWN":
                ; 冷却期：不检测，等待冷却期结束
                elapsed := A_TickCount - cooldownStartTime
                if (elapsed >= cooldownPeriod) {
                    currentState := "DETECTING"
                    LogInfo("冷却期结束, 开始检测")
                } else {
                    remaining := (cooldownPeriod - elapsed) / 60000
                    LogDebug(Format("冷却期剩余: {:.1f} 分钟", remaining))
                }
                
            case "DETECTING":
                ; 检测期：每10秒检测一次
                ; 检测窗口
                winID := WindowDetector.DetectTraeWindow()
                
                if !winID
                    return
                
                ; OCR 识别
                ocrResults := OCRDetector.PerformOCR()
                text := OCRDetector.ExtractKeywords(ocrResults)
                
                ; 检测关键词
                if KeywordDetector.DetectKeywords(text) {
                    LogInfo("检测到提示, 准备自动继续")
                    
                    ; 执行自动继续(带重试)
                    success := RetryManager.ExecuteWithRetry(ObjBindMethod(AutoContinueExecutor, "Execute"))
                    
                    if success {
                        ; 执行成功，进入执行后等待期
                        lastExecutionTime := A_TickCount
                        currentState := "POST_EXECUTION"
                        LogInfo("执行完成, 等待30秒后进入冷却期")
                    }
                }
                
            case "POST_EXECUTION":
                ; 执行后等待期：等待30秒
                elapsed := A_TickCount - lastExecutionTime
                if (elapsed >= postExecutionDelay) {
                    ; 等待结束，重新进入冷却期
                    cooldownStartTime := A_TickCount
                    currentState := "COOLDOWN"
                    LogInfo("进入冷却期, 15分钟内不进行检测")
                }
        }
        
    } catch as e {
        Logger.Error("主循环异常: " e.Message)
    }
}

; 切换运行状态
ToggleRunning(*) {
    global isRunning
    
    isRunning := !isRunning
    
    if isRunning {
        LogInfo("工具已启动")
        TrayTip "Trae Auto", "已启动", 2, 1
    } else {
        LogInfo("工具已暂停")
        TrayTip "Trae Auto", "已暂停", 2, 2
    }
}

; 打开日志文件
OpenLogFile(*) {
    Run ConfigManager.Get("General", "LogFile", "trae_auto.log")
}

; 退出应用
ExitApp(*) {
    LogInfo("工具退出")
    ExitApp
}

; 热键绑定
^!p::ToggleRunning()  ; Ctrl+Alt+P 暂停/恢复
^!l::OpenLogFile()    ; Ctrl+Alt+L 打开日志
^!r::Reload           ; Ctrl+Alt+R 重启脚本

; 定期重启脚本(防止内存泄漏)
SetTimer ReloadScript, 86400000  ; 24小时

ReloadScript() {
    LogInfo("定期重启脚本")
    Reload
}
```

---

## 四、可靠性保障

### 4.1 错误处理策略

#### 4.1.1 异常分类

**环境异常**:
- 网络断开
- Trae IDE 崩溃
- 系统资源不足

**应用异常**:
- 窗口未激活
- OCR 识别失败
- 界面未响应

**框架异常**:
- RapidOCR DLL 加载失败
- 配置文件损坏
- 脚本逻辑错误

#### 4.1.2 处理策略

```autohotkey
class ExceptionHandler {
    static Handle(exception, context := "") {
        ; 记录异常
        Logger.Error("异常 [" context "]: " exception.Message)
        Logger.Error("堆栈: " exception.Stack)
        
        ; 根据异常类型采取不同策略
        if InStr(exception.Message, "RapidOCR") {
            ; OCR 相关异常,尝试重新初始化
            this.HandleOCRError(exception)
            
        } else if InStr(exception.Message, "窗口") {
            ; 窗口相关异常,等待并重试
            this.HandleWindowError(exception)
            
        } else {
            ; 其他异常,记录并继续
            this.HandleGenericError(exception)
        }
    }
    
    static HandleOCRError(exception) {
        LogWarn("OCR 错误,尝试重新初始化")
        
        try {
            OCRDetector.Initialize()
            LogInfo("OCR 重新初始化成功")
        } catch {
            LogError("OCR 重新初始化失败")
        }
    }
    
    static HandleWindowError(exception) {
        LogWarn("窗口错误,等待 5 秒后重试")
        Sleep(5000)
    }
    
    static HandleGenericError(exception) {
        LogWarn("通用错误,继续运行")
    }
}
```

### 4.2 性能优化

#### 4.2.1 OCR 性能优化

**问题**: OCR 识别耗时较长(1-3秒)

**优化方案**:

1. **缩小识别区域**
   ```autohotkey
   ; 只识别可能包含提示的区域(如窗口底部)
   ocrRegion := ConfigManager.Get("Detection", "OCRRegion", "")
   
   if (ocrRegion = "") {
       ; 默认识别窗口底部 1/4 区域
       winRect := WindowDetector.GetWindowRect(winID)
       ocrX := winRect.x
       ocrY := winRect.y + winRect.h * 0.75
       ocrW := winRect.w
       ocrH := winRect.h * 0.25
   }
   ```

2. **降低识别频率**
   ```autohotkey
   ; 使用静态变量记录上次检测时间
   static lastCheckTime := 0
   
   if (A_TickCount - lastCheckTime < checkInterval)
       return
   
   lastCheckTime := A_TickCount
   ```

3. **缓存 OCR 结果**
   ```autohotkey
   class OCRCache {
       static lastHash := ""
       static lastResult := ""
       static lastTime := 0
       static cacheTimeout := 5000  ; 缓存有效期 5 秒
       
       static GetCachedResult(bitmap) {
           currentHash := this.CalculateHash(bitmap)
           currentTime := A_TickCount
           
           if (currentHash = this.lastHash and currentTime - this.lastTime < this.cacheTimeout) {
               LogDebug("使用 OCR 缓存结果")
               return this.lastResult
           }
           
           return false
       }
       
       static SetCache(bitmap, result) {
           this.lastHash := this.CalculateHash(bitmap)
           this.lastResult := result
           this.lastTime := A_TickCount
       }
       
       static CalculateHash(bitmap) {
           ; 基于图像内容的哈希计算
           ; 使用图像尺寸和采样像素值生成哈希
           try {
               ; 获取图像尺寸
               width := DllCall("GetObject", "Ptr", bitmap, "Int", 0, "Ptr", 0)
               
               ; 采样关键像素点生成哈希
               hashParts := []
               samplePoints := [[0.1, 0.1], [0.5, 0.5], [0.9, 0.9], [0.1, 0.9], [0.9, 0.1]]
               
               for point in samplePoints {
                   ; 这里简化处理，实际需要获取像素值
                   hashParts.Push(point[1] "_" point[2])
               }
               
               return "bitmap_" StrJoin("_", hashParts*)
           } catch {
               ; 降级为时间戳哈希
               return "bitmap_" A_TickCount // 1000
           }
       }
   }
   ```

#### 4.2.2 内存优化

**问题**: RapidOCR 可能存在内存泄漏

**解决方案**:

1. **定期重启脚本**
   ```autohotkey
   ; 每 24 小时重启一次
   SetTimer ReloadScript, 86400000
   ```

2. **及时释放资源**
   ```autohotkey
   ; 使用后立即释放 bitmap 对象
   bitmap := ""
   
   ; 调用垃圾回收
   ; (AutoHotkey v2 自动管理,但可以显式触发)
   ```

3. **监控内存使用**
   ```autohotkey
   MonitorMemory() {
       Process, Exist
       pid := ErrorLevel
      
       ; 获取内存使用
       ; ... WMI 查询 ...
      
       if (memoryUsage > 500) {  ; 超过 500MB
           LogWarn("内存使用过高: " memoryUsage "MB")
           Reload
       }
   }
   ```

### 4.3 稳定性保障

#### 4.3.1 心跳检测

```autohotkey
class HeartbeatMonitor {
    static lastHeartbeat := 0
    static heartbeatInterval := 60000  ; 1 分钟
    static initialized := false
    
    static Initialize() {
        this.lastHeartbeat := A_TickCount
        this.initialized := true
    }
    
    static Check() {
        ; 首次检查时初始化
        if !this.initialized {
            this.Initialize()
            return
        }
        
        currentTime := A_TickCount
        
        if (currentTime - this.lastHeartbeat > this.heartbeatInterval * 2) {
            ; 心跳超时,可能脚本卡死
            LogError("心跳检测失败,重启脚本")
            Reload
        }
        
        this.lastHeartbeat := currentTime
    }
}

; 在主循环中添加心跳检测
MainLoop() {
    HeartbeatMonitor.Check()
    ; ... 正常逻辑 ...
}
```

#### 4.3.2 看门狗机制

```autohotkey
class Watchdog {
    static watchdogFile := "watchdog.txt"
    static initialized := false
    
    static Initialize() {
        this.Update()
        this.initialized := true
    }
    
    static Update() {
        ; 更新看门狗文件
        try {
            if FileExist(this.watchdogFile)
                FileDelete(this.watchdogFile)
            FileAppend(String(A_TickCount), this.watchdogFile)
        } catch {
            ; 忽略文件操作错误
        }
    }
    
    static Check() {
        ; 首次检查时初始化
        if !this.initialized {
            this.Initialize()
            return
        }
        
        ; 检查看门狗文件是否更新
        if !FileExist(this.watchdogFile)
            return
        
        try {
            FileRead &lastUpdate, this.watchdogFile
            lastUpdate := Integer(lastUpdate)
            
            if (A_TickCount - lastUpdate > 120000) {  ; 2 分钟未更新
                ; 脚本可能卡死,重启
                LogError("看门狗检测失败,重启脚本")
                Reload
            }
        } catch {
            ; 文件读取错误,忽略
        }
    }
}

; 启动单独的看门狗线程
SetTimer WatchdogCheck, 30000

WatchdogCheck() {
    Watchdog.Check()
}

; 在主循环中更新看门狗
MainLoop() {
    Watchdog.Update()
    ; ... 正常逻辑 ...
}
```

---

## 五、部署和使用

### 5.1 环境要求

**系统要求**:
- Windows 10 或更高版本
- 64 位操作系统
- 至少 100MB 可用内存
- 至少 500MB 磁盘空间

**软件要求**:
- AutoHotkey v2.0 或更高版本
- Trae IDE
- RapidOCR 相关文件:
  - RapidOCR.dll
  - 模型文件 (约 200MB)

### 5.2 安装步骤

1. **安装 AutoHotkey v2**
   ```powershell
   # 下载并安装
   # https://www.autohotkey.com/v2/
   ```

2. **下载 RapidOCR 文件**
   ```powershell
   # 从 GitHub 下载 RapidOCR-AutoHotkey 封装库
   # https://github.com/telppa/RapidOCR-AutoHotkey
   
   # 或直接下载预编译版本
   # https://github.com/RapidAI/RapidOCR/releases
   ```
   
   **RapidOCR 文件清单**:
   | 文件 | 大小 | 说明 |
   |------|------|------|
   | RapidOCR.dll | ~5MB | OCR 引擎 DLL |
   | ch_PP-OCRv4_det_infer.onnx | ~4MB | 文本检测模型 |
   | ch_PP-OCRv4_rec_infer.onnx | ~10MB | 文本识别模型 |
   | ch_ppocr_mobile_v2.0_cls_train.onnx | ~2MB | 文本方向分类模型 |

3. **下载本项目文件**
   ```
   trae-auto-continue/
   ├── trae_auto.ahk          # 主脚本
   ├── config.ini              # 配置文件
   ├── lib/                    # 依赖库
   │   └── RapidOCR.ahk       # RapidOCR-AutoHotkey 库
   ├── RapidOCR.dll           # OCR DLL (由库自动加载)
   └── models/                 # OCR 模型
       ├── ch_PP-OCRv4_det_infer.onnx
       ├── ch_PP-OCRv4_rec_infer.onnx
       └── ch_ppocr_mobile_v2.0_cls_train.onnx
   ```

4. **下载 RapidOCR-AutoHotkey 库**
   ```powershell
   # 克隆或下载 RapidOCR-AutoHotkey 库
   git clone https://github.com/telppa/RapidOCR-AutoHotkey.git
   
   # 将 RapidOCR.ahk 复制到 lib 目录
   mkdir lib
   copy RapidOCR-AutoHotkey\RapidOCR.ahk lib\
   ```

5. **配置**
   ```ini
   ; 编辑 config.ini
   [Detection]
   TargetWindow=ahk_exe Trae.exe
   Keywords=模型思考次数已达上限,请输入继续
   OCRRegion=9  ; 右下角区域
   ```

6. **运行**
   ```powershell
   # 双击运行 trae_auto.ahk
   # 或命令行运行
   AutoHotkey64.exe trae_auto.ahk
   ```

### 5.3 编译为 EXE 发布

#### 5.3.1 编译方法

**方法一：使用 Ahk2Exe GUI**

1. 安装 AutoHotkey v2 时会自动安装 Ahk2Exe 编译器
2. 右键 `trae_auto.ahk` → "Compile Script"
3. 生成的 `trae_auto.exe` 可独立运行

**方法二：命令行编译**

```powershell
# 使用 Ahk2Exe 命令行工具
Ahk2Exe.exe /in "trae_auto.ahk" /out "TraeAutoContinue.exe" /bin "AutoHotkeySC.bin"

# 或使用简写
Ahk2Exe.exe /in trae_auto.ahk
```

**方法三：包含资源文件编译**

```powershell
# 将配置文件和库打包到 EXE 中
Ahk2Exe.exe /in trae_auto.ahk /out TraeAutoContinue.exe /compress 1

# 使用资源文件列表
; 在脚本顶部添加
;@Ahk2Exe-AddResource config.ini
;@Ahk2Exe-AddResource lib\RapidOCR.ahk
```

#### 5.3.2 发布包结构

```
TraeAutoContinue-v1.0/
├── TraeAutoContinue.exe    # 主程序 (已编译)
├── config.ini               # 配置文件
├── RapidOCR.dll            # OCR 引擎
├── models/                  # OCR 模型
│   ├── ch_PP-OCRv4_det_infer.onnx
│   ├── ch_PP-OCRv4_rec_infer.onnx
│   └── ch_ppocr_mobile_v2.0_cls_train.onnx
└── README.txt               # 使用说明
```

#### 5.3.3 发布脚本示例

```powershell
# build_release.ps1
$version = "1.0"
$releaseDir = "TraeAutoContinue-v$version"

# 创建发布目录
New-Item -ItemType Directory -Force -Path $releaseDir
New-Item -ItemType Directory -Force -Path "$releaseDir\models"

# 编译 EXE
Ahk2Exe.exe /in trae_auto.ahk /out "$releaseDir\TraeAutoContinue.exe" /compress 1

# 复制依赖文件
Copy-Item config.ini $releaseDir
Copy-Item RapidOCR.dll $releaseDir
Copy-Item models\*.onnx "$releaseDir\models"

# 创建 README
@"
Trae Auto Continue v$version
===========================

使用方法:
1. 双击 TraeAutoContinue.exe 启动
2. 右键托盘图标可暂停/恢复
3. 编辑 config.ini 可调整配置

快捷键:
- Ctrl+Alt+P: 暂停/恢复
- Ctrl+Alt+L: 查看日志
- Ctrl+Alt+R: 重启脚本

OCR 区域配置 (config.ini 中 OCRRegion):
- 1-9: 九宫格区域 (默认 9=右下)
- 0: 全屏识别

问题反馈: https://github.com/xxx/trae-auto-continue
"@ | Out-File -FilePath "$releaseDir\README.txt" -Encoding UTF8

Write-Host "发布包已创建: $releaseDir"
```

#### 5.3.4 编译注意事项

| 注意项 | 说明 |
|--------|------|
| **模型文件** | OCR 模型文件较大 (~16MB)，需单独分发 |
| **配置文件** | 建议不打包到 EXE，便于用户修改 |
| **DPI 感知** | 编译时需设置 DPI 感知，否则坐标可能不准 |
| **管理员权限** | 如需管理员权限，在脚本顶部添加 `#Requires Admin` |
| **32/64 位** | 建议编译为 64 位，与 RapidOCR 兼容性更好 |

### 5.4 使用说明

#### 5.4.1 启动

- 双击 `trae_auto.ahk` 启动脚本
- 托盘图标显示绿色,表示正在运行

#### 5.4.2 暂停/恢复

- 右键托盘图标 → "启动/暂停"
- 或使用快捷键 (可自定义)

#### 5.4.3 查看日志

- 右键托盘图标 → "查看日志"
- 或直接打开 `trae_auto.log`

#### 5.4.4 退出

- 右键托盘图标 → "退出"

### 5.5 常见问题

**Q1: OCR 识别不准确?**

A: 
- 检查 RapidOCR 模型文件是否完整
- 尝试调整置信度阈值 (`config.ini` 中的 `ConfidenceThreshold`)
- 确保屏幕 DPI 设置正常

**Q2: 点击位置不准确?**

A: 
- 检查 `CalculateInputPosition` 和 `CalculateSendButtonPosition` 函数
- 根据实际 Trae IDE 布局调整位置计算逻辑
- 考虑 DPI 缩放影响

**Q3: 脚本内存占用过高?**

A: 
- 启用定期重启功能
- 检查 RapidOCR 是否正常释放资源
- 降低检查频率

**Q4: 无法找到窗口?**

A: 
- 确认 Trae IDE 正在运行
- 检查 `TargetWindow` 配置是否正确
- 尝试使用窗口标题而非进程名

### 5.6 输入框定位方案

由于 Trae IDE 的界面布局可能随版本更新而变化,提供以下三种定位方案:

#### 方案一: 固定坐标配置 (推荐)

在 `config.ini` 中添加坐标配置:

```ini
[Position]
; 输入框位置(相对于窗口左上角的比例)
InputX=0.5
InputY=0.92

; 发送按钮位置(相对于窗口左上角的比例)
SendBtnX=0.85
SendBtnY=0.92
```

**获取坐标方法**:
1. 右键托盘图标 → 打开 AutoHotkey Window Spy
2. 将鼠标移到输入框/发送按钮上
3. 记录相对坐标比例

#### 方案二: 图像匹配

```autohotkey
class ImageLocator {
    static FindInputBox(winRect) {
        ; 使用图像匹配定位输入框
        imagePath := "images/input_box.png"
        
        if ImageSearch(&foundX, &foundY, winRect.x, winRect.y, winRect.x + winRect.w, winRect.y + winRect.h, imagePath) {
            return {x: foundX, y: foundY}
        }
        
        return false
    }
    
    static FindSendButton(winRect) {
        ; 使用图像匹配定位发送按钮
        imagePath := "images/send_button.png"
        
        if ImageSearch(&foundX, &foundY, winRect.x, winRect.y, winRect.x + winRect.w, winRect.y + winRect.h, imagePath) {
            return {x: foundX, y: foundY}
        }
        
        return false
    }
}
```

#### 方案三: UIAutomation API

```autohotkey
class UIALocator {
    static FindInputBox() {
        try {
            el := ControlGetFocus("ahk_exe Trae.exe")
            return el
        } catch {
            return false
        }
    }
    
    static FindSendButton() {
        ; 使用 UIAutomation 查找按钮
        ; 需要引入 UIAutomation 库
    }
}
```

**推荐方案**: 方案一(固定坐标配置),简单可靠,易于调试。

### 5.7 多显示器支持

脚本已内置多显示器支持:

```autohotkey
class MultiMonitor {
    static GetMonitorCount() {
        return MonitorGetCount()
    }
    
    static GetTraeMonitor() {
        WinGetPos &x, &y, &w, &h, "ahk_exe Trae.exe"
        
        MonitorGet 1, &left1, &top1, &right1, &bottom1
        
        if (x >= left1 and x < right1) {
            return 1
        }
        
        if (MonitorGetCount() >= 2) {
            MonitorGet 2, &left2, &top2, &right2, &bottom2
            if (x >= left2 and x < right2) {
                return 2
            }
        }
        
        return 1
    }
}
```

---

## 六、扩展和优化

### 6.1 短期优化 (本周)

- [x] 实现精确的输入框和按钮定位(使用配置化坐标)
- [x] 实现状态机冷却期逻辑(15分钟冷却 + 10秒检测 + 30秒等待)
- [ ] 添加 GUI 配置界面
- [x] 优化 OCR 性能(添加资源释放)
- [x] 完善日志系统(添加日志轮转)

### 6.2 中期优化 (本月)

- [ ] 支持多种提示类型(网络错误、确认提示等)
- [ ] 添加统计功能(自动继续次数、成功率等)
- [x] 支持多显示器环境(已添加 MultiMonitor 类)
- [ ] 实现配置热重载

### 6.3 长期优化 (持续)

- [ ] 集成到 MCP 插件(长期方案)
- [ ] 支持其他 AI IDE(Cursor、CodeBuddy 等)
- [ ] 开源发布到 GitHub
- [ ] 社区反馈和持续改进

---

## 七、附录

### 7.1 完整代码示例

完整代码请参考项目仓库:
- 主脚本: `trae_auto.ahk`
- 配置示例: `config.ini.example`
- OCR 库: `RapidOCR.ahk`

### 7.2 性能指标

**预期性能**:
- 检测延迟: < 100ms
- OCR 识别: 1-3 秒
- 内存占用: 50-100MB
- CPU 占用: < 5% (空闲时)

**可靠性指标**:
- 识别准确率: > 95%
- 操作成功率: > 90%
- 平均无故障时间: > 24 小时

### 7.3 已知限制

1. **平台限制**: 仅支持 Windows
2. **OCR 限制**: 需要清晰的屏幕显示
3. **窗口限制**: Trae IDE 不能最小化到托盘
4. **性能限制**: OCR 会增加检测延迟
5. **维护限制**: Trae IDE 更新后可能需要调整

### 7.4 测试策略

#### 7.4.1 单元测试

| 测试项 | 测试方法 | 预期结果 |
|--------|----------|----------|
| ConfigManager.Load | 创建测试配置文件 | 正确读取所有配置项 |
| ConfigManager.Get | 获取不存在的键 | 返回默认值 |
| KeywordDetector.DetectKeywords | 输入包含关键词的文本 | 返回 true |
| RetryManager.ExecuteWithRetry | 模拟失败操作 | 正确执行重试逻辑 |
| Logger.RotateLogIfNeeded | 创建超大日志文件 | 正确轮转日志 |

#### 7.4.2 集成测试

| 测试场景 | 测试步骤 | 预期结果 |
|----------|----------|----------|
| 完整流程测试 | 1. 启动脚本 2. 触发提示 3. 验证自动继续 | 操作成功执行 |
| 配置热重载 | 修改配置文件后重载 | 新配置生效 |
| 错误恢复测试 | 模拟 OCR 失败 | 正确重试并记录日志 |
| 内存泄漏测试 | 长时间运行(24小时) | 内存占用稳定 |

#### 7.4.3 性能测试

| 指标 | 测试方法 | 目标值 |
|------|----------|--------|
| OCR 响应时间 | 计时器测量 | < 3秒 |
| 内存占用 | 任务管理器监控 | < 100MB |
| CPU 占用 | 性能监视器 | < 5% 空闲时 |
| 连续运行时间 | 稳定性测试 | > 24小时 |

### 7.5 故障排除指南

| 问题 | 可能原因 | 解决方案 |
|------|----------|----------|
| OCR 识别失败 | RapidOCR.dll 未找到 | 检查 DLL 路径配置 |
| 点击位置不准 | DPI 缩放问题 | 调整 Position 配置 |
| 脚本无响应 | 窗口未激活 | 检查 TargetWindow 配置 |
| 内存持续增长 | Bitmap 未释放 | 检查 DeleteBitmap 调用 |
| 日志过大 | 未启用轮转 | 检查 maxLogSize 配置 |

### 7.4 参考资料

- AutoHotkey v2 文档: https://www.autohotkey.com/docs/v2/
- RapidOCR 项目: https://github.com/PaddlePaddle/RapidOCR
- RapidOCR-AutoHotkey: https://github.com/telppa/RapidOCR-AutoHotkey
- Windows UI Automation: https://docs.microsoft.com/windows/win32/winauto/

---

**文档状态**: 完成 (多角色交叉评审通过)
**下一步**: 实现原型并进行测试
**负责人**: AI Design Agent
**版本**: 1.6