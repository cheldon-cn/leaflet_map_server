#include "ogc/navi/route/waypoint.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <atomic>
#include <map>

namespace ogc {
namespace navi {

class WaypointImpl : public Waypoint {
public:
    WaypointImpl()
        : longitude_(0.0)
        , latitude_(0.0)
        , arrival_radius_(0.5)
        , speed_limit_(0.0)
        , action_(WaypointAction::None)
        , sequence_(0)
        , is_mandatory_(true)
        , ref_count_(1)
    {}
    
    explicit WaypointImpl(const WaypointData& data)
        : id_(data.id)
        , name_(data.name)
        , longitude_(data.longitude)
        , latitude_(data.latitude)
        , arrival_radius_(data.arrival_radius)
        , speed_limit_(data.speed_limit)
        , action_(data.action)
        , notes_(data.notes)
        , sequence_(data.sequence)
        , is_mandatory_(data.is_mandatory)
        , ref_count_(1)
    {}
    
    ~WaypointImpl() override = default;
    
    const std::string& GetId() const override { return id_; }
    void SetId(const std::string& id) override { id_ = id; }
    
    const std::string& GetName() const override { return name_; }
    void SetName(const std::string& name) override { name_ = name; }
    
    double GetLongitude() const override { return longitude_; }
    void SetLongitude(double lon) override { longitude_ = lon; }
    
    double GetLatitude() const override { return latitude_; }
    void SetLatitude(double lat) override { latitude_ = lat; }
    
    GeoPoint GetPosition() const override { return GeoPoint(longitude_, latitude_); }
    void SetPosition(double lon, double lat) override { longitude_ = lon; latitude_ = lat; }
    
    double GetArrivalRadius() const override { return arrival_radius_; }
    void SetArrivalRadius(double radius) override { arrival_radius_ = radius; }
    
    double GetSpeedLimit() const override { return speed_limit_; }
    void SetSpeedLimit(double speed) override { speed_limit_ = speed; }
    
    WaypointAction GetAction() const override { return action_; }
    void SetAction(WaypointAction action) override { action_ = action; }
    
    int GetSequence() const override { return sequence_; }
    void SetSequence(int seq) override { sequence_ = seq; }
    
    bool IsMandatory() const override { return is_mandatory_; }
    void SetMandatory(bool mandatory) override { is_mandatory_ = mandatory; }
    
    const std::string& GetNotes() const override { return notes_; }
    void SetNotes(const std::string& notes) override { notes_ = notes; }
    
    WaypointData ToData() const override {
        WaypointData data;
        data.id = id_;
        data.name = name_;
        data.longitude = longitude_;
        data.latitude = latitude_;
        data.arrival_radius = arrival_radius_;
        data.speed_limit = speed_limit_;
        data.action = action_;
        data.notes = notes_;
        data.sequence = sequence_;
        data.is_mandatory = is_mandatory_;
        return data;
    }
    
    void FromData(const WaypointData& data) override {
        id_ = data.id;
        name_ = data.name;
        longitude_ = data.longitude;
        latitude_ = data.latitude;
        arrival_radius_ = data.arrival_radius;
        speed_limit_ = data.speed_limit;
        action_ = data.action;
        notes_ = data.notes;
        sequence_ = data.sequence;
        is_mandatory_ = data.is_mandatory;
    }
    
    void ReleaseReference() override {
        if (--ref_count_ == 0) {
            delete this;
        }
    }
    
private:
    std::string id_;
    std::string name_;
    double longitude_;
    double latitude_;
    double arrival_radius_;
    double speed_limit_;
    WaypointAction action_;
    std::string notes_;
    int sequence_;
    bool is_mandatory_;
    std::atomic<int> ref_count_;
};

Waypoint* Waypoint::Create() {
    return new WaypointImpl();
}

Waypoint* Waypoint::Create(const WaypointData& data) {
    return new WaypointImpl(data);
}

}
}
