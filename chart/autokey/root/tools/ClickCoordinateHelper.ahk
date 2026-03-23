#Requires AutoHotkey v2.0
#SingleInstance Force

CoordMode("Mouse", "Screen")

global clickCount := 0
global inputX := 0, inputY := 0
global sendX := 0, sendY := 0
global targetWinID := 0
global cw := 0, ch := 0, cx := 0, cy := 0

ShowInstructions()

ShowInstructions() {
    ToolTip(
        "坐标获取工具`n"
        . "═══════════════════════════`n"
        . "操作步骤:`n"
        . "  1. 点击输入框位置`n"
        . "  2. 点击发送按钮位置`n"
        . "`n"
        . "按 F1 重新开始`n"
        . "按 Esc 退出"
        , A_ScreenWidth/2 - 120, 50
    )
    SetTimer(RemoveInstructions, 5000)
}

RemoveInstructions() {
    ToolTip()
    SetTimer(, 0)
}

~LButton::
{
    global clickCount, targetWinID, cw, ch, cx, cy
    global inputX, inputY, sendX, sendY
    
    Sleep(100)
    
    MouseGetPos(&mx, &my, &winID)
    
    if (winID = 0) {
        ToolTip("无效窗口，请重试")
        SetTimer(() => ToolTip(), 2000)
        return
    }
    
    if (targetWinID = 0) {
        targetWinID := winID
        WinGetClientPos(&cx, &cy, &cw, &ch, "ahk_id " winID)
    }
    
    if (winID != targetWinID) {
        ToolTip("请保持在同一窗口内点击")
        SetTimer(() => ToolTip(), 2000)
        return
    }
    
    clickCount++
    
    clientX := mx - cx
    clientY := my - cy
    
    ratioX := cw > 0 ? Round(clientX / cw, 4) : 0
    ratioY := ch > 0 ? Round(clientY / ch, 4) : 0
    
    if (clickCount = 1) {
        inputX := ratioX
        inputY := ratioY
        ToolTip(Format("输入框坐标已记录`nInputX={}, InputY={}`n`n请点击发送按钮位置", ratioX, ratioY), mx - 150, my - 80)
        SetTimer(() => ToolTip(), 3000)
    } else if (clickCount = 2) {
        sendX := ratioX
        sendY := ratioY
        Sleep(500)
        CalculateAndSave()
    }
}

CalculateAndSave() {
    global inputX, inputY, sendX, sendY
    global cw, ch, cx, cy, targetWinID
    
    result :=
        "═══════════════════════════════════════`n"
        . "坐标采集完成`n"
        . "═══════════════════════════════════════`n"
        . "客户区: (" cx ", " cy ") " cw " x " ch "`n"
        . "───────────────────────────────────────`n"
        . "输入框: InputX=" inputX ", InputY=" inputY "`n"
        . "发送按钮: SendBtnX=" sendX ", SendBtnY=" sendY "`n"
        . "───────────────────────────────────────`n"
        . "配置文件格式:`n"
        . "[Position]`n"
        . "InputX=" inputX "`n"
        . "InputY=" inputY "`n"
        . "SendBtnX=" sendX "`n"
        . "SendBtnY=" sendY "`n"
        . "═══════════════════════════════════════`n"
        . "已保存到日志文件`n"
        . "按 F1 重新采集，按 Esc 退出"
    
    ToolTip(result, A_ScreenWidth/2 - 180, 100)
    
    logPath := A_ScriptDir "\..\log\trae_auto.log"
    logDir := A_ScriptDir "\..\log"
    if !DirExist(logDir)
        DirCreate(logDir)
    
    logFile := FileOpen(logPath, "a", "UTF-8")
    if (logFile) {
        logFile.Write("`n[" A_YYYY "-" A_MM "-" A_DD " " A_Hour ":" A_Min ":" A_Sec "] 坐标采集结果`n")
        logFile.Write("输入框: InputX=" inputX ", InputY=" inputY "`n")
        logFile.Write("发送按钮: SendBtnX=" sendX ", SendBtnY=" sendY "`n")
        logFile.Close()
    }
}

F1::
{
    global clickCount, targetWinID
    clickCount := 0
    targetWinID := 0
    ToolTip()
    ShowInstructions()
}

Esc::
{
    ToolTip()
    ExitApp()
}
