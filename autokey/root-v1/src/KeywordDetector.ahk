; ============================================
; KeywordDetector.ahk - 关键词检测模块 (V1版本)
; 负责加载关键词配置并检测文本中的关键词
; ============================================

; 关键词列表
global g_Keywords := []

; 输入文本
global g_InputText := "继续"

KeywordDetector_Init() {
    global g_Keywords, g_InputText, g_Config
    
    g_Keywords := g_Config.Keywords.PromptKeywords
    g_InputText := g_Config.Keywords.InputText
    
    keywordCount := 0
    if IsObject(g_Keywords) {
        keywordCount := g_Keywords.MaxIndex()
    }
    LogInfo("关键词检测模块初始化完成, 关键词数量: " . keywordCount)
    
    if IsObject(g_Keywords) {
        for index, kw in g_Keywords {
            LogDebug("关键词 " . index . ": [" . kw . "] 长度: " . StrLen(kw))
        }
    }
    
    LogDebug("输入文本: [" . g_InputText . "]")
}

KeywordDetector_DetectKeywords(text) {
    global g_Keywords
    
    if (StrLen(text) = 0) {
        result := {}
        result.found := false
        result.keyword := ""
        return result
    }
    
    for index, keyword in g_Keywords {
        if (StrLen(keyword) > 0 && InStr(text, keyword) > 0) {
            result := {}
            result.found := true
            result.keyword := keyword
            LogDebug("关键词匹配成功: [" . keyword . "] 在文本中找到")
            return result
        }
    }
    
    LogDebug("未找到任何关键词 (检测了 " . g_Keywords.MaxIndex() . " 个关键词)")
    
    result := {}
    result.found := false
    result.keyword := ""
    return result
}

KeywordDetector_DetectAllKeywords(text) {
    global g_Keywords
    
    found := []
    
    if (StrLen(text) = 0) {
        return found
    }
    
    for index, keyword in g_Keywords {
        if (StrLen(keyword) > 0 && InStr(text, keyword) > 0) {
            found.Push(keyword)
        }
    }
    
    return found
}

KeywordDetector_GetInputText() {
    global g_InputText
    return g_InputText
}

KeywordDetector_SetInputText(text) {
    global g_InputText
    g_InputText := text
}

KeywordDetector_AddKeyword(keyword) {
    global g_Keywords
    
    if (StrLen(keyword) = 0) {
        return false
    }
    
    for index, existing in g_Keywords {
        if (existing = keyword) {
            return false
        }
    }
    
    g_Keywords.Push(keyword)
    return true
}

KeywordDetector_RemoveKeyword(keyword) {
    global g_Keywords
    
    for index, existing in g_Keywords {
        if (existing = keyword) {
            g_Keywords.RemoveAt(index)
            return true
        }
    }
    
    return false
}

KeywordDetector_GetKeywords() {
    global g_Keywords
    return g_Keywords
}

KeywordDetector_SetKeywords(keywords) {
    global g_Keywords
    g_Keywords := keywords
}

KeywordDetector_ReloadFromConfig() {
    global g_Config
    
    g_Keywords := g_Config.Keywords.PromptKeywords
    g_InputText := g_Config.Keywords.InputText
    
    LogInfo("关键词配置已重新加载")
}

KeywordDetector_MatchPattern(text, pattern) {
    if (StrLen(text) = 0 || StrLen(pattern) = 0) {
        return false
    }
    
    return RegExMatch(text, pattern) > 0
}

KeywordDetector_CountOccurrences(text, keyword) {
    if (StrLen(text) = 0 || StrLen(keyword) = 0) {
        return 0
    }
    
    count := 0
    pos := 1
    
    Loop {
        foundPos := InStr(text, keyword, false, pos)
        if (foundPos = 0) {
            break
        }
        count++
        pos := foundPos + StrLen(keyword)
    }
    
    return count
}
