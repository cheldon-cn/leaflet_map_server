class Utils {
    static dpiCache := 0
    
    static GetDpiScalingFactor() {
        if (this.dpiCache != 0)
            return this.dpiCache
        
        try {
            hdc := DllCall("GetDC", "Ptr", 0, "Ptr")
            dpi := DllCall("GetDeviceCaps", "Ptr", hdc, "Int", 90)
            DllCall("ReleaseDC", "Ptr", 0, "Ptr", hdc)
            
            this.dpiCache := dpi / 96.0
            return this.dpiCache
        } catch {
            return 1.0
        }
    }
    
    static CaptureScreen(x, y, w, h) {
        try {
            hdcScreen := DllCall("GetDC", "Ptr", 0, "Ptr")
            hdcMem := DllCall("CreateCompatibleDC", "Ptr", hdcScreen, "Ptr")
            hBitmap := DllCall("CreateCompatibleBitmap", "Ptr", hdcScreen, "Int", w, "Int", h, "Ptr")
            
            DllCall("SelectObject", "Ptr", hdcMem, "Ptr", hBitmap, "Ptr")
            DllCall("BitBlt", "Ptr", hdcMem, "Int", 0, "Int", 0, "Int", w, "Int", h
                , "Ptr", hdcScreen, "Int", x, "Int", y, "UInt", 0x00CC0020)
            
            DllCall("DeleteDC", "Ptr", hdcMem)
            DllCall("ReleaseDC", "Ptr", 0, "Ptr", hdcScreen)
            
            return hBitmap
        } catch as e {
            OutputDebug("Utils: 截图失败: " e.Message)
            return 0
        }
    }
    
    static DeleteBitmap(hBitmap) {
        if (hBitmap != 0) {
            try {
                DllCall("DeleteObject", "Ptr", hBitmap)
            } catch {
            }
        }
    }
    
    static StrJoin(arr, delimiter := ",") {
        result := ""
        for i, item in arr {
            if (i > 1)
                result .= delimiter
            result .= String(item)
        }
        return result
    }
    
    static IsProcessRunning(processName) {
        try {
            pid := ProcessExist(processName)
            return (pid != 0)
        } catch {
            return false
        }
    }
    
    static EnsureDirectory(dirPath) {
        if !DirExist(dirPath) {
            try {
                DirCreate(dirPath)
                return true
            } catch as e {
                OutputDebug("Utils: 创建目录失败: " e.Message)
                return false
            }
        }
        return true
    }
    
    static FileExists(filePath) {
        return FileExist(filePath) != ""
    }
    
    static GetTimestamp() {
        return A_Now
    }
    
    static FormatTimestamp(timestamp := "") {
        if (timestamp = "")
            timestamp := A_Now
        return FormatTime(timestamp, "yyyy-MM-dd HH:mm:ss")
    }
    
    static RandomInt(min, max) {
        return Random(min, max)
    }
    
    static Clamp(value, min, max) {
        if (value < min)
            return min
        if (value > max)
            return max
        return value
    }
    
    static MapRange(value, inMin, inMax, outMin, outMax) {
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin
    }
    
    static Sleep(ms) {
        Sleep(ms)
    }
    
    static GetMousePos() {
        CoordMode("Mouse", "Screen")
        MouseGetPos(&x, &y)
        return {x: x, y: y}
    }
    
    static SetClipboard(text) {
        try {
            A_Clipboard := text
            return true
        } catch {
            return false
        }
    }
    
    static GetClipboard() {
        try {
            return A_Clipboard
        } catch {
            return ""
        }
    }
    
    static SendInputText(text) {
        try {
            SendInput("{Text}" text)
            return true
        } catch {
            return false
        }
    }
    
    static Click(x, y, button := "Left") {
        try {
            Click(button " " x " " y)
            return true
        } catch {
            return false
        }
    }
}

GetDpiScalingFactor() {
    return Utils.GetDpiScalingFactor()
}

CaptureScreen(x, y, w, h) {
    return Utils.CaptureScreen(x, y, w, h)
}

DeleteBitmap(hBitmap) {
    Utils.DeleteBitmap(hBitmap)
}

StrJoin(arr, delimiter := ",") {
    return Utils.StrJoin(arr, delimiter)
}

IsProcessRunning(processName) {
    return Utils.IsProcessRunning(processName)
}

EnsureDirectory(dirPath) {
    return Utils.EnsureDirectory(dirPath)
}
