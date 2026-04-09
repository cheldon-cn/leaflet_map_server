#include "ogc/alert/notice_parser.h"
#include "ogc/base/log.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/point.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>
#include <cctype>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace alert {

namespace {
const double kEarthRadiusKm = 6371.0;

double HaversineDistance(const Coordinate& from, const Coordinate& to) {
    double lat1 = from.latitude * M_PI / 180.0;
    double lat2 = to.latitude * M_PI / 180.0;
    double dLat = (to.latitude - from.latitude) * M_PI / 180.0;
    double dLon = (to.longitude - from.longitude) * M_PI / 180.0;
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return kEarthRadiusKm * c;
}

std::string Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        token = Trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

}

NoticeParser::NoticeParser() {
}

NoticeParser::~NoticeParser() {
}

ParsedNotice NoticeParser::Parse(const std::string& raw_content) {
    ParsedNotice result;
    result.valid = false;
    result.notice.raw_content = raw_content;
    
    if (raw_content.empty()) {
        result.parse_errors.push_back("Empty content");
        return result;
    }
    
    if (!ParseHeader(raw_content, result.notice)) {
        result.parse_errors.push_back("Failed to parse header");
    }
    
    if (!ParseBody(raw_content, result.notice)) {
        result.parse_warnings.push_back("Partial body parsing");
    }
    
    result.notice.severity = DetermineSeverity(result.notice);
    
    result.valid = Validate(result.notice);
    
    return result;
}

std::vector<ParsedNotice> NoticeParser::ParseBatch(const std::vector<std::string>& raw_contents) {
    std::vector<ParsedNotice> results;
    results.reserve(raw_contents.size());
    
    for (const auto& content : raw_contents) {
        results.push_back(Parse(content));
    }
    
    return results;
}

bool NoticeParser::Validate(const NavigationNotice& notice) const {
    if (notice.notice_id.empty()) {
        return false;
    }
    if (notice.title.empty()) {
        return false;
    }
    if (notice.effective_from.ToTimestamp() >= notice.effective_to.ToTimestamp()) {
        return false;
    }
    return true;
}

bool NoticeParser::ParseHeader(const std::string& content, NavigationNotice& notice) {
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        if (line.find("NOTICE ID:") == 0 || line.find("编号:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                notice.notice_id = Trim(line.substr(pos + 1));
            }
        }
        else if (line.find("TITLE:") == 0 || line.find("标题:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                notice.title = Trim(line.substr(pos + 1));
            }
        }
        else if (line.find("TYPE:") == 0 || line.find("类型:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                notice.type = DetermineNoticeType(Trim(line.substr(pos + 1)));
            }
        }
        else if (line.find("STATUS:") == 0 || line.find("状态:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string status = Trim(line.substr(pos + 1));
                if (status == "ACTIVE" || status == "生效") {
                    notice.status = NoticeStatus::ACTIVE;
                } else if (status == "PENDING" || status == "待生效") {
                    notice.status = NoticeStatus::PENDING;
                } else if (status == "CANCELLED" || status == "取消") {
                    notice.status = NoticeStatus::CANCELLED;
                } else {
                    notice.status = NoticeStatus::EXPIRED;
                }
            }
        }
        else if (line.find("ISSUE TIME:") == 0 || line.find("发布时间:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                ParseDateTime(Trim(line.substr(pos + 1)), notice.issue_time);
            }
        }
        else if (line.find("EFFECTIVE FROM:") == 0 || line.find("生效时间:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                ParseDateTime(Trim(line.substr(pos + 1)), notice.effective_from);
            }
        }
        else if (line.find("EFFECTIVE TO:") == 0 || line.find("结束时间:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                ParseDateTime(Trim(line.substr(pos + 1)), notice.effective_to);
            }
        }
        else if (line.find("AUTHORITY:") == 0 || line.find("发布单位:") == 0) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                notice.authority = Trim(line.substr(pos + 1));
            }
        }
    }
    
    return !notice.notice_id.empty() && !notice.title.empty();
}

bool NoticeParser::ParseBody(const std::string& content, NavigationNotice& notice) {
    std::istringstream iss(content);
    std::string line;
    bool inDescription = false;
    bool inRestrictions = false;
    bool inRecommendations = false;
    
    while (std::getline(iss, line)) {
        line = Trim(line);
        if (line.empty()) continue;
        
        if (line.find("DESCRIPTION:") == 0 || line.find("描述:") == 0) {
            inDescription = true;
            inRestrictions = false;
            inRecommendations = false;
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                notice.description = Trim(line.substr(pos + 1));
            }
        }
        else if (line.find("COORDINATES:") == 0 || line.find("坐标:") == 0) {
            inDescription = false;
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                ParseCoordinates(Trim(line.substr(pos + 1)), notice.affected_coordinates);
            }
        }
        else if (line.find("AREA:") == 0 || line.find("区域:") == 0) {
            inDescription = false;
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                ParseArea(Trim(line.substr(pos + 1)), notice.affected_area);
            }
        }
        else if (line.find("RESTRICTIONS:") == 0 || line.find("限制:") == 0) {
            inDescription = false;
            inRestrictions = true;
            inRecommendations = false;
        }
        else if (line.find("RECOMMENDATIONS:") == 0 || line.find("建议:") == 0) {
            inDescription = false;
            inRestrictions = false;
            inRecommendations = true;
        }
        else if (inDescription) {
            notice.description += "\n" + line;
        }
        else if (inRestrictions) {
            if (line[0] == '-' || line[0] == '*' || ::isdigit(static_cast<unsigned char>(line[0]))) {
                notice.restrictions.push_back(line);
            }
        }
        else if (inRecommendations) {
            if (line[0] == '-' || line[0] == '*' || ::isdigit(static_cast<unsigned char>(line[0]))) {
                notice.recommendations.push_back(line);
            }
        }
    }
    
    return true;
}

bool NoticeParser::ParseCoordinates(const std::string& text, std::vector<Coordinate>& coords) {
    coords.clear();
    
    std::vector<std::string> parts = Split(text, ';');
    
    for (const auto& part : parts) {
        std::vector<std::string> latLon = Split(part, ',');
        if (latLon.size() >= 2) {
            Coordinate coord;
            coord.longitude = std::stod(Trim(latLon[0]));
            coord.latitude = std::stod(Trim(latLon[1]));
            coords.push_back(coord);
        }
    }
    
    return !coords.empty();
}

bool NoticeParser::ParseDateTime(const std::string& text, DateTime& dt) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    
    if (sscanf(text.c_str(), "%d-%d-%d %d:%d:%d", 
               &year, &month, &day, &hour, &minute, &second) >= 3) {
        dt.year = year;
        dt.month = month;
        dt.day = day;
        dt.hour = hour;
        dt.minute = minute;
        dt.second = second;
        dt.millisecond = 0;
        return true;
    }
    
    if (sscanf(text.c_str(), "%d/%d/%d %d:%d:%d", 
               &year, &month, &day, &hour, &minute, &second) >= 3) {
        dt.year = year;
        dt.month = month;
        dt.day = day;
        dt.hour = hour;
        dt.minute = minute;
        dt.second = second;
        dt.millisecond = 0;
        return true;
    }
    
    return false;
}

bool NoticeParser::ParseArea(const std::string& text, ogc::GeometrySharedPtr& area) {
    std::vector<Coordinate> coords;
    if (ParseCoordinates(text, coords) && coords.size() >= 3) {
        ogc::CoordinateList coordList;
        for (const auto& c : coords) {
            coordList.push_back(ogc::Coordinate(c.longitude, c.latitude));
        }
        auto ring = ogc::LinearRing::Create(coordList);
        area = ogc::Polygon::Create(std::move(ring));
        return true;
    }
    return false;
}

NoticeType NoticeParser::DetermineNoticeType(const std::string& text) {
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower.find("closed") != std::string::npos || 
        lower.find("关闭") != std::string::npos) {
        return NoticeType::CHANNEL_CLOSED;
    }
    if (lower.find("ban") != std::string::npos || 
        lower.find("禁航") != std::string::npos) {
        return NoticeType::TEMPORARY_BAN;
    }
    if (lower.find("construction") != std::string::npos || 
        lower.find("施工") != std::string::npos) {
        return NoticeType::CONSTRUCTION_AREA;
    }
    if (lower.find("military") != std::string::npos || 
        lower.find("军事") != std::string::npos) {
        return NoticeType::MILITARY_EXERCISE;
    }
    if (lower.find("dredging") != std::string::npos || 
        lower.find("疏浚") != std::string::npos) {
        return NoticeType::DREDGING_OPERATION;
    }
    if (lower.find("underwater") != std::string::npos || 
        lower.find("水下") != std::string::npos) {
        return NoticeType::UNDERWATER_WORK;
    }
    if (lower.find("regatta") != std::string::npos || 
        lower.find("帆船赛") != std::string::npos) {
        return NoticeType::REGATTA;
    }
    
    return NoticeType::OTHER;
}

int NoticeParser::DetermineSeverity(const NavigationNotice& notice) {
    int severity = 1;
    
    switch (notice.type) {
        case NoticeType::CHANNEL_CLOSED:
            severity = 4;
            break;
        case NoticeType::TEMPORARY_BAN:
            severity = 4;
            break;
        case NoticeType::MILITARY_EXERCISE:
            severity = 3;
            break;
        case NoticeType::CONSTRUCTION_AREA:
            severity = 2;
            break;
        case NoticeType::DREDGING_OPERATION:
            severity = 2;
            break;
        case NoticeType::UNDERWATER_WORK:
            severity = 2;
            break;
        case NoticeType::REGATTA:
            severity = 1;
            break;
        default:
            severity = 1;
            break;
    }
    
    if (notice.status == NoticeStatus::CANCELLED) {
        severity = 0;
    }
    
    return severity;
}

std::vector<std::string> NoticeParser::ExtractKeywords(const std::string& text) {
    std::vector<std::string> keywords;
    
    std::vector<std::string> words = Split(text, ' ');
    for (const auto& word : words) {
        if (word.length() >= 2) {
            keywords.push_back(word);
        }
    }
    
    return keywords;
}

std::unique_ptr<INoticeParser> INoticeParser::Create() {
    return std::unique_ptr<INoticeParser>(new NoticeParser());
}

NoticeProvider::NoticeProvider() {
}

NoticeProvider::~NoticeProvider() {
}

std::vector<NavigationNotice> NoticeProvider::GetNotices(const NoticeFilter& filter) {
    std::vector<NavigationNotice> result;
    
    for (const auto& pair : m_notices) {
        if (MatchesFilter(pair.second, filter)) {
            result.push_back(pair.second);
        }
    }
    
    std::sort(result.begin(), result.end(),
        [](const NavigationNotice& a, const NavigationNotice& b) {
            return a.severity > b.severity;
        });
    
    return result;
}

NavigationNotice NoticeProvider::GetNoticeById(const std::string& notice_id) {
    auto it = m_notices.find(notice_id);
    if (it != m_notices.end()) {
        return it->second;
    }
    return NavigationNotice();
}

void NoticeProvider::AddNotice(const NavigationNotice& notice) {
    m_notices[notice.notice_id] = notice;
}

void NoticeProvider::UpdateNotice(const NavigationNotice& notice) {
    m_notices[notice.notice_id] = notice;
}

void NoticeProvider::RemoveNotice(const std::string& notice_id) {
    m_notices.erase(notice_id);
}

bool NoticeProvider::MatchesFilter(const NavigationNotice& notice, const NoticeFilter& filter) const {
    if (!filter.types.empty()) {
        bool typeMatch = false;
        for (NoticeType type : filter.types) {
            if (notice.type == type) {
                typeMatch = true;
                break;
            }
        }
        if (!typeMatch) return false;
    }
    
    if (!filter.statuses.empty()) {
        bool statusMatch = false;
        for (NoticeStatus status : filter.statuses) {
            if (notice.status == status) {
                statusMatch = true;
                break;
            }
        }
        if (!statusMatch) return false;
    }
    
    if (filter.effective_at.ToTimestamp() > 0) {
        if (!NoticeMatcher::IsNoticeActive(notice, filter.effective_at)) {
            return false;
        }
    }
    
    if (filter.min_severity > 0 && notice.severity < filter.min_severity) {
        return false;
    }
    
    if (filter.radius_km > 0 && !IsInRadius(filter.location, filter.radius_km, 
                                             notice.affected_coordinates)) {
        return false;
    }
    
    return true;
}

bool NoticeProvider::IsInRadius(const Coordinate& center, double radius_km, 
                                 const std::vector<Coordinate>& coords) const {
    for (const auto& coord : coords) {
        if (HaversineDistance(center, coord) <= radius_km) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<INoticeProvider> INoticeProvider::Create() {
    return std::unique_ptr<INoticeProvider>(new NoticeProvider());
}

std::vector<NavigationNotice> NoticeMatcher::MatchByLocation(
    const std::vector<NavigationNotice>& notices,
    const Coordinate& location) {
    std::vector<NavigationNotice> result;
    
    auto point = ogc::Point::Create(location.longitude, location.latitude);
    
    for (const auto& notice : notices) {
        bool matched = false;
        if (notice.affected_area && notice.affected_area->IsValid()) {
            matched = notice.affected_area->Contains(point.get());
        }
        
        if (!matched) {
            for (const auto& coord : notice.affected_coordinates) {
                if (HaversineDistance(location, coord) < 1.0) {
                    matched = true;
                    break;
                }
            }
        }
        
        if (matched) {
            result.push_back(notice);
        }
    }
    
    return result;
}

std::vector<NavigationNotice> NoticeMatcher::MatchByRoute(
    const std::vector<NavigationNotice>& notices,
    const std::vector<Coordinate>& route) {
    std::vector<NavigationNotice> result;
    
    for (const auto& notice : notices) {
        for (const auto& routePoint : route) {
            bool matched = false;
            
            if (notice.affected_area && notice.affected_area->IsValid()) {
                auto point = ogc::Point::Create(routePoint.longitude, routePoint.latitude);
                matched = notice.affected_area->Contains(point.get());
            }
            
            if (!matched) {
                for (const auto& coord : notice.affected_coordinates) {
                    if (HaversineDistance(routePoint, coord) < 1.0) {
                        matched = true;
                        break;
                    }
                }
            }
            
            if (matched) {
                result.push_back(notice);
                break;
            }
        }
    }
    
    return result;
}

std::vector<NavigationNotice> NoticeMatcher::MatchByArea(
    const std::vector<NavigationNotice>& notices,
    const ogc::GeometrySharedPtr& area) {
    std::vector<NavigationNotice> result;
    
    if (!area || !area->IsValid()) {
        return result;
    }
    
    for (const auto& notice : notices) {
        if (notice.affected_area && notice.affected_area->IsValid()) {
            if (notice.affected_area->Intersects(area.get())) {
                result.push_back(notice);
            }
        }
    }
    
    return result;
}

bool NoticeMatcher::IsNoticeActive(const NavigationNotice& notice, const DateTime& time) {
    return notice.status == NoticeStatus::ACTIVE &&
           time >= notice.effective_from && 
           time <= notice.effective_to;
}

std::string NoticeFormatter::FormatSummary(const NavigationNotice& notice) {
    std::ostringstream oss;
    oss << "[" << notice.notice_id << "] " << notice.title;
    return oss.str();
}

std::string NoticeFormatter::FormatDetail(const NavigationNotice& notice) {
    std::ostringstream oss;
    oss << "Notice ID: " << notice.notice_id << "\n";
    oss << "Title: " << notice.title << "\n";
    oss << "Type: " << static_cast<int>(notice.type) << "\n";
    oss << "Status: " << static_cast<int>(notice.status) << "\n";
    oss << "Effective: " << notice.effective_from.year << "-" 
        << notice.effective_from.month << "-" << notice.effective_from.day;
    oss << " to " << notice.effective_to.year << "-" 
        << notice.effective_to.month << "-" << notice.effective_to.day << "\n";
    oss << "Description: " << notice.description << "\n";
    oss << "Authority: " << notice.authority << "\n";
    return oss.str();
}

std::string NoticeFormatter::FormatAlert(const NavigationNotice& notice) {
    std::ostringstream oss;
    oss << "ALERT [" << notice.severity << "]: " << notice.title << "\n";
    oss << notice.description << "\n";
    
    if (!notice.restrictions.empty()) {
        oss << "Restrictions:\n";
        for (const auto& r : notice.restrictions) {
            oss << "  " << r << "\n";
        }
    }
    
    if (!notice.recommendations.empty()) {
        oss << "Recommendations:\n";
        for (const auto& r : notice.recommendations) {
            oss << "  " << r << "\n";
        }
    }
    
    return oss.str();
}

}
}
