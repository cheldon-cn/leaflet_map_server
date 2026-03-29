#include "ogc/draw/async_render_task.h"
#include <chrono>
#include <algorithm>

namespace ogc {
namespace draw {

TileAsyncRenderTask::TileAsyncRenderTask(TileDevice* device, TileBasedEngine* engine,
                                         const std::vector<Geometry*>& geometries, const DrawStyle& style)
    : m_device(device)
    , m_engine(engine)
    , m_geometries(geometries)
    , m_style(style)
    , m_state(RenderState::kPending)
    , m_progress(0.0f)
    , m_cancelled(false)
{
}

TileAsyncRenderTask::~TileAsyncRenderTask()
{
    m_cancelled.store(true);
}

RenderState TileAsyncRenderTask::GetState() const
{
    return m_state.load();
}

float TileAsyncRenderTask::GetProgress() const
{
    return m_progress.load();
}

std::string TileAsyncRenderTask::GetCurrentStage() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentStage;
}

void TileAsyncRenderTask::SetProgressCallback(ProgressCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_progressCallback = callback;
}

void TileAsyncRenderTask::SetCompletionCallback(CompletionCallback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_completionCallback = callback;
}

void TileAsyncRenderTask::Cancel()
{
    m_cancelled.store(true);
}

bool TileAsyncRenderTask::Wait(uint32_t timeoutMs)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (timeoutMs == 0) {
        m_cv.wait(lock, [this] {
            RenderState state = m_state.load();
            return state == RenderState::kCompleted ||
                   state == RenderState::kFailed ||
                   state == RenderState::kCancelled;
        });
        return true;
    } else {
        return m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this] {
            RenderState state = m_state.load();
            return state == RenderState::kCompleted ||
                   state == RenderState::kFailed ||
                   state == RenderState::kCancelled;
        });
    }
}

RenderResult TileAsyncRenderTask::GetResult() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_result;
}

void TileAsyncRenderTask::Execute()
{
    RenderState expected = RenderState::kPending;
    if (!m_state.compare_exchange_strong(expected, RenderState::kRunning)) {
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    UpdateProgress(0.0f, "Initializing");

    if (!m_device || !m_engine) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_result.state = RenderState::kFailed;
        m_result.drawResult = DrawResult::kInvalidParameter;
        m_result.errorMessage = "Invalid device or engine";
        m_state.store(RenderState::kFailed);
        m_cv.notify_all();
        NotifyCompletion();
        return;
    }

    if (m_cancelled.load()) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_result.state = RenderState::kCancelled;
        m_result.drawResult = DrawResult::kCancelled;
        m_result.errorMessage = "Task was cancelled";
        m_state.store(RenderState::kCancelled);
        m_cv.notify_all();
        NotifyCompletion();
        return;
    }

    m_engine->SetProgressCallback([this](float progress) {
        if (!m_cancelled.load()) {
            UpdateProgress(progress, "Rendering");
        }
    });

    DrawResult beginResult = m_engine->Begin();
    if (beginResult != DrawResult::kSuccess) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_result.state = RenderState::kFailed;
        m_result.drawResult = beginResult;
        m_result.errorMessage = "Failed to begin rendering";
        m_state.store(RenderState::kFailed);
        m_cv.notify_all();
        NotifyCompletion();
        return;
    }

    UpdateProgress(0.1f, "Rendering");

    DrawResult drawResult = DrawResult::kSuccess;
    size_t geometryCount = 0;

    for (size_t i = 0; i < m_geometries.size() && !m_cancelled.load(); ++i) {
        if (m_geometries[i]) {
            drawResult = m_engine->DrawGeometry(m_geometries[i], m_style);
            if (drawResult != DrawResult::kSuccess) {
                break;
            }
            geometryCount++;
            float progress = 0.1f + 0.8f * static_cast<float>(i + 1) / m_geometries.size();
            UpdateProgress(progress, "Rendering geometries");
        }
    }

    m_engine->End();

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_cancelled.load()) {
            m_result.state = RenderState::kCancelled;
            m_result.drawResult = DrawResult::kCancelled;
            m_result.errorMessage = "Task was cancelled";
            m_state.store(RenderState::kCancelled);
        } else if (drawResult != DrawResult::kSuccess) {
            m_result.state = RenderState::kFailed;
            m_result.drawResult = drawResult;
            m_result.errorMessage = "Render operation failed";
            m_state.store(RenderState::kFailed);
        } else {
            m_result.state = RenderState::kCompleted;
            m_result.drawResult = DrawResult::kSuccess;
            m_result.elapsedMs = elapsedMs;
            m_result.geometryCount = geometryCount;
            m_state.store(RenderState::kCompleted);
        }

        m_result.elapsedMs = elapsedMs;
    }

    UpdateProgress(1.0f, "Completed");
    m_cv.notify_all();
    NotifyCompletion();
}

void TileAsyncRenderTask::UpdateProgress(float progress, const std::string& stage)
{
    m_progress.store(progress);
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentStage = stage;
    }

    ProgressCallback callback;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        callback = m_progressCallback;
    }

    if (callback) {
        callback(progress, stage);
    }
}

void TileAsyncRenderTask::NotifyCompletion()
{
    CompletionCallback callback;
    RenderResult result;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        callback = m_completionCallback;
        result = m_result;
    }

    if (callback) {
        callback(result);
    }
}

} 
} 
