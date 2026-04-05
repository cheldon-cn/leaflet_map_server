#include "ogc/navi/route/route_manager.h"
#include <fstream>
#include <map>

namespace ogc {
namespace navi {

struct RouteManager::Impl {
    std::map<std::string, Route*> routes;
    Route* active_route = nullptr;
    int route_counter = 0;
    bool initialized = false;
};

RouteManager::RouteManager()
    : impl_(new Impl())
{
}

RouteManager::~RouteManager() {
    Shutdown();
}

RouteManager& RouteManager::Instance() {
    static RouteManager instance;
    return instance;
}

bool RouteManager::Initialize() {
    if (impl_->initialized) {
        return true;
    }
    impl_->initialized = true;
    return true;
}

void RouteManager::Shutdown() {
    for (auto& pair : impl_->routes) {
        if (pair.second) {
            pair.second->ReleaseReference();
        }
    }
    impl_->routes.clear();
    impl_->active_route = nullptr;
    impl_->initialized = false;
}

Route* RouteManager::CreateRoute(const std::string& name) {
    Route* route = Route::Create();
    route->SetName(name);
    
    impl_->route_counter++;
    std::string id = "route-" + std::to_string(impl_->route_counter);
    route->SetId(id);
    
    impl_->routes[id] = route;
    return route;
}

Route* RouteManager::LoadRoute(const std::string& route_id) {
    auto it = impl_->routes.find(route_id);
    if (it != impl_->routes.end()) {
        return it->second;
    }
    return nullptr;
}

bool RouteManager::SaveRoute(Route* route) {
    if (!route) {
        return false;
    }
    
    const std::string& id = route->GetId();
    if (id.empty()) {
        impl_->route_counter++;
        route->SetId("route-" + std::to_string(impl_->route_counter));
    }
    
    impl_->routes[route->GetId()] = route;
    return true;
}

bool RouteManager::DeleteRoute(const std::string& route_id) {
    auto it = impl_->routes.find(route_id);
    if (it == impl_->routes.end()) {
        return false;
    }
    
    if (impl_->active_route == it->second) {
        impl_->active_route = nullptr;
    }
    
    if (it->second) {
        it->second->ReleaseReference();
    }
    impl_->routes.erase(it);
    return true;
}

int RouteManager::GetRouteCount() const {
    return static_cast<int>(impl_->routes.size());
}

std::vector<Route*> RouteManager::GetAllRoutes() {
    std::vector<Route*> result;
    for (auto& pair : impl_->routes) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<Route*> RouteManager::GetActiveRoutes() {
    std::vector<Route*> result;
    if (impl_->active_route) {
        result.push_back(impl_->active_route);
    }
    return result;
}

Route* RouteManager::GetRoute(const std::string& route_id) const {
    auto it = impl_->routes.find(route_id);
    if (it != impl_->routes.end()) {
        return it->second;
    }
    return nullptr;
}

Route* RouteManager::GetActiveRoute() const {
    return impl_->active_route;
}

void RouteManager::SetActiveRoute(Route* route) {
    impl_->active_route = route;
}

bool RouteManager::ExportRoute(Route* route, const std::string& file_path) {
    if (!route) {
        return false;
    }
    
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    RouteData data = route->ToData();
    
    file << "[Route]\n";
    file << "id=" << data.id << "\n";
    file << "name=" << data.name << "\n";
    file << "description=" << data.description << "\n";
    file << "departure=" << data.departure << "\n";
    file << "destination=" << data.destination << "\n";
    file << "total_distance=" << data.total_distance << "\n";
    
    file << "\n[Waypoints]\n";
    file << "count=" << data.waypoints.size() << "\n";
    
    for (size_t i = 0; i < data.waypoints.size(); ++i) {
        const auto& wp = data.waypoints[i];
        file << "\n[Waypoint_" << i << "]\n";
        file << "id=" << wp.id << "\n";
        file << "name=" << wp.name << "\n";
        file << "longitude=" << wp.longitude << "\n";
        file << "latitude=" << wp.latitude << "\n";
        file << "arrival_radius=" << wp.arrival_radius << "\n";
        file << "speed_limit=" << wp.speed_limit << "\n";
        file << "sequence=" << wp.sequence << "\n";
    }
    
    file.close();
    return true;
}

Route* RouteManager::ImportRoute(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return nullptr;
    }
    
    RouteData data;
    std::string line;
    std::string section;
    int wp_index = -1;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (line[0] == '[') {
            section = line;
            if (line.find("[Waypoint_") != std::string::npos) {
                size_t start = line.find('_');
                size_t end = line.find(']');
                if (start != std::string::npos && end != std::string::npos) {
                    wp_index = std::stoi(line.substr(start + 1, end - start - 1));
                    if (wp_index >= static_cast<int>(data.waypoints.size())) {
                        data.waypoints.resize(wp_index + 1);
                    }
                }
            }
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (section == "[Route]") {
            if (key == "id") data.id = value;
            else if (key == "name") data.name = value;
            else if (key == "description") data.description = value;
            else if (key == "departure") data.departure = value;
            else if (key == "destination") data.destination = value;
            else if (key == "total_distance") data.total_distance = std::stod(value);
        }
        else if (wp_index >= 0 && wp_index < static_cast<int>(data.waypoints.size())) {
            auto& wp = data.waypoints[wp_index];
            if (key == "id") wp.id = value;
            else if (key == "name") wp.name = value;
            else if (key == "longitude") wp.longitude = std::stod(value);
            else if (key == "latitude") wp.latitude = std::stod(value);
            else if (key == "arrival_radius") wp.arrival_radius = std::stod(value);
            else if (key == "speed_limit") wp.speed_limit = std::stod(value);
            else if (key == "sequence") wp.sequence = std::stoi(value);
        }
    }
    
    file.close();
    
    Route* route = Route::Create(data);
    SaveRoute(route);
    return route;
}

bool RouteManager::ValidateRoute(Route* route, std::vector<std::string>& errors) {
    if (!route) {
        errors.push_back("Route is null");
        return false;
    }
    
    bool valid = true;
    
    if (route->GetName().empty()) {
        errors.push_back("Route name is empty");
        valid = false;
    }
    
    int wp_count = route->GetWaypointCount();
    if (wp_count < 2) {
        errors.push_back("Route must have at least 2 waypoints");
        valid = false;
    }
    
    for (int i = 0; i < wp_count; ++i) {
        const Waypoint* wp = route->GetWaypoint(i);
        if (!wp) {
            errors.push_back("Waypoint " + std::to_string(i) + " is null");
            valid = false;
        }
    }
    
    return valid;
}

}
}
