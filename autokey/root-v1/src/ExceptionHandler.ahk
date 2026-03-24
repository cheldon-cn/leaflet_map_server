; ============================================
; ExceptionHandler.ahk - 异常处理模块 (V1版本)
; 分类处理不同类型异常，提供恢复策略
; ============================================

; 异常类型常量
global EXCEPTION_OCR := "OCR_ERROR"
global EXCEPTION_WINDOW := "WINDOW_ERROR"
global EXCEPTION_EXECUTION := "EXECUTION_ERROR"
global EXCEPTION_CONFIG := "CONFIG_ERROR"
global EXCEPTION_UNKNOWN := "UNKNOWN_ERROR"

; 异常计数
global g_ExceptionCounts := {}

; 上次异常信息
global g_LastException := ""

ExceptionHandler_Init() {
    global g_ExceptionCounts
    
    g_ExceptionCounts := {}
    g_ExceptionCounts[EXCEPTION_OCR] := 0
    g_ExceptionCounts[EXCEPTION_WINDOW] := 0
    g_ExceptionCounts[EXCEPTION_EXECUTION] := 0
    g_ExceptionCounts[EXCEPTION_CONFIG] := 0
    g_ExceptionCounts[EXCEPTION_UNKNOWN] := 0
    
    LogInfo("异常处理模块初始化完成")
}

ExceptionHandler_Handle(exceptionType, message, context := "") {
    global g_ExceptionCounts, g_LastException
    
    g_ExceptionCounts[exceptionType]++
    g_LastException := {type: exceptionType, message: message, context: context, time: A_TickCount}
    
    LogError("异常 [" . exceptionType . "]: " . message)
    
    if (exceptionType = EXCEPTION_OCR) {
        return ExceptionHandler_HandleOCRException(message, context)
    } else if (exceptionType = EXCEPTION_WINDOW) {
        return ExceptionHandler_HandleWindowException(message, context)
    } else if (exceptionType = EXCEPTION_EXECUTION) {
        return ExceptionHandler_HandleExecutionException(message, context)
    } else if (exceptionType = EXCEPTION_CONFIG) {
        return ExceptionHandler_HandleConfigException(message, context)
    } else {
        return ExceptionHandler_HandleUnknownException(message, context)
    }
}

ExceptionHandler_HandleOCRException(message, context) {
    LogWarn("处理 OCR 异常: " . message)
    
    if InStr(message, "初始化") {
        LogInfo("尝试重新初始化 OCR 引擎")
        
        if OCRDetector_Reinitialize() {
            LogInfo("OCR 引擎重新初始化成功")
            return {recovered: true, action: "reinit"}
        }
        
        return {recovered: false, action: "fail"}
    }
    
    if InStr(message, "识别") || InStr(message, "超时") {
        LogInfo("OCR 识别失败，建议使用像素检测备选方案")
        return {recovered: false, action: "fallback_pixel"}
    }
    
    return {recovered: false, action: "retry"}
}

ExceptionHandler_HandleWindowException(message, context) {
    LogWarn("处理窗口异常: " . message)
    
    if InStr(message, "未检测到") || InStr(message, "不存在") {
        LogInfo("窗口未检测到，等待窗口出现")
        return {recovered: false, action: "wait_window"}
    }
    
    if InStr(message, "最小化") {
        LogInfo("窗口已最小化，尝试恢复")
        WindowDetector_RestoreWindow()
        return {recovered: true, action: "restore"}
    }
    
    if InStr(message, "无效") {
        LogInfo("窗口句柄无效，重新检测")
        WindowDetector_DetectTraeWindow()
        return {recovered: true, action: "redetect"}
    }
    
    return {recovered: false, action: "retry"}
}

ExceptionHandler_HandleExecutionException(message, context) {
    LogWarn("处理执行异常: " . message)
    
    if InStr(message, "点击") {
        LogInfo("点击失败，可能需要调整坐标")
        return {recovered: false, action: "adjust_position"}
    }
    
    if InStr(message, "输入") {
        LogInfo("输入失败，检查剪贴板")
        return {recovered: false, action: "check_clipboard"}
    }
    
    return {recovered: false, action: "retry"}
}

ExceptionHandler_HandleConfigException(message, context) {
    LogWarn("处理配置异常: " . message)
    
    if InStr(message, "不存在") {
        LogInfo("配置文件不存在，使用默认配置")
        ConfigManager_LoadDefaults()
        return {recovered: true, action: "use_defaults"}
    }
    
    if InStr(message, "格式") || InStr(message, "无效") {
        LogInfo("配置格式错误，重新加载默认配置")
        ConfigManager_LoadDefaults()
        return {recovered: true, action: "use_defaults"}
    }
    
    return {recovered: false, action: "fail"}
}

ExceptionHandler_HandleUnknownException(message, context) {
    LogError("处理未知异常: " . message)
    return {recovered: false, action: "fail"}
}

ExceptionHandler_GetExceptionCount(exceptionType := "") {
    global g_ExceptionCounts
    
    if (exceptionType = "") {
        total := 0
        for type, count in g_ExceptionCounts {
            total += count
        }
        return total
    }
    
    return g_ExceptionCounts[exceptionType]
}

ExceptionHandler_GetLastException() {
    global g_LastException
    return g_LastException
}

ExceptionHandler_ResetCounts() {
    global g_ExceptionCounts
    
    for type, count in g_ExceptionCounts {
        g_ExceptionCounts[type] := 0
    }
    
    LogInfo("异常计数器已重置")
}

ExceptionHandler_IsRecoverable(exceptionType) {
    if (exceptionType = EXCEPTION_OCR) {
        return true
    } else if (exceptionType = EXCEPTION_WINDOW) {
        return true
    } else if (exceptionType = EXCEPTION_EXECUTION) {
        return true
    } else if (exceptionType = EXCEPTION_CONFIG) {
        return true
    } else {
        return false
    }
}

ExceptionHandler_GetRecoveryAction(exceptionType) {
    if (exceptionType = EXCEPTION_OCR) {
        return "reinit_or_fallback"
    } else if (exceptionType = EXCEPTION_WINDOW) {
        return "wait_or_redetect"
    } else if (exceptionType = EXCEPTION_EXECUTION) {
        return "retry"
    } else if (exceptionType = EXCEPTION_CONFIG) {
        return "use_defaults"
    } else {
        return "fail"
    }
}

ExceptionHandler_LogSummary() {
    global g_ExceptionCounts
    
    LogInfo("异常统计:")
    
    for type, count in g_ExceptionCounts {
        LogInfo("  " . type . ": " . count)
    }
}

ExceptionHandler_Wrap(func, args*) {
    try {
        return %func%(args*)
    } catch {
        ExceptionHandler_Handle(EXCEPTION_UNKNOWN, "函数调用异常: " . func, "")
        return ""
    }
}
