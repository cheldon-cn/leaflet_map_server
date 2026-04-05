#include "ogc/navi/positioning/position_provider.h"
#include "ogc/navi/positioning/nmea_parser.h"
#include <thread>
#include <atomic>
#include <mutex>

namespace ogc {
namespace navi {

class SerialPositionProvider : public IPositionProvider {
public:
    SerialPositionProvider() 
        : connected_(false)
        , running_(false)
    {}
    
    ~SerialPositionProvider() override {
        Shutdown();
    }
    
    bool Initialize(const ProviderConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) return false;
        
        config_ = config;
        connected_ = true;
        running_ = true;
        return true;
    }
    
    void Shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        connected_ = false;
    }
    
    bool IsConnected() const override {
        return connected_;
    }
    
    PositionData GetLastPosition() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return last_position_;
    }
    
    void SetPositionCallback(PositionCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        position_callback_ = callback;
    }
    
    void SetErrorCallback(ErrorCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        error_callback_ = callback;
    }
    
private:
    ProviderConfig config_;
    PositionData last_position_;
    PositionCallback position_callback_;
    ErrorCallback error_callback_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    mutable std::mutex mutex_;
};

class FilePositionProvider : public IPositionProvider {
public:
    FilePositionProvider() 
        : connected_(false)
        , running_(false)
    {}
    
    ~FilePositionProvider() override {
        Shutdown();
    }
    
    bool Initialize(const ProviderConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) return false;
        
        config_ = config;
        connected_ = true;
        running_ = true;
        return true;
    }
    
    void Shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        connected_ = false;
    }
    
    bool IsConnected() const override {
        return connected_;
    }
    
    PositionData GetLastPosition() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return last_position_;
    }
    
    void SetPositionCallback(PositionCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        position_callback_ = callback;
    }
    
    void SetErrorCallback(ErrorCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        error_callback_ = callback;
    }
    
private:
    ProviderConfig config_;
    PositionData last_position_;
    PositionCallback position_callback_;
    ErrorCallback error_callback_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    mutable std::mutex mutex_;
};

class SystemPositionProvider : public IPositionProvider {
public:
    SystemPositionProvider() 
        : connected_(false)
        , running_(false)
    {}
    
    ~SystemPositionProvider() override {
        Shutdown();
    }
    
    bool Initialize(const ProviderConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) return false;
        
        config_ = config;
        connected_ = true;
        running_ = true;
        return true;
    }
    
    void Shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        connected_ = false;
    }
    
    bool IsConnected() const override {
        return connected_;
    }
    
    PositionData GetLastPosition() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return last_position_;
    }
    
    void SetPositionCallback(PositionCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        position_callback_ = callback;
    }
    
    void SetErrorCallback(ErrorCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        error_callback_ = callback;
    }
    
private:
    ProviderConfig config_;
    PositionData last_position_;
    PositionCallback position_callback_;
    ErrorCallback error_callback_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    mutable std::mutex mutex_;
};

IPositionProvider* IPositionProvider::Create(PositionSource source) {
    switch (source) {
        case PositionSource::Serial:
            return new SerialPositionProvider();
        case PositionSource::File:
            return new FilePositionProvider();
        case PositionSource::System:
            return new SystemPositionProvider();
        default:
            return new SerialPositionProvider();
    }
}

}
}
