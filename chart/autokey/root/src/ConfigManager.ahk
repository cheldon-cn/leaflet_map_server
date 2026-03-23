class ConfigManager {
    static config := Map()
    static configFile := "config\config.ini"
    
    static Load(configFile := "") {
        if (configFile != "")
            this.configFile := configFile
        
        this.LoadDefaults()
        
        if !FileExist(this.configFile) {
            this.LogWarn("配置文件不存在: " this.configFile ", 使用默认配置")
            return
        }
        
        try {
            this.config["General"]["CooldownPeriod"] := Integer(this.ReadIni("General", "CooldownPeriod", "900000"))
            this.config["General"]["CheckInterval"] := Integer(this.ReadIni("General", "CheckInterval", "10000"))
            this.config["General"]["PostExecutionDelay"] := Integer(this.ReadIni("General", "PostExecutionDelay", "30000"))
            this.config["General"]["EnableLog"] := this.ReadIni("General", "EnableLog", "true")
            this.config["General"]["LogFile"] := this.ReadIni("General", "LogFile", "log\trae_auto.log")
            this.config["General"]["MaxLogSize"] := Integer(this.ReadIni("General", "MaxLogSize", "10485760"))
            
            this.config["Detection"]["TargetWindow"] := this.ReadIni("Detection", "TargetWindow", "ahk_exe Trae CN.exe")
            this.config["Detection"]["Keywords"] := this.ReadIni("Detection", "Keywords", "模型思考次数已达上限,请输入继续,获得更多结果")
            this.config["Detection"]["OCRRegion"] := Integer(this.ReadIni("Detection", "OCRRegion", "9"))
            this.config["Detection"]["UseOCR"] := this.ReadIni("Detection", "UseOCR", "true")
            
            this.config["OCR"]["Engine"] := this.ReadIni("OCR", "Engine", "RapidOCR")
            this.config["OCR"]["RapidOCRPath"] := this.ReadIni("OCR", "RapidOCRPath", "lib\RapidOCR\RapidOCR.ahk")
            this.config["OCR"]["ConfidenceThreshold"] := Float(this.ReadIni("OCR", "ConfidenceThreshold", "0.9"))
            
            this.config["Execution"]["InputDelay"] := Integer(this.ReadIni("Execution", "InputDelay", "100"))
            this.config["Execution"]["ClickDelay"] := Integer(this.ReadIni("Execution", "ClickDelay", "200"))
            this.config["Execution"]["MaxRetries"] := Integer(this.ReadIni("Execution", "MaxRetries", "3"))
            this.config["Execution"]["RetryDelay"] := Integer(this.ReadIni("Execution", "RetryDelay", "1000"))
            this.config["Execution"]["VerifyResult"] := this.ReadIni("Execution", "VerifyResult", "true")
            this.config["Execution"]["ConsecutiveFailuresThreshold"] := Integer(this.ReadIni("Execution", "ConsecutiveFailuresThreshold", "5"))
            this.config["Execution"]["EnableRandomDelay"] := this.ReadIni("Execution", "EnableRandomDelay", "true")
            this.config["Execution"]["RandomDelayMin"] := Integer(this.ReadIni("Execution", "RandomDelayMin", "10"))
            this.config["Execution"]["RandomDelayMax"] := Integer(this.ReadIni("Execution", "RandomDelayMax", "30"))
            this.config["Execution"]["SendText"] := this.ReadIni("Execution", "SendText", "继续")
            
            this.config["UI"]["ShowTrayIcon"] := this.ReadIni("UI", "ShowTrayIcon", "true")
            this.config["UI"]["ShowNotifications"] := this.ReadIni("UI", "ShowNotifications", "true")
            
            this.config["Position"]["InputX"] := Float(this.ReadIni("Position", "InputX", "0.5"))
            this.config["Position"]["InputY"] := Float(this.ReadIni("Position", "InputY", "0.92"))
            this.config["Position"]["SendBtnX"] := Float(this.ReadIni("Position", "SendBtnX", "0.85"))
            this.config["Position"]["SendBtnY"] := Float(this.ReadIni("Position", "SendBtnY", "0.92"))
            
            OutputDebug(Format("ConfigManager: Position loaded - InputX={}, InputY={}, SendBtnX={}, SendBtnY={}", 
                this.config["Position"]["InputX"], this.config["Position"]["InputY"], 
                this.config["Position"]["SendBtnX"], this.config["Position"]["SendBtnY"]))
            
        } catch as e {
            this.LogError("配置加载失败: " e.Message ", 使用默认配置")
            this.LoadDefaults()
        }
    }
    
    static ReadIni(section, key, default := "") {
        try {
            file := FileOpen(this.configFile, "r", "UTF-8")
            if (!file)
                return default
            
            content := file.Read()
            file.Close()
            
            pattern := "s)\[" . section . "\](.*?)(?=\[|$)"
            if !RegExMatch(content, pattern, &sectionMatch)
                return default
            
            sectionContent := sectionMatch[1]
            
            keyPattern := "m)^" . key . "\s*=\s*(.*)$"
            if RegExMatch(sectionContent, keyPattern, &keyMatch)
                return Trim(keyMatch[1])
            
            return default
        } catch {
            return default
        }
    }
    
    static LoadDefaults() {
        this.config["General"] := Map(
            "CooldownPeriod", 900000,
            "CheckInterval", 10000,
            "PostExecutionDelay", 30000,
            "EnableLog", "true",
            "LogFile", "log\trae_auto.log",
            "MaxLogSize", 10485760
        )
        this.config["Detection"] := Map(
            "TargetWindow", "ahk_exe Trae CN.exe",
            "Keywords", "模型思考次数已达上限,请输入继续,获得更多结果",
            "OCRRegion", 9,
            "UseOCR", "true"
        )
        this.config["OCR"] := Map(
            "Engine", "RapidOCR",
            "RapidOCRPath", "lib\RapidOCR\RapidOCR.ahk",
            "ConfidenceThreshold", 0.9
        )
        this.config["Execution"] := Map(
            "InputDelay", 100,
            "ClickDelay", 200,
            "MaxRetries", 3,
            "RetryDelay", 1000,
            "VerifyResult", "true",
            "ConsecutiveFailuresThreshold", 5,
            "EnableRandomDelay", "true",
            "RandomDelayMin", 10,
            "RandomDelayMax", 30,
            "SendText", "继续"
        )
        this.config["UI"] := Map(
            "ShowTrayIcon", "true",
            "ShowNotifications", "true"
        )
        this.config["Position"] := Map(
            "InputX", 0.5,
            "InputY", 0.92,
            "SendBtnX", 0.85,
            "SendBtnY", 0.92
        )
    }
    
    static Get(section, key, default := "") {
        try {
            return this.config[section][key]
        } catch {
            return default
        }
    }
    
    static Set(section, key, value) {
        if !this.config.Has(section)
            this.config[section] := Map()
        
        this.config[section][key] := value
    }
    
    static Save(configFile := "") {
        if (configFile != "")
            this.configFile := configFile
        
        try {
            for section, keys in this.config {
                for key, value in keys {
                    IniWrite(String(value), this.configFile, section, key)
                }
            }
        } catch as e {
            this.LogError("配置保存失败: " e.Message)
        }
    }
    
    static Reload() {
        this.Load(this.configFile)
    }
    
    static GetKeywords() {
        keywordsStr := this.Get("Detection", "Keywords", "模型思考次数已达上限,请输入继续,获得更多结果")
        return StrSplit(keywordsStr, ",")
    }
    
    static LogWarn(msg) {
        OutputDebug("ConfigManager WARN: " msg)
    }
    
    static LogError(msg) {
        OutputDebug("ConfigManager ERROR: " msg)
    }
}
