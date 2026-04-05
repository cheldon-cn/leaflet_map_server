#include "ogc/navi/positioning/position_manager.h"
#include "ogc/navi/positioning/nmea_parser.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <algorithm>

namespace ogc {
namespace navi {

PositionManager::PositionManager()
    : provider_(nullptr)
    , filter_()
    , current_position_()
    , last_valid_position_()
    , position_callback_(nullptr)
    , error_callback_(nullptr)
    , running_(false)
{
}

PositionManager::~PositionManager() {
    Shutdown();
}

PositionManager& PositionManager::Instance() {
    static PositionManager instance;
    return instance;
}

bool PositionManager::Initialize(PositionSource source, const ProviderConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (running_) {
        Shutdown();
    }
    
    provider_.reset(IPositionProvider::Create(source));
    if (!provider_) {
        if (error_callback_) {
            error_callback_("Failed to create position provider");
        }
        return false;
    }
    
    provider_->SetPositionCallback([this](const PositionData& data) {
        OnPositionReceived(data);
    });
    
    provider_->SetErrorCallback([this](const std::string& error) {
        OnErrorReceived(error);
    });
    
    if (!provider_->Initialize(config)) {
        if (error_callback_) {
            error_callback_("Failed to initialize position provider");
        }
        provider_.reset();
        return false;
    }
    
    running_ = true;
    return true;
}

void PositionManager::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    if (provider_) {
        provider_->Shutdown();
        provider_.reset();
    }
    
    current_position_ = PositionData();
    last_valid_position_ = PositionData();
}

bool PositionManager::IsRunning() const {
    return running_;
}

PositionData PositionManager::GetCurrentPosition() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return current_position_;
}

PositionData PositionManager::GetLastValidPosition() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return last_valid_position_;
}

void PositionManager::SetPositionCallback(PositionCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    position_callback_ = callback;
}

void PositionManager::SetErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    error_callback_ = callback;
}

void PositionManager::SetFilterConfig(double max_speed, double max_acceleration,
                                      int window_size, double max_hdop, int min_satellites) {
    std::lock_guard<std::mutex> lock(mutex_);
    filter_.SetMaxSpeed(max_speed);
    filter_.SetMaxAcceleration(max_acceleration);
    filter_.SetSmoothWindowSize(window_size);
    filter_.SetMaxHdop(max_hdop);
    filter_.SetMinSatellites(min_satellites);
}

FilterStatistics PositionManager::GetFilterStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return filter_.GetStatistics();
}

void PositionManager::ResetFilter() {
    std::lock_guard<std::mutex> lock(mutex_);
    filter_.Reset();
}

void PositionManager::OnPositionReceived(const PositionData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PositionData filtered_data = filter_.Filter(data);
    current_position_ = filtered_data;
    
    if (filtered_data.valid) {
        last_valid_position_ = filtered_data;
    }
    
    if (position_callback_) {
        position_callback_(current_position_);
    }
}

void PositionManager::OnErrorReceived(const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (error_callback_) {
        error_callback_(error);
    }
}

}
}
