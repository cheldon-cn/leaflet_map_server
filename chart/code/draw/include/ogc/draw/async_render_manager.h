#ifndef OGC_DRAW_ASYNC_RENDER_MANAGER_H
#define OGC_DRAW_ASYNC_RENDER_MANAGER_H

#include <ogc/draw/export.h>
#include <ogc/draw/async_render_task.h>
#include <ogc/draw/draw_device.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/geometry.h>

#include <memory>
#include <vector>
#include <queue>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace ogc {
namespace draw {

enum class TaskPriority {
    kLow = 0,
    kNormal = 1,
    kHigh = 2,
    kUrgent = 3
};

struct TaskQueueEntry {
    std::shared_ptr<AsyncRenderTask> task;
    TaskPriority priority;
    uint64_t submitTime;
    
    bool operator<(const TaskQueueEntry& other) const {
        if (priority != other.priority) {
            return static_cast<int>(priority) < static_cast<int>(other.priority);
        }
        return submitTime > other.submitTime;
    }
};

class OGC_DRAW_API AsyncRenderManager {
public:
    static AsyncRenderManager& Instance();
    
    std::shared_ptr<AsyncRenderTask> SubmitRenderTask(
        DrawDevice* device,
        DrawEngine* engine,
        const std::vector<Geometry*>& geometries,
        const DrawStyle& style,
        TaskPriority priority = TaskPriority::kNormal
    );
    
    void SetMaxConcurrentTasks(int maxTasks);
    int GetMaxConcurrentTasks() const { return m_maxConcurrentTasks; }
    int GetActiveTaskCount() const;
    int GetPendingTaskCount() const;
    
    void CancelAllTasks();
    void WaitForAll(uint32_t timeoutMs = 0);
    
    void Pause();
    void Resume();
    bool IsPaused() const;
    
    void Shutdown();

private:
    AsyncRenderManager();
    ~AsyncRenderManager();
    
    AsyncRenderManager(const AsyncRenderManager&) = delete;
    AsyncRenderManager& operator=(const AsyncRenderManager&) = delete;
    
    void WorkerThread();
    void ProcessTask(std::shared_ptr<AsyncRenderTask> task);
    
    std::priority_queue<TaskQueueEntry> m_pendingQueue;
    std::set<std::shared_ptr<AsyncRenderTask>> m_activeTasks;
    std::vector<std::thread> m_workers;
    
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    
    int m_maxConcurrentTasks;
    bool m_paused;
    bool m_shutdown;
    uint64_t m_nextSubmitTime;
};

} 
} 

#endif 
