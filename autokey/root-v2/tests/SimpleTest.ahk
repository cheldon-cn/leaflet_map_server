#Requires AutoHotkey v2.0
#SingleInstance Force

SetWorkingDir(A_ScriptDir "\..")

logFile := A_WorkingDir "\log\simple_test.log"

if !DirExist(A_WorkingDir "\log")
    DirCreate(A_WorkingDir "\log")

try {
    FileAppend("测试开始`n", logFile)
    FileAppend("工作目录: " A_WorkingDir "`n", logFile)
    
    #Include ..\src\ConfigManager.ahk
    
    FileAppend("ConfigManager 加载成功`n", logFile)
    
    ConfigManager.LoadDefaults()
    FileAppend("LoadDefaults 完成`n", logFile)
    
    val := ConfigManager.Get("General", "CooldownPeriod", 0)
    FileAppend(Format("CooldownPeriod: {}`n", val), logFile)
    
    FileAppend("测试完成`n", logFile)
    MsgBox("测试成功! 请查看日志: " logFile)
} catch as e {
    FileAppend("错误: " e.Message "`n", logFile)
    MsgBox("错误: " e.Message)
}
