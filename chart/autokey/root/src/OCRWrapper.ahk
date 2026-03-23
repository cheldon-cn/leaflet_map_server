class OCRWrapper {
    static exePath := "lib\RapidOCR\exe\RapidOCR-json.exe"
    static modelPath := "lib\RapidOCR\exe\models"
    static initialized := false
    static ocrAvailable := false
    static shell := ""
    static exec := ""
    static prevWorkingDir := ""
    static gdiplusToken := 0
    
    static model := "zh_hans"
    static numThreads := 4
    static boxScoreThresh := 0.5
    static boxThresh := 0.3
    static padding := 50
    static maxSideLen := 1024
    static unclipRatio := 1.6
    static doAngle := 1
    static mostAngle := 1
    
    static modelConfig := Map(
        "zh_hans", Map("cls", "ch_ppocr_mobile_v2.0_cls_infer.onnx", "det", "ch_PP-OCRv4_det_infer.onnx", "rec", "ch_PP-OCRv4_rec_infer.onnx", "keys", "dict_chinese.txt"),
        "zh_hant", Map("cls", "ch_ppocr_mobile_v2.0_cls_infer.onnx", "det", "ch_PP-OCRv4_det_infer.onnx", "rec", "chinese_cht_PP-OCRv3_rec_infer.onnx", "keys", "dict_chinese_cht.txt"),
        "en", Map("cls", "ch_ppocr_mobile_v2.0_cls_infer.onnx", "det", "en_PP-OCRv3_det_infer.onnx", "rec", "en_PP-OCRv4_rec_infer.onnx", "keys", "dict_en.txt"),
        "ja", Map("cls", "ch_ppocr_mobile_v2.0_cls_infer.onnx", "det", "Multilingual_PP-OCRv3_det_infer.onnx", "rec", "japan_PP-OCRv4_rec_infer.onnx", "keys", "dict_japan.txt"),
        "ko", Map("cls", "ch_ppocr_mobile_v2.0_cls_infer.onnx", "det", "Multilingual_PP-OCRv3_det_infer.onnx", "rec", "korean_PP-OCRv4_rec_infer.onnx", "keys", "dict_korean.txt")
    )
    
    static Initialize(config := "") {
        if (config != "") {
            if (config.Has("ExePath"))
                this.exePath := config["ExePath"]
            if (config.Has("ModelPath"))
                this.modelPath := config["ModelPath"]
            if (config.Has("Model"))
                this.model := config["Model"]
            if (config.Has("NumThreads"))
                this.numThreads := config["NumThreads"]
            if (config.Has("BoxScoreThresh"))
                this.boxScoreThresh := config["BoxScoreThresh"]
            if (config.Has("BoxThresh"))
                this.boxThresh := config["BoxThresh"]
        }
        
        if !this.InitGdiplus() {
            LogError("GDI+ 初始化失败")
            return false
        }
        
        exeFullPath := A_WorkingDir "\" this.exePath
        if !FileExist(exeFullPath) {
            LogError("RapidOCR-json.exe 不存在: " exeFullPath)
            this.initialized := false
            this.ocrAvailable := false
            return false
        }
        
        modelFullPath := A_WorkingDir "\" this.modelPath
        if !DirExist(modelFullPath) {
            LogError("模型目录不存在: " modelFullPath)
            this.initialized := false
            this.ocrAvailable := false
            return false
        }
        
        if !this.CheckModelFiles() {
            LogError("模型文件不完整")
            this.initialized := false
            this.ocrAvailable := false
            return false
        }
        
        if !this.StartOCRProcess() {
            LogError("启动 OCR 进程失败")
            this.initialized := false
            this.ocrAvailable := false
            return false
        }
        
        this.initialized := true
        this.ocrAvailable := true
        LogInfo("RapidOCR 初始化成功, 模型: " this.model)
        return true
    }
    
    static InitGdiplus() {
        if (this.gdiplusToken != 0)
            return true
        
        try {
            si := Buffer(24, 0)
            NumPut("UInt", 1, si, 0)
            token := 0
            result := DllCall("Gdiplus\GdiplusStartup", "Ptr*", &token, "Ptr", si, "Ptr", 0)
            if (result = 0) {
                this.gdiplusToken := token
                return true
            }
            LogError("GdiplusStartup 失败: " result)
            return false
        } catch as e {
            LogError("GDI+ 初始化异常: " e.Message)
            return false
        }
    }
    
    static ShutdownGdiplus() {
        if (this.gdiplusToken != 0) {
            try {
                DllCall("Gdiplus\GdiplusShutdown", "Ptr", this.gdiplusToken)
            } catch {
            }
            this.gdiplusToken := 0
        }
    }
    
    static CheckModelFiles() {
        modelDir := A_WorkingDir "\" this.modelPath "\"
        config := this.modelConfig[this.model]
        
        files := [
            config["cls"],
            config["det"],
            config["rec"],
            config["keys"]
        ]
        
        for file in files {
            if !FileExist(modelDir file) {
                LogError("模型文件缺失: " file)
                return false
            }
        }
        return true
    }
    
    static StartOCRProcess() {
        try {
            config := this.modelConfig[this.model]
            
            exeDir := A_WorkingDir "\lib\RapidOCR\exe"
            this.prevWorkingDir := A_WorkingDir
            SetWorkingDir exeDir
            
            if !FileExist("models\" config["cls"]) {
                LogError("cls 模型文件不存在: " config["cls"])
                SetWorkingDir this.prevWorkingDir
                return false
            }
            if !FileExist("models\" config["det"]) {
                LogError("det 模型文件不存在: " config["det"])
                SetWorkingDir this.prevWorkingDir
                return false
            }
            if !FileExist("models\" config["rec"]) {
                LogError("rec 模型文件不存在: " config["rec"])
                SetWorkingDir this.prevWorkingDir
                return false
            }
            if !FileExist("models\" config["keys"]) {
                LogError("keys 文件不存在: " config["keys"])
                SetWorkingDir this.prevWorkingDir
                return false
            }
            
            template := Format(
                'RapidOCR-json.exe --models="models" --ensureAscii=1 --cls="{}" --det="{}" --rec="{}" --keys="{}" --numThread="{}" --padding="{}" --maxSideLen="{}" --boxThresh="{}" --boxScoreThresh="{}" --unClipRatio="{}" --doAngle="{}" --mostAngle="{}"',
                config["cls"],
                config["det"],
                config["rec"],
                config["keys"],
                this.numThreads,
                this.padding,
                this.maxSideLen,
                this.boxThresh,
                this.boxScoreThresh,
                this.unclipRatio,
                this.doAngle,
                this.mostAngle
            )
            
            cmdPid := 0
            Run A_ComSpec,, "Hide", &cmdPid
            if (cmdPid = 0) {
                LogError("启动命令行失败")
                SetWorkingDir this.prevWorkingDir
                return false
            }
            
            DetectHiddenWindows true
            WinWait "ahk_pid " cmdPid, , 5
            
            DllCall("AttachConsole", "UInt", cmdPid)
            ProcessClose cmdPid
            
            this.shell := ComObject("WScript.Shell")
            this.exec := this.shell.Exec(template)
            
            DllCall("FreeConsole")
            SetWorkingDir this.prevWorkingDir
            
            loop 100 {
                Sleep 100
                if (this.exec.Status = 0) {
                    this.exec.StdOut.ReadLine()
                    line := this.exec.StdOut.ReadLine()
                    if (line = "OCR init completed.") {
                        return true
                    }
                    LogError("OCR 初始化失败: " line)
                    return false
                }
                
                if (A_Index >= 100) {
                    LogError("OCR 启动超时")
                    return false
                }
            }
            
            return false
        } catch as e {
            LogError("启动 OCR 进程异常: " e.Message)
            if (this.prevWorkingDir != "")
                SetWorkingDir this.prevWorkingDir
            return false
        }
    }
    
    static IsInitialized() {
        return this.initialized
    }
    
    static IsAvailable() {
        return this.ocrAvailable
    }
    
    static OCR(bitmap) {
        if !this.ocrAvailable {
            LogError("OCR 不可用")
            return []
        }
        
        try {
            base64Data := this.BitmapToBase64(bitmap)
            if (base64Data = "") {
                LogError("图像转 Base64 失败")
                return []
            }
            
            jsonReq := '{"image_base64":"' base64Data '"}'
            this.exec.StdIn.WriteLine(jsonReq)
            
            jsonResp := this.exec.StdOut.ReadLine()
            LogInfo("OCR 响应长度: " StrLen(jsonResp) ", 前100字符: " SubStr(jsonResp, 1, 100))
            return this.ParseOCRResult(jsonResp)
        } catch as e {
            LogError("OCR 识别失败: " e.Message)
            return []
        }
    }
    
    static OCRFile(filePath) {
        if !this.ocrAvailable {
            LogError("OCR 不可用")
            return []
        }
        
        if !FileExist(filePath) {
            LogError("文件不存在: " filePath)
            return []
        }
        
        try {
            base64Data := this.FileToBase64(filePath)
            if (base64Data = "") {
                LogError("文件转 Base64 失败")
                return []
            }
            
            jsonReq := '{"image_base64":"' base64Data '"}'
            this.exec.StdIn.WriteLine(jsonReq)
            
            jsonResp := this.exec.StdOut.ReadLine()
            return this.ParseOCRResult(jsonResp)
        } catch as e {
            LogError("OCR 文件识别失败: " e.Message)
            return []
        }
    }
    
    static OCRScreen(x, y, w, h) {
        if !this.ocrAvailable {
            LogError("OCR 不可用")
            return []
        }
        
        bitmap := Utils.CaptureScreen(x, y, w, h)
        if (bitmap = 0) {
            LogError("截图失败")
            return []
        }
        
        try {
            result := this.OCR(bitmap)
            return result
        } finally {
            Utils.DeleteBitmap(bitmap)
        }
    }
    
    static OCRByCoords(coords) {
        if !this.ocrAvailable {
            LogError("OCR 不可用")
            return ""
        }
        
        try {
            if (coords.Length < 4) {
                LogError("坐标参数不足")
                return ""
            }
            
            x := coords[1]
            y := coords[2]
            w := coords[3] - coords[1]
            h := coords[4] - coords[2]
            
            results := this.OCRScreen(x, y, w, h)
            return this.ExtractText(results)
        } catch as e {
            LogError("坐标 OCR 失败: " e.Message)
            return ""
        }
    }
    
    static ParseOCRResult(jsonStr) {
        try {
            result := Jxon_Load(&jsonStr)
            
            code := result["code"]
            if (code != 100 && code != 101) {
                LogError("OCR 返回错误: " result["data"])
                return []
            }
            
            data := result["data"]
            results := []
            
            for item in data {
                results.Push(Map(
                    "text", item["text"],
                    "score", item["score"],
                    "box", item["box"]
                ))
            }
            
            return results
        } catch as e {
            LogError("解析 OCR 结果失败: " e.Message)
            return []
        }
    }
    
    static ExtractText(results) {
        text := ""
        for item in results {
            if (item.Has("text"))
                text .= item["text"] " "
        }
        return Trim(text)
    }
    
    static BitmapToBase64(hBitmap) {
        gdiplusBitmap := 0
        try {
            result := DllCall("Gdiplus\GdipCreateBitmapFromHBITMAP", "Ptr", hBitmap, "Ptr", 0, "Ptr*", &gdiplusBitmap)
            if (result != 0 || gdiplusBitmap = 0) {
                LogError("GdipCreateBitmapFromHBITMAP 失败: " result)
                return ""
            }
            
            pngClsid := Buffer(16, 0)
            NumPut("UInt", 0x557CF406, pngClsid, 0)
            NumPut("UShort", 0x1A04, pngClsid, 4)
            NumPut("UShort", 0x11D3, pngClsid, 6)
            NumPut("UChar", 0x9A, pngClsid, 8)
            NumPut("UChar", 0x73, pngClsid, 9)
            NumPut("UChar", 0x00, pngClsid, 10)
            NumPut("UChar", 0x00, pngClsid, 11)
            NumPut("UChar", 0xF8, pngClsid, 12)
            NumPut("UChar", 0x1E, pngClsid, 13)
            NumPut("UChar", 0xF3, pngClsid, 14)
            NumPut("UChar", 0x2E, pngClsid, 15)
            
            pStream := 0
            DllCall("Ole32\CreateStreamOnHGlobal", "Ptr", 0, "Int", true, "Ptr*", &pStream)
            
            if (pStream = 0) {
                DllCall("Gdiplus\GdipDisposeImage", "Ptr", gdiplusBitmap)
                return ""
            }
            
            result := DllCall("Gdiplus\GdipSaveImageToStream", "Ptr", gdiplusBitmap, "Ptr", pStream, "Ptr", pngClsid.Ptr, "Ptr", 0)
            
            if (result != 0) {
                ObjRelease(pStream)
                DllCall("Gdiplus\GdipDisposeImage", "Ptr", gdiplusBitmap)
                LogError("GdipSaveImageToStream 失败: " result)
                return ""
            }
            
            hGlobal := 0
            DllCall("Ole32\GetHGlobalFromStream", "Ptr", pStream, "UInt*", &hGlobal)
            
            if (hGlobal = 0) {
                ObjRelease(pStream)
                DllCall("Gdiplus\GdipDisposeImage", "Ptr", gdiplusBitmap)
                return ""
            }
            
            pData := DllCall("GlobalLock", "Ptr", hGlobal, "Ptr")
            dataSize := DllCall("GlobalSize", "Ptr", hGlobal, "UPtr")
            
            if (pData = 0 || dataSize = 0) {
                DllCall("GlobalUnlock", "Ptr", hGlobal)
                ObjRelease(pStream)
                DllCall("Gdiplus\GdipDisposeImage", "Ptr", gdiplusBitmap)
                return ""
            }
            
            binData := Buffer(dataSize, 0)
            DllCall("RtlMoveMemory", "Ptr", binData.Ptr, "Ptr", pData, "UPtr", dataSize)
            
            DllCall("GlobalUnlock", "Ptr", hGlobal)
            ObjRelease(pStream)
            DllCall("Gdiplus\GdipDisposeImage", "Ptr", gdiplusBitmap)
            
            base64Result := this.Base64Encode(binData)
            LogInfo("Base64 编码完成, 数据大小: " dataSize ", Base64长度: " StrLen(base64Result) ", 前50字符: " SubStr(base64Result, 1, 50))
            return base64Result
        } catch as e {
            if (gdiplusBitmap != 0)
                DllCall("Gdiplus\GdipDisposeImage", "Ptr", gdiplusBitmap)
            LogError("BitmapToBase64 失败: " e.Message)
            return ""
        }
    }
    
    static FileToBase64(filePath) {
        try {
            file := FileOpen(filePath, "r")
            if (!file)
                return ""
            
            size := file.Length
            data := Buffer(size)
            file.RawRead(data)
            file.Close()
            
            return this.Base64Encode(data)
        } catch as e {
            LogError("FileToBase64 失败: " e.Message)
            return ""
        }
    }
    
    static Base64Encode(data) {
        try {
            size := data.Size
            base64Len := 0
            DllCall("Crypt32\CryptBinaryToStringW", "Ptr", data.Ptr, "UInt", size, "UInt", 0x40000001, "Ptr", 0, "UInt*", &base64Len)
            
            if (base64Len = 0)
                return ""
            
            base64Str := Buffer(base64Len * 2, 0)
            DllCall("Crypt32\CryptBinaryToStringW", "Ptr", data.Ptr, "UInt", size, "UInt", 0x40000001, "Ptr", base64Str.Ptr, "UInt*", &base64Len)
            
            result := StrGet(base64Str.Ptr, "UTF-16")
            result := StrReplace(result, "`r", "")
            result := StrReplace(result, "`n", "")
            return result
        } catch as e {
            LogError("Base64Encode 失败: " e.Message)
            return ""
        }
    }
    
    static Reinitialize() {
        this.Shutdown()
        return this.Initialize()
    }
    
    static Shutdown() {
        try {
            if (this.exec != "") {
                this.exec.Terminate()
                this.exec := ""
            }
        } catch {
        }
        
        this.shell := ""
        this.initialized := false
        this.ocrAvailable := false
        this.ShutdownGdiplus()
    }
}

Jxon_Load(&src, args*) {
    global jxon_pos, jxon_src
    jxon_pos := 1
    jxon_src := src
    
    result := Jxon_ParseValue()
    src := jxon_src
    return result
}

Jxon_ParseValue() {
    global jxon_pos, jxon_src
    Jxon_SkipWhitespace()
    ch := SubStr(jxon_src, jxon_pos, 1)
    
    if (ch = "{")
        return Jxon_ParseObject()
    else if (ch = "[")
        return Jxon_ParseArray()
    else if (ch = '"')
        return Jxon_ParseString()
    else if (ch = "t") {
        jxon_pos += 4
        return true
    } else if (ch = "f") {
        jxon_pos += 5
        return false
    } else if (ch = "n") {
        jxon_pos += 4
        return ""
    } else if InStr("-0123456789", ch)
        return Jxon_ParseNumber()
    else
        throw Error("JSON Parse Error at position " jxon_pos)
}

Jxon_SkipWhitespace() {
    global jxon_pos, jxon_src
    while (SubStr(jxon_src, jxon_pos, 1) = " " || SubStr(jxon_src, jxon_pos, 1) = "`t" || SubStr(jxon_src, jxon_pos, 1) = "`n" || SubStr(jxon_src, jxon_pos, 1) = "`r")
        jxon_pos++
}

Jxon_ParseObject() {
    global jxon_pos, jxon_src
    obj := Map()
    jxon_pos++
    Jxon_SkipWhitespace()
    
    if (SubStr(jxon_src, jxon_pos, 1) = "}") {
        jxon_pos++
        return obj
    }
    
    loop {
        Jxon_SkipWhitespace()
        key := Jxon_ParseString()
        Jxon_SkipWhitespace()
        jxon_pos++
        value := Jxon_ParseValue()
        obj[key] := value
        Jxon_SkipWhitespace()
        
        ch := SubStr(jxon_src, jxon_pos, 1)
        if (ch = "}") {
            jxon_pos++
            return obj
        } else if (ch = ",") {
            jxon_pos++
        } else
            throw Error("JSON Parse Error: Expected ',' or '}' at position " jxon_pos)
    }
}

Jxon_ParseArray() {
    global jxon_pos, jxon_src
    arr := []
    jxon_pos++
    Jxon_SkipWhitespace()
    
    if (SubStr(jxon_src, jxon_pos, 1) = "]") {
        jxon_pos++
        return arr
    }
    
    loop {
        Jxon_SkipWhitespace()
        arr.Push(Jxon_ParseValue())
        Jxon_SkipWhitespace()
        
        ch := SubStr(jxon_src, jxon_pos, 1)
        if (ch = "]") {
            jxon_pos++
            return arr
        } else if (ch = ",") {
            jxon_pos++
        } else
            throw Error("JSON Parse Error: Expected ',' or ']' at position " jxon_pos)
    }
}

Jxon_ParseString() {
    global jxon_pos, jxon_src
    jxon_pos++
    result := ""
    
    loop {
        ch := SubStr(jxon_src, jxon_pos, 1)
        if (ch = '"') {
            jxon_pos++
            return result
        } else if (ch = "\") {
            jxon_pos++
            esc := SubStr(jxon_src, jxon_pos, 1)
            if (esc = "n")
                result .= "`n"
            else if (esc = "r")
                result .= "`r"
            else if (esc = "t")
                result .= "`t"
            else if (esc = "b")
                result .= "`b"
            else if (esc = "f")
                result .= "`f"
            else if (esc = "u") {
                hex := SubStr(jxon_src, jxon_pos + 1, 4)
                result .= Chr(Integer("0x" hex))
                jxon_pos += 4
            } else
                result .= esc
            jxon_pos++
        } else {
            result .= ch
            jxon_pos++
        }
    }
}

Jxon_ParseNumber() {
    global jxon_pos, jxon_src
    start := jxon_pos
    if (SubStr(jxon_src, jxon_pos, 1) = "-")
        jxon_pos++
    while InStr("0123456789", SubStr(jxon_src, jxon_pos, 1))
        jxon_pos++
    if (SubStr(jxon_src, jxon_pos, 1) = ".") {
        jxon_pos++
        while InStr("0123456789", SubStr(jxon_src, jxon_pos, 1))
            jxon_pos++
    }
    if (SubStr(jxon_src, jxon_pos, 1) = "e" || SubStr(jxon_src, jxon_pos, 1) = "E") {
        jxon_pos++
        if (SubStr(jxon_src, jxon_pos, 1) = "+" || SubStr(jxon_src, jxon_pos, 1) = "-")
            jxon_pos++
        while InStr("0123456789", SubStr(jxon_src, jxon_pos, 1))
            jxon_pos++
    }
    
    numStr := SubStr(jxon_src, start, jxon_pos - start)
    if (InStr(numStr, ".") || InStr(numStr, "e") || InStr(numStr, "E"))
        return Float(numStr)
    else
        return Integer(numStr)
}
