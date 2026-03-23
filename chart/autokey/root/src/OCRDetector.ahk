class OCRDetector {
    static confidenceThreshold := 0.9
    static ocrRegion := 9
    static cache := Map()
    static cacheEnabled := true
    static cacheTimeout := 5000
    
    static Initialize(config := "") {
        if (config != "" && config.Has("ConfidenceThreshold"))
            this.confidenceThreshold := config["ConfidenceThreshold"]
        if (config != "" && config.Has("OCRRegion"))
            this.ocrRegion := config["OCRRegion"]
        if (config != "" && config.Has("CacheEnabled"))
            this.cacheEnabled := config["CacheEnabled"]
        if (config != "" && config.Has("CacheTimeout"))
            this.cacheTimeout := config["CacheTimeout"]
        
        this.cache := Map()
        
        LogInfo("OCRDetector 初始化完成, 置信度阈值: " this.confidenceThreshold ", 区域: " this.ocrRegion)
    }
    
    static PerformOCR(winID := 0, regionCode := 0, useCache := true) {
        if (useCache && this.cacheEnabled) {
            cacheKey := this.GetCacheKey(winID, regionCode)
            if (this.cache.Has(cacheKey)) {
                cached := this.cache[cacheKey]
                if (A_TickCount - cached.time < this.cacheTimeout) {
                    LogDebug("使用 OCR 缓存")
                    return cached.results
                }
            }
        }
        
        bitmap := 0
        try {
            if (winID = 0)
                winID := WindowDetector.lastWindowID
            
            if (winID = 0) {
                LogError("无效的窗口ID")
                return []
            }
            
            winRect := WindowDetector.GetWindowRect(winID)
            if (winRect.w = 0 || winRect.h = 0) {
                LogError("窗口尺寸无效")
                return []
            }
            
            if (regionCode = 0)
                regionCode := this.ocrRegion
            
            region := this.CalculateOCRRegion(winRect, regionCode)
            
            LogDebug(Format("OCR 区域: ({}, {}) {}x{}", region.x, region.y, region.w, region.h))
            
            bitmap := Utils.CaptureScreen(region.x, region.y, region.w, region.h)
            if (bitmap = 0) {
                LogError("截图失败")
                return []
            }
            
            if !OCRWrapper.IsInitialized() {
                LogError("OCR 引擎未初始化")
                return []
            }
            
            result := OCRWrapper.OCR(bitmap)
            
            filteredResults := this.FilterResults(result)
            
            ocrText := this.ExtractText(filteredResults)
            if (ocrText != "")
                LogInfo("OCR 识别结果: " ocrText)
            
            if (useCache && this.cacheEnabled && filteredResults.Length > 0) {
                cacheKey := this.GetCacheKey(winID, regionCode)
                this.cache[cacheKey] := {results: filteredResults, time: A_TickCount}
            }
            
            return filteredResults
        } catch as e {
            LogError("OCR 识别失败: " e.Message)
            return []
        } finally {
            if (bitmap != 0)
                Utils.DeleteBitmap(bitmap)
        }
    }
    
    static GetCacheKey(winID, regionCode) {
        return Format("{}:{}", winID, regionCode)
    }
    
    static ClearCache() {
        this.cache := Map()
        LogDebug("OCR 缓存已清除")
    }
    
    static CalculateOCRRegion(winRect, regionCode := 0) {
        if (regionCode = 0) {
            return {x: winRect.x, y: winRect.y, w: winRect.w, h: winRect.h}
        }
        
        regionCode := Utils.Clamp(regionCode, 1, 9)
        
        thirdW := winRect.w / 3
        thirdH := winRect.h / 3
        
        col := Mod(regionCode - 1, 3)
        row := (regionCode - 1) // 3
        
        x := winRect.x + col * thirdW
        y := winRect.y + row * thirdH
        
        return {x: Round(x), y: Round(y), w: Round(thirdW), h: Round(thirdH)}
    }
    
    static FilterResults(results, threshold := 0) {
        if (threshold = 0)
            threshold := this.confidenceThreshold
        
        filtered := []
        
        try {
            for result in results {
                if (result.Has("score") && result["score"] >= threshold) {
                    filtered.Push(result)
                }
            }
        } catch {
            for result in results {
                filtered.Push(result)
            }
        }
        
        return filtered
    }
    
    static ExtractText(ocrResults) {
        text := ""
        
        for result in ocrResults {
            if (result.Has("text"))
                text .= result["text"] " "
        }
        
        return Trim(text)
    }
    
    static ExtractAllText(ocrResults) {
        texts := []
        
        for result in ocrResults {
            if (result.Has("text"))
                texts.Push(result["text"])
        }
        
        return texts
    }
    
    static GetResultPositions(ocrResults) {
        positions := []
        
        for result in ocrResults {
            if (result.Has("box")) {
                positions.Push({
                    text: result.Has("text") ? result["text"] : "",
                    box: result["box"]
                })
            }
        }
        
        return positions
    }
    
    static SetConfidenceThreshold(threshold) {
        this.confidenceThreshold := threshold
    }
    
    static SetOCRRegion(region) {
        this.ocrRegion := region
    }
    
    static EnableCache(enabled := true) {
        this.cacheEnabled := enabled
        if (!enabled)
            this.ClearCache()
    }
    
    static GetCacheStats() {
        return {
            enabled: this.cacheEnabled,
            count: this.cache.Count,
            timeout: this.cacheTimeout
        }
    }
}
