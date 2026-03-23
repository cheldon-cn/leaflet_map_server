class Main {
    static running := false
    static timerID := 0
    static timerInterval := 1000
    
    static Initialize() {
        LogInfo("========== Trae Auto Continue 启动 ==========")
        
        ConfigManager.Load("config\config.ini")
        
        Logger.Initialize(Map(
            "LogFile", ConfigManager.Get("General", "LogFile", "log\trae_auto.log"),
            "MaxLogSize", ConfigManager.Get("General", "MaxLogSize", 10485760),
            "EnableLog", ConfigManager.Get("General", "EnableLog", "true")
        ))
        
        WindowDetector.Initialize(Map(
            "TargetWindow", ConfigManager.Get("Detection", "TargetWindow", "ahk_exe Trae.exe")
        ))
        
        OCRWrapper.Initialize(Map(
            "RapidOCRPath", ConfigManager.Get("OCR", "RapidOCRPath", "lib\RapidOCR\RapidOCR.ahk")
        ))
        
        OCRDetector.Initialize(Map(
            "ConfidenceThreshold", ConfigManager.Get("OCR", "ConfidenceThreshold", 0.9),
            "OCRRegion", ConfigManager.Get("Detection", "OCRRegion", 9)
        ))
        
        KeywordDetector.Initialize(Map(
            "Keywords", ConfigManager.Get("Detection", "Keywords", "模型思考次数已达上限,请输入继续,获得更多结果")
        ))
        
        AutoContinueExecutor.Initialize(Map(
            "InputDelay", ConfigManager.Get("Execution", "InputDelay", 100),
            "ClickDelay", ConfigManager.Get("Execution", "ClickDelay", 200),
            "InputX", ConfigManager.Get("Position", "InputX", 0.5),
            "InputY", ConfigManager.Get("Position", "InputY", 0.92),
            "SendBtnX", ConfigManager.Get("Position", "SendBtnX", 0.85),
            "SendBtnY", ConfigManager.Get("Position", "SendBtnY", 0.92),
            "EnableRandomDelay", ConfigManager.Get("Execution", "EnableRandomDelay", "true"),
            "RandomDelayMin", ConfigManager.Get("Execution", "RandomDelayMin", 10),
            "RandomDelayMax", ConfigManager.Get("Execution", "RandomDelayMax", 30),
            "SendText", ConfigManager.Get("Execution", "SendText", "继续")
        ))
        
        RetryManager.Initialize(Map(
            "MaxRetries", ConfigManager.Get("Execution", "MaxRetries", 3),
            "RetryDelay", ConfigManager.Get("Execution", "RetryDelay", 1000),
            "ConsecutiveFailuresThreshold", ConfigManager.Get("Execution", "ConsecutiveFailuresThreshold", 5)
        ))
        
        StateMachine.Initialize(Map(
            "CooldownPeriod", ConfigManager.Get("General", "CooldownPeriod", 900000),
            "CheckInterval", ConfigManager.Get("General", "CheckInterval", 10000),
            "PostExecutionDelay", ConfigManager.Get("General", "PostExecutionDelay", 30000)
        ))
        
        HeartbeatMonitor.Initialize()
        Watchdog.Initialize()
        Watchdog.SetRestartCallback(ObjBindMethod(this, "Restart"))
        
        LogInfo("初始化完成")
    }
    
    static Start() {
        if (this.running) {
            LogWarn("程序已在运行")
            return
        }
        
        this.running := true
        this.timerID := SetTimer(ObjBindMethod(this, "MainLoop"), this.timerInterval)
        
        LogInfo("主循环已启动")
        
        if (ConfigManager.Get("UI", "ShowTrayIcon", "true") = "true") {
            this.SetupTray()
        }
    }
    
    static Stop() {
        if !this.running {
            return
        }
        
        this.running := false
        if (this.timerID != 0) {
            SetTimer(ObjBindMethod(this, "MainLoop"), 0)
            this.timerID := 0
        }
        
        LogInfo("主循环已停止")
    }
    
    static Restart(*) {
        LogInfo("重启程序...")
        this.Stop()
        Sleep(1000)
        this.Initialize()
        this.Start()
    }
    
    static MainLoop() {
        try {
            HeartbeatMonitor.Beat()
            
            Watchdog.Check()
            
            if !StateMachine.Update() {
                return
            }
            
            if (StateMachine.GetCurrentState() = StateMachine.STATE_DETECTING) {
                this.DetectAndExecute()
            }
            
        } catch as e {
            ExceptionHandler.Handle(e)
        }
    }
    
    static DetectAndExecute() {
        winID := WindowDetector.DetectTraeWindow()
        if (winID = 0) {
            LogDebug("Trae 窗口未找到")
            return
        }
        
        ocrResults := OCRDetector.PerformOCR(winID)
        if (ocrResults.Length = 0) {
            LogDebug("OCR 未识别到文本")
            return
        }
        
        text := OCRDetector.ExtractText(ocrResults)
        LogDebug("OCR 识别文本: " text)
        
        if !KeywordDetector.HasKeyword(text) {
            LogDebug("未检测到关键词")
            return
        }
        
        keywords := KeywordDetector.DetectKeywords(text)
        LogInfo("检测到关键词: " Utils.StrJoin(keywords, ", "))
        
        success := RetryManager.ExecuteWithRetry(ObjBindMethod(this, "ExecuteOnce"))
        
        if (success) {
            StateMachine.OnDetected()
            this.ShowNotification("已自动输入继续", "检测到提示并已处理")
        } else {
            LogError("执行失败")
            if (RetryManager.ShouldRestart()) {
                Watchdog.TriggerRestart()
            }
        }
    }
    
    static ExecuteOnce() {
        return AutoContinueExecutor.Execute()
    }
    
    static SetupTray() {
        A_TrayMenu.Delete()
        A_TrayMenu.Add("状态", ObjBindMethod(this, "ShowStatus"))
        A_TrayMenu.Add("重启", ObjBindMethod(this, "Restart"))
        A_TrayMenu.Add()
        A_TrayMenu.Add("退出", ObjBindMethod(this, "Exit"))
        
        A_IconTip := "Trae Auto Continue"
    }
    
    static ShowStatus(*) {
        stateStats := StateMachine.GetStats()
        retryStats := RetryManager.GetStats()
        
        status := Format(
            "状态: {}`n剩余时间: {}秒`n连续失败: {}`n成功率: {:.1f}%",
            stateStats.currentState,
            Round(stateStats.remainingTime / 1000),
            retryStats.consecutiveFailures,
            retryStats.successRate
        )
        
        MsgBox(status, "Trae Auto Continue 状态", 64)
    }
    
    static ShowNotification(title, msg) {
        if (ConfigManager.Get("UI", "ShowNotifications", "true") = "true") {
            try {
                TrayTip(title, msg, 1)
            } catch {
                OutputDebug("Notification: " title " - " msg)
            }
        }
    }
    
    static Exit(*) {
        this.Stop()
        LogInfo("程序退出")
        ExitApp()
    }
}
