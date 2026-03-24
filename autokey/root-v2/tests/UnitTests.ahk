#Requires AutoHotkey v2.0
#SingleInstance Force

#Include ..\src\ConfigManager.ahk
#Include ..\src\Logger.ahk
#Include ..\src\Utils.ahk
#Include ..\src\KeywordDetector.ahk
#Include ..\src\GridRegion.ahk
#Include ..\src\StateMachine.ahk
#Include ..\src\RetryManager.ahk

SetWorkingDir(A_ScriptDir "\..")

class TestLogger {
    static logFile := ""
    static initialized := false
    
    static Initialize() {
        this.logFile := A_WorkingDir "\log\test_unit.log"
        
        if !DirExist(A_WorkingDir "\log")
            DirCreate(A_WorkingDir "\log")
        
        try {
            FileDelete(this.logFile)
        } catch {
        }
        this.initialized := true
        this.Log("========== 单元测试日志 ==========")
    }
    
    static Log(msg) {
        timestamp := FormatTime(, "yyyy-MM-dd HH:mm:ss")
        logLine := Format("[{}] {}", timestamp, msg)
        
        OutputDebug(logLine)
        
        if this.initialized {
            try {
                file := FileOpen(this.logFile, "a", "UTF-8")
                if (file) {
                    file.Write(logLine "`n")
                    file.Close()
                }
            } catch {
            }
        }
    }
}

class TestRunner {
    static passed := 0
    static failed := 0
    static tests := []
    static startTime := 0
    
    static Run() {
        this.startTime := A_TickCount
        TestLogger.Initialize()
        
        TestLogger.Log("========== 开始单元测试 ==========")
        TestLogger.Log("")
        
        this.TestConfigManager()
        this.TestKeywordDetector()
        this.TestGridRegion()
        this.TestStateMachine()
        this.TestRetryManager()
        this.TestUtils()
        
        elapsed := A_TickCount - this.startTime
        TestLogger.Log("")
        TestLogger.Log("========== 测试完成 ==========")
        TestLogger.Log(Format("通过: {}, 失败: {}, 总计: {}", this.passed, this.failed, this.passed + this.failed))
        TestLogger.Log(Format("耗时: {} ms", elapsed))
        TestLogger.Log("")
        TestLogger.Log(Format("日志文件: {}", TestLogger.logFile))
        
        if (this.failed > 0) {
            MsgBox(Format("测试失败: {} / {}`n`n日志文件: {}", this.failed, this.passed + this.failed, TestLogger.logFile), "测试结果", 16)
        } else {
            MsgBox(Format("所有测试通过: {} 个`n`n日志文件: {}", this.passed, TestLogger.logFile), "测试结果", 64)
        }
    }
    
    static Assert(condition, testName) {
        if (condition) {
            this.passed++
            TestLogger.Log(Format("[PASS] {}", testName))
        } else {
            this.failed++
            TestLogger.Log(Format("[FAIL] {}", testName))
        }
    }
    
    static TestConfigManager() {
        TestLogger.Log("--- 测试 ConfigManager ---")
        
        ConfigManager.LoadDefaults()
        
        this.Assert(ConfigManager.Get("General", "CooldownPeriod", 0) = 900000, "ConfigManager: 默认冷却期")
        this.Assert(ConfigManager.Get("General", "CheckInterval", 0) = 10000, "ConfigManager: 默认检测间隔")
        this.Assert(ConfigManager.Get("Detection", "TargetWindow", "") = "ahk_exe Trae CN.exe", "ConfigManager: 默认目标窗口")
        this.Assert(ConfigManager.Get("OCR", "ConfidenceThreshold", 0) = 0.9, "ConfigManager: 默认置信度阈值")
        
        ConfigManager.Set("Test", "Key", "Value")
        this.Assert(ConfigManager.Get("Test", "Key", "") = "Value", "ConfigManager: 设置配置")
        
        keywords := ConfigManager.GetKeywords()
        this.Assert(keywords.Length > 0, "ConfigManager: 获取关键词列表")
        
        TestLogger.Log("")
    }
    
    static TestKeywordDetector() {
        TestLogger.Log("--- 测试 KeywordDetector ---")
        
        KeywordDetector.Initialize(Map("Keywords", "测试关键词,另一个关键词"))
        
        this.Assert(KeywordDetector.HasKeyword("这是一个测试关键词的文本"), "KeywordDetector: 检测关键词存在")
        this.Assert(!KeywordDetector.HasKeyword("这段文本没有关键词"), "KeywordDetector: 检测关键词不存在")
        
        found := KeywordDetector.DetectKeywords("测试关键词和另一个关键词都在这里")
        this.Assert(found.Length = 2, "KeywordDetector: 检测多个关键词")
        
        first := KeywordDetector.DetectFirstKeyword("这里有测试关键词")
        this.Assert(first = "测试关键词", "KeywordDetector: 检测第一个关键词")
        
        TestLogger.Log("")
    }
    
    static TestGridRegion() {
        TestLogger.Log("--- 测试 GridRegion ---")
        
        winRect := {x: 0, y: 0, w: 900, h: 600}
        
        region0 := GridRegion.Calculate(winRect, 0)
        this.Assert(region0.w = 900 && region0.h = 600, "GridRegion: 全屏区域")
        
        region1 := GridRegion.Calculate(winRect, 1)
        this.Assert(region1.x = 0 && region1.y = 0, "GridRegion: 左上区域")
        
        region9 := GridRegion.Calculate(winRect, 9)
        this.Assert(region9.x = 600 && region9.y = 400, "GridRegion: 右下区域")
        
        name := GridRegion.GetRegionName(9)
        this.Assert(name = "右下", "GridRegion: 区域名称")
        
        pos := GridRegion.GetRelativePosition(winRect, 750, 500)
        this.Assert(pos = 9, "GridRegion: 相对位置计算")
        
        TestLogger.Log("")
    }
    
    static TestStateMachine() {
        TestLogger.Log("--- 测试 StateMachine ---")
        
        StateMachine.Initialize(Map(
            "CooldownPeriod", 1000,
            "CheckInterval", 100,
            "PostExecutionDelay", 500
        ))
        
        this.Assert(StateMachine.GetCurrentState() = "COOLDOWN", "StateMachine: 初始状态为冷却期")
        
        Sleep(1100)
        StateMachine.Update()
        this.Assert(StateMachine.GetCurrentState() = "DETECTING", "StateMachine: 冷却期后进入检测期")
        
        StateMachine.OnDetected()
        this.Assert(StateMachine.GetCurrentState() = "POST_EXECUTION", "StateMachine: 检测后进入执行后等待")
        
        Sleep(600)
        StateMachine.Update()
        this.Assert(StateMachine.GetCurrentState() = "COOLDOWN", "StateMachine: 执行后等待后回到冷却期")
        
        TestLogger.Log("")
    }
    
    static TestRetryManager() {
        TestLogger.Log("--- 测试 RetryManager ---")
        
        RetryManager.Initialize(Map(
            "MaxRetries", 3,
            "RetryDelay", 100,
            "ConsecutiveFailuresThreshold", 2
        ))
        
        callCount := 0
        successCallback := () => (callCount++, true)
        
        RetryManager.ResetConsecutiveFailures()
        result := RetryManager.ExecuteWithRetry(successCallback)
        this.Assert(result = true, "RetryManager: 成功回调返回 true")
        this.Assert(RetryManager.GetStats().consecutiveFailures = 0, "RetryManager: 成功后重置连续失败")
        
        callCount := 0
        failCallback := () => (callCount++, false)
        result := RetryManager.ExecuteWithRetry(failCallback, 2)
        this.Assert(result = false, "RetryManager: 失败回调返回 false")
        this.Assert(callCount = 2, "RetryManager: 重试次数正确")
        
        TestLogger.Log("")
    }
    
    static TestUtils() {
        TestLogger.Log("--- 测试 Utils ---")
        
        dpi := Utils.GetDpiScalingFactor()
        this.Assert(dpi > 0, "Utils: DPI 缩放因子 > 0")
        
        joined := Utils.StrJoin(["a", "b", "c"], ",")
        this.Assert(joined = "a,b,c", "Utils: 字符串连接")
        
        clamped := Utils.Clamp(150, 0, 100)
        this.Assert(clamped = 100, "Utils: Clamp 上限")
        
        clamped := Utils.Clamp(-10, 0, 100)
        this.Assert(clamped = 0, "Utils: Clamp 下限")
        
        mapped := Utils.MapRange(50, 0, 100, 0, 1000)
        this.Assert(mapped = 500, "Utils: MapRange")
        
        TestLogger.Log("")
    }
}

TestRunner.Run()
