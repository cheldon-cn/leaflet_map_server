#pragma once

#include <string>
#include <cstdint>

namespace ogc {
namespace navi {

enum class PositionQuality {
    Invalid = 0,
    Gps = 1,
    DGps = 2,
    Pps = 3,
    Rtk = 4,
    FloatRtk = 5,
    Simulation = 6,
    ManualInput = 7
};

enum class PositionSource {
    Unknown = 0,
    System = 1,
    Serial = 2,
    Bluetooth = 3,
    Network = 4,
    File = 5
};

enum class NavigationStatus {
    Inactive = 0,
    Active = 1,
    Paused = 2,
    Completed = 3,
    OffRoute = 4,
    Arrived = 5
};

enum class RouteStatus {
    Planned = 0,
    Active = 1,
    Completed = 2,
    Cancelled = 3
};

enum class WaypointAction {
    None = 0,
    Turn = 1,
    Stop = 2,
    SpeedChange = 3,
    CourseChange = 4
};

enum class AlertLevel {
    Info = 0,
    Warning = 1,
    Critical = 2,
    Emergency = 3
};

enum class AlertType {
    OffCourse = 0,
    WaypointApproach = 1,
    WaypointArrival = 2,
    ShallowWater = 3,
    ObstacleProximity = 4,
    AisCollision = 5,
    GpsSignalLost = 6,
    LowBattery = 7,
    SystemError = 8
};

enum class TrackType {
    RealTime = 0,
    Historical = 1,
    Planned = 2
};

enum class AisNavigationStatus {
    UnderWayUsingEngine = 0,
    AtAnchor = 1,
    NotUnderCommand = 2,
    RestrictedManeuverability = 3,
    ConstrainedByDraught = 4,
    Moored = 5,
    Aground = 6,
    EngagedInFishing = 7,
    UnderWaySailing = 8,
    ReservedHSC = 9,
    ReservedWIG = 10,
    NotAvailable = 15
};

enum class AisShipType {
    NotAvailable = 0,
    Reserved1_19 = 1,
    WIG = 20,
    Fishing = 30,
    Towing = 31,
    TowingLarge = 32,
    Dredging = 33,
    DivingOps = 34,
    MilitaryOps = 35,
    Sailing = 36,
    PleasureCraft = 37,
    Reserved38_39 = 38,
    HighSpeedCraft = 40,
    PilotVessel = 50,
    SearchAndRescue = 51,
    Tug = 52,
    PortTender = 53,
    AntiPollution = 54,
    LawEnforcement = 55,
    Reserved56_57 = 56,
    MedicalTransport = 58,
    NonCombatShip = 59,
    Passenger = 60,
    Cargo = 70,
    Tanker = 80,
    Other = 90
};

struct PositionData {
    double timestamp;
    double longitude;
    double latitude;
    double altitude;
    double heading;
    double speed;
    double course;
    double hdop;
    double vdop;
    double pdop;
    int satellite_count;
    int gps_quality;
    PositionQuality quality;
    PositionSource source;
    uint32_t mmsi;
    bool valid;
    
    PositionData()
        : timestamp(0.0)
        , longitude(0.0)
        , latitude(0.0)
        , altitude(0.0)
        , heading(0.0)
        , speed(0.0)
        , course(0.0)
        , hdop(99.9)
        , vdop(99.9)
        , pdop(99.9)
        , satellite_count(0)
        , gps_quality(0)
        , quality(PositionQuality::Invalid)
        , source(PositionSource::Unknown)
        , mmsi(0)
        , valid(false)
    {}
};

struct GeoPoint {
    double longitude;
    double latitude;
    
    GeoPoint() : longitude(0.0), latitude(0.0) {}
    GeoPoint(double lon, double lat) : longitude(lon), latitude(lat) {}
};

struct BoundingBox {
    double min_lon;
    double min_lat;
    double max_lon;
    double max_lat;
    
    BoundingBox()
        : min_lon(0.0), min_lat(0.0), max_lon(0.0), max_lat(0.0) {}
    
    BoundingBox(double minLon, double minLat, double maxLon, double maxLat)
        : min_lon(minLon), min_lat(minLat), max_lon(maxLon), max_lat(maxLat) {}
    
    bool Contains(double lon, double lat) const {
        return lon >= min_lon && lon <= max_lon &&
               lat >= min_lat && lat <= max_lat;
    }
    
    bool Intersects(const BoundingBox& other) const {
        return !(max_lon < other.min_lon || min_lon > other.max_lon ||
                 max_lat < other.min_lat || min_lat > other.max_lat);
    }
};

}
}
