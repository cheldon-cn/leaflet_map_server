#pragma once

#include "ogc/navi/types.h"
#include "ogc/navi/export.h"
#include <string>
#include <functional>
#include <vector>

namespace ogc {
namespace navi {

struct GgaData {
    double utc_time;
    double latitude;
    double longitude;
    int gps_quality;
    int satellite_count;
    double hdop;
    double altitude;
    double geoidal_separation;
    double dgps_age;
    int dgps_station_id;
    
    GgaData()
        : utc_time(0.0)
        , latitude(0.0)
        , longitude(0.0)
        , gps_quality(0)
        , satellite_count(0)
        , hdop(99.9)
        , altitude(0.0)
        , geoidal_separation(0.0)
        , dgps_age(0.0)
        , dgps_station_id(0)
    {}
};

struct RmcData {
    double utc_time;
    char status;
    double latitude;
    double longitude;
    double speed_knots;
    double track_angle;
    int date;
    double magnetic_variation;
    char magnetic_direction;
    
    RmcData()
        : utc_time(0.0)
        , status('V')
        , latitude(0.0)
        , longitude(0.0)
        , speed_knots(0.0)
        , track_angle(0.0)
        , date(0)
        , magnetic_variation(0.0)
        , magnetic_direction(' ')
    {}
};

struct VtgData {
    double track_true;
    char track_true_indicator;
    double track_magnetic;
    char track_magnetic_indicator;
    double speed_knots;
    char speed_knots_indicator;
    double speed_kph;
    char speed_kph_indicator;
    
    VtgData()
        : track_true(0.0)
        , track_true_indicator('T')
        , track_magnetic(0.0)
        , track_magnetic_indicator('M')
        , speed_knots(0.0)
        , speed_knots_indicator('N')
        , speed_kph(0.0)
        , speed_kph_indicator('K')
    {}
};

struct GsaData {
    char mode;
    int fix_type;
    int satellites[12];
    double pdop;
    double hdop;
    double vdop;
    
    GsaData()
        : mode('M')
        , fix_type(1)
        , pdop(99.9)
        , hdop(99.9)
        , vdop(99.9)
    {
        for (int i = 0; i < 12; ++i) {
            satellites[i] = 0;
        }
    }
};

struct GsvData {
    int total_messages;
    int message_number;
    int satellites_in_view;
    struct SatelliteInfo {
        int prn;
        int elevation;
        int azimuth;
        int snr;
        SatelliteInfo() : prn(0), elevation(0), azimuth(0), snr(0) {}
    } satellites[4];
    
    GsvData()
        : total_messages(0)
        , message_number(0)
        , satellites_in_view(0)
    {}
};

class OGC_NAVI_API NmeaParser {
public:
    static NmeaParser& Instance();
    
    bool Parse(const std::string& nmea_sentence, PositionData& data);
    bool ParseGGA(const std::string& sentence, GgaData& data);
    bool ParseRMC(const std::string& sentence, RmcData& data);
    bool ParseVTG(const std::string& sentence, VtgData& data);
    bool ParseGSA(const std::string& sentence, GsaData& data);
    bool ParseGSV(const std::string& sentence, GsvData& data);
    
    bool ValidateChecksum(const std::string& sentence);
    std::string CalculateChecksum(const std::string& sentence);
    
private:
    NmeaParser() = default;
    NmeaParser(const NmeaParser&) = delete;
    NmeaParser& operator=(const NmeaParser&) = delete;
    
    bool ParseField(const std::vector<std::string>& fields, size_t index, 
                    std::string& value);
    bool ParseField(const std::vector<std::string>& fields, size_t index, 
                    double& value);
    bool ParseField(const std::vector<std::string>& fields, size_t index, 
                    int& value);
    double ParseCoordinate(const std::string& coord, const std::string& hemisphere);
    double ParseTime(const std::string& time_str);
    double ParseDate(const std::string& date_str);
    std::vector<std::string> Split(const std::string& str, char delimiter);
};

}
}
