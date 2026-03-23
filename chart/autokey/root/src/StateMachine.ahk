class StateMachine {
    static STATE_COOLDOWN := "COOLDOWN"
    static STATE_DETECTING := "DETECTING"
    static STATE_POST_EXECUTION := "POST_EXECUTION"
    
    static currentState := ""
    static stateStartTime := 0
    static cooldownPeriod := 900000
    static checkInterval := 10000
    static postExecutionDelay := 30000
    static lastCheckTime := 0
    static stateTransitions := 0
    
    static Initialize(config := "") {
        if (config != "" && config.Has("CooldownPeriod"))
            this.cooldownPeriod := config["CooldownPeriod"]
        if (config != "" && config.Has("CheckInterval"))
            this.checkInterval := config["CheckInterval"]
        if (config != "" && config.Has("PostExecutionDelay"))
            this.postExecutionDelay := config["PostExecutionDelay"]
        
        this.currentState := this.STATE_COOLDOWN
        this.stateStartTime := A_TickCount
        
        LogInfo("StateMachine 初始化完成, 初始状态: " this.currentState)
        LogInfo(Format("冷却期: {}ms, 检测间隔: {}ms, 执行后等待: {}ms"
            , this.cooldownPeriod, this.checkInterval, this.postExecutionDelay))
    }
    
    static GetCurrentState() {
        return this.currentState
    }
    
    static Update() {
        elapsed := A_TickCount - this.stateStartTime
        
        switch this.currentState {
            case this.STATE_COOLDOWN:
                if (elapsed >= this.cooldownPeriod) {
                    this.TransitionTo(this.STATE_DETECTING)
                    return true
                }
                
            case this.STATE_DETECTING:
                if (A_TickCount - this.lastCheckTime >= this.checkInterval) {
                    this.lastCheckTime := A_TickCount
                    return true
                }
                
            case this.STATE_POST_EXECUTION:
                if (elapsed >= this.postExecutionDelay) {
                    this.TransitionTo(this.STATE_COOLDOWN)
                    return true
                }
        }
        
        return false
    }
    
    static ShouldCheck() {
        return (this.currentState = this.STATE_DETECTING 
            && A_TickCount - this.lastCheckTime >= this.checkInterval)
    }
    
    static OnDetected() {
        if (this.currentState = this.STATE_DETECTING) {
            this.TransitionTo(this.STATE_POST_EXECUTION)
            return true
        }
        return false
    }
    
    static TransitionTo(newState) {
        oldState := this.currentState
        this.currentState := newState
        this.stateStartTime := A_TickCount
        this.stateTransitions++
        
        LogInfo(Format("状态转换: {} -> {}", oldState, newState))
        
        if (newState = this.STATE_DETECTING) {
            this.lastCheckTime := A_TickCount
        }
    }
    
    static GetStateDuration() {
        return A_TickCount - this.stateStartTime
    }
    
    static GetRemainingTime() {
        elapsed := this.GetStateDuration()
        
        switch this.currentState {
            case this.STATE_COOLDOWN:
                return Max(0, this.cooldownPeriod - elapsed)
            case this.STATE_POST_EXECUTION:
                return Max(0, this.postExecutionDelay - elapsed)
            case this.STATE_DETECTING:
                return Max(0, this.checkInterval - (A_TickCount - this.lastCheckTime))
        }
        
        return 0
    }
    
    static ForceState(state) {
        this.currentState := state
        this.stateStartTime := A_TickCount
        LogInfo("强制切换状态到: " state)
    }
    
    static Reset() {
        this.currentState := this.STATE_COOLDOWN
        this.stateStartTime := A_TickCount
        this.lastCheckTime := 0
        LogInfo("状态机已重置")
    }
    
    static GetStats() {
        return {
            currentState: this.currentState,
            stateDuration: this.GetStateDuration(),
            remainingTime: this.GetRemainingTime(),
            stateTransitions: this.stateTransitions
        }
    }
}
