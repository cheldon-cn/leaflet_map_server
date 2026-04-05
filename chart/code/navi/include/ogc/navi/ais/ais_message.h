#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <cstdint>

namespace ogc {
namespace navi {

struct AisMessage {
    int message_type;
    uint32_t mmsi;
    int repeat_indicator;
    double timestamp;
    
    AisMessage()
        : message_type(0)
        , mmsi(0)
        , repeat_indicator(0)
        , timestamp(0.0)
    {}
};

struct AisPositionReport {
    uint32_t mmsi;
    AisNavigationStatus nav_status;
    double longitude;
    double latitude;
    double speed_over_ground;
    double course_over_ground;
    double heading;
    int message_type;
    double timestamp;
    
    AisPositionReport()
        : mmsi(0)
        , nav_status(AisNavigationStatus::NotAvailable)
        , longitude(0.0)
        , latitude(0.0)
        , speed_over_ground(0.0)
        , course_over_ground(0.0)
        , heading(0.0)
        , message_type(0)
        , timestamp(0.0)
    {}
};

struct AisStaticData {
    uint32_t mmsi;
    std::string callsign;
    std::string ship_name;
    AisShipType ship_type;
    int dimension_to_bow;
    int dimension_to_stern;
    int dimension_to_port;
    int dimension_to_starboard;
    int eta_month;
    int eta_day;
    int eta_hour;
    int eta_minute;
    double draught;
    std::string destination;
    
    AisStaticData()
        : mmsi(0)
        , ship_type(AisShipType::NotAvailable)
        , dimension_to_bow(0)
        , dimension_to_stern(0)
        , dimension_to_port(0)
        , dimension_to_starboard(0)
        , eta_month(0)
        , eta_day(0)
        , eta_hour(24)
        , eta_minute(60)
        , draught(0.0)
    {}
};

}
}
