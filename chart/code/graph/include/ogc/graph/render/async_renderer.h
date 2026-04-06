#ifndef OGC_GRAPH_ASYNC_RENDERER_H
#define OGC_GRAPH_ASYNC_RENDERER_H

#include "ogc/graph/export.h"
#include "ogc/graph/render/render_task.h"
#include "ogc/graph/render/render_queue.h"
#include <ogc/draw/draw_context.h>
#include <ogc/draw/symbolizer.h>
#include <ogc/geometry.h>
#include <ogc/envelope.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <chrono>
#include <map>

namespace ogc {
namespace graph {

using ProgressCallback = std::function<void(double progress, const std::string& message)>;
using CompletionCallback = std::function<void(bool success, const std::string& message)>;
using ErrorCallback = std::function<void(const std::string& error)>;

struct AsyncRenderConfig {
    size_t maxConcurrentTasks = 4;
    size_t taskTimeoutMs = 30000;
    bool enableProgressReporting = true;
    double progressReportInterval = 0.1;
    bool enableCancellation = true;
    bool preserveTaskOrder = false;
    
    static AsyncRenderConfig Default() {
        return AsyncRenderConfig();
    }
    
    static AsyncRenderConfig HighPerformance() {
        AsyncRenderConfig config;
        config.maxConcurrentTasks = 8;
        config.taskTimeoutMs = 60000;
        return config;
    }
    
    static AsyncRenderConfig SafeMode() {
        AsyncRenderConfig config;
        config.maxConcurrentTasks = 2;
        config.taskTimeoutMs = 10000;
        config.enableCancellation = true;
        return config;
    }
};

struct AsyncRenderStats {
    size_t totalTasks = 0;
    size_t completedTasks = 0;
    size_t failedTasks = 0;
    size_t cancelledTasks = 0;
    size_t pendingTasks = 0;
    size_t runningTasks = 0;
    double totalRenderTimeMs = 0.0;
    double averageTaskTimeMs = 0.0;
    double progress = 0.0;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    bool isRunning = false;
};

class AsyncRenderer;
using AsyncRendererPtr = std::shared_ptr<AsyncRenderer>;

class OGC_GRAPH_API AsyncRenderer {
public:
    
    AsyncRenderer();
    explicit AsyncRenderer(const AsyncRenderConfig& config);
    ~AsyncRenderer();
    
    void SetConfig(const AsyncRenderConfig& config);
    AsyncRenderConfig GetConfig() const;
    
    void SetProgressCallback(ProgressCallback callback);
    void SetCompletionCallback(CompletionCallback callback);
    void SetErrorCallback(ErrorCallback callback);
    
    std::string StartAsync(RenderQueuePtr queue);
    std::string StartAsync(const std::vector<RenderTaskPtr>& tasks);
    std::string StartAsync(RenderTaskPtr task);
    
    void Cancel(const std::string& renderId);
    void CancelAll();
    
    bool IsRunning(const std::string& renderId) const;
    bool IsCompleted(const std::string& renderId) const;
    bool IsCancelled(const std::string& renderId) const;
    
    double GetProgress(const std::string& renderId) const;
    AsyncRenderStats GetStats(const std::string& renderId) const;
    
    void WaitForCompletion(const std::string& renderId);
    bool WaitForCompletion(const std::string& renderId, int64_t timeoutMs);
    
    void Pause(const std::string& renderId);
    void Resume(const std::string& renderId);
    bool IsPaused(const std::string& renderId) const;
    
    size_t GetActiveRenderCount() const;
    std::vector<std::string> GetActiveRenderIds() const;
    
    void SetMaxConcurrentTasks(size_t maxTasks);
    size_t GetMaxConcurrentTasks() const;
    
    void Shutdown();
    bool IsShutdown() const;
    
    static AsyncRendererPtr Create();
    static AsyncRendererPtr Create(const AsyncRenderConfig& config);

private:
    struct RenderSession {
        std::string id;
        RenderQueuePtr queue;
        AsyncRenderStats stats;
        std::atomic<bool> cancelled;
        std::atomic<bool> paused;
        std::atomic<bool> completed;
        std::vector<std::thread> workers;
        std::mutex mutex;
        std::condition_variable cv;
        ProgressCallback progressCallback;
        CompletionCallback completionCallback;
        ErrorCallback errorCallback;
    };
    
    using RenderSessionPtr = std::shared_ptr<RenderSession>;
    
    void WorkerThread(RenderSessionPtr session);
    void UpdateProgress(RenderSessionPtr session);
    void ReportCompletion(RenderSessionPtr session, bool success, const std::string& message);
    void ReportError(RenderSessionPtr session, const std::string& error);
    void ProcessTask(RenderSessionPtr session, RenderTaskPtr task);
    
    std::string GenerateRenderId();
    
    AsyncRenderConfig m_config;
    ProgressCallback m_progressCallback;
    CompletionCallback m_completionCallback;
    ErrorCallback m_errorCallback;
    
    mutable std::mutex m_sessionsMutex;
    std::map<std::string, RenderSessionPtr> m_sessions;
    std::atomic<bool> m_shutdown;
    std::atomic<size_t> m_nextRenderId;
};

class OGC_GRAPH_API AsyncRenderBuilder {
public:
    AsyncRenderBuilder();
    ~AsyncRenderBuilder() = default;
    
    AsyncRenderBuilder& SetMaxConcurrentTasks(size_t maxTasks);
    AsyncRenderBuilder& SetTaskTimeout(int64_t timeoutMs);
    AsyncRenderBuilder& EnableProgressReporting(bool enable);
    AsyncRenderBuilder& EnableCancellation(bool enable);
    AsyncRenderBuilder& SetProgressCallback(ProgressCallback callback);
    AsyncRenderBuilder& SetCompletionCallback(CompletionCallback callback);
    AsyncRenderBuilder& SetErrorCallback(ErrorCallback callback);
    AsyncRenderBuilder& AddTask(RenderTaskPtr task);
    AsyncRenderBuilder& AddTasks(const std::vector<RenderTaskPtr>& tasks);
    AsyncRenderBuilder& SetQueue(RenderQueuePtr queue);
    
    AsyncRendererPtr Build();
    std::string BuildAndStart();

private:
    AsyncRenderConfig m_config;
    ProgressCallback m_progressCallback;
    CompletionCallback m_completionCallback;
    ErrorCallback m_errorCallback;
    std::vector<RenderTaskPtr> m_tasks;
    RenderQueuePtr m_queue;
};

} 
} 

#endif 
