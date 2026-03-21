#include "ogc/feature/datetime.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ogc {

namespace {

bool IsLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DaysInMonth(int year, int month) {
    static const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && IsLeapYear(year)) {
        return 29;
    }
    return days[month];
}

int64_t DateToDays(int year, int month, int day) {
    int64_t days = 0;
    for (int y = 1; y < year; ++y) {
        days += IsLeapYear(y) ? 366 : 365;
    }
    for (int m = 1; m < month; ++m) {
        days += DaysInMonth(year, m);
    }
    days += day - 1;
    return days;
}

void DaysToDate(int64_t days, int& year, int& month, int& day) {
    year = 1;
    while (true) {
        int days_in_year = IsLeapYear(year) ? 366 : 365;
        if (days < days_in_year) break;
        days -= days_in_year;
        ++year;
    }
    month = 1;
    while (true) {
        int dim = DaysInMonth(year, month);
        if (days < dim) break;
        days -= dim;
        ++month;
    }
    day = static_cast<int>(days) + 1;
}

} // namespace

CNDateTime::CNDateTime(int y, int m, int d, int h, int min, int s, int ms, int tz)
    : year(y), month(m), day(d), hour(h), minute(min), second(s), 
      millisecond(ms), tz_type(TimeZoneType::kFixedOffset), tz_offset(tz) {
}

CNDateTime::TimeZoneType CNDateTime::FromLegacyTZFlag(int tz_flag) {
    if (tz_flag == 0) return TimeZoneType::kUnknown;
    if (tz_flag == 1) return TimeZoneType::kLocal;
    if (tz_flag == 100) return TimeZoneType::kUTC;
    if (tz_flag > 100) return TimeZoneType::kFixedOffset;
    if (tz_flag < 0) return TimeZoneType::kFixedOffset;
    return TimeZoneType::kUnknown;
}

int CNDateTime::ToLegacyTZFlag() const {
    switch (tz_type) {
        case TimeZoneType::kUnknown: return 0;
        case TimeZoneType::kLocal: return 1;
        case TimeZoneType::kUTC: return 100;
        case TimeZoneType::kFixedOffset: return 100 + tz_offset;
        default: return 0;
    }
}

CNDateTime CNDateTime::FromISO8601(const std::string& iso_string) {
    CNDateTime dt;
    if (iso_string.empty()) return dt;
    
    std::istringstream iss(iso_string);
    char sep;
    
    if (iso_string.find('T') != std::string::npos) {
        iss >> dt.year >> sep >> dt.month >> sep >> dt.day;
        iss >> dt.hour >> sep >> dt.minute >> sep >> dt.second;
        if (iss.good()) {
            if (iso_string.back() == 'Z') {
                dt.tz_type = TimeZoneType::kUTC;
            } else {
                dt.tz_type = TimeZoneType::kFixedOffset;
                iss >> sep >> dt.tz_offset;
            }
        }
    } else if (iso_string.find(':') != std::string::npos) {
        iss >> dt.hour >> sep >> dt.minute >> sep >> dt.second;
        dt.tz_type = TimeZoneType::kLocal;
    } else {
        iss >> dt.year >> sep >> dt.month >> sep >> dt.day;
        dt.tz_type = TimeZoneType::kLocal;
    }
    
    return dt;
}

CNDateTime CNDateTime::NowUTC() {
    CNDateTime dt;
    std::time_t now = std::time(nullptr);
    std::tm* utc = std::gmtime(&now);
    dt.year = utc->tm_year + 1900;
    dt.month = utc->tm_mon + 1;
    dt.day = utc->tm_mday;
    dt.hour = utc->tm_hour;
    dt.minute = utc->tm_min;
    dt.second = utc->tm_sec;
    dt.tz_type = TimeZoneType::kUTC;
    return dt;
}

CNDateTime CNDateTime::NowLocal() {
    CNDateTime dt;
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    dt.year = local->tm_year + 1900;
    dt.month = local->tm_mon + 1;
    dt.day = local->tm_mday;
    dt.hour = local->tm_hour;
    dt.minute = local->tm_min;
    dt.second = local->tm_sec;
    dt.tz_type = TimeZoneType::kLocal;
    return dt;
}

bool CNDateTime::IsValid() const {
    if (year < 1 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > DaysInMonth(year, month)) return false;
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    if (second < 0 || second > 59) return false;
    if (millisecond < 0 || millisecond > 999) return false;
    return true;
}

bool CNDateTime::IsDateOnly() const {
    return hour == 0 && minute == 0 && second == 0 && millisecond == 0;
}

bool CNDateTime::IsTimeOnly() const {
    return year == 0 && month == 0 && day == 0;
}

bool CNDateTime::HasTimeZone() const {
    return tz_type == TimeZoneType::kUTC || 
           tz_type == TimeZoneType::kLocal ||
           tz_type == TimeZoneType::kFixedOffset;
}

bool CNDateTime::IsEmpty() const {
    return year == 0 && month == 0 && day == 0 && 
           hour == 0 && minute == 0 && second == 0 && millisecond == 0;
}

bool CNDateTime::operator==(const CNDateTime& other) const {
    return year == other.year && month == other.month && day == other.day &&
           hour == other.hour && minute == other.minute && second == other.second &&
           millisecond == other.millisecond && tz_type == other.tz_type &&
           tz_offset == other.tz_offset;
}

bool CNDateTime::operator!=(const CNDateTime& other) const {
    return !(*this == other);
}

bool CNDateTime::operator<(const CNDateTime& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    if (day != other.day) return day < other.day;
    if (hour != other.hour) return hour < other.hour;
    if (minute != other.minute) return minute < other.minute;
    if (second != other.second) return second < other.second;
    return millisecond < other.millisecond;
}

bool CNDateTime::operator<=(const CNDateTime& other) const {
    return !(other < *this);
}

bool CNDateTime::operator>(const CNDateTime& other) const {
    return other < *this;
}

bool CNDateTime::operator>=(const CNDateTime& other) const {
    return !(*this < other);
}

std::string CNDateTime::ToISO8601() const {
    std::ostringstream oss;
    if (IsTimeOnly()) {
        oss << std::setfill('0') << std::setw(2) << hour << ':'
            << std::setw(2) << minute << ':' << std::setw(2) << second;
        if (millisecond > 0) {
            oss << '.' << std::setw(3) << millisecond;
        }
    } else {
        oss << std::setfill('0') << std::setw(4) << year << '-'
            << std::setw(2) << month << '-' << std::setw(2) << day;
        if (!IsDateOnly()) {
            oss << 'T' << std::setw(2) << hour << ':'
                << std::setw(2) << minute << ':' << std::setw(2) << second;
            if (millisecond > 0) {
                oss << '.' << std::setw(3) << millisecond;
            }
        }
        if (tz_type == TimeZoneType::kUTC) {
            oss << 'Z';
        } else if (tz_type == TimeZoneType::kFixedOffset) {
            int offset = tz_offset;
            char sign = '+';
            if (offset < 0) {
                sign = '-';
                offset = -offset;
            }
            oss << sign << std::setw(2) << (offset / 60) << ':'
                << std::setw(2) << (offset % 60);
        }
    }
    return oss.str();
}

std::string CNDateTime::ToWKT() const {
    std::ostringstream oss;
    if (IsDateOnly()) {
        oss << "DATE '" << std::setfill('0') << std::setw(4) << year << '-'
            << std::setw(2) << month << '-' << std::setw(2) << day << "'";
    } else if (IsTimeOnly()) {
        oss << "TIME '" << std::setfill('0') << std::setw(2) << hour << ':'
            << std::setw(2) << minute << ':' << std::setw(2) << second << "'";
    } else {
        oss << "TIMESTAMP '" << std::setfill('0') << std::setw(4) << year << '-'
            << std::setw(2) << month << '-' << std::setw(2) << day << ' '
            << std::setw(2) << hour << ':' << std::setw(2) << minute << ':'
            << std::setw(2) << second << "'";
    }
    return oss.str();
}

int64_t CNDateTime::ToTimestamp() const {
    int64_t days = DateToDays(year, month, day);
    return days * 86400LL + hour * 3600LL + minute * 60LL + second;
}

CNDateTime CNDateTime::FromTimestamp(int64_t timestamp) {
    CNDateTime dt;
    int64_t days = timestamp / 86400LL;
    int64_t remaining = timestamp % 86400LL;
    DaysToDate(days, dt.year, dt.month, dt.day);
    dt.hour = static_cast<int>(remaining / 3600LL);
    remaining %= 3600LL;
    dt.minute = static_cast<int>(remaining / 60LL);
    dt.second = static_cast<int>(remaining % 60LL);
    return dt;
}

} // namespace ogc
