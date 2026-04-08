#ifndef OGC_ALERT_QUERY_SERVICE_H
#define OGC_ALERT_QUERY_SERVICE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

class IAlertRepository;

struct PageCursor {
    std::string last_id;
    DateTime last_time;
    bool has_more;
};

struct OptimizedQueryParams {
    std::string user_id;
    std::vector<AlertType> alert_types;
    std::vector<AlertLevel> alert_levels;
    DateTime start_time;
    DateTime end_time;
    int limit;
    PageCursor cursor;
    bool use_cursor;
    std::string sort_field;
    bool ascending;
    
    OptimizedQueryParams()
        : limit(20)
        , use_cursor(false)
        , sort_field("issue_time")
        , ascending(false) {}
};

struct QueryIndexStats {
    int total_alerts;
    int indexed_alerts;
    int cache_hits;
    int cache_misses;
    double avg_query_time_ms;
};

class OGC_ALERT_API IQueryService {
public:
    virtual ~IQueryService() = default;
    
    virtual AlertListResult GetAlertList(const AlertQueryParams& params) = 0;
    virtual AlertPtr GetAlertDetail(const std::string& alert_id) = 0;
    virtual AlertListResult GetAlertHistory(const std::string& user_id, 
                                             const DateTime& start_time,
                                             const DateTime& end_time,
                                             int page, int page_size) = 0;
    
    virtual std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) = 0;
    virtual std::vector<AlertPtr> GetAlertsByType(const std::string& user_id, AlertType type) = 0;
    virtual std::vector<AlertPtr> GetAlertsByLevel(const std::string& user_id, AlertLevel level) = 0;
    
    virtual int GetActiveAlertCount(const std::string& user_id) = 0;
    virtual int GetAlertCountByType(const std::string& user_id, AlertType type) = 0;
    
    virtual std::vector<AlertPtr> SearchAlerts(const std::string& user_id, const std::string& keyword) = 0;
    
    virtual AlertListResult GetAlertListOptimized(const OptimizedQueryParams& params) = 0;
    virtual AlertListResult GetAlertListByCursor(const std::string& user_id, 
                                                  const PageCursor& cursor, 
                                                  int limit) = 0;
    virtual PageCursor GetFirstPageCursor(const std::string& user_id) = 0;
    
    virtual void BuildIndex(const std::string& user_id, const std::string& field) = 0;
    virtual void DropIndex(const std::string& user_id, const std::string& field) = 0;
    virtual QueryIndexStats GetIndexStats(const std::string& user_id) = 0;
    
    virtual void PrefetchPages(const std::string& user_id, int start_page, int end_page, int page_size) = 0;
    virtual void ClearCache() = 0;
    
    static std::unique_ptr<IQueryService> Create();
};

class OGC_ALERT_API QueryService : public IQueryService {
public:
    QueryService();
    explicit QueryService(std::shared_ptr<IAlertRepository> repository);
    ~QueryService();
    
    AlertListResult GetAlertList(const AlertQueryParams& params) override;
    AlertPtr GetAlertDetail(const std::string& alert_id) override;
    AlertListResult GetAlertHistory(const std::string& user_id, 
                                    const DateTime& start_time,
                                    const DateTime& end_time,
                                    int page, int page_size) override;
    
    std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) override;
    std::vector<AlertPtr> GetAlertsByType(const std::string& user_id, AlertType type) override;
    std::vector<AlertPtr> GetAlertsByLevel(const std::string& user_id, AlertLevel level) override;
    
    int GetActiveAlertCount(const std::string& user_id) override;
    int GetAlertCountByType(const std::string& user_id, AlertType type) override;
    
    std::vector<AlertPtr> SearchAlerts(const std::string& user_id, const std::string& keyword) override;
    
    AlertListResult GetAlertListOptimized(const OptimizedQueryParams& params) override;
    AlertListResult GetAlertListByCursor(const std::string& user_id, 
                                         const PageCursor& cursor, 
                                         int limit) override;
    PageCursor GetFirstPageCursor(const std::string& user_id) override;
    
    void BuildIndex(const std::string& user_id, const std::string& field) override;
    void DropIndex(const std::string& user_id, const std::string& field) override;
    QueryIndexStats GetIndexStats(const std::string& user_id) override;
    
    void PrefetchPages(const std::string& user_id, int start_page, int end_page, int page_size) override;
    void ClearCache() override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
