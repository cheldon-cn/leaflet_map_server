#ifndef OGC_ALERT_SPEED_ZONE_MATCHER_H
#define OGC_ALERT_SPEED_ZONE_MATCHER_H

#include "export.h"
#include "types.h"
#include "ogc/geom/geometry.h"
#include <vector>
#include <map>
#include <memory>
#include <string>

namespace ogc {
namespace alert {

struct SpeedZone {
    std::string zone_id;
    std::string zone_name;
    ogc::GeometrySharedPtr area;
    double max_speed;
    double min_speed;
    int speed_unit;
    std::string time_restriction;
    std::string vessel_type_restriction;
    bool is_active;
    DateTime valid_from;
    DateTime valid_to;
};

struct SpeedZoneMatch {
    SpeedZone zone;
    bool is_inside;
    double distance_to_zone;
    double current_speed;
    double speed_limit;
    double speed_excess;
    bool is_violation;
};

class OGC_ALERT_API ISpeedZoneMatcher {
public:
    virtual ~ISpeedZoneMatcher() = default;
    
    virtual void AddZone(const SpeedZone& zone) = 0;
    virtual void RemoveZone(const std::string& zone_id) = 0;
    virtual void UpdateZone(const SpeedZone& zone) = 0;
    virtual SpeedZone GetZone(const std::string& zone_id) const = 0;
    
    virtual std::vector<SpeedZoneMatch> Match(const Coordinate& position,
                                               double speed,
                                               const DateTime& time) = 0;
    
    virtual std::vector<SpeedZone> GetZonesAtPosition(const Coordinate& position) = 0;
    virtual std::vector<SpeedZone> GetNearbyZones(const Coordinate& position,
                                                   double radius_km) = 0;
    
    virtual void Clear() = 0;
    
    static std::unique_ptr<ISpeedZoneMatcher> Create();
};

class OGC_ALERT_API SpeedZoneMatcher : public ISpeedZoneMatcher {
public:
    SpeedZoneMatcher();
    ~SpeedZoneMatcher() override;
    
    void AddZone(const SpeedZone& zone) override;
    void RemoveZone(const std::string& zone_id) override;
    void UpdateZone(const SpeedZone& zone) override;
    SpeedZone GetZone(const std::string& zone_id) const override;
    
    std::vector<SpeedZoneMatch> Match(const Coordinate& position,
                                       double speed,
                                       const DateTime& time) override;
    
    std::vector<SpeedZone> GetZonesAtPosition(const Coordinate& position) override;
    std::vector<SpeedZone> GetNearbyZones(const Coordinate& position,
                                           double radius_km) override;
    
    void Clear() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    bool IsZoneActive(const SpeedZone& zone, const DateTime& time) const;
    double CalculateDistanceToZone(const Coordinate& position, const SpeedZone& zone) const;
    bool IsInsideZone(const Coordinate& position, const SpeedZone& zone) const;
};

class OGC_ALERT_API SpeedZoneIndex {
public:
    void Build(const std::vector<SpeedZone>& zones);
    std::vector<SpeedZone> Query(const Coordinate& position);
    void Clear();

private:
    std::vector<SpeedZone> m_zones;
};

class OGC_ALERT_API SpeedViolationChecker {
public:
    static bool IsViolation(double current_speed, double max_speed, double tolerance = 0.0);
    static double CalculateExcess(double current_speed, double max_speed);
    static int DetermineSeverity(double excess, double max_speed);
};

}
}

#endif
