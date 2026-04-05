#pragma once

#include "ogc/navi/ais/ais_message.h"
#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <memory>

namespace ogc {
namespace navi {

struct AisTargetData {
    uint32_t mmsi;
    std::string ship_name;
    std::string callsign;
    AisShipType ship_type;
    AisNavigationStatus nav_status;
    double longitude;
    double latitude;
    double speed_over_ground;
    double course_over_ground;
    double heading;
    int ship_length;
    int ship_width;
    double draught;
    std::string destination;
    double last_update;
    double cpa;
    double tcpa;
    bool collision_risk;
    
    AisTargetData()
        : mmsi(0)
        , ship_type(AisShipType::NotAvailable)
        , nav_status(AisNavigationStatus::NotAvailable)
        , longitude(0.0)
        , latitude(0.0)
        , speed_over_ground(0.0)
        , course_over_ground(0.0)
        , heading(0.0)
        , ship_length(0)
        , ship_width(0)
        , draught(0.0)
        , last_update(0.0)
        , cpa(0.0)
        , tcpa(0.0)
        , collision_risk(false)
    {}
};

class OGC_NAVI_API AisTarget {
public:
    static AisTarget* Create();
    static AisTarget* Create(uint32_t mmsi);
    
    virtual ~AisTarget() = default;
    
    virtual uint32_t GetMmsi() const = 0;
    
    virtual const std::string& GetShipName() const = 0;
    virtual void SetShipName(const std::string& name) = 0;
    
    virtual const std::string& GetCallsign() const = 0;
    virtual void SetCallsign(const std::string& callsign) = 0;
    
    virtual AisShipType GetShipType() const = 0;
    virtual void SetShipType(AisShipType type) = 0;
    
    virtual AisNavigationStatus GetNavigationStatus() const = 0;
    virtual void SetNavigationStatus(AisNavigationStatus status) = 0;
    
    virtual double GetLongitude() const = 0;
    virtual void SetLongitude(double lon) = 0;
    
    virtual double GetLatitude() const = 0;
    virtual void SetLatitude(double lat) = 0;
    
    virtual GeoPoint GetPosition() const = 0;
    virtual void SetPosition(double lon, double lat) = 0;
    
    virtual double GetSpeedOverGround() const = 0;
    virtual void SetSpeedOverGround(double speed) = 0;
    
    virtual double GetCourseOverGround() const = 0;
    virtual void SetCourseOverGround(double course) = 0;
    
    virtual double GetHeading() const = 0;
    virtual void SetHeading(double heading) = 0;
    
    virtual int GetShipLength() const = 0;
    virtual void SetShipLength(int length) = 0;
    
    virtual int GetShipWidth() const = 0;
    virtual void SetShipWidth(int width) = 0;
    
    virtual double GetDraught() const = 0;
    virtual void SetDraught(double draught) = 0;
    
    virtual const std::string& GetDestination() const = 0;
    virtual void SetDestination(const std::string& destination) = 0;
    
    virtual double GetLastUpdate() const = 0;
    virtual void SetLastUpdate(double timestamp) = 0;
    
    virtual double GetCpa() const = 0;
    virtual void SetCpa(double cpa) = 0;
    
    virtual double GetTcpa() const = 0;
    virtual void SetTcpa(double tcpa) = 0;
    
    virtual bool HasCollisionRisk() const = 0;
    virtual void SetCollisionRisk(bool risk) = 0;
    
    virtual void UpdatePositionReport(const AisPositionReport& report) = 0;
    virtual void UpdateStaticData(const AisStaticData& data) = 0;
    
    virtual AisTargetData ToData() const = 0;
    virtual void FromData(const AisTargetData& data) = 0;
    
    virtual void ReleaseReference() = 0;
};

}
}
