#ifndef OGC_ALERT_ENGINE_H
#define OGC_ALERT_ENGINE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ogc {
namespace alert {

struct EngineConfig {
    int check_interval_ms;
    int max_concurrent_checks;
    bool enable_deduplication;
    bool enable_aggregation;
    int dedup_window_seconds;
};

struct AlertStatistics {
    int total_alerts_generated;
    int total_alerts_pushed;
    int total_alerts_acknowledged;
    int active_alerts;
    DateTime last_check_time;
};

struct CheckContext {
    DateTime check_time;
    Coordinate ship_position;
    double ship_speed;
    double ship_heading;
    std::string ship_id;
    std::string user_id;
};

struct AlertEvent {
    std::string event_id;
    std::string event_type;
    DateTime timestamp;
    Coordinate position;
    double speed;
    double heading;
    std::string ship_id;
    std::string user_id;
    int priority;
    std::string payload;
};

class IAlertChecker;
using IAlertCheckerPtr = std::shared_ptr<IAlertChecker>;

class IAlertProcessor;
using IAlertProcessorPtr = std::shared_ptr<IAlertProcessor>;

class IScheduler;
using ISchedulerPtr = std::shared_ptr<IScheduler>;

class OGC_ALERT_API IAlertEngine {
public:
    virtual ~IAlertEngine() = default;
    
    virtual void Initialize(const EngineConfig& config) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    virtual void RegisterChecker(IAlertCheckerPtr checker) = 0;
    virtual void UnregisterChecker(const std::string& checker_id) = 0;
    virtual IAlertCheckerPtr GetChecker(const std::string& checker_id) const = 0;
    
    virtual void SetProcessor(IAlertProcessorPtr processor) = 0;
    virtual IAlertProcessorPtr GetProcessor() const = 0;
    
    virtual void SubmitEvent(const AlertEvent& event) = 0;
    virtual AlertStatistics GetStatistics() const = 0;
    
    static std::unique_ptr<IAlertEngine> Create();
};

class OGC_ALERT_API AlertEngine : public IAlertEngine {
public:
    AlertEngine();
    ~AlertEngine();
    
    void Initialize(const EngineConfig& config) override;
    void Start() override;
    void Stop() override;
    bool IsRunning() const override;
    
    void RegisterChecker(IAlertCheckerPtr checker) override;
    void UnregisterChecker(const std::string& checker_id) override;
    IAlertCheckerPtr GetChecker(const std::string& checker_id) const override;
    
    void SetProcessor(IAlertProcessorPtr processor) override;
    IAlertProcessorPtr GetProcessor() const override;
    
    void SubmitEvent(const AlertEvent& event) override;
    AlertStatistics GetStatistics() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    
    void RunCheck();
    void ProcessEvent(const AlertEvent& event);
};

}
}

#endif
