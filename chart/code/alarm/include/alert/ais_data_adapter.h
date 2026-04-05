#ifndef ALERT_AIS_DATA_ADAPTER_H
#define ALERT_AIS_DATA_ADAPTER_H

#include "alert/external_data_gateway.h"
#include "alert/coordinate.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace alert {

struct AISVoyageData {
    std::string mmsi;
    std::string destination;
    double draft;
    double cargoType;
    std::string eta;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    AISVoyageData()
        : draft(0), cargoType(0), valid(false) {}
};

struct AISTargetTrack {
    std::string mmsi;
    std::vector<Coordinate> positions;
    std::vector<std::string> timestamps;
    std::vector<double> speeds;
    std::vector<double> courses;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    AISTargetTrack()
        : valid(false) {}
};

struct AISStaticData {
    std::string mmsi;
    std::string shipName;
    std::string callSign;
    int shipType;
    double length;
    double beam;
    double draft;
    int imoNumber;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    AISStaticData()
        : shipType(0), length(0), beam(0), draft(0), imoNumber(0), valid(false) {}
};

struct AISDynamicData {
    std::string mmsi;
    Coordinate position;
    double speed;
    double course;
    double heading;
    int navStatus;
    double rot;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    AISDynamicData()
        : speed(0), course(0), heading(0), navStatus(0), rot(0), valid(false) {}
};

struct AISTargetSummary {
    std::string mmsi;
    std::string shipName;
    Coordinate position;
    double speed;
    double course;
    double heading;
    int shipType;
    int navStatus;
    double distance;
    double bearing;
    double cpa;
    double tcpa;
    std::string dataSource;
    std::string timestamp;
    bool valid;
    
    AISTargetSummary()
        : speed(0), course(0), heading(0), shipType(0), navStatus(0),
          distance(0), bearing(0), cpa(0), tcpa(0), valid(false) {}
};

struct AISAreaFilter {
    Coordinate center;
    double radiusNm;
    std::vector<int> shipTypes;
    std::vector<int> navStatuses;
    double minSpeed;
    double maxSpeed;
    bool valid;
    
    AISAreaFilter()
        : radiusNm(10), minSpeed(0), maxSpeed(30), valid(false) {}
};

class IAISDataAdapter : public IDataProvider {
public:
    virtual ~IAISDataAdapter() {}
    
    virtual std::vector<AISData> GetTargetsInArea(const Coordinate& center, double radiusNm) = 0;
    virtual AISData GetTargetByMMSI(const std::string& mmsi) = 0;
    virtual std::vector<AISData> GetTargetsByType(int shipType, const Coordinate& center, double radiusNm) = 0;
    
    virtual AISTargetTrack GetTargetTrack(const std::string& mmsi, int hours) = 0;
    virtual AISTargetTrack GetTargetTrackByTime(const std::string& mmsi, 
                                                  const std::string& startTime, 
                                                  const std::string& endTime) = 0;
    virtual std::vector<AISTargetTrack> GetAreaTracks(const Coordinate& center, 
                                                       double radiusNm, 
                                                       int hours) = 0;
    
    virtual AISStaticData GetStaticData(const std::string& mmsi) = 0;
    virtual AISDynamicData GetDynamicData(const std::string& mmsi) = 0;
    virtual AISVoyageData GetVoyageData(const std::string& mmsi) = 0;
    
    virtual AISTargetSummary GetTargetSummary(const std::string& mmsi, 
                                               const Coordinate& referencePosition) = 0;
    virtual std::vector<AISTargetSummary> GetTargetSummaries(const Coordinate& referencePosition, 
                                                               double radiusNm) = 0;
    
    virtual int GetTargetCount(const Coordinate& center, double radiusNm) = 0;
    virtual std::map<int, int> GetTargetCountByType(const Coordinate& center, double radiusNm) = 0;
    
    virtual void SetAISApiConfig(const DataSourceConfig& config) = 0;
    virtual DataSourceConfig GetAISApiConfig() const = 0;
    
    virtual void SetCacheEnabled(bool enabled) = 0;
    virtual void SetCacheTTL(int ttlSeconds) = 0;
    virtual void ClearCache() = 0;
};

class AISDataAdapter : public IAISDataAdapter {
public:
    AISDataAdapter();
    virtual ~AISDataAdapter();
    
    std::string GetProviderId() const override { return "ais"; }
    std::string GetProviderType() const override { return "ais"; }
    
    bool Initialize(const DataSourceConfig& config) override;
    void Shutdown() override;
    bool IsConnected() const override;
    void SetConnectionCallback(std::function<void(bool)> callback) override;
    
    std::vector<AISData> GetTargetsInArea(const Coordinate& center, double radiusNm) override;
    AISData GetTargetByMMSI(const std::string& mmsi) override;
    std::vector<AISData> GetTargetsByType(int shipType, const Coordinate& center, double radiusNm) override;
    
    AISTargetTrack GetTargetTrack(const std::string& mmsi, int hours) override;
    AISTargetTrack GetTargetTrackByTime(const std::string& mmsi, 
                                         const std::string& startTime, 
                                         const std::string& endTime) override;
    std::vector<AISTargetTrack> GetAreaTracks(const Coordinate& center, 
                                               double radiusNm, 
                                               int hours) override;
    
    AISStaticData GetStaticData(const std::string& mmsi) override;
    AISDynamicData GetDynamicData(const std::string& mmsi) override;
    AISVoyageData GetVoyageData(const std::string& mmsi) override;
    
    AISTargetSummary GetTargetSummary(const std::string& mmsi, 
                                        const Coordinate& referencePosition) override;
    std::vector<AISTargetSummary> GetTargetSummaries(const Coordinate& referencePosition, 
                                                       double radiusNm) override;
    
    int GetTargetCount(const Coordinate& center, double radiusNm) override;
    std::map<int, int> GetTargetCountByType(const Coordinate& center, double radiusNm) override;
    
    void SetAISApiConfig(const DataSourceConfig& config) override;
    DataSourceConfig GetAISApiConfig() const override;
    
    void SetCacheEnabled(bool enabled) override;
    void SetCacheTTL(int ttlSeconds) override;
    void ClearCache() override;
    
    std::string GetLastError() const;
    
private:
    DataSourceConfig m_aisApiConfig;
    std::function<void(bool)> m_connectionCallback;
    bool m_connected;
    bool m_cacheEnabled;
    int m_cacheTTL;
    std::string m_lastError;
    
    std::map<std::string, AISData> m_targetCache;
    std::map<std::string, AISStaticData> m_staticDataCache;
    std::map<std::string, AISTargetTrack> m_trackCache;
    
    std::string GenerateCacheKey(const Coordinate& position, const std::string& type);
    std::string GenerateCacheKey(const std::string& id, const std::string& type);
    std::string GetCurrentTimestamp();
    double CalculateDistanceNm(const Coordinate& p1, const Coordinate& p2);
    double CalculateBearing(const Coordinate& from, const Coordinate& to);
    void CalculateCPA_TCPA(const Coordinate& myPos, double mySpeed, double myCourse,
                           const Coordinate& targetPos, double targetSpeed, 
                           double targetCourse, double& cpa, double& tcpa);
};

}

#endif
