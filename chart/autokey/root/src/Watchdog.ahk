class Watchdog {
    static checkInterval := 300000
    static maxMemoryMB := 500
    static lastCheck := 0
    static enabled := true
    static restartCallback := ""
    
    static Initialize(config := "") {
        if (config != "" && config.Has("WatchdogInterval"))
            this.checkInterval := config["WatchdogInterval"]
        if (config != "" && config.Has("MaxMemoryMB"))
            this.maxMemoryMB := config["MaxMemoryMB"]
        
        this.lastCheck := A_TickCount
        LogInfo("Watchdog 初始化完成, 检查间隔: " this.checkInterval "ms")
    }
    
    static SetRestartCallback(callback) {
        this.restartCallback := callback
    }
    
    static Check() {
        if !this.enabled
            return true
        
        if (A_TickCount - this.lastCheck < this.checkInterval)
            return true
        
        this.lastCheck := A_TickCount
        
        memoryOK := this.CheckMemory()
        if !memoryOK {
            LogWarn("内存使用过高, 触发重启")
            this.TriggerRestart()
            return false
        }
        
        LogDebug("Watchdog 检查通过")
        return true
    }
    
    static CheckMemory() {
        try {
            processName := A_ScriptName
            if (processName = "")
                return true
            
            for process in ComObjGet("winmgmts:").ExecQuery(
                "Select * from Win32_Process Where Name like '%AutoHotkey%'") {
                memoryMB := process.WorkingSetSize / 1024 / 1024
                if (memoryMB > this.maxMemoryMB) {
                    LogWarn(Format("内存使用过高: {:.2f}MB > {}MB", memoryMB, this.maxMemoryMB))
                    return false
                }
            }
            return true
        } catch as e {
            LogDebug("内存检查失败: " e.Message)
            return true
        }
    }
    
    static TriggerRestart() {
        LogWarn("触发看门狗重启")
        
        if (this.restartCallback != "") {
            try {
                this.restartCallback()
            } catch as e {
                LogError("重启回调失败: " e.Message)
            }
        }
    }
    
    static Enable() {
        this.enabled := true
        LogInfo("看门狗已启用")
    }
    
    static Disable() {
        this.enabled := false
        LogInfo("看门狗已禁用")
    }
    
    static GetStats() {
        return {
            lastCheck: this.lastCheck,
            enabled: this.enabled,
            checkInterval: this.checkInterval,
            maxMemoryMB: this.maxMemoryMB
        }
    }
}
