#ifndef OGC_DRAW_ASYNC_RENDER_TASK_H
#define OGC_DRAW_ASYNC_RENDER_TASK_H

#include <ogc/draw/draw_device.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/tile_device.h>
#include <ogc/draw/tile_based_engine.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/geometry.h>
#include <ogc/draw/draw_result.h>

#include <functional>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace ogc {
namespace draw {

enum class RenderState {
    kPending = 0,
    kRunning = 1,
    kCompleted = 2,
    kFailed = 3,
    kCancelled = 4
};

inline const char* RenderStateToString(RenderState state) {
    switch (state) {
        case RenderState::kPending:   return "Pending";
        case RenderState::kRunning:   return "Running";
        case RenderState::kCompleted: return "Completed";
        case RenderState::kFailed:    return "Failed";
        case RenderState::kCancelled: return "Cancelled";
        default:                      return "Unknown";
    }
}

struct RenderResult {
    RenderState state = RenderState::kPending;
    DrawResult drawResult = DrawResult::kSuccess;
    std::string errorMessage;
    double elapsedMs = 0.0;
    size_t geometryCount = 0;
};

class OGC_DRAW_API AsyncRenderTask {
public:
    using ProgressCallback = std::function<void(float progress, const std::string& stage)>;
    using CompletionCallback = std::function<void(const RenderResult& result)>;

    virtual ~AsyncRenderTask() = default;

    virtual RenderState GetState() const = 0;
    virtual float GetProgress() const = 0;
    virtual std::string GetCurrentStage() const = 0;

    virtual void SetProgressCallback(ProgressCallback callback) = 0;
    virtual void SetCompletionCallback(CompletionCallback callback) = 0;

    virtual void Cancel() = 0;
    virtual bool Wait(uint32_t timeoutMs = 0) = 0;

    virtual RenderResult GetResult() const = 0;
};

class OGC_DRAW_API TileAsyncRenderTask : public AsyncRenderTask {
public:
    TileAsyncRenderTask(TileDevice* device, TileBasedEngine* engine,
                       const std::vector<Geometry*>& geometries, const DrawStyle& style);
    ~TileAsyncRenderTask() override;

    RenderState GetState() const override;
    float GetProgress() const override;
    std::string GetCurrentStage() const override;

    void SetProgressCallback(ProgressCallback callback) override;
    void SetCompletionCallback(CompletionCallback callback) override;

    void Cancel() override;
    bool Wait(uint32_t timeoutMs = 0) override;

    RenderResult GetResult() const override;

    void Execute();

private:
    void UpdateProgress(float progress, const std::string& stage);
    void NotifyCompletion();

    TileDevice* m_device;
    TileBasedEngine* m_engine;
    std::vector<Geometry*> m_geometries;
    DrawStyle m_style;

    std::atomic<RenderState> m_state;
    std::atomic<float> m_progress;
    std::atomic<bool> m_cancelled;
    std::string m_currentStage;
    RenderResult m_result;

    ProgressCallback m_progressCallback;
    CompletionCallback m_completionCallback;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};

} 
} 

#endif 
