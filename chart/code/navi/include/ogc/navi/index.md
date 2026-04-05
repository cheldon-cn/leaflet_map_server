# OGC Navigation Module Index

## Module Overview

The OGC Navigation module provides comprehensive navigation functionality for marine applications, including positioning, route planning, navigation guidance, track recording, and AIS integration.

## Submodules

### positioning
Positioning module handles GPS/GNSS data reception, parsing, and processing.

**Key Classes:**
- `NmeaParser` - NMEA sentence parser (singleton)
- `IPositionProvider` - Position data provider interface
- `PositionFilter` - Position data filter for outlier detection and smoothing
- `CoordinateConverter` - Coordinate transformation utilities (singleton)
- `PositionManager` - Central position management (singleton)

**Header Files:**
- `ogc/navi/positioning/nmea_parser.h`
- `ogc/navi/positioning/position_provider.h`
- `ogc/navi/positioning/position_filter.h`
- `ogc/navi/positioning/coordinate_converter.h`
- `ogc/navi/positioning/position_manager.h`

**Key Methods:**
- `NmeaParser::Instance().Parse(sentence, data)` - Parse NMEA sentence
- `CoordinateConverter::Instance().CalculateGreatCircleDistance()` - Calculate distance
- `CoordinateConverter::Instance().CalculateBearing()` - Calculate bearing
- `PositionManager::Instance().Initialize()` - Initialize positioning system

### route
Route module provides route planning, editing, and management capabilities.

**Key Classes:**
- `Waypoint` - Waypoint representation with arrival radius and actions
- `Route` - Route container with waypoints and distance calculations

**Header Files:**
- `ogc/navi/route/waypoint.h`
- `ogc/navi/route/route.h`

**Key Methods:**
- `Waypoint::Create()` - Create waypoint instance
- `Waypoint::GetPosition()` / `SetPosition()` - Get/set coordinates
- `Route::Create()` - Create route instance
- `Route::AddWaypoint()` - Add waypoint to route
- `Route::CalculateTotalDistance()` - Calculate total route distance
- `Route::CalculateLegBearing()` - Calculate leg bearing

### navigation
Navigation module provides real-time navigation guidance and alerts.

**Key Classes:**
- `NavigationCalculator` - Navigation calculations (singleton)
- `OffCourseDetector` - Off-course detection (singleton)

**Header Files:**
- `ogc/navi/navigation/navigation_calculator.h`
- `ogc/navi/navigation/off_course_detector.h`

**Key Methods:**
- `NavigationCalculator::Instance().Calculate()` - Calculate navigation data
- `NavigationCalculator::Instance().CalculateCrossTrackError()` - Calculate XTD
- `NavigationCalculator::Instance().CalculateDistanceToWaypoint()` - Calculate distance
- `OffCourseDetector::Instance().Detect()` - Detect off-course condition

### track
Track module handles trajectory recording, playback, and analysis.

**Key Classes:**
- `TrackPoint` - Single track point with position and time
- `Track` - Track container with points and statistics

**Header Files:**
- `ogc/navi/track/track_point.h`
- `ogc/navi/track/track.h`

**Key Methods:**
- `TrackPoint::Create()` - Create track point instance
- `Track::Create()` - Create track instance
- `Track::AddPoint()` - Add point to track
- `Track::Simplify()` - Simplify track using Douglas-Peucker algorithm
- `Track::GetPointsInTimeRange()` - Query points by time range

### ais
AIS module handles AIS data reception, parsing, and collision assessment.

**Key Classes:**
- `AisMessage` - AIS message types (position report, static data)
- `AisTarget` - AIS target representation with CPA/TCPA

**Header Files:**
- `ogc/navi/ais/ais_message.h`
- `ogc/navi/ais/ais_target.h`

**Key Methods:**
- `AisTarget::Create(mmsi)` - Create AIS target with MMSI
- `AisTarget::UpdatePositionReport()` - Update from position report
- `AisTarget::UpdateStaticData()` - Update from static data
- `AisTarget::SetCpa()` / `GetCpa()` - Set/get CPA value
- `AisTarget::HasCollisionRisk()` - Check collision risk

## Common Types

All common types are defined in `ogc/navi/types.h`:

**Enumerations:**
- `PositionQuality` - GPS fix quality (Invalid, Gps, DGps, Rtk, etc.)
- `PositionSource` - Position data source (Serial, File, System, etc.)
- `NavigationStatus` - Navigation status (Inactive, Active, Paused, etc.)
- `RouteStatus` - Route status (Planned, Active, Completed, etc.)
- `WaypointAction` - Waypoint action (None, Turn, Stop, etc.)
- `AlertLevel` - Alert severity level
- `AlertType` - Alert type enumeration
- `TrackType` - Track type (RealTime, Historical, Planned)
- `AisNavigationStatus` - AIS navigation status
- `AisShipType` - AIS ship type classification

**Structures:**
- `PositionData` - Position data with coordinates, speed, course
- `GeoPoint` - Geographic point (longitude, latitude)
- `BoundingBox` - Geographic bounding box

## Dependencies

- **ogc_geometry** - Geometry types and operations
- **ogc_database** - Database connectivity and storage

## Usage Examples

### Positioning

```cpp
#include <ogc/navi/positioning/position_manager.h>
#include <ogc/navi/positioning/nmea_parser.h>

using namespace ogc::navi;

// Parse NMEA sentence
NmeaParser& parser = NmeaParser::Instance();
PositionData pos;
if (parser.Parse("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47", pos)) {
    // Use position data
    double lat = pos.latitude;
    double lon = pos.longitude;
}

// Calculate distance
double distance = CoordinateConverter::Instance().CalculateGreatCircleDistance(
    31.0, 121.0,  // Start point
    32.0, 122.0   // End point
);

// Calculate bearing
double bearing = CoordinateConverter::Instance().CalculateBearing(
    31.0, 121.0,  // Start point
    32.0, 122.0   // End point
);
```

### Route Management

```cpp
#include <ogc/navi/route/waypoint.h>
#include <ogc/navi/route/route.h>

using namespace ogc::navi;

// Create route
Route* route = Route::Create();
route->SetName("Shanghai to Tokyo");
route->SetDeparture("Shanghai");
route->SetDestination("Tokyo");

// Create waypoints
Waypoint* wp1 = Waypoint::Create();
wp1->SetName("Start Point");
wp1->SetPosition(121.5, 31.2);
wp1->SetArrivalRadius(0.5);

Waypoint* wp2 = Waypoint::Create();
wp2->SetName("Waypoint 1");
wp2->SetPosition(122.0, 32.0);

// Add waypoints to route
route->AddWaypoint(wp1);
route->AddWaypoint(wp2);

// Calculate total distance
double total_distance = route->CalculateTotalDistance();

// Cleanup
route->ReleaseReference();
```

### Navigation Calculations

```cpp
#include <ogc/navi/navigation/navigation_calculator.h>
#include <ogc/navi/navigation/off_course_detector.h>

using namespace ogc::navi;

// Calculate navigation data
PositionData current_pos;
current_pos.longitude = 121.5;
current_pos.latitude = 31.5;
current_pos.speed = 10.0;
current_pos.course = 45.0;

GeoPoint waypoint(122.0, 32.0);

NavigationData nav_data = NavigationCalculator::Instance().Calculate(
    current_pos, waypoint, 10.0);

// Check off-course
GeoPoint route_start(121.0, 31.0);
GeoPoint route_end(122.0, 32.0);

OffCourseResult result = OffCourseDetector::Instance().Detect(
    current_pos, route_start, route_end);

if (result.is_off_course) {
    // Handle off-course situation
    double recommended_bearing = result.recommended_bearing;
}
```

### Track Recording

```cpp
#include <ogc/navi/track/track_point.h>
#include <ogc/navi/track/track.h>

using namespace ogc::navi;

// Create track
Track* track = Track::Create();
track->SetName("Morning Trip");
track->SetType(TrackType::RealTime);

// Add track points
TrackPoint* point1 = TrackPoint::Create();
point1->SetPosition(121.5, 31.5);
point1->SetTimestamp(1000.0);
point1->SetSpeed(10.0);

track->AddPoint(point1);

// Get statistics
double total_distance = track->GetTotalDistance();
double total_duration = track->GetTotalDuration();

// Simplify track
track->Simplify(10.0);  // 10 meter tolerance

// Cleanup
track->ReleaseReference();
```

### AIS Target Handling

```cpp
#include <ogc/navi/ais/ais_target.h>

using namespace ogc::navi;

// Create AIS target
AisTarget* target = AisTarget::Create(123456789);
target->SetShipName("Test Vessel");
target->SetPosition(121.5, 31.5);
target->SetSpeedOverGround(12.0);
target->SetCourseOverGround(90.0);

// Update from AIS position report
AisPositionReport report;
report.mmsi = 123456789;
report.longitude = 121.6;
report.latitude = 31.6;
report.speed_over_ground = 12.5;
report.course_over_ground = 95.0;
target->UpdatePositionReport(report);

// Set collision risk data
target->SetCpa(500.0);   // 500 meters
target->SetTcpa(300.0);  // 5 minutes
target->SetCollisionRisk(true);

// Check collision risk
if (target->HasCollisionRisk()) {
    // Handle collision warning
}

// Cleanup
target->ReleaseReference();
```

## Build Configuration

The module is built as a shared library (ogc_navi.dll) with the following CMake configuration:

```cmake
# Build output directories
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"

# For MSVC multi-config generators
RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
```

## Test Coverage

The module includes comprehensive unit tests for all components:

| Component | Test File | Coverage |
|-----------|-----------|----------|
| NmeaParser | test_nmea_parser.cpp | GGA/RMC/VTG/GSA/GSV parsing |
| PositionFilter | test_position_filter.cpp | Outlier detection, smoothing |
| CoordinateConverter | test_coordinate_converter.cpp | Distance, bearing, projection |
| Waypoint | test_waypoint.cpp | CRUD operations, serialization |
| Route | test_route.cpp | Waypoint management, calculations |
| NavigationCalculator | test_navigation_calculator.cpp | XTD, TTG, ETA, VMG |
| OffCourseDetector | test_off_course_detector.cpp | Detection thresholds |
| TrackPoint | test_track_point.cpp | Position, time, serialization |
| Track | test_track.cpp | Point management, simplification |
| AisTarget | test_ais_target.cpp | Position updates, collision risk |

## Version

- **Version**: 1.0.0
- **C++ Standard**: C++11
- **Last Updated**: 2026-04-05
