#include "ogc/alert/speed_zone_matcher.h"
#include "ogc/geom/point.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

namespace {
const double kEarthRadiusKm = 6371.0;

double HaversineDistance(const Coordinate& from, const Coordinate& to) {
    double lat1 = from.latitude * M_PI / 180.0;
    double lat2 = to.latitude * M_PI / 180.0;
    double dLat = (to.latitude - from.latitude) * M_PI / 180.0;
    double dLon = (to.longitude - from.longitude) * M_PI / 180.0;
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return kEarthRadiusKm * c;
}

}

SpeedZoneMatcher::SpeedZoneMatcher() {
}

SpeedZoneMatcher::~SpeedZoneMatcher() {
}

void SpeedZoneMatcher::AddZone(const SpeedZone& zone) {
    m_zones[zone.zone_id] = zone;
}

void SpeedZoneMatcher::RemoveZone(const std::string& zone_id) {
    m_zones.erase(zone_id);
}

void SpeedZoneMatcher::UpdateZone(const SpeedZone& zone) {
    m_zones[zone.zone_id] = zone;
}

SpeedZone SpeedZoneMatcher::GetZone(const std::string& zone_id) const {
    auto it = m_zones.find(zone_id);
    if (it != m_zones.end()) {
        return it->second;
    }
    return SpeedZone();
}

std::vector<SpeedZoneMatch> SpeedZoneMatcher::Match(const Coordinate& position,
                                                     double speed,
                                                     const DateTime& time) {
    std::vector<SpeedZoneMatch> matches;
    
    for (const auto& pair : m_zones) {
        const SpeedZone& zone = pair.second;
        
        if (!IsZoneActive(zone, time)) {
            continue;
        }
        
        SpeedZoneMatch match;
        match.zone = zone;
        match.current_speed = speed;
        match.speed_limit = zone.max_speed;
        
        if (IsInsideZone(position, zone)) {
            match.is_inside = true;
            match.distance_to_zone = 0.0;
            match.is_violation = SpeedViolationChecker::IsViolation(speed, zone.max_speed);
            match.speed_excess = SpeedViolationChecker::CalculateExcess(speed, zone.max_speed);
            matches.push_back(match);
        }
    }
    
    std::sort(matches.begin(), matches.end(),
        [](const SpeedZoneMatch& a, const SpeedZoneMatch& b) {
            if (a.is_violation != b.is_violation) {
                return a.is_violation > b.is_violation;
            }
            return a.speed_excess > b.speed_excess;
        });
    
    return matches;
}

std::vector<SpeedZone> SpeedZoneMatcher::GetZonesAtPosition(const Coordinate& position) {
    std::vector<SpeedZone> result;
    
    for (const auto& pair : m_zones) {
        if (IsInsideZone(position, pair.second)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<SpeedZone> SpeedZoneMatcher::GetNearbyZones(const Coordinate& position,
                                                         double radius_km) {
    std::vector<std::pair<double, SpeedZone>> zones_with_distance;
    
    for (const auto& pair : m_zones) {
        double distance = CalculateDistanceToZone(position, pair.second);
        if (distance <= radius_km) {
            zones_with_distance.push_back({distance, pair.second});
        }
    }
    
    std::sort(zones_with_distance.begin(), zones_with_distance.end(),
        [](const std::pair<double, SpeedZone>& a, const std::pair<double, SpeedZone>& b) {
            return a.first < b.first;
        });
    
    std::vector<SpeedZone> result;
    for (const auto& p : zones_with_distance) {
        result.push_back(p.second);
    }
    
    return result;
}

void SpeedZoneMatcher::Clear() {
    m_zones.clear();
}

bool SpeedZoneMatcher::IsZoneActive(const SpeedZone& zone, const DateTime& time) const {
    if (!zone.is_active) {
        return false;
    }
    
    std::time_t validFromTs = zone.valid_from.ToTimestamp();
    std::time_t validToTs = zone.valid_to.ToTimestamp();
    std::time_t currentTs = time.ToTimestamp();
    
    if (validFromTs > 0 && currentTs < validFromTs) {
        return false;
    }
    
    if (validToTs > 0 && currentTs > validToTs) {
        return false;
    }
    
    return true;
}

double SpeedZoneMatcher::CalculateDistanceToZone(const Coordinate& position, 
                                                  const SpeedZone& zone) const {
    if (zone.area && zone.area->IsValid()) {
        auto point = ogc::Point::Create(position.longitude, position.latitude);
        return zone.area->Distance(point.get());
    }
    return std::numeric_limits<double>::max();
}

bool SpeedZoneMatcher::IsInsideZone(const Coordinate& position, const SpeedZone& zone) const {
    if (zone.area && zone.area->IsValid()) {
        auto point = ogc::Point::Create(position.longitude, position.latitude);
        return zone.area->Contains(point.get());
    }
    return false;
}

std::unique_ptr<ISpeedZoneMatcher> ISpeedZoneMatcher::Create() {
    return std::unique_ptr<ISpeedZoneMatcher>(new SpeedZoneMatcher());
}

void SpeedZoneIndex::Build(const std::vector<SpeedZone>& zones) {
    m_zones = zones;
}

std::vector<SpeedZone> SpeedZoneIndex::Query(const Coordinate& position) {
    std::vector<SpeedZone> result;
    
    for (const auto& zone : m_zones) {
        if (zone.area && zone.area->IsValid()) {
            auto point = ogc::Point::Create(position.longitude, position.latitude);
            if (zone.area->Contains(point.get())) {
                result.push_back(zone);
            }
        }
    }
    
    return result;
}

void SpeedZoneIndex::Clear() {
    m_zones.clear();
}

bool SpeedViolationChecker::IsViolation(double current_speed, double max_speed, double tolerance) {
    if (max_speed <= 0) {
        return false;
    }
    return current_speed > (max_speed + tolerance);
}

double SpeedViolationChecker::CalculateExcess(double current_speed, double max_speed) {
    if (current_speed <= max_speed) {
        return 0.0;
    }
    return current_speed - max_speed;
}

int SpeedViolationChecker::DetermineSeverity(double excess, double max_speed) {
    if (excess <= 0) {
        return 0;
    }
    
    double ratio = excess / max_speed;
    
    if (ratio < 0.1) {
        return 1;
    } else if (ratio < 0.2) {
        return 2;
    } else if (ratio < 0.3) {
        return 3;
    } else {
        return 4;
    }
}

}
}
