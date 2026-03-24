; ============================================
; Logger.ahk - 日志系统模块 (V1版本)
; 支持多级别日志、文件轮转和托盘通知
; ============================================

; 日志级别常量
global LOG_DEBUG := 0
global LOG_INFO := 1
global LOG_WARN := 2
global LOG_ERROR := 3

; 日志级别名称
global LOG_LEVEL_NAMES := ["DEBUG", "INFO", "WARN", "ERROR"]

; 当前日志级别
global g_LogLevel := LOG_INFO

; 日志文件路径
global g_LogFilePath := ""

; 日志文件最大大小 (KB)
global g_LogMaxSize := 1024

; 日志保留天数
global g_LogRetentionDays := 7

; 日志文件句柄
global g_LogFileHandle := ""

Logger_Init() {
    global g_LogLevel, g_LogFilePath, g_LogMaxSize, g_LogRetentionDays
    
    g_LogFilePath := ConfigManager_GetLogFilePath()
    g_LogMaxSize := ConfigManager_Get("Log", "LogMaxSize", 1024)
    g_LogRetentionDays := ConfigManager_Get("Log", "LogRetentionDays", 7)
    
    logLevelStr := ConfigManager_Get("Log", "LogLevel", "INFO")
    g_LogLevel := Logger_ParseLogLevel(logLevelStr)
    
    if (ConfigManager_Get("General", "DebugMode", false)) {
        g_LogLevel := LOG_DEBUG
        LogInfo("DebugMode enabled, log level set to DEBUG")
    }
    
    Logger_CleanOldLogs()
    
    Logger_RotateIfNeeded()
    
    LogInfo("Logger system initialized")
}

Logger_ParseLogLevel(levelStr) {
    StringUpper, levelStr, levelStr
    
    if (levelStr = "DEBUG") {
        return LOG_DEBUG
    } else if (levelStr = "INFO") {
        return LOG_INFO
    } else if (levelStr = "WARN") {
        return LOG_WARN
    } else if (levelStr = "ERROR") {
        return LOG_ERROR
    }
    
    return LOG_INFO
}

Logger_Write(level, message) {
    global g_LogLevel, g_LogFilePath
    
    if (level < g_LogLevel) {
        return
    }
    
    timestamp := A_YYYY . "-" . A_MM . "-" . A_DD . " " . A_Hour . ":" . A_Min . ":" . A_Sec
    levelName := LOG_LEVEL_NAMES[level + 1]
    logLine := "[" . timestamp . "] [" . levelName . "] " . message . "`n"
    
    try {
        if FileExist(g_LogFilePath) {
            FileAppend, %logLine%, %g_LogFilePath%, UTF-8
        } else {
            FileAppend, %logLine%, %g_LogFilePath%, UTF-8
        }
    } catch {
        OutputDebug, "[Logger] Failed to write log"
    }
    
    if (level >= LOG_ERROR) {
        Logger_ShowTrayNotification(levelName, message)
    }
}

LogDebug(message) {
    Logger_Write(LOG_DEBUG, message)
}

LogInfo(message) {
    Logger_Write(LOG_INFO, message)
}

LogWarn(message) {
    Logger_Write(LOG_WARN, message)
}

LogError(message) {
    Logger_Write(LOG_ERROR, message)
}

Logger_ShowTrayNotification(level, message) {
    global g_Config
    
    if (g_Config.General.DebugMode) {
        TrayTip, Trae Auto [%level%], %message%, 5, 3
    }
}

Logger_RotateIfNeeded() {
    global g_LogFilePath, g_LogMaxSize
    
    if !FileExist(g_LogFilePath) {
        return
    }
    
    FileGetSize, fileSize, %g_LogFilePath%, K
    
    if (fileSize >= g_LogMaxSize) {
        Logger_Rotate()
    }
}

Logger_Rotate() {
    global g_LogFilePath
    
    timestamp := A_YYYY . A_MM . A_DD . "_" . A_Hour . A_Min . A_Sec
    backupPath := g_LogFilePath . "." . timestamp . ".bak"
    
    FileMove, %g_LogFilePath%, %backupPath%
    
    LogInfo("日志文件轮转完成: " . backupPath)
}

Logger_CleanOldLogs() {
    global g_LogFilePath, g_LogRetentionDays
    
    SplitPath, g_LogFilePath, , logDir
    
    if !FileExist(logDir) {
        return
    }
    
    logFilePattern := logDir . "\*.log.bak"
    
    cutoffDate := A_Now
    EnvSub, cutoffDate, %g_LogRetentionDays%, Days
    
    Loop, Files, %logFilePattern%
    {
        FileGetTime, fileTime, %A_LoopFileFullPath%, M
        if (fileTime < cutoffDate) {
            FileDelete, %A_LoopFileFullPath%
        }
    }
}

Logger_SetLevel(level) {
    global g_LogLevel
    g_LogLevel := level
}

Logger_GetLevelName() {
    global g_LogLevel
    return LOG_LEVEL_NAMES[g_LogLevel + 1]
}

Logger_Flush() {
}
