#include "ogc/alert/alert_engine.h"
#include "ogc/alert/alert_checker.h"
#include "ogc/alert/alert_processor.h"
#include "ogc/alert/scheduler.h"
#include "ogc/base/log.h"
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <queue>

using ogc::base::LogLevel;
using ogc::base::LogHelper;

namespace ogc {
namespace alert {

class AlertEngine::Impl {
public:
    Impl() : m_running(false), m_checkIntervalMs(5000) {}
    
    EngineConfig m_config;
    std::map<std::string, IAlertCheckerPtr> m_checkers;
    IAlertProcessorPtr m_processor;
    ISchedulerPtr m_scheduler;
    AlertStatistics m_statistics;
    
    std::atomic<bool> m_running;
    int m_checkIntervalMs;
    std::mutex m_mutex;
};

AlertEngine::AlertEngine() : m_impl(std::make_unique<Impl>()) {
}

AlertEngine::~AlertEngine() {
    Stop();
}

void AlertEngine::Initialize(const EngineConfig& config) {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    m_impl->m_config = config;
    m_impl->m_checkIntervalMs = config.check_interval_ms;
    
    if (!m_impl->m_scheduler) {
        m_impl->m_scheduler = IScheduler::Create();
    }
    
    m_impl->m_statistics = AlertStatistics{};
}

void AlertEngine::Start() {
    if (m_impl->m_running) {
        return;
    }
    
    m_impl->m_running = true;
    m_impl->m_scheduler->Start();
    
    m_impl->m_scheduler->ScheduleTask([this]() {
        RunCheck();
    }, m_impl->m_checkIntervalMs, 0);
    
    LOG_INFO() << "AlertEngine started";
}

void AlertEngine::Stop() {
    if (!m_impl->m_running) {
        return;
    }
    
    m_impl->m_running = false;
    m_impl->m_scheduler->Stop();
    
    LOG_INFO() << "AlertEngine stopped";
}

bool AlertEngine::IsRunning() const {
    return m_impl->m_running;
}

void AlertEngine::RegisterChecker(IAlertCheckerPtr checker) {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    if (checker) {
        m_impl->m_checkers[checker->GetCheckerId()] = checker;
    }
}

void AlertEngine::UnregisterChecker(const std::string& checker_id) {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    m_impl->m_checkers.erase(checker_id);
}

IAlertCheckerPtr AlertEngine::GetChecker(const std::string& checker_id) const {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    auto it = m_impl->m_checkers.find(checker_id);
    if (it != m_impl->m_checkers.end()) {
        return it->second;
    }
    return nullptr;
}

void AlertEngine::SetProcessor(IAlertProcessorPtr processor) {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    m_impl->m_processor = processor;
}

IAlertProcessorPtr AlertEngine::GetProcessor() const {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    return m_impl->m_processor;
}

void AlertEngine::SubmitEvent(const AlertEvent& event) {
    m_impl->m_scheduler->SubmitEvent([this, event]() {
        ProcessEvent(event);
    }, event.priority);
}

AlertStatistics AlertEngine::GetStatistics() const {
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    return m_impl->m_statistics;
}

void AlertEngine::RunCheck() {
    CheckContext context;
    context.check_time = DateTime::Now();
    
    std::vector<AlertPtr> allAlerts;
    
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    for (auto& pair : m_impl->m_checkers) {
        auto& checker = pair.second;
        if (checker && checker->IsEnabled()) {
            auto alerts = checker->Check(context);
            allAlerts.insert(allAlerts.end(), alerts.begin(), alerts.end());
        }
    }
    
    if (m_impl->m_processor && !allAlerts.empty()) {
        allAlerts = m_impl->m_processor->Process(allAlerts);
    }
    
    m_impl->m_statistics.last_check_time = context.check_time;
    m_impl->m_statistics.total_alerts_generated += static_cast<int>(allAlerts.size());
}

void AlertEngine::ProcessEvent(const AlertEvent& event) {
    CheckContext context;
    context.check_time = event.timestamp;
    context.ship_position = event.position;
    context.ship_speed = event.speed;
    context.ship_heading = event.heading;
    context.ship_id = event.ship_id;
    context.user_id = event.user_id;
    
    std::vector<AlertPtr> allAlerts;
    
    std::lock_guard<std::mutex> lock(m_impl->m_mutex);
    for (auto& pair : m_impl->m_checkers) {
        auto& checker = pair.second;
        if (checker && checker->IsEnabled()) {
            auto alerts = checker->Check(context);
            allAlerts.insert(allAlerts.end(), alerts.begin(), alerts.end());
        }
    }
    
    if (m_impl->m_processor && !allAlerts.empty()) {
        allAlerts = m_impl->m_processor->Process(allAlerts);
    }
    
    m_impl->m_statistics.total_alerts_generated += static_cast<int>(allAlerts.size());
}

std::unique_ptr<IAlertEngine> IAlertEngine::Create() {
    return std::unique_ptr<IAlertEngine>(new AlertEngine());
}

}
}
