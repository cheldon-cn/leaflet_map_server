#pragma once

/**
 * @file datetime.h
 * @brief 日期时间结构定义
 */

#include "export.h"
#include <cstdint>
#include <string>

namespace ogc {

/**
 * @brief 日期时间结构
 * 
 * 支持日期、时间、日期时间三种类�? * 遵循 ISO 8601 标准
 */
struct OGC_FEATURE_API CNDateTime {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int millisecond = 0;
    
    /**
     * @brief 时区类型枚举
     */
    enum class TimeZoneType : int8_t {
        kUnknown = 0,
        kLocal = 1,
        kUTC = 2,
        kFixedOffset = 3
    };
    
    TimeZoneType tz_type = TimeZoneType::kUnknown;
    int tz_offset = 0;
    
    CNDateTime() = default;
    
    CNDateTime(int y, int m, int d, int h = 0, int min = 0, 
               int s = 0, int ms = 0, int tz = 0);
    
    static TimeZoneType FromLegacyTZFlag(int tz_flag);
    
    int ToLegacyTZFlag() const;
    
    static CNDateTime FromISO8601(const std::string& iso_string);
    
    static CNDateTime NowUTC();
    
    static CNDateTime NowLocal();
    
    bool IsValid() const;
    
    bool IsDateOnly() const;
    
    bool IsTimeOnly() const;
    
    bool HasTimeZone() const;
    
    bool IsEmpty() const;
    
    bool operator==(const CNDateTime& other) const;
    bool operator!=(const CNDateTime& other) const;
    bool operator<(const CNDateTime& other) const;
    bool operator<=(const CNDateTime& other) const;
    bool operator>(const CNDateTime& other) const;
    bool operator>=(const CNDateTime& other) const;
    
    std::string ToISO8601() const;
    
    std::string ToWKT() const;
    
    int64_t ToTimestamp() const;
    
    static CNDateTime FromTimestamp(int64_t timestamp);
};

} // namespace ogc
