class HeartbeatMonitor {
    static lastHeartbeat := 0
    static heartbeatInterval := 60000
    static enabled := true
    static heartbeatCount := 0
    
    static Initialize(config := "") {
        if (config != "" && config.Has("HeartbeatInterval"))
            this.heartbeatInterval := config["HeartbeatInterval"]
        
        this.lastHeartbeat := A_TickCount
        LogInfo("HeartbeatMonitor 初始化完成, 间隔: " this.heartbeatInterval "ms")
    }
    
    static Beat() {
        this.lastHeartbeat := A_TickCount
        this.heartbeatCount++
        LogDebug("心跳 #" this.heartbeatCount)
    }
    
    static Check() {
        if !this.enabled
            return true
        
        elapsed := A_TickCount - this.lastHeartbeat
        return (elapsed < this.heartbeatInterval * 2)
    }
    
    static GetTimeSinceLastBeat() {
        return A_TickCount - this.lastHeartbeat
    }
    
    static IsHealthy() {
        return this.Check()
    }
    
    static Enable() {
        this.enabled := true
        LogInfo("心跳监控已启用")
    }
    
    static Disable() {
        this.enabled := false
        LogInfo("心跳监控已禁用")
    }
    
    static GetStats() {
        return {
            lastHeartbeat: this.lastHeartbeat,
            heartbeatCount: this.heartbeatCount,
            timeSinceLastBeat: this.GetTimeSinceLastBeat(),
            isHealthy: this.IsHealthy()
        }
    }
    
    static Reset() {
        this.lastHeartbeat := A_TickCount
        this.heartbeatCount := 0
    }
}
