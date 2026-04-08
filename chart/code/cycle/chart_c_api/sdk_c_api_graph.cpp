/**
 * @file sdk_c_api_graph.cpp
 * @brief OGC Chart SDK C API - Graph Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/graph/render/draw_facade.h>
#include <ogc/graph/render/draw_params.h>
#include <ogc/graph/render/render_task.h>
#include <ogc/graph/render/render_queue.h>
#include <ogc/graph/view/chart_view.h>
#include <ogc/graph/view/viewport.h>
#include <ogc/graph/layer/layer_manager.h>
#include <ogc/graph/label/label_engine.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::graph;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_chart_view_t* ogc_chart_view_create(void) {
    return reinterpret_cast<ogc_chart_view_t*>(ChartView::Create().release());
}

void ogc_chart_view_destroy(ogc_chart_view_t* view) {
    delete reinterpret_cast<ChartView*>(view);
}

ogc_viewport_t* ogc_chart_view_get_viewport(ogc_chart_view_t* view) {
    if (view) {
        return reinterpret_cast<ogc_viewport_t*>(
            reinterpret_cast<ChartView*>(view)->GetViewport());
    }
    return nullptr;
}

ogc_layer_manager_t* ogc_chart_view_get_layer_manager(ogc_chart_view_t* view) {
    if (view) {
        return reinterpret_cast<ogc_layer_manager_t*>(
            reinterpret_cast<ChartView*>(view)->GetLayerManager());
    }
    return nullptr;
}

void ogc_chart_view_set_extent(ogc_chart_view_t* view, const ogc_envelope_t* extent) {
    if (view && extent) {
        reinterpret_cast<ChartView*>(view)->SetExtent(
            *reinterpret_cast<const ogc::geom::Envelope*>(extent));
    }
}

ogc_envelope_t* ogc_chart_view_get_extent(const ogc_chart_view_t* view) {
    if (view) {
        const ogc::geom::Envelope& env = reinterpret_cast<const ChartView*>(view)->GetExtent();
        return reinterpret_cast<ogc_envelope_t*>(new ogc::geom::Envelope(env));
    }
    return nullptr;
}

void ogc_chart_view_set_center(ogc_chart_view_t* view, double x, double y) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->SetCenter(x, y);
    }
}

void ogc_chart_view_get_center(const ogc_chart_view_t* view, double* x, double* y) {
    if (view && x && y) {
        auto center = reinterpret_cast<const ChartView*>(view)->GetCenter();
        *x = center.x;
        *y = center.y;
    }
}

void ogc_chart_view_set_scale(ogc_chart_view_t* view, double scale) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->SetScale(scale);
    }
}

double ogc_chart_view_get_scale(const ogc_chart_view_t* view) {
    if (view) {
        return reinterpret_cast<const ChartView*>(view)->GetScale();
    }
    return 1.0;
}

void ogc_chart_view_set_rotation(ogc_chart_view_t* view, double angle) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->SetRotation(angle);
    }
}

double ogc_chart_view_get_rotation(const ogc_chart_view_t* view) {
    if (view) {
        return reinterpret_cast<const ChartView*>(view)->GetRotation();
    }
    return 0.0;
}

void ogc_chart_view_zoom_in(ogc_chart_view_t* view, double factor) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->ZoomIn(factor);
    }
}

void ogc_chart_view_zoom_out(ogc_chart_view_t* view, double factor) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->ZoomOut(factor);
    }
}

void ogc_chart_view_zoom_to_extent(ogc_chart_view_t* view, const ogc_envelope_t* extent) {
    if (view && extent) {
        reinterpret_cast<ChartView*>(view)->ZoomToExtent(
            *reinterpret_cast<const ogc::geom::Envelope*>(extent));
    }
}

void ogc_chart_view_pan(ogc_chart_view_t* view, double dx, double dy) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->Pan(dx, dy);
    }
}

int ogc_chart_view_screen_to_world(const ogc_chart_view_t* view, int screen_x, int screen_y, double* world_x, double* world_y) {
    if (view && world_x && world_y) {
        return reinterpret_cast<const ChartView*>(view)->ScreenToWorld(
            screen_x, screen_y, *world_x, *world_y) ? 1 : 0;
    }
    return 0;
}

int ogc_chart_view_world_to_screen(const ogc_chart_view_t* view, double world_x, double world_y, int* screen_x, int* screen_y) {
    if (view && screen_x && screen_y) {
        return reinterpret_cast<const ChartView*>(view)->WorldToScreen(
            world_x, world_y, *screen_x, *screen_y) ? 1 : 0;
    }
    return 0;
}

int ogc_chart_view_render(ogc_chart_view_t* view, ogc_draw_device_t* device) {
    if (view && device) {
        return reinterpret_cast<ChartView*>(view)->Render(
            reinterpret_cast<ogc::draw::DrawDevice*>(device)) ? 1 : 0;
    }
    return 0;
}

void ogc_chart_view_refresh(ogc_chart_view_t* view) {
    if (view) {
        reinterpret_cast<ChartView*>(view)->Refresh();
    }
}

ogc_viewport_t* ogc_viewport_create(void) {
    return reinterpret_cast<ogc_viewport_t*>(Viewport::Create().release());
}

void ogc_viewport_destroy(ogc_viewport_t* viewport) {
    delete reinterpret_cast<Viewport*>(viewport);
}

int ogc_viewport_get_width(const ogc_viewport_t* viewport) {
    if (viewport) {
        return reinterpret_cast<const Viewport*>(viewport)->GetWidth();
    }
    return 0;
}

int ogc_viewport_get_height(const ogc_viewport_t* viewport) {
    if (viewport) {
        return reinterpret_cast<const Viewport*>(viewport)->GetHeight();
    }
    return 0;
}

void ogc_viewport_set_size(ogc_viewport_t* viewport, int width, int height) {
    if (viewport) {
        reinterpret_cast<Viewport*>(viewport)->SetSize(width, height);
    }
}

ogc_envelope_t* ogc_viewport_get_visible_extent(const ogc_viewport_t* viewport) {
    if (viewport) {
        const ogc::geom::Envelope& env = reinterpret_cast<const Viewport*>(viewport)->GetVisibleExtent();
        return reinterpret_cast<ogc_envelope_t*>(new ogc::geom::Envelope(env));
    }
    return nullptr;
}

ogc_layer_manager_t* ogc_layer_manager_create(void) {
    return reinterpret_cast<ogc_layer_manager_t*>(LayerManager::Create().release());
}

void ogc_layer_manager_destroy(ogc_layer_manager_t* manager) {
    delete reinterpret_cast<LayerManager*>(manager);
}

size_t ogc_layer_manager_get_layer_count(const ogc_layer_manager_t* manager) {
    if (manager) {
        return reinterpret_cast<const LayerManager*>(manager)->GetLayerCount();
    }
    return 0;
}

ogc_layer_t* ogc_layer_manager_get_layer(ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<LayerManager*>(manager)->GetLayer(index));
    }
    return nullptr;
}

int ogc_layer_manager_add_layer(ogc_layer_manager_t* manager, ogc_layer_t* layer) {
    if (manager && layer) {
        return reinterpret_cast<LayerManager*>(manager)->AddLayer(
            ogc::layer::LayerPtr(reinterpret_cast<ogc::layer::Layer*>(layer))) ? 1 : 0;
    }
    return 0;
}

int ogc_layer_manager_remove_layer(ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        return reinterpret_cast<LayerManager*>(manager)->RemoveLayer(index) ? 1 : 0;
    }
    return 0;
}

int ogc_layer_manager_move_layer(ogc_layer_manager_t* manager, size_t from_index, size_t to_index) {
    if (manager) {
        return reinterpret_cast<LayerManager*>(manager)->MoveLayer(from_index, to_index) ? 1 : 0;
    }
    return 0;
}

ogc_layer_t* ogc_layer_manager_find_layer_by_name(ogc_layer_manager_t* manager, const char* name) {
    if (manager && name) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<LayerManager*>(manager)->FindLayerByName(SafeString(name)));
    }
    return nullptr;
}

void ogc_layer_manager_set_layer_visible(ogc_layer_manager_t* manager, size_t index, int visible) {
    if (manager) {
        reinterpret_cast<LayerManager*>(manager)->SetLayerVisible(index, visible != 0);
    }
}

ogc_label_engine_t* ogc_label_engine_create(void) {
    return reinterpret_cast<ogc_label_engine_t*>(LabelEngine::Create().release());
}

void ogc_label_engine_destroy(ogc_label_engine_t* engine) {
    delete reinterpret_cast<LabelEngine*>(engine);
}

void ogc_label_engine_clear(ogc_label_engine_t* engine) {
    if (engine) {
        reinterpret_cast<LabelEngine*>(engine)->Clear();
    }
}

int ogc_label_engine_add_label(ogc_label_engine_t* engine, const char* text, double x, double y, ogc_font_t* font) {
    if (engine && text && font) {
        return reinterpret_cast<LabelEngine*>(engine)->AddLabel(
            SafeString(text), x, y, *reinterpret_cast<ogc::draw::Font*>(font)) ? 1 : 0;
    }
    return 0;
}

int ogc_label_engine_process(ogc_label_engine_t* engine, const ogc_viewport_t* viewport) {
    if (engine && viewport) {
        return reinterpret_cast<LabelEngine*>(engine)->Process(
            *reinterpret_cast<const Viewport*>(viewport)) ? 1 : 0;
    }
    return 0;
}

size_t ogc_label_engine_get_label_count(const ogc_label_engine_t* engine) {
    if (engine) {
        return reinterpret_cast<const LabelEngine*>(engine)->GetLabelCount();
    }
    return 0;
}

ogc_draw_facade_t* ogc_draw_facade_create(void) {
    return reinterpret_cast<ogc_draw_facade_t*>(DrawFacade::Create().release());
}

void ogc_draw_facade_destroy(ogc_draw_facade_t* facade) {
    delete reinterpret_cast<DrawFacade*>(facade);
}

int ogc_draw_facade_initialize(ogc_draw_facade_t* facade, ogc_draw_device_t* device) {
    if (facade && device) {
        return reinterpret_cast<DrawFacade*>(facade)->Initialize(
            reinterpret_cast<ogc::draw::DrawDevice*>(device)) ? 1 : 0;
    }
    return 0;
}

void ogc_draw_facade_shutdown(ogc_draw_facade_t* facade) {
    if (facade) {
        reinterpret_cast<DrawFacade*>(facade)->Shutdown();
    }
}

void ogc_draw_facade_begin_frame(ogc_draw_facade_t* facade) {
    if (facade) {
        reinterpret_cast<DrawFacade*>(facade)->BeginFrame();
    }
}

void ogc_draw_facade_end_frame(ogc_draw_facade_t* facade) {
    if (facade) {
        reinterpret_cast<DrawFacade*>(facade)->EndFrame();
    }
}

void ogc_draw_facade_set_viewport(ogc_draw_facade_t* facade, const ogc_viewport_t* viewport) {
    if (facade && viewport) {
        reinterpret_cast<DrawFacade*>(facade)->SetViewport(
            *reinterpret_cast<const Viewport*>(viewport));
    }
}

ogc_render_queue_t* ogc_render_queue_create(void) {
    return reinterpret_cast<ogc_render_queue_t*>(RenderQueue::Create().release());
}

void ogc_render_queue_destroy(ogc_render_queue_t* queue) {
    delete reinterpret_cast<RenderQueue*>(queue);
}

void ogc_render_queue_clear(ogc_render_queue_t* queue) {
    if (queue) {
        reinterpret_cast<RenderQueue*>(queue)->Clear();
    }
}

size_t ogc_render_queue_get_size(const ogc_render_queue_t* queue) {
    if (queue) {
        return reinterpret_cast<const RenderQueue*>(queue)->GetSize();
    }
    return 0;
}

int ogc_render_queue_add_task(ogc_render_queue_t* queue, ogc_render_task_t* task) {
    if (queue && task) {
        return reinterpret_cast<RenderQueue*>(queue)->AddTask(
            RenderTaskPtr(reinterpret_cast<RenderTask*>(task))) ? 1 : 0;
    }
    return 0;
}

void ogc_render_queue_sort(ogc_render_queue_t* queue) {
    if (queue) {
        reinterpret_cast<RenderQueue*>(queue)->Sort();
    }
}

int ogc_render_queue_execute(ogc_render_queue_t* queue, ogc_draw_device_t* device) {
    if (queue && device) {
        return reinterpret_cast<RenderQueue*>(queue)->Execute(
            reinterpret_cast<ogc::draw::DrawDevice*>(device)) ? 1 : 0;
    }
    return 0;
}

ogc_render_task_t* ogc_render_task_create(int type) {
    return reinterpret_cast<ogc_render_task_t*>(RenderTask::Create(type).release());
}

void ogc_render_task_destroy(ogc_render_task_t* task) {
    delete reinterpret_cast<RenderTask*>(task);
}

int ogc_render_task_get_type(const ogc_render_task_t* task) {
    if (task) {
        return reinterpret_cast<const RenderTask*>(task)->GetType();
    }
    return 0;
}

int ogc_render_task_get_priority(const ogc_render_task_t* task) {
    if (task) {
        return reinterpret_cast<const RenderTask*>(task)->GetPriority();
    }
    return 0;
}

void ogc_render_task_set_priority(ogc_render_task_t* task, int priority) {
    if (task) {
        reinterpret_cast<RenderTask*>(task)->SetPriority(priority);
    }
}

int ogc_render_task_execute(ogc_render_task_t* task, ogc_draw_device_t* device) {
    if (task && device) {
        return reinterpret_cast<RenderTask*>(task)->Execute(
            reinterpret_cast<ogc::draw::DrawDevice*>(device)) ? 1 : 0;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
