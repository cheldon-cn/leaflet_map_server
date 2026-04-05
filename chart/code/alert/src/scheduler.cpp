#include "ogc/alert/scheduler.h"
#include "ogc/draw/log.h"
#include <map>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <chrono>

using ogc::draw::LogLevel;
using ogc::draw::LogHelper;

namespace ogc {
namespace alert {

struct TaskItem {
    std::string id;
    std::function<void()> func;
    int interval_ms;
    int priority;
    bool enabled;
    DateTime next_run_time;
    DateTime last_run_time;
    int run_count;
};

struct EventItem {
    std::function<void()> func;
    int priority;
    bool operator<(const EventItem& other) const {
        return priority < other.priority;
    }
};

class Scheduler::Impl {
public:
    Impl() : m_running(false), m_taskIdCounter(0) {}
    
    ~Impl() {
        Stop();
    }
    
    void Start() {
        if (m_running) return;
        m_running = true;
        m_thread = std::thread(&Impl::RunLoop, this);
    }
    
    void Stop() {
        if (!m_running) return;
        m_running = false;
        m_condition.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
    
    std::string ScheduleTask(std::function<void()> task, int interval_ms, int priority) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::string id = "task_" + std::to_string(++m_taskIdCounter);
        TaskItem item;
        item.id = id;
        item.func = task;
        item.interval_ms = interval_ms;
        item.priority = priority;
        item.enabled = true;
        item.next_run_time = DateTime::Now();
        item.run_count = 0;
        
        m_tasks[id] = item;
        return id;
    }
    
    void CancelTask(const std::string& task_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.erase(task_id);
    }
    
    void PauseTask(const std::string& task_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_tasks.find(task_id);
        if (it != m_tasks.end()) {
            it->second.enabled = false;
        }
    }
    
    void ResumeTask(const std::string& task_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_tasks.find(task_id);
        if (it != m_tasks.end()) {
            it->second.enabled = true;
            it->second.next_run_time = DateTime::Now();
        }
    }
    
    void SubmitEvent(std::function<void()> event, int priority) {
        std::lock_guard<std::mutex> lock(m_mutex);
        EventItem item;
        item.func = event;
        item.priority = priority;
        m_eventQueue.push(item);
        m_condition.notify_one();
    }
    
    std::vector<ScheduledTaskInfo> GetScheduledTasks() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<ScheduledTaskInfo> result;
        for (const auto& pair : m_tasks) {
            const TaskItem& item = pair.second;
            ScheduledTaskInfo task;
            task.task_id = item.id;
            task.task_name = item.id;
            task.interval_ms = item.interval_ms;
            task.priority = item.priority;
            task.enabled = item.enabled;
            task.next_run_time = item.next_run_time;
            task.last_run_time = item.last_run_time;
            task.run_count = item.run_count;
            result.push_back(task);
        }
        return result;
    }
    
    ScheduledTaskInfo GetTask(const std::string& task_id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_tasks.find(task_id);
        if (it != m_tasks.end()) {
            const TaskItem& item = it->second;
            ScheduledTaskInfo task;
            task.task_id = item.id;
            task.task_name = item.id;
            task.interval_ms = item.interval_ms;
            task.priority = item.priority;
            task.enabled = item.enabled;
            task.next_run_time = item.next_run_time;
            task.last_run_time = item.last_run_time;
            task.run_count = item.run_count;
            return task;
        }
        return ScheduledTaskInfo();
    }
    
    bool IsRunning() const { return m_running; }
    
private:
    void RunLoop() {
        while (m_running) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait_for(lock, std::chrono::milliseconds(100));
            
            if (!m_running) break;
            
            while (!m_eventQueue.empty()) {
                EventItem event = m_eventQueue.top();
                m_eventQueue.pop();
                lock.unlock();
                try {
                    event.func();
                } catch (const std::exception& e) {
                    LOG_ERROR() << "Event execution error: " << e.what();
                }
                lock.lock();
            }
            
            DateTime now = DateTime::Now();
            for (auto& pair : m_tasks) {
                TaskItem& task = pair.second;
                if (task.enabled && task.next_run_time.ToTimestamp() <= now.ToTimestamp()) {
                    lock.unlock();
                    try {
                        task.func();
                    } catch (const std::exception& e) {
                        LOG_ERROR() << "Task execution error: " << e.what();
                    }
                    lock.lock();
                    
                    task.last_run_time = now;
                    task.run_count++;
                    task.next_run_time = DateTime::FromTimestamp(now.ToTimestamp() + task.interval_ms / 1000);
                }
            }
        }
    }
    
    std::atomic<bool> m_running;
    std::map<std::string, TaskItem> m_tasks;
    std::priority_queue<EventItem> m_eventQueue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_thread;
    int m_taskIdCounter;
};

Scheduler::Scheduler() : m_impl(std::make_unique<Impl>()) {
}

Scheduler::~Scheduler() {
}

void Scheduler::Start() {
    m_impl->Start();
}

void Scheduler::Stop() {
    m_impl->Stop();
}

bool Scheduler::IsRunning() const {
    return m_impl->IsRunning();
}

std::string Scheduler::ScheduleTask(std::function<void()> task, int interval_ms, int priority) {
    return m_impl->ScheduleTask(task, interval_ms, priority);
}

void Scheduler::CancelTask(const std::string& task_id) {
    m_impl->CancelTask(task_id);
}

void Scheduler::PauseTask(const std::string& task_id) {
    m_impl->PauseTask(task_id);
}

void Scheduler::ResumeTask(const std::string& task_id) {
    m_impl->ResumeTask(task_id);
}

std::vector<ScheduledTaskInfo> Scheduler::GetScheduledTasks() const {
    return m_impl->GetScheduledTasks();
}

ScheduledTaskInfo Scheduler::GetTask(const std::string& task_id) const {
    return m_impl->GetTask(task_id);
}

void Scheduler::SubmitEvent(std::function<void()> event, int priority) {
    m_impl->SubmitEvent(event, priority);
}

std::unique_ptr<IScheduler> IScheduler::Create() {
    return std::unique_ptr<IScheduler>(new Scheduler());
}

}
}
