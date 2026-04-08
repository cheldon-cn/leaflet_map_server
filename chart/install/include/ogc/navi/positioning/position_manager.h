#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/positioning/position_provider.h"
#include "ogc/navi/positioning/position_filter.h"
#include "ogc/navi/export.h"
#include <memory>
#include <mutex>

namespace ogc {
namespace navi {

class OGC_NAVI_API PositionManager {
public:
    static PositionManager& Instance();
    
    bool Initialize(PositionSource source, const ProviderConfig& config);
    void Shutdown();
    
    bool IsRunning() const;
    PositionData GetCurrentPosition() const;
    PositionData GetLastValidPosition() const;
    
    void SetPositionCallback(PositionCallback callback);
    void SetErrorCallback(ErrorCallback callback);
    
    void SetFilterConfig(double max_speed, double max_acceleration,
                         int window_size, double max_hdop, int min_satellites);
    FilterStatistics GetFilterStatistics() const;
    void ResetFilter();
    
private:
    PositionManager();
    ~PositionManager();
    PositionManager(const PositionManager&) = delete;
    PositionManager& operator=(const PositionManager&) = delete;
    
    void OnPositionReceived(const PositionData& data);
    void OnErrorReceived(const std::string& error);
    
    std::unique_ptr<IPositionProvider> provider_;
    PositionFilter filter_;
    PositionData current_position_;
    PositionData last_valid_position_;
    PositionCallback position_callback_;
    ErrorCallback error_callback_;
    mutable std::mutex mutex_;
    bool running_;
};

}
}
