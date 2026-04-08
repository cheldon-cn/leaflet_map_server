#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <deque>

namespace ogc {
namespace navi {

struct FilterStatistics {
    int total_received;
    int valid_count;
    int outlier_count;
    int extrapolated_count;
    double outlier_rate;
    
    FilterStatistics()
        : total_received(0)
        , valid_count(0)
        , outlier_count(0)
        , extrapolated_count(0)
        , outlier_rate(0.0)
    {}
};

class OGC_NAVI_API PositionFilter {
public:
    PositionFilter();
    
    void SetMaxSpeed(double max_speed_knots);
    void SetMaxAcceleration(double max_acceleration);
    void SetSmoothWindowSize(int window_size);
    void SetMaxHdop(double max_hdop);
    void SetMinSatellites(int min_count);
    
    PositionData Filter(const PositionData& raw_data);
    void Reset();
    
    FilterStatistics GetStatistics() const;
    
private:
    bool IsValid(const PositionData& data);
    bool IsOutlier(const PositionData& data);
    PositionData Smooth(const PositionData& data);
    PositionData Extrapolate(double timestamp);
    
    std::deque<PositionData> history_;
    double max_speed_;
    double max_acceleration_;
    int smooth_window_size_;
    double max_hdop_;
    int min_satellites_;
    PositionData last_valid_;
    FilterStatistics stats_;
};

}
}
