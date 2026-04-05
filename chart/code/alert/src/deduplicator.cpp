#include "ogc/alert/deduplicator.h"
#include "ogc/alert/exception.h"
#include <algorithm>
#include <cmath>

namespace ogc {
namespace alert {

bool AlertSignature::operator<(const AlertSignature& other) const {
    if (static_cast<int>(type) != static_cast<int>(other.type)) {
        return static_cast<int>(type) < static_cast<int>(other.type);
    }
    if (source_id != other.source_id) {
        return source_id < other.source_id;
    }
    if (target_id != other.target_id) {
        return target_id < other.target_id;
    }
    if (std::abs(position.latitude - other.position.latitude) > 1e-6) {
        return position.latitude < other.position.latitude;
    }
    return position.longitude < other.position.longitude;
}

bool AlertSignature::operator==(const AlertSignature& other) const {
    return type == other.type &&
           source_id == other.source_id &&
           target_id == other.target_id &&
           std::abs(position.latitude - other.position.latitude) < 1e-6 &&
           std::abs(position.longitude - other.position.longitude) < 1e-6;
}

AlertDeduplicator::AlertDeduplicator()
    : m_processedCount(0)
    , m_duplicateCount(0)
    , m_suppressedCount(0) {
    m_config.enabled = true;
    m_config.time_window_seconds = 300;
    m_config.spatial_threshold = 0.001;
    m_config.merge_similar = true;
    m_config.max_duplicate_count = 10;
    m_config.suppress_repeated = true;
    m_config.suppress_interval_seconds = 60;
}

AlertDeduplicator::AlertDeduplicator(const DeduplicationConfig& config)
    : m_config(config)
    , m_processedCount(0)
    , m_duplicateCount(0)
    , m_suppressedCount(0) {
}

AlertDeduplicator::~AlertDeduplicator() {
}

void AlertDeduplicator::SetConfig(const DeduplicationConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
}

DeduplicationConfig AlertDeduplicator::GetConfig() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_config;
}

AlertPtr AlertDeduplicator::Process(AlertPtr alert) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_config.enabled) {
        m_processedCount++;
        return alert;
    }
    
    AlertSignature signature = ComputeSignature(alert);
    
    CleanupExpiredGroups();
    
    if (ShouldSuppress(signature)) {
        m_suppressedCount++;
        return nullptr;
    }
    
    auto it = m_duplicateGroups.find(signature);
    if (it != m_duplicateGroups.end()) {
        if (IsInTimeWindow(alert->issue_time, it->second.last_occurrence)) {
            m_duplicateCount++;
            
            if (m_config.merge_similar) {
                UpdateGroup(signature, alert);
                return it->second.representative;
            }
            
            return nullptr;
        }
    }
    
    UpdateGroup(signature, alert);
    m_processedCount++;
    
    m_lastAlertTime[signature] = alert->issue_time;
    
    return alert;
}

std::vector<AlertPtr> AlertDeduplicator::ProcessBatch(const std::vector<AlertPtr>& alerts) {
    std::vector<AlertPtr> result;
    result.reserve(alerts.size());
    
    for (auto& alert : alerts) {
        auto processed = Process(alert);
        if (processed) {
            result.push_back(processed);
        }
    }
    
    return result;
}

bool AlertDeduplicator::IsDuplicate(const AlertPtr& alert) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_config.enabled) {
        return false;
    }
    
    AlertSignature signature = ComputeSignature(alert);
    auto it = m_duplicateGroups.find(signature);
    
    if (it == m_duplicateGroups.end()) {
        return false;
    }
    
    return IsInTimeWindow(alert->issue_time, it->second.last_occurrence);
}

bool AlertDeduplicator::IsSuppressed(const AlertPtr& alert) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_config.enabled || !m_config.suppress_repeated) {
        return false;
    }
    
    AlertSignature signature = ComputeSignature(alert);
    return ShouldSuppress(signature);
}

std::vector<DuplicateGroup> AlertDeduplicator::GetDuplicateGroups() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<DuplicateGroup> groups;
    groups.reserve(m_duplicateGroups.size());
    
    for (const auto& pair : m_duplicateGroups) {
        groups.push_back(pair.second);
    }
    
    return groups;
}

void AlertDeduplicator::ClearHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_duplicateGroups.clear();
    m_lastAlertTime.clear();
}

size_t AlertDeduplicator::GetProcessedCount() const {
    return m_processedCount;
}

size_t AlertDeduplicator::GetDuplicateCount() const {
    return m_duplicateCount;
}

size_t AlertDeduplicator::GetSuppressedCount() const {
    return m_suppressedCount;
}

void AlertDeduplicator::ResetStatistics() {
    m_processedCount = 0;
    m_duplicateCount = 0;
    m_suppressedCount = 0;
}

void AlertDeduplicator::SetTimeWindow(int seconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.time_window_seconds = seconds;
}

void AlertDeduplicator::SetSpatialThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.spatial_threshold = threshold;
}

void AlertDeduplicator::EnableMerge(bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.merge_similar = enable;
}

void AlertDeduplicator::EnableSuppress(bool enable, int interval_seconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.suppress_repeated = enable;
    m_config.suppress_interval_seconds = interval_seconds;
}

AlertSignature AlertDeduplicator::ComputeSignature(const AlertPtr& alert) const {
    AlertSignature signature;
    signature.type = alert->alert_type;
    signature.source_id = alert->user_id;
    signature.position = alert->position;
    signature.position_tolerance = m_config.spatial_threshold;
    
    switch (alert->alert_type) {
        case AlertType::kCollision: {
            auto collision = std::dynamic_pointer_cast<CollisionAlert>(alert);
            if (collision) {
                signature.target_id = collision->target_ship_id;
            }
            break;
        }
        case AlertType::kDepth: {
            auto depth = std::dynamic_pointer_cast<DepthAlert>(alert);
            if (depth) {
                signature.target_id = std::to_string(static_cast<int>(depth->ukc * 100));
            }
            break;
        }
        default:
            break;
    }
    
    return signature;
}

bool AlertDeduplicator::IsInTimeWindow(const DateTime& current, const DateTime& previous) const {
    int64_t diff = current.ToTimestamp() - previous.ToTimestamp();
    return diff <= m_config.time_window_seconds;
}

bool AlertDeduplicator::IsInSpatialWindow(const Coordinate& a, const Coordinate& b) const {
    double dx = a.longitude - b.longitude;
    double dy = a.latitude - b.latitude;
    double distance = std::sqrt(dx * dx + dy * dy);
    return distance <= m_config.spatial_threshold;
}

AlertPtr AlertDeduplicator::MergeAlerts(const std::vector<AlertPtr>& alerts) const {
    if (alerts.empty()) {
        return nullptr;
    }
    
    if (alerts.size() == 1) {
        return alerts[0];
    }
    
    AlertPtr merged = alerts[0];
    AlertLevel max_level = merged->alert_level;
    
    for (size_t i = 1; i < alerts.size(); ++i) {
        if (alerts[i]->alert_level > max_level) {
            max_level = alerts[i]->alert_level;
        }
    }
    
    merged->alert_level = max_level;
    
    return merged;
}

bool AlertDeduplicator::ShouldSuppress(const AlertSignature& signature) const {
    if (!m_config.suppress_repeated) {
        return false;
    }
    
    auto it = m_lastAlertTime.find(signature);
    if (it == m_lastAlertTime.end()) {
        return false;
    }
    
    DateTime now = DateTime::Now();
    int64_t diff = now.ToTimestamp() - it->second.ToTimestamp();
    
    return diff < m_config.suppress_interval_seconds;
}

void AlertDeduplicator::UpdateGroup(const AlertSignature& signature, AlertPtr alert) {
    auto it = m_duplicateGroups.find(signature);
    
    if (it == m_duplicateGroups.end()) {
        DuplicateGroup group;
        group.signature = signature;
        group.alerts.push_back(alert);
        group.representative = alert;
        group.first_occurrence = alert->issue_time;
        group.last_occurrence = alert->issue_time;
        group.occurrence_count = 1;
        m_duplicateGroups[signature] = group;
    } else {
        it->second.alerts.push_back(alert);
        it->second.last_occurrence = alert->issue_time;
        it->second.occurrence_count++;
        
        if (it->second.alerts.size() > static_cast<size_t>(m_config.max_duplicate_count)) {
            it->second.alerts.erase(it->second.alerts.begin());
        }
        
        if (m_config.merge_similar) {
            it->second.representative = MergeAlerts(it->second.alerts);
        }
    }
}

void AlertDeduplicator::CleanupExpiredGroups() {
    DateTime now = DateTime::Now();
    int64_t expiry_seconds = m_config.time_window_seconds * 2;
    
    for (auto it = m_duplicateGroups.begin(); it != m_duplicateGroups.end(); ) {
        int64_t diff = now.ToTimestamp() - it->second.last_occurrence.ToTimestamp();
        if (diff > expiry_seconds) {
            it = m_duplicateGroups.erase(it);
        } else {
            ++it;
        }
    }
}

AlertAggregator::AlertAggregator() {
    m_config.strategy = AggregationStrategy::kCombined;
    m_config.time_window_seconds = 300;
    m_config.spatial_threshold = 0.01;
    m_config.max_group_size = 100;
    m_config.escalate_severity = true;
}

AlertAggregator::AlertAggregator(const AggregationConfig& config)
    : m_config(config) {
}

AlertAggregator::~AlertAggregator() {
}

void AlertAggregator::SetConfig(const AggregationConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
}

AlertAggregator::AggregationConfig AlertAggregator::GetConfig() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_config;
}

std::vector<AlertAggregator::AggregatedAlert> AlertAggregator::Aggregate(const std::vector<AlertPtr>& alerts) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_groups.clear();
    
    for (const auto& alert : alerts) {
        std::string key = ComputeGroupKey(alert);
        AddToGroup(key, alert);
    }
    
    std::vector<AggregatedAlert> result;
    result.reserve(m_groups.size());
    
    for (const auto& pair : m_groups) {
        result.push_back(FinalizeGroup(pair.first));
    }
    
    return result;
}

void AlertAggregator::SetStrategy(AggregationStrategy strategy) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config.strategy = strategy;
}

AlertAggregator::AggregationStrategy AlertAggregator::GetStrategy() const {
    return m_config.strategy;
}

void AlertAggregator::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groups.clear();
}

size_t AlertAggregator::GetGroupCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groups.size();
}

std::string AlertAggregator::ComputeGroupKey(const AlertPtr& alert) const {
    std::string key;
    
    switch (m_config.strategy) {
        case AggregationStrategy::kByType:
            key = std::to_string(static_cast<int>(alert->alert_type));
            break;
            
        case AggregationStrategy::kByLocation: {
            int lat_bucket = static_cast<int>(alert->position.latitude / m_config.spatial_threshold);
            int lon_bucket = static_cast<int>(alert->position.longitude / m_config.spatial_threshold);
            key = std::to_string(lat_bucket) + "_" + std::to_string(lon_bucket);
            break;
        }
        
        case AggregationStrategy::kByTime: {
            int64_t time_bucket = alert->issue_time.ToTimestamp() / m_config.time_window_seconds;
            key = std::to_string(time_bucket);
            break;
        }
        
        case AggregationStrategy::kBySeverity:
            key = std::to_string(static_cast<int>(alert->alert_level));
            break;
            
        case AggregationStrategy::kBySource:
            key = alert->user_id;
            break;
            
        case AggregationStrategy::kCombined: {
            int lat_bucket = static_cast<int>(alert->position.latitude / m_config.spatial_threshold);
            int lon_bucket = static_cast<int>(alert->position.longitude / m_config.spatial_threshold);
            key = std::to_string(static_cast<int>(alert->alert_type)) + "_" +
                  std::to_string(lat_bucket) + "_" + std::to_string(lon_bucket) + "_" +
                  alert->user_id;
            break;
        }
        
        default:
            key = alert->alert_id;
            break;
    }
    
    return key;
}

void AlertAggregator::AddToGroup(const std::string& key, AlertPtr alert) {
    auto it = m_groups.find(key);
    
    if (it == m_groups.end()) {
        AggregatedAlert group;
        group.primary_alert = alert;
        group.merge_count = 1;
        group.max_level = alert->alert_level;
        group.min_level = alert->alert_level;
        group.earliest_time = alert->issue_time;
        group.latest_time = alert->issue_time;
        group.affected_sources.insert(alert->user_id);
        m_groups[key] = group;
    } else {
        it->second.merged_alerts.push_back(alert);
        it->second.merge_count++;
        
        if (alert->alert_level > it->second.max_level) {
            it->second.max_level = alert->alert_level;
        }
        if (alert->alert_level < it->second.min_level) {
            it->second.min_level = alert->alert_level;
        }
        
        if (alert->issue_time.ToTimestamp() < it->second.earliest_time.ToTimestamp()) {
            it->second.earliest_time = alert->issue_time;
        }
        if (alert->issue_time.ToTimestamp() > it->second.latest_time.ToTimestamp()) {
            it->second.latest_time = alert->issue_time;
        }
        
        it->second.affected_sources.insert(alert->user_id);
        
        if (m_config.escalate_severity && 
            it->second.max_level > it->second.primary_alert->alert_level) {
            it->second.primary_alert->alert_level = it->second.max_level;
        }
    }
}

AlertAggregator::AggregatedAlert AlertAggregator::FinalizeGroup(const std::string& key) const {
    auto it = m_groups.find(key);
    if (it == m_groups.end()) {
        return AggregatedAlert();
    }
    return it->second;
}

AlertFilter::AlertFilter()
    : m_defaultInclude(true) {
}

AlertFilter::~AlertFilter() {
}

void AlertFilter::AddRule(const FilterRule& rule) {
    m_rules.push_back(rule);
}

void AlertFilter::RemoveRule(size_t index) {
    if (index < m_rules.size()) {
        m_rules.erase(m_rules.begin() + index);
    }
}

void AlertFilter::ClearRules() {
    m_rules.clear();
}

std::vector<AlertFilter::FilterRule> AlertFilter::GetRules() const {
    return m_rules;
}

bool AlertFilter::Matches(const AlertPtr& alert) const {
    if (m_rules.empty()) {
        return m_defaultInclude;
    }
    
    bool has_include_rule = false;
    bool matched_include = false;
    
    for (const auto& rule : m_rules) {
        bool matches = EvaluateRule(rule, alert);
        
        if (rule.type == FilterRule::Type::kInclude) {
            has_include_rule = true;
            if (matches) {
                matched_include = true;
            }
        } else if (rule.type == FilterRule::Type::kExclude) {
            if (matches) {
                return false;
            }
        }
    }
    
    if (has_include_rule) {
        return matched_include;
    }
    
    return m_defaultInclude;
}

std::vector<AlertPtr> AlertFilter::Filter(const std::vector<AlertPtr>& alerts) const {
    std::vector<AlertPtr> result;
    result.reserve(alerts.size());
    
    for (const auto& alert : alerts) {
        if (Matches(alert)) {
            result.push_back(alert);
        }
    }
    
    return result;
}

void AlertFilter::SetDefaultAction(bool include_by_default) {
    m_defaultInclude = include_by_default;
}

bool AlertFilter::GetDefaultAction() const {
    return m_defaultInclude;
}

bool AlertFilter::EvaluateRule(const FilterRule& rule, const AlertPtr& alert) const {
    switch (rule.type) {
        case FilterRule::Type::kInclude:
        case FilterRule::Type::kExclude:
            if (rule.alert_type != AlertType::kUnknown && alert->alert_type != rule.alert_type) {
                return false;
            }
            if (alert->alert_level < rule.min_level || alert->alert_level > rule.max_level) {
                return false;
            }
            return true;
            
        case FilterRule::Type::kThreshold:
            if (alert->alert_level < rule.min_level) {
                return false;
            }
            return true;
            
        case FilterRule::Type::kTimeRange:
            if (alert->issue_time.ToTimestamp() < rule.start_time.ToTimestamp() ||
                alert->issue_time.ToTimestamp() > rule.end_time.ToTimestamp()) {
                return false;
            }
            return true;
            
        case FilterRule::Type::kLocation: {
            double dx = alert->position.longitude - rule.center.longitude;
            double dy = alert->position.latitude - rule.center.latitude;
            double distance = std::sqrt(dx * dx + dy * dy);
            return distance <= rule.radius;
        }
            
        default:
            return true;
    }
}

}
}
