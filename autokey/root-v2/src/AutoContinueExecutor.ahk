class AutoContinueExecutor {
    static inputDelay := 100
    static clickDelay := 200
    static inputX := 0.5
    static inputY := 0.92
    static sendBtnX := 0.85
    static sendBtnY := 0.92
    static enableRandomDelay := true
    static randomDelayMin := 10
    static randomDelayMax := 30
    static sendText := "继续"
    
    static Initialize(config := "") {
        if (config != "" && config.Has("InputDelay"))
            this.inputDelay := config["InputDelay"]
        if (config != "" && config.Has("ClickDelay"))
            this.clickDelay := config["ClickDelay"]
        if (config != "" && config.Has("InputX"))
            this.inputX := config["InputX"]
        if (config != "" && config.Has("InputY"))
            this.inputY := config["InputY"]
        if (config != "" && config.Has("SendBtnX"))
            this.sendBtnX := config["SendBtnX"]
        if (config != "" && config.Has("SendBtnY"))
            this.sendBtnY := config["SendBtnY"]
        if (config != "" && config.Has("EnableRandomDelay"))
            this.enableRandomDelay := (config["EnableRandomDelay"] = "true")
        if (config != "" && config.Has("RandomDelayMin"))
            this.randomDelayMin := config["RandomDelayMin"]
        if (config != "" && config.Has("RandomDelayMax"))
            this.randomDelayMax := config["RandomDelayMax"]
        if (config != "" && config.Has("SendText"))
            this.sendText := config["SendText"]
        
        LogInfo(Format("AutoContinueExecutor 初始化完成: InputX={}, InputY={}, SendBtnX={}, SendBtnY={}, EnableRandomDelay={}, RandomDelayMin={}, RandomDelayMax={}, SendText={}", 
            this.inputX, this.inputY, this.sendBtnX, this.sendBtnY, this.enableRandomDelay, this.randomDelayMin, this.randomDelayMax, this.sendText))
    }
    
    static Execute(winID := 0) {
        if (winID = 0)
            winID := WindowDetector.lastWindowID
        
        if (winID = 0) {
            LogError("无效的窗口ID")
            return false
        }
        
        try {
            if (this.enableRandomDelay) {
                randomSeconds := Random(this.randomDelayMin, this.randomDelayMax)
                LogInfo(Format("随机延迟 {} 秒后执行...", randomSeconds))
                Sleep(randomSeconds * 1000)
            }
            
            if !WindowDetector.ActivateWindow(winID) {
                LogError("无法激活窗口")
                return false
            }
            
            Sleep(200)
            
            WinGetPos(&wx, &wy, &ww, &wh, "ahk_id " winID)
            WinGetClientPos(, , &cw, &ch, "ahk_id " winID)
            
            if (cw = 0 || ch = 0) {
                LogError("客户区尺寸无效")
                return false
            }
            
            borderWidth := (ww - cw) / 2
            titleHeight := wh - ch - borderWidth
            
            clientScreenX := wx + borderWidth
            clientScreenY := wy + titleHeight
            
            inputPosX := Round(clientScreenX + cw * this.inputX)
            inputPosY := Round(clientScreenY + ch * this.inputY)
            sendPosX := Round(clientScreenX + cw * this.sendBtnX)
            sendPosY := Round(clientScreenY + ch * this.sendBtnY)
            
            LogInfo(Format("窗口: ({}, {}) {}x{}, 客户区尺寸: {}x{}, 边框: {}, 标题栏: {}", wx, wy, ww, wh, cw, ch, borderWidth, titleHeight))
            LogInfo(Format("客户区屏幕起点: ({}, {})", clientScreenX, clientScreenY))
            LogInfo(Format("点击输入框: ({}, {}), 比例: ({}, {})", inputPosX, inputPosY, this.inputX, this.inputY))
            
            if !this.ClickAt(inputPosX, inputPosY) {
                LogError("点击输入框失败")
                return false
            }
            
            Sleep(this.clickDelay)
            
            LogInfo(Format("输入文本: {}", this.sendText))
            if !this.InputText(this.sendText) {
                LogError("输入文本失败")
                return false
            }
            
            Sleep(this.inputDelay)
            
            LogInfo(Format("点击发送按钮: ({}, {}), 比例: ({}, {})", sendPosX, sendPosY, this.sendBtnX, this.sendBtnY))
            if !this.ClickAt(sendPosX, sendPosY) {
                LogError("点击发送按钮失败")
                return false
            }
            
            LogInfo("执行完成")
            return true
        } catch as e {
            LogError("执行失败: " e.Message)
            return false
        }
    }
    
    static ClickAt(x, y) {
        try {
            CoordMode("Mouse", "Screen")
            Click(x " " y)
            return true
        } catch as e {
            LogError("点击失败: " e.Message)
            return false
        }
    }
    
    static InputText(text) {
        try {
            A_Clipboard := text
            Sleep(50)
            Send("^v")
            Sleep(50)
            return true
        } catch as e {
            LogError("输入失败: " e.Message)
            try {
                SendText(text)
                return true
            } catch {
                return false
            }
        }
    }
    
    static SetInputPosition(x, y) {
        this.inputX := x
        this.inputY := y
    }
    
    static SetSendButtonPosition(x, y) {
        this.sendBtnX := x
        this.sendBtnY := y
    }
    
    static SetDelays(inputDelay, clickDelay) {
        this.inputDelay := inputDelay
        this.clickDelay := clickDelay
    }
}
