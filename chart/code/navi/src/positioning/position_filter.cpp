#include "ogc/navi/positioning/position_filter.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace navi {

namespace {
    const double PI = 3.14159265358979323846;
    const double KNOTS_TO_METERS_PER_SECOND = 0.514444;
}

PositionFilter::PositionFilter()
    : max_speed_(30.0)
    , max_acceleration_(10.0)
    , smooth_window_size_(5)
    , max_hdop_(5.0)
    , min_satellites_(4)
    , last_valid_()
    , stats_()
{
}

void PositionFilter::SetMaxSpeed(double max_speed_knots) {
    max_speed_ = max_speed_knots;
}

void PositionFilter::SetMaxAcceleration(double max_acceleration) {
    max_acceleration_ = max_acceleration;
}

void PositionFilter::SetSmoothWindowSize(int window_size) {
    smooth_window_size_ = std::max(1, window_size);
}

void PositionFilter::SetMaxHdop(double max_hdop) {
    max_hdop_ = max_hdop;
}

void PositionFilter::SetMinSatellites(int min_count) {
    min_satellites_ = min_count;
}

PositionData PositionFilter::Filter(const PositionData& raw_data) {
    stats_.total_received++;
    
    if (!IsValid(raw_data)) {
        stats_.extrapolated_count++;
        return Extrapolate(raw_data.timestamp);
    }
    
    if (IsOutlier(raw_data)) {
        stats_.outlier_count++;
        return Extrapolate(raw_data.timestamp);
    }
    
    PositionData smoothed = Smooth(raw_data);
    
    history_.push_back(smoothed);
    if (static_cast<int>(history_.size()) > smooth_window_size_) {
        history_.pop_front();
    }
    
    last_valid_ = smoothed;
    stats_.valid_count++;
    stats_.outlier_rate = 
        static_cast<double>(stats_.outlier_count) / stats_.total_received;
    
    return smoothed;
}

void PositionFilter::Reset() {
    history_.clear();
    last_valid_ = PositionData();
    stats_ = FilterStatistics();
}

FilterStatistics PositionFilter::GetStatistics() const {
    return stats_;
}

bool PositionFilter::IsValid(const PositionData& data) {
    if (data.longitude < -180.0 || data.longitude > 180.0) {
        return false;
    }
    
    if (data.latitude < -90.0 || data.latitude > 90.0) {
        return false;
    }
    
    if (data.hdop > max_hdop_) {
        return false;
    }
    
    if (data.satellite_count < min_satellites_) {
        return false;
    }
    
    if (data.quality == PositionQuality::Invalid) {
        return false;
    }
    
    return true;
}

bool PositionFilter::IsOutlier(const PositionData& data) {
    if (history_.empty()) {
        return false;
    }
    
    double distance = CoordinateConverter::Instance().CalculateGreatCircleDistance(
        last_valid_.latitude, last_valid_.longitude,
        data.latitude, data.longitude);
    
    double time_diff = data.timestamp - last_valid_.timestamp;
    
    if (time_diff <= 0) {
        return false;
    }
    
    double max_distance = max_speed_ * KNOTS_TO_METERS_PER_SECOND * time_diff;
    
    if (distance > max_distance * 3.0) {
        return true;
    }
    
    double speed_change = std::abs(data.speed - last_valid_.speed);
    double acceleration = speed_change * KNOTS_TO_METERS_PER_SECOND / time_diff;
    
    if (acceleration > max_acceleration_) {
        return true;
    }
    
    return false;
}

PositionData PositionFilter::Smooth(const PositionData& data) {
    if (history_.size() < 2) {
        return data;
    }
    
    PositionData smoothed = data;
    
    double weight_sum = 0.0;
    double lon_sum = 0.0;
    double lat_sum = 0.0;
    double speed_sum = 0.0;
    double heading_sum_x = 0.0;
    double heading_sum_y = 0.0;
    
    for (size_t i = 0; i < history_.size(); ++i) {
        double weight = static_cast<double>(i + 1);
        
        lon_sum += history_[i].longitude * weight;
        lat_sum += history_[i].latitude * weight;
        speed_sum += history_[i].speed * weight;
        
        double heading_rad = history_[i].heading * PI / 180.0;
        heading_sum_x += std::sin(heading_rad) * weight;
        heading_sum_y += std::cos(heading_rad) * weight;
        
        weight_sum += weight;
    }
    
    double current_weight = static_cast<double>(history_.size() + 1);
    lon_sum += data.longitude * current_weight;
    lat_sum += data.latitude * current_weight;
    speed_sum += data.speed * current_weight;
    
    double heading_rad = data.heading * PI / 180.0;
    heading_sum_x += std::sin(heading_rad) * current_weight;
    heading_sum_y += std::cos(heading_rad) * current_weight;
    
    weight_sum += current_weight;
    
    smoothed.longitude = lon_sum / weight_sum;
    smoothed.latitude = lat_sum / weight_sum;
    smoothed.speed = speed_sum / weight_sum;
    
    double smoothed_heading = std::atan2(heading_sum_x, heading_sum_y) * 180.0 / PI;
    if (smoothed_heading < 0) {
        smoothed_heading += 360.0;
    }
    smoothed.heading = smoothed_heading;
    
    return smoothed;
}

PositionData PositionFilter::Extrapolate(double timestamp) {
    if (history_.size() < 2) {
        return last_valid_;
    }
    
    PositionData extrapolated = last_valid_;
    double time_diff = timestamp - last_valid_.timestamp;
    
    if (time_diff > 0 && time_diff < 300.0) {
        double speed_ms = last_valid_.speed * KNOTS_TO_METERS_PER_SECOND;
        double heading_rad = last_valid_.heading * PI / 180.0;
        
        double distance = speed_ms * time_diff;
        double lat_change = distance * std::cos(heading_rad) / 111320.0;
        double lon_change = distance * std::sin(heading_rad) / 
                           (111320.0 * std::cos(last_valid_.latitude * PI / 180.0));
        
        extrapolated.latitude += lat_change;
        extrapolated.longitude += lon_change;
        extrapolated.timestamp = timestamp;
    }
    
    return extrapolated;
}

}
}
