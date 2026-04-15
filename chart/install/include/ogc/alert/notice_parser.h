#ifndef OGC_ALERT_NOTICE_PARSER_H
#define OGC_ALERT_NOTICE_PARSER_H

#include "export.h"
#include "types.h"
#include "ogc/geom/geometry.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

enum class NoticeType {
    CHANNEL_CLOSED,
    TEMPORARY_BAN,
    CONSTRUCTION_AREA,
    MILITARY_EXERCISE,
    DREDGING_OPERATION,
    UNDERWATER_WORK,
    REGATTA,
    OTHER
};

enum class NoticeStatus {
    ACTIVE,
    PENDING,
    CANCELLED,
    EXPIRED
};

struct NavigationNotice {
    std::string notice_id;
    std::string title;
    NoticeType type;
    NoticeStatus status;
    DateTime issue_time;
    DateTime effective_from;
    DateTime effective_to;
    ogc::GeometrySharedPtr affected_area;
    std::vector<Coordinate> affected_coordinates;
    std::string description;
    std::string authority;
    std::string contact_info;
    std::vector<std::string> restrictions;
    std::vector<std::string> recommendations;
    int severity;
    std::string source;
    std::string raw_content;
};

struct ParsedNotice {
    bool valid;
    NavigationNotice notice;
    std::vector<std::string> parse_errors;
    std::vector<std::string> parse_warnings;
};

struct NoticeFilter {
    std::vector<NoticeType> types;
    std::vector<NoticeStatus> statuses;
    DateTime effective_at;
    Coordinate location;
    double radius_km;
    int min_severity;
};

class OGC_ALERT_API INoticeParser {
public:
    virtual ~INoticeParser() = default;
    
    virtual ParsedNotice Parse(const std::string& raw_content) = 0;
    virtual std::vector<ParsedNotice> ParseBatch(const std::vector<std::string>& raw_contents) = 0;
    
    virtual bool Validate(const NavigationNotice& notice) const = 0;
    
    static std::unique_ptr<INoticeParser> Create();
};

class OGC_ALERT_API INoticeProvider {
public:
    virtual ~INoticeProvider() = default;
    
    virtual std::vector<NavigationNotice> GetNotices(const NoticeFilter& filter) = 0;
    virtual NavigationNotice GetNoticeById(const std::string& notice_id) = 0;
    virtual void AddNotice(const NavigationNotice& notice) = 0;
    virtual void UpdateNotice(const NavigationNotice& notice) = 0;
    virtual void RemoveNotice(const std::string& notice_id) = 0;
    
    static std::unique_ptr<INoticeProvider> Create();
};

class OGC_ALERT_API NoticeParser : public INoticeParser {
public:
    NoticeParser();
    ~NoticeParser() override;
    
    ParsedNotice Parse(const std::string& raw_content) override;
    std::vector<ParsedNotice> ParseBatch(const std::vector<std::string>& raw_contents) override;
    
    bool Validate(const NavigationNotice& notice) const override;

private:
    bool ParseHeader(const std::string& content, NavigationNotice& notice);
    bool ParseBody(const std::string& content, NavigationNotice& notice);
    bool ParseCoordinates(const std::string& text, std::vector<Coordinate>& coords);
    bool ParseDateTime(const std::string& text, DateTime& dt);
    bool ParseArea(const std::string& text, ogc::GeometrySharedPtr& area);
    
    NoticeType DetermineNoticeType(const std::string& text);
    int DetermineSeverity(const NavigationNotice& notice);
    std::vector<std::string> ExtractKeywords(const std::string& text);
};

class OGC_ALERT_API NoticeProvider : public INoticeProvider {
public:
    NoticeProvider();
    ~NoticeProvider() override;
    
    std::vector<NavigationNotice> GetNotices(const NoticeFilter& filter) override;
    NavigationNotice GetNoticeById(const std::string& notice_id) override;
    void AddNotice(const NavigationNotice& notice) override;
    void UpdateNotice(const NavigationNotice& notice) override;
    void RemoveNotice(const std::string& notice_id) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    bool MatchesFilter(const NavigationNotice& notice, const NoticeFilter& filter) const;
    bool IsInRadius(const Coordinate& center, double radius_km, 
                    const std::vector<Coordinate>& coords) const;
};

class OGC_ALERT_API NoticeMatcher {
public:
    static std::vector<NavigationNotice> MatchByLocation(
        const std::vector<NavigationNotice>& notices,
        const Coordinate& location);
    
    static std::vector<NavigationNotice> MatchByRoute(
        const std::vector<NavigationNotice>& notices,
        const std::vector<Coordinate>& route);
    
    static std::vector<NavigationNotice> MatchByArea(
        const std::vector<NavigationNotice>& notices,
        const ogc::GeometrySharedPtr& area);
    
    static bool IsNoticeActive(const NavigationNotice& notice, const DateTime& time);
};

class OGC_ALERT_API NoticeFormatter {
public:
    static std::string FormatSummary(const NavigationNotice& notice);
    static std::string FormatDetail(const NavigationNotice& notice);
    static std::string FormatAlert(const NavigationNotice& notice);
};

}
}

#endif
