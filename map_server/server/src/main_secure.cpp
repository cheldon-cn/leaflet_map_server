#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <thread>
#include "config/config.h"
#include "utils/logger.h"
#include "utils/file_system.h"
#include "database/database_factory.h"
#include "encoder/encoder_factory.h"
#include "cache/memory_cache.h"
#include "renderer/renderer.h"
#include "service/map_service.h"
#include "server/http_server.h"
#include "auth/jwt_auth.h"

namespace cycle {

class MapServerApp {
private:
    bool LoadConfig(const std::string& configPath) {
        if (!config_.LoadFromFile(configPath)) {
            std::cerr << "Failed to load config from: " << configPath << std::endl;
            return false;
        }
        
        if (!config_.Validate()) {
            std::cerr << "Configuration validation failed" << std::endl;
            return false;
        }
        
        LOG_INFO("Configuration loaded successfully from: " + configPath);
        return true;
    }
    
    bool CreateDirectories() {
        try {
            cycle::utils::create_directories("./logs");
            cycle::utils::create_directories("./data");
            cycle::utils::create_directories("./cache");
            cycle::utils::create_directories("./ssl");
            
            LOG_INFO("Required directories created");
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to create directories: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool InitializeLogger() {
        Logger::Init(config_.log);
        
        LOG_INFO("Logger initialized");
        LOG_INFO("Log level: " + std::to_string(config_.log.level));
        LOG_INFO("Log file: " + config_.log.file);
        
        return true;
    }
    
    bool InitializeComponents() {
        LOG_INFO("Initializing database connection");
        database_ = database::DatabaseFactory::Create(config_.database.type, config_.database.sqlite_path);

        if (!database_ || !database_->Open()) {
            LOG_ERROR("Failed to initialize database");
            return false;
        }
        
        LOG_INFO("Initializing encoder");
        encoder_ = encoder::EncoderFactory::Create(config_.encoder.format);
        if (!encoder_ ) {
            LOG_ERROR("Failed to initialize encoder");
            return false;
        }
        
        LOG_INFO("Initializing cache");
        cache_ = std::make_shared<cache::MemoryCache>(config_.cache.memory_cache_size);
        
        LOG_INFO("Initializing renderer");
        renderer_ = std::make_shared<renderer::Renderer>(database_, encoder_, cache_);
        
        LOG_INFO("Initializing map service");
        mapService_ = std::make_shared<service::MapService>(renderer_, cache_, config_);
        
        LOG_INFO("Initializing HTTP server");
        httpServer_ = std::make_unique<server::HttpServer>(config_);
        
        if (!httpServer_) {
            LOG_ERROR("Failed to create HTTP server");
            return false;
        }
        
        httpServer_->SetRenderer(renderer_);
        httpServer_->SetMapService(mapService_);
        
        LOG_INFO("Initializing JWT auth");
        auth::JWTConfig config; 
        config.secret_key = config_.server.jwt_secret;
        auth_ = std::make_shared<auth::JWTAuth>(config);
        
        LOG_INFO("All components initialized successfully");
        return true;
    }
    
    bool SetupSignalHandlers() {
        std::signal(SIGINT, [](int signal) {
            (void)signal;
            LOG_INFO("Received SIGINT, shutting down...");
            std::exit(0);
        });
        
        std::signal(SIGTERM, [](int signal) {
            (void)signal;
            LOG_INFO("Received SIGTERM, shutting down...");
            std::exit(0);
        });
        
        LOG_INFO("Signal handlers configured");
        return true;
    }
    
public:
    MapServerApp() : shuttingDown_(false) {}
    
    bool Initialize(const std::string& configPath) {
        LOG_INFO("Initializing Secure Map Server Application");
        
        if (!LoadConfig(configPath)) {
            LOG_ERROR("Failed to load configuration");
            return false;
        }
        
        if (!CreateDirectories()) {
            LOG_ERROR("Failed to create required directories");
            return false;
        }
        
        if (!InitializeLogger()) {
            LOG_ERROR("Failed to initialize logger");
            return false;
        }
        
        if (!InitializeComponents()) {
            LOG_ERROR("Failed to initialize components");
            return false;
        }
        
        if (!SetupSignalHandlers()) {
            LOG_ERROR("Failed to setup signal handlers");
            return false;
        }
        
        LOG_INFO("Secure Map Server Application initialized successfully");
        return true;
    }
    
    bool Start() {
        if (!httpServer_) {
            LOG_ERROR("HTTP server not initialized");
            return false;
        }
        
        LOG_INFO("Starting Secure Map Server");
        
        if (config_.server.enable_https) {
            if (!httpServer_->EnableSSL(config_.server.ssl_cert_file, 
                                       config_.server.ssl_key_file, 
                                       config_.server.ssl_ca_file)) {
                LOG_WARN("Failed to enable HTTPS, continuing with HTTP only");
            } else {
                LOG_INFO("HTTPS enabled on port: " + std::to_string(config_.server.https_port));
            }
        }
        
        if (!httpServer_->Start()) {
            LOG_ERROR("Failed to start HTTP server");
            return false;
        }
        
        LOG_INFO("Secure Map Server started successfully");
        LOG_INFO("HTTP server listening on: " + config_.server.host + ":" + 
                 std::to_string(config_.server.port));
        
        if (config_.server.enable_https && httpServer_->IsSSLEnabled()) {
            LOG_INFO("HTTPS server listening on: " + config_.server.host + ":" + 
                     std::to_string(config_.server.https_port));
        }
        
        return true;
    }
    
    void Stop() {
        if (shuttingDown_) {
            return;
        }
        
        shuttingDown_ = true;
        
        LOG_INFO("Stopping Secure Map Server");
        
        if (httpServer_) {
            httpServer_->Stop();
        }
        
        if (database_) {
            database_->Close();
        }
        
        Logger::Shutdown();
        
        LOG_INFO("Secure Map Server stopped");
    }
    
    void WaitForShutdown() {
        LOG_INFO("Secure Map Server is running. Press Ctrl+C to stop.");
        
        while (!shuttingDown_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        Stop();
    }
    
private:
    Config config_;
    std::shared_ptr<database::IDatabase> database_;
    std::shared_ptr<encoder::IEncoder> encoder_;
    std::shared_ptr<cache::MemoryCache> cache_;
    std::shared_ptr<renderer::Renderer> renderer_;
    std::shared_ptr<service::MapService> mapService_;
    std::unique_ptr<server::HttpServer> httpServer_;
    std::shared_ptr<auth::JWTAuth> auth_;
    std::atomic<bool> shuttingDown_;
};

} // namespace cycle

int main(int argc, char* argv[]) {
    std::string configPath = "./config.json";
    
    if (argc > 1) {
        configPath = argv[1];
    }
    
    cycle::MapServerApp app;
    
    if (!app.Initialize(configPath)) {
        std::cerr << "Failed to initialize Map Server Application" << std::endl;
        return 1;
    }
    
    if (!app.Start()) {
        std::cerr << "Failed to start Map Server Application" << std::endl;
        return 1;
    }
    
    app.WaitForShutdown();
    
    return 0;
}
