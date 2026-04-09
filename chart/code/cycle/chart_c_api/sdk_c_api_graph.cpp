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
#include <ogc/graph/layer/layer_manager.h>
#include <ogc/graph/label/label_engine.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;
using namespace ogc::graph;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

struct ViewportData {
    double center_x;
    double center_y;
    double scale;
    double rotation;
    Envelope bounds;
    
    ViewportData() : center_x(0.0), center_y(0.0), scale(1.0), rotation(0.0) {}
};

}  

ogc_viewport_t* ogc_viewport_create(void) {
    return reinterpret_cast<ogc_viewport_t*>(new ViewportData());
}

void ogc_viewport_destroy(ogc_viewport_t* viewport) {
    delete reinterpret_cast<ViewportData*>(viewport);
}

double ogc_viewport_get_center_x(const ogc_viewport_t* viewport) {
    if (viewport) {
        return reinterpret_cast<const ViewportData*>(viewport)->center_x;
    }
    return 0.0;
}

double ogc_viewport_get_center_y(const ogc_viewport_t* viewport) {
    if (viewport) {
        return reinterpret_cast<const ViewportData*>(viewport)->center_y;
    }
    return 0.0;
}

double ogc_viewport_get_scale(const ogc_viewport_t* viewport) {
    if (viewport) {
        return reinterpret_cast<const ViewportData*>(viewport)->scale;
    }
    return 1.0;
}

double ogc_viewport_get_rotation(const ogc_viewport_t* viewport) {
    if (viewport) {
        return reinterpret_cast<const ViewportData*>(viewport)->rotation;
    }
    return 0.0;
}

void ogc_viewport_set_center(ogc_viewport_t* viewport, double x, double y) {
    if (viewport) {
        ViewportData* data = reinterpret_cast<ViewportData*>(viewport);
        data->center_x = x;
        data->center_y = y;
    }
}

void ogc_viewport_set_scale(ogc_viewport_t* viewport, double scale) {
    if (viewport) {
        reinterpret_cast<ViewportData*>(viewport)->scale = scale;
    }
}

void ogc_viewport_set_rotation(ogc_viewport_t* viewport, double rotation) {
    if (viewport) {
        reinterpret_cast<ViewportData*>(viewport)->rotation = rotation;
    }
}

ogc_envelope_t* ogc_viewport_get_bounds(const ogc_viewport_t* viewport) {
    if (viewport) {
        const ViewportData* data = reinterpret_cast<const ViewportData*>(viewport);
        return reinterpret_cast<ogc_envelope_t*>(new Envelope(data->bounds));
    }
    return nullptr;
}

int ogc_viewport_zoom_to_extent(ogc_viewport_t* viewport, const ogc_envelope_t* extent) {
    if (viewport && extent) {
        ViewportData* data = reinterpret_cast<ViewportData*>(viewport);
        const Envelope* env = reinterpret_cast<const Envelope*>(extent);
        data->bounds = *env;
        data->center_x = (env->GetMinX() + env->GetMaxX()) / 2.0;
        data->center_y = (env->GetMinY() + env->GetMaxY()) / 2.0;
        return 0;
    }
    return -1;
}

int ogc_viewport_zoom_to_scale(ogc_viewport_t* viewport, double scale) {
    if (viewport) {
        reinterpret_cast<ViewportData*>(viewport)->scale = scale;
        return 0;
    }
    return -1;
}

ogc_chart_viewer_t* ogc_chart_viewer_create(void) {
    return reinterpret_cast<ogc_chart_viewer_t*>(&DrawFacade::Instance());
}

void ogc_chart_viewer_destroy(ogc_chart_viewer_t* viewer) {
}

int ogc_chart_viewer_initialize(ogc_chart_viewer_t* viewer) {
    if (viewer) {
        DrawResult result = reinterpret_cast<DrawFacade*>(viewer)->Initialize();
        return result == DrawResult::kSuccess ? 0 : -1;
    }
    return -1;
}

void ogc_chart_viewer_shutdown(ogc_chart_viewer_t* viewer) {
    if (viewer) {
        reinterpret_cast<DrawFacade*>(viewer)->Finalize();
    }
}

int ogc_chart_viewer_load_chart(ogc_chart_viewer_t* viewer, const char* path) {
    return -1;
}

int ogc_chart_viewer_render(ogc_chart_viewer_t* viewer, ogc_draw_device_t* device, int width, int height) {
    if (viewer && device) {
        return 0;
    }
    return -1;
}

void ogc_chart_viewer_set_viewport(ogc_chart_viewer_t* viewer, double center_x, double center_y, double scale) {
}

void ogc_chart_viewer_get_viewport(ogc_chart_viewer_t* viewer, double* center_x, double* center_y, double* scale) {
    if (center_x) *center_x = 0.0;
    if (center_y) *center_y = 0.0;
    if (scale) *scale = 1.0;
}

void ogc_chart_viewer_pan(ogc_chart_viewer_t* viewer, double dx, double dy) {
}

void ogc_chart_viewer_zoom(ogc_chart_viewer_t* viewer, double factor, double center_x, double center_y) {
}

ogc_feature_t* ogc_chart_viewer_query_feature(ogc_chart_viewer_t* viewer, double x, double y) {
    return nullptr;
}

void ogc_chart_viewer_screen_to_world(ogc_chart_viewer_t* viewer, double screen_x, double screen_y, double* world_x, double* world_y) {
    if (world_x) *world_x = screen_x;
    if (world_y) *world_y = screen_y;
}

void ogc_chart_viewer_world_to_screen(ogc_chart_viewer_t* viewer, double world_x, double world_y, double* screen_x, double* screen_y) {
    if (screen_x) *screen_x = world_x;
    if (screen_y) *screen_y = world_y;
}

ogc_layer_manager_t* ogc_layer_manager_create(void) {
    return reinterpret_cast<ogc_layer_manager_t*>(new LayerManager());
}

void ogc_layer_manager_destroy(ogc_layer_manager_t* manager) {
    delete reinterpret_cast<LayerManager*>(manager);
}

size_t ogc_layer_manager_get_layer_count(const ogc_layer_manager_t* manager) {
    if (manager) {
        return static_cast<size_t>(reinterpret_cast<const LayerManager*>(manager)->GetLayerCount());
    }
    return 0;
}

ogc_layer_t* ogc_layer_manager_get_layer_at(ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        LayerItem* item = reinterpret_cast<LayerManager*>(manager)->GetLayer(static_cast<int>(index));
        if (item) {
            return reinterpret_cast<ogc_layer_t*>(item->GetLayer());
        }
    }
    return nullptr;
}

void ogc_layer_manager_add_layer(ogc_layer_manager_t* manager, ogc_layer_t* layer) {
    if (manager && layer) {
        reinterpret_cast<LayerManager*>(manager)->AddLayer(
            reinterpret_cast<CNLayer*>(layer));
    }
}

void ogc_layer_manager_remove_layer(ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        reinterpret_cast<LayerManager*>(manager)->RemoveLayer(static_cast<int>(index));
    }
}

void ogc_layer_manager_move_layer(ogc_layer_manager_t* manager, size_t from_index, size_t to_index) {
    if (manager) {
        reinterpret_cast<LayerManager*>(manager)->MoveLayer(static_cast<int>(from_index), static_cast<int>(to_index));
    }
}

ogc_layer_t* ogc_layer_manager_find_layer_by_name(ogc_layer_manager_t* manager, const char* name) {
    if (manager && name) {
        LayerItem* item = reinterpret_cast<LayerManager*>(manager)->GetLayer(SafeString(name));
        if (item) {
            return reinterpret_cast<ogc_layer_t*>(item->GetLayer());
        }
    }
    return nullptr;
}

void ogc_layer_manager_set_layer_visible(ogc_layer_manager_t* manager, size_t index, int visible) {
    if (manager) {
        LayerVisibility visibility = visible ? LayerVisibility::kVisible : LayerVisibility::kHidden;
        reinterpret_cast<LayerManager*>(manager)->SetLayerVisibility(static_cast<int>(index), visibility);
    }
}

int ogc_layer_manager_get_layer_visible(const ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        const LayerItem* item = reinterpret_cast<const LayerManager*>(manager)->GetLayer(static_cast<int>(index));
        if (item) {
            return item->IsVisible() ? 1 : 0;
        }
    }
    return 0;
}

double ogc_layer_manager_get_layer_opacity(const ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        const LayerItem* item = reinterpret_cast<const LayerManager*>(manager)->GetLayer(static_cast<int>(index));
        if (item) {
            return item->GetConfig().GetOpacity();
        }
    }
    return 1.0;
}

void ogc_layer_manager_set_layer_opacity(ogc_layer_manager_t* manager, size_t index, double opacity) {
    if (manager) {
        reinterpret_cast<LayerManager*>(manager)->SetLayerOpacity(static_cast<int>(index), opacity);
    }
}

ogc_label_engine_t* ogc_label_engine_create(void) {
    return reinterpret_cast<ogc_label_engine_t*>(new LabelEngine());
}

void ogc_label_engine_destroy(ogc_label_engine_t* engine) {
    delete reinterpret_cast<LabelEngine*>(engine);
}

void ogc_label_engine_set_max_labels(ogc_label_engine_t* engine, int max_labels) {
    (void)engine;
    (void)max_labels;
}

int ogc_label_engine_get_max_labels(const ogc_label_engine_t* engine) {
    (void)engine;
    return 100;
}

void ogc_label_engine_set_collision_detection(ogc_label_engine_t* engine, int enable) {
    (void)engine;
    (void)enable;
}

void ogc_label_engine_clear(ogc_label_engine_t* engine) {
    if (engine) {
        reinterpret_cast<LabelEngine*>(engine)->ClearCache();
    }
}

ogc_render_queue_t* ogc_render_queue_create(void) {
    return reinterpret_cast<ogc_render_queue_t*>(new RenderQueue());
}

void ogc_render_queue_destroy(ogc_render_queue_t* queue) {
    delete reinterpret_cast<RenderQueue*>(queue);
}

void ogc_render_queue_push(ogc_render_queue_t* queue, ogc_render_task_t* task) {
    if (queue && task) {
        RenderTaskPtr* taskPtr = reinterpret_cast<RenderTaskPtr*>(task);
        reinterpret_cast<RenderQueue*>(queue)->Enqueue(*taskPtr);
    }
}

ogc_render_task_t* ogc_render_queue_pop(ogc_render_queue_t* queue) {
    if (queue) {
        RenderTaskPtr task = reinterpret_cast<RenderQueue*>(queue)->Dequeue();
        if (task) {
            return reinterpret_cast<ogc_render_task_t*>(new RenderTaskPtr(std::move(task)));
        }
    }
    return nullptr;
}

size_t ogc_render_queue_get_size(const ogc_render_queue_t* queue) {
    if (queue) {
        return reinterpret_cast<const RenderQueue*>(queue)->GetSize();
    }
    return 0;
}

void ogc_render_queue_clear(ogc_render_queue_t* queue) {
    if (queue) {
        reinterpret_cast<RenderQueue*>(queue)->Clear();
    }
}

int ogc_render_queue_execute(ogc_render_queue_t* queue, ogc_draw_device_t* device) {
    (void)queue;
    (void)device;
    return 0;
}

ogc_render_task_t* ogc_render_task_create(void) {
    return reinterpret_cast<ogc_render_task_t*>(new RenderTaskPtr(RenderTask::Create()));
}

void ogc_render_task_destroy(ogc_render_task_t* task) {
    if (task) {
        RenderTaskPtr* ptr = reinterpret_cast<RenderTaskPtr*>(task);
        delete ptr;
    }
}

int ogc_render_task_execute(ogc_render_task_t* task) {
    (void)task;
    return 0;
}

int ogc_render_task_is_complete(const ogc_render_task_t* task) {
    if (task) {
        const RenderTaskPtr* ptr = reinterpret_cast<const RenderTaskPtr*>(task);
        return (*ptr)->IsCompleted() ? 1 : 0;
    }
    return 0;
}

void ogc_render_task_cancel(ogc_render_task_t* task) {
    if (task) {
        RenderTaskPtr* ptr = reinterpret_cast<RenderTaskPtr*>(task);
        (*ptr)->Cancel();
    }
}

#ifdef __cplusplus
}
#endif
