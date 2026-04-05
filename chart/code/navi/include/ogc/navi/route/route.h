#pragma once

#include "ogc/navi/route/waypoint.h"
#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace navi {

struct RouteData {
    std::string id;
    std::string name;
    std::string description;
    RouteStatus status;
    double total_distance;
    double estimated_duration;
    std::string departure;
    std::string destination;
    std::vector<WaypointData> waypoints;
    double created_time;
    double modified_time;
    
    RouteData()
        : status(RouteStatus::Planned)
        , total_distance(0.0)
        , estimated_duration(0.0)
        , created_time(0.0)
        , modified_time(0.0)
    {}
};

class OGC_NAVI_API Route {
public:
    static Route* Create();
    static Route* Create(const RouteData& data);
    
    virtual ~Route() = default;
    
    virtual const std::string& GetId() const = 0;
    virtual void SetId(const std::string& id) = 0;
    
    virtual const std::string& GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;
    
    virtual const std::string& GetDescription() const = 0;
    virtual void SetDescription(const std::string& desc) = 0;
    
    virtual RouteStatus GetStatus() const = 0;
    virtual void SetStatus(RouteStatus status) = 0;
    
    virtual double GetTotalDistance() const = 0;
    virtual double GetEstimatedDuration() const = 0;
    
    virtual const std::string& GetDeparture() const = 0;
    virtual void SetDeparture(const std::string& departure) = 0;
    
    virtual const std::string& GetDestination() const = 0;
    virtual void SetDestination(const std::string& destination) = 0;
    
    virtual int GetWaypointCount() const = 0;
    virtual Waypoint* GetWaypoint(int index) = 0;
    virtual const Waypoint* GetWaypoint(int index) const = 0;
    
    virtual void AddWaypoint(Waypoint* waypoint) = 0;
    virtual void InsertWaypoint(int index, Waypoint* waypoint) = 0;
    virtual void RemoveWaypoint(int index) = 0;
    virtual void ClearWaypoints() = 0;
    
    virtual void Reverse() = 0;
    virtual void Optimize() = 0;
    
    virtual double CalculateTotalDistance() = 0;
    virtual double CalculateLegDistance(int leg_index) = 0;
    virtual double CalculateLegBearing(int leg_index) = 0;
    
    virtual RouteData ToData() const = 0;
    virtual void FromData(const RouteData& data) = 0;
    
    virtual void ReleaseReference() = 0;
};

}
}
