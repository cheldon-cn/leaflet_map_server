#include "ogc/alert/types.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

DateTime::DateTime()
    : year(1970), month(1), day(1), hour(0), minute(0), second(0), millisecond(0) {
}

DateTime::DateTime(std::time_t timestamp) {
    std::tm* tm = std::gmtime(&timestamp);
    if (tm) {
        year = tm->tm_year + 1900;
        month = tm->tm_mon + 1;
        day = tm->tm_mday;
        hour = tm->tm_hour;
        minute = tm->tm_min;
        second = tm->tm_sec;
        millisecond = 0;
    } else {
        year = 1970;
        month = 1;
        day = 1;
        hour = 0;
        minute = 0;
        second = 0;
        millisecond = 0;
    }
}

DateTime DateTime::Now() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    DateTime dt(timestamp);
    dt.millisecond = static_cast<int>(ms.count());
    return dt;
}

DateTime DateTime::FromString(const std::string& str) {
    DateTime dt;
    if (str.length() >= 19) {
        dt.year = std::atoi(str.substr(0, 4).c_str());
        dt.month = std::atoi(str.substr(5, 2).c_str());
        dt.day = std::atoi(str.substr(8, 2).c_str());
        dt.hour = std::atoi(str.substr(11, 2).c_str());
        dt.minute = std::atoi(str.substr(14, 2).c_str());
        dt.second = std::atoi(str.substr(17, 2).c_str());
        if (str.length() >= 23 && str[19] == '.') {
            dt.millisecond = std::atoi(str.substr(20, 3).c_str());
        }
    }
    return dt;
}

DateTime DateTime::FromTimestamp(std::time_t timestamp) {
    return DateTime(timestamp);
}

std::string DateTime::ToString() const {
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << year << "-"
        << std::setw(2) << month << "-"
        << std::setw(2) << day << "T"
        << std::setw(2) << hour << ":"
        << std::setw(2) << minute << ":"
        << std::setw(2) << second << "."
        << std::setw(3) << millisecond;
    return oss.str();
}

std::time_t DateTime::ToTimestamp() const {
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    return std::mktime(&tm);
}

bool DateTime::IsValid() const {
    return year >= 1970 && year <= 9999 &&
           month >= 1 && month <= 12 &&
           day >= 1 && day <= 31 &&
           hour >= 0 && hour <= 23 &&
           minute >= 0 && minute <= 59 &&
           second >= 0 && second <= 59 &&
           millisecond >= 0 && millisecond <= 999;
}

bool DateTime::operator==(const DateTime& other) const {
    return year == other.year &&
           month == other.month &&
           day == other.day &&
           hour == other.hour &&
           minute == other.minute &&
           second == other.second;
}

bool DateTime::operator<(const DateTime& other) const {
    return ToTimestamp() < other.ToTimestamp();
}

bool DateTime::operator<=(const DateTime& other) const {
    return ToTimestamp() <= other.ToTimestamp();
}

bool DateTime::operator>(const DateTime& other) const {
    return ToTimestamp() > other.ToTimestamp();
}

bool DateTime::operator>=(const DateTime& other) const {
    return ToTimestamp() >= other.ToTimestamp();
}

Coordinate::Coordinate()
    : longitude(0.0), latitude(0.0), altitude(0.0) {
}

Coordinate::Coordinate(double lon, double lat, double alt)
    : longitude(lon), latitude(lat), altitude(alt) {
}

bool Coordinate::IsValid() const {
    return longitude >= -180.0 && longitude <= 180.0 &&
           latitude >= -90.0 && latitude <= 90.0;
}

bool Coordinate::operator==(const Coordinate& other) const {
    return std::abs(longitude - other.longitude) < 1e-9 &&
           std::abs(latitude - other.latitude) < 1e-9 &&
           std::abs(altitude - other.altitude) < 1e-9;
}

double Coordinate::DistanceTo(const Coordinate& other) const {
    const double R = 6371000.0;
    double lat1 = latitude * M_PI / 180.0;
    double lat2 = other.latitude * M_PI / 180.0;
    double dLat = (other.latitude - latitude) * M_PI / 180.0;
    double dLon = (other.longitude - longitude) * M_PI / 180.0;
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return R * c;
}

AlertThreshold::AlertThreshold()
    : level1_threshold(0.0), level2_threshold(0.0), level3_threshold(0.0), level4_threshold(0.0) {
}

AlertLevel AlertThreshold::DetermineLevel(double value) const {
    if (value >= level4_threshold) return AlertLevel::kLevel4;
    if (value >= level3_threshold) return AlertLevel::kLevel3;
    if (value >= level2_threshold) return AlertLevel::kLevel2;
    if (value >= level1_threshold) return AlertLevel::kLevel1;
    return AlertLevel::kNone;
}

}
}
