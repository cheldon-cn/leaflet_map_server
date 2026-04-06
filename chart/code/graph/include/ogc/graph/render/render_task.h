#ifndef OGC_GRAPH_RENDER_TASK_H
#define OGC_GRAPH_RENDER_TASK_H

#include "ogc/graph/export.h"
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_result.h>
#include "ogc/envelope.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <functional>
#include <chrono>
#include <atomic>

namespace ogc {
namespace graph {

class RenderTask;
typedef std::shared_ptr<RenderTask> RenderTaskPtr;

enum class RenderTaskState {
    kPending = 0,
    kQueued = 1,
    kRunning = 2,
    kCompleted = 3,
    kFailed = 4,
    kCancelled = 5
};

enum class RenderTaskPriority {
    kLow = 0,
    kNormal = 1,
    kHigh = 2,
    kUrgent = 3
};

struct RenderTaskStats {
    int64_t queueTime;
    int64_t startTime;
    int64_t endTime;
    int64_t renderTime;
    size_t featureCount;
    size_t geometryCount;
    size_t memoryUsage;
    
    RenderTaskStats()
        : queueTime(0)
        , startTime(0)
        , endTime(0)
        , renderTime(0)
        , featureCount(0)
        , geometryCount(0)
        , memoryUsage(0) {}
};

typedef std::function<void(const RenderTaskPtr&)> RenderTaskCallback;

class OGC_GRAPH_API RenderTask : public std::enable_shared_from_this<RenderTask> {
public:
    RenderTask();
    RenderTask(const std::string& id, const DrawParams& params);
    virtual ~RenderTask() = default;
    
    void SetId(const std::string& id);
    std::string GetId() const;
    
    void SetParams(const DrawParams& params);
    DrawParams GetParams() const;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    
    void SetPriority(RenderTaskPriority priority);
    RenderTaskPriority GetPriority() const;
    
    void SetState(RenderTaskState state);
    RenderTaskState GetState() const;
    
    void SetProgress(double progress);
    double GetProgress() const;
    
    void SetResult(const ogc::draw::DrawResult& result);
    ogc::draw::DrawResult GetResult() const;
    
    void SetError(const std::string& error);
    std::string GetError() const;
    
    void SetStats(const RenderTaskStats& stats);
    RenderTaskStats GetStats() const;
    
    void SetUserData(void* data);
    void* GetUserData() const;
    
    void SetTag(const std::string& tag);
    std::string GetTag() const;
    
    void SetCallback(RenderTaskCallback callback);
    RenderTaskCallback GetCallback() const;
    
    void SetTimeout(int64_t milliseconds);
    int64_t GetTimeout() const;
    
    bool IsPending() const;
    bool IsRunning() const;
    bool IsCompleted() const;
    bool IsFailed() const;
    bool IsCancelled() const;
    bool IsTimedOut() const;
    
    void Cancel();
    
    virtual bool Execute() = 0;
    
    void NotifyCompletion();
    
    int GetPriorityValue() const;
    
    bool operator<(const RenderTask& other) const;
    
    static RenderTaskPtr Create();
    static RenderTaskPtr Create(const std::string& id, const DrawParams& params);

protected:
    void UpdateState(RenderTaskState state);
    
    std::string m_id;
    DrawParams m_params;
    Envelope m_extent;
    RenderTaskPriority m_priority;
    std::atomic<RenderTaskState> m_state;
    std::atomic<double> m_progress;
    ogc::draw::DrawResult m_result;
    std::string m_error;
    RenderTaskStats m_stats;
    void* m_userData;
    std::string m_tag;
    RenderTaskCallback m_callback;
    int64_t m_timeout;
    std::chrono::steady_clock::time_point m_createdTime;
};

}
}

#endif
