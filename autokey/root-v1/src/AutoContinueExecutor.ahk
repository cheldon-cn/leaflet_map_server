; ============================================
; AutoContinueExecutor.ahk - 自动继续执行模块 (V1版本)
; 负责执行自动继续操作：点击输入框、输入文本、点击按钮
; ============================================

; 执行计数器
global g_ExecuteCount := 0

; 上次执行时间
global g_LastExecuteTime := 0

; 执行状态
global g_Executing := false

AutoContinueExecutor_Init() {
    global g_ExecuteCount, g_LastExecuteTime, g_Executing
    
    g_ExecuteCount := 0
    g_LastExecuteTime := 0
    g_Executing := false
    
    LogInfo("自动继续执行模块初始化完成")
}

AutoContinueExecutor_Execute() {
    global g_ExecuteCount, g_LastExecuteTime, g_Executing, g_Config
    
    if g_Executing {
        LogWarn("正在执行中，跳过本次请求")
        return false
    }
    
    g_Executing := true
    
    try {
        if !WindowDetector_IsWindowValid() {
            LogError("窗口无效，无法执行")
            g_Executing := false
            return false
        }
        
        if WindowDetector_IsWindowMinimized() {
            LogInfo("窗口已最小化，尝试恢复")
            WindowDetector_RestoreWindow()
            Sleep, 500
        }
        
        WindowDetector_ActivateWindow()
        Sleep, 200
        
        randomDelay := AutoContinueExecutor_GetRandomDelay()
        LogInfo("响应前随机延迟: " . (randomDelay / 1000) . " 秒")
        Sleep, %randomDelay%
        
        if !AutoContinueExecutor_ClickInputArea() {
            LogError("点击输入区域失败")
            g_Executing := false
            return false
        }
        
        Sleep, 100
        
        if !AutoContinueExecutor_InputText() {
            LogError("输入文本失败")
            g_Executing := false
            return false
        }
        
        Sleep, 100
        
        if !AutoContinueExecutor_ClickSendButton() {
            LogError("点击发送按钮失败")
            g_Executing := false
            return false
        }
        
        g_ExecuteCount++
        g_LastExecuteTime := A_TickCount
        
        LogInfo("自动继续执行成功 (第 " . g_ExecuteCount . " 次)")
        
        g_Executing := false
        return true
        
    } catch {
        LogError("执行过程中发生异常")
        g_Executing := false
        return false
    }
}

AutoContinueExecutor_GetRandomDelay() {
    global g_Config
    
    minDelay := g_Config.General.ResponseDelayMin
    maxDelay := g_Config.General.ResponseDelayMax
    
    if (minDelay < 0) {
        minDelay := 10000
    }
    if (maxDelay < minDelay) {
        maxDelay := minDelay
    }
    
    range := maxDelay - minDelay
    Random, rand, 0, 1.0
    delay := minDelay + Round(range * rand)
    
    return delay
}

AutoContinueExecutor_ClickInputArea() {
    global g_Config
    
    inputPos := WindowDetector_CalculateInputPosition()
    inputX := inputPos.x
    inputY := inputPos.y
    
    CoordMode, Mouse, Screen
    
    MouseMove, %inputX%, %inputY%, 5
    Sleep, 50
    Click, %inputX%, %inputY%
    
    LogDebug("点击输入区域: (" . inputX . ", " . inputY . ")")
    
    return true
}

AutoContinueExecutor_InputText() {
    global g_InputText
    
    text := KeywordDetector_GetInputText()
    
    if (StrLen(text) = 0) {
        text := "继续"
    }
    
    Clipboard := text
    Sleep, 50
    
    Send, ^v
    
    LogDebug("输入文本: " . text)
    
    return true
}

AutoContinueExecutor_ClickSendButton() {
    buttonPos := WindowDetector_CalculateButtonPosition()
    buttonX := buttonPos.x
    buttonY := buttonPos.y
    
    CoordMode, Mouse, Screen
    
    MouseMove, %buttonX%, %buttonY%, 5
    Sleep, 50
    Click, %buttonX%, %buttonY%
    
    LogDebug("点击发送按钮: (" . buttonX . ", " . buttonY . ")")
    
    return true
}

AutoContinueExecutor_GetExecuteCount() {
    global g_ExecuteCount
    return g_ExecuteCount
}

AutoContinueExecutor_GetLastExecuteTime() {
    global g_LastExecuteTime
    return g_LastExecuteTime
}

AutoContinueExecutor_IsExecuting() {
    global g_Executing
    return g_Executing
}

AutoContinueExecutor_ResetCount() {
    global g_ExecuteCount
    g_ExecuteCount := 0
    LogInfo("执行计数器已重置")
}

AutoContinueExecutor_VerifyResult() {
    Sleep, 500
    
    return true
}

AutoContinueExecutor_ExecuteWithRetry(maxRetries := 3) {
    global g_Config
    
    if !IsObject(g_Config.Retry) {
        return AutoContinueExecutor_Execute()
    }
    
    retryCount := 0
    
    Loop, %maxRetries% {
        if AutoContinueExecutor_Execute() {
            return true
        }
        
        retryCount++
        
        if (retryCount < maxRetries) {
            delay := g_Config.Retry.RetryBaseDelay * (2 ** (retryCount - 1))
            if (delay > g_Config.Retry.RetryMaxDelay) {
                delay := g_Config.Retry.RetryMaxDelay
            }
            
            LogInfo("执行失败，" . delay . "ms 后重试 (" . retryCount . "/" . maxRetries . ")")
            Sleep, %delay%
        }
    }
    
    LogError("执行失败，已达到最大重试次数")
    return false
}

AutoContinueExecutor_SetInputPosition(x, y) {
    global g_WindowRect
    
    g_WindowRect.inputX := x
    g_WindowRect.inputY := y
    
    LogDebug("设置输入位置: (" . x . ", " . y . ")")
}

AutoContinueExecutor_SetButtonPosition(x, y) {
    global g_WindowRect
    
    g_WindowRect.buttonX := x
    g_WindowRect.buttonY := y
    
    LogDebug("设置按钮位置: (" . x . ", " . y . ")")
}
