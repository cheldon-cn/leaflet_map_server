; ============================================
; Main.ahk - 主程序入口 (V1版本)
; Trae IDE 自动继续工具
; ============================================

#SingleInstance Force
#NoEnv
SetWorkingDir %A_ScriptDir%
SendMode Input
SetBatchLines -1
SetTitleMatchMode, 2

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

; 状态机状态常量
global STATE_COOLDOWN := "COOLDOWN"
global STATE_DETECTING := "DETECTING"
global STATE_POST_EXECUTION := "POST_EXECUTION"

; 当前状态
global g_CurrentState := STATE_COOLDOWN

; 状态切换时间
global g_StateStartTime := 0

; 是否暂停
global g_IsPaused := false

; 是否运行中
global g_IsRunning := true

; ============================================
; 程序入口
; ============================================

Main_Init() {
    global g_CurrentState, g_StateStartTime
    
    LogInfo("========================================")
    LogInfo("Trae 自动继续工具启动")
    LogInfo("版本: 1.0 (AutoHotkey V1)")
    LogInfo("========================================")
    
    ConfigManager_Init()
    Logger_Init()
    Utils_Init()
    WindowDetector_Init()
    OCRDetector_Init()
    KeywordDetector_Init()
    AutoContinueExecutor_Init()
    RetryManager_Init()
    HeartbeatMonitor_Init()
    Watchdog_Init()
    ExceptionHandler_Init()
    
    g_CurrentState := STATE_COOLDOWN
    g_StateStartTime := A_TickCount
    
    Main_SetupTrayMenu()
    Main_SetupHotkeys()
    
    timerInterval := g_Config.General.DetectInterval
    SetTimer, Main_Timer, %timerInterval%
    
    LogInfo("初始化完成，开始运行")
    LogInfo("状态: " . g_CurrentState . ", 冷却时间: " . (g_Config.General.CooldownTime / 1000) . "秒, 检测间隔: " . (timerInterval / 1000) . "秒")
}

Main_SetupTrayMenu() {
    global g_IsPaused
    
    Menu, Tray, NoStandard
    Menu, Tray, Add, 显示状态, Main_ShowStatus
    Menu, Tray, Add, 暂停/继续, Main_TogglePause
    Menu, Tray, Add
    Menu, Tray, Add, 重置计数, Main_ResetCounters
    Menu, Tray, Add, 查看日志, Main_OpenLog
    Menu, Tray, Add
    Menu, Tray, Add, 退出, Main_Exit
    
    Menu, Tray, Tip, Trae 自动继续工具
    
    if g_IsPaused {
        Menu, Tray, Icon, Shell32.dll, 110
    } else {
        Menu, Tray, Icon, Shell32.dll, 14
    }
}

Main_SetupHotkeys() {
    Hotkey, ^!p, Main_TogglePause
    Hotkey, ^!s, Main_ShowStatus
    Hotkey, ^!r, Main_ResetCounters
    
    LogDebug("热键注册完成: Ctrl+Alt+P (暂停), Ctrl+Alt+S (状态), Ctrl+Alt+R (重置)")
}

Main_Timer() {
    global g_IsRunning, g_IsPaused, g_CurrentState, g_StateStartTime
    
    if !g_IsRunning {
        return
    }
    
    if g_IsPaused {
        return
    }
    
    Watchdog_Update()
    
    Main_ProcessStateMachine()
}

Main_ProcessStateMachine() {
    global g_CurrentState, g_StateStartTime, g_Config
    
    elapsed := A_TickCount - g_StateStartTime
    
    if (g_CurrentState = STATE_COOLDOWN) {
        if (elapsed >= g_Config.General.CooldownTime) {
            Main_TransitionToState(STATE_DETECTING)
        }
    } else if (g_CurrentState = STATE_DETECTING) {
        Main_PerformDetection()
    } else if (g_CurrentState = STATE_POST_EXECUTION) {
        if (elapsed >= g_Config.General.PostExecutionWait) {
            Main_TransitionToState(STATE_COOLDOWN)
        }
    }
}

Main_PerformDetection() {
    global g_Config
    
    if !WindowDetector_DetectTraeWindow() {
        LogDebug("未检测到 Trae 窗口，等待下次检测")
        return
    }
    
    WindowDetector_UpdateWindowRect()
    
    if WindowDetector_IsWindowMinimized() {
        LogDebug("窗口已最小化，跳过检测")
        return
    }
    
    priorityRegions := g_Config.DetectionRegion.PriorityRegions
    regionStr := ""
    for index, region in priorityRegions {
        if (regionStr != "") {
            regionStr .= ","
        }
        regionStr .= region
    }
    LogDebug("开始 OCR 检测 (优先区域: " . regionStr . ")...")
    
    detectedKeywords := ""
    
    if !OCRDetector_PerformSmartDetection(detectedKeywords) {
        LogDebug("OCR 智能检测未发现关键词")
        return
    }
    
    LogInfo("检测到关键词: " . detectedKeywords.keyword)
    
    if g_Config.General.EnableAutoContinue {
        if AutoContinueExecutor_Execute() {
            Main_TransitionToState(STATE_POST_EXECUTION)
        }
    } else {
        LogInfo("自动继续已禁用，跳过执行")
    }
}

Main_TransitionToState(newState) {
    global g_CurrentState, g_StateStartTime, g_Config
    
    oldState := g_CurrentState
    g_CurrentState := newState
    g_StateStartTime := A_TickCount
    
    LogInfo("状态切换: " . oldState . " -> " . newState)
    
    if (newState = STATE_COOLDOWN) {
        LogDebug("进入冷却期，等待 " . (g_Config.General.CooldownTime / 1000) . " 秒")
    } else if (newState = STATE_DETECTING) {
        LogDebug("进入检测期")
    } else if (newState = STATE_POST_EXECUTION) {
        LogDebug("进入执行后等待，等待 " . (g_Config.General.PostExecutionWait / 1000) . " 秒")
    }
}

Main_ShowStatus() {
    global g_CurrentState, g_IsPaused, g_Config
    global g_ExecuteCount, g_ConsecutiveFailures
    
    status := "状态: " . g_CurrentState
    status .= "`n暂停: " . (g_IsPaused ? "是" : "否")
    status .= "`n执行次数: " . AutoContinueExecutor_GetExecuteCount()
    status .= "`n连续失败: " . RetryManager_GetConsecutiveFailures()
    status .= "`n心跳计数: " . HeartbeatMonitor_GetHeartbeatCount()
    status .= "`n看门狗计数: " . Watchdog_GetCount()
    status .= "`n运行时间: " . HeartbeatMonitor_GetUptimeFormatted()
    
    MsgBox, 0, Trae 自动继续工具 - 状态, %status%
}

Main_TogglePause() {
    global g_IsPaused
    
    g_IsPaused := !g_IsPaused
    
    if g_IsPaused {
        LogInfo("脚本已暂停")
        Menu, Tray, Icon, Shell32.dll, 110
        TrayTip, Trae Auto, 脚本已暂停, 3, 1
    } else {
        LogInfo("脚本已恢复")
        Menu, Tray, Icon, Shell32.dll, 14
        TrayTip, Trae Auto, 脚本已恢复, 3, 1
    }
}

Main_ResetCounters() {
    AutoContinueExecutor_ResetCount()
    RetryManager_ResetConsecutiveFailures()
    HeartbeatMonitor_Reset()
    Watchdog_Reset()
    
    LogInfo("所有计数器已重置")
    TrayTip, Trae Auto, 计数器已重置, 3, 1
}

Main_OpenLog() {
    global g_LogFilePath
    
    if FileExist(g_LogFilePath) {
        Run, notepad.exe "%g_LogFilePath%"
    } else {
        MsgBox, 日志文件不存在
    }
}

Main_Exit() {
    global g_IsRunning
    
    g_IsRunning := false
    
    SetTimer, Main_Timer, Off
    
    HeartbeatMonitor_Stop()
    Watchdog_Cleanup()
    
    LogInfo("========================================")
    LogInfo("Trae 自动继续工具退出")
    LogInfo("========================================")
    
    ExitApp
}

; ============================================
; 启动程序
; ============================================

Main_Init()

; 主循环
Loop {
    Sleep, 100
}
