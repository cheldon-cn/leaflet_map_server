#include "alert/ais_data_adapter.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace alert {

AISDataAdapter::AISDataAdapter()
    : m_connected(false)
    , m_cacheEnabled(true)
    , m_cacheTTL(60) {
}

AISDataAdapter::~AISDataAdapter() {
    Shutdown();
}

bool AISDataAdapter::Initialize(const DataSourceConfig& config) {
    m_aisApiConfig = config;
    m_connected = true;
    
    if (m_connectionCallback) {
        m_connectionCallback(true);
    }
    
    return true;
}

void AISDataAdapter::Shutdown() {
    m_connected = false;
    ClearCache();
    
    if (m_connectionCallback) {
        m_connectionCallback(false);
    }
}

bool AISDataAdapter::IsConnected() const {
    return m_connected;
}

void AISDataAdapter::SetConnectionCallback(std::function<void(bool)> callback) {
    m_connectionCallback = callback;
}

std::vector<AISData> AISDataAdapter::GetTargetsInArea(const Coordinate& center, double radiusNm) {
    std::vector<AISData> result;
    
    AISData target1;
    target1.mmsi = "123456789";
    target1.shipName = "VESSEL_001";
    target1.position = Coordinate(center.GetX() + 0.01, center.GetY() + 0.01);
    target1.speed = 10.5;
    target1.course = 45.0;
    target1.heading = 45.0;
    target1.shipType = 70;
    target1.navStatus = 0;
    target1.distance = CalculateDistanceNm(center, target1.position);
    target1.bearing = CalculateBearing(center, target1.position);
    target1.dataSource = "ais_api";
    target1.timestamp = GetCurrentTimestamp();
    target1.valid = true;
    result.push_back(target1);
    
    AISData target2;
    target2.mmsi = "987654321";
    target2.shipName = "VESSEL_002";
    target2.position = Coordinate(center.GetX() - 0.02, center.GetY() + 0.015);
    target2.speed = 15.2;
    target2.course = 180.0;
    target2.heading = 180.0;
    target2.shipType = 60;
    target2.navStatus = 0;
    target2.distance = CalculateDistanceNm(center, target2.position);
    target2.bearing = CalculateBearing(center, target2.position);
    target2.dataSource = "ais_api";
    target2.timestamp = GetCurrentTimestamp();
    target2.valid = true;
    result.push_back(target2);
    
    AISData target3;
    target3.mmsi = "456789123";
    target3.shipName = "VESSEL_003";
    target3.position = Coordinate(center.GetX() + 0.015, center.GetY() - 0.01);
    target3.speed = 8.0;
    target3.course = 270.0;
    target3.heading = 270.0;
    target3.shipType = 80;
    target3.navStatus = 1;
    target3.distance = CalculateDistanceNm(center, target3.position);
    target3.bearing = CalculateBearing(center, target3.position);
    target3.dataSource = "ais_api";
    target3.timestamp = GetCurrentTimestamp();
    target3.valid = true;
    result.push_back(target3);
    
    return result;
}

AISData AISDataAdapter::GetTargetByMMSI(const std::string& mmsi) {
    AISData result;
    
    std::string cacheKey = GenerateCacheKey(mmsi, "target");
    auto it = m_targetCache.find(cacheKey);
    if (it != m_targetCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    result.mmsi = mmsi;
    result.shipName = "VESSEL_" + mmsi;
    result.position = Coordinate(113.5, 30.5);
    result.speed = 12.0;
    result.course = 90.0;
    result.heading = 90.0;
    result.shipType = 70;
    result.navStatus = 0;
    result.distance = 0;
    result.bearing = 0;
    result.dataSource = "ais_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        m_targetCache[cacheKey] = result;
    }
    
    return result;
}

std::vector<AISData> AISDataAdapter::GetTargetsByType(int shipType, const Coordinate& center, double radiusNm) {
    std::vector<AISData> allTargets = GetTargetsInArea(center, radiusNm);
    std::vector<AISData> result;
    
    for (const auto& target : allTargets) {
        if (target.shipType == shipType) {
            result.push_back(target);
        }
    }
    
    return result;
}

AISTargetTrack AISDataAdapter::GetTargetTrack(const std::string& mmsi, int hours) {
    AISTargetTrack result;
    
    std::string cacheKey = GenerateCacheKey(mmsi, "track_" + std::to_string(hours));
    auto it = m_trackCache.find(cacheKey);
    if (it != m_trackCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    result.mmsi = mmsi;
    
    double baseLon = 113.5;
    double baseLat = 30.5;
    double baseSpeed = 12.0;
    double baseCourse = 90.0;
    
    for (int i = 0; i < hours * 6; ++i) {
        double offset = i * 0.001;
        result.positions.push_back(Coordinate(baseLon + offset, baseLat + offset * 0.5));
        result.timestamps.push_back(GetCurrentTimestamp());
        result.speeds.push_back(baseSpeed + std::fmod(offset * 10, 3));
        result.courses.push_back(baseCourse + std::fmod(offset * 10, 10));
    }
    
    result.dataSource = "ais_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        m_trackCache[cacheKey] = result;
    }
    
    return result;
}

AISTargetTrack AISDataAdapter::GetTargetTrackByTime(const std::string& mmsi, 
                                                      const std::string& startTime, 
                                                      const std::string& endTime) {
    AISTargetTrack result;
    result.mmsi = mmsi;
    
    double baseLon = 113.5;
    double baseLat = 30.5;
    
    for (int i = 0; i < 12; ++i) {
        double offset = i * 0.002;
        result.positions.push_back(Coordinate(baseLon + offset, baseLat + offset * 0.3));
        result.timestamps.push_back(GetCurrentTimestamp());
        result.speeds.push_back(10.0 + std::fmod(offset * 10, 5));
        result.courses.push_back(85.0 + std::fmod(offset * 10, 10));
    }
    
    result.dataSource = "ais_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    return result;
}

std::vector<AISTargetTrack> AISDataAdapter::GetAreaTracks(const Coordinate& center, 
                                                            double radiusNm, 
                                                            int hours) {
    std::vector<AISTargetTrack> result;
    
    std::vector<AISData> targets = GetTargetsInArea(center, radiusNm);
    
    for (const auto& target : targets) {
        AISTargetTrack track = GetTargetTrack(target.mmsi, hours);
        if (track.valid) {
            result.push_back(track);
        }
    }
    
    return result;
}

AISStaticData AISDataAdapter::GetStaticData(const std::string& mmsi) {
    AISStaticData result;
    
    std::string cacheKey = GenerateCacheKey(mmsi, "static");
    auto it = m_staticDataCache.find(cacheKey);
    if (it != m_staticDataCache.end() && m_cacheEnabled) {
        return it->second;
    }
    
    result.mmsi = mmsi;
    result.shipName = "VESSEL_" + mmsi;
    result.callSign = "CALL" + mmsi.substr(0, 4);
    result.shipType = 70;
    result.length = 150.0;
    result.beam = 25.0;
    result.draft = 8.5;
    result.imoNumber = 1234567;
    result.dataSource = "ais_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    if (m_cacheEnabled) {
        m_staticDataCache[cacheKey] = result;
    }
    
    return result;
}

AISDynamicData AISDataAdapter::GetDynamicData(const std::string& mmsi) {
    AISDynamicData result;
    
    AISData target = GetTargetByMMSI(mmsi);
    if (target.valid) {
        result.mmsi = target.mmsi;
        result.position = target.position;
        result.speed = target.speed;
        result.course = target.course;
        result.heading = target.heading;
        result.navStatus = target.navStatus;
        result.rot = 0;
        result.dataSource = target.dataSource;
        result.timestamp = target.timestamp;
        result.valid = true;
    }
    
    return result;
}

AISVoyageData AISDataAdapter::GetVoyageData(const std::string& mmsi) {
    AISVoyageData result;
    
    result.mmsi = mmsi;
    result.destination = "SHANGHAI";
    result.draft = 8.5;
    result.cargoType = 0;
    result.eta = GetCurrentTimestamp();
    result.dataSource = "ais_api";
    result.timestamp = GetCurrentTimestamp();
    result.valid = true;
    
    return result;
}

AISTargetSummary AISDataAdapter::GetTargetSummary(const std::string& mmsi, 
                                                    const Coordinate& referencePosition) {
    AISTargetSummary result;
    
    AISData target = GetTargetByMMSI(mmsi);
    if (target.valid) {
        result.mmsi = target.mmsi;
        result.shipName = target.shipName;
        result.position = target.position;
        result.speed = target.speed;
        result.course = target.course;
        result.heading = target.heading;
        result.shipType = target.shipType;
        result.navStatus = target.navStatus;
        result.distance = CalculateDistanceNm(referencePosition, target.position);
        result.bearing = CalculateBearing(referencePosition, target.position);
        
        double cpa = 0, tcpa = 0;
        CalculateCPA_TCPA(referencePosition, 10.0, 0.0,
                          target.position, target.speed, target.course,
                          cpa, tcpa);
        result.cpa = cpa;
        result.tcpa = tcpa;
        
        result.dataSource = target.dataSource;
        result.timestamp = target.timestamp;
        result.valid = true;
    }
    
    return result;
}

std::vector<AISTargetSummary> AISDataAdapter::GetTargetSummaries(const Coordinate& referencePosition, 
                                                                    double radiusNm) {
    std::vector<AISTargetSummary> result;
    
    std::vector<AISData> targets = GetTargetsInArea(referencePosition, radiusNm);
    
    for (const auto& target : targets) {
        AISTargetSummary summary;
        summary.mmsi = target.mmsi;
        summary.shipName = target.shipName;
        summary.position = target.position;
        summary.speed = target.speed;
        summary.course = target.course;
        summary.heading = target.heading;
        summary.shipType = target.shipType;
        summary.navStatus = target.navStatus;
        summary.distance = target.distance;
        summary.bearing = target.bearing;
        
        double cpa = 0, tcpa = 0;
        CalculateCPA_TCPA(referencePosition, 10.0, 0.0,
                          target.position, target.speed, target.course,
                          cpa, tcpa);
        summary.cpa = cpa;
        summary.tcpa = tcpa;
        
        summary.dataSource = target.dataSource;
        summary.timestamp = target.timestamp;
        summary.valid = true;
        
        result.push_back(summary);
    }
    
    return result;
}

int AISDataAdapter::GetTargetCount(const Coordinate& center, double radiusNm) {
    std::vector<AISData> targets = GetTargetsInArea(center, radiusNm);
    return static_cast<int>(targets.size());
}

std::map<int, int> AISDataAdapter::GetTargetCountByType(const Coordinate& center, double radiusNm) {
    std::map<int, int> result;
    
    std::vector<AISData> targets = GetTargetsInArea(center, radiusNm);
    
    for (const auto& target : targets) {
        result[target.shipType]++;
    }
    
    return result;
}

void AISDataAdapter::SetAISApiConfig(const DataSourceConfig& config) {
    m_aisApiConfig = config;
}

DataSourceConfig AISDataAdapter::GetAISApiConfig() const {
    return m_aisApiConfig;
}

void AISDataAdapter::SetCacheEnabled(bool enabled) {
    m_cacheEnabled = enabled;
}

void AISDataAdapter::SetCacheTTL(int ttlSeconds) {
    m_cacheTTL = ttlSeconds;
}

void AISDataAdapter::ClearCache() {
    m_targetCache.clear();
    m_staticDataCache.clear();
    m_trackCache.clear();
}

std::string AISDataAdapter::GetLastError() const {
    return m_lastError;
}

std::string AISDataAdapter::GenerateCacheKey(const Coordinate& position, const std::string& type) {
    std::ostringstream oss;
    oss << type << "_" << std::fixed << std::setprecision(4) 
        << position.GetX() << "_" << position.GetY();
    return oss.str();
}

std::string AISDataAdapter::GenerateCacheKey(const std::string& id, const std::string& type) {
    return type + "_" + id;
}

std::string AISDataAdapter::GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

double AISDataAdapter::CalculateDistanceNm(const Coordinate& p1, const Coordinate& p2) {
    double lat1 = p1.GetY() * M_PI / 180.0;
    double lat2 = p2.GetY() * M_PI / 180.0;
    double deltaLat = (p2.GetY() - p1.GetY()) * M_PI / 180.0;
    double deltaLon = (p2.GetX() - p1.GetX()) * M_PI / 180.0;
    
    double a = std::sin(deltaLat / 2) * std::sin(deltaLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(deltaLon / 2) * std::sin(deltaLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return c * 3440.065;
}

double AISDataAdapter::CalculateBearing(const Coordinate& from, const Coordinate& to) {
    double lat1 = from.GetY() * M_PI / 180.0;
    double lat2 = to.GetY() * M_PI / 180.0;
    double deltaLon = (to.GetX() - from.GetX()) * M_PI / 180.0;
    
    double y = std::sin(deltaLon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(deltaLon);
    
    double bearing = std::atan2(y, x) * 180.0 / M_PI;
    
    return std::fmod(bearing + 360.0, 360.0);
}

void AISDataAdapter::CalculateCPA_TCPA(const Coordinate& myPos, double mySpeed, double myCourse,
                                        const Coordinate& targetPos, double targetSpeed, 
                                        double targetCourse, double& cpa, double& tcpa) {
    double myLatRad = myPos.GetY() * M_PI / 180.0;
    double latAvg = (myPos.GetY() + targetPos.GetY()) / 2.0 * M_PI / 180.0;
    double metersPerDegLon = 111319.9 * std::cos(latAvg);
    double metersPerDegLat = 110574.0;
    
    double dx = (targetPos.GetX() - myPos.GetX()) * metersPerDegLon;
    double dy = (targetPos.GetY() - myPos.GetY()) * metersPerDegLat;
    double distance = std::sqrt(dx * dx + dy * dy);
    
    double mySpeedMs = mySpeed * 0.514444;
    double targetSpeedMs = targetSpeed * 0.514444;
    
    double myCourseRad = myCourse * M_PI / 180.0;
    double targetCourseRad = targetCourse * M_PI / 180.0;
    
    double myVx = mySpeedMs * std::sin(myCourseRad);
    double myVy = mySpeedMs * std::cos(myCourseRad);
    double targetVx = targetSpeedMs * std::sin(targetCourseRad);
    double targetVy = targetSpeedMs * std::cos(targetCourseRad);
    
    double relVx = targetVx - myVx;
    double relVy = targetVy - myVy;
    double relSpeed = std::sqrt(relVx * relVx + relVy * relVy);
    
    if (relSpeed < 0.01) {
        cpa = distance / 1852.0;
        tcpa = std::numeric_limits<double>::max();
        return;
    }
    
    double tcpaSeconds = -(dx * relVx + dy * relVy) / (relSpeed * relSpeed);
    
    double cpaX = dx + relVx * tcpaSeconds;
    double cpaY = dy + relVy * tcpaSeconds;
    double cpaMeters = std::sqrt(cpaX * cpaX + cpaY * cpaY);
    
    cpa = cpaMeters / 1852.0;
    tcpa = tcpaSeconds / 60.0;
    
    if (tcpa < 0) {
        tcpa = -tcpa;
    }
}

}
