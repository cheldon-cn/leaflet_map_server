#include "HttpServer.h"
#include "Config.h"
#include <iostream>
#include <string>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>

using namespace cycle;

std::atomic<bool> g_bRunning{true};

void SignalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    g_bRunning = false;
}

void PrintHelp(const char* programName) {
    std::cout << "Map Server - C++11 Implementation" << std::endl;
    std::cout << "Renders spatial data as PNG images via HTTP API" << std::endl << std::endl;
    
    std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                 Show this help message and exit" << std::endl;
    std::cout << "  -c, --config FILE          Load configuration from JSON file" << std::endl;
    std::cout << "  -p, --port PORT            HTTP server port (default: 8080)" << std::endl;
    std::cout << "  -o, --output DIR           Output directory for generated files (default: ./leaf/output)" << std::endl;
    std::cout << "  -d, --database PATH        Path to spatial database (default: ./spatial_data.db)" << std::endl;
    std::cout << "  -v, --verbose              Enable verbose logging" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --port 8080" << std::endl;
    std::cout << "  " << programName << " --config config.json" << std::endl;
    std::cout << "  " << programName << " --port 9090 --output ./maps" << std::endl;
}

bool ParseCommandLine(int argc, char* argv[], ServerConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintHelp(argv[0]);
            return false;
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                std::string configFile = argv[++i];
                if (!LoadConfigFromFile(configFile, config)) {
                    std::cerr << "Error: Failed to load configuration from " << configFile << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: --config requires a file path" << std::endl;
                return false;
            }
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                try {
                    config.m_nPort = static_cast<uint16_t>(std::stoi(argv[++i]));
                } catch (...) {
                    std::cerr << "Error: Invalid port number" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: --port requires a port number" << std::endl;
                return false;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                config.m_strOutputDir = argv[++i];
            } else {
                std::cerr << "Error: --output requires a directory path" << std::endl;
                return false;
            }
        } else if (arg == "-d" || arg == "--database") {
            if (i + 1 < argc) {
                config.m_strDatabasePath = argv[++i];
            } else {
                std::cerr << "Error: --database requires a database path" << std::endl;
                return false;
            }
        } else if (arg == "-v" || arg == "--verbose") {
            config.m_nLogLevel = 2;
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    // 设置信号处理
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
#ifdef _WIN32
    std::signal(SIGBREAK, SignalHandler);
#endif
    
    // 加载配置
    ServerConfig config = GetDefaultConfig();
    
    if (!ParseCommandLine(argc, argv, config)) {
        return 1;
    }
    
    // 输出配置信息
    std::cout << "=== Map Server Configuration ===" << std::endl;
    std::cout << "Host: " << config.m_strHost << std::endl;
    std::cout << "Port: " << config.m_nPort << std::endl;
    std::cout << "Output Directory: " << config.m_strOutputDir << std::endl;
    std::cout << "Database: " << config.m_strDatabasePath << std::endl;
    std::cout << "Max Image Size: " << config.m_nMaxImageWidth << "x" << config.m_nMaxImageHeight << std::endl;
    std::cout << "Worker Threads: " << config.m_nWorkerThreads << std::endl;
    std::cout << "===============================" << std::endl;
    
    // 创建HTTP服务器
    HttpServer server(config);
    
    // 启动服务器
    if (!server.Start()) {
        std::cerr << "Failed to start server: " << server.GetError() << std::endl;
        return 1;
    }
    
    std::cout << "Server started successfully. Press Ctrl+C to stop." << std::endl;
    std::cout << "Available endpoints:" << std::endl;
    std::cout << "  GET  /health          - Health check" << std::endl;
    std::cout << "  GET  /layers          - List available layers" << std::endl;
    std::cout << "  POST /generate        - Generate map image (JSON body)" << std::endl;
    std::cout << "  GET  /tile/{z}/{x}/{y}.png - Get map tile" << std::endl;
    std::cout << std::endl;
    
    // 主循环
    while (g_bRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // 检查服务器状态
        if (!server.IsRunning()) {
            std::cerr << "Server stopped unexpectedly" << std::endl;
            break;
        }
    }
    
    // 停止服务器
    server.Stop();
    
    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}