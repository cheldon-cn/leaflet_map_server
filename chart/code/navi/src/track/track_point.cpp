#include "ogc/navi/track/track_point.h"
#include <atomic>

namespace ogc {
namespace navi {

class TrackPointImpl : public TrackPoint {
public:
    TrackPointImpl()
        : timestamp_(0.0)
        , longitude_(0.0)
        , latitude_(0.0)
        , altitude_(0.0)
        , heading_(0.0)
        , speed_(0.0)
        , course_(0.0)
        , hdop_(99.9)
        , satellite_count_(0)
        , quality_(PositionQuality::Invalid)
        , ref_count_(1)
    {}
    
    explicit TrackPointImpl(const TrackPointData& data)
        : timestamp_(data.timestamp)
        , longitude_(data.longitude)
        , latitude_(data.latitude)
        , altitude_(data.altitude)
        , heading_(data.heading)
        , speed_(data.speed)
        , course_(data.course)
        , hdop_(data.hdop)
        , satellite_count_(data.satellite_count)
        , quality_(data.quality)
        , ref_count_(1)
    {}
    
    ~TrackPointImpl() override = default;
    
    double GetTimestamp() const override { return timestamp_; }
    void SetTimestamp(double timestamp) override { timestamp_ = timestamp; }
    
    double GetLongitude() const override { return longitude_; }
    void SetLongitude(double lon) override { longitude_ = lon; }
    
    double GetLatitude() const override { return latitude_; }
    void SetLatitude(double lat) override { latitude_ = lat; }
    
    GeoPoint GetPosition() const override { return GeoPoint(longitude_, latitude_); }
    void SetPosition(double lon, double lat) override { longitude_ = lon; latitude_ = lat; }
    
    double GetAltitude() const override { return altitude_; }
    void SetAltitude(double altitude) override { altitude_ = altitude; }
    
    double GetHeading() const override { return heading_; }
    void SetHeading(double heading) override { heading_ = heading; }
    
    double GetSpeed() const override { return speed_; }
    void SetSpeed(double speed) override { speed_ = speed; }
    
    double GetCourse() const override { return course_; }
    void SetCourse(double course) override { course_ = course; }
    
    double GetHdop() const override { return hdop_; }
    void SetHdop(double hdop) override { hdop_ = hdop; }
    
    int GetSatelliteCount() const override { return satellite_count_; }
    void SetSatelliteCount(int count) override { satellite_count_ = count; }
    
    PositionQuality GetQuality() const override { return quality_; }
    void SetQuality(PositionQuality quality) override { quality_ = quality; }
    
    TrackPointData ToData() const override {
        TrackPointData data;
        data.timestamp = timestamp_;
        data.longitude = longitude_;
        data.latitude = latitude_;
        data.altitude = altitude_;
        data.heading = heading_;
        data.speed = speed_;
        data.course = course_;
        data.hdop = hdop_;
        data.satellite_count = satellite_count_;
        data.quality = quality_;
        return data;
    }
    
    void FromData(const TrackPointData& data) override {
        timestamp_ = data.timestamp;
        longitude_ = data.longitude;
        latitude_ = data.latitude;
        altitude_ = data.altitude;
        heading_ = data.heading;
        speed_ = data.speed;
        course_ = data.course;
        hdop_ = data.hdop;
        satellite_count_ = data.satellite_count;
        quality_ = data.quality;
    }
    
    void ReleaseReference() override {
        if (--ref_count_ == 0) {
            delete this;
        }
    }
    
private:
    double timestamp_;
    double longitude_;
    double latitude_;
    double altitude_;
    double heading_;
    double speed_;
    double course_;
    double hdop_;
    int satellite_count_;
    PositionQuality quality_;
    std::atomic<int> ref_count_;
};

TrackPoint* TrackPoint::Create() {
    return new TrackPointImpl();
}

TrackPoint* TrackPoint::Create(const TrackPointData& data) {
    return new TrackPointImpl(data);
}

}
}
