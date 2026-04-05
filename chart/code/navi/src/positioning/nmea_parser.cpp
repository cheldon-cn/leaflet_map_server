#include "ogc/navi/positioning/nmea_parser.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ogc {
namespace navi {

namespace {
    const double PI = 3.14159265358979323846;
}

NmeaParser& NmeaParser::Instance() {
    static NmeaParser instance;
    return instance;
}

bool NmeaParser::Parse(const std::string& nmea_sentence, PositionData& data) {
    if (!ValidateChecksum(nmea_sentence)) {
        return false;
    }
    
    std::string sentence = nmea_sentence;
    size_t asterisk_pos = sentence.find('*');
    if (asterisk_pos != std::string::npos) {
        sentence = sentence.substr(0, asterisk_pos);
    }
    
    if (sentence.find("$GPGGA") == 0 || sentence.find("$GNGGA") == 0) {
        GgaData gga;
        if (ParseGGA(sentence, gga)) {
            data.timestamp = gga.utc_time;
            data.latitude = gga.latitude;
            data.longitude = gga.longitude;
            data.altitude = gga.altitude;
            data.satellite_count = gga.satellite_count;
            data.hdop = gga.hdop;
            data.gps_quality = gga.gps_quality;
            
            if (gga.gps_quality == 0) {
                data.quality = PositionQuality::Invalid;
            } else if (gga.gps_quality == 1) {
                data.quality = PositionQuality::Gps;
            } else if (gga.gps_quality == 2) {
                data.quality = PositionQuality::DGps;
            } else if (gga.gps_quality == 4) {
                data.quality = PositionQuality::Rtk;
            } else if (gga.gps_quality == 5) {
                data.quality = PositionQuality::FloatRtk;
            }
            
            data.valid = (gga.gps_quality > 0);
            return true;
        }
    } else if (sentence.find("$GPRMC") == 0 || sentence.find("$GNRMC") == 0) {
        RmcData rmc;
        if (ParseRMC(sentence, rmc)) {
            data.timestamp = rmc.utc_time;
            data.latitude = rmc.latitude;
            data.longitude = rmc.longitude;
            data.speed = rmc.speed_knots;
            data.course = rmc.track_angle;
            data.valid = (rmc.status == 'A');
            return true;
        }
    } else if (sentence.find("$GPVTG") == 0 || sentence.find("$GNVTG") == 0) {
        VtgData vtg;
        if (ParseVTG(sentence, vtg)) {
            data.course = vtg.track_true;
            data.speed = vtg.speed_knots;
            return true;
        }
    } else if (sentence.find("$GPGSA") == 0 || sentence.find("$GNGSA") == 0) {
        GsaData gsa;
        if (ParseGSA(sentence, gsa)) {
            data.pdop = gsa.pdop;
            data.hdop = gsa.hdop;
            data.vdop = gsa.vdop;
            return true;
        }
    }
    
    return false;
}

bool NmeaParser::ParseGGA(const std::string& sentence, GgaData& data) {
    std::vector<std::string> fields = Split(sentence, ',');
    
    if (fields.size() < 10) {
        return false;
    }
    
    std::string time_str;
    if (!ParseField(fields, 1, time_str)) {
        return false;
    }
    data.utc_time = ParseTime(time_str);
    
    std::string lat_str, lat_hem;
    if (!ParseField(fields, 2, lat_str) || !ParseField(fields, 3, lat_hem)) {
        return false;
    }
    data.latitude = ParseCoordinate(lat_str, lat_hem);
    
    std::string lon_str, lon_hem;
    if (!ParseField(fields, 4, lon_str) || !ParseField(fields, 5, lon_hem)) {
        return false;
    }
    data.longitude = ParseCoordinate(lon_str, lon_hem);
    
    ParseField(fields, 6, data.gps_quality);
    ParseField(fields, 7, data.satellite_count);
    ParseField(fields, 8, data.hdop);
    ParseField(fields, 9, data.altitude);
    ParseField(fields, 11, data.geoidal_separation);
    ParseField(fields, 13, data.dgps_age);
    ParseField(fields, 14, data.dgps_station_id);
    
    return true;
}

bool NmeaParser::ParseRMC(const std::string& sentence, RmcData& data) {
    std::vector<std::string> fields = Split(sentence, ',');
    
    if (fields.size() < 12) {
        return false;
    }
    
    std::string time_str;
    if (!ParseField(fields, 1, time_str)) {
        return false;
    }
    data.utc_time = ParseTime(time_str);
    
    std::string status;
    if (!ParseField(fields, 2, status)) {
        return false;
    }
    data.status = status.empty() ? 'V' : status[0];
    
    std::string lat_str, lat_hem;
    if (!ParseField(fields, 3, lat_str) || !ParseField(fields, 4, lat_hem)) {
        return false;
    }
    data.latitude = ParseCoordinate(lat_str, lat_hem);
    
    std::string lon_str, lon_hem;
    if (!ParseField(fields, 5, lon_str) || !ParseField(fields, 6, lon_hem)) {
        return false;
    }
    data.longitude = ParseCoordinate(lon_str, lon_hem);
    
    ParseField(fields, 7, data.speed_knots);
    ParseField(fields, 8, data.track_angle);
    
    std::string date_str;
    if (ParseField(fields, 9, date_str)) {
        data.date = std::stoi(date_str);
    }
    
    ParseField(fields, 10, data.magnetic_variation);
    
    std::string mag_dir;
    if (ParseField(fields, 11, mag_dir) && !mag_dir.empty()) {
        data.magnetic_direction = mag_dir[0];
    }
    
    return true;
}

bool NmeaParser::ParseVTG(const std::string& sentence, VtgData& data) {
    std::vector<std::string> fields = Split(sentence, ',');
    
    if (fields.size() < 9) {
        return false;
    }
    
    ParseField(fields, 1, data.track_true);
    
    std::string indicator;
    if (ParseField(fields, 2, indicator) && !indicator.empty()) {
        data.track_true_indicator = indicator[0];
    }
    
    ParseField(fields, 3, data.track_magnetic);
    
    if (ParseField(fields, 4, indicator) && !indicator.empty()) {
        data.track_magnetic_indicator = indicator[0];
    }
    
    ParseField(fields, 5, data.speed_knots);
    
    if (ParseField(fields, 6, indicator) && !indicator.empty()) {
        data.speed_knots_indicator = indicator[0];
    }
    
    ParseField(fields, 7, data.speed_kph);
    
    if (ParseField(fields, 8, indicator) && !indicator.empty()) {
        data.speed_kph_indicator = indicator[0];
    }
    
    return true;
}

bool NmeaParser::ParseGSA(const std::string& sentence, GsaData& data) {
    std::vector<std::string> fields = Split(sentence, ',');
    
    if (fields.size() < 18) {
        return false;
    }
    
    std::string mode;
    if (ParseField(fields, 1, mode) && !mode.empty()) {
        data.mode = mode[0];
    }
    
    ParseField(fields, 2, data.fix_type);
    
    for (int i = 0; i < 12; ++i) {
        ParseField(fields, 3 + i, data.satellites[i]);
    }
    
    ParseField(fields, 15, data.pdop);
    ParseField(fields, 16, data.hdop);
    ParseField(fields, 17, data.vdop);
    
    return true;
}

bool NmeaParser::ParseGSV(const std::string& sentence, GsvData& data) {
    std::vector<std::string> fields = Split(sentence, ',');
    
    if (fields.size() < 4) {
        return false;
    }
    
    ParseField(fields, 1, data.total_messages);
    ParseField(fields, 2, data.message_number);
    ParseField(fields, 3, data.satellites_in_view);
    
    int satellite_count = (static_cast<int>(fields.size()) - 4) / 4;
    if (satellite_count > 4) {
        satellite_count = 4;
    }
    
    for (int i = 0; i < satellite_count; ++i) {
        ParseField(fields, 4 + i * 4, data.satellites[i].prn);
        ParseField(fields, 5 + i * 4, data.satellites[i].elevation);
        ParseField(fields, 6 + i * 4, data.satellites[i].azimuth);
        ParseField(fields, 7 + i * 4, data.satellites[i].snr);
    }
    
    return true;
}

bool NmeaParser::ValidateChecksum(const std::string& sentence) {
    size_t asterisk_pos = sentence.find('*');
    if (asterisk_pos == std::string::npos) {
        return false;
    }
    
    if (asterisk_pos + 3 > sentence.length()) {
        return false;
    }
    
    std::string expected_checksum = sentence.substr(asterisk_pos + 1, 2);
    std::string calculated = CalculateChecksum(sentence.substr(0, asterisk_pos));
    
    std::transform(expected_checksum.begin(), expected_checksum.end(), expected_checksum.begin(), ::toupper);
    
    return expected_checksum == calculated;
}

std::string NmeaParser::CalculateChecksum(const std::string& sentence) {
    unsigned char checksum = 0;
    
    for (size_t i = 0; i < sentence.length(); ++i) {
        if (sentence[i] == '$') {
            continue;
        }
        checksum ^= static_cast<unsigned char>(sentence[i]);
    }
    
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
        << static_cast<int>(checksum);
    
    return oss.str();
}

bool NmeaParser::ParseField(const std::vector<std::string>& fields, size_t index, 
                            std::string& value) {
    if (index >= fields.size()) {
        return false;
    }
    value = fields[index];
    return !value.empty();
}

bool NmeaParser::ParseField(const std::vector<std::string>& fields, size_t index, 
                            double& value) {
    std::string str;
    if (!ParseField(fields, index, str)) {
        return false;
    }
    
    try {
        value = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool NmeaParser::ParseField(const std::vector<std::string>& fields, size_t index, 
                            int& value) {
    std::string str;
    if (!ParseField(fields, index, str)) {
        return false;
    }
    
    try {
        value = std::stoi(str);
        return true;
    } catch (...) {
        return false;
    }
}

double NmeaParser::ParseCoordinate(const std::string& coord, const std::string& hemisphere) {
    if (coord.empty() || hemisphere.empty()) {
        return 0.0;
    }
    
    double value = std::stod(coord);
    
    int degrees = static_cast<int>(value / 100.0);
    double minutes = value - degrees * 100.0;
    
    double decimal_degrees = degrees + minutes / 60.0;
    
    if (hemisphere == "S" || hemisphere == "W") {
        decimal_degrees = -decimal_degrees;
    }
    
    return decimal_degrees;
}

double NmeaParser::ParseTime(const std::string& time_str) {
    if (time_str.length() < 6) {
        return 0.0;
    }
    
    double time = std::stod(time_str);
    
    int hours = static_cast<int>(time / 10000.0);
    int minutes = static_cast<int>((time - hours * 10000) / 100.0);
    double seconds = time - hours * 10000 - minutes * 100;
    
    return hours * 3600.0 + minutes * 60.0 + seconds;
}

double NmeaParser::ParseDate(const std::string& date_str) {
    if (date_str.length() < 6) {
        return 0.0;
    }
    
    int date = std::stoi(date_str);
    
    int day = date / 10000;
    int month = (date - day * 10000) / 100;
    int year = date % 100;
    
    return year * 10000 + month * 100 + day;
}

std::vector<std::string> NmeaParser::Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

}
}
