#ifndef OGC_ALERT_SCHEDULER_H
#define OGC_ALERT_SCHEDULER_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

namespace ogc {
namespace alert {

struct ScheduledTaskInfo {
    std::string task_id;
    std::string task_name;
    int interval_ms;
    int priority;
    bool enabled;
    DateTime next_run_time;
    DateTime last_run_time;
    int run_count;
};

class OGC_ALERT_API IScheduler {
public:
    virtual ~IScheduler() = default;
    
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    virtual std::string ScheduleTask(std::function<void()> task, int interval_ms, int priority = 0) = 0;
    virtual void CancelTask(const std::string& task_id) = 0;
    virtual void PauseTask(const std::string& task_id) = 0;
    virtual void ResumeTask(const std::string& task_id) = 0;
    
    virtual std::vector<ScheduledTaskInfo> GetScheduledTasks() const = 0;
    virtual ScheduledTaskInfo GetTask(const std::string& task_id) const = 0;
    
    virtual void SubmitEvent(std::function<void()> event, int priority = 0) = 0;
    
    static std::unique_ptr<IScheduler> Create();
};

class OGC_ALERT_API Scheduler : public IScheduler {
public:
    Scheduler();
    ~Scheduler();
    
    void Start() override;
    void Stop() override;
    bool IsRunning() const override;
    
    std::string ScheduleTask(std::function<void()> task, int interval_ms, int priority) override;
    void CancelTask(const std::string& task_id) override;
    void PauseTask(const std::string& task_id) override;
    void ResumeTask(const std::string& task_id) override;
    
    std::vector<ScheduledTaskInfo> GetScheduledTasks() const override;
    ScheduledTaskInfo GetTask(const std::string& task_id) const override;
    
    void SubmitEvent(std::function<void()> event, int priority) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
