class KeywordDetector {
    static keywords := []
    
    static Initialize(config := "") {
        keywordsStr := ""
        if (config != "" && config.Has("Keywords")) {
            keywordsStr := config["Keywords"]
            this.keywords := StrSplit(keywordsStr, ",")
        } else {
            keywordsStr := "模型思考次数已达上限,请输入继续,获得更多结果"
            this.keywords := ["模型思考次数已达上限", "请输入继续", "获得更多结果"]
        }
        
        LogInfo("KeywordDetector 初始化完成, 关键词数量: " this.keywords.Length)
        LogInfo("关键词列表: " keywordsStr)
    }
    
    static DetectKeywords(text) {
        found := []
        
        for keyword in this.keywords {
            if (InStr(text, keyword) > 0) {
                found.Push(keyword)
                LogInfo("检测到关键词: " keyword)
            }
        }
        
        if (found.Length > 0)
            LogInfo("共检测到 " found.Length " 个关键词")
        
        return found
    }
    
    static HasKeyword(text) {
        for keyword in this.keywords {
            if (InStr(text, keyword) > 0) {
                LogInfo("检测到目标关键词: " keyword)
                return true
            }
        }
        return false
    }
    
    static DetectFirstKeyword(text) {
        for keyword in this.keywords {
            if (InStr(text, keyword) > 0) {
                LogInfo("检测到首个关键词: " keyword)
                return keyword
            }
        }
        return ""
    }
    
    static SetKeywords(keywords) {
        if (IsObject(keywords))
            this.keywords := keywords
        else
            this.keywords := StrSplit(keywords, ",")
    }
    
    static AddKeyword(keyword) {
        for k in this.keywords {
            if (k = keyword)
                return
        }
        this.keywords.Push(keyword)
    }
    
    static RemoveKeyword(keyword) {
        newKeywords := []
        for k in this.keywords {
            if (k != keyword)
                newKeywords.Push(k)
        }
        this.keywords := newKeywords
    }
    
    static GetKeywords() {
        return this.keywords
    }
    
    static GetKeywordsString() {
        return Utils.StrJoin(this.keywords, ",")
    }
}
