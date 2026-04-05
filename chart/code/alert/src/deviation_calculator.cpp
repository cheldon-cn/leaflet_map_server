#include "ogc/alert/deviation_calculator.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

namespace {
const double kEarthRadiusM = 6371000.0;

double ToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double ToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}
}

DeviationCalculator::DeviationCalculator() {
}

DeviationCalculator::~DeviationCalculator() {
}

DeviationResult DeviationCalculator::Calculate(const Coordinate& position,
                                                const std::vector<Coordinate>& route) {
    DeviationResult result;
    result.deviation_distance = 0.0;
    result.cross_track_distance = 0.0;
    result.along_track_distance = 0.0;
    result.nearest_segment_index = 0;
    result.bearing_to_nearest = 0.0;
    result.distance_to_end = 0.0;
    result.is_left_of_route = false;
    
    if (route.empty()) {
        return result;
    }
    
    if (route.size() == 1) {
        result.deviation_distance = HaversineDistance(position, route[0]);
        result.nearest_point = route[0];
        result.bearing_to_nearest = CalculateBearing(position, route[0]);
        return result;
    }
    
    double min_distance = std::numeric_limits<double>::max();
    int nearest_index = 0;
    Coordinate nearest_point;
    double total_along_track = 0.0;
    
    for (size_t i = 0; i < route.size() - 1; ++i) {
        double segment_distance = 0.0;
        Coordinate projected = ProjectPointOnSegment(position, route[i], route[i + 1], 
                                                      segment_distance);
        
        if (segment_distance < min_distance) {
            min_distance = segment_distance;
            nearest_index = static_cast<int>(i);
            nearest_point = projected;
        }
    }
    
    result.deviation_distance = min_distance;
    result.nearest_point = nearest_point;
    result.nearest_segment_index = nearest_index;
    result.bearing_to_nearest = CalculateBearing(position, nearest_point);
    
    result.cross_track_distance = CalculateCrossTrack(position, 
                                                       route[nearest_index], 
                                                       route[nearest_index + 1]);
    
    result.is_left_of_route = CrossTrackCalculator::CalculateSigned(position,
                                                                      route[nearest_index],
                                                                      route[nearest_index + 1]) > 0;
    
    for (int i = 0; i < nearest_index; ++i) {
        total_along_track += HaversineDistance(route[i], route[i + 1]);
    }
    
    total_along_track += CalculateAlongTrack(position, route[nearest_index], 
                                              route[nearest_index + 1]);
    result.along_track_distance = total_along_track;
    
    for (size_t i = nearest_index + 1; i < route.size(); ++i) {
        result.distance_to_end += HaversineDistance(route[i - 1], route[i]);
    }
    result.distance_to_end -= CalculateAlongTrack(position, route[nearest_index], 
                                                   route[nearest_index + 1]);
    
    return result;
}

double DeviationCalculator::CalculateCrossTrack(const Coordinate& position,
                                                 const Coordinate& line_start,
                                                 const Coordinate& line_end) {
    return std::abs(CrossTrackCalculator::CalculateSigned(position, line_start, line_end));
}

double DeviationCalculator::CalculateAlongTrack(const Coordinate& position,
                                                 const Coordinate& line_start,
                                                 const Coordinate& line_end) {
    double d13 = HaversineDistance(line_start, position) / kEarthRadiusM;
    double brng13 = ToRadians(CalculateBearing(line_start, position));
    double brng12 = ToRadians(CalculateBearing(line_start, line_end));
    
    double dxt = std::asin(std::sin(d13) * std::sin(brng13 - brng12));
    
    double dat = std::acos(std::cos(d13) / std::cos(dxt));
    
    return dat * kEarthRadiusM;
}

Coordinate DeviationCalculator::FindNearestPoint(const Coordinate& position,
                                                  const std::vector<Coordinate>& route) {
    DeviationResult result = Calculate(position, route);
    return result.nearest_point;
}

double DeviationCalculator::HaversineDistance(const Coordinate& from, const Coordinate& to) const {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLat = ToRadians(to.latitude - from.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return kEarthRadiusM * c;
}

double DeviationCalculator::CalculateBearing(const Coordinate& from, const Coordinate& to) const {
    double lat1 = ToRadians(from.latitude);
    double lat2 = ToRadians(to.latitude);
    double dLon = ToRadians(to.longitude - from.longitude);
    
    double x = std::sin(dLon) * std::cos(lat2);
    double y = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
    
    double bearing = ToDegrees(std::atan2(x, y));
    return NormalizeAngle(bearing);
}

Coordinate DeviationCalculator::CalculateDestination(const Coordinate& start, 
                                                      double bearing, double distance) const {
    double brng = ToRadians(bearing);
    double d = distance / kEarthRadiusM;
    double lat1 = ToRadians(start.latitude);
    double lon1 = ToRadians(start.longitude);
    
    double lat2 = std::asin(std::sin(lat1) * std::cos(d) +
                            std::cos(lat1) * std::sin(d) * std::cos(brng));
    double lon2 = lon1 + std::atan2(std::sin(brng) * std::sin(d) * std::cos(lat1),
                                     std::cos(d) - std::sin(lat1) * std::sin(lat2));
    
    return Coordinate(ToDegrees(lon2), ToDegrees(lat2));
}

double DeviationCalculator::NormalizeAngle(double angle) const {
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}

RouteSegment DeviationCalculator::CreateSegment(const Coordinate& start, const Coordinate& end) const {
    RouteSegment segment;
    segment.start = start;
    segment.end = end;
    segment.length = HaversineDistance(start, end);
    segment.bearing = CalculateBearing(start, end);
    return segment;
}

Coordinate DeviationCalculator::ProjectPointOnSegment(const Coordinate& point,
                                                       const Coordinate& line_start,
                                                       const Coordinate& line_end,
                                                       double& distance) const {
    double d13 = HaversineDistance(line_start, point) / kEarthRadiusM;
    double brng13 = ToRadians(CalculateBearing(line_start, point));
    double brng12 = ToRadians(CalculateBearing(line_start, line_end));
    double d12 = HaversineDistance(line_start, line_end) / kEarthRadiusM;
    
    double dxt = std::asin(std::sin(d13) * std::sin(brng13 - brng12));
    
    double dat = std::acos(std::cos(d13) / std::cos(dxt));
    
    if (dat < 0 || dat > d12) {
        double dist_to_start = HaversineDistance(point, line_start);
        double dist_to_end = HaversineDistance(point, line_end);
        
        if (dist_to_start < dist_to_end) {
            distance = dist_to_start;
            return line_start;
        } else {
            distance = dist_to_end;
            return line_end;
        }
    }
    
    distance = std::abs(dxt * kEarthRadiusM);
    
    return CalculateDestination(line_start, ToDegrees(brng12), dat * kEarthRadiusM);
}

std::unique_ptr<IDeviationCalculator> IDeviationCalculator::Create() {
    return std::unique_ptr<IDeviationCalculator>(new DeviationCalculator());
}

double RouteAnalyzer::CalculateTotalLength(const std::vector<Coordinate>& route) {
    double total = 0.0;
    for (size_t i = 1; i < route.size(); ++i) {
        double lat1 = ToRadians(route[i - 1].latitude);
        double lat2 = ToRadians(route[i].latitude);
        double dLat = ToRadians(route[i].latitude - route[i - 1].latitude);
        double dLon = ToRadians(route[i].longitude - route[i - 1].longitude);
        
        double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                   std::cos(lat1) * std::cos(lat2) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        
        total += kEarthRadiusM * c;
    }
    return total;
}

std::vector<RouteSegment> RouteAnalyzer::CreateSegments(const std::vector<Coordinate>& route) {
    std::vector<RouteSegment> segments;
    segments.reserve(route.size() - 1);
    
    for (size_t i = 0; i < route.size() - 1; ++i) {
        RouteSegment segment;
        segment.start = route[i];
        segment.end = route[i + 1];
        
        double lat1 = ToRadians(route[i].latitude);
        double lat2 = ToRadians(route[i + 1].latitude);
        double dLat = ToRadians(route[i + 1].latitude - route[i].latitude);
        double dLon = ToRadians(route[i + 1].longitude - route[i].longitude);
        
        double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                   std::cos(lat1) * std::cos(lat2) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        segment.length = kEarthRadiusM * c;
        
        double x = std::sin(dLon) * std::cos(lat2);
        double y = std::cos(lat1) * std::sin(lat2) -
                   std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
        segment.bearing = ToDegrees(std::atan2(x, y));
        if (segment.bearing < 0) segment.bearing += 360;
        
        segments.push_back(segment);
    }
    
    return segments;
}

int RouteAnalyzer::FindNearestSegment(const Coordinate& position,
                                       const std::vector<RouteSegment>& segments) {
    int nearest = 0;
    double min_distance = std::numeric_limits<double>::max();
    
    for (size_t i = 0; i < segments.size(); ++i) {
        double lat1 = ToRadians(segments[i].start.latitude);
        double lat2 = ToRadians(position.latitude);
        double dLat = ToRadians(position.latitude - segments[i].start.latitude);
        double dLon = ToRadians(position.longitude - segments[i].start.longitude);
        
        double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                   std::cos(lat1) * std::cos(lat2) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        double distance = kEarthRadiusM * c;
        
        if (distance < min_distance) {
            min_distance = distance;
            nearest = static_cast<int>(i);
        }
    }
    
    return nearest;
}

double RouteAnalyzer::CalculateProgress(const Coordinate& position,
                                         const std::vector<Coordinate>& route) {
    if (route.empty()) return 0.0;
    
    double total_length = CalculateTotalLength(route);
    if (total_length < 0.001) return 0.0;
    
    DeviationCalculator calc;
    DeviationResult result = calc.Calculate(position, route);
    
    return result.along_track_distance / total_length;
}

double CrossTrackCalculator::Calculate(const Coordinate& position,
                                        const Coordinate& line_start,
                                        const Coordinate& line_end) {
    return std::abs(CalculateSigned(position, line_start, line_end));
}

double CrossTrackCalculator::CalculateSigned(const Coordinate& position,
                                              const Coordinate& line_start,
                                              const Coordinate& line_end) {
    double lat1 = ToRadians(line_start.latitude);
    double lat2 = ToRadians(line_end.latitude);
    double lat3 = ToRadians(position.latitude);
    double dLon13 = ToRadians(position.longitude - line_start.longitude);
    double dLon12 = ToRadians(line_end.longitude - line_start.longitude);
    
    double brng13 = std::atan2(std::sin(dLon13) * std::cos(lat3),
                                std::cos(lat1) * std::sin(lat3) -
                                std::sin(lat1) * std::cos(lat3) * std::cos(dLon13));
    
    double brng12 = std::atan2(std::sin(dLon12) * std::cos(lat2),
                                std::cos(lat1) * std::sin(lat2) -
                                std::sin(lat1) * std::cos(lat2) * std::cos(dLon12));
    
    double dLat13 = ToRadians(position.latitude - line_start.latitude);
    double a13 = std::sin(dLat13 / 2) * std::sin(dLat13 / 2) +
                 std::cos(lat1) * std::cos(lat3) *
                 std::sin(dLon13 / 2) * std::sin(dLon13 / 2);
    double d13 = 2 * std::atan2(std::sqrt(a13), std::sqrt(1 - a13));
    
    double dxt = std::asin(std::sin(d13) * std::sin(brng13 - brng12));
    
    return dxt * kEarthRadiusM;
}

}
}
