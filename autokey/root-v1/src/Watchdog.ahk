; ============================================
; Watchdog.ahk - 看门狗模块 (V1版本)
; 使用文件作为看门狗标志，检测脚本卡死
; ============================================

; 看门狗文件路径
global g_WatchdogFilePath := ""

; 看门狗激活状态
global g_WatchdogActive := false

; 上次更新时间
global g_WatchdogLastUpdate := 0

; 看门狗计数
global g_WatchdogCount := 0

Watchdog_Init() {
    global g_WatchdogFilePath, g_WatchdogActive, g_WatchdogLastUpdate, g_WatchdogCount, g_Config
    
    g_WatchdogFilePath := ConfigManager_GetWatchdogFilePath()
    g_WatchdogActive := true
    g_WatchdogLastUpdate := A_TickCount
    g_WatchdogCount := 0
    
    SplitPath, g_WatchdogFilePath, , logDir
    if !FileExist(logDir) {
        FileCreateDir, %logDir%
    }
    
    Watchdog_Update()
    
    LogInfo("看门狗模块初始化完成，文件: " . g_WatchdogFilePath)
}

Watchdog_Update() {
    global g_WatchdogFilePath, g_WatchdogActive, g_WatchdogLastUpdate, g_WatchdogCount
    
    if !g_WatchdogActive {
        return false
    }
    
    timestamp := A_TickCount
    content := timestamp . "|" . g_WatchdogCount . "|" . Utils_GetTimestampFormatted()
    
    if FileExist(g_WatchdogFilePath) {
        FileDelete, %g_WatchdogFilePath%
    }
    
    FileAppend, %content%, %g_WatchdogFilePath%, UTF-8
    
    if ErrorLevel {
        LogError("看门狗文件更新失败")
        return false
    }
    
    g_WatchdogLastUpdate := timestamp
    g_WatchdogCount++
    
    return true
}

Watchdog_Check() {
    global g_WatchdogFilePath, g_Config
    
    if !FileExist(g_WatchdogFilePath) {
        result := {}
        result.healthy := false
        result.reason := "看门狗文件不存在"
        return result
    }
    
    FileRead, content, %g_WatchdogFilePath%
    
    if ErrorLevel {
        result := {}
        result.healthy := false
        result.reason := "看门狗文件读取失败"
        return result
    }
    
    parts := StrSplit(content, "|")
    
    if (parts.MaxIndex() < 2) {
        result := {}
        result.healthy := false
        result.reason := "看门狗文件格式错误"
        return result
    }
    
    lastUpdate := parts[1] + 0
    count := parts[2] + 0
    
    elapsed := A_TickCount - lastUpdate
    timeout := g_Config.Watchdog.WatchdogTimeout
    
    if (elapsed > timeout) {
        result := {}
        result.healthy := false
        result.reason := "看门狗超时"
        result.elapsed := elapsed
        result.timeout := timeout
        result.count := count
        return result
    }
    
    result := {}
    result.healthy := true
    result.elapsed := elapsed
    result.count := count
    return result
}

Watchdog_GetLastUpdate() {
    global g_WatchdogLastUpdate
    return g_WatchdogLastUpdate
}

Watchdog_GetCount() {
    global g_WatchdogCount
    return g_WatchdogCount
}

Watchdog_IsActive() {
    global g_WatchdogActive
    return g_WatchdogActive
}

Watchdog_Stop() {
    global g_WatchdogActive
    
    g_WatchdogActive := false
    
    LogInfo("看门狗已停止")
}

Watchdog_Start() {
    global g_WatchdogActive
    
    g_WatchdogActive := true
    Watchdog_Update()
    
    LogInfo("看门狗已启动")
}

Watchdog_Reset() {
    global g_WatchdogCount
    
    g_WatchdogCount := 0
    Watchdog_Update()
    
    LogInfo("看门狗计数器已重置")
}

Watchdog_Cleanup() {
    global g_WatchdogFilePath
    
    if FileExist(g_WatchdogFilePath) {
        FileDelete, %g_WatchdogFilePath%
    }
    
    LogDebug("看门狗文件已清理")
}

Watchdog_GetFilePath() {
    global g_WatchdogFilePath
    return g_WatchdogFilePath
}

Watchdog_GetElapsedTime() {
    global g_WatchdogLastUpdate
    
    return A_TickCount - g_WatchdogLastUpdate
}

Watchdog_GetStatus() {
    global g_WatchdogActive, g_WatchdogLastUpdate, g_WatchdogCount, g_WatchdogFilePath
    
    result := {}
    result.active := g_WatchdogActive
    result.lastUpdate := g_WatchdogLastUpdate
    result.count := g_WatchdogCount
    result.elapsed := Watchdog_GetElapsedTime()
    result.filePath := g_WatchdogFilePath
    return result
}

Watchdog_LogStatus() {
    status := Watchdog_GetStatus()
    
    LogInfo("看门狗状态 - 激活: " . status.active 
        . ", 计数: " . status.count 
        . ", 距上次更新: " . status.elapsed . "ms")
}
