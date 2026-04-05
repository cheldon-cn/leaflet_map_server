#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>

namespace ogc {
namespace navi {

struct TrackPointData {
    double timestamp;
    double longitude;
    double latitude;
    double altitude;
    double heading;
    double speed;
    double course;
    double hdop;
    int satellite_count;
    PositionQuality quality;
    
    TrackPointData()
        : timestamp(0.0)
        , longitude(0.0)
        , latitude(0.0)
        , altitude(0.0)
        , heading(0.0)
        , speed(0.0)
        , course(0.0)
        , hdop(99.9)
        , satellite_count(0)
        , quality(PositionQuality::Invalid)
    {}
};

class OGC_NAVI_API TrackPoint {
public:
    static TrackPoint* Create();
    static TrackPoint* Create(const TrackPointData& data);
    
    virtual ~TrackPoint() = default;
    
    virtual double GetTimestamp() const = 0;
    virtual void SetTimestamp(double timestamp) = 0;
    
    virtual double GetLongitude() const = 0;
    virtual void SetLongitude(double lon) = 0;
    
    virtual double GetLatitude() const = 0;
    virtual void SetLatitude(double lat) = 0;
    
    virtual GeoPoint GetPosition() const = 0;
    virtual void SetPosition(double lon, double lat) = 0;
    
    virtual double GetAltitude() const = 0;
    virtual void SetAltitude(double altitude) = 0;
    
    virtual double GetHeading() const = 0;
    virtual void SetHeading(double heading) = 0;
    
    virtual double GetSpeed() const = 0;
    virtual void SetSpeed(double speed) = 0;
    
    virtual double GetCourse() const = 0;
    virtual void SetCourse(double course) = 0;
    
    virtual double GetHdop() const = 0;
    virtual void SetHdop(double hdop) = 0;
    
    virtual int GetSatelliteCount() const = 0;
    virtual void SetSatelliteCount(int count) = 0;
    
    virtual PositionQuality GetQuality() const = 0;
    virtual void SetQuality(PositionQuality quality) = 0;
    
    virtual TrackPointData ToData() const = 0;
    virtual void FromData(const TrackPointData& data) = 0;
    
    virtual void ReleaseReference() = 0;
};

}
}
