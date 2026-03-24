; ============================================
; Utils.ahk - 辅助函数库 (V1版本)
; 包含 DPI 缩放、屏幕截图、GDI 对象管理等通用函数
; ============================================

; DPI 缩放因子
global g_DpiScaleX := 1.0
global g_DpiScaleY := 1.0

; 是否已初始化 DPI
global g_DpiInitialized := false

Utils_Init() {
    Utils_GetDpiScalingFactor()
    LogInfo("辅助函数库初始化完成")
}

Utils_GetDpiScalingFactor() {
    global g_DpiScaleX, g_DpiScaleY, g_DpiInitialized
    
    if g_DpiInitialized {
        result := {}
        result.x := g_DpiScaleX
        result.y := g_DpiScaleY
        return result
    }
    
    try {
        hDC := DllCall("GetDC", "Ptr", 0, "Ptr")
        
        g_DpiScaleX := DllCall("GetDeviceCaps", "Ptr", hDC, "Int", 88) / 96.0
        g_DpiScaleY := DllCall("GetDeviceCaps", "Ptr", hDC, "Int", 90) / 96.0
        
        DllCall("ReleaseDC", "Ptr", 0, "Ptr", hDC)
    } catch {
        g_DpiScaleX := 1.0
        g_DpiScaleY := 1.0
    }
    
    g_DpiInitialized := true
    
    result := {}
    result.x := g_DpiScaleX
    result.y := g_DpiScaleY
    return result
}

Utils_ScaleX(x) {
    global g_DpiScaleX
    return Round(x * g_DpiScaleX)
}

Utils_ScaleY(y) {
    global g_DpiScaleY
    return Round(y * g_DpiScaleY)
}

Utils_UnscaleX(x) {
    global g_DpiScaleX
    return Round(x / g_DpiScaleX)
}

Utils_UnscaleY(y) {
    global g_DpiScaleY
    return Round(y / g_DpiScaleY)
}

Utils_CaptureScreen(x, y, w, h, ByRef bitmap) {
    try {
        hDC := DllCall("GetDC", "Ptr", 0, "Ptr")
        if !hDC {
            return false
        }
        
        hMemDC := DllCall("CreateCompatibleDC", "Ptr", hDC, "Ptr")
        
        hBitmap := DllCall("CreateCompatibleBitmap", "Ptr", hDC, "Int", w, "Int", h, "Ptr")
        
        hOldBitmap := DllCall("SelectObject", "Ptr", hMemDC, "Ptr", hBitmap, "Ptr")
        
        DllCall("BitBlt", "Ptr", hMemDC, "Int", 0, "Int", 0, "Int", w, "Int", h
            , "Ptr", hDC, "Int", x, "Int", y, "UInt", 0x00CC0020)
        
        DllCall("SelectObject", "Ptr", hMemDC, "Ptr", hOldBitmap, "Ptr")
        DllCall("DeleteDC", "Ptr", hMemDC)
        DllCall("ReleaseDC", "Ptr", 0, "Ptr", hDC)
        
        bitmap := hBitmap
        return true
    } catch {
        return false
    }
}

Utils_CaptureWindow(hWnd, ByRef bitmap) {
    if !hWnd {
        return false
    }
    
    WinGetPos, x, y, w, h, ahk_id %hWnd%
    
    if (w = 0 || h = 0) {
        return false
    }
    
    return Utils_CaptureScreen(x, y, w, h, bitmap)
}

Utils_CaptureWindowClient(hWnd, ByRef bitmap) {
    if !hWnd {
        return false
    }
    
    VarSetCapacity(rect, 16)
    DllCall("GetClientRect", "Ptr", hWnd, "Ptr", &rect)
    
    w := NumGet(rect, 8, "Int")
    h := NumGet(rect, 12, "Int")
    
    if (w = 0 || h = 0) {
        return false
    }
    
    VarSetCapacity(point, 8)
    NumPut(0, point, 0, "Int")
    NumPut(0, point, 4, "Int")
    DllCall("ClientToScreen", "Ptr", hWnd, "Ptr", &point)
    x := NumGet(point, 0, "Int")
    y := NumGet(point, 4, "Int")
    
    return Utils_CaptureScreen(x, y, w, h, bitmap)
}

Utils_DeleteBitmap(bitmap) {
    if bitmap {
        DllCall("DeleteObject", "Ptr", bitmap)
    }
}

Utils_GetPixelColor(x, y) {
    return DllCall("GetPixel", "Ptr", DllCall("GetDC", "Ptr", 0, "Ptr"), "Int", x, "Int", y, "UInt")
}

Utils_CompareColors(color1, color2, tolerance := 10) {
    r1 := (color1 >> 16) & 0xFF
    g1 := (color1 >> 8) & 0xFF
    b1 := color1 & 0xFF
    
    r2 := (color2 >> 16) & 0xFF
    g2 := (color2 >> 8) & 0xFF
    b2 := color2 & 0xFF
    
    dr := Abs(r1 - r2)
    dg := Abs(g1 - g2)
    db := Abs(b1 - b2)
    
    return (dr <= tolerance && dg <= tolerance && db <= tolerance)
}

Utils_ParseColor(colorStr) {
    if (SubStr(colorStr, 1, 2) = "0x") {
        return colorStr + 0
    }
    
    if (SubStr(colorStr, 1, 1) = "#") {
        colorStr := SubStr(colorStr, 2)
    }
    
    return "0x" . colorStr + 0
}

Utils_ParseRegion(regionStr) {
    parts := StrSplit(regionStr, ",")
    
    if (parts.MaxIndex() < 4) {
        result := {}
        result.x := 0
        result.y := 0
        result.w := 100
        result.h := 100
        return result
    }
    
    result := {}
    result.x := parts[1] + 0
    result.y := parts[2] + 0
    result.w := parts[3] + 0
    result.h := parts[4] + 0
    return result
}

Utils_CalculateRegion(windowW, windowH, regionNum) {
    thirdW := windowW // 3
    thirdH := windowH // 3
    
    if (regionNum = 0) {
        result := {}
        result.x := 0
        result.y := 0
        result.w := windowW
        result.h := windowH
        return result
    }
    
    col := Mod(regionNum - 1, 3)
    row := (regionNum - 1) // 3
    
    result := {}
    result.x := col * thirdW
    result.y := row * thirdH
    result.w := thirdW
    result.h := thirdH
    return result
}

Utils_GetTimestamp() {
    return A_YYYY . A_MM . A_DD . A_Hour . A_Min . A_Sec
}

Utils_GetTimestampFormatted() {
    return A_YYYY . "-" . A_MM . "-" . A_DD . " " . A_Hour . ":" . A_Min . ":" . A_Sec
}

Utils_Sleep(ms) {
    if (ms > 0) {
        Sleep, %ms%
    }
}

Utils_Random(min, max) {
    Random, result, %min%, %max%
    return result
}

Utils_Min(a, b) {
    return (a < b) ? a : b
}

Utils_Max(a, b) {
    return (a > b) ? a : b
}

Utils_Clamp(value, min, max) {
    if (value < min) {
        return min
    }
    if (value > max) {
        return max
    }
    return value
}

Utils_FileExists(filePath) {
    return FileExist(filePath) != ""
}

Utils_DirExists(dirPath) {
    return InStr(FileExist(dirPath), "D") > 0
}

Utils_EnsureDir(dirPath) {
    if !Utils_DirExists(dirPath) {
        FileCreateDir, %dirPath%
    }
}

Utils_GetScriptDir() {
    return A_ScriptDir
}

Utils_GetScriptName() {
    SplitPath, A_ScriptFullPath, name
    return name
}

Utils_Is64BitOS() {
    EnvGet, procArch, PROCESSOR_ARCHITECTURE
    return (procArch = "AMD64")
}

Utils_GetActiveWindow() {
    return WinExist("A")
}

Utils_IsWindowVisible(hWnd) {
    return DllCall("IsWindowVisible", "Ptr", hWnd, "UInt")
}

Utils_IsWindow(hWnd) {
    return DllCall("IsWindow", "Ptr", hWnd, "UInt")
}
