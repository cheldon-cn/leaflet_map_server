class OCRCache {
    static cache := Map()
    static maxSize := 100
    static timeout := 60000
    static hits := 0
    static misses := 0
    
    static Initialize(config := "") {
        if (config != "" && config.Has("CacheMaxSize"))
            this.maxSize := config["CacheMaxSize"]
        if (config != "" && config.Has("CacheTimeout"))
            this.timeout := config["CacheTimeout"]
        
        LogInfo("OCRCache 初始化完成, 最大缓存: " this.maxSize ", 超时: " this.timeout "ms")
    }
    
    static GetCachedResult(bitmap) {
        hash := this.CalculateHash(bitmap)
        
        if !this.cache.Has(hash) {
            this.misses++
            return ""
        }
        
        cached := this.cache[hash]
        
        if (A_TickCount - cached.timestamp > this.timeout) {
            this.cache.Delete(hash)
            this.misses++
            return ""
        }
        
        this.hits++
        LogDebug("OCR 缓存命中")
        return cached.result
    }
    
    static SetCache(bitmap, result) {
        if (this.cache.Count >= this.maxSize) {
            this.EvictOldest()
        }
        
        hash := this.CalculateHash(bitmap)
        
        this.cache[hash] := {
            result: result,
            timestamp: A_TickCount
        }
    }
    
    static CalculateHash(bitmap) {
        try {
            size := DllCall("GetObject", "Ptr", bitmap, "Int", 24, "Ptr", 0)
            if (size = 0)
                return String(bitmap)
            
            return String(bitmap) "_" String(A_TickCount & 0xFFFF)
        } catch {
            return String(bitmap) "_" String(A_TickCount & 0xFFFF)
        }
    }
    
    static EvictOldest() {
        oldestKey := ""
        oldestTime := A_TickCount
        
        for key, value in this.cache {
            if (value.timestamp < oldestTime) {
                oldestTime := value.timestamp
                oldestKey := key
            }
        }
        
        if (oldestKey != "")
            this.cache.Delete(oldestKey)
    }
    
    static Clear() {
        this.cache := Map()
        this.hits := 0
        this.misses := 0
    }
    
    static GetStats() {
        total := this.hits + this.misses
        hitRate := total > 0 ? (this.hits / total * 100) : 0
        
        return {
            hits: this.hits,
            misses: this.misses,
            hitRate: hitRate,
            size: this.cache.Count,
            maxSize: this.maxSize
        }
    }
    
    static SetMaxSize(size) {
        this.maxSize := size
    }
    
    static SetTimeout(timeout) {
        this.timeout := timeout
    }
}
