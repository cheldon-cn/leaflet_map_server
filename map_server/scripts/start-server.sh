#!/bin/bash
# 启动Leaf地图服务系统的跨平台脚本 (Linux/macOS Shell版本)
# 功能: 1) 启动后端地图服务器(map_server) 2) 启动前端Web开发服务器 3) 在浏览器中打开应用
# 要求: bash, curl, npm, 支持xdg-open(linux)或open(mac)命令
# 注意: 本脚本假设项目已构建完成，后端可执行文件位于build/map_server

# 配置参数
SERVER_EXE_PATH="../build/map_server"          # 后端服务器可执行文件路径
CONFIG_PATH="../build/config.example.json"     # 服务器配置文件路径
SERVER_PORT=8080                               # 后端服务器端口
FRONTEND_PORT=3000                             # 前端开发服务器端口
FRONTEND_DIR="../web"                          # 前端项目目录
HEALTH_CHECK_TIMEOUT=30                        # 健康检查超时(秒)
HEALTH_CHECK_INTERVAL=2                        # 健康检查间隔(秒)

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Leaf 地图服务系统启动脚本 (Shell)${NC}"
echo -e "${CYAN}================================================${NC}"
echo -e "${YELLOW}后端端口: $SERVER_PORT, 前端端口: $FRONTEND_PORT${NC}"
echo ""

# 函数: 检查后端服务器健康状态
test_server_health() {
    local port=$1
    if curl -s -f --max-time 3 "http://localhost:$port/health" > /dev/null; then
        return 0  # 成功
    else
        return 1  # 失败
    fi
}

# 函数: 启动后端服务器进程
start_backend_server() {
    local exe_path=$1
    local config_path=$2
    
    if [ ! -f "$exe_path" ]; then
        echo -e "${RED}错误: 找不到后端服务器可执行文件: $exe_path${NC}"
        echo -e "${YELLOW}请先构建项目: cmake --build build --target map_server${NC}"
        return 1
    fi
    
    if [ ! -f "$config_path" ]; then
        echo -e "${YELLOW}警告: 找不到配置文件 $config_path，使用默认配置运行${NC}"
        config_path=""
    fi
    
    echo -e "${GREEN}启动后端服务器...${NC}"
    echo -e "${GRAY}可执行文件: $(realpath "$exe_path")${NC}"
    
    # 启动服务器进程
    if [ -n "$config_path" ]; then
        "$exe_path" "$config_path" &
    else
        "$exe_path" &
    fi
    
    SERVER_PID=$!
    echo -e "${GREEN}后端服务器进程已启动 (PID: $SERVER_PID)${NC}"
    
    return 0
}

# 函数: 启动前端开发服务器
start_frontend_server() {
    local frontend_dir=$1
    local port=$2
    
    if [ ! -d "$frontend_dir" ]; then
        echo -e "${RED}错误: 找不到前端目录: $frontend_dir${NC}"
        return 1
    fi
    
    local package_json="$frontend_dir/package.json"
    if [ ! -f "$package_json" ]; then
        echo -e "${RED}错误: 前端目录中找不到 package.json${NC}"
        return 1
    fi
    
    echo -e "${GREEN}启动前端开发服务器...${NC}"
    echo -e "${GRAY}前端目录: $(realpath "$frontend_dir")${NC}"
    
    # 切换到前端目录并启动开发服务器
    (cd "$frontend_dir" && npm run dev &)
    FRONTEND_PID=$!
    
    echo -e "${GREEN}前端开发服务器已启动 (PID: $FRONTEND_PID)${NC}"
    return 0
}

# 函数: 在浏览器中打开应用
open_in_browser() {
    local port=$1
    local url="http://localhost:$port"
    
    echo -e "${CYAN}在浏览器中打开: $url${NC}"
    
    # 检测操作系统并选择打开命令
    case "$(uname -s)" in
        Linux*)     xdg-open "$url" || echo -e "${YELLOW}无法自动打开浏览器，请手动访问: $url${NC}" ;;
        Darwin*)    open "$url" || echo -e "${YELLOW}无法自动打开浏览器，请手动访问: $url${NC}" ;;
        CYGWIN*|MINGW*|MSYS*) 
                    # Windows环境可能通过Git Bash运行
                    if command -v start >/dev/null 2>&1; then
                        start "$url"
                    else
                        echo -e "${YELLOW}无法自动打开浏览器，请手动访问: $url${NC}"
                    fi ;;
        *)          echo -e "${YELLOW}无法自动打开浏览器，请手动访问: $url${NC}" ;;
    esac
}

# 函数: 清理函数 (Ctrl+C处理)
cleanup() {
    echo ""
    echo -e "${YELLOW}正在清理进程...${NC}"
    
    if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
        echo -e "${YELLOW}停止后端服务器 (PID: $SERVER_PID)...${NC}"
        kill "$SERVER_PID" 2>/dev/null
    fi
    
    if [ -n "$FRONTEND_PID" ] && kill -0 "$FRONTEND_PID" 2>/dev/null; then
        echo -e "${YELLOW}停止前端开发服务器 (PID: $FRONTEND_PID)...${NC}"
        kill "$FRONTEND_PID" 2>/dev/null
    fi
    
    # 清理后台作业
    jobs -p | xargs -r kill
    
    echo -e "${GREEN}清理完成${NC}"
    exit 0
}

# 设置信号处理
trap cleanup INT TERM EXIT

# 变量初始化
SERVER_PID=""
FRONTEND_PID=""

# 主执行流程
main() {
    # 步骤1: 检查后端服务器是否已在运行
    echo -e "${CYAN}检查后端服务器状态...${NC}"
    if test_server_health "$SERVER_PORT"; then
        echo -e "${GREEN}后端服务器已在运行 (端口: $SERVER_PORT)${NC}"
    else
        echo -e "${YELLOW}后端服务器未运行，尝试启动...${NC}"
        
        # 启动后端服务器
        if ! start_backend_server "$SERVER_EXE_PATH" "$CONFIG_PATH"; then
            echo -e "${RED}后端服务器启动失败，退出脚本${NC}"
            exit 1
        fi
        
        # 等待服务器启动完成
        echo -e "${CYAN}等待后端服务器启动...${NC}"
        local time_elapsed=0
        local server_ready=false
        
        while [ $time_elapsed -lt $HEALTH_CHECK_TIMEOUT ]; do
            sleep $HEALTH_CHECK_INTERVAL
            time_elapsed=$((time_elapsed + HEALTH_CHECK_INTERVAL))
            
            if test_server_health "$SERVER_PORT"; then
                server_ready=true
                break
            fi
            
            echo -e "${GRAY}等待服务器响应... ($time_elapsed/$HEALTH_CHECK_TIMEOUT 秒)${NC}"
        done
        
        if [ "$server_ready" = false ]; then
            echo -e "${RED}错误: 后端服务器在 $HEALTH_CHECK_TIMEOUT 秒内未响应${NC}"
            cleanup
            exit 1
        fi
        
        echo -e "${GREEN}后端服务器已就绪!${NC}"
    fi
    
    # 步骤2: 启动前端开发服务器
    echo ""
    echo -e "${CYAN}检查前端开发服务器状态...${NC}"
    
    # 简单检查前端端口是否被占用 (不严谨，但可作为参考)
    if curl -s --max-time 3 "http://localhost:$FRONTEND_PORT" > /dev/null 2>&1; then
        echo -e "${GREEN}前端开发服务器可能已在运行 (端口: $FRONTEND_PORT)${NC}"
    else
        # 端口未响应，启动前端服务器
        if ! start_frontend_server "$FRONTEND_DIR" "$FRONTEND_PORT"; then
            echo -e "${RED}前端开发服务器启动失败，退出脚本${NC}"
            cleanup
            exit 1
        fi
        
        echo -e "${CYAN}等待前端服务器启动...${NC}"
        sleep 5  # 给Vite一些启动时间
    fi
    
    # 步骤3: 在浏览器中打开应用
    echo ""
    open_in_browser "$FRONTEND_PORT"
    
    echo ""
    echo -e "${CYAN}================================================${NC}"
    echo -e "${GREEN}Leaf 地图服务系统已成功启动!${NC}"
    echo -e "${GRAY}后端API: http://localhost:$SERVER_PORT${NC}"
    echo -e "${GRAY}前端应用: http://localhost:$FRONTEND_PORT${NC}"
    echo -e "${YELLOW}按 Ctrl+C 停止所有服务${NC}"
    echo -e "${CYAN}================================================${NC}"
    
    # 保持脚本运行，直到用户中断
    while true; do
        sleep 1
    done
}

# 执行主函数
main "$@"