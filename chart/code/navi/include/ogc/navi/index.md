# OGC Navigation Module - API Reference

**Version**: 1.0.0  
**C++ Standard**: C++11  
**Last Updated**: 2026-04-05

---

## Table of Contents

1. [Module Overview](#module-overview)
2. [Common Types](#common-types)
3. [Positioning Module](#positioning-module)
4. [Route Module](#route-module)
5. [Navigation Module](#navigation-module)
6. [Track Module](#track-module)
7. [AIS Module](#ais-module)
8. [Usage Examples](#usage-examples)

---

## Module Overview

The OGC Navigation module provides comprehensive navigation functionality for marine applications.

```
ogc::navi
├── positioning    # GPS/GNSS data processing
├── route          # Route planning and management
├── navigation     # Navigation guidance
├── track          # Trajectory recording
└── ais            # AIS integration
```

---

## Common Types

All common types are defined in `ogc/navi/types.h`.

### Enumerations

#### PositionQuality

GPS fix quality indicator.

```cpp
enum class PositionQuality {
    Invalid = 0,      // Invalid fix
    Gps = 1,          // GPS fix
    DGps = 2,         // Differential GPS
    Pps = 3,          // PPS fix
    Rtk = 4,          // RTK fixed solution
    FloatRtk = 5,     // RTK float solution
    Simulation = 6,   // Simulation mode
    ManualInput = 7   // Manual input
};
```

#### PositionSource

Position data source.

```cpp
enum class PositionSource {
    Unknown,          // Unknown source
    Serial,           // Serial port
    File,             // File playback
    System,           // System GPS
    Network,          // Network source
    Simulation        // Simulation
};
```

#### NavigationStatus

Navigation engine status.

```cpp
enum class NavigationStatus {
    Inactive = 0,     // Not active
    Active = 1,       // Active navigation
    Paused = 2,       // Paused
    Completed = 3     // Route completed
};
```

#### WaypointAction

Action to take at waypoint.

```cpp
enum class WaypointAction {
    None = 0,         // No action
    Turn = 1,         // Turn
    Stop = 2,         // Stop
    Alert = 3         // Alert only
};
```

#### AlertLevel

Alert severity level.

```cpp
enum class AlertLevel {
    None = 0,         // No alert
    Info = 1,         // Information
    Warning = 2,      // Warning
    Alarm = 3         // Alarm
};
```

#### AisShipType

AIS ship type classification.

```cpp
enum class AisShipType {
    NotAvailable = 0,
    Reserved = 1,
    WingInGround = 20,
    Fishing = 30,
    Towing = 31,
    Dredging = 33,
    DivingOps = 34,
    MilitaryOps = 35,
    Sailing = 36,
    PleasureCraft = 37,
    HighSpeedCraft = 40,
    PilotVessel = 50,
    SearchAndRescue = 51,
    Tug = 52,
    PortTender = 53,
    AntiPollution = 54,
    LawEnforcement = 55,
    MedicalTransport = 58,
    Passenger = 60,
    Cargo = 70,
    Tanker = 80,
    Other = 90
};
```

### Structures

#### PositionData

Complete position information.

```cpp
struct PositionData {
    double timestamp;           // UTC timestamp
    double longitude;           // Longitude (degrees)
    double latitude;            // Latitude (degrees)
    double altitude;            // Altitude (meters)
    double heading;             // Heading (degrees)
    double speed;               // Speed (knots)
    double course;              // Course over ground (degrees)
    double hdop;                // Horizontal dilution of precision
    double vdop;                // Vertical dilution of precision
    double pdop;                // Position dilution of precision
    int satellite_count;        // Number of satellites
    int gps_quality;            // GPS quality indicator
    PositionQuality quality;    // Position quality enum
    PositionSource source;      // Data source
    uint32_t mmsi;              // MMSI (for AIS)
    bool valid;                 // Data validity flag
};
```

#### GeoPoint

Geographic coordinate point.

```cpp
struct GeoPoint {
    double longitude;           // Longitude (degrees)
    double latitude;            // Latitude (degrees)
    
    GeoPoint();
    GeoPoint(double lon, double lat);
};
```

#### BoundingBox

Geographic bounding box.

```cpp
struct BoundingBox {
    double min_lon;             // Minimum longitude
    double min_lat;             // Minimum latitude
    double max_lon;             // Maximum longitude
    double max_lat;             // Maximum latitude
    
    BoundingBox();
    BoundingBox(double minLon, double minLat, double maxLon, double maxLat);
    
    bool Contains(double lon, double lat) const;
    bool Intersects(const BoundingBox& other) const;
};
```

---

## Positioning Module

Header: `ogc/navi/positioning/*.h`

### NmeaParser

NMEA 0183 sentence parser (Singleton).

**Header**: `ogc/navi/positioning/nmea_parser.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Instance()` | `NmeaParser&` | Get singleton instance |
| `ParseGGA(sentence, data)` | `bool` | Parse GGA sentence |
| `ParseRMC(sentence, data)` | `bool` | Parse RMC sentence |
| `ParseVTG(sentence, data)` | `bool` | Parse VTG sentence |
| `ParseGSA(sentence, data)` | `bool` | Parse GSA sentence |
| `ParseGSV(sentence, data)` | `bool` | Parse GSV sentence |
| `ValidateChecksum(sentence)` | `bool` | Validate NMEA checksum |

#### Data Structures

```cpp
struct GgaData {
    double utc_time;            // UTC time (HHMMSS.SSS)
    double latitude;            // Latitude (degrees)
    double longitude;           // Longitude (degrees)
    int gps_quality;            // GPS quality indicator
    int satellite_count;        // Number of satellites
    double hdop;                // Horizontal dilution
    double altitude;            // Altitude (meters)
    double geoidal_separation;  // Geoidal separation
    double dgps_age;            // DGPS data age
    int dgps_station_id;        // DGPS station ID
};

struct RmcData {
    double utc_time;            // UTC time
    char status;                // Status (A=valid, V=invalid)
    double latitude;            // Latitude (degrees)
    double longitude;           // Longitude (degrees)
    double speed_knots;         // Speed (knots)
    double track_angle;         // Track angle (degrees)
    int date;                   // Date (DDMMYY)
    double magnetic_variation;  // Magnetic variation
    char magnetic_direction;    // Magnetic direction
};

struct VtgData {
    double track_true;          // True track (degrees)
    char track_true_indicator;  // 'T'
    double track_magnetic;      // Magnetic track (degrees)
    char track_magnetic_indicator; // 'M'
    double speed_knots;         // Speed (knots)
    char speed_knots_indicator; // 'N'
    double speed_kph;           // Speed (km/h)
    char speed_kph_indicator;   // 'K'
};

struct GsaData {
    char mode;                  // Mode (M=manual, A=automatic)
    int fix_type;               // Fix type (1-3)
    int satellites[12];         // Satellite PRNs
    double pdop;                // Position dilution
    double hdop;                // Horizontal dilution
    double vdop;                // Vertical dilution
};

struct GsvData {
    int total_messages;         // Total messages
    int message_number;         // Message number
    int satellites_in_view;     // Satellites in view
    GsvSatellite satellites[4]; // Satellite info (max 4 per message)
};
```

#### Example

```cpp
#include <ogc/navi/positioning/nmea_parser.h>

using namespace ogc::navi;

NmeaParser& parser = NmeaParser::Instance();

std::string gga = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
GgaData data;
if (parser.ParseGGA(gga, data)) {
    printf("Position: %.6f, %.6f\n", data.latitude, data.longitude);
    printf("Satellites: %d, HDOP: %.2f\n", data.satellite_count, data.hdop);
}
```

---

### CoordinateConverter

Coordinate transformation utilities (Singleton).

**Header**: `ogc/navi/positioning/coordinate_converter.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Instance()` | `CoordinateConverter&` | Get singleton instance |
| `CalculateGreatCircleDistance(lat1, lon1, lat2, lon2)` | `double` | Calculate distance (meters) |
| `CalculateBearing(lat1, lon1, lat2, lon2)` | `double` | Calculate bearing (degrees) |
| `CalculateDestination(lat, lon, bearing, distance, &dest_lat, &dest_lon)` | `void` | Calculate destination point |
| `LatLonToUtm(lat, lon, &zone, &easting, &northing)` | `void` | Convert to UTM |
| `UtmToLatLon(zone, easting, northing, &lat, &lon)` | `void` | Convert from UTM |

#### Example

```cpp
#include <ogc/navi/positioning/coordinate_converter.h>

using namespace ogc::navi;

CoordinateConverter& converter = CoordinateConverter::Instance();

// Calculate distance
double distance = converter.CalculateGreatCircleDistance(
    31.2304, 121.4737,  // Shanghai
    39.9042, 116.4074   // Beijing
);
// Result: ~1,067,310 meters

// Calculate bearing
double bearing = converter.CalculateBearing(
    31.2304, 121.4737,
    39.9042, 116.4074
);
// Result: ~343 degrees (NNW)

// Calculate destination
double dest_lat, dest_lon;
converter.CalculateDestination(31.2304, 121.4737, 45.0, 100000.0, dest_lat, dest_lon);
// Result: Point 100km away at bearing 45°
```

---

### PositionFilter

Position data filter for outlier detection and smoothing.

**Header**: `ogc/navi/positioning/position_filter.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `PositionFilter()` | | Constructor |
| `Filter(data)` | `PositionData` | Filter position data |
| `Reset()` | `void` | Reset filter state |
| `SetMaxSpeed(knots)` | `void` | Set maximum speed threshold |
| `SetMaxAcceleration(mps2)` | `void` | Set maximum acceleration |
| `SetSmoothWindowSize(size)` | `void` | Set smoothing window size |
| `SetMaxHdop(hdop)` | `void` | Set maximum HDOP |
| `SetMinSatellites(count)` | `void` | Set minimum satellite count |
| `GetStatistics()` | `FilterStatistics` | Get filter statistics |

#### Data Structures

```cpp
struct FilterStatistics {
    int total_received;         // Total positions received
    int valid_count;            // Valid positions
    int outlier_count;          // Outlier positions
    int extrapolated_count;     // Extrapolated positions
    double outlier_rate;        // Outlier rate (0.0-1.0)
};
```

#### Example

```cpp
#include <ogc/navi/positioning/position_filter.h>

using namespace ogc::navi;

PositionFilter filter;
filter.SetMaxSpeed(25.0);
filter.SetMaxHdop(3.0);
filter.SetMinSatellites(6);

PositionData raw;
raw.latitude = 31.2304;
raw.longitude = 121.4737;
raw.hdop = 1.0;
raw.satellite_count = 8;
raw.quality = PositionQuality::Gps;

PositionData filtered = filter.Filter(raw);

FilterStatistics stats = filter.GetStatistics();
printf("Valid: %d, Outliers: %d\n", stats.valid_count, stats.outlier_count);
```

---

## Route Module

Header: `ogc/navi/route/*.h`

### Waypoint

Waypoint representation.

**Header**: `ogc/navi/route/waypoint.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Create(data)` | `Waypoint*` | Factory method |
| `ReleaseReference()` | `void` | Release reference |
| `GetId()` | `const std::string&` | Get waypoint ID |
| `GetName()` | `const std::string&` | Get waypoint name |
| `SetName(name)` | `void` | Set waypoint name |
| `GetLongitude()` | `double` | Get longitude |
| `GetLatitude()` | `double` | Get latitude |
| `SetPosition(lon, lat)` | `void` | Set position |
| `GetArrivalRadius()` | `double` | Get arrival radius (NM) |
| `SetArrivalRadius(radius)` | `void` | Set arrival radius |
| `GetAction()` | `WaypointAction` | Get waypoint action |
| `SetAction(action)` | `void` | Set waypoint action |
| `ToData()` | `WaypointData` | Export to data struct |

#### Data Structure

```cpp
struct WaypointData {
    std::string id;             // Waypoint ID
    std::string name;           // Waypoint name
    double longitude;           // Longitude (degrees)
    double latitude;            // Latitude (degrees)
    double arrival_radius;      // Arrival radius (NM)
    WaypointAction action;      // Waypoint action
    std::string notes;          // Notes
};
```

#### Example

```cpp
#include <ogc/navi/route/waypoint.h>

using namespace ogc::navi;

WaypointData data;
data.id = "WP001";
data.name = "Shanghai Port";
data.longitude = 121.4737;
data.latitude = 31.2304;
data.arrival_radius = 0.5;  // 0.5 NM

Waypoint* wp = Waypoint::Create(data);
printf("Waypoint: %s at (%.4f, %.4f)\n", 
       wp->GetName().c_str(), wp->GetLongitude(), wp->GetLatitude());

wp->ReleaseReference();
```

---

### Route

Route container with waypoints.

**Header**: `ogc/navi/route/route.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Create()` | `Route*` | Factory method |
| `ReleaseReference()` | `void` | Release reference |
| `GetName()` | `const std::string&` | Get route name |
| `SetName(name)` | `void` | Set route name |
| `GetDeparture()` | `const std::string&` | Get departure port |
| `SetDeparture(port)` | `void` | Set departure port |
| `GetDestination()` | `const std::string&` | Get destination port |
| `SetDestination(port)` | `void` | Set destination port |
| `AddWaypoint(wp)` | `void` | Add waypoint |
| `RemoveWaypoint(index)` | `bool` | Remove waypoint |
| `GetWaypoint(index)` | `Waypoint*` | Get waypoint |
| `GetWaypointCount()` | `int` | Get waypoint count |
| `CalculateTotalDistance()` | `double` | Calculate total distance (m) |
| `CalculateLegBearing(leg_index)` | `double` | Calculate leg bearing |
| `Clear()` | `void` | Clear all waypoints |

#### Example

```cpp
#include <ogc/navi/route/route.h>
#include <ogc/navi/route/waypoint.h>

using namespace ogc::navi;

Route* route = Route::Create();
route->SetName("Shanghai-Tokyo");
route->SetDeparture("Shanghai");
route->SetDestination("Tokyo");

WaypointData wp1_data;
wp1_data.id = "WP001";
wp1_data.name = "Shanghai";
wp1_data.longitude = 121.4737;
wp1_data.latitude = 31.2304;

WaypointData wp2_data;
wp2_data.id = "WP002";
wp2_data.name = "Tokyo";
wp2_data.longitude = 139.6917;
wp2_data.latitude = 35.6895;

Waypoint* wp1 = Waypoint::Create(wp1_data);
Waypoint* wp2 = Waypoint::Create(wp2_data);

route->AddWaypoint(wp1);
route->AddWaypoint(wp2);

wp1->ReleaseReference();
wp2->ReleaseReference();

printf("Route: %s\n", route->GetName().c_str());
printf("Waypoints: %d\n", route->GetWaypointCount());
printf("Total distance: %.2f km\n", route->CalculateTotalDistance() / 1000.0);

route->ReleaseReference();
```

---

## Navigation Module

Header: `ogc/navi/navigation/*.h`

### NavigationCalculator

Navigation calculations (Singleton).

**Header**: `ogc/navi/navigation/navigation_calculator.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Instance()` | `NavigationCalculator&` | Get singleton instance |
| `CalculateCrossTrackError(pos, start, end)` | `double` | Calculate XTD (meters) |
| `CalculateTimeToWaypoint(distance, speed)` | `double` | Calculate TTG (seconds) |
| `CalculateEstimatedTimeArrival(distance, speed, current_time)` | `double` | Calculate ETA |
| `CalculateVelocityMadeGood(speed, course, bearing)` | `double` | Calculate VMG |

#### Example

```cpp
#include <ogc/navi/navigation/navigation_calculator.h>

using namespace ogc::navi;

NavigationCalculator& calc = NavigationCalculator::Instance();

// Cross track error
GeoPoint position(121.5, 31.5);
GeoPoint start(121.0, 31.0);
GeoPoint end(122.0, 32.0);
double xtd = calc.CalculateCrossTrackError(position, start, end);
printf("XTD: %.1f meters\n", xtd);

// Time to waypoint
double distance = 10000.0;  // 10 km
double speed = 10.0;        // 10 knots
double ttg = calc.CalculateTimeToWaypoint(distance, speed);
printf("TTG: %.1f seconds (%.1f minutes)\n", ttg, ttg / 60.0);
```

---

### OffCourseDetector

Off-course detection (Singleton).

**Header**: `ogc/navi/navigation/off_course_detector.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Instance()` | `OffCourseDetector&` | Get singleton instance |
| `Detect(pos, start, end)` | `OffCourseResult` | Detect off-course |
| `SetXtdWarning(meters)` | `void` | Set warning threshold |
| `SetXtdAlarm(meters)` | `void` | Set alarm threshold |
| `GetXtdWarning()` | `double` | Get warning threshold |
| `GetXtdAlarm()` | `double` | Get alarm threshold |

#### Data Structure

```cpp
struct OffCourseResult {
    bool is_off_course;         // Is off course
    double xtd;                 // Cross track distance (meters)
    AlertLevel level;           // Alert level
    double recommended_bearing; // Recommended bearing to return
};
```

#### Example

```cpp
#include <ogc/navi/navigation/off_course_detector.h>

using namespace ogc::navi;

OffCourseDetector& detector = OffCourseDetector::Instance();
detector.SetXtdWarning(100.0);  // 100m warning
detector.SetXtdAlarm(200.0);    // 200m alarm

GeoPoint position(121.5, 31.5);
GeoPoint start(121.0, 31.0);
GeoPoint end(122.0, 32.0);

OffCourseResult result = detector.Detect(position, start, end);
if (result.is_off_course) {
    printf("Off course! XTD: %.1f m\n", result.xtd);
    printf("Recommended bearing: %.1f°\n", result.recommended_bearing);
}
```

---

## Track Module

Header: `ogc/navi/track/*.h`

### TrackPoint

Single track point.

**Header**: `ogc/navi/track/track_point.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Create(data)` | `TrackPoint*` | Factory method |
| `ReleaseReference()` | `void` | Release reference |
| `GetLongitude()` | `double` | Get longitude |
| `GetLatitude()` | `double` | Get latitude |
| `SetPosition(lon, lat)` | `void` | Set position |
| `GetTimestamp()` | `double` | Get timestamp |
| `SetTimestamp(ts)` | `void` | Set timestamp |
| `GetSpeed()` | `double` | Get speed (knots) |
| `SetSpeed(speed)` | `void` | Set speed |
| `GetCourse()` | `double` | Get course (degrees) |
| `SetCourse(course)` | `void` | Set course |
| `ToData()` | `TrackPointData` | Export to data struct |

#### Data Structure

```cpp
struct TrackPointData {
    double longitude;           // Longitude (degrees)
    double latitude;            // Latitude (degrees)
    double timestamp;           // Timestamp (seconds)
    double speed;               // Speed (knots)
    double course;              // Course (degrees)
    double heading;             // Heading (degrees)
    double depth;               // Depth (meters)
};
```

---

### Track

Track container with points.

**Header**: `ogc/navi/track/track.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Create()` | `Track*` | Factory method |
| `ReleaseReference()` | `void` | Release reference |
| `GetName()` | `const std::string&` | Get track name |
| `SetName(name)` | `void` | Set track name |
| `GetType()` | `TrackType` | Get track type |
| `SetType(type)` | `void` | Set track type |
| `AddPoint(point)` | `void` | Add track point |
| `GetPoint(index)` | `TrackPoint*` | Get track point |
| `GetPointCount()` | `int` | Get point count |
| `GetTotalDistance()` | `double` | Get total distance (m) |
| `GetTotalDuration()` | `double` | Get total duration (s) |
| `Simplify(tolerance)` | `void` | Simplify track (Douglas-Peucker) |
| `GetPointsInTimeRange(start, end)` | `std::vector<TrackPoint*>` | Query by time |
| `Clear()` | `void` | Clear all points |

#### Example

```cpp
#include <ogc/navi/track/track.h>
#include <ogc/navi/track/track_point.h>

using namespace ogc::navi;

Track* track = Track::Create();
track->SetName("Morning Trip");
track->SetType(TrackType::RealTime);

for (int i = 0; i < 100; ++i) {
    TrackPointData data;
    data.longitude = 121.4737 + i * 0.001;
    data.latitude = 31.2304 + i * 0.001;
    data.timestamp = 1000.0 + i * 60.0;
    data.speed = 10.0;
    data.course = 45.0;
    
    TrackPoint* point = TrackPoint::Create(data);
    track->AddPoint(point);
    point->ReleaseReference();
}

printf("Points: %d\n", track->GetPointCount());
printf("Distance: %.2f km\n", track->GetTotalDistance() / 1000.0);
printf("Duration: %.1f minutes\n", track->GetTotalDuration() / 60.0);

// Simplify track
track->Simplify(10.0);  // 10 meter tolerance
printf("After simplification: %d points\n", track->GetPointCount());

track->ReleaseReference();
```

---

## AIS Module

Header: `ogc/navi/ais/*.h`

### AisTarget

AIS target representation.

**Header**: `ogc/navi/ais/ais_target.h`

#### Methods

| Method | Return | Description |
|--------|--------|-------------|
| `Create(mmsi)` | `AisTarget*` | Factory method |
| `ReleaseReference()` | `void` | Release reference |
| `GetMmsi()` | `uint32_t` | Get MMSI number |
| `GetShipName()` | `const std::string&` | Get ship name |
| `SetShipName(name)` | `void` | Set ship name |
| `GetCallsign()` | `const std::string&` | Get callsign |
| `SetCallsign(callsign)` | `void` | Set callsign |
| `GetLongitude()` | `double` | Get longitude |
| `GetLatitude()` | `double` | Get latitude |
| `SetPosition(lon, lat)` | `void` | Set position |
| `GetSpeedOverGround()` | `double` | Get SOG (knots) |
| `SetSpeedOverGround(sog)` | `void` | Set SOG |
| `GetCourseOverGround()` | `double` | Get COG (degrees) |
| `SetCourseOverGround(cog)` | `void` | Set COG |
| `GetCpa()` | `double` | Get CPA (meters) |
| `SetCpa(cpa)` | `void` | Set CPA |
| `GetTcpa()` | `double` | Get TCPA (seconds) |
| `SetTcpa(tcpa)` | `void` | Set TCPA |
| `HasCollisionRisk()` | `bool` | Check collision risk |
| `SetCollisionRisk(risk)` | `void` | Set collision risk flag |
| `GetShipType()` | `AisShipType` | Get ship type |
| `SetShipType(type)` | `void` | Set ship type |
| `GetShipLength()` | `double` | Get ship length (meters) |
| `SetShipLength(length)` | `void` | Set ship length |
| `GetShipWidth()` | `double` | Get ship width (meters) |
| `SetShipWidth(width)` | `void` | Set ship width |
| `GetDestination()` | `const std::string&` | Get destination |
| `SetDestination(dest)` | `void` | Set destination |

#### Example

```cpp
#include <ogc/navi/ais/ais_target.h>

using namespace ogc::navi;

AisTarget* target = AisTarget::Create(123456789);
target->SetShipName("TEST VESSEL");
target->SetCallsign("TEST123");
target->SetPosition(121.5, 31.5);
target->SetSpeedOverGround(12.5);
target->SetCourseOverGround(90.0);
target->SetShipType(AisShipType::Cargo);
target->SetShipLength(150.0);
target->SetShipWidth(25.0);

// Set collision risk data
target->SetCpa(500.0);   // 500 meters
target->SetTcpa(300.0);  // 5 minutes

if (target->HasCollisionRisk()) {
    printf("Collision risk with %s!\n", target->GetShipName().c_str());
    printf("CPA: %.0f m, TCPA: %.0f s\n", target->GetCpa(), target->GetTcpa());
}

target->ReleaseReference();
```

---

## Usage Examples

### Complete Navigation Workflow

```cpp
#include <ogc/navi/positioning/nmea_parser.h>
#include <ogc/navi/positioning/position_filter.h>
#include <ogc/navi/positioning/coordinate_converter.h>
#include <ogc/navi/route/route.h>
#include <ogc/navi/route/waypoint.h>
#include <ogc/navi/navigation/navigation_calculator.h>
#include <ogc/navi/navigation/off_course_detector.h>

using namespace ogc::navi;

void NavigationWorkflow() {
    // 1. Parse NMEA data
    NmeaParser& parser = NmeaParser::Instance();
    std::string rmc = "$GPRMC,092751.000,A,3120.1234,N,12130.5678,E,12.5,45.0,191124,,,A*XX";
    
    RmcData rmc_data;
    if (parser.ParseRMC(rmc, rmc_data)) {
        // 2. Filter position
        PositionFilter filter;
        PositionData raw_pos;
        raw_pos.latitude = rmc_data.latitude;
        raw_pos.longitude = rmc_data.longitude;
        raw_pos.speed = rmc_data.speed_knots;
        raw_pos.course = rmc_data.track_angle;
        raw_pos.hdop = 1.0;
        raw_pos.satellite_count = 8;
        raw_pos.quality = PositionQuality::Gps;
        
        PositionData filtered = filter.Filter(raw_pos);
        
        // 3. Create route
        Route* route = Route::Create();
        route->SetName("Test Route");
        
        WaypointData wp_data;
        wp_data.id = "WP001";
        wp_data.name = "Destination";
        wp_data.longitude = 122.0;
        wp_data.latitude = 32.0;
        
        Waypoint* dest = Waypoint::Create(wp_data);
        route->AddWaypoint(dest);
        
        // 4. Calculate navigation
        CoordinateConverter& converter = CoordinateConverter::Instance();
        NavigationCalculator& calc = NavigationCalculator::Instance();
        
        double distance = converter.CalculateGreatCircleDistance(
            filtered.latitude, filtered.longitude,
            dest->GetLatitude(), dest->GetLongitude());
        
        double bearing = converter.CalculateBearing(
            filtered.latitude, filtered.longitude,
            dest->GetLatitude(), dest->GetLongitude());
        
        double ttg = calc.CalculateTimeToWaypoint(distance, filtered.speed);
        
        printf("Distance: %.2f km\n", distance / 1000.0);
        printf("Bearing: %.1f°\n", bearing);
        printf("Time to go: %.1f minutes\n", ttg / 60.0);
        
        // 5. Check off-course
        OffCourseDetector& detector = OffCourseDetector::Instance();
        GeoPoint pos(filtered.longitude, filtered.latitude);
        GeoPoint start(121.0, 31.0);
        GeoPoint end(122.0, 32.0);
        
        OffCourseResult result = detector.Detect(pos, start, end);
        if (result.is_off_course) {
            printf("Off course by %.1f meters!\n", result.xtd);
        }
        
        // Cleanup
        dest->ReleaseReference();
        route->ReleaseReference();
    }
}
```

---

## Build Configuration

```cmake
# CMakeLists.txt example
find_package(ogc_geometry REQUIRED)
find_package(ogc_database REQUIRED)

add_executable(your_app main.cpp)

target_link_libraries(your_app
    PRIVATE
        ogc_navi
        ogc_geometry
        ogc_database
)
```

---

## Version

- **Version**: 1.0.0
- **C++ Standard**: C++11
- **Last Updated**: 2026-04-05
