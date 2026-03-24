; ============================================
; OCRDetector.ahk - OCR 识别模块 (V1版本)
; 使用 RapidOCR-AutoHotkey 库进行文字识别
; ============================================

#Include %A_ScriptDir%\..\lib\RapidOCR\RapidOCR.ahk

; OCR 引擎实例
global g_OCREngine := ""

; OCR 缓存
global g_OCRCache := {}

; 缓存启用状态
global g_OCRCacheEnabled := true

; 缓存有效期 (毫秒)
global g_OCRCacheExpiry := 60000

OCRDetector_Init() {
    global g_OCREngine, g_OCRCacheEnabled, g_OCRCacheExpiry, g_Config
    
    g_OCRCacheEnabled := g_Config.OCR.EnableOCRCache
    g_OCRCacheExpiry := g_Config.OCR.OCRCacheExpiry
    
    try {
        g_OCREngine := new RapidOCR
        LogInfo("OCR 引擎初始化成功 (RapidOCR-AutoHotkey V1)")
        return true
    } catch {
        LogError("OCR 初始化失败")
        return false
    }
}

OCRDetector_PerformOCR(image, ByRef results) {
    global g_OCREngine, g_Config
    
    if !IsObject(g_OCREngine) {
        LogError("OCR 引擎未初始化")
        return false
    }
    
    try {
        results := g_OCREngine.ocr(image)
        if IsObject(results) {
            count := 0
            for k, v in results {
                count++
            }
            LogDebug("OCR 返回 " . count . " 个结果项")
        } else {
            LogDebug("OCR 返回非对象结果: " . results)
        }
        return true
    } catch e {
        LogError("OCR 识别异常: " . (IsObject(e) ? e.message : e))
        return false
    }
}

OCRDetector_PerformOCRRegion(regionNum, ByRef results) {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        LogError("窗口未检测到")
        return false
    }
    
    region := WindowDetector_CalculateRegionPosition(regionNum)
    LogDebug("区域 " . regionNum . " 坐标: x=" . region.x . ", y=" . region.y . ", w=" . region.w . ", h=" . region.h)
    
    bitmap := ""
    if !Utils_CaptureScreen(region.x, region.y, region.w, region.h, bitmap) {
        LogError("区域 " . regionNum . " 截图失败")
        return false
    }
    
    ocrResult := ""
    success := OCRDetector_PerformOCR(bitmap, ocrResult)
    
    Utils_DeleteBitmap(bitmap)
    
    if success {
        results := ocrResult
    }
    
    return success
}

OCRDetector_PerformOCRFullWindow(ByRef results) {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        LogError("窗口未检测到")
        return false
    }
    
    bitmap := ""
    if !Utils_CaptureWindow(g_TraeWindowHwnd, bitmap) {
        LogError("窗口截图失败")
        return false
    }
    
    ocrResult := ""
    success := OCRDetector_PerformOCR(bitmap, ocrResult)
    
    Utils_DeleteBitmap(bitmap)
    
    if success {
        results := ocrResult
    }
    
    return success
}

OCRDetector_PerformOCRClientArea(ByRef results) {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        LogError("窗口未检测到")
        return false
    }
    
    clientRect := WindowDetector_GetClientRect()
    LogDebug("客户区截图范围: x=" . clientRect.x . ", y=" . clientRect.y . ", w=" . clientRect.w . ", h=" . clientRect.h)
    
    bitmap := ""
    if !Utils_CaptureWindowClient(g_TraeWindowHwnd, bitmap) {
        LogError("客户区截图失败")
        return false
    }
    
    LogDebug("客户区截图成功，开始 OCR 识别...")
    
    ocrResult := ""
    success := OCRDetector_PerformOCR(bitmap, ocrResult)
    
    Utils_DeleteBitmap(bitmap)
    
    if success {
        results := ocrResult
    }
    
    return success
}

OCRDetector_ExtractText(ocrResults) {
    if !IsObject(ocrResults) {
        if (StrLen(ocrResults) > 0) {
            LogDebug("OCR 返回字符串结果 (长度 " . StrLen(ocrResults) . "): " . SubStr(ocrResults, 1, 200))
            return ocrResults
        }
        LogDebug("OCR 结果为空")
        return ""
    }
    
    text := ""
    itemCount := 0
    
    for index, item in ocrResults {
        itemCount++
        if IsObject(item) {
            if (item.text != "") {
                text .= item.text . " "
                LogDebug("OCR 项目 " . index . ": [" . item.text . "] 置信度: " . (item.confidence ? item.confidence : "N/A"))
            }
        } else if (StrLen(item) > 0) {
            text .= item . " "
            LogDebug("OCR 项目 " . index . ": [" . item . "]")
        }
    }
    
    LogDebug("OCR 共识别 " . itemCount . " 个文本块")
    
    return Trim(text)
}

OCRDetector_GetAllTextWithConfidence(ocrResults, minConfidence := 0) {
    if !IsObject(ocrResults) {
        return []
    }
    
    results := []
    
    for index, item in ocrResults {
        if IsObject(item) {
            confidence := item.confidence ? item.confidence : 1.0
            
            if (confidence >= minConfidence) {
                results.Push({text: item.text, confidence: confidence})
            }
        }
    }
    
    return results
}

OCRDetector_GetCachedResult(cacheKey) {
    global g_OCRCache, g_OCRCacheEnabled
    
    if !g_OCRCacheEnabled {
        return ""
    }
    
    if !IsObject(g_OCRCache[cacheKey]) {
        return ""
    }
    
    cached := g_OCRCache[cacheKey]
    
    if (A_TickCount - cached.timestamp > g_OCRCacheExpiry) {
        g_OCRCache.Delete(cacheKey)
        return ""
    }
    
    return cached.result
}

OCRDetector_SetCachedResult(cacheKey, result) {
    global g_OCRCache, g_OCRCacheEnabled
    
    if !g_OCRCacheEnabled {
        return
    }
    
    g_OCRCache[cacheKey] := {result: result, timestamp: A_TickCount}
}

OCRDetector_ClearCache() {
    global g_OCRCache
    g_OCRCache := {}
}

OCRDetector_PerformOCRWithCache(image, ByRef results, cacheKey := "") {
    if (cacheKey != "") {
        cached := OCRDetector_GetCachedResult(cacheKey)
        if (cached != "") {
            results := cached
            return true
        }
    }
    
    success := OCRDetector_PerformOCR(image, results)
    
    if success && (cacheKey != "") {
        OCRDetector_SetCachedResult(cacheKey, results)
    }
    
    return success
}

OCRDetector_IsInitialized() {
    global g_OCREngine
    return IsObject(g_OCREngine)
}

OCRDetector_Reinitialize() {
    global g_OCREngine
    
    g_OCREngine := ""
    
    return OCRDetector_Init()
}

; ============================================
; 性能优化函数
; ============================================

OCRDetector_PerformSmartDetection(ByRef detectedKeywords) {
    global g_Config
    
    priorityRegions := g_Config.DetectionRegion.PriorityRegions
    
    for index, regionNum in priorityRegions {
        regionNum := regionNum + 0
        LogDebug("正在检测区域 " . regionNum . "...")
        
        results := ""
        if OCRDetector_PerformOCRRegion(regionNum, results) {
            text := OCRDetector_ExtractText(results)
            
            if (StrLen(text) > 0) {
                LogDebug("区域 " . regionNum . " 识别到文本: " . SubStr(text, 1, 100))
            }
            
            detection := KeywordDetector_DetectKeywords(text)
            if detection.found {
                detectedKeywords := detection
                LogInfo("在区域 " . regionNum . " 检测到关键词: " . detection.keyword)
                return true
            }
        }
    }
    
    if g_Config.DetectionRegion.DetectFullWindowIfNotFound {
        LogDebug("优先区域未检测到关键词，开始全窗口检测...")
        results := ""
        if OCRDetector_PerformOCRClientArea(results) {
            text := OCRDetector_ExtractText(results)
            
            detection := KeywordDetector_DetectKeywords(text)
            if detection.found {
                detectedKeywords := detection
                LogInfo("在全窗口检测到关键词: " . detection.keyword)
                return true
            }
        }
    }
    
    return false
}

OCRDetector_PerformQuickCheck() {
    global g_TraeWindowHwnd
    
    if !g_TraeWindowHwnd {
        return false
    }
    
    clientRect := WindowDetector_GetClientRect()
    checkRegion := {x: clientRect.x, y: clientRect.y + clientRect.h - 200, w: clientRect.w, h: 200}
    
    bitmap := ""
    if !Utils_CaptureScreen(checkRegion.x, checkRegion.y, checkRegion.w, checkRegion.h, bitmap) {
        return false
    }
    
    results := ""
    success := OCRDetector_PerformOCR(bitmap, results)
    
    Utils_DeleteBitmap(bitmap)
    
    if success {
        text := OCRDetector_ExtractText(results)
        detection := KeywordDetector_DetectKeywords(text)
        return detection.found
    }
    
    return false
}

OCRDetector_GetPerformanceStats() {
    global g_OCRCache
    
    cacheSize := 0
    for key, value in g_OCRCache {
        cacheSize++
    }
    
    result := {}
    result.cacheSize := cacheSize
    result.cacheEnabled := g_OCRCacheEnabled
    result.cacheExpiry := g_OCRCacheExpiry
    return result
}

OCRDetector_OptimizeCache() {
    global g_OCRCache, g_OCRCacheExpiry
    
    expiredCount := 0
    keysToDelete := []
    
    for key, value in g_OCRCache {
        if (A_TickCount - value.timestamp > g_OCRCacheExpiry) {
            keysToDelete.Push(key)
        }
    }
    
    for index, key in keysToDelete {
        g_OCRCache.Delete(key)
        expiredCount++
    }
    
    if (expiredCount > 0) {
        LogDebug("清理了 " . expiredCount . " 个过期缓存项")
    }
    
    return expiredCount
}
