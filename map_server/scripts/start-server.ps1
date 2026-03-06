#!/usr/bin/env pwsh
# 启动Leaf地图服务系统的跨平台脚本 (Windows PowerShell版本)
# 功能: 1) 启动后端地图服务器(map_server) 2) 启动前端Web开发服务器 3) 在浏览器中打开应用
# 要求: PowerShell 5.1+ (Windows), 或 PowerShell Core (跨平台)
# 注意: 本脚本假设项目已构建完成，后端可执行文件位于build/bin/server.exe

param(
    [string]$ServerExePath = "..\build\bin\server.exe",  # 后端服务器可执行文件路径
    [string]$ConfigPath = "..\build\config.example.json",      # 服务器配置文件路径
    [int]$ServerPort = 8080,                                   # 后端服务器端口
    [int]$FrontendPort = 3000,                                 # 前端开发服务器端口
    [string]$FrontendDir = "..\web",                           # 前端项目目录
    [int]$HealthCheckTimeout = 30,                             # 健康检查超时(秒)
    [int]$HealthCheckInterval = 2,                             # 健康检查间隔(秒)
    [switch]$SkipBackend                                       # 跳过启动后端服务器
)

# 获取脚本所在目录作为根目录
$ScriptRoot = $PSScriptRoot
if (-Not $ScriptRoot) {
    $ScriptRoot = (Get-Location).Path
}
Write-Host "脚本根目录: $ScriptRoot" -ForegroundColor Gray

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "Leaf 地图服务系统启动脚本 (PowerShell)" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "后端端口: $ServerPort, 前端端口: $FrontendPort" -ForegroundColor Yellow
Write-Host ""

# 函数: 检查后端服务器健康状态
function Test-ServerHealth {
    param([int]$Port)
    try {
        $response = Invoke-WebRequest -Uri "http://localhost:$Port/health" -TimeoutSec 3 -ErrorAction Stop -UseBasicParsing
        return $response.StatusCode -eq 200
    } catch {
        return $false
    }
}

# 函数: 查找后端可执行文件
function Find-BackendExe {
    param([string]$DefaultPath)
    
    # 尝试多个可能的路径（基于脚本根目录和当前目录）
    $possiblePaths = @(
        $DefaultPath,                                 # 默认路径
        "..\build\bin\server.exe",                    # 相对路径 server.exe
        "..\build\bin\map_server.exe",                # 相对路径 map_server.exe
        "..\build\x64\Debug\map_server.exe",          # Debug构建目录
        (Join-Path $ScriptRoot "..\build\bin\server.exe"),      # 基于脚本根目录
        (Join-Path $ScriptRoot "..\build\bin\map_server.exe"),  # 基于脚本根目录
        (Join-Path $ScriptRoot "..\build\x64\Debug\map_server.exe") # 基于脚本根目录
    )
    
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            Write-Host "找到后端可执行文件: $path" -ForegroundColor Green
            return $path
        }
    }
    
    return $null
}

# 函数: 查找前端目录
function Find-FrontendDir {
    param([string]$DefaultPath)
    
    # 尝试多个可能的路径（基于脚本根目录和当前目录）
    $possiblePaths = @(
        $DefaultPath,                                 # 默认路径
        "..\web",                                     # 相对于脚本目录
        "web",                                        # 相对于当前目录
        (Join-Path $ScriptRoot "..\web"),             # 基于脚本根目录
        (Join-Path $ScriptRoot "web")                 # 基于脚本根目录
    )
    
    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            Write-Host "找到前端目录: $path" -ForegroundColor Green
            return $path
        }
    }
    
    return $null
}

# 函数: 启动后端服务器进程
function Start-BackendServer {
    param([string]$ExePath, [string]$ConfigPath)
    
    # 先尝试查找可执行文件
    $foundExePath = Find-BackendExe -DefaultPath $ExePath
    
    if (-Not $foundExePath) {
        Write-Host "错误: 找不到后端服务器可执行文件" -ForegroundColor Red
        Write-Host "已检查的路径:" -ForegroundColor Gray
        Write-Host "  - ..\build\bin\server.exe" -ForegroundColor Gray
        Write-Host "  - ..\build\bin\map_server.exe" -ForegroundColor Gray
        Write-Host "  - ..\build\x64\Debug\map_server.exe" -ForegroundColor Gray
        Write-Host "  - $((Join-Path $ScriptRoot "..\build\bin\server.exe"))" -ForegroundColor Gray
        Write-Host "  - $((Join-Path $ScriptRoot "..\build\bin\map_server.exe"))" -ForegroundColor Gray
        Write-Host "  - $((Join-Path $ScriptRoot "..\build\x64\Debug\map_server.exe"))" -ForegroundColor Gray
        Write-Host "请先构建项目: cmake --build build --target map_server" -ForegroundColor Yellow
        Write-Host "或手动将 server.exe / map_server.exe 放置到上述目录之一" -ForegroundColor Yellow
        return $false
    }
    
    # 使用找到的路径
    $ExePath = $foundExePath
    Write-Host "可执行文件: $(Resolve-Path $ExePath)" -ForegroundColor Gray
    
    # 处理配置文件路径
    $ConfigFullPath = Join-Path $ScriptRoot $ConfigPath
    if (-Not (Test-Path $ConfigFullPath)) {
        Write-Host "警告: 找不到配置文件 $ConfigFullPath，使用默认配置运行" -ForegroundColor Yellow
        $ConfigFullPath = ""
    }
    
    Write-Host "启动后端服务器..." -ForegroundColor Green
    
    try {
        # 启动服务器进程
        if ($ConfigFullPath -ne "") {
            Write-Host "使用配置文件: $ConfigFullPath" -ForegroundColor Gray
            # 服务器期望 --config 标志，而不是单独的文件路径参数
            $serverProcess = Start-Process -FilePath $ExePath -ArgumentList "--config", $ConfigFullPath -PassThru -NoNewWindow
        }
        else {
            Write-Host "使用默认配置运行" -ForegroundColor Gray
            $serverProcess = Start-Process -FilePath $ExePath -PassThru -NoNewWindow
        }
        
        # 保存进程ID供后续使用
        $script:ServerProcessId = $serverProcess.Id
        Write-Host "后端服务器进程已启动 (PID: $($serverProcess.Id))" -ForegroundColor Green
        
        return $true
    } catch {
        Write-Host "启动后端服务器失败: $_" -ForegroundColor Red
        return $false
    }
}

# 函数: 启动前端开发服务器
function Start-FrontendServer {
    param([string]$FrontendDir, [int]$Port)
    
    # 先尝试查找前端目录
    $FrontendTarDir = $((Join-Path $ScriptRoot $FrontendDir))
    $foundFrontendDir = Find-FrontendDir -DefaultPath $FrontendTarDir
    
    if (-Not $foundFrontendDir) {
        Write-Host "错误: 找不到前端目录" -ForegroundColor Red
        Write-Host "已检查的路径:" -ForegroundColor Gray
        Write-Host "  - $FrontendDir" -ForegroundColor Gray
        Write-Host "  - ..\web" -ForegroundColor Gray
        Write-Host "  - web" -ForegroundColor Gray
        Write-Host "  - $((Join-Path $ScriptRoot "..\web"))" -ForegroundColor Gray
        Write-Host "  - $((Join-Path $ScriptRoot "web"))" -ForegroundColor Gray
        Write-Host "请确保前端项目存在于上述目录之一" -ForegroundColor Yellow
        return $false
    }
    
    # 使用找到的目录
    $FrontendDir = $foundFrontendDir
    Write-Host "前端目录: $(Resolve-Path $FrontendDir)" -ForegroundColor Green

    $packageJson = Join-Path $FrontendDir "package.json"
    if (-Not (Test-Path $packageJson)) {
        Write-Host "错误: 前端目录中找不到 package.json" -ForegroundColor Red
        Write-Host "前端目录: $(Resolve-Path $FrontendDir)" -ForegroundColor Gray
        return $false
    }
    
    Write-Host "启动前端开发服务器..." -ForegroundColor Green
    Write-Host "前端目录: $(Resolve-Path $FrontendDir)" -ForegroundColor Gray
    
    # 检查 npm 是否可用
    if (-Not (Get-Command npm -ErrorAction SilentlyContinue)) {
        Write-Host "错误: 未找到 npm 命令，请确保 Node.js 已安装并添加到 PATH" -ForegroundColor Red
        return $false
    }
    
    # 检查 node_modules 是否存在
    $nodeModulesPath = Join-Path $FrontendDir "node_modules"
    if (-Not (Test-Path $nodeModulesPath)) {
        Write-Host "警告: node_modules 不存在，尝试安装依赖..." -ForegroundColor Yellow
        Write-Host "运行 npm install..." -ForegroundColor Gray
        try {
            Push-Location $(Resolve-Path $FrontendDir)
            npm install 2>&1 | Out-Host
            if ($LASTEXITCODE -ne 0) {
                Write-Host "错误: npm install 失败" -ForegroundColor Red
                Pop-Location
                return $false
            }
            Pop-Location
        } catch {
            Write-Host "错误: 安装依赖时出错: $_" -ForegroundColor Red
            Pop-Location -ErrorAction SilentlyContinue
            return $false
        }
    }
    
    try {
        # 获取 npm.cmd 的完整路径
        $npmCmdPath = (Get-Command npm.cmd).Source
        Write-Host "使用 npm 路径: $npmCmdPath" -ForegroundColor Gray
        
        # 切换到前端目录
        Write-Host "切换到前端目录..." -ForegroundColor Green
        $resolvedFrontendDir = Resolve-Path $FrontendDir
        Push-Location $resolvedFrontendDir
        
        Write-Host "执行 npm run dev..." -ForegroundColor Green
        
        # 使用 cmd.exe 启动 npm，并重定向所有输入输出到 NUL 设备以避免 "Terminate batch job" 提示和 Vite 输出
        $script:FrontendProcess = Start-Process -FilePath "cmd.exe" -ArgumentList "/c", "`"npm run dev < NUL > NUL 2>&1`"" -PassThru -NoNewWindow -WorkingDirectory $resolvedFrontendDir
        
        # 保存进程ID
        $script:FrontendProcessId = $script:FrontendProcess.Id
        
        # 等待片刻，检查进程是否仍在运行
        Start-Sleep -Seconds 2
        if ($script:FrontendProcess.HasExited) {
            Write-Host "错误: 前端服务器进程已退出 (退出代码: $($script:FrontendProcess.ExitCode))" -ForegroundColor Red
            Pop-Location
            return $false
        }
        
        Write-Host "前端开发服务器已启动 (PID: $($script:FrontendProcessId))" -ForegroundColor Green
        Pop-Location
        
        return $true
    } catch {
        Write-Host "启动前端开发服务器失败: $_" -ForegroundColor Red
        Pop-Location -ErrorAction SilentlyContinue
        return $false
    }
}

# 函数: 在浏览器中打开应用
function Open-InBrowser {
    param([int]$Port)
    
    $url = "http://localhost:$Port"
    Write-Host "在浏览器中打开: $url" -ForegroundColor Cyan
    
    try {
        Start-Process $url
        Write-Host "浏览器已启动" -ForegroundColor Green
    } catch {
        Write-Host "无法自动打开浏览器，请手动访问: $url" -ForegroundColor Yellow
    }
}

# 函数: 清理函数 (Ctrl+C处理)
function Cleanup {
    Write-Host ""
    Write-Host "正在清理进程..." -ForegroundColor Yellow
    
    if ($script:ServerProcessId -gt 0) {
        Write-Host "停止后端服务器 (PID: $($script:ServerProcessId))..." -ForegroundColor Yellow
        Stop-Process -Id $script:ServerProcessId -Force -ErrorAction SilentlyContinue
    }
    
    # 停止前端开发服务器（包括其子进程）
    if ($script:FrontendProcessId -gt 0) {
        Write-Host "停止前端开发服务器 (PID: $($script:FrontendProcessId))..." -ForegroundColor Yellow
        try {
            # 首先尝试使用 PowerShell 停止进程（包括子进程）
            Stop-Process -Id $script:FrontendProcessId -Force -IncludeChildProcess -ErrorAction SilentlyContinue
        } catch {
            # 如果失败，使用 taskkill 强制终止进程树
            try {
                taskkill /F /T /PID $script:FrontendProcessId 2>&1 | Out-Null
            } catch {
                Write-Host "警告: 无法终止前端服务器进程" -ForegroundColor Yellow
            }
        }
    }
    
    # 额外的清理：停止任何可能遗留的 npm 或 node 进程
    if ($script:FrontendProcess -and $script:FrontendProcess.Id -gt 0) {
        $pidToKill = $script:FrontendProcess.Id
        try {
            Stop-Process -Id $pidToKill -Force -ErrorAction SilentlyContinue
        } catch {
            # 忽略错误
        }
    }
    
    # 强制清理：查找并终止占用前端端口的所有进程
    Write-Host "清理占用端口 $FrontendPort 的进程..." -ForegroundColor Yellow
    try {
        # 使用 netstat 查找占用端口的进程
        $portProcesses = netstat -ano | findstr ":$FrontendPort"
        foreach ($line in $portProcesses) {
            if ($line -match '\s+(\d+)\s*$') {
                $foundPid = $matches[1]
                Write-Host "发现占用端口 $FrontendPort 的进程 PID: $foundPid" -ForegroundColor Gray
                try {
                    taskkill /F /PID $foundPid 2>&1 | Out-Null
                } catch {
                    try {
                        Stop-Process -Id $foundPid -Force -ErrorAction SilentlyContinue
                    } catch {
                        # 忽略错误
                    }
                }
            }
        }
    } catch {
        # 忽略 netstat 错误
    }
    
    Write-Host "清理完成" -ForegroundColor Green
    exit 0
}

# 设置Ctrl+C处理和进程变量
$script:ServerProcessId = 0
$script:FrontendProcessId = 0
$script:FrontendProcess = $null

# 注册Ctrl+C处理程序
$ctrlCHandler = [ConsoleCancelEventHandler]{
    param($sender, $e)
    $e.Cancel = $true  # 防止PowerShell立即终止
    Write-Host ""
    Write-Host "Ctrl+C 被按下，正在清理进程..." -ForegroundColor Yellow
    Cleanup
}

try {
    [Console]::CancelKeyPress.AddEventHandler($ctrlCHandler)
    Write-Host "Ctrl+C 处理程序已注册" -ForegroundColor Gray
} catch {
    Write-Host "警告: 无法注册高级Ctrl+C处理程序，使用基本异常处理" -ForegroundColor Yellow
}

# 通用的异常处理（备用方案）
trap {
    Write-Host "脚本异常: $_" -ForegroundColor Red
    Cleanup
    break
}



# 主执行流程
try {
    # 步骤1: 检查后端服务器是否已在运行 (可跳过)
    if (-Not $SkipBackend) {
        Write-Host "检查后端服务器状态..." -ForegroundColor Cyan
        if (Test-ServerHealth -Port $ServerPort) {
            Write-Host "后端服务器已在运行 (端口: $ServerPort)" -ForegroundColor Green
        }
        else {
            Write-Host "后端服务器未运行，尝试启动..." -ForegroundColor Yellow
            
            # 启动后端服务器
            if (-Not (Start-BackendServer -ExePath $ServerExePath -ConfigPath $ConfigPath)) {
                Write-Host "后端服务器启动失败，退出脚本" -ForegroundColor Red
                exit 1
            }
            
            # 等待服务器启动完成
            Write-Host "等待后端服务器启动..." -ForegroundColor Cyan
            $timeElapsed = 0
            $serverReady = $false
            
            while ($timeElapsed -lt $HealthCheckTimeout) {
                Start-Sleep -Seconds $HealthCheckInterval
                $timeElapsed += $HealthCheckInterval
                
                if (Test-ServerHealth -Port $ServerPort) {
                    $serverReady = $true
                    break
                }
                
                Write-Host "等待服务器响应... ($timeElapsed/$HealthCheckTimeout 秒)" -ForegroundColor Gray
            }
            
            if (-Not $serverReady) {
                Write-Host "错误: 后端服务器在 $HealthCheckTimeout 秒内未响应" -ForegroundColor Red
                Cleanup
                exit 1
            }
            
            Write-Host "后端服务器已就绪!" -ForegroundColor Green
        }
    } else {
        Write-Host "跳过启动后端服务器 (用户指定 -SkipBackend)" -ForegroundColor Yellow
    }
    
    # 步骤2: 启动前端开发服务器
    Write-Host ""
    Write-Host "检查前端开发服务器状态..." -ForegroundColor Cyan
    
    # 简单检查前端端口是否被占用 (不严谨，但可作为参考)
    try {
        $response = Invoke-WebRequest -Uri "http://localhost:$FrontendPort" -TimeoutSec 3 -ErrorAction Stop -UseBasicParsing
        Write-Host "前端开发服务器可能已在运行 (端口: $FrontendPort)" -ForegroundColor Green
    } catch {
        # 端口未响应，启动前端服务器
        if (-Not (Start-FrontendServer -FrontendDir $FrontendDir -Port $FrontendPort)) {
            Write-Host "前端开发服务器启动失败，退出脚本" -ForegroundColor Red
            Cleanup
            exit 1
        }
        
        Write-Host "等待前端服务器启动..." -ForegroundColor Cyan
        Start-Sleep -Seconds 5  # 给Vite一些启动时间
    }
    
    # 步骤3: 在浏览器中打开应用
    Write-Host ""
    Open-InBrowser -Port $FrontendPort
    
    Write-Host ""
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host "Leaf 地图服务系统已成功启动!" -ForegroundColor Green
    Write-Host "后端API: http://localhost:$ServerPort" -ForegroundColor Gray
    Write-Host "前端应用: http://localhost:$FrontendPort" -ForegroundColor Gray
    Write-Host "按 Ctrl+C 停止所有服务" -ForegroundColor Yellow
    Write-Host "================================================" -ForegroundColor Cyan
    
    # 保持脚本运行，直到用户中断
    while ($true) {
        Start-Sleep -Seconds 1
    }
}
catch {
    Write-Host ("脚本执行出错: " + $_) -ForegroundColor Red
    Cleanup
    exit 1
}