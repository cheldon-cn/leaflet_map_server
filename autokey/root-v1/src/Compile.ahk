; ============================================
; Compile.ahk - 编译打包脚本 (V1版本)
; 使用 Ahk2Exe 编译主程序
; ============================================

#SingleInstance Force
#NoEnv
SetWorkingDir %A_ScriptDir%

; AutoHotkey 安装目录
global AHK_INSTALL_DIR := "D:\Program Files\AutoHotKey"

; 编译器路径
global COMPILER_PATH := AHK_INSTALL_DIR . "\Compiler\Ahk2Exe.exe"

; 主程序路径
global MAIN_SCRIPT := A_ScriptDir . "\Main.ahk"

; 输出路径
global OUTPUT_EXE := A_ScriptDir . "\..\TraeAutoContinue.exe"

; 图标路径 (可选)
global ICON_PATH := ""

Compile_Run() {
    global COMPILER_PATH, MAIN_SCRIPT, OUTPUT_EXE, ICON_PATH
    
    LogInfo("========================================")
    LogInfo("开始编译")
    LogInfo("========================================")
    
    if !FileExist(COMPILER_PATH) {
        LogError("编译器不存在: " . COMPILER_PATH)
        MsgBox, 16, 编译错误, 编译器不存在:`n%COMPILER_PATH%
        return false
    }
    
    if !FileExist(MAIN_SCRIPT) {
        LogError("主程序不存在: " . MAIN_SCRIPT)
        MsgBox, 16, 编译错误, 主程序不存在:`n%MAIN_SCRIPT%
        return false
    }
    
    LogInfo("编译器: " . COMPILER_PATH)
    LogInfo("源文件: " . MAIN_SCRIPT)
    LogInfo("输出: " . OUTPUT_EXE)
    
    RunWait, "%COMPILER_PATH%" /in "%MAIN_SCRIPT%" /out "%OUTPUT_EXE%", %A_ScriptDir%, UseErrorLevel
    
    if ErrorLevel {
        LogError("编译失败，错误代码: " . ErrorLevel)
        MsgBox, 16, 编译错误, 编译失败，错误代码: %ErrorLevel%
        return false
    }
    
    if FileExist(OUTPUT_EXE) {
        FileGetSize, exeSize, %OUTPUT_EXE%
        exeSizeMB := Round(exeSize / 1024 / 1024, 2)
        
        LogInfo("编译成功!")
        LogInfo("输出文件: " . OUTPUT_EXE)
        LogInfo("文件大小: " . exeSizeMB . " MB")
        
        MsgBox, 64, 编译成功, 编译成功!`n`n输出文件: %OUTPUT_EXE%`n文件大小: %exeSizeMB% MB
        return true
    } else {
        LogError("编译输出文件不存在")
        MsgBox, 16, 编译错误, 编译输出文件不存在
        return false
    }
}

LogInfo(message) {
    OutputDebug, [INFO] %message%
}

LogError(message) {
    OutputDebug, [ERROR] %message%
}

Compile_Run()
