#include "ogc/navi/ais/ais_target.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <cmath>
#include <atomic>

namespace ogc {
namespace navi {

namespace {
    const double KNOTS_TO_METERS_PER_SECOND = 0.514444;
    const double PI = 3.14159265358979323846;
}

class AisTargetImpl : public AisTarget {
public:
    AisTargetImpl() 
        : mmsi_(0)
        , latitude_(0.0)
        , longitude_(0.0)
        , speed_over_ground_(0.0)
        , course_over_ground_(0.0)
        , heading_(0.0)
        , ship_type_(AisShipType::NotAvailable)
        , nav_status_(AisNavigationStatus::NotAvailable)
        , ship_length_(0)
        , ship_width_(0)
        , draught_(0.0)
        , last_update_(0.0)
        , cpa_(0.0)
        , tcpa_(0.0)
        , collision_risk_(false)
        , ref_count_(1)
    {}
    
    explicit AisTargetImpl(uint32_t mmsi)
        : mmsi_(mmsi)
        , latitude_(0.0)
        , longitude_(0.0)
        , speed_over_ground_(0.0)
        , course_over_ground_(0.0)
        , heading_(0.0)
        , ship_type_(AisShipType::NotAvailable)
        , nav_status_(AisNavigationStatus::NotAvailable)
        , ship_length_(0)
        , ship_width_(0)
        , draught_(0.0)
        , last_update_(0.0)
        , cpa_(0.0)
        , tcpa_(0.0)
        , collision_risk_(false)
        , ref_count_(1)
    {}
    
    ~AisTargetImpl() override = default;
    
    uint32_t GetMmsi() const override { return mmsi_; }
    
    const std::string& GetShipName() const override { return ship_name_; }
    void SetShipName(const std::string& name) override { ship_name_ = name; }
    
    const std::string& GetCallsign() const override { return callsign_; }
    void SetCallsign(const std::string& callsign) override { callsign_ = callsign; }
    
    AisShipType GetShipType() const override { return ship_type_; }
    void SetShipType(AisShipType type) override { ship_type_ = type; }
    
    AisNavigationStatus GetNavigationStatus() const override { return nav_status_; }
    void SetNavigationStatus(AisNavigationStatus status) override { nav_status_ = status; }
    
    double GetLongitude() const override { return longitude_; }
    void SetLongitude(double lon) override { longitude_ = lon; }
    
    double GetLatitude() const override { return latitude_; }
    void SetLatitude(double lat) override { latitude_ = lat; }
    
    GeoPoint GetPosition() const override { return GeoPoint(longitude_, latitude_); }
    void SetPosition(double lon, double lat) override { longitude_ = lon; latitude_ = lat; }
    
    double GetSpeedOverGround() const override { return speed_over_ground_; }
    void SetSpeedOverGround(double speed) override { speed_over_ground_ = speed; }
    
    double GetCourseOverGround() const override { return course_over_ground_; }
    void SetCourseOverGround(double course) override { course_over_ground_ = course; }
    
    double GetHeading() const override { return heading_; }
    void SetHeading(double heading) override { heading_ = heading; }
    
    int GetShipLength() const override { return ship_length_; }
    void SetShipLength(int length) override { ship_length_ = length; }
    
    int GetShipWidth() const override { return ship_width_; }
    void SetShipWidth(int width) override { ship_width_ = width; }
    
    double GetDraught() const override { return draught_; }
    void SetDraught(double draught) override { draught_ = draught; }
    
    const std::string& GetDestination() const override { return destination_; }
    void SetDestination(const std::string& destination) override { destination_ = destination; }
    
    double GetLastUpdate() const override { return last_update_; }
    void SetLastUpdate(double timestamp) override { last_update_ = timestamp; }
    
    double GetCpa() const override { return cpa_; }
    void SetCpa(double cpa) override { cpa_ = cpa; }
    
    double GetTcpa() const override { return tcpa_; }
    void SetTcpa(double tcpa) override { tcpa_ = tcpa; }
    
    bool HasCollisionRisk() const override { return collision_risk_; }
    void SetCollisionRisk(bool risk) override { collision_risk_ = risk; }
    
    void UpdatePositionReport(const AisPositionReport& report) override {
        longitude_ = report.longitude;
        latitude_ = report.latitude;
        speed_over_ground_ = report.speed_over_ground;
        course_over_ground_ = report.course_over_ground;
        heading_ = report.heading;
        nav_status_ = report.nav_status;
        last_update_ = report.timestamp;
    }
    
    void UpdateStaticData(const AisStaticData& data) override {
        ship_name_ = data.ship_name;
        callsign_ = data.callsign;
        ship_type_ = data.ship_type;
        ship_length_ = data.dimension_to_bow + data.dimension_to_stern;
        ship_width_ = data.dimension_to_port + data.dimension_to_starboard;
        draught_ = data.draught;
        destination_ = data.destination;
    }
    
    AisTargetData ToData() const override {
        AisTargetData data;
        data.mmsi = mmsi_;
        data.ship_name = ship_name_;
        data.callsign = callsign_;
        data.ship_type = ship_type_;
        data.nav_status = nav_status_;
        data.longitude = longitude_;
        data.latitude = latitude_;
        data.speed_over_ground = speed_over_ground_;
        data.course_over_ground = course_over_ground_;
        data.heading = heading_;
        data.ship_length = ship_length_;
        data.ship_width = ship_width_;
        data.draught = draught_;
        data.destination = destination_;
        data.last_update = last_update_;
        data.cpa = cpa_;
        data.tcpa = tcpa_;
        data.collision_risk = collision_risk_;
        return data;
    }
    
    void FromData(const AisTargetData& data) override {
        mmsi_ = data.mmsi;
        ship_name_ = data.ship_name;
        callsign_ = data.callsign;
        ship_type_ = data.ship_type;
        nav_status_ = data.nav_status;
        longitude_ = data.longitude;
        latitude_ = data.latitude;
        speed_over_ground_ = data.speed_over_ground;
        course_over_ground_ = data.course_over_ground;
        heading_ = data.heading;
        ship_length_ = data.ship_length;
        ship_width_ = data.ship_width;
        draught_ = data.draught;
        destination_ = data.destination;
        last_update_ = data.last_update;
        cpa_ = data.cpa;
        tcpa_ = data.tcpa;
        collision_risk_ = data.collision_risk;
    }
    
    void ReleaseReference() override {
        if (--ref_count_ == 0) {
            delete this;
        }
    }
    
private:
    uint32_t mmsi_;
    std::string ship_name_;
    std::string callsign_;
    AisShipType ship_type_;
    AisNavigationStatus nav_status_;
    double longitude_;
    double latitude_;
    double speed_over_ground_;
    double course_over_ground_;
    double heading_;
    int ship_length_;
    int ship_width_;
    double draught_;
    std::string destination_;
    double last_update_;
    double cpa_;
    double tcpa_;
    bool collision_risk_;
    std::atomic<int> ref_count_;
};

AisTarget* AisTarget::Create() {
    return new AisTargetImpl();
}

AisTarget* AisTarget::Create(uint32_t mmsi) {
    return new AisTargetImpl(mmsi);
}

}
}
