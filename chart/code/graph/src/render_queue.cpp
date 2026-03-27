#include "ogc/draw/render_queue.h"
#include <algorithm>

namespace ogc {
namespace draw {

RenderQueue::RenderQueue()
    : m_maxSize(1000)
    , m_priorityMode(true)
    , m_paused(false) {
}

RenderQueue::RenderQueue(size_t maxQueueSize)
    : m_maxSize(maxQueueSize)
    , m_priorityMode(true)
    , m_paused(false) {
}

RenderQueue::~RenderQueue() {
    CancelAll();
}

bool RenderQueue::Enqueue(RenderTaskPtr task) {
    if (!task) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_queue.size() >= m_maxSize) {
        return false;
    }
    
    if (task->IsCancelled()) {
        return false;
    }
    
    task->SetState(RenderTaskState::kQueued);
    m_queue.push(task);
    m_allTasks.push_back(task);
    m_stats.totalTasks++;
    m_stats.pendingTasks++;
    
    NotifyEnqueued(task);
    m_condition.notify_one();
    
    return true;
}

RenderTaskPtr RenderQueue::Dequeue() {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    m_condition.wait(lock, [this] { return !m_queue.empty() && !m_paused; });
    
    if (m_queue.empty()) {
        return nullptr;
    }
    
    RenderTaskPtr task = m_queue.top();
    m_queue.pop();
    
    m_stats.pendingTasks--;
    
    NotifyDequeued(task);
    
    return task;
}

RenderTaskPtr RenderQueue::TryDequeue(int64_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    if (timeoutMs > 0) {
        if (!m_condition.wait_for(lock, std::chrono::milliseconds(timeoutMs), 
            [this] { return !m_queue.empty() && !m_paused; })) {
            return nullptr;
        }
    } else {
        if (m_queue.empty() || m_paused) {
            return nullptr;
        }
    }
    
    if (m_queue.empty()) {
        return nullptr;
    }
    
    RenderTaskPtr task = m_queue.top();
    m_queue.pop();
    
    m_stats.pendingTasks--;
    
    NotifyDequeued(task);
    
    return task;
}

RenderTaskPtr RenderQueue::Peek() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_queue.empty()) {
        return nullptr;
    }
    
    return m_queue.top();
}

bool RenderQueue::IsEmpty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

size_t RenderQueue::GetSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
}

size_t RenderQueue::GetMaxSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_maxSize;
}

void RenderQueue::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxSize = maxSize;
}

void RenderQueue::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    while (!m_queue.empty()) {
        m_queue.pop();
    }
    m_allTasks.clear();
    m_stats.pendingTasks = 0;
}

void RenderQueue::CancelAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    while (!m_queue.empty()) {
        auto task = m_queue.top();
        if (task) {
            task->Cancel();
            m_stats.cancelledTasks++;
        }
        m_queue.pop();
    }
    
    for (auto& task : m_allTasks) {
        if (task && task->IsPending()) {
            task->Cancel();
            m_stats.cancelledTasks++;
        }
    }
    
    m_allTasks.clear();
    m_stats.pendingTasks = 0;
}

void RenderQueue::RemoveTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_allTasks.erase(
        std::remove_if(m_allTasks.begin(), m_allTasks.end(),
            [&taskId](const RenderTaskPtr& task) {
                return task && task->GetId() == taskId;
            }),
        m_allTasks.end());
}

RenderTaskPtr RenderQueue::GetTask(const std::string& taskId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& task : m_allTasks) {
        if (task && task->GetId() == taskId) {
            return task;
        }
    }
    
    return nullptr;
}

bool RenderQueue::HasTask(const std::string& taskId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& task : m_allTasks) {
        if (task && task->GetId() == taskId) {
            return true;
        }
    }
    
    return false;
}

RenderQueueStats RenderQueue::GetStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stats;
}

void RenderQueue::ResetStats() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_stats = RenderQueueStats();
}

void RenderQueue::SetOnTaskEnqueued(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_onTaskEnqueued = handler;
}

void RenderQueue::SetOnTaskDequeued(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_onTaskDequeued = handler;
}

void RenderQueue::SetOnTaskCompleted(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_onTaskCompleted = handler;
}

void RenderQueue::SetOnTaskFailed(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_onTaskFailed = handler;
}

void RenderQueue::SetPriorityMode(bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_priorityMode = enable;
}

bool RenderQueue::IsPriorityModeEnabled() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_priorityMode;
}

void RenderQueue::Pause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_paused = true;
}

void RenderQueue::Resume() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_paused = false;
    m_condition.notify_all();
}

bool RenderQueue::IsPaused() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_paused;
}

void RenderQueue::WaitForNotEmpty() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition.wait(lock, [this] { return !m_queue.empty() && !m_paused; });
}

bool RenderQueue::WaitForNotEmpty(int64_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_condition.wait_for(lock, std::chrono::milliseconds(timeoutMs),
        [this] { return !m_queue.empty() && !m_paused; });
}

void RenderQueue::NotifyAll() {
    m_condition.notify_all();
}

void RenderQueue::UpdateStats() {
    m_stats.pendingTasks = m_queue.size();
}

void RenderQueue::NotifyEnqueued(const RenderTaskPtr& task) {
    if (m_onTaskEnqueued) {
        m_onTaskEnqueued(task);
    }
}

void RenderQueue::NotifyDequeued(const RenderTaskPtr& task) {
    if (m_onTaskDequeued) {
        m_onTaskDequeued(task);
    }
}

RenderQueuePtr RenderQueue::Create() {
    return std::make_shared<RenderQueue>();
}

RenderQueuePtr RenderQueue::Create(size_t maxQueueSize) {
    return std::make_shared<RenderQueue>(maxQueueSize);
}

}
}
