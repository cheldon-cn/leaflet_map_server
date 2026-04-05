#include "ogc/navi/navigation/navigation_engine.h"
#include "ogc/navi/route/waypoint.h"
#include <map>
#include <cmath>

namespace ogc {
namespace navi {

struct NavigationEngine::Impl {
    Route* active_route = nullptr;
    int current_waypoint_index = 0;
    NavigationStatus status = NavigationStatus::Inactive;
    NavigationData current_data;
    bool initialized = false;
    
    NavigationUpdateCallback update_callback;
    WaypointCallback waypoint_callback;
    AlertCallback alert_callback;
    
    double off_course_threshold = 0.5;
    double waypoint_approach_radius = 1.0;
    double waypoint_arrival_radius = 0.25;
};

NavigationEngine::NavigationEngine()
    : impl_(new Impl())
{
}

NavigationEngine::~NavigationEngine() {
    Shutdown();
}

NavigationEngine& NavigationEngine::Instance() {
    static NavigationEngine instance;
    return instance;
}

bool NavigationEngine::Initialize() {
    if (impl_->initialized) {
        return true;
    }
    
    impl_->initialized = true;
    return true;
}

void NavigationEngine::Shutdown() {
    StopNavigation();
    impl_->initialized = false;
}

bool NavigationEngine::StartNavigation(Route* route) {
    if (!route) {
        return false;
    }
    
    if (route->GetWaypointCount() < 2) {
        return false;
    }
    
    impl_->active_route = route;
    impl_->current_waypoint_index = 0;
    impl_->status = NavigationStatus::Active;
    
    return true;
}

void NavigationEngine::StopNavigation() {
    impl_->active_route = nullptr;
    impl_->current_waypoint_index = 0;
    impl_->status = NavigationStatus::Inactive;
}

void NavigationEngine::PauseNavigation() {
    if (impl_->status == NavigationStatus::Active) {
        impl_->status = NavigationStatus::Paused;
    }
}

void NavigationEngine::ResumeNavigation() {
    if (impl_->status == NavigationStatus::Paused) {
        impl_->status = NavigationStatus::Active;
    }
}

bool NavigationEngine::IsNavigating() const {
    return impl_->status == NavigationStatus::Active ||
           impl_->status == NavigationStatus::Paused;
}

NavigationStatus NavigationEngine::GetStatus() const {
    return impl_->status;
}

NavigationData NavigationEngine::GetCurrentData() const {
    return impl_->current_data;
}

Route* NavigationEngine::GetActiveRoute() const {
    return impl_->active_route;
}

int NavigationEngine::GetCurrentWaypointIndex() const {
    return impl_->current_waypoint_index;
}

int NavigationEngine::GetNextWaypointIndex() const {
    if (!impl_->active_route) {
        return -1;
    }
    
    int next_index = impl_->current_waypoint_index + 1;
    if (next_index >= impl_->active_route->GetWaypointCount()) {
        return -1;
    }
    
    return next_index;
}

void NavigationEngine::SkipWaypoint() {
    if (!impl_->active_route) {
        return;
    }
    
    int next_index = GetNextWaypointIndex();
    if (next_index >= 0) {
        impl_->current_waypoint_index = next_index;
        
        if (impl_->waypoint_callback && impl_->active_route) {
            const Waypoint* wp = impl_->active_route->GetWaypoint(next_index);
            if (wp) {
                impl_->waypoint_callback(next_index, wp->GetName());
            }
        }
    }
}

void NavigationEngine::GoToWaypoint(int index) {
    if (!impl_->active_route) {
        return;
    }
    
    if (index >= 0 && index < impl_->active_route->GetWaypointCount()) {
        impl_->current_waypoint_index = index;
        
        if (impl_->waypoint_callback && impl_->active_route) {
            const Waypoint* wp = impl_->active_route->GetWaypoint(index);
            if (wp) {
                impl_->waypoint_callback(index, wp->GetName());
            }
        }
    }
}

void NavigationEngine::ReverseRoute() {
    if (!impl_->active_route) {
        return;
    }
    
    int wp_count = impl_->active_route->GetWaypointCount();
    impl_->current_waypoint_index = wp_count - 1 - impl_->current_waypoint_index;
}

void NavigationEngine::SetNavigationUpdateCallback(NavigationUpdateCallback callback) {
    impl_->update_callback = callback;
}

void NavigationEngine::SetWaypointCallback(WaypointCallback callback) {
    impl_->waypoint_callback = callback;
}

void NavigationEngine::SetAlertCallback(AlertCallback callback) {
    impl_->alert_callback = callback;
}

void NavigationEngine::SetOffCourseThreshold(double threshold_nm) {
    impl_->off_course_threshold = threshold_nm;
}

void NavigationEngine::SetWaypointApproachRadius(double radius_nm) {
    impl_->waypoint_approach_radius = radius_nm;
}

void NavigationEngine::SetWaypointArrivalRadius(double radius_nm) {
    impl_->waypoint_arrival_radius = radius_nm;
}

bool NavigationEngine::SaveState(const std::string& file_path) {
    return false;
}

bool NavigationEngine::LoadState(const std::string& file_path) {
    return false;
}

void NavigationEngine::OnPositionUpdate(const PositionData& position) {
    if (!IsNavigating()) {
        return;
    }
    
    UpdateNavigation(position);
    CheckWaypointArrival(position);
}

void NavigationEngine::UpdateNavigation(const PositionData& position) {
    if (!impl_->active_route) {
        return;
    }
    
    const Waypoint* current_wp = impl_->active_route->GetWaypoint(
        impl_->current_waypoint_index);
    if (!current_wp) {
        return;
    }
    
    GeoPoint current_pos(position.longitude, position.latitude);
    GeoPoint wp_pos = current_wp->GetPosition();
    
    double distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        current_pos, wp_pos);
    impl_->current_data.distance_to_waypoint = distance;
    
    double bearing = NavigationCalculator::Instance().CalculateBearingToWaypoint(
        current_pos, wp_pos);
    impl_->current_data.bearing_to_waypoint = bearing;
    
    if (position.speed > 0.1) {
        double time_seconds = distance / (position.speed * 0.514444);
        impl_->current_data.time_to_waypoint = time_seconds / 60.0;
    } else {
        impl_->current_data.time_to_waypoint = 0.0;
    }
    
    int next_idx = GetNextWaypointIndex();
    if (next_idx >= 0) {
        const Waypoint* prev_wp = impl_->active_route->GetWaypoint(
            impl_->current_waypoint_index > 0 ? 
            impl_->current_waypoint_index - 1 : 0);
        const Waypoint* next_wp = impl_->active_route->GetWaypoint(next_idx);
        
        if (prev_wp && next_wp) {
            GeoPoint prev_pos = prev_wp->GetPosition();
            GeoPoint next_pos = next_wp->GetPosition();
            
            double xtd = NavigationCalculator::Instance().CalculateCrossTrackError(
                current_pos, prev_pos, next_pos);
            impl_->current_data.cross_track_error = xtd;
        }
    }
    
    if (impl_->update_callback) {
        impl_->update_callback(impl_->current_data);
    }
}

void NavigationEngine::CheckWaypointArrival(const PositionData& position) {
    if (!impl_->active_route) {
        return;
    }
    
    const Waypoint* current_wp = impl_->active_route->GetWaypoint(
        impl_->current_waypoint_index);
    if (!current_wp) {
        return;
    }
    
    GeoPoint current_pos(position.longitude, position.latitude);
    GeoPoint wp_pos = current_wp->GetPosition();
    
    double distance = NavigationCalculator::Instance().CalculateDistanceToWaypoint(
        current_pos, wp_pos);
    double distance_nm = distance / 1852.0;
    
    if (distance_nm <= impl_->waypoint_arrival_radius) {
        int next_idx = GetNextWaypointIndex();
        if (next_idx >= 0) {
            SkipWaypoint();
        } else {
            impl_->status = NavigationStatus::Completed;
        }
    }
}

void NavigationEngine::HandleOffCourse(const PositionData& position) {
    double xtd_nm = std::abs(impl_->current_data.cross_track_error) / 1852.0;
    
    if (xtd_nm > impl_->off_course_threshold) {
        if (impl_->alert_callback) {
            NavigationAlert alert;
            alert.type = AlertType::OffCourse;
            alert.level = xtd_nm > impl_->off_course_threshold * 2.0 ?
                AlertLevel::Critical : AlertLevel::Warning;
            alert.message = "Off course by " + std::to_string(xtd_nm) + " nm";
            alert.timestamp = position.timestamp;
            alert.position = GeoPoint(position.longitude, position.latitude);
            alert.value = xtd_nm;
            alert.threshold = impl_->off_course_threshold;
            impl_->alert_callback(alert);
        }
    }
}

}
}
