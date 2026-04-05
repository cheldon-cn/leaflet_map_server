#ifndef OGC_ALERT_DATA_SOURCE_MANAGER_H
#define OGC_ALERT_DATA_SOURCE_MANAGER_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

enum class DataSourceType : uint8_t {
    kUnknown = 0,
    kAis = 1,
    kDepth = 2,
    kTide = 3,
    kWeather = 4,
    kNotice = 5,
    kRoute = 6,
    kSpeedLimit = 7,
    kRestrictedArea = 8
};

struct DataSourceConfig {
    std::string source_id;
    DataSourceType type;
    std::string connection_string;
    int refresh_interval_ms;
    bool enabled;
};

struct AisData {
    std::string ship_id;
    std::string ship_name;
    Coordinate position;
    double speed;
    double heading;
    DateTime timestamp;
};

struct DepthData {
    Coordinate position;
    double depth;
    double chart_depth;
    std::string chart_id;
    DateTime timestamp;
};

struct TideData {
    std::string station_id;
    double tide_height;
    DateTime timestamp;
    DateTime next_high_tide;
    DateTime next_low_tide;
};

struct WeatherData {
    Coordinate position;
    double wind_speed;
    double wind_direction;
    double wave_height;
    double visibility;
    std::string weather_condition;
    DateTime timestamp;
};

struct NoticeData {
    std::string notice_id;
    std::string title;
    std::string content;
    std::string affected_area;
    DateTime start_time;
    DateTime end_time;
    std::string notice_type;
};

struct RouteData {
    std::string route_id;
    std::string route_name;
    std::vector<Coordinate> waypoints;
    double max_deviation;
};

struct SpeedLimitData {
    std::string zone_id;
    std::string zone_name;
    double speed_limit;
    std::string geometry;
};

struct RestrictedAreaData {
    std::string area_id;
    std::string area_name;
    std::string restriction_type;
    std::string geometry;
    DateTime start_time;
    DateTime end_time;
};

class OGC_ALERT_API IDataSource {
public:
    virtual ~IDataSource() = default;
    
    virtual std::string GetSourceId() const = 0;
    virtual DataSourceType GetType() const = 0;
    virtual bool IsConnected() const = 0;
    virtual bool Connect() = 0;
    virtual void Disconnect() = 0;
    virtual bool Refresh() = 0;
};

using IDataSourcePtr = std::shared_ptr<IDataSource>;

class OGC_ALERT_API IDataSourceManager {
public:
    virtual ~IDataSourceManager() = default;
    
    virtual void RegisterSource(IDataSourcePtr source) = 0;
    virtual void UnregisterSource(const std::string& source_id) = 0;
    virtual IDataSourcePtr GetSource(const std::string& source_id) const = 0;
    virtual IDataSourcePtr GetSource(DataSourceType type) const = 0;
    virtual std::vector<IDataSourcePtr> GetAllSources() const = 0;
    
    virtual bool ConnectAll() = 0;
    virtual void DisconnectAll() = 0;
    virtual bool RefreshAll() = 0;
    
    virtual void SetRefreshInterval(int interval_ms) = 0;
    virtual void StartAutoRefresh() = 0;
    virtual void StopAutoRefresh() = 0;
    
    static std::unique_ptr<IDataSourceManager> Create();
};

class OGC_ALERT_API DataSourceManager : public IDataSourceManager {
public:
    DataSourceManager();
    ~DataSourceManager();
    
    void RegisterSource(IDataSourcePtr source) override;
    void UnregisterSource(const std::string& source_id) override;
    IDataSourcePtr GetSource(const std::string& source_id) const override;
    IDataSourcePtr GetSource(DataSourceType type) const override;
    std::vector<IDataSourcePtr> GetAllSources() const override;
    
    bool ConnectAll() override;
    void DisconnectAll() override;
    bool RefreshAll() override;
    
    void SetRefreshInterval(int interval_ms) override;
    void StartAutoRefresh() override;
    void StopAutoRefresh() override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

inline const char* GetDataSourceTypeName(DataSourceType type) noexcept {
    switch (type) {
        case DataSourceType::kAis: return "AIS";
        case DataSourceType::kDepth: return "Depth";
        case DataSourceType::kTide: return "Tide";
        case DataSourceType::kWeather: return "Weather";
        case DataSourceType::kNotice: return "Notice";
        case DataSourceType::kRoute: return "Route";
        case DataSourceType::kSpeedLimit: return "SpeedLimit";
        case DataSourceType::kRestrictedArea: return "RestrictedArea";
        default: return "Unknown";
    }
}

}
}

#endif
