#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <memory>

namespace ogc {
namespace navi {

struct WaypointData {
    std::string id;
    std::string name;
    double longitude;
    double latitude;
    double arrival_radius;
    double speed_limit;
    WaypointAction action;
    std::string notes;
    int sequence;
    bool is_mandatory;
    
    WaypointData()
        : longitude(0.0)
        , latitude(0.0)
        , arrival_radius(0.5)
        , speed_limit(0.0)
        , action(WaypointAction::None)
        , sequence(0)
        , is_mandatory(true)
    {}
};

class OGC_NAVI_API Waypoint {
public:
    static Waypoint* Create();
    static Waypoint* Create(const WaypointData& data);
    
    virtual ~Waypoint() = default;
    
    virtual const std::string& GetId() const = 0;
    virtual void SetId(const std::string& id) = 0;
    
    virtual const std::string& GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;
    
    virtual double GetLongitude() const = 0;
    virtual void SetLongitude(double lon) = 0;
    
    virtual double GetLatitude() const = 0;
    virtual void SetLatitude(double lat) = 0;
    
    virtual GeoPoint GetPosition() const = 0;
    virtual void SetPosition(double lon, double lat) = 0;
    
    virtual double GetArrivalRadius() const = 0;
    virtual void SetArrivalRadius(double radius) = 0;
    
    virtual double GetSpeedLimit() const = 0;
    virtual void SetSpeedLimit(double speed) = 0;
    
    virtual WaypointAction GetAction() const = 0;
    virtual void SetAction(WaypointAction action) = 0;
    
    virtual int GetSequence() const = 0;
    virtual void SetSequence(int seq) = 0;
    
    virtual bool IsMandatory() const = 0;
    virtual void SetMandatory(bool mandatory) = 0;
    
    virtual const std::string& GetNotes() const = 0;
    virtual void SetNotes(const std::string& notes) = 0;
    
    virtual WaypointData ToData() const = 0;
    virtual void FromData(const WaypointData& data) = 0;
    
    virtual void ReleaseReference() = 0;
};

}
}
