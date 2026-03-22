#ifndef OGC_DRAW_BASIC_RENDER_TASK_H
#define OGC_DRAW_BASIC_RENDER_TASK_H

#include "ogc/draw/render_task.h"
#include "ogc/draw/draw_context.h"
#include <functional>

namespace ogc {
namespace draw {

typedef std::function<bool(RenderTaskPtr, DrawContext&)> RenderFunction;

class BasicRenderTask : public RenderTask {
public:
    BasicRenderTask();
    BasicRenderTask(const std::string& id, const DrawParams& params);
    ~BasicRenderTask() override = default;
    
    void SetRenderFunction(RenderFunction func);
    RenderFunction GetRenderFunction() const;
    
    void SetDrawContext(DrawContext* context);
    DrawContext* GetDrawContext() const;
    
    bool Execute() override;
    
    static std::shared_ptr<BasicRenderTask> Create();
    static std::shared_ptr<BasicRenderTask> Create(const std::string& id, const DrawParams& params);

private:
    RenderFunction m_renderFunction;
    DrawContext* m_context;
};

typedef std::shared_ptr<BasicRenderTask> BasicRenderTaskPtr;

}
}

#endif
