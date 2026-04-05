#pragma once

#include "ogc/navi/track/track_point.h"
#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace navi {

struct TrackData {
    std::string id;
    std::string name;
    std::string description;
    TrackType type;
    double start_time;
    double end_time;
    double total_distance;
    double total_duration;
    int point_count;
    std::vector<TrackPointData> points;
    
    TrackData()
        : type(TrackType::RealTime)
        , start_time(0.0)
        , end_time(0.0)
        , total_distance(0.0)
        , total_duration(0.0)
        , point_count(0)
    {}
};

class OGC_NAVI_API Track {
public:
    static Track* Create();
    static Track* Create(const TrackData& data);
    
    virtual ~Track() = default;
    
    virtual const std::string& GetId() const = 0;
    virtual void SetId(const std::string& id) = 0;
    
    virtual const std::string& GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;
    
    virtual const std::string& GetDescription() const = 0;
    virtual void SetDescription(const std::string& desc) = 0;
    
    virtual TrackType GetType() const = 0;
    virtual void SetType(TrackType type) = 0;
    
    virtual double GetStartTime() const = 0;
    virtual double GetEndTime() const = 0;
    virtual double GetTotalDistance() const = 0;
    virtual double GetTotalDuration() const = 0;
    
    virtual int GetPointCount() const = 0;
    virtual TrackPoint* GetPoint(int index) = 0;
    virtual const TrackPoint* GetPoint(int index) const = 0;
    
    virtual void AddPoint(TrackPoint* point) = 0;
    virtual void InsertPoint(int index, TrackPoint* point) = 0;
    virtual void RemovePoint(int index) = 0;
    virtual void ClearPoints() = 0;
    
    virtual void Simplify(double tolerance) = 0;
    virtual std::vector<TrackPoint*> GetPointsInTimeRange(double start, double end) = 0;
    virtual std::vector<TrackPoint*> GetPointsInBoundingBox(const BoundingBox& bbox) = 0;
    
    virtual TrackData ToData() const = 0;
    virtual void FromData(const TrackData& data) = 0;
    
    virtual void ReleaseReference() = 0;
};

}
}
