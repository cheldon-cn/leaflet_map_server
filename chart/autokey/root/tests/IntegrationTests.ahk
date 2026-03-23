#Requires AutoHotkey v2.0
#SingleInstance Force

#Include ..\src\ConfigManager.ahk
#Include ..\src\Logger.ahk
#Include ..\src\Utils.ahk
#Include ..\src\WindowDetector.ahk
#Include ..\src\OCRWrapper.ahk
#Include ..\src\OCRDetector.ahk
#Include ..\src\KeywordDetector.ahk
#Include ..\src\AutoContinueExecutor.ahk
#Include ..\src\StateMachine.ahk
#Include ..\src\RetryManager.ahk
#Include ..\src\ExceptionHandler.ahk

SetWorkingDir(A_ScriptDir "\..")

class TestLogger {
    static logFile := ""
    static initialized := false
    
    static Initialize() {
        this.logFile := A_WorkingDir "\log\test_integration.log"
        
        if !DirExist(A_WorkingDir "\log")
            DirCreate(A_WorkingDir "\log")
        
        try {
            FileDelete(this.logFile)
        } catch {
        }
        this.initialized := true
        this.Log("========== 集成测试日志 ==========")
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

class IntegrationTests {
    static passed := 0
    static failed := 0
    static startTime := 0
    
    static Run() {
        this.startTime := A_TickCount
        TestLogger.Initialize()
        
        TestLogger.Log("========== 开始集成测试 ==========")
        TestLogger.Log("")
        
        this.TestFullWorkflow()
        this.TestOCRIntegration()
        this.TestOCRDetector()
        this.TestWindowDetection()
        this.TestExceptionHandler()
        
        elapsed := A_TickCount - this.startTime
        TestLogger.Log("")
        TestLogger.Log("========== 集成测试完成 ==========")
        TestLogger.Log(Format("通过: {}, 失败: {}", this.passed, this.failed))
        TestLogger.Log(Format("耗时: {} ms", elapsed))
        TestLogger.Log("")
        TestLogger.Log(Format("日志文件: {}", TestLogger.logFile))
        
        OCRWrapper.Shutdown()
        
        if (this.failed > 0) {
            MsgBox(Format("集成测试失败: {} / {}`n`n日志文件: {}", this.failed, this.passed + this.failed, TestLogger.logFile), "集成测试结果", 16)
        } else {
            MsgBox(Format("所有集成测试通过: {} 个`n`n日志文件: {}", this.passed, TestLogger.logFile), "集成测试结果", 64)
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
    
    static TestFullWorkflow() {
        TestLogger.Log("--- 测试完整工作流 ---")
        
        ConfigManager.LoadDefaults()
        
        KeywordDetector.Initialize(Map(
            "Keywords", ConfigManager.Get("Detection", "Keywords", "模型思考次数已达上限")
        ))
        
        StateMachine.Initialize(Map(
            "CooldownPeriod", 100,
            "CheckInterval", 50,
            "PostExecutionDelay", 50
        ))
        
        RetryManager.Initialize(Map(
            "MaxRetries", 2,
            "RetryDelay", 50,
            "ConsecutiveFailuresThreshold", 3
        ))
        
        this.Assert(StateMachine.GetCurrentState() = "COOLDOWN", "工作流: 初始状态")
        
        Sleep(150)
        StateMachine.Update()
        this.Assert(StateMachine.GetCurrentState() = "DETECTING", "工作流: 进入检测状态")
        
        text := "模型思考次数已达上限, 请输入继续"
        this.Assert(KeywordDetector.HasKeyword(text), "工作流: 关键词检测")
        
        StateMachine.OnDetected()
        this.Assert(StateMachine.GetCurrentState() = "POST_EXECUTION", "工作流: 进入执行后状态")
        
        TestLogger.Log("")
    }
    
    static TestOCRIntegration() {
        TestLogger.Log("--- 测试 OCR 集成 ---")
        
        try {
            initialized := OCRWrapper.Initialize(Map(
                "Model", "zh_hans"
            ))
            
            this.Assert(initialized, "OCR: 初始化成功")
            this.Assert(OCRWrapper.IsInitialized(), "OCR: 状态检查")
            this.Assert(OCRWrapper.IsAvailable(), "OCR: 可用性检查")
            
            if (initialized) {
                TestLogger.Log("OCR 引擎已就绪, 模型: zh_hans")
            }
        } catch as e {
            TestLogger.Log("[ERROR] OCR 测试异常: " e.Message)
            this.failed++
        }
        
        TestLogger.Log("")
    }
    
    static TestOCRDetector() {
        TestLogger.Log("--- 测试 OCRDetector ---")
        
        try {
            OCRDetector.Initialize(Map(
                "ConfidenceThreshold", 0.5,
                "OCRRegion", 9,
                "CacheEnabled", true
            ))
            
            this.Assert(OCRDetector.confidenceThreshold = 0.5, "OCRDetector: 置信度阈值设置")
            this.Assert(OCRDetector.ocrRegion = 9, "OCRDetector: OCR 区域设置")
            this.Assert(OCRDetector.cacheEnabled = true, "OCRDetector: 缓存启用")
            
            winRect := {x: 0, y: 0, w: 900, h: 600}
            region := OCRDetector.CalculateOCRRegion(winRect, 9)
            this.Assert(region.x = 600 && region.y = 400, "OCRDetector: 区域计算")
            
            region0 := OCRDetector.CalculateOCRRegion(winRect, 0)
            this.Assert(region0.w = 900 && region0.h = 600, "OCRDetector: 全屏区域")
            
            mockResults := [
                Map("text", "测试文本1", "score", 0.95, "box", Map()),
                Map("text", "测试文本2", "score", 0.75, "box", Map()),
                Map("text", "测试文本3", "score", 0.45, "box", Map())
            ]
            
            filtered := OCRDetector.FilterResults(mockResults, 0.5)
            this.Assert(filtered.Length = 2, "OCRDetector: 结果过滤")
            
            text := OCRDetector.ExtractText(filtered)
            this.Assert(InStr(text, "测试文本1") > 0, "OCRDetector: 文本提取")
            
            cacheStats := OCRDetector.GetCacheStats()
            this.Assert(cacheStats.enabled = true, "OCRDetector: 缓存状态")
            
        } catch as e {
            TestLogger.Log("[ERROR] OCRDetector 测试异常: " e.Message)
            this.failed++
        }
        
        TestLogger.Log("")
    }
    
    static TestWindowDetection() {
        TestLogger.Log("--- 测试窗口检测 ---")
        
        WindowDetector.Initialize(Map(
            "TargetWindow", "ahk_exe Trae CN.exe"
        ))
        
        winID := WindowDetector.DetectTraeWindow()
        
        if (winID != 0) {
            rect := WindowDetector.GetWindowRect(winID)
            this.Assert(rect.w > 0 && rect.h > 0, "窗口检测: 获取窗口尺寸")
            TestLogger.Log(Format("窗口尺寸: {}x{}", rect.w, rect.h))
        } else {
            TestLogger.Log("[SKIP] 窗口检测: Trae 窗口未运行")
            this.passed++
        }
        
        TestLogger.Log("")
    }
    
    static TestExceptionHandler() {
        TestLogger.Log("--- 测试异常处理器 ---")
        
        ExceptionHandler.ResetErrorCount()
        
        try {
            throw Error("测试异常")
        } catch as e {
            ExceptionHandler.Handle(e, "测试上下文")
        }
        
        this.Assert(ExceptionHandler.GetErrorCount() = 1, "异常处理器: 错误计数")
        this.Assert(ExceptionHandler.GetLastError() != "", "异常处理器: 记录最后错误")
        
        ExceptionHandler.ResetErrorCount()
        this.Assert(ExceptionHandler.GetErrorCount() = 0, "异常处理器: 重置错误计数")
        
        TestLogger.Log("")
    }
}

IntegrationTests.Run()
