class ExceptionHandler {
    static lastError := ""
    static errorCount := 0
    
    static Handle(exception, context := "") {
        this.lastError := exception.Message
        this.errorCount++
        
        errorType := this.ClassifyError(exception)
        
        switch errorType {
            case "OCR":
                this.HandleOCRError(exception, context)
            case "Window":
                this.HandleWindowError(exception, context)
            case "Execution":
                this.HandleExecutionError(exception, context)
            default:
                this.HandleGenericError(exception, context)
        }
        
        return errorType
    }
    
    static ClassifyError(exception) {
        msg := exception.Message
        
        if (InStr(msg, "OCR") > 0 || InStr(msg, "识别") > 0)
            return "OCR"
        if (InStr(msg, "窗口") > 0 || InStr(msg, "Window") > 0)
            return "Window"
        if (InStr(msg, "执行") > 0 || InStr(msg, "点击") > 0 || InStr(msg, "输入") > 0)
            return "Execution"
        
        return "Generic"
    }
    
    static HandleOCRError(exception, context) {
        LogError("OCR 错误: " exception.Message)
        
        if (context != "" && context.Has("reinit") && context["reinit"]) {
            LogInfo("尝试重新初始化 OCR...")
            OCRWrapper.Reinitialize()
        }
    }
    
    static HandleWindowError(exception, context) {
        LogError("窗口错误: " exception.Message)
        
        if (context != "" && context.Has("retryDetect") && context["retryDetect"]) {
            Sleep(1000)
            WindowDetector.DetectTraeWindow()
        }
    }
    
    static HandleExecutionError(exception, context) {
        LogError("执行错误: " exception.Message)
    }
    
    static HandleGenericError(exception, context) {
        LogError("通用错误: " exception.Message)
    }
    
    static GetLastError() {
        return this.lastError
    }
    
    static GetErrorCount() {
        return this.errorCount
    }
    
    static ClearLastError() {
        this.lastError := ""
    }
    
    static ResetErrorCount() {
        this.errorCount := 0
    }
}
