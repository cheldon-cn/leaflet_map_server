#Requires AutoHotkey v2.0
#SingleInstance Force

CoordMode("Mouse", "Screen")
CoordMode("Pixel", "Screen")

SetTimer(UpdateInfo, 100)

OnExit(ExitFunc)

UpdateInfo() {
    MouseGetPos(&mx, &my, &winID, &ctrl)
    
    if (winID = 0) {
        ToolTip("请将鼠标移到目标窗口")
        return
    }
    
    WinGetPos(&wx, &wy, &ww, &wh, "ahk_id " winID)
    WinGetClientPos(&cx, &cy, &cw, &ch, "ahk_id " winID)
    
    procName := "Unknown"
    try {
        procName := WinGetProcessName("ahk_id " winID)
    }
    
    clientX := mx - wx - (ww - cw) / 2
    clientY := my - wy - (wh - ch)
    
    ratioX := cw > 0 ? Round(clientX / cw, 4) : 0
    ratioY := ch > 0 ? Round(clientY / ch, 4) : 0
    
    ToolTip(
        "进程: " procName "`n"
        . "窗口ID: " winID "`n"
        . "─────────────────`n"
        . "窗口位置: (" wx ", " wy ")`n"
        . "窗口大小: " ww " x " wh "`n"
        . "客户区大小: " cw " x " ch "`n"
        . "─────────────────`n"
        . "鼠标屏幕坐标: (" mx ", " my ")`n"
        . "客户区坐标: (" Round(clientX) ", " Round(clientY) ")`n"
        . "─────────────────`n"
        . "比例坐标 (配置用):`n"
        . "  InputX = " ratioX "`n"
        . "  InputY = " ratioY "`n"
        . "─────────────────`n"
        . "按 Ctrl+C 复制坐标`n"
        . "按 Esc 退出"
        , mx + 20, my + 20
    )
}

^c:: {
    MouseGetPos(&mx, &my, &winID, &ctrl)
    
    if (winID = 0) {
        ToolTip("无效窗口")
        SetTimer(() => ToolTip(), 1500)
        return
    }
    
    WinGetPos(&wx, &wy, &ww, &wh, "ahk_id " winID)
    WinGetClientPos(&cx, &cy, &cw, &ch, "ahk_id " winID)
    
    clientX := mx - wx - (ww - cw) / 2
    clientY := my - wy - (wh - ch)
    
    ratioX := cw > 0 ? Round(clientX / cw, 4) : 0
    ratioY := ch > 0 ? Round(clientY / ch, 4) : 0
    
    text := "InputX=" ratioX "`nInputY=" ratioY
    A_Clipboard := text
    ToolTip("已复制:`n" text)
    SetTimer(() => ToolTip(), 2000)
}

Esc:: {
    ExitApp()
}

ExitFunc(*) {
    ToolTip()
}
