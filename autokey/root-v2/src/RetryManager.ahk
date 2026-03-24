class RetryManager {
    static maxRetries := 3
    static retryDelay := 1000
    static consecutiveFailures := 0
    static consecutiveFailuresThreshold := 5
    static totalAttempts := 0
    static totalSuccesses := 0
    static totalFailures := 0
    
    static Initialize(config := "") {
        if (config != "" && config.Has("MaxRetries"))
            this.maxRetries := config["MaxRetries"]
        if (config != "" && config.Has("RetryDelay"))
            this.retryDelay := config["RetryDelay"]
        if (config != "" && config.Has("ConsecutiveFailuresThreshold"))
            this.consecutiveFailuresThreshold := config["ConsecutiveFailuresThreshold"]
        
        LogInfo("RetryManager 初始化完成, 最大重试: " this.maxRetries)
    }
    
    static ExecuteWithRetry(callback, maxRetries := 0) {
        if (maxRetries = 0)
            maxRetries := this.maxRetries
        
        this.totalAttempts++
        
        loop maxRetries {
            attempt := A_Index
            
            try {
                result := callback.Call()
                
                if (result) {
                    this.OnSuccess()
                    return true
                }
            } catch as e {
                LogWarn(Format("尝试 {} 失败: {}", attempt, e.Message))
            }
            
            if (attempt < maxRetries) {
                LogInfo(Format("等待 {}ms 后重试...", this.retryDelay))
                Sleep(this.retryDelay)
            }
        }
        
        this.OnFailure()
        return false
    }
    
    static OnSuccess() {
        this.consecutiveFailures := 0
        this.totalSuccesses++
        LogDebug("操作成功, 连续失败计数重置")
    }
    
    static OnFailure() {
        this.consecutiveFailures++
        this.totalFailures++
        LogWarn(Format("操作失败, 连续失败次数: {}", this.consecutiveFailures))
    }
    
    static ShouldRestart() {
        return (this.consecutiveFailures >= this.consecutiveFailuresThreshold)
    }
    
    static ResetConsecutiveFailures() {
        this.consecutiveFailures := 0
        LogInfo("连续失败计数已重置")
    }
    
    static GetStats() {
        return {
            totalAttempts: this.totalAttempts,
            totalSuccesses: this.totalSuccesses,
            totalFailures: this.totalFailures,
            consecutiveFailures: this.consecutiveFailures,
            successRate: this.totalAttempts > 0 
                ? (this.totalSuccesses / this.totalAttempts * 100) 
                : 0
        }
    }
    
    static SetMaxRetries(max) {
        this.maxRetries := max
    }
    
    static SetRetryDelay(delay) {
        this.retryDelay := delay
    }
    
    static SetConsecutiveFailuresThreshold(threshold) {
        this.consecutiveFailuresThreshold := threshold
    }
}
