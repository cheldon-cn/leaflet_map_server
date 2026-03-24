; ============================================
; RetryManager.ahk - 重试管理模块 (V1版本)
; 支持指数退避重试、失败计数和自动重启
; ============================================

; 连续失败计数
global g_ConsecutiveFailures := 0

; 总失败计数
global g_TotalFailures := 0

; 总成功计数
global g_TotalSuccesses := 0

; 上次失败时间
global g_LastFailureTime := 0

RetryManager_Init() {
    global g_ConsecutiveFailures, g_TotalFailures, g_TotalSuccesses, g_LastFailureTime
    
    g_ConsecutiveFailures := 0
    g_TotalFailures := 0
    g_TotalSuccesses := 0
    g_LastFailureTime := 0
    
    LogInfo("重试管理模块初始化完成")
}

RetryManager_ExecuteWithRetry(func, maxRetries := 0) {
    global g_Config
    
    if (maxRetries = 0) {
        maxRetries := g_Config.Retry.MaxRetryCount
    }
    
    retryCount := 0
    lastError := ""
    
    Loop, %maxRetries% {
        try {
            result := %func%()
            
            if result {
                RetryManager_RecordSuccess()
                return true
            }
        } catch {
            lastError := "执行异常"
        }
        
        retryCount++
        
        if (retryCount < maxRetries) {
            delay := RetryManager_CalculateDelay(retryCount)
            LogInfo("重试 " . retryCount . "/" . maxRetries . ", 延迟 " . delay . "ms")
            Sleep, %delay%
        }
    }
    
    RetryManager_RecordFailure()
    LogError("重试失败，已达到最大重试次数: " . maxRetries)
    return false
}

RetryManager_CalculateDelay(retryCount) {
    global g_Config
    
    baseDelay := g_Config.Retry.RetryBaseDelay
    maxDelay := g_Config.Retry.RetryMaxDelay
    
    delay := baseDelay * (2 ** (retryCount - 1))
    
    jitter := Utils_Random(0, delay // 4)
    delay := delay + jitter
    
    if (delay > maxDelay) {
        delay := maxDelay
    }
    
    return delay
}

RetryManager_RecordSuccess() {
    global g_ConsecutiveFailures, g_TotalSuccesses
    
    g_ConsecutiveFailures := 0
    g_TotalSuccesses++
    
    LogDebug("操作成功，连续失败计数重置")
}

RetryManager_RecordFailure() {
    global g_ConsecutiveFailures, g_TotalFailures, g_LastFailureTime, g_Config
    
    g_ConsecutiveFailures++
    g_TotalFailures++
    g_LastFailureTime := A_TickCount
    
    LogWarn("操作失败，连续失败次数: " . g_ConsecutiveFailures)
    
    threshold := g_Config.Retry.ConsecutiveFailureThreshold
    
    if (g_ConsecutiveFailures >= threshold) {
        LogError("连续失败达到阈值 (" . threshold . ")，建议重启脚本")
        RetryManager_TriggerRestart()
    }
}

RetryManager_TriggerRestart() {
    LogWarn("触发自动重启机制")
    
    TrayTip, Trae Auto, 连续失败过多，即将重启脚本, 5, 2
    
    Sleep, 3000
    
    Reload
}

RetryManager_GetConsecutiveFailures() {
    global g_ConsecutiveFailures
    return g_ConsecutiveFailures
}

RetryManager_GetTotalFailures() {
    global g_TotalFailures
    return g_TotalFailures
}

RetryManager_GetTotalSuccesses() {
    global g_TotalSuccesses
    return g_TotalSuccesses
}

RetryManager_ResetConsecutiveFailures() {
    global g_ConsecutiveFailures
    g_ConsecutiveFailures := 0
    LogInfo("连续失败计数已重置")
}

RetryManager_ResetAll() {
    global g_ConsecutiveFailures, g_TotalFailures, g_TotalSuccesses
    
    g_ConsecutiveFailures := 0
    g_TotalFailures := 0
    g_TotalSuccesses := 0
    
    LogInfo("所有重试计数器已重置")
}

RetryManager_GetFailureRate() {
    global g_TotalFailures, g_TotalSuccesses
    
    total := g_TotalFailures + g_TotalSuccesses
    
    if (total = 0) {
        return 0
    }
    
    return (g_TotalFailures / total) * 100
}

RetryManager_ShouldRetry(errorType := "") {
    global g_Config
    
    if (g_ConsecutiveFailures >= g_Config.Retry.ConsecutiveFailureThreshold) {
        return false
    }
    
    return true
}

RetryManager_GetStatus() {
    global g_ConsecutiveFailures, g_TotalFailures, g_TotalSuccesses, g_LastFailureTime
    
    result := {}
    result.consecutiveFailures := g_ConsecutiveFailures
    result.totalFailures := g_TotalFailures
    result.totalSuccesses := g_TotalSuccesses
    result.lastFailureTime := g_LastFailureTime
    result.failureRate := RetryManager_GetFailureRate()
    return result
}

RetryManager_LogStatus() {
    status := RetryManager_GetStatus()
    
    LogInfo("重试状态 - 成功: " . status.totalSuccesses 
        . ", 失败: " . status.totalFailures 
        . ", 连续失败: " . status.consecutiveFailures
        . ", 失败率: " . Round(status.failureRate, 2) . "%")
}
