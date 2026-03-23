#Requires AutoHotkey v2.0
#SingleInstance Force

SetWorkingDir(A_ScriptDir "\..")

logFile := A_WorkingDir "\log\step_test.log"

if !DirExist(A_WorkingDir "\log")
    DirCreate(A_WorkingDir "\log")

Log(msg) {
    global logFile
    FileAppend(msg "`n", logFile)
    OutputDebug(msg)
}

try {
    Log("=== 逐步测试开始 ===")
    Log("工作目录: " A_WorkingDir)
    
    Log("步骤1: 加载 ConfigManager")
    #Include ..\src\ConfigManager.ahk
    Log("ConfigManager 加载成功")
    
    Log("步骤2: 加载 Logger")
    #Include ..\src\Logger.ahk
    Log("Logger 加载成功")
    
    Log("步骤3: 加载 Utils")
    #Include ..\src\Utils.ahk
    Log("Utils 加载成功")
    
    Log("步骤4: 加载 KeywordDetector")
    #Include ..\src\KeywordDetector.ahk
    Log("KeywordDetector 加载成功")
    
    Log("步骤5: 加载 GridRegion")
    #Include ..\src\GridRegion.ahk
    Log("GridRegion 加载成功")
    
    Log("步骤6: 加载 StateMachine")
    #Include ..\src\StateMachine.ahk
    Log("StateMachine 加载成功")
    
    Log("步骤7: 加载 RetryManager")
    #Include ..\src\RetryManager.ahk
    Log("RetryManager 加载成功")
    
    Log("=== 所有模块加载成功 ===")
    
    Log("")
    Log("=== 开始测试 ===")
    
    ConfigManager.LoadDefaults()
    Log("ConfigManager.LoadDefaults() 完成")
    
    KeywordDetector.Initialize(Map("Keywords", "测试关键词,另一个关键词"))
    Log("KeywordDetector.Initialize() 完成")
    
    hasKey := KeywordDetector.HasKeyword("这是一个测试关键词的文本")
    Log("HasKeyword 测试: " hasKey)
    
    Log("=== 测试完成 ===")
    MsgBox("所有测试通过!`n`n日志: " logFile)
    
} catch as e {
    Log("错误: " e.Message)
    Log("文件: " e.File)
    Log("行号: " e.Line)
    MsgBox("错误: " e.Message "`n`n文件: " e.File "`n行号: " e.Line)
}
