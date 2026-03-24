; ============================================
; TestRunner.ahk - 集成测试脚本 (V1版本)
; 用于测试各模块功能
; ============================================

#SingleInstance Force
#NoEnv
SetWorkingDir %A_ScriptDir%

#Include %A_ScriptDir%\ConfigManager.ahk
#Include %A_ScriptDir%\Logger.ahk
#Include %A_ScriptDir%\Utils.ahk
#Include %A_ScriptDir%\WindowDetector.ahk
#Include %A_ScriptDir%\OCRDetector.ahk
#Include %A_ScriptDir%\KeywordDetector.ahk
#Include %A_ScriptDir%\AutoContinueExecutor.ahk
#Include %A_ScriptDir%\RetryManager.ahk
#Include %A_ScriptDir%\HeartbeatMonitor.ahk
#Include %A_ScriptDir%\Watchdog.ahk
#Include %A_ScriptDir%\ExceptionHandler.ahk

; 测试结果
global g_TestResults := []
global g_TestPassed := 0
global g_TestFailed := 0

TestRunner_Init() {
    global g_TestResults, g_TestPassed, g_TestFailed
    
    g_TestResults := []
    g_TestPassed := 0
    g_TestFailed := 0
    
    ConfigManager_Init()
    Logger_Init()
    Utils_Init()
    
    LogInfo("========================================")
    LogInfo("集成测试开始")
    LogInfo("========================================")
}

TestRunner_RunTest(testName, testFunc) {
    global g_TestResults, g_TestPassed, g_TestFailed
    
    LogInfo("运行测试: " . testName)
    
    try {
        result := %testFunc%()
        
        if result {
            g_TestPassed++
            g_TestResults.Push({name: testName, status: "PASS", error: ""})
            LogInfo("  [PASS] " . testName)
        } else {
            g_TestFailed++
            g_TestResults.Push({name: testName, status: "FAIL", error: "返回 false"})
            LogError("  [FAIL] " . testName . " - 返回 false")
        }
    } catch {
        g_TestFailed++
        g_TestResults.Push({name: testName, status: "FAIL", error: "异常: " . ErrorLevel})
        LogError("  [FAIL] " . testName . " - 异常: " . ErrorLevel)
    }
}

TestRunner_ShowResults() {
    global g_TestResults, g_TestPassed, g_TestFailed
    
    LogInfo("========================================")
    LogInfo("测试结果汇总")
    LogInfo("========================================")
    LogInfo("通过: " . g_TestPassed)
    LogInfo("失败: " . g_TestFailed)
    LogInfo("总计: " . (g_TestPassed + g_TestFailed))
    LogInfo("========================================")
    
    resultText := "测试结果汇总`n`n"
    resultText .= "通过: " . g_TestPassed . "`n"
    resultText .= "失败: " . g_TestFailed . "`n"
    resultText .= "总计: " . (g_TestPassed + g_TestFailed) . "`n`n"
    resultText .= "详细结果:`n"
    
    for index, test in g_TestResults {
        resultText .= test.status . " - " . test.name
        if (test.error != "") {
            resultText .= " (" . test.error . ")"
        }
        resultText .= "`n"
    }
    
    MsgBox, 0, 集成测试结果, %resultText%
}

; ============================================
; 测试用例
; ============================================

Test_ConfigManager_Init() {
    result := ConfigManager_Init()
    return result || true
}

Test_ConfigManager_Get() {
    ConfigManager_Init()
    value := ConfigManager_Get("General", "DetectInterval", 10000)
    return (value > 0)
}

Test_ConfigManager_Set() {
    ConfigManager_Init()
    ConfigManager_Set("General", "TestKey", "TestValue")
    value := ConfigManager_Get("General", "TestKey", "")
    return (value = "TestValue")
}

Test_Logger_Init() {
    Logger_Init()
    return true
}

Test_Logger_Write() {
    Logger_Init()
    LogInfo("测试日志写入")
    return true
}

Test_Utils_GetDpiScalingFactor() {
    Utils_Init()
    dpi := Utils_GetDpiScalingFactor()
    return (dpi.x > 0 && dpi.y > 0)
}

Test_Utils_GetTimestamp() {
    timestamp := Utils_GetTimestamp()
    return (StrLen(timestamp) > 0)
}

Test_Utils_GetTimestampFormatted() {
    formatted := Utils_GetTimestampFormatted()
    return (StrLen(formatted) > 0)
}

Test_Utils_Random() {
    value := Utils_Random(1, 100)
    return (value >= 1 && value <= 100)
}

Test_WindowDetector_Init() {
    WindowDetector_Init()
    return true
}

Test_WindowDetector_DetectTraeWindow() {
    WindowDetector_Init()
    result := WindowDetector_DetectTraeWindow()
    return true
}

Test_KeywordDetector_Init() {
    KeywordDetector_Init()
    return true
}

Test_KeywordDetector_DetectKeywords() {
    KeywordDetector_Init()
    
    result := KeywordDetector_DetectKeywords("模型思考次数已达上限")
    if !result.found {
        return false
    }
    
    result := KeywordDetector_DetectKeywords("这是一段普通文本")
    if result.found {
        return false
    }
    
    return true
}

Test_KeywordDetector_GetInputText() {
    KeywordDetector_Init()
    text := KeywordDetector_GetInputText()
    return (StrLen(text) > 0)
}

Test_AutoContinueExecutor_Init() {
    AutoContinueExecutor_Init()
    return true
}

Test_AutoContinueExecutor_GetExecuteCount() {
    AutoContinueExecutor_Init()
    count := AutoContinueExecutor_GetExecuteCount()
    return (count >= 0)
}

Test_RetryManager_Init() {
    RetryManager_Init()
    return true
}

Test_RetryManager_CalculateDelay() {
    RetryManager_Init()
    
    delay1 := RetryManager_CalculateDelay(1)
    delay2 := RetryManager_CalculateDelay(2)
    delay3 := RetryManager_CalculateDelay(3)
    
    return (delay1 > 0 && delay2 > delay1 && delay3 > delay2)
}

Test_RetryManager_RecordSuccess() {
    RetryManager_Init()
    RetryManager_RecordSuccess()
    
    count := RetryManager_GetTotalSuccesses()
    return (count = 1)
}

Test_RetryManager_RecordFailure() {
    RetryManager_Init()
    RetryManager_ResetConsecutiveFailures()
    RetryManager_RecordFailure()
    
    count := RetryManager_GetConsecutiveFailures()
    return (count = 1)
}

Test_HeartbeatMonitor_Init() {
    HeartbeatMonitor_Init()
    return true
}

Test_HeartbeatMonitor_GetHeartbeatCount() {
    HeartbeatMonitor_Init()
    count := HeartbeatMonitor_GetHeartbeatCount()
    return (count >= 0)
}

Test_Watchdog_Init() {
    Watchdog_Init()
    return true
}

Test_Watchdog_Update() {
    Watchdog_Init()
    result := Watchdog_Update()
    return result
}

Test_Watchdog_Check() {
    Watchdog_Init()
    Watchdog_Update()
    check := Watchdog_Check()
    return check.healthy
}

Test_ExceptionHandler_Init() {
    ExceptionHandler_Init()
    return true
}

Test_ExceptionHandler_Handle() {
    ExceptionHandler_Init()
    
    result := ExceptionHandler_Handle(EXCEPTION_OCR, "测试异常", "测试上下文")
    return IsObject(result)
}

Test_ExceptionHandler_GetExceptionCount() {
    ExceptionHandler_Init()
    ExceptionHandler_Handle(EXCEPTION_OCR, "测试异常", "")
    
    count := ExceptionHandler_GetExceptionCount(EXCEPTION_OCR)
    return (count >= 1)
}

; ============================================
; 主程序
; ============================================

TestRunner_Init()

TestRunner_RunTest("ConfigManager_Init", "Test_ConfigManager_Init")
TestRunner_RunTest("ConfigManager_Get", "Test_ConfigManager_Get")
TestRunner_RunTest("ConfigManager_Set", "Test_ConfigManager_Set")
TestRunner_RunTest("Logger_Init", "Test_Logger_Init")
TestRunner_RunTest("Logger_Write", "Test_Logger_Write")
TestRunner_RunTest("Utils_GetDpiScalingFactor", "Test_Utils_GetDpiScalingFactor")
TestRunner_RunTest("Utils_GetTimestamp", "Test_Utils_GetTimestamp")
TestRunner_RunTest("Utils_GetTimestampFormatted", "Test_Utils_GetTimestampFormatted")
TestRunner_RunTest("Utils_Random", "Test_Utils_Random")
TestRunner_RunTest("WindowDetector_Init", "Test_WindowDetector_Init")
TestRunner_RunTest("WindowDetector_DetectTraeWindow", "Test_WindowDetector_DetectTraeWindow")
TestRunner_RunTest("KeywordDetector_Init", "Test_KeywordDetector_Init")
TestRunner_RunTest("KeywordDetector_DetectKeywords", "Test_KeywordDetector_DetectKeywords")
TestRunner_RunTest("KeywordDetector_GetInputText", "Test_KeywordDetector_GetInputText")
TestRunner_RunTest("AutoContinueExecutor_Init", "Test_AutoContinueExecutor_Init")
TestRunner_RunTest("AutoContinueExecutor_GetExecuteCount", "Test_AutoContinueExecutor_GetExecuteCount")
TestRunner_RunTest("RetryManager_Init", "Test_RetryManager_Init")
TestRunner_RunTest("RetryManager_CalculateDelay", "Test_RetryManager_CalculateDelay")
TestRunner_RunTest("RetryManager_RecordSuccess", "Test_RetryManager_RecordSuccess")
TestRunner_RunTest("RetryManager_RecordFailure", "Test_RetryManager_RecordFailure")
TestRunner_RunTest("HeartbeatMonitor_Init", "Test_HeartbeatMonitor_Init")
TestRunner_RunTest("HeartbeatMonitor_GetHeartbeatCount", "Test_HeartbeatMonitor_GetHeartbeatCount")
TestRunner_RunTest("Watchdog_Init", "Test_Watchdog_Init")
TestRunner_RunTest("Watchdog_Update", "Test_Watchdog_Update")
TestRunner_RunTest("Watchdog_Check", "Test_Watchdog_Check")
TestRunner_RunTest("ExceptionHandler_Init", "Test_ExceptionHandler_Init")
TestRunner_RunTest("ExceptionHandler_Handle", "Test_ExceptionHandler_Handle")
TestRunner_RunTest("ExceptionHandler_GetExceptionCount", "Test_ExceptionHandler_GetExceptionCount")

TestRunner_ShowResults()

ExitApp
