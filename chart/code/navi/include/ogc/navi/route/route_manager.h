#pragma once

#include "ogc/navi/route/route.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace navi {

class OGC_NAVI_API RouteManager {
public:
    static RouteManager& Instance();
    
    bool Initialize();
    void Shutdown();
    
    Route* CreateRoute(const std::string& name);
    Route* LoadRoute(const std::string& route_id);
    bool SaveRoute(Route* route);
    bool DeleteRoute(const std::string& route_id);
    
    int GetRouteCount() const;
    std::vector<Route*> GetAllRoutes();
    std::vector<Route*> GetActiveRoutes();
    
    Route* GetRoute(const std::string& route_id) const;
    Route* GetActiveRoute() const;
    void SetActiveRoute(Route* route);
    
    bool ExportRoute(Route* route, const std::string& file_path);
    Route* ImportRoute(const std::string& file_path);
    
    bool ValidateRoute(Route* route, std::vector<std::string>& errors);
    
private:
    RouteManager();
    ~RouteManager();
    RouteManager(const RouteManager&) = delete;
    RouteManager& operator=(const RouteManager&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}
