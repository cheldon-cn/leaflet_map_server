#include "ogc/alert/query_service.h"
#include "ogc/alert/alert_repository.h"
#include "ogc/draw/log.h"
#include <algorithm>
#include <map>
#include <mutex>
#include <chrono>

namespace ogc {
namespace alert {

using ogc::draw::LogLevel;
using ogc::draw::LogHelper;

class QueryService::Impl {
public:
    Impl() : m_cacheHits(0), m_cacheMisses(0), m_totalQueryTimeMs(0.0), m_queryCount(0) {}
    explicit Impl(std::shared_ptr<IAlertRepository> repository) 
        : m_repository(repository)
        , m_cacheHits(0)
        , m_cacheMisses(0)
        , m_totalQueryTimeMs(0.0)
        , m_queryCount(0) {}
    
    AlertListResult GetAlertList(const AlertQueryParams& params) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        AlertListResult result;
        if (m_repository) {
            auto alerts = m_repository->Query(params);
            result.total_count = static_cast<int>(alerts.size());
            
            int start = (params.page - 1) * params.page_size;
            int end = std::min(start + params.page_size, static_cast<int>(alerts.size()));
            
            for (int i = start; i < end; ++i) {
                result.alerts.push_back(alerts[i]);
            }
            
            result.page = params.page;
            result.page_size = params.page_size;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        UpdateQueryStats(elapsedMs);
        
        return result;
    }
    
    AlertPtr GetAlertDetail(const std::string& alert_id) {
        std::string cacheKey = "detail_" + alert_id;
        auto cached = GetFromCache(cacheKey);
        if (cached) {
            m_cacheHits++;
            return cached;
        }
        
        m_cacheMisses++;
        if (m_repository) {
            auto alert = m_repository->FindById(alert_id);
            if (alert) {
                PutToCache(cacheKey, alert);
            }
            return alert;
        }
        return nullptr;
    }
    
    AlertListResult GetAlertHistory(const std::string& user_id,
                                    const DateTime& start_time,
                                    const DateTime& end_time,
                                    int page, int page_size) {
        AlertQueryParams params;
        params.user_id = user_id;
        params.start_time = start_time;
        params.end_time = end_time;
        params.page = page;
        params.page_size = page_size;
        return GetAlertList(params);
    }
    
    std::vector<AlertPtr> GetActiveAlerts(const std::string& user_id) {
        std::string cacheKey = "active_" + user_id;
        auto cached = GetFromCache(cacheKey);
        if (cached) {
            m_cacheHits++;
        } else {
            m_cacheMisses++;
        }
        
        if (m_repository) {
            return m_repository->GetActiveAlerts(user_id);
        }
        return {};
    }
    
    std::vector<AlertPtr> GetAlertsByType(const std::string& user_id, AlertType type) {
        if (m_repository) {
            return m_repository->GetAlertsByType(type, user_id);
        }
        return {};
    }
    
    std::vector<AlertPtr> GetAlertsByLevel(const std::string& user_id, AlertLevel level) {
        if (m_repository) {
            return m_repository->GetAlertsByLevel(level, user_id);
        }
        return {};
    }
    
    int GetActiveAlertCount(const std::string& user_id) {
        return static_cast<int>(GetActiveAlerts(user_id).size());
    }
    
    int GetAlertCountByType(const std::string& user_id, AlertType type) {
        return static_cast<int>(GetAlertsByType(user_id, type).size());
    }
    
    std::vector<AlertPtr> SearchAlerts(const std::string& user_id, const std::string& keyword) {
        std::vector<AlertPtr> result;
        auto alerts = GetActiveAlerts(user_id);
        for (const auto& alert : alerts) {
            if (alert->content.title.find(keyword) != std::string::npos ||
                alert->content.message.find(keyword) != std::string::npos) {
                result.push_back(alert);
            }
        }
        return result;
    }
    
    AlertListResult GetAlertListOptimized(const OptimizedQueryParams& params) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        AlertListResult result;
        
        if (params.use_cursor && !params.cursor.last_id.empty()) {
            result = GetAlertListByCursorInternal(params.user_id, params.cursor, params.limit);
        } else {
            AlertQueryParams queryParams;
            queryParams.user_id = params.user_id;
            queryParams.alert_types = params.alert_types;
            queryParams.alert_levels = params.alert_levels;
            queryParams.start_time = params.start_time;
            queryParams.end_time = params.end_time;
            queryParams.page = 1;
            queryParams.page_size = params.limit;
            
            result = GetAlertList(queryParams);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        UpdateQueryStats(elapsedMs);
        
        LOG_DEBUG() << "Optimized query completed in " << elapsedMs << "ms";
        return result;
    }
    
    AlertListResult GetAlertListByCursor(const std::string& user_id, 
                                         const PageCursor& cursor, 
                                         int limit) {
        return GetAlertListByCursorInternal(user_id, cursor, limit);
    }
    
    PageCursor GetFirstPageCursor(const std::string& user_id) {
        PageCursor cursor;
        cursor.last_id = "";
        cursor.last_time = DateTime::Now();
        cursor.has_more = true;
        
        auto alerts = GetActiveAlerts(user_id);
        if (!alerts.empty()) {
            cursor.has_more = alerts.size() > 1;
        } else {
            cursor.has_more = false;
        }
        
        return cursor;
    }
    
    void BuildIndex(const std::string& user_id, const std::string& field) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto alerts = GetActiveAlerts(user_id);
        std::map<std::string, std::vector<AlertPtr>> index;
        
        for (const auto& alert : alerts) {
            std::string key;
            if (field == "alert_type") {
                key = std::to_string(static_cast<int>(alert->alert_type));
            } else if (field == "alert_level") {
                key = std::to_string(static_cast<int>(alert->alert_level));
            } else if (field == "issue_time") {
                key = alert->issue_time.ToString();
            }
            
            if (!key.empty()) {
                index[key].push_back(alert);
            }
        }
        
        m_indexes[user_id + "_" + field] = index;
        LOG_INFO() << "Index built for user " << user_id << " on field " << field;
    }
    
    void DropIndex(const std::string& user_id, const std::string& field) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_indexes.erase(user_id + "_" + field);
        LOG_INFO() << "Index dropped for user " << user_id << " on field " << field;
    }
    
    QueryIndexStats GetIndexStats(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        QueryIndexStats stats;
        stats.total_alerts = static_cast<int>(GetActiveAlerts(user_id).size());
        stats.indexed_alerts = 0;
        stats.cache_hits = m_cacheHits;
        stats.cache_misses = m_cacheMisses;
        stats.avg_query_time_ms = m_queryCount > 0 ? m_totalQueryTimeMs / m_queryCount : 0.0;
        
        for (const auto& pair : m_indexes) {
            if (pair.first.find(user_id) == 0) {
                for (const auto& indexPair : pair.second) {
                    stats.indexed_alerts += static_cast<int>(indexPair.second.size());
                }
            }
        }
        
        return stats;
    }
    
    void PrefetchPages(const std::string& user_id, int start_page, int end_page, int page_size) {
        for (int page = start_page; page <= end_page; ++page) {
            std::string cacheKey = "page_" + user_id + "_" + std::to_string(page) + "_" + std::to_string(page_size);
            
            AlertQueryParams params;
            params.user_id = user_id;
            params.page = page;
            params.page_size = page_size;
            
            auto result = GetAlertList(params);
            
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& alert : result.alerts) {
                m_cache[cacheKey + "_" + alert->alert_id] = alert;
            }
        }
        
        LOG_INFO() << "Prefetched pages " << start_page << "-" << end_page << " for user " << user_id;
    }
    
    void ClearCache() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache.clear();
        m_cacheHits = 0;
        m_cacheMisses = 0;
        LOG_INFO() << "Query cache cleared";
    }
    
private:
    AlertListResult GetAlertListByCursorInternal(const std::string& user_id, 
                                                  const PageCursor& cursor, 
                                                  int limit) {
        AlertListResult result;
        
        auto allAlerts = GetActiveAlerts(user_id);
        
        std::sort(allAlerts.begin(), allAlerts.end(), 
            [](const AlertPtr& a, const AlertPtr& b) {
                return a->issue_time > b->issue_time;
            });
        
        size_t startIndex = 0;
        if (!cursor.last_id.empty()) {
            for (size_t i = 0; i < allAlerts.size(); ++i) {
                if (allAlerts[i]->alert_id == cursor.last_id) {
                    startIndex = i + 1;
                    break;
                }
            }
        }
        
        size_t endIndex = std::min(startIndex + static_cast<size_t>(limit), allAlerts.size());
        
        for (size_t i = startIndex; i < endIndex; ++i) {
            result.alerts.push_back(allAlerts[i]);
        }
        
        result.total_count = static_cast<int>(allAlerts.size());
        result.page = 0;
        result.page_size = limit;
        
        return result;
    }
    
    AlertPtr GetFromCache(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    void PutToCache(const std::string& key, AlertPtr alert) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cache[key] = alert;
    }
    
    void UpdateQueryStats(double elapsedMs) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_totalQueryTimeMs += elapsedMs;
        m_queryCount++;
    }
    
    std::shared_ptr<IAlertRepository> m_repository;
    std::map<std::string, AlertPtr> m_cache;
    std::map<std::string, std::map<std::string, std::vector<AlertPtr>>> m_indexes;
    mutable std::mutex m_mutex;
    int m_cacheHits;
    int m_cacheMisses;
    double m_totalQueryTimeMs;
    int m_queryCount;
};

QueryService::QueryService() 
    : m_impl(std::make_unique<Impl>()) {
}

QueryService::QueryService(std::shared_ptr<IAlertRepository> repository) 
    : m_impl(std::make_unique<Impl>(repository)) {
}

QueryService::~QueryService() {
}

AlertListResult QueryService::GetAlertList(const AlertQueryParams& params) {
    return m_impl->GetAlertList(params);
}

AlertPtr QueryService::GetAlertDetail(const std::string& alert_id) {
    return m_impl->GetAlertDetail(alert_id);
}

AlertListResult QueryService::GetAlertHistory(const std::string& user_id,
                                               const DateTime& start_time,
                                               const DateTime& end_time,
                                               int page, int page_size) {
    return m_impl->GetAlertHistory(user_id, start_time, end_time, page, page_size);
}

std::vector<AlertPtr> QueryService::GetActiveAlerts(const std::string& user_id) {
    return m_impl->GetActiveAlerts(user_id);
}

std::vector<AlertPtr> QueryService::GetAlertsByType(const std::string& user_id, AlertType type) {
    return m_impl->GetAlertsByType(user_id, type);
}

std::vector<AlertPtr> QueryService::GetAlertsByLevel(const std::string& user_id, AlertLevel level) {
    return m_impl->GetAlertsByLevel(user_id, level);
}

int QueryService::GetActiveAlertCount(const std::string& user_id) {
    return m_impl->GetActiveAlertCount(user_id);
}

int QueryService::GetAlertCountByType(const std::string& user_id, AlertType type) {
    return m_impl->GetAlertCountByType(user_id, type);
}

std::vector<AlertPtr> QueryService::SearchAlerts(const std::string& user_id, const std::string& keyword) {
    return m_impl->SearchAlerts(user_id, keyword);
}

AlertListResult QueryService::GetAlertListOptimized(const OptimizedQueryParams& params) {
    return m_impl->GetAlertListOptimized(params);
}

AlertListResult QueryService::GetAlertListByCursor(const std::string& user_id, 
                                                    const PageCursor& cursor, 
                                                    int limit) {
    return m_impl->GetAlertListByCursor(user_id, cursor, limit);
}

PageCursor QueryService::GetFirstPageCursor(const std::string& user_id) {
    return m_impl->GetFirstPageCursor(user_id);
}

void QueryService::BuildIndex(const std::string& user_id, const std::string& field) {
    m_impl->BuildIndex(user_id, field);
}

void QueryService::DropIndex(const std::string& user_id, const std::string& field) {
    m_impl->DropIndex(user_id, field);
}

QueryIndexStats QueryService::GetIndexStats(const std::string& user_id) {
    return m_impl->GetIndexStats(user_id);
}

void QueryService::PrefetchPages(const std::string& user_id, int start_page, int end_page, int page_size) {
    m_impl->PrefetchPages(user_id, start_page, end_page, page_size);
}

void QueryService::ClearCache() {
    m_impl->ClearCache();
}

std::unique_ptr<IQueryService> IQueryService::Create() {
    return std::unique_ptr<IQueryService>(new QueryService());
}

}
}
