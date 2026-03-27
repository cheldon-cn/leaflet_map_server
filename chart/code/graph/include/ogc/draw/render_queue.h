#ifndef OGC_DRAW_RENDER_QUEUE_H
#define OGC_DRAW_RENDER_QUEUE_H

#include "ogc/draw/export.h"
#include "ogc/draw/render_task.h"
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace ogc {
namespace draw {

class RenderQueue;
typedef std::shared_ptr<RenderQueue> RenderQueuePtr;

struct RenderQueueStats {
    size_t totalTasks;
    size_t pendingTasks;
    size_t runningTasks;
    size_t completedTasks;
    size_t failedTasks;
    size_t cancelledTasks;
    int64_t totalWaitTime;
    int64_t totalRenderTime;
    
    RenderQueueStats()
        : totalTasks(0)
        , pendingTasks(0)
        , runningTasks(0)
        , completedTasks(0)
        , failedTasks(0)
        , cancelledTasks(0)
        , totalWaitTime(0)
        , totalRenderTime(0) {}
};

typedef std::function<void(const RenderTaskPtr&)> TaskEventHandler;

class OGC_GRAPH_API RenderQueue {
public:
    RenderQueue();
    explicit RenderQueue(size_t maxQueueSize);
    ~RenderQueue();
    
    bool Enqueue(RenderTaskPtr task);
    RenderTaskPtr Dequeue();
    RenderTaskPtr TryDequeue(int64_t timeoutMs = 0);
    
    RenderTaskPtr Peek() const;
    
    bool IsEmpty() const;
    size_t GetSize() const;
    size_t GetMaxSize() const;
    void SetMaxSize(size_t maxSize);
    
    void Clear();
    
    void CancelAll();
    
    void RemoveTask(const std::string& taskId);
    RenderTaskPtr GetTask(const std::string& taskId) const;
    
    bool HasTask(const std::string& taskId) const;
    
    RenderQueueStats GetStats() const;
    void ResetStats();
    
    void SetOnTaskEnqueued(TaskEventHandler handler);
    void SetOnTaskDequeued(TaskEventHandler handler);
    void SetOnTaskCompleted(TaskEventHandler handler);
    void SetOnTaskFailed(TaskEventHandler handler);
    
    void SetPriorityMode(bool enable);
    bool IsPriorityModeEnabled() const;
    
    void Pause();
    void Resume();
    bool IsPaused() const;
    
    void WaitForNotEmpty();
    bool WaitForNotEmpty(int64_t timeoutMs);
    
    void NotifyAll();
    
    static RenderQueuePtr Create();
    static RenderQueuePtr Create(size_t maxQueueSize);

private:
    void UpdateStats();
    void NotifyEnqueued(const RenderTaskPtr& task);
    void NotifyDequeued(const RenderTaskPtr& task);
    
    struct TaskComparator {
        bool operator()(const RenderTaskPtr& a, const RenderTaskPtr& b) const {
            return a->GetPriorityValue() < b->GetPriorityValue();
        }
    };
    
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::priority_queue<RenderTaskPtr, std::vector<RenderTaskPtr>, TaskComparator> m_queue;
    std::vector<RenderTaskPtr> m_allTasks;
    size_t m_maxSize;
    bool m_priorityMode;
    bool m_paused;
    RenderQueueStats m_stats;
    
    TaskEventHandler m_onTaskEnqueued;
    TaskEventHandler m_onTaskDequeued;
    TaskEventHandler m_onTaskCompleted;
    TaskEventHandler m_onTaskFailed;
};

}
}

#endif
