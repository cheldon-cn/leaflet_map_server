; ============================================
; ConfigManager.ahk - 配置管理模块 (V1版本)
; 负责加载、读取和管理配置信息
; ============================================

; 全局配置对象
global g_Config := {}

; 配置文件路径
global g_ConfigFile := ""

; 配置文件缓存
global g_ConfigContent := ""

ConfigManager_Init(configFile := "config.ini") {
    global g_Config, g_ConfigFile, g_ConfigContent
    
    g_ConfigFile := A_ScriptDir . "\..\" . configFile
    
    g_Config := {}
    g_Config.General := {}
    g_Config.Window := {}
    g_Config.InputPosition := {}
    g_Config.ButtonPosition := {}
    g_Config.DetectionRegion := {}
    g_Config.OCR := {}
    g_Config.Keywords := {}
    g_Config.Retry := {}
    g_Config.Log := {}
    g_Config.Watchdog := {}
    g_Config.Heartbeat := {}
    g_Config.PixelDetection := {}
    
    if !FileExist(g_ConfigFile) {
        ConfigManager_LoadDefaults()
        return false
    }
    
    FileRead, g_ConfigContent, *P65001 %g_ConfigFile%
    
    ConfigManager_LoadFromFile()
    return true
}

ConfigManager_ReadUTF8(section, key, defaultValue := "") {
    global g_ConfigContent
    
    if (g_ConfigContent = "") {
        return defaultValue
    }
    
    content := "`n" . g_ConfigContent . "`n"
    
    sectionStart := InStr(content, "`n[" . section . "]`n")
    if (sectionStart = 0) {
        return defaultValue
    }
    
    sectionEnd := InStr(content, "`n[", false, sectionStart + 1)
    if (sectionEnd = 0) {
        sectionEnd := StrLen(content)
    }
    
    sectionContent := SubStr(content, sectionStart, sectionEnd - sectionStart)
    
    keyStart := InStr(sectionContent, "`n" . key . "=")
    if (keyStart = 0) {
        return defaultValue
    }
    
    valueStart := keyStart + StrLen(key) + 2
    valueEnd := InStr(sectionContent, "`n", false, valueStart)
    if (valueEnd = 0) {
        valueEnd := StrLen(sectionContent)
    }
    
    value := SubStr(sectionContent, valueStart, valueEnd - valueStart)
    
    if (StrLen(value) = 0) {
        return defaultValue
    }
    
    return value
}

ConfigManager_LoadDefaults() {
    global g_Config
    
    g_Config.General.DetectInterval := 10000
    g_Config.General.CooldownTime := 900000
    g_Config.General.PostExecutionWait := 30000
    g_Config.General.EnableAutoContinue := true
    g_Config.General.DebugMode := false
    g_Config.General.ResponseDelayMin := 10000
    g_Config.General.ResponseDelayMax := 30000
    
    g_Config.Window.WindowTitle := "Trae"
    g_Config.Window.WindowDetectTimeout := 5000
    
    g_Config.InputPosition.InputOffsetX := 0.5
    g_Config.InputPosition.InputOffsetY := 0.9
    g_Config.InputPosition.InputAbsolutePos := "-1,-1"
    
    g_Config.ButtonPosition.ButtonOffsetX := 0.95
    g_Config.ButtonPosition.ButtonOffsetY := 0.95
    g_Config.ButtonPosition.ButtonAbsolutePos := "-1,-1"
    
    g_Config.DetectionRegion.PriorityRegions := [1, 2, 3]
    g_Config.DetectionRegion.DetectFullWindowIfNotFound := true
    g_Config.DetectionRegion.CustomRegion := ""
    
    g_Config.OCR.OCREngine := "RapidOCR"
    g_Config.OCR.OCRTimeout := 3000
    g_Config.OCR.ConfidenceThreshold := 60
    g_Config.OCR.EnableOCRCache := true
    g_Config.OCR.OCRCacheExpiry := 60000
    
    g_Config.Keywords.PromptKeywords := ["模型思考次数已达上限", "思考次数已达上限", "已达上限"]
    g_Config.Keywords.InputText := "继续"
    
    g_Config.Retry.MaxRetryCount := 3
    g_Config.Retry.RetryBaseDelay := 1000
    g_Config.Retry.RetryMaxDelay := 30000
    g_Config.Retry.ConsecutiveFailureThreshold := 5
    
    g_Config.Log.LogLevel := "INFO"
    g_Config.Log.LogFilePath := "log\trae_auto.log"
    g_Config.Log.LogMaxSize := 1024
    g_Config.Log.LogRetentionDays := 7
    
    g_Config.Watchdog.WatchdogFilePath := "log\watchdog.txt"
    g_Config.Watchdog.WatchdogTimeout := 300000
    
    g_Config.Heartbeat.HeartbeatInterval := 30000
    g_Config.Heartbeat.HeartbeatTimeout := 120000
    
    g_Config.PixelDetection.EnablePixelDetection := false
    g_Config.PixelDetection.TargetRegion := "0,0,100,100"
    g_Config.PixelDetection.TargetColor := "0xFFFFFF"
    g_Config.PixelDetection.ColorTolerance := 10
}

ConfigManager_LoadFromFile() {
    global g_Config, g_ConfigFile
    
    IniRead, value, %g_ConfigFile%, General, DetectInterval, 10000
    g_Config.General.DetectInterval := value + 0
    
    IniRead, value, %g_ConfigFile%, General, CooldownTime, 900000
    g_Config.General.CooldownTime := value + 0
    
    IniRead, value, %g_ConfigFile%, General, PostExecutionWait, 30000
    g_Config.General.PostExecutionWait := value + 0
    
    IniRead, value, %g_ConfigFile%, General, EnableAutoContinue, 1
    g_Config.General.EnableAutoContinue := (value = "1" || value = "true")
    
    IniRead, value, %g_ConfigFile%, General, DebugMode, 0
    g_Config.General.DebugMode := (value = "1" || value = "true")
    
    IniRead, value, %g_ConfigFile%, General, ResponseDelayMin, 10000
    g_Config.General.ResponseDelayMin := value + 0
    
    IniRead, value, %g_ConfigFile%, General, ResponseDelayMax, 30000
    g_Config.General.ResponseDelayMax := value + 0
    
    IniRead, value, %g_ConfigFile%, Window, WindowTitle, Trae
    g_Config.Window.WindowTitle := value
    
    IniRead, value, %g_ConfigFile%, Window, TitleMatchMode, 2
    g_Config.Window.TitleMatchMode := value + 0
    
    IniRead, value, %g_ConfigFile%, Window, WindowDetectTimeout, 5000
    g_Config.Window.WindowDetectTimeout := value + 0
    
    IniRead, value, %g_ConfigFile%, InputPosition, InputOffsetX, 0.5
    g_Config.InputPosition.InputOffsetX := value + 0.0
    
    IniRead, value, %g_ConfigFile%, InputPosition, InputOffsetY, 0.9
    g_Config.InputPosition.InputOffsetY := value + 0.0
    
    IniRead, value, %g_ConfigFile%, InputPosition, InputAbsolutePos, -1,-1
    g_Config.InputPosition.InputAbsolutePos := value
    
    IniRead, value, %g_ConfigFile%, ButtonPosition, ButtonOffsetX, 0.95
    g_Config.ButtonPosition.ButtonOffsetX := value + 0.0
    
    IniRead, value, %g_ConfigFile%, ButtonPosition, ButtonOffsetY, 0.95
    g_Config.ButtonPosition.ButtonOffsetY := value + 0.0
    
    IniRead, value, %g_ConfigFile%, ButtonPosition, ButtonAbsolutePos, -1,-1
    g_Config.ButtonPosition.ButtonAbsolutePos := value
    
    IniRead, value, %g_ConfigFile%, DetectionRegion, PriorityRegions, 1,2,3
    g_Config.DetectionRegion.PriorityRegions := StrSplit(value, ",")
    
    IniRead, value, %g_ConfigFile%, DetectionRegion, DetectFullWindowIfNotFound, 1
    g_Config.DetectionRegion.DetectFullWindowIfNotFound := (value = "1" || value = "true")
    
    IniRead, value, %g_ConfigFile%, DetectionRegion, CustomRegion,
    g_Config.DetectionRegion.CustomRegion := value
    
    IniRead, value, %g_ConfigFile%, OCR, OCREngine, RapidOCR
    g_Config.OCR.OCREngine := value
    
    IniRead, value, %g_ConfigFile%, OCR, OCRTimeout, 3000
    g_Config.OCR.OCRTimeout := value + 0
    
    IniRead, value, %g_ConfigFile%, OCR, ConfidenceThreshold, 60
    g_Config.OCR.ConfidenceThreshold := value + 0
    
    IniRead, value, %g_ConfigFile%, OCR, EnableOCRCache, 1
    g_Config.OCR.EnableOCRCache := (value = "1" || value = "true")
    
    IniRead, value, %g_ConfigFile%, OCR, OCRCacheExpiry, 60000
    g_Config.OCR.OCRCacheExpiry := value + 0
    
    value := ConfigManager_ReadUTF8("Keywords", "PromptKeywords", "模型思考次数已达上限,思考次数已达上限,已达上限")
    g_Config.Keywords.PromptKeywords := StrSplit(value, ",")
    
    value := ConfigManager_ReadUTF8("Keywords", "InputText", "继续")
    g_Config.Keywords.InputText := value
    
    IniRead, value, %g_ConfigFile%, Retry, MaxRetryCount, 3
    g_Config.Retry.MaxRetryCount := value + 0
    
    IniRead, value, %g_ConfigFile%, Retry, RetryBaseDelay, 1000
    g_Config.Retry.RetryBaseDelay := value + 0
    
    IniRead, value, %g_ConfigFile%, Retry, RetryMaxDelay, 30000
    g_Config.Retry.RetryMaxDelay := value + 0
    
    IniRead, value, %g_ConfigFile%, Retry, ConsecutiveFailureThreshold, 5
    g_Config.Retry.ConsecutiveFailureThreshold := value + 0
    
    IniRead, value, %g_ConfigFile%, Log, LogLevel, INFO
    g_Config.Log.LogLevel := value
    
    IniRead, value, %g_ConfigFile%, Log, LogFilePath, log\trae_auto.log
    g_Config.Log.LogFilePath := value
    
    IniRead, value, %g_ConfigFile%, Log, LogMaxSize, 1024
    g_Config.Log.LogMaxSize := value + 0
    
    IniRead, value, %g_ConfigFile%, Log, LogRetentionDays, 7
    g_Config.Log.LogRetentionDays := value + 0
    
    IniRead, value, %g_ConfigFile%, Watchdog, WatchdogFilePath, log\watchdog.txt
    g_Config.Watchdog.WatchdogFilePath := value
    
    IniRead, value, %g_ConfigFile%, Watchdog, WatchdogTimeout, 300000
    g_Config.Watchdog.WatchdogTimeout := value + 0
    
    IniRead, value, %g_ConfigFile%, Heartbeat, HeartbeatInterval, 30000
    g_Config.Heartbeat.HeartbeatInterval := value + 0
    
    IniRead, value, %g_ConfigFile%, Heartbeat, HeartbeatTimeout, 120000
    g_Config.Heartbeat.HeartbeatTimeout := value + 0
    
    IniRead, value, %g_ConfigFile%, PixelDetection, EnablePixelDetection, 0
    g_Config.PixelDetection.EnablePixelDetection := (value = "1" || value = "true")
    
    IniRead, value, %g_ConfigFile%, PixelDetection, TargetRegion, 0,0,100,100
    g_Config.PixelDetection.TargetRegion := value
    
    IniRead, value, %g_ConfigFile%, PixelDetection, TargetColor, 0xFFFFFF
    g_Config.PixelDetection.TargetColor := value
    
    IniRead, value, %g_ConfigFile%, PixelDetection, ColorTolerance, 10
    g_Config.PixelDetection.ColorTolerance := value + 0
}

ConfigManager_Get(section, key, defaultValue := "") {
    global g_Config
    
    if !IsObject(g_Config[section]) {
        return defaultValue
    }
    
    if (g_Config[section][key] = "") {
        return defaultValue
    }
    
    return g_Config[section][key]
}

ConfigManager_Set(section, key, value) {
    global g_Config, g_ConfigFile
    
    if !IsObject(g_Config[section]) {
        g_Config[section] := {}
    }
    
    g_Config[section][key] := value
    
    IniWrite, %value%, %g_ConfigFile%, %section%, %key%
}

ConfigManager_Reload() {
    global g_ConfigFile
    
    if FileExist(g_ConfigFile) {
        ConfigManager_LoadFromFile()
        return true
    }
    
    return false
}

ConfigManager_GetLogFilePath() {
    global g_Config
    
    logPath := g_Config.Log.LogFilePath
    
    if (SubStr(logPath, 2, 1) != ":") {
        logPath := A_ScriptDir . "\" . logPath
    }
    
    SplitPath, logPath, , logDir
    if !FileExist(logDir) {
        FileCreateDir, %logDir%
    }
    
    return logPath
}

ConfigManager_GetWatchdogFilePath() {
    global g_Config
    
    watchdogPath := g_Config.Watchdog.WatchdogFilePath
    
    if (SubStr(watchdogPath, 2, 1) != ":") {
        watchdogPath := A_ScriptDir . "\" . watchdogPath
    }
    
    SplitPath, watchdogPath, , watchdogDir
    if !FileExist(watchdogDir) {
        FileCreateDir, %watchdogDir%
    }
    
    return watchdogPath
}
