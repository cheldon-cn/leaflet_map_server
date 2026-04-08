#ifndef OGC_GRAPH_BASIC_RENDER_TASK_H
#define OGC_GRAPH_BASIC_RENDER_TASK_H

#include "ogc/graph/render/render_task.h"
#include <ogc/draw/draw_context.h>
#include <functional>

namespace ogc {
namespace graph {

typedef std::function<bool(RenderTaskPtr, ogc::draw::DrawContext&)> RenderFunction;

class OGC_GRAPH_API BasicRenderTask : public RenderTask {
public:
    BasicRenderTask();
    BasicRenderTask(const std::string& id, const DrawParams& params);
    ~BasicRenderTask() override = default;
    
    void SetRenderFunction(RenderFunction func);
    RenderFunction GetRenderFunction() const;
    
    void SetDrawContext(ogc::draw::DrawContext* context);
    ogc::draw::DrawContext* GetDrawContext() const;
    
    bool Execute() override;
    
    static std::shared_ptr<BasicRenderTask> Create();
    static std::shared_ptr<BasicRenderTask> Create(const std::string& id, const DrawParams& params);

private:
    RenderFunction m_renderFunction;
    ogc::draw::DrawContext* m_context;
};

typedef std::shared_ptr<BasicRenderTask> BasicRenderTaskPtr;

}
}

#endif
