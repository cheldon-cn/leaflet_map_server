; ============================================
; WindowDetector.ahk - 窗口检测模块 (V1版本)
; 负责检测 Trae IDE 窗口、获取窗口位置和大小
; ============================================

; 当前检测到的窗口句柄
global g_TraeWindowHwnd := ""

; 窗口位置缓存
global g_WindowRect := {x: 0, y: 0, w: 0, h: 0}

WindowDetector_Init() {
    global g_Config
    
    matchMode := g_Config.Window.TitleMatchMode
    SetTitleMatchMode, %matchMode%
    LogInfo("窗口检测模块初始化完成 (匹配模式: " . matchMode . ")")
}

WindowDetector_DetectTraeWindow() {
    global g_TraeWindowHwnd, g_Config
    
    windowTitle := g_Config.Window.WindowTitle
    
    hWnd := WinExist(windowTitle)
    
    if hWnd {
        g_TraeWindowHwnd := hWnd
        WindowDetector_UpdateWindowRect()
        LogInfo("检测到 Trae 窗口: " . hWnd . " (标题包含: " . windowTitle . ")")
        return true
    }
    
    g_TraeWindowHwnd := ""
    LogDebug("未检测到包含 '" . windowTitle . "' 的窗口")
    return false
}

WindowDetector_GetWindowHwnd() {
    global g_TraeWindowHwnd
    return g_TraeWindowHwnd
}

WindowDetector_UpdateWindowRect() {
    global g_TraeWindowHwnd, g_WindowRect
    
    if !g_TraeWindowHwnd {
        return false
    }
    
    WinGetPos, x, y, w, h, ahk_id %g_TraeWindowHwnd%
    
    if (w = 0 || h = 0) {
        return false
    }
    
    g_WindowRect.x := x
    g_WindowRect.y := y
    g_WindowRect.w := w
    g_WindowRect.h := h
    
    return true
}

WindowDetector_GetWindowRect() {
    global g_WindowRect
    return g_WindowRect
}

WindowDetector_GetClientRect() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return {x: 0, y: 0, w: 0, h: 0}
    }
    
    VarSetCapacity(rect, 16)
    DllCall("GetClientRect", "Ptr", g_TraeWindowHwnd, "Ptr", &rect)
    
    w := NumGet(rect, 8, "Int")
    h := NumGet(rect, 12, "Int")
    
    VarSetCapacity(point, 8)
    NumPut(0, point, 0, "Int")
    NumPut(0, point, 4, "Int")
    DllCall("ClientToScreen", "Ptr", g_TraeWindowHwnd, "Ptr", &point)
    x := NumGet(point, 0, "Int")
    y := NumGet(point, 4, "Int")
    
    return {x: x, y: y, w: w, h: h}
}

WindowDetector_IsWindowVisible() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return false
    }
    
    return DllCall("IsWindowVisible", "Ptr", g_TraeWindowHwnd, "UInt")
}

WindowDetector_IsWindowMinimized() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return true
    }
    
    WinGet, minState, MinMax, ahk_id %g_TraeWindowHwnd%
    
    return (minState = -1)
}

WindowDetector_ActivateWindow() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return false
    }
    
    try {
        WinActivate, ahk_id %g_TraeWindowHwnd%
        WinWaitActive, ahk_id %g_TraeWindowHwnd%, , 2
        return true
    } catch {
        return false
    }
}

WindowDetector_RestoreWindow() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return false
    }
    
    if WindowDetector_IsWindowMinimized() {
        WinRestore, ahk_id %g_TraeWindowHwnd%
        Sleep, 200
    }
    
    return true
}

WindowDetector_GetWindowPosition() {
    global g_WindowRect
    return {x: g_WindowRect.x, y: g_WindowRect.y}
}

WindowDetector_GetWindowSize() {
    global g_WindowRect
    return {w: g_WindowRect.w, h: g_WindowRect.h}
}

WindowDetector_CalculateInputPosition() {
    global g_WindowRect, g_Config
    
    absPos := g_Config.InputPosition.InputAbsolutePos
    
    if (absPos != "" && absPos != "-1,-1") {
        parts := StrSplit(absPos, ",")
        if (parts.MaxIndex() >= 2) {
            clientRect := WindowDetector_GetClientRect()
            inputX := clientRect.x + parts[1]
            inputY := clientRect.y + parts[2]
            return {x: inputX, y: inputY}
        }
    }
    
    clientRect := WindowDetector_GetClientRect()
    inputX := clientRect.x + Round(clientRect.w * g_Config.InputPosition.InputOffsetX)
    inputY := clientRect.y + Round(clientRect.h * g_Config.InputPosition.InputOffsetY)
    
    return {x: inputX, y: inputY}
}

WindowDetector_CalculateButtonPosition() {
    global g_WindowRect, g_Config
    
    absPos := g_Config.ButtonPosition.ButtonAbsolutePos
    
    if (absPos != "" && absPos != "-1,-1") {
        parts := StrSplit(absPos, ",")
        if (parts.MaxIndex() >= 2) {
            clientRect := WindowDetector_GetClientRect()
            buttonX := clientRect.x + parts[1]
            buttonY := clientRect.y + parts[2]
            return {x: buttonX, y: buttonY}
        }
    }
    
    clientRect := WindowDetector_GetClientRect()
    buttonX := clientRect.x + Round(clientRect.w * g_Config.ButtonPosition.ButtonOffsetX)
    buttonY := clientRect.y + Round(clientRect.h * g_Config.ButtonPosition.ButtonOffsetY)
    
    return {x: buttonX, y: buttonY}
}

WindowDetector_CalculateRegionPosition(regionNum) {
    global g_WindowRect
    
    clientRect := WindowDetector_GetClientRect()
    
    region := Utils_CalculateRegion(clientRect.w, clientRect.h, regionNum)
    
    region.x += clientRect.x
    region.y += clientRect.y
    
    return region
}

WindowDetector_IsWindowValid() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return false
    }
    
    return DllCall("IsWindow", "Ptr", g_TraeWindowHwnd, "UInt")
}

WindowDetector_Refresh() {
    if WindowDetector_IsWindowValid() {
        return WindowDetector_UpdateWindowRect()
    }
    
    return WindowDetector_DetectTraeWindow()
}

WindowDetector_WaitForWindow(timeout := 5000) {
    global g_Config
    
    startTime := A_TickCount
    
    Loop {
        if WindowDetector_DetectTraeWindow() {
            return true
        }
        
        if (A_TickCount - startTime > timeout) {
            return false
        }
        
        Sleep, 500
    }
}
