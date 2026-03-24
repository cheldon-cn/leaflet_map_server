; ============================================
; HeartbeatMonitor.ahk - 心跳检测模块 (V1版本)
; 定期更新心跳时间戳，检测心跳超时
; ============================================

; 心跳状态
global g_HeartbeatActive := false

; 上次心跳时间
global g_LastHeartbeat := 0

; 心跳计数
global g_HeartbeatCount := 0

HeartbeatMonitor_Init() {
    global g_HeartbeatActive, g_LastHeartbeat, g_HeartbeatCount, g_Config
    
    g_HeartbeatActive := true
    g_LastHeartbeat := A_TickCount
    g_HeartbeatCount := 0
    
    interval := g_Config.Heartbeat.HeartbeatInterval
    SetTimer, HeartbeatMonitor_Update, %interval%
    
    LogInfo("心跳检测模块初始化完成，间隔: " . interval . "ms")
}

HeartbeatMonitor_Update() {
    global g_HeartbeatActive, g_LastHeartbeat, g_HeartbeatCount
    
    if !g_HeartbeatActive {
        return
    }
    
    g_LastHeartbeat := A_TickCount
    g_HeartbeatCount++
    
    LogDebug("心跳更新 #" . g_HeartbeatCount)
}

HeartbeatMonitor_Check() {
    global g_HeartbeatActive, g_LastHeartbeat, g_Config
    
    if !g_HeartbeatActive {
        return {healthy: false, reason: "心跳未激活"}
    }
    
    elapsed := A_TickCount - g_LastHeartbeat
    timeout := g_Config.Heartbeat.HeartbeatTimeout
    
    if (elapsed > timeout) {
        return {healthy: false, reason: "心跳超时", elapsed: elapsed, timeout: timeout}
    }
    
    return {healthy: true, elapsed: elapsed}
}

HeartbeatMonitor_GetLastHeartbeat() {
    global g_LastHeartbeat
    return g_LastHeartbeat
}

HeartbeatMonitor_GetHeartbeatCount() {
    global g_HeartbeatCount
    return g_HeartbeatCount
}

HeartbeatMonitor_IsActive() {
    global g_HeartbeatActive
    return g_HeartbeatActive
}

HeartbeatMonitor_Stop() {
    global g_HeartbeatActive
    
    g_HeartbeatActive := false
    SetTimer, HeartbeatMonitor_Update, Off
    
    LogInfo("心跳检测已停止")
}

HeartbeatMonitor_Start() {
    global g_HeartbeatActive, g_Config
    
    g_HeartbeatActive := true
    g_LastHeartbeat := A_TickCount
    
    interval := g_Config.Heartbeat.HeartbeatInterval
    SetTimer, HeartbeatMonitor_Update, %interval%
    
    LogInfo("心跳检测已启动")
}

HeartbeatMonitor_Reset() {
    global g_LastHeartbeat, g_HeartbeatCount
    
    g_LastHeartbeat := A_TickCount
    g_HeartbeatCount := 0
    
    LogInfo("心跳计数器已重置")
}

HeartbeatMonitor_GetUptime() {
    global g_HeartbeatCount, g_Config
    
    interval := g_Config.Heartbeat.HeartbeatInterval
    uptimeMs := g_HeartbeatCount * interval
    
    return uptimeMs
}

HeartbeatMonitor_GetUptimeFormatted() {
    uptimeMs := HeartbeatMonitor_GetUptime()
    
    seconds := uptimeMs // 1000
    minutes := seconds // 60
    hours := minutes // 60
    
    seconds := Mod(seconds, 60)
    minutes := Mod(minutes, 60)
    
    return hours . "h " . minutes . "m " . seconds . "s"
}

HeartbeatMonitor_ForceUpdate() {
    global g_LastHeartbeat, g_HeartbeatCount
    
    g_LastHeartbeat := A_TickCount
    g_HeartbeatCount++
    
    LogDebug("强制心跳更新")
}
