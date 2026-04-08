#ifndef OGC_ALERT_DEDUPLICATOR_H
#define OGC_ALERT_DEDUPLICATOR_H

#include "export.h"
#include "types.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <mutex>
#include <chrono>

namespace ogc {
namespace alert {

struct DeduplicationConfig {
    bool enabled;
    int time_window_seconds;
    double spatial_threshold;
    bool merge_similar;
    int max_duplicate_count;
    bool suppress_repeated;
    int suppress_interval_seconds;
};

struct AlertSignature {
    AlertType type;
    std::string source_id;
    Coordinate position;
    double position_tolerance;
    std::string target_id;
    
    bool operator<(const AlertSignature& other) const;
    bool operator==(const AlertSignature& other) const;
};

struct DuplicateGroup {
    AlertSignature signature;
    std::vector<AlertPtr> alerts;
    AlertPtr representative;
    DateTime first_occurrence;
    DateTime last_occurrence;
    int occurrence_count;
};

class OGC_ALERT_API AlertDeduplicator {
public:
    AlertDeduplicator();
    explicit AlertDeduplicator(const DeduplicationConfig& config);
    ~AlertDeduplicator();
    
    void SetConfig(const DeduplicationConfig& config);
    DeduplicationConfig GetConfig() const;
    
    AlertPtr Process(AlertPtr alert);
    std::vector<AlertPtr> ProcessBatch(const std::vector<AlertPtr>& alerts);
    
    bool IsDuplicate(const AlertPtr& alert) const;
    bool IsSuppressed(const AlertPtr& alert) const;
    
    std::vector<DuplicateGroup> GetDuplicateGroups() const;
    void ClearHistory();
    
    size_t GetProcessedCount() const;
    size_t GetDuplicateCount() const;
    size_t GetSuppressedCount() const;
    void ResetStatistics();
    
    void SetTimeWindow(int seconds);
    void SetSpatialThreshold(double threshold);
    void EnableMerge(bool enable);
    void EnableSuppress(bool enable, int interval_seconds = 300);

private:
    DeduplicationConfig m_config;
    mutable std::mutex m_mutex;
    
    std::map<AlertSignature, DuplicateGroup> m_duplicateGroups;
    std::map<AlertSignature, DateTime> m_lastAlertTime;
    
    size_t m_processedCount;
    size_t m_duplicateCount;
    size_t m_suppressedCount;
    
    AlertSignature ComputeSignature(const AlertPtr& alert) const;
    bool IsInTimeWindow(const DateTime& current, const DateTime& previous) const;
    bool IsInSpatialWindow(const Coordinate& a, const Coordinate& b) const;
    AlertPtr MergeAlerts(const std::vector<AlertPtr>& alerts) const;
    bool ShouldSuppress(const AlertSignature& signature) const;
    void UpdateGroup(const AlertSignature& signature, AlertPtr alert);
    void CleanupExpiredGroups();
};

class OGC_ALERT_API AlertAggregator {
public:
    enum class AggregationStrategy {
        kNone,
        kByType,
        kByLocation,
        kByTime,
        kBySeverity,
        kBySource,
        kCombined
    };
    
    struct AggregationConfig {
        AggregationStrategy strategy;
        int time_window_seconds;
        double spatial_threshold;
        int max_group_size;
        bool escalate_severity;
    };
    
    struct AggregatedAlert {
        AlertPtr primary_alert;
        std::vector<AlertPtr> merged_alerts;
        int merge_count;
        AlertLevel max_level;
        AlertLevel min_level;
        DateTime earliest_time;
        DateTime latest_time;
        std::set<std::string> affected_sources;
    };
    
    AlertAggregator();
    explicit AlertAggregator(const AggregationConfig& config);
    ~AlertAggregator();
    
    void SetConfig(const AggregationConfig& config);
    AggregationConfig GetConfig() const;
    
    std::vector<AggregatedAlert> Aggregate(const std::vector<AlertPtr>& alerts);
    
    void SetStrategy(AggregationStrategy strategy);
    AggregationStrategy GetStrategy() const;
    
    void Clear();
    size_t GetGroupCount() const;

private:
    AggregationConfig m_config;
    mutable std::mutex m_mutex;
    std::map<std::string, AggregatedAlert> m_groups;
    
    std::string ComputeGroupKey(const AlertPtr& alert) const;
    void AddToGroup(const std::string& key, AlertPtr alert);
    AggregatedAlert FinalizeGroup(const std::string& key) const;
};

class OGC_ALERT_API AlertFilter {
public:
    struct FilterRule {
        enum class Type {
            kInclude,
            kExclude,
            kThreshold,
            kTimeRange,
            kLocation
        };
        
        Type type;
        AlertType alert_type;
        AlertLevel min_level;
        AlertLevel max_level;
        std::string source_pattern;
        DateTime start_time;
        DateTime end_time;
        Coordinate center;
        double radius;
    };
    
    AlertFilter();
    ~AlertFilter();
    
    void AddRule(const FilterRule& rule);
    void RemoveRule(size_t index);
    void ClearRules();
    std::vector<FilterRule> GetRules() const;
    
    bool Matches(const AlertPtr& alert) const;
    std::vector<AlertPtr> Filter(const std::vector<AlertPtr>& alerts) const;
    
    void SetDefaultAction(bool include_by_default);
    bool GetDefaultAction() const;

private:
    std::vector<FilterRule> m_rules;
    bool m_defaultInclude;
    
    bool EvaluateRule(const FilterRule& rule, const AlertPtr& alert) const;
};

}
}

#endif
