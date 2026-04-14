#include "ogc/graph/render/render_queue.h"
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace ogc {
namespace graph {

struct RenderQueue::Impl {
    struct TaskComparator {
        bool operator()(const RenderTaskPtr& a, const RenderTaskPtr& b) const {
            return a->GetPriorityValue() < b->GetPriorityValue();
        }
    };
    
    mutable std::mutex mutex;
    std::condition_variable condition;
    std::priority_queue<RenderTaskPtr, std::vector<RenderTaskPtr>, TaskComparator> queue;
    std::vector<RenderTaskPtr> allTasks;
    size_t maxSize;
    bool priorityMode;
    bool paused;
    RenderQueueStats stats;
    
    TaskEventHandler onTaskEnqueued;
    TaskEventHandler onTaskDequeued;
    TaskEventHandler onTaskCompleted;
    TaskEventHandler onTaskFailed;
    
    Impl() : maxSize(1000), priorityMode(true), paused(false) {}
    explicit Impl(size_t maxSz) : maxSize(maxSz), priorityMode(true), paused(false) {}
};

RenderQueue::RenderQueue()
    : impl_(std::make_unique<Impl>())
{
}

RenderQueue::RenderQueue(size_t maxQueueSize)
    : impl_(std::make_unique<Impl>(maxQueueSize))
{
}

RenderQueue::~RenderQueue() {
    CancelAll();
}

bool RenderQueue::Enqueue(RenderTaskPtr task) {
    if (!task) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (impl_->queue.size() >= impl_->maxSize) {
        return false;
    }
    
    if (task->IsCancelled()) {
        return false;
    }
    
    task->SetState(RenderTaskState::kQueued);
    impl_->queue.push(task);
    impl_->allTasks.push_back(task);
    impl_->stats.totalTasks++;
    impl_->stats.pendingTasks++;
    
    NotifyEnqueued(task);
    impl_->condition.notify_one();
    
    return true;
}

RenderTaskPtr RenderQueue::Dequeue() {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    
    impl_->condition.wait(lock, [this] { return !impl_->queue.empty() && !impl_->paused; });
    
    if (impl_->queue.empty()) {
        return nullptr;
    }
    
    RenderTaskPtr task = impl_->queue.top();
    impl_->queue.pop();
    
    impl_->stats.pendingTasks--;
    
    NotifyDequeued(task);
    
    return task;
}

RenderTaskPtr RenderQueue::TryDequeue(int64_t timeoutMs) {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    
    if (timeoutMs > 0) {
        if (!impl_->condition.wait_for(lock, std::chrono::milliseconds(timeoutMs), 
            [this] { return !impl_->queue.empty() && !impl_->paused; })) {
            return nullptr;
        }
    } else {
        if (impl_->queue.empty() || impl_->paused) {
            return nullptr;
        }
    }
    
    if (impl_->queue.empty()) {
        return nullptr;
    }
    
    RenderTaskPtr task = impl_->queue.top();
    impl_->queue.pop();
    
    impl_->stats.pendingTasks--;
    
    NotifyDequeued(task);
    
    return task;
}

RenderTaskPtr RenderQueue::Peek() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    if (impl_->queue.empty()) {
        return nullptr;
    }
    
    return impl_->queue.top();
}

bool RenderQueue::IsEmpty() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->queue.empty();
}

size_t RenderQueue::GetSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->queue.size();
}

size_t RenderQueue::GetMaxSize() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->maxSize;
}

void RenderQueue::SetMaxSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->maxSize = maxSize;
}

void RenderQueue::Clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    while (!impl_->queue.empty()) {
        impl_->queue.pop();
    }
    impl_->allTasks.clear();
    impl_->stats.pendingTasks = 0;
}

void RenderQueue::CancelAll() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    while (!impl_->queue.empty()) {
        auto task = impl_->queue.top();
        if (task) {
            task->Cancel();
            impl_->stats.cancelledTasks++;
        }
        impl_->queue.pop();
    }
    
    for (auto& task : impl_->allTasks) {
        if (task && task->IsPending()) {
            task->Cancel();
            impl_->stats.cancelledTasks++;
        }
    }
    
    impl_->allTasks.clear();
    impl_->stats.pendingTasks = 0;
}

void RenderQueue::RemoveTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    impl_->allTasks.erase(
        std::remove_if(impl_->allTasks.begin(), impl_->allTasks.end(),
            [&taskId](const RenderTaskPtr& task) {
                return task && task->GetId() == taskId;
            }),
        impl_->allTasks.end());
}

RenderTaskPtr RenderQueue::GetTask(const std::string& taskId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& task : impl_->allTasks) {
        if (task && task->GetId() == taskId) {
            return task;
        }
    }
    
    return nullptr;
}

bool RenderQueue::HasTask(const std::string& taskId) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    for (const auto& task : impl_->allTasks) {
        if (task && task->GetId() == taskId) {
            return true;
        }
    }
    
    return false;
}

RenderQueueStats RenderQueue::GetStats() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->stats;
}

void RenderQueue::ResetStats() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->stats = RenderQueueStats();
}

void RenderQueue::SetOnTaskEnqueued(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->onTaskEnqueued = handler;
}

void RenderQueue::SetOnTaskDequeued(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->onTaskDequeued = handler;
}

void RenderQueue::SetOnTaskCompleted(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->onTaskCompleted = handler;
}

void RenderQueue::SetOnTaskFailed(TaskEventHandler handler) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->onTaskFailed = handler;
}

void RenderQueue::SetPriorityMode(bool enable) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->priorityMode = enable;
}

bool RenderQueue::IsPriorityModeEnabled() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->priorityMode;
}

void RenderQueue::Pause() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->paused = true;
}

void RenderQueue::Resume() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->paused = false;
    impl_->condition.notify_all();
}

bool RenderQueue::IsPaused() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->paused;
}

void RenderQueue::WaitForNotEmpty() {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    impl_->condition.wait(lock, [this] { return !impl_->queue.empty() && !impl_->paused; });
}

bool RenderQueue::WaitForNotEmpty(int64_t timeoutMs) {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    return impl_->condition.wait_for(lock, std::chrono::milliseconds(timeoutMs),
        [this] { return !impl_->queue.empty() && !impl_->paused; });
}

void RenderQueue::NotifyAll() {
    impl_->condition.notify_all();
}

void RenderQueue::UpdateStats() {
    impl_->stats.pendingTasks = impl_->queue.size();
}

void RenderQueue::NotifyEnqueued(const RenderTaskPtr& task) {
    if (impl_->onTaskEnqueued) {
        impl_->onTaskEnqueued(task);
    }
}

void RenderQueue::NotifyDequeued(const RenderTaskPtr& task) {
    if (impl_->onTaskDequeued) {
        impl_->onTaskDequeued(task);
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
