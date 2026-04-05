#include "alert/alert_judge_service.h"
#include "alert/alert_engine_factory.h"
#include "alert/alert_exception.h"
#include <algorithm>

namespace alert {

AlertJudgeService::AlertJudgeService()
    : m_maxAlertCount(10)
    , m_filter(nullptr) {
    AlertEngineFactory& factory = AlertEngineFactory::GetInstance();
    
    std::vector<AlertType> types = {
        AlertType::kDepthAlert,
        AlertType::kCollisionAlert,
        AlertType::kWeatherAlert,
        AlertType::kChannelAlert
    };
    
    for (AlertType type : types) {
        if (factory.IsEngineRegistered(type)) {
            IAlertEngine* engine = factory.CreateEngine(type);
            if (engine) {
                m_engines[type] = engine;
                m_enabledEngines[type] = true;
            }
        }
    }
}

AlertJudgeService::~AlertJudgeService() {
    for (auto& pair : m_engines) {
        delete pair.second;
    }
    m_engines.clear();
}

std::vector<Alert> AlertJudgeService::Evaluate(const AlertContext& context) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    std::vector<Alert> results;
    
    for (const auto& pair : m_engines) {
        if (m_enabledEngines[pair.first]) {
            try {
                Alert alert = pair.second->Evaluate(context);
                if (ShouldIncludeAlert(alert)) {
                    results.push_back(alert);
                }
            } catch (const std::exception& e) {
            }
        }
    }
    
    results = SortAlertsByPriority(results);
    
    if (results.size() > m_maxAlertCount) {
        results.resize(m_maxAlertCount);
    }
    
    return results;
}

std::vector<Alert> AlertJudgeService::EvaluateByType(const AlertContext& context, AlertType type) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    std::vector<Alert> results;
    
    auto it = m_engines.find(type);
    if (it != m_engines.end() && m_enabledEngines[type]) {
        try {
            Alert alert = it->second->Evaluate(context);
            if (ShouldIncludeAlert(alert)) {
                results.push_back(alert);
            }
        } catch (const std::exception& e) {
        }
    }
    
    return results;
}

std::vector<Alert> AlertJudgeService::EvaluateByTypes(const AlertContext& context, const std::vector<AlertType>& types) {
    if (!context.IsValid()) {
        throw InvalidParameterException("Invalid alert context");
    }
    
    std::vector<Alert> results;
    
    for (AlertType type : types) {
        auto it = m_engines.find(type);
        if (it != m_engines.end() && m_enabledEngines[type]) {
            try {
                Alert alert = it->second->Evaluate(context);
                if (ShouldIncludeAlert(alert)) {
                    results.push_back(alert);
                }
            } catch (const std::exception& e) {
            }
        }
    }
    
    results = SortAlertsByPriority(results);
    
    if (results.size() > m_maxAlertCount) {
        results.resize(m_maxAlertCount);
    }
    
    return results;
}

void AlertJudgeService::EnableEngine(AlertType type, bool enable) {
    m_enabledEngines[type] = enable;
}

bool AlertJudgeService::IsEngineEnabled(AlertType type) const {
    auto it = m_enabledEngines.find(type);
    if (it != m_enabledEngines.end()) {
        return it->second;
    }
    return false;
}

void AlertJudgeService::SetEngineThreshold(AlertType type, const ThresholdConfig& config) {
    m_thresholds[type] = config;
    
    auto it = m_engines.find(type);
    if (it != m_engines.end()) {
        it->second->SetThreshold(config);
    }
}

ThresholdConfig AlertJudgeService::GetEngineThreshold(AlertType type) const {
    auto it = m_thresholds.find(type);
    if (it != m_thresholds.end()) {
        return it->second;
    }
    return ThresholdConfig();
}

std::vector<AlertType> AlertJudgeService::GetSupportedTypes() const {
    std::vector<AlertType> types;
    for (const auto& pair : m_engines) {
        types.push_back(pair.first);
    }
    return types;
}

void AlertJudgeService::RegisterEngine(AlertType type, IAlertEngine* engine) {
    if (engine == nullptr) {
        throw InvalidParameterException("Engine cannot be null");
    }
    
    auto it = m_engines.find(type);
    if (it != m_engines.end()) {
        delete it->second;
    }
    
    m_engines[type] = engine;
    m_enabledEngines[type] = true;
}

void AlertJudgeService::UnregisterEngine(AlertType type) {
    auto it = m_engines.find(type);
    if (it != m_engines.end()) {
        delete it->second;
        m_engines.erase(it);
        m_enabledEngines.erase(type);
        m_thresholds.erase(type);
    }
}

void AlertJudgeService::SetMaxAlertCount(size_t count) {
    m_maxAlertCount = count;
}

size_t AlertJudgeService::GetMaxAlertCount() const {
    return m_maxAlertCount;
}

void AlertJudgeService::SetAlertFilter(std::function<bool(const Alert&)> filter) {
    m_filter = filter;
}

bool AlertJudgeService::ShouldIncludeAlert(const Alert& alert) const {
    if (alert.GetAlertLevel() == AlertLevel::kNone) {
        return false;
    }
    
    if (m_filter) {
        return m_filter(alert);
    }
    
    return true;
}

std::vector<Alert> AlertJudgeService::SortAlertsByPriority(const std::vector<Alert>& alerts) const {
    std::vector<Alert> sorted = alerts;
    
    std::sort(sorted.begin(), sorted.end(), [](const Alert& a, const Alert& b) {
        int priorityA = AlertLevelToPriority(a.GetAlertLevel());
        int priorityB = AlertLevelToPriority(b.GetAlertLevel());
        return priorityA > priorityB;
    });
    
    return sorted;
}

}
