#include "ogc/graph/render/basic_render_task.h"
#include <ogc/draw/draw_context.h>

namespace ogc {
namespace graph {

BasicRenderTask::BasicRenderTask()
    : RenderTask()
    , m_context(nullptr) {
}

BasicRenderTask::BasicRenderTask(const std::string& id, const DrawParams& params)
    : RenderTask(id, params)
    , m_context(nullptr) {
}

void BasicRenderTask::SetRenderFunction(RenderFunction func) {
    m_renderFunction = func;
}

RenderFunction BasicRenderTask::GetRenderFunction() const {
    return m_renderFunction;
}

void BasicRenderTask::SetDrawContext(ogc::draw::DrawContext* context) {
    m_context = context;
}

ogc::draw::DrawContext* BasicRenderTask::GetDrawContext() const {
    return m_context;
}

bool BasicRenderTask::Execute() {
    if (IsCancelled()) {
        return false;
    }
    
    if (IsTimedOut()) {
        SetState(RenderTaskState::kFailed);
        SetError("Task timed out");
        return false;
    }
    
    SetState(RenderTaskState::kRunning);
    SetProgress(0.0);
    
    bool success = false;
    
    try {
        if (m_renderFunction && m_context) {
            success = m_renderFunction(shared_from_this(), *m_context);
        }
        
        if (success) {
            SetProgress(1.0);
            SetState(RenderTaskState::kCompleted);
        } else {
            SetState(RenderTaskState::kFailed);
            if (GetError().empty()) {
                SetError("Render function returned false");
            }
        }
    } catch (const std::exception& e) {
        SetState(RenderTaskState::kFailed);
        SetError(e.what());
        success = false;
    } catch (...) {
        SetState(RenderTaskState::kFailed);
        SetError("Unknown error during rendering");
        success = false;
    }
    
    NotifyCompletion();
    return success;
}

std::shared_ptr<BasicRenderTask> BasicRenderTask::Create() {
    return std::shared_ptr<BasicRenderTask>(new BasicRenderTask());
}

std::shared_ptr<BasicRenderTask> BasicRenderTask::Create(const std::string& id, const DrawParams& params) {
    return std::shared_ptr<BasicRenderTask>(new BasicRenderTask(id, params));
}

}
}
