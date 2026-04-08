#pragma once

#include "ogc/navi/ais/ais_target.h"
#include "ogc/navi/ais/ais_message.h"
#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace navi {

struct CollisionRisk {
    uint32_t target_mmsi;
    double cpa;
    double tcpa;
    double distance;
    double bearing;
    bool is_risk;
    AlertLevel risk_level;
    
    CollisionRisk()
        : target_mmsi(0)
        , cpa(0.0)
        , tcpa(0.0)
        , distance(0.0)
        , bearing(0.0)
        , is_risk(false)
        , risk_level(AlertLevel::Info)
    {}
};

using AisTargetCallback = std::function<void(const AisTargetData&)>;
using CollisionRiskCallback = std::function<void(const CollisionRisk&)>;

class OGC_NAVI_API AisManager {
public:
    static AisManager& Instance();
    
    bool Initialize();
    void Shutdown();
    
    void ProcessAisMessage(const std::string& nmea_message);
    void ProcessAisMessage(const std::vector<uint8_t>& payload);
    
    int GetTargetCount() const;
    std::vector<AisTarget*> GetAllTargets();
    AisTarget* GetTarget(uint32_t mmsi) const;
    
    void RemoveStaleTargets(double max_age_seconds);
    
    void CalculateCpaTcpa(
        const PositionData& own_position,
        const GeoPoint& target_position,
        double target_course,
        double target_speed,
        double& cpa,
        double& tcpa);
    
    std::vector<CollisionRisk> AssessCollisionRisks(
        const PositionData& own_position,
        double cpa_threshold_nm,
        double tcpa_threshold_minutes);
    
    void SetCpaThreshold(double cpa_nm);
    void SetTcpaThreshold(double tcpa_minutes);
    
    void SetTargetUpdateCallback(AisTargetCallback callback);
    void SetCollisionRiskCallback(CollisionRiskCallback callback);
    
private:
    AisManager();
    ~AisManager();
    AisManager(const AisManager&) = delete;
    AisManager& operator=(const AisManager&) = delete;
    
    void ParsePositionReport(const std::vector<uint8_t>& payload, int message_type);
    void ParseStaticData(const std::vector<uint8_t>& payload, int message_type);
    std::vector<uint8_t> DecodePayload(const std::string& payload);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
    double cpa_threshold_;
    double tcpa_threshold_;
    AisTargetCallback target_callback_;
    CollisionRiskCallback collision_callback_;
};

}
}
