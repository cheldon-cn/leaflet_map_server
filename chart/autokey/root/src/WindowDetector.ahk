class WindowDetector {
    static targetWindow := "ahk_exe Trae CN.exe"
    static lastWindowID := 0
    
    static Initialize(config := "") {
        if (config != "" && config.Has("TargetWindow"))
            this.targetWindow := config["TargetWindow"]
        LogInfo("WindowDetector 初始化完成, 目标窗口: " this.targetWindow)
    }
    
    static DetectTraeWindow() {
        try {
            if !WinExist(this.targetWindow) {
                LogDebug("Trae 窗口不存在")
                return 0
            }
            
            winID := WinGetID(this.targetWindow)
            winState := WinGetMinMax(this.targetWindow)
            
            if (winState = -1) {
                LogDebug("窗口已最小化, 尝试恢复")
                WinRestore "ahk_id " winID
                Sleep(500)
            }
            
            WinActivate "ahk_id " winID
            try {
                WinWaitActive "ahk_id " winID, , 2
            } catch {
                LogError("窗口激活失败")
                return 0
            }
            
            this.lastWindowID := winID
            return winID
        } catch as e {
            LogError("检测窗口失败: " e.Message)
            return 0
        }
    }
    
    static GetWindowRect(winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return {x: 0, y: 0, w: 0, h: 0}
        
        try {
            WinGetPos(&x, &y, &w, &h, "ahk_id " winID)
            
            dpiFactor := Utils.GetDpiScalingFactor()
            
            return {
                x: x,
                y: y,
                w: Round(w * dpiFactor),
                h: Round(h * dpiFactor)
            }
        } catch as e {
            LogError("获取窗口位置失败: " e.Message)
            return {x: 0, y: 0, w: 0, h: 0}
        }
    }
    
    static IsWindowActive(winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return false
        
        try {
            activeID := WinGetID("A")
            return (activeID = winID)
        } catch {
            return false
        }
    }
    
    static ActivateWindow(winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return false
        
        try {
            WinActivate "ahk_id " winID
            WinWaitActive "ahk_id " winID, , 2
            return true
        } catch {
            return false
        }
    }
    
    static GetClientArea(winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return {x: 0, y: 0, w: 0, h: 0}
        
        try {
            rect := Buffer(16, 0)
            DllCall("GetClientRect", "Ptr", winID, "Ptr", rect)
            
            x := NumGet(rect, 0, "Int")
            y := NumGet(rect, 4, "Int")
            w := NumGet(rect, 8, "Int")
            h := NumGet(rect, 12, "Int")
            
            dpiFactor := Utils.GetDpiScalingFactor()
            
            return {
                x: x,
                y: y,
                w: Round(w * dpiFactor),
                h: Round(h * dpiFactor)
            }
        } catch as e {
            LogError("获取客户区失败: " e.Message)
            return {x: 0, y: 0, w: 0, h: 0}
        }
    }
    
    static WindowToScreen(clientX, clientY, winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return {x: clientX, y: clientY}
        
        try {
            point := Buffer(8, 0)
            NumPut("Int", clientX, point, 0)
            NumPut("Int", clientY, point, 4)
            
            DllCall("ClientToScreen", "Ptr", winID, "Ptr", point)
            
            return {
                x: NumGet(point, 0, "Int"),
                y: NumGet(point, 4, "Int")
            }
        } catch {
            return {x: clientX, y: clientY}
        }
    }
    
    static IsWindowMinimized(winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return false
        
        try {
            return (WinGetMinMax("ahk_id " winID) = -1)
        } catch {
            return false
        }
    }
    
    static RestoreWindow(winID := 0) {
        if (winID = 0)
            winID := this.lastWindowID
        
        if (winID = 0)
            return false
        
        try {
            WinRestore "ahk_id " winID
            return true
        } catch {
            return false
        }
    }
}
