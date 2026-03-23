class ResultVerifier {
    static verifyTimeout := 2000
    static verifyRetries := 3
    static verifyDelay := 500
    
    static Initialize(config := "") {
        if (config != "" && config.Has("VerifyTimeout"))
            this.verifyTimeout := config["VerifyTimeout"]
        if (config != "" && config.Has("VerifyRetries"))
            this.verifyRetries := config["VerifyRetries"]
        if (config != "" && config.Has("VerifyDelay"))
            this.verifyDelay := config["VerifyDelay"]
        
        LogInfo("ResultVerifier 初始化完成")
    }
    
    static VerifyResult(winID := 0) {
        if (winID = 0)
            winID := WindowDetector.lastWindowID
        
        if (winID = 0) {
            LogError("无效的窗口ID")
            return false
        }
        
        Sleep(this.verifyDelay)
        
        loop this.verifyRetries {
            LogDebug(Format("验证尝试 {} / {}", A_Index, this.verifyRetries))
            
            if (this.CheckPromptDisappeared(winID)) {
                LogInfo("验证成功: 提示已消失")
                return true
            }
            
            Sleep(this.verifyTimeout / this.verifyRetries)
        }
        
        LogWarn("验证失败: 提示仍然存在")
        return false
    }
    
    static CheckPromptDisappeared(winID) {
        try {
            if !WindowDetector.ActivateWindow(winID) {
                LogDebug("无法激活窗口")
                return false
            }
            
            Sleep(100)
            
            ocrResults := OCRDetector.PerformOCR(winID)
            
            if (ocrResults.Length = 0) {
                LogDebug("OCR 未识别到文本, 假定提示已消失")
                return true
            }
            
            text := OCRDetector.ExtractText(ocrResults)
            
            if !KeywordDetector.HasKeyword(text) {
                LogDebug("未检测到关键词, 提示已消失")
                return true
            }
            
            LogDebug("仍检测到关键词, 提示未消失")
            return false
        } catch as e {
            LogError("验证过程出错: " e.Message)
            return false
        }
    }
    
    static VerifyWithOCR(winID, expectedText := "") {
        try {
            ocrResults := OCRDetector.PerformOCR(winID)
            
            if (ocrResults.Length = 0) {
                return false
            }
            
            text := OCRDetector.ExtractText(ocrResults)
            
            if (expectedText = "") {
                return true
            }
            
            return (InStr(text, expectedText) > 0)
        } catch {
            return false
        }
    }
    
    static VerifyClickPosition(x, y, tolerance := 10) {
        MouseGetPos(&currentX, &currentY)
        
        diffX := Abs(currentX - x)
        diffY := Abs(currentY - y)
        
        return (diffX <= tolerance && diffY <= tolerance)
    }
    
    static SetVerifyTimeout(timeout) {
        this.verifyTimeout := timeout
    }
    
    static SetVerifyRetries(retries) {
        this.verifyRetries := retries
    }
}
