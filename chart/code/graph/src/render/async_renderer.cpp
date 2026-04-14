#include "ogc/graph/render/async_renderer.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ogc {
namespace graph {

struct AsyncRenderer::Impl {
    AsyncRenderConfig config;
    ProgressCallback progressCallback;
    CompletionCallback completionCallback;
    ErrorCallback errorCallback;
    mutable std::mutex sessionsMutex;
    std::map<std::string, RenderSessionPtr> sessions;
    std::atomic<bool> shutdown;
    std::atomic<size_t> nextRenderId;
    
    Impl()
        : shutdown(false)
        , nextRenderId(1) {
    }
    
    Impl(const AsyncRenderConfig& cfg)
        : config(cfg)
        , shutdown(false)
        , nextRenderId(1) {
    }
};

AsyncRenderer::AsyncRenderer()
    : impl_(std::make_unique<Impl>()) {
}

AsyncRenderer::AsyncRenderer(const AsyncRenderConfig& config)
    : impl_(std::make_unique<Impl>(config)) {
}

AsyncRenderer::~AsyncRenderer() {
    Shutdown();
}

void AsyncRenderer::SetConfig(const AsyncRenderConfig& config) {
    impl_->config = config;
}

AsyncRenderConfig AsyncRenderer::GetConfig() const {
    return impl_->config;
}

void AsyncRenderer::SetProgressCallback(ProgressCallback callback) {
    impl_->progressCallback = callback;
}

void AsyncRenderer::SetCompletionCallback(CompletionCallback callback) {
    impl_->completionCallback = callback;
}

void AsyncRenderer::SetErrorCallback(ErrorCallback callback) {
    impl_->errorCallback = callback;
}

std::string AsyncRenderer::GenerateRenderId() {
    std::ostringstream oss;
    oss << "render_" << std::setfill('0') << std::setw(8) << impl_->nextRenderId++;
    return oss.str();
}

std::string AsyncRenderer::StartAsync(RenderQueuePtr queue) {
    if (!queue || impl_->shutdown) {
        return "";
    }
    
    std::string renderId = GenerateRenderId();
    
    auto session = std::make_shared<RenderSession>();
    session->id = renderId;
    session->queue = queue;
    session->cancelled = false;
    session->paused = false;
    session->completed = false;
    session->stats.totalTasks = queue->GetSize();
    session->stats.pendingTasks = queue->GetSize();
    session->stats.startTime = std::chrono::steady_clock::now();
    session->stats.isRunning = true;
    session->progressCallback = impl_->progressCallback;
    session->completionCallback = impl_->completionCallback;
    session->errorCallback = impl_->errorCallback;
    
    {
        std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
        impl_->sessions[renderId] = session;
    }
    
    size_t numWorkers = std::min(impl_->config.maxConcurrentTasks, queue->GetSize());
    if (numWorkers == 0) {
        numWorkers = 1;
    }
    
    for (size_t i = 0; i < numWorkers; ++i) {
        session->workers.emplace_back(&AsyncRenderer::WorkerThread, this, session);
    }
    
    return renderId;
}

std::string AsyncRenderer::StartAsync(const std::vector<RenderTaskPtr>& tasks) {
    if (tasks.empty() || impl_->shutdown) {
        return "";
    }
    
    auto queue = RenderQueue::Create(tasks.size() * 2);
    for (auto& task : tasks) {
        if (task) {
            queue->Enqueue(task);
        }
    }
    
    return StartAsync(queue);
}

std::string AsyncRenderer::StartAsync(RenderTaskPtr task) {
    if (!task || impl_->shutdown) {
        return "";
    }
    
    std::vector<RenderTaskPtr> tasks = { task };
    return StartAsync(tasks);
}

void AsyncRenderer::WorkerThread(RenderSessionPtr session) {
    while (!session->cancelled && !impl_->shutdown) {
        if (session->paused) {
            std::unique_lock<std::mutex> lock(session->mutex);
            session->cv.wait(lock, [&session, this] {
                return !session->paused || session->cancelled || impl_->shutdown;
            });
            continue;
        }
        
        RenderTaskPtr task = session->queue->TryDequeue(100);
        if (!task) {
            if (session->queue->IsEmpty()) {
                break;
            }
            continue;
        }
        
        ProcessTask(session, task);
    }
    
    bool shouldReportCompletion = false;
    {
        std::lock_guard<std::mutex> lock(session->mutex);
        if (!session->completed) {
            session->completed = true;
            session->stats.endTime = std::chrono::steady_clock::now();
            session->stats.isRunning = false;
            shouldReportCompletion = true;
        }
    }
    
    if (shouldReportCompletion) {
        bool success = (session->stats.failedTasks == 0 && session->stats.cancelledTasks == 0);
        std::string message = success ? "All tasks completed successfully" 
                                      : "Some tasks failed or were cancelled";
        ReportCompletion(session, success, message);
    }
}

void AsyncRenderer::ProcessTask(RenderSessionPtr session, RenderTaskPtr task) {
    if (session->cancelled || impl_->shutdown) {
        task->Cancel();
        std::lock_guard<std::mutex> lock(session->mutex);
        session->stats.cancelledTasks++;
        session->stats.pendingTasks--;
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(session->mutex);
        session->stats.runningTasks++;
        session->stats.pendingTasks--;
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    bool success = false;
    try {
        success = task->Execute();
    } catch (const std::exception& e) {
        ReportError(session, std::string("Task execution exception: ") + e.what());
        success = false;
    } catch (...) {
        ReportError(session, "Unknown task execution exception");
        success = false;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    double taskTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    {
        std::lock_guard<std::mutex> lock(session->mutex);
        session->stats.runningTasks--;
        if (success) {
            session->stats.completedTasks++;
        } else if (task->GetState() == RenderTaskState::kCancelled) {
            session->stats.cancelledTasks++;
        } else {
            session->stats.failedTasks++;
        }
        session->stats.totalRenderTimeMs += taskTimeMs;
        if (session->stats.completedTasks > 0) {
            session->stats.averageTaskTimeMs = 
                session->stats.totalRenderTimeMs / session->stats.completedTasks;
        }
    }
    
    UpdateProgress(session);
}

void AsyncRenderer::UpdateProgress(RenderSessionPtr session) {
    if (!impl_->config.enableProgressReporting) {
        return;
    }
    
    double progress = 0.0;
    std::string message;
    
    {
        std::lock_guard<std::mutex> lock(session->mutex);
        if (session->stats.totalTasks > 0) {
            progress = static_cast<double>(session->stats.completedTasks + 
                                           session->stats.failedTasks + 
                                           session->stats.cancelledTasks) / 
                       session->stats.totalTasks;
            progress = std::min(progress, 1.0);
        }
        session->stats.progress = progress;
        
        std::ostringstream oss;
        oss << "Progress: " << static_cast<int>(progress * 100) << "% "
            << "(" << session->stats.completedTasks << "/" << session->stats.totalTasks << ")";
        message = oss.str();
    }
    
    if (session->progressCallback) {
        session->progressCallback(progress, message);
    } else if (impl_->progressCallback) {
        impl_->progressCallback(progress, message);
    }
}

void AsyncRenderer::ReportCompletion(RenderSessionPtr session, bool success, const std::string& message) {
    if (session->completionCallback) {
        session->completionCallback(success, message);
    } else if (impl_->completionCallback) {
        impl_->completionCallback(success, message);
    }
}

void AsyncRenderer::ReportError(RenderSessionPtr session, const std::string& error) {
    if (session->errorCallback) {
        session->errorCallback(error);
    } else if (impl_->errorCallback) {
        impl_->errorCallback(error);
    }
}

void AsyncRenderer::Cancel(const std::string& renderId) {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        it->second->cancelled = true;
        it->second->cv.notify_all();
    }
}

void AsyncRenderer::CancelAll() {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    for (auto& pair : impl_->sessions) {
        pair.second->cancelled = true;
        pair.second->cv.notify_all();
    }
}

bool AsyncRenderer::IsRunning(const std::string& renderId) const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        return it->second->stats.isRunning;
    }
    return false;
}

bool AsyncRenderer::IsCompleted(const std::string& renderId) const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        return it->second->completed;
    }
    return false;
}

bool AsyncRenderer::IsCancelled(const std::string& renderId) const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        return it->second->cancelled;
    }
    return false;
}

double AsyncRenderer::GetProgress(const std::string& renderId) const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        return it->second->stats.progress;
    }
    return 0.0;
}

AsyncRenderStats AsyncRenderer::GetStats(const std::string& renderId) const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        return it->second->stats;
    }
    return AsyncRenderStats();
}

void AsyncRenderer::WaitForCompletion(const std::string& renderId) {
    std::vector<std::thread> workers;
    {
        std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
        auto it = impl_->sessions.find(renderId);
        if (it != impl_->sessions.end()) {
            workers = std::move(it->second->workers);
        }
    }
    
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

bool AsyncRenderer::WaitForCompletion(const std::string& renderId, int64_t timeoutMs) {
    auto startTime = std::chrono::steady_clock::now();
    
    while (true) {
        if (!IsRunning(renderId)) {
            return true;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        
        if (elapsed.count() >= timeoutMs) {
            return false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void AsyncRenderer::Pause(const std::string& renderId) {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        it->second->paused = true;
    }
}

void AsyncRenderer::Resume(const std::string& renderId) {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        it->second->paused = false;
        it->second->cv.notify_all();
    }
}

bool AsyncRenderer::IsPaused(const std::string& renderId) const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    auto it = impl_->sessions.find(renderId);
    if (it != impl_->sessions.end()) {
        return it->second->paused;
    }
    return false;
}

size_t AsyncRenderer::GetActiveRenderCount() const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    size_t count = 0;
    for (const auto& pair : impl_->sessions) {
        if (pair.second->stats.isRunning) {
            count++;
        }
    }
    return count;
}

std::vector<std::string> AsyncRenderer::GetActiveRenderIds() const {
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    std::vector<std::string> ids;
    for (const auto& pair : impl_->sessions) {
        if (pair.second->stats.isRunning) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

void AsyncRenderer::SetMaxConcurrentTasks(size_t maxTasks) {
    impl_->config.maxConcurrentTasks = maxTasks;
}

size_t AsyncRenderer::GetMaxConcurrentTasks() const {
    return impl_->config.maxConcurrentTasks;
}

void AsyncRenderer::Shutdown() {
    if (impl_->shutdown) {
        return;
    }
    
    impl_->shutdown = true;
    
    std::vector<RenderSessionPtr> sessions;
    {
        std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
        for (auto& pair : impl_->sessions) {
            pair.second->cancelled = true;
            pair.second->paused = false;
            pair.second->cv.notify_all();
            sessions.push_back(pair.second);
        }
    }
    
    for (auto& session : sessions) {
        for (auto& worker : session->workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    std::lock_guard<std::mutex> lock(impl_->sessionsMutex);
    impl_->sessions.clear();
}

bool AsyncRenderer::IsShutdown() const {
    return impl_->shutdown;
}

AsyncRendererPtr AsyncRenderer::Create() {
    return std::make_shared<AsyncRenderer>();
}

AsyncRendererPtr AsyncRenderer::Create(const AsyncRenderConfig& config) {
    return std::make_shared<AsyncRenderer>(config);
}

struct AsyncRenderBuilder::Impl {
    AsyncRenderConfig config;
    ProgressCallback progressCallback;
    CompletionCallback completionCallback;
    ErrorCallback errorCallback;
    std::vector<RenderTaskPtr> tasks;
    RenderQueuePtr queue;
    
    Impl()
        : config(AsyncRenderConfig::Default()) {
    }
};

AsyncRenderBuilder::AsyncRenderBuilder()
    : impl_(std::make_unique<Impl>()) {
}

AsyncRenderBuilder::~AsyncRenderBuilder() = default;

AsyncRenderBuilder& AsyncRenderBuilder::SetMaxConcurrentTasks(size_t maxTasks) {
    impl_->config.maxConcurrentTasks = maxTasks;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::SetTaskTimeout(int64_t timeoutMs) {
    impl_->config.taskTimeoutMs = timeoutMs;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::EnableProgressReporting(bool enable) {
    impl_->config.enableProgressReporting = enable;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::EnableCancellation(bool enable) {
    impl_->config.enableCancellation = enable;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::SetProgressCallback(ProgressCallback callback) {
    impl_->progressCallback = callback;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::SetCompletionCallback(CompletionCallback callback) {
    impl_->completionCallback = callback;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::SetErrorCallback(ErrorCallback callback) {
    impl_->errorCallback = callback;
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::AddTask(RenderTaskPtr task) {
    if (task) {
        impl_->tasks.push_back(task);
    }
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::AddTasks(const std::vector<RenderTaskPtr>& tasks) {
    for (const auto& task : tasks) {
        if (task) {
            impl_->tasks.push_back(task);
        }
    }
    return *this;
}

AsyncRenderBuilder& AsyncRenderBuilder::SetQueue(RenderQueuePtr queue) {
    impl_->queue = queue;
    return *this;
}

AsyncRendererPtr AsyncRenderBuilder::Build() {
    auto renderer = AsyncRenderer::Create(impl_->config);
    
    if (impl_->progressCallback) {
        renderer->SetProgressCallback(impl_->progressCallback);
    }
    if (impl_->completionCallback) {
        renderer->SetCompletionCallback(impl_->completionCallback);
    }
    if (impl_->errorCallback) {
        renderer->SetErrorCallback(impl_->errorCallback);
    }
    
    return renderer;
}

std::string AsyncRenderBuilder::BuildAndStart() {
    auto renderer = Build();
    
    if (impl_->queue) {
        return renderer->StartAsync(impl_->queue);
    } else if (!impl_->tasks.empty()) {
        return renderer->StartAsync(impl_->tasks);
    }
    
    return "";
}

} 
} 
