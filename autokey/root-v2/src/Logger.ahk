class Logger {
    static logFile := "log\trae_auto.log"
    static maxLogSize := 10485760
    static enableLog := true
    static logLevel := 1
    
    static LEVEL_DEBUG := 0
    static LEVEL_INFO := 1
    static LEVEL_WARN := 2
    static LEVEL_ERROR := 3
    
    static Initialize(config := "") {
        if (config != "" && config.Has("LogFile"))
            this.logFile := config["LogFile"]
        if (config != "" && config.Has("MaxLogSize"))
            this.maxLogSize := config["MaxLogSize"]
        if (config != "" && config.Has("EnableLog"))
            this.enableLog := (config["EnableLog"] = "true")
        
        this.EnsureLogDirectory()
    }
    
    static EnsureLogDirectory() {
        logDir := ""
        if (InStr(this.logFile, "\") > 0) {
            logDir := SubStr(this.logFile, 1, InStr(this.logFile, "\", , -1) - 1)
        }
        
        if (logDir != "" && !DirExist(logDir)) {
            try {
                DirCreate(logDir)
            } catch {
                OutputDebug("Logger: 无法创建日志目录: " logDir)
            }
        }
    }
    
    static Debug(msg) {
        if (this.logLevel <= this.LEVEL_DEBUG)
            this.WriteLog("DEBUG", msg)
    }
    
    static Info(msg) {
        if (this.logLevel <= this.LEVEL_INFO)
            this.WriteLog("INFO", msg)
    }
    
    static Warn(msg) {
        if (this.logLevel <= this.LEVEL_WARN)
            this.WriteLog("WARN", msg)
    }
    
    static Error(msg) {
        if (this.logLevel <= this.LEVEL_ERROR) {
            this.WriteLog("ERROR", msg)
            this.ShowErrorNotification(msg)
        }
    }
    
    static WriteLog(level, msg) {
        if !this.enableLog
            return
        
        timestamp := FormatTime(, "yyyy-MM-dd HH:mm:ss")
        logLine := Format("[{}] [{}] {}`r`n", timestamp, level, msg)
        
        OutputDebug(logLine)
        
        try {
            this.CheckLogRotation()
            this.AppendLog(logLine)
        } catch as e {
            OutputDebug("Logger: 写入日志失败: " e.Message)
        }
    }
    
    static AppendLog(logLine) {
        try {
            file := FileOpen(this.logFile, "a", "UTF-8")
            if (file) {
                file.Write(logLine)
                file.Close()
            }
        } catch as e {
            OutputDebug("Logger: 写入日志失败: " e.Message)
        }
    }
    
    static CheckLogRotation() {
        if !FileExist(this.logFile)
            return
        
        try {
            fileSize := FileGetSize(this.logFile)
            if (fileSize > this.maxLogSize) {
                backupFile := this.logFile ".bak"
                if FileExist(backupFile)
                    FileDelete(backupFile)
                FileMove(this.logFile, backupFile)
            }
        } catch as e {
            OutputDebug("Logger: 日志轮转失败: " e.Message)
        }
    }
    
    static ShowErrorNotification(msg) {
        OutputDebug("Logger ERROR: " msg)
    }
    
    static SetLogFile(filePath) {
        this.logFile := filePath
        this.EnsureLogDirectory()
    }
    
    static SetMaxSize(size) {
        this.maxLogSize := size
    }
    
    static SetEnable(enable) {
        this.enableLog := enable
    }
    
    static SetLevel(level) {
        this.logLevel := level
    }
    
    static Clear() {
        try {
            if FileExist(this.logFile)
                FileDelete(this.logFile)
        } catch as e {
            OutputDebug("Logger: 清除日志失败: " e.Message)
        }
    }
}

LogDebug(msg) {
    Logger.Debug(msg)
}

LogInfo(msg) {
    Logger.Info(msg)
}

LogWarn(msg) {
    Logger.Warn(msg)
}

LogError(msg) {
    Logger.Error(msg)
}
