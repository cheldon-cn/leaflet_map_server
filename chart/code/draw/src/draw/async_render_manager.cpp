#include "ogc/draw/async_render_manager.h"
#include "ogc/draw/async_render_task.h"
#include <chrono>
#include <algorithm>

namespace ogc {
namespace draw {

AsyncRenderManager& AsyncRenderManager::Instance()
{
    static AsyncRenderManager instance;
    return instance;
}

AsyncRenderManager::AsyncRenderManager()
    : m_maxConcurrentTasks(4)
    , m_paused(false)
    , m_shutdown(false)
    , m_nextSubmitTime(0)
{
    for (int i = 0; i < m_maxConcurrentTasks; ++i) {
        m_workers.emplace_back(&AsyncRenderManager::WorkerThread, this);
    }
}

AsyncRenderManager::~AsyncRenderManager()
{
    Shutdown();
}

std::shared_ptr<AsyncRenderTask> AsyncRenderManager::SubmitRenderTask(
    DrawDevice* device,
    DrawEngine* engine,
    const std::vector<Geometry*>& geometries,
    const DrawStyle& style,
    TaskPriority priority)
{
    std::shared_ptr<AsyncRenderTask> task;
    
    TileDevice* tileDevice = dynamic_cast<TileDevice*>(device);
    TileBasedEngine* tileEngine = dynamic_cast<TileBasedEngine*>(engine);
    
    if (tileDevice && tileEngine) {
        task = std::make_shared<TileAsyncRenderTask>(tileDevice, tileEngine, geometries, style);
    } else {
        return nullptr;
    }
    
    TaskQueueEntry entry;
    entry.task = task;
    entry.priority = priority;
    entry.submitTime = m_nextSubmitTime++;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pendingQueue.push(entry);
    }
    
    m_cv.notify_one();
    
    return task;
}

void AsyncRenderManager::SetMaxConcurrentTasks(int maxTasks)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    int oldMax = m_maxConcurrentTasks;
    m_maxConcurrentTasks = std::max(1, maxTasks);
    
    if (m_maxConcurrentTasks > oldMax && !m_shutdown) {
        for (int i = oldMax; i < m_maxConcurrentTasks; ++i) {
            m_workers.emplace_back(&AsyncRenderManager::WorkerThread, this);
        }
    }
}

int AsyncRenderManager::GetActiveTaskCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_activeTasks.size());
}

int AsyncRenderManager::GetPendingTaskCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_pendingQueue.size());
}

void AsyncRenderManager::CancelAllTasks()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    while (!m_pendingQueue.empty()) {
        m_pendingQueue.pop();
    }
    
    for (auto& task : m_activeTasks) {
        if (task) {
            task->Cancel();
        }
    }
}

void AsyncRenderManager::WaitForAll(uint32_t timeoutMs)
{
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (m_pendingQueue.empty() && m_activeTasks.empty()) {
            return;
        }
        
        lock.unlock();
        
        if (timeoutMs > 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count();
            if (elapsed >= timeoutMs) {
                return;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void AsyncRenderManager::Pause()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_paused = true;
}

void AsyncRenderManager::Resume()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_paused = false;
    }
    m_cv.notify_all();
}

bool AsyncRenderManager::IsPaused() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_paused;
}

void AsyncRenderManager::Shutdown()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_shutdown) {
            return;
        }
        m_shutdown = true;
    }
    
    m_cv.notify_all();
    
    for (auto& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    m_workers.clear();
}

void AsyncRenderManager::WorkerThread()
{
    while (true) {
        std::shared_ptr<AsyncRenderTask> task;
        
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            
            m_cv.wait(lock, [this] {
                return (m_shutdown || (!m_pendingQueue.empty() && !m_paused));
            });
            
            if (m_shutdown) {
                return;
            }
            
            if (m_paused || m_pendingQueue.empty()) {
                continue;
            }
            
            if (static_cast<int>(m_activeTasks.size()) >= m_maxConcurrentTasks) {
                m_cv.wait(lock, [this] {
                    return m_shutdown || 
                           static_cast<int>(m_activeTasks.size()) < m_maxConcurrentTasks;
                });
                
                if (m_shutdown) {
                    return;
                }
            }
            
            if (!m_pendingQueue.empty()) {
                task = m_pendingQueue.top().task;
                m_pendingQueue.pop();
                
                if (task) {
                    m_activeTasks.insert(task);
                }
            }
        }
        
        if (task) {
            ProcessTask(task);
            
            std::lock_guard<std::mutex> lock(m_mutex);
            m_activeTasks.erase(task);
            m_cv.notify_all();
        }
    }
}

void AsyncRenderManager::ProcessTask(std::shared_ptr<AsyncRenderTask> task)
{
    if (task) {
        TileAsyncRenderTask* tileTask = dynamic_cast<TileAsyncRenderTask*>(task.get());
        if (tileTask) {
            tileTask->Execute();
        }
    }
}

} 
} 
