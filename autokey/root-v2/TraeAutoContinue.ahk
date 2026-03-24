#Requires AutoHotkey v2.0
#SingleInstance Force

SetWorkingDir(A_ScriptDir)

#Include src\ConfigManager.ahk
#Include src\Logger.ahk
#Include src\Utils.ahk
#Include src\WindowDetector.ahk
#Include src\OCRWrapper.ahk
#Include src\OCRDetector.ahk
#Include src\GridRegion.ahk
#Include src\KeywordDetector.ahk
#Include src\AutoContinueExecutor.ahk
#Include src\RetryManager.ahk
#Include src\ExceptionHandler.ahk
#Include src\StateMachine.ahk
#Include src\HeartbeatMonitor.ahk
#Include src\Watchdog.ahk
#Include src\ResultVerifier.ahk
#Include src\Main.ahk

Main.Initialize()
Main.Start()
