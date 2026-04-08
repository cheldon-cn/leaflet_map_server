#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/route/route.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include "ogc/navi/navigation/off_course_detector.h"
#include "ogc/navi/navigation/navigation_alerter.h"
#include "ogc/navi/positioning/position_manager.h"
#include "ogc/navi/export.h"
#include <memory>
#include <functional>

namespace ogc {
namespace navi {

using NavigationUpdateCallback = std::function<void(const NavigationData&)>;
using WaypointCallback = std::function<void(int waypoint_index, const std::string& waypoint_name)>;

class OGC_NAVI_API NavigationEngine {
public:
    static NavigationEngine& Instance();
    
    bool Initialize();
    void Shutdown();
    
    bool StartNavigation(Route* route);
    void StopNavigation();
    void PauseNavigation();
    void ResumeNavigation();
    
    bool IsNavigating() const;
    NavigationStatus GetStatus() const;
    NavigationData GetCurrentData() const;
    
    Route* GetActiveRoute() const;
    int GetCurrentWaypointIndex() const;
    int GetNextWaypointIndex() const;
    
    void SkipWaypoint();
    void GoToWaypoint(int index);
    void ReverseRoute();
    
    void SetNavigationUpdateCallback(NavigationUpdateCallback callback);
    void SetWaypointCallback(WaypointCallback callback);
    void SetAlertCallback(AlertCallback callback);
    
    void SetOffCourseThreshold(double threshold_nm);
    void SetWaypointApproachRadius(double radius_nm);
    void SetWaypointArrivalRadius(double radius_nm);
    
    bool SaveState(const std::string& file_path);
    bool LoadState(const std::string& file_path);
    
private:
    NavigationEngine();
    ~NavigationEngine();
    NavigationEngine(const NavigationEngine&) = delete;
    NavigationEngine& operator=(const NavigationEngine&) = delete;
    
    void OnPositionUpdate(const PositionData& position);
    void UpdateNavigation(const PositionData& position);
    void CheckWaypointArrival(const PositionData& position);
    void HandleOffCourse(const PositionData& position);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}
