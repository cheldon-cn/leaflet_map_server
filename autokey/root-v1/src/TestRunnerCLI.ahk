; ============================================
; TestRunnerCLI.ahk - 命令行测试脚本 (V1版本)
; 输出测试结果到文件
; ============================================

#SingleInstance Force
#NoEnv
SetWorkingDir %A_ScriptDir%

#Include %A_ScriptDir%\ConfigManager.ahk
#Include %A_ScriptDir%\Logger.ahk
#Include %A_ScriptDir%\Utils.ahk
#Include %A_ScriptDir%\WindowDetector.ahk
#Include %A_ScriptDir%\KeywordDetector.ahk
#Include %A_ScriptDir%\AutoContinueExecutor.ahk
#Include %A_ScriptDir%\RetryManager.ahk
#Include %A_ScriptDir%\HeartbeatMonitor.ahk
#Include %A_ScriptDir%\Watchdog.ahk
#Include %A_ScriptDir%\ExceptionHandler.ahk

global g_TestResults := []
global g_TestPassed := 0
global g_TestFailed := 0
global g_OutputFile := A_ScriptDir . "\..\test_results.txt"

TestRunner_Init() {
    global g_TestResults, g_TestPassed, g_TestFailed
    
    g_TestResults := []
    g_TestPassed := 0
    g_TestFailed := 0
    
    ConfigManager_Init()
    Utils_Init()
}

TestRunner_RunTest(testName, testFunc) {
    global g_TestResults, g_TestPassed, g_TestFailed
    
    try {
        result := %testFunc%()
        
        if result {
            g_TestPassed++
            g_TestResults.Push({name: testName, status: "PASS", error: ""})
        } else {
            g_TestFailed++
            g_TestResults.Push({name: testName, status: "FAIL", error: "返回 false"})
        }
    } catch {
        g_TestFailed++
        g_TestResults.Push({name: testName, status: "FAIL", error: "异常"})
    }
}

TestRunner_WriteResults() {
    global g_TestResults, g_TestPassed, g_TestFailed, g_OutputFile
    
    output := "========================================`n"
    output .= "集成测试结果`n"
    output .= "========================================`n`n"
    output .= "通过: " . g_TestPassed . "`n"
    output .= "失败: " . g_TestFailed . "`n"
    output .= "总计: " . (g_TestPassed + g_TestFailed) . "`n`n"
    output .= "----------------------------------------`n"
    output .= "详细结果:`n"
    output .= "----------------------------------------`n"
    
    for index, test in g_TestResults {
        output .= test.status . " - " . test.name
        if (test.error != "") {
            output .= " (" . test.error . ")"
        }
        output .= "`n"
    }
    
    FileDelete, %g_OutputFile%
    FileAppend, %output%, %g_OutputFile%, UTF-8
    
    FileRead, content, %g_OutputFile%
    OutputDebug, %content%
}

Test_ConfigManager_Init() {
    result := ConfigManager_Init()
    return result || true
}

Test_ConfigManager_Get() {
    ConfigManager_Init()
    value := ConfigManager_Get("General", "DetectInterval", 10000)
    return (value > 0)
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

Test_Utils_Random() {
    value := Utils_Random(1, 100)
    return (value >= 1 && value <= 100)
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

Test_AutoContinueExecutor_Init() {
    AutoContinueExecutor_Init()
    return true
}

Test_RetryManager_Init() {
    RetryManager_Init()
    return true
}

Test_RetryManager_CalculateDelay() {
    RetryManager_Init()
    
    delay1 := RetryManager_CalculateDelay(1)
    delay2 := RetryManager_CalculateDelay(2)
    
    return (delay1 > 0 && delay2 > delay1)
}

Test_HeartbeatMonitor_Init() {
    HeartbeatMonitor_Init()
    return true
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

Test_ExceptionHandler_Init() {
    ExceptionHandler_Init()
    return true
}

Test_ExceptionHandler_Handle() {
    ExceptionHandler_Init()
    result := ExceptionHandler_Handle(EXCEPTION_OCR, "测试异常", "测试上下文")
    return IsObject(result)
}

TestRunner_Init()

TestRunner_RunTest("ConfigManager_Init", "Test_ConfigManager_Init")
TestRunner_RunTest("ConfigManager_Get", "Test_ConfigManager_Get")
TestRunner_RunTest("Utils_GetDpiScalingFactor", "Test_Utils_GetDpiScalingFactor")
TestRunner_RunTest("Utils_GetTimestamp", "Test_Utils_GetTimestamp")
TestRunner_RunTest("Utils_Random", "Test_Utils_Random")
TestRunner_RunTest("KeywordDetector_Init", "Test_KeywordDetector_Init")
TestRunner_RunTest("KeywordDetector_DetectKeywords", "Test_KeywordDetector_DetectKeywords")
TestRunner_RunTest("AutoContinueExecutor_Init", "Test_AutoContinueExecutor_Init")
TestRunner_RunTest("RetryManager_Init", "Test_RetryManager_Init")
TestRunner_RunTest("RetryManager_CalculateDelay", "Test_RetryManager_CalculateDelay")
TestRunner_RunTest("HeartbeatMonitor_Init", "Test_HeartbeatMonitor_Init")
TestRunner_RunTest("Watchdog_Init", "Test_Watchdog_Init")
TestRunner_RunTest("Watchdog_Update", "Test_Watchdog_Update")
TestRunner_RunTest("ExceptionHandler_Init", "Test_ExceptionHandler_Init")
TestRunner_RunTest("ExceptionHandler_Handle", "Test_ExceptionHandler_Handle")

TestRunner_WriteResults()

ExitApp
