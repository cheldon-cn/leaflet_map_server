#include "ogc/navi/route/route.h"
#include "ogc/navi/positioning/coordinate_converter.h"
#include <algorithm>
#include <atomic>
#include <cmath>

namespace ogc {
namespace navi {

namespace {
    const double PI = 3.14159265358979323846;
}

class RouteImpl : public Route {
public:
    RouteImpl()
        : status_(RouteStatus::Planned)
        , total_distance_(0.0)
        , estimated_duration_(0.0)
        , created_time_(0.0)
        , modified_time_(0.0)
        , ref_count_(1)
    {}
    
    explicit RouteImpl(const RouteData& data)
        : id_(data.id)
        , name_(data.name)
        , description_(data.description)
        , status_(data.status)
        , total_distance_(data.total_distance)
        , estimated_duration_(data.estimated_duration)
        , departure_(data.departure)
        , destination_(data.destination)
        , created_time_(data.created_time)
        , modified_time_(data.modified_time)
        , ref_count_(1)
    {
        for (const auto& wp_data : data.waypoints) {
            Waypoint* wp = Waypoint::Create(wp_data);
            waypoints_.push_back(wp);
        }
    }
    
    ~RouteImpl() override {
        for (auto* wp : waypoints_) {
            wp->ReleaseReference();
        }
    }
    
    const std::string& GetId() const override { return id_; }
    void SetId(const std::string& id) override { id_ = id; }
    
    const std::string& GetName() const override { return name_; }
    void SetName(const std::string& name) override { name_ = name; }
    
    const std::string& GetDescription() const override { return description_; }
    void SetDescription(const std::string& desc) override { description_ = desc; }
    
    RouteStatus GetStatus() const override { return status_; }
    void SetStatus(RouteStatus status) override { status_ = status; }
    
    double GetTotalDistance() const override { return total_distance_; }
    double GetEstimatedDuration() const override { return estimated_duration_; }
    
    const std::string& GetDeparture() const override { return departure_; }
    void SetDeparture(const std::string& departure) override { departure_ = departure; }
    
    const std::string& GetDestination() const override { return destination_; }
    void SetDestination(const std::string& destination) override { destination_ = destination; }
    
    int GetWaypointCount() const override { return static_cast<int>(waypoints_.size()); }
    
    Waypoint* GetWaypoint(int index) override {
        if (index >= 0 && index < static_cast<int>(waypoints_.size())) {
            return waypoints_[index];
        }
        return nullptr;
    }
    
    const Waypoint* GetWaypoint(int index) const override {
        if (index >= 0 && index < static_cast<int>(waypoints_.size())) {
            return waypoints_[index];
        }
        return nullptr;
    }
    
    void AddWaypoint(Waypoint* waypoint) override {
        if (waypoint) {
            waypoint->SetSequence(static_cast<int>(waypoints_.size()));
            waypoints_.push_back(waypoint);
            UpdateDistances();
        }
    }
    
    void InsertWaypoint(int index, Waypoint* waypoint) override {
        if (waypoint && index >= 0 && index <= static_cast<int>(waypoints_.size())) {
            waypoints_.insert(waypoints_.begin() + index, waypoint);
            RenumberWaypoints();
            UpdateDistances();
        }
    }
    
    void RemoveWaypoint(int index) override {
        if (index >= 0 && index < static_cast<int>(waypoints_.size())) {
            waypoints_[index]->ReleaseReference();
            waypoints_.erase(waypoints_.begin() + index);
            RenumberWaypoints();
            UpdateDistances();
        }
    }
    
    void ClearWaypoints() override {
        for (auto* wp : waypoints_) {
            wp->ReleaseReference();
        }
        waypoints_.clear();
        total_distance_ = 0.0;
    }
    
    void Reverse() override {
        std::reverse(waypoints_.begin(), waypoints_.end());
        RenumberWaypoints();
        std::swap(departure_, destination_);
    }
    
    void Optimize() override {
        UpdateDistances();
    }
    
    double CalculateTotalDistance() override {
        UpdateDistances();
        return total_distance_;
    }
    
    double CalculateLegDistance(int leg_index) override {
        if (leg_index < 0 || leg_index >= static_cast<int>(waypoints_.size()) - 1) {
            return 0.0;
        }
        
        const Waypoint* start = waypoints_[leg_index];
        const Waypoint* end = waypoints_[leg_index + 1];
        
        return CoordinateConverter::Instance().CalculateGreatCircleDistance(
            start->GetLatitude(), start->GetLongitude(),
            end->GetLatitude(), end->GetLongitude());
    }
    
    double CalculateLegBearing(int leg_index) override {
        if (leg_index < 0 || leg_index >= static_cast<int>(waypoints_.size()) - 1) {
            return 0.0;
        }
        
        const Waypoint* start = waypoints_[leg_index];
        const Waypoint* end = waypoints_[leg_index + 1];
        
        return CoordinateConverter::Instance().CalculateBearing(
            start->GetLatitude(), start->GetLongitude(),
            end->GetLatitude(), end->GetLongitude());
    }
    
    RouteData ToData() const override {
        RouteData data;
        data.id = id_;
        data.name = name_;
        data.description = description_;
        data.status = status_;
        data.total_distance = total_distance_;
        data.estimated_duration = estimated_duration_;
        data.departure = departure_;
        data.destination = destination_;
        data.created_time = created_time_;
        data.modified_time = modified_time_;
        
        for (const auto* wp : waypoints_) {
            data.waypoints.push_back(wp->ToData());
        }
        
        return data;
    }
    
    void FromData(const RouteData& data) override {
        ClearWaypoints();
        
        id_ = data.id;
        name_ = data.name;
        description_ = data.description;
        status_ = data.status;
        total_distance_ = data.total_distance;
        estimated_duration_ = data.estimated_duration;
        departure_ = data.departure;
        destination_ = data.destination;
        created_time_ = data.created_time;
        modified_time_ = data.modified_time;
        
        for (const auto& wp_data : data.waypoints) {
            Waypoint* wp = Waypoint::Create(wp_data);
            waypoints_.push_back(wp);
        }
    }
    
    void ReleaseReference() override {
        if (--ref_count_ == 0) {
            delete this;
        }
    }
    
private:
    void UpdateDistances() {
        total_distance_ = 0.0;
        for (size_t i = 0; i + 1 < waypoints_.size(); ++i) {
            total_distance_ += CoordinateConverter::Instance().CalculateGreatCircleDistance(
                waypoints_[i]->GetLatitude(), waypoints_[i]->GetLongitude(),
                waypoints_[i + 1]->GetLatitude(), waypoints_[i + 1]->GetLongitude());
        }
    }
    
    void RenumberWaypoints() {
        for (size_t i = 0; i < waypoints_.size(); ++i) {
            waypoints_[i]->SetSequence(static_cast<int>(i));
        }
    }
    
    std::string id_;
    std::string name_;
    std::string description_;
    RouteStatus status_;
    double total_distance_;
    double estimated_duration_;
    std::string departure_;
    std::string destination_;
    std::vector<Waypoint*> waypoints_;
    double created_time_;
    double modified_time_;
    std::atomic<int> ref_count_;
};

Route* Route::Create() {
    return new RouteImpl();
}

Route* Route::Create(const RouteData& data) {
    return new RouteImpl(data);
}

}
}
