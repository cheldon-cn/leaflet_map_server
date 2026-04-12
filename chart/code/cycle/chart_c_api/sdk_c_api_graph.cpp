/**
 * @file sdk_c_api_graph.cpp
 * @brief OGC Chart SDK C API - Graph Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#define _USE_MATH_DEFINES
#include "sdk_c_api.h"

#include <ogc/graph/render/draw_facade.h>
#include <ogc/graph/render/draw_params.h>
#include <ogc/graph/render/render_task.h>
#include <ogc/graph/render/render_queue.h>
#include <ogc/graph/layer/layer_manager.h>
#include <ogc/graph/label/label_engine.h>
#include <ogc/graph/util/transform_manager.h>
#include <ogc/graph/interaction/hit_test.h>

#include <ogc/layer/memory_layer.h>
#include <ogc/geom/envelope.h>
#include <ogc/geom/coordinate.h>

#include <ogc/draw/draw_context.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <ogc/draw/transform_matrix.h>
#include <ogc/draw/geometry_types.h>
#include <ogc/draw/raster_image_device.h>

#include <parser/s57_parser.h>
#include <parser/chart_parser.h>

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace ogc;
using namespace ogc::graph;
using namespace chart::parser;

typedef struct ogc_image_device_t ogc_image_device_t;
extern "C" const unsigned char* ogc_image_device_get_pixels(ogc_image_device_t* device, size_t* size);

namespace ogc {
namespace draw {
    class DrawContext;
    class RasterImageDevice;
}
}

extern "C" ogc::draw::DrawContext* ogc_image_device_get_context(ogc_image_device_t* device);
extern "C" ogc::draw::RasterImageDevice* ogc_image_device_get_raster_device(ogc_image_device_t* device);

#ifdef __cplusplus
extern "C" {
#endif

static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

struct ViewportData {
    double center_x;
    double center_y;
    double scale;
    double rotation;
    int32_t pixel_width;
    int32_t pixel_height;
    Envelope bounds;
    
    ViewportData() : center_x(0.0), center_y(0.0), scale(1.0), rotation(0.0), 
                     pixel_width(800), pixel_height(600) {}
};

struct ChartViewerData {
    DrawFacade* facade;
    LayerManager* layer_manager;
    ViewportData* viewport;
    Envelope full_extent;
    std::string chart_path;
    bool chart_loaded;
    std::vector<chart::parser::Feature> features;
    
    ChartViewerData() : facade(nullptr), layer_manager(nullptr), viewport(nullptr), 
                         chart_loaded(false) {
        facade = &DrawFacade::Instance();
        layer_manager = new LayerManager();
        viewport = new ViewportData();
    }
    
    ~ChartViewerData() {
        if (layer_manager) {
            delete layer_manager;
            layer_manager = nullptr;
        }
        if (viewport) {
            delete viewport;
            viewport = nullptr;
        }
    }
};

static std::map<ogc_chart_viewer_t*, ChartViewerData*> g_chart_viewers;

static ChartViewerData* GetChartViewerData(ogc_chart_viewer_t* viewer) {
    auto it = g_chart_viewers.find(viewer);
    if (it != g_chart_viewers.end()) {
        return it->second;
    }
    return nullptr;
}

static uint32_t GetFeatureColor(chart::parser::FeatureType type) {
    switch (type) {
        case chart::parser::FeatureType::DEPARE: return 0xFF406080;
        case chart::parser::FeatureType::DEPCNT: return 0xFF306090;
        case chart::parser::FeatureType::COALNE: return 0xFF505050;
        case chart::parser::FeatureType::LNDARE: return 0xFF80C080;
        case chart::parser::FeatureType::SOUNDG: return 0xFF000080;
        case chart::parser::FeatureType::LIGHTS: return 0xFFFFFF00;
        case chart::parser::FeatureType::BOYLAT: return 0xFFFF8000;
        case chart::parser::FeatureType::BOYSAW: return 0xFF00FF00;
        case chart::parser::FeatureType::BCNLAT: return 0xFF000000;
        case chart::parser::FeatureType::BCNSAW: return 0xFF000000;
        default: return 0xFF808080;
    }
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

int ogc_viewport_screen_to_world(const ogc_viewport_t* viewport, double sx, double sy, double* wx, double* wy) {
    if (!viewport || !wx || !wy) {
        return -1;
    }
    
    const ViewportData* data = reinterpret_cast<const ViewportData*>(viewport);
    
    double half_width = data->pixel_width / 2.0;
    double half_height = data->pixel_height / 2.0;
    
    double dx = (sx - half_width) / data->scale;
    double dy = (half_height - sy) / data->scale;
    
    if (data->rotation != 0.0) {
        double rad = data->rotation * M_PI / 180.0;
        double cos_r = cos(rad);
        double sin_r = sin(rad);
        double new_dx = dx * cos_r - dy * sin_r;
        double new_dy = dx * sin_r + dy * cos_r;
        dx = new_dx;
        dy = new_dy;
    }
    
    *wx = data->center_x + dx;
    *wy = data->center_y + dy;
    
    return 0;
}

int ogc_viewport_world_to_screen(const ogc_viewport_t* viewport, double wx, double wy, double* sx, double* sy) {
    if (!viewport || !sx || !sy) {
        return -1;
    }
    
    const ViewportData* data = reinterpret_cast<const ViewportData*>(viewport);
    
    double dx = wx - data->center_x;
    double dy = wy - data->center_y;
    
    if (data->rotation != 0.0) {
        double rad = -data->rotation * M_PI / 180.0;
        double cos_r = cos(rad);
        double sin_r = sin(rad);
        double new_dx = dx * cos_r - dy * sin_r;
        double new_dy = dx * sin_r + dy * cos_r;
        dx = new_dx;
        dy = new_dy;
    }
    
    double half_width = data->pixel_width / 2.0;
    double half_height = data->pixel_height / 2.0;
    
    *sx = half_width + dx * data->scale;
    *sy = half_height - dy * data->scale;
    
    return 0;
}

SDK_C_API void ogc_viewport_pan(ogc_viewport_t* viewport, double dx, double dy) {
    if (viewport) {
        ViewportData* data = reinterpret_cast<ViewportData*>(viewport);
        data->center_x += dx;
        data->center_y += dy;
    }
}

SDK_C_API void ogc_viewport_zoom(ogc_viewport_t* viewport, double factor) {
    if (viewport && factor > 0) {
        ViewportData* data = reinterpret_cast<ViewportData*>(viewport);
        data->scale *= factor;
    }
}

SDK_C_API void ogc_viewport_zoom_at(ogc_viewport_t* viewport, double factor, double center_x, double center_y) {
    if (viewport && factor > 0) {
        ViewportData* data = reinterpret_cast<ViewportData*>(viewport);
        double dx = center_x - data->center_x;
        double dy = center_y - data->center_y;
        data->center_x += dx * (1.0 - 1.0 / factor);
        data->center_y += dy * (1.0 - 1.0 / factor);
        data->scale *= factor;
    }
}

SDK_C_API int ogc_viewport_set_extent(ogc_viewport_t* viewport, double min_x, double min_y, double max_x, double max_y) {
    if (viewport) {
        ViewportData* data = reinterpret_cast<ViewportData*>(viewport);
        data->bounds = Envelope(min_x, min_y, max_x, max_y);
        data->center_x = (min_x + max_x) / 2.0;
        data->center_y = (min_y + max_y) / 2.0;
        if (data->pixel_width > 0 && data->pixel_height > 0) {
            double width = max_x - min_x;
            double height = max_y - min_y;
            double scale_x = data->pixel_width / width;
            double scale_y = data->pixel_height / height;
            data->scale = (scale_x < scale_y) ? scale_x : scale_y;
        }
        return 0;
    }
    return -1;
}

SDK_C_API int ogc_viewport_get_extent(const ogc_viewport_t* viewport, double* min_x, double* min_y, double* max_x, double* max_y) {
    if (!viewport || !min_x || !min_y || !max_x || !max_y) {
        return -1;
    }
    
    const ViewportData* data = reinterpret_cast<const ViewportData*>(viewport);
    
    double half_width = (data->pixel_width / 2.0) / data->scale;
    double half_height = (data->pixel_height / 2.0) / data->scale;
    
    *min_x = data->center_x - half_width;
    *min_y = data->center_y - half_height;
    *max_x = data->center_x + half_width;
    *max_y = data->center_y + half_height;
    
    return 0;
}

ogc_chart_viewer_t* ogc_chart_viewer_create(void) {
    ChartViewerData* data = new ChartViewerData();
    ogc_chart_viewer_t* viewer = reinterpret_cast<ogc_chart_viewer_t*>(data);
    g_chart_viewers[viewer] = data;
    return viewer;
}

void ogc_chart_viewer_destroy(ogc_chart_viewer_t* viewer) {
    auto it = g_chart_viewers.find(viewer);
    if (it != g_chart_viewers.end()) {
        ChartViewerData* data = it->second;
        g_chart_viewers.erase(it);
        delete data;
    }
}

int ogc_chart_viewer_initialize(ogc_chart_viewer_t* viewer) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->facade) {
        DrawResult result = data->facade->Initialize();
        return result == DrawResult::kSuccess ? 0 : -1;
    }
    return -1;
}

void ogc_chart_viewer_shutdown(ogc_chart_viewer_t* viewer) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->facade) {
        data->facade->Finalize();
    }
}

int ogc_chart_viewer_load_chart(ogc_chart_viewer_t* viewer, const char* path) {
    if (!viewer || !path) {
        return -1;
    }
    
    ChartViewerData* data = GetChartViewerData(viewer);
    if (!data) {
        return -1;
    }
    
    S57Parser parser;
    ParseResult result = parser.ParseChart(path);
    
    if (!result.success) {
        return -2;
    }
    
    data->features = std::move(result.features);
    data->chart_path = path;
    data->chart_loaded = true;
    
    std::cout << "[DEBUG] Loaded " << data->features.size() << " features" << std::endl;
    
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();
    
    for (const auto& feature : data->features) {
        const auto& geom = feature.geometry;
        for (const auto& pt : geom.points) {
            min_x = std::min(min_x, pt.x);
            min_y = std::min(min_y, pt.y);
            max_x = std::max(max_x, pt.x);
            max_y = std::max(max_y, pt.y);
        }
        for (const auto& ring : geom.rings) {
            for (const auto& pt : ring) {
                min_x = std::min(min_x, pt.x);
                min_y = std::min(min_y, pt.y);
                max_x = std::max(max_x, pt.x);
                max_y = std::max(max_y, pt.y);
            }
        }
    }
    
    std::cout << "[DEBUG] Calculated extent: " << min_x << ", " << min_y << " - " << max_x << ", " << max_y << std::endl;
    
    if (min_x < max_x && min_y < max_y) {
        data->full_extent = Envelope(min_x, min_y, max_x, max_y);
        if (data->viewport) {
            data->viewport->bounds = data->full_extent;
            data->viewport->center_x = (min_x + max_x) / 2.0;
            data->viewport->center_y = (min_y + max_y) / 2.0;
        }
    }
    
    return 0;
}

int ogc_chart_viewer_render(ogc_chart_viewer_t* viewer, ogc_draw_device_t* device, int width, int height) {
    if (!viewer || !device || width <= 0 || height <= 0) {
        return -1;
    }
    
    ChartViewerData* data = GetChartViewerData(viewer);
    if (!data) {
        return -1;
    }
    
    ogc::draw::DrawContext* context = ogc_image_device_get_context(
        reinterpret_cast<ogc_image_device_t*>(device));
    
    if (!context) {
        std::cerr << "[ERROR] Failed to get DrawContext" << std::endl;
        return -1;
    }
    
    ogc::draw::RasterImageDevice* raster_device = ogc_image_device_get_raster_device(
        reinterpret_cast<ogc_image_device_t*>(device));
    
    if (!data->chart_loaded || data->features.empty()) {
        if (context->Begin() == ogc::draw::DrawResult::kSuccess) {
            ogc::draw::Color bg_color(200, 200, 200, 255);
            context->Clear(bg_color);
            context->End();
        }
        return 0;
    }
    
    ViewportData* vp = data->viewport;
    if (!vp) {
        return 0;
    }
    
    double extent_width = data->full_extent.GetWidth();
    double extent_height = data->full_extent.GetHeight();
    
    if (extent_width <= 0 || extent_height <= 0) {
        return 0;
    }
    
    double scale_x = width / extent_width;
    double scale_y = height / extent_height;
    double scale = (scale_x < scale_y) ? scale_x : scale_y;
    
    double offset_x = (width - extent_width * scale) / 2.0;
    double offset_y = (height - extent_height * scale) / 2.0;
    
    std::cout << "[DEBUG] Extent: " << data->full_extent.GetMinX() << ", " << data->full_extent.GetMinY() 
              << " - " << data->full_extent.GetMaxX() << ", " << data->full_extent.GetMaxY() << std::endl;
    std::cout << "[DEBUG] Extent size: " << extent_width << " x " << extent_height << std::endl;
    std::cout << "[DEBUG] Canvas size: " << width << " x " << height << std::endl;
    std::cout << "[DEBUG] Scale: " << scale << std::endl;
    std::cout << "[DEBUG] Offset: " << offset_x << ", " << offset_y << std::endl;
    
    if (context->Begin() != ogc::draw::DrawResult::kSuccess) {
        std::cerr << "[ERROR] DrawContext::Begin() failed" << std::endl;
        return -1;
    }
    
    ogc::draw::Color bg_color(240, 248, 255, 255);
    context->Clear(bg_color);
    
    ogc::draw::TransformMatrix transform;
    transform = ogc::draw::TransformMatrix::Translate(offset_x, offset_y);
    transform = transform * ogc::draw::TransformMatrix::Scale(scale, scale);
    transform = transform * ogc::draw::TransformMatrix::Translate(-data->full_extent.GetMinX(), -data->full_extent.GetMinY());
    
    ogc::draw::TransformMatrix flip_y;
    flip_y.m[0][0] = 1.0;
    flip_y.m[1][1] = -1.0;
    flip_y.m[1][2] = height;
    
    transform = flip_y * transform;
    
    context->SetTransform(transform);
    
    int point_count = 0;
    int line_count = 0;
    int area_count = 0;
    
    ogc::draw::Color area_colors[3] = {
        ogc::draw::Color(173, 216, 230, 180),
        ogc::draw::Color(255, 255, 224, 180),
        ogc::draw::Color(144, 238, 144, 180)
    };
    ogc::draw::Color area_stroke_colors[3] = {
        ogc::draw::Color(100, 149, 237, 255),
        ogc::draw::Color(218, 165, 32, 255),
        ogc::draw::Color(34, 139, 34, 255)
    };
    
    int area_idx = 0;
    for (const auto& feature : data->features) {
        if (feature.geometry.type == chart::parser::GeometryType::Area) {
            area_count++;
            int color_idx = (area_idx / 2) % 3;
            ogc::draw::Pen area_pen(area_stroke_colors[color_idx], 1.0);
            ogc::draw::Brush area_brush(area_colors[color_idx]);
            context->SetPen(area_pen);
            context->SetBrush(area_brush);
            if (!feature.geometry.rings.empty() && feature.geometry.rings[0].size() >= 3) {
                std::vector<ogc::draw::Point> points;
                for (const auto& pt : feature.geometry.rings[0]) {
                    points.push_back(ogc::draw::Point(pt.x, pt.y));
                }
                context->DrawPolygon(points, true);
            }
            area_idx++;
        }
    }
    
    double line_width_pixels = 2.0 * 96.0 / 25.4;
    for (const auto& feature : data->features) {
        if (feature.geometry.type == chart::parser::GeometryType::Line) {
            line_count++;
            uint32_t color_value = GetFeatureColor(feature.type);
            ogc::draw::Color stroke_color(
                (color_value >> 16) & 0xFF,
                (color_value >> 8) & 0xFF,
                (color_value >> 0) & 0xFF,
                255
            );
            ogc::draw::Pen pen(stroke_color, line_width_pixels / scale);
            context->SetPen(pen);
            if (feature.geometry.points.size() >= 2) {
                std::vector<ogc::draw::Point> points;
                for (const auto& pt : feature.geometry.points) {
                    points.push_back(ogc::draw::Point(pt.x, pt.y));
                }
                context->DrawLineString(points);
            }
        }
    }
    
    double point_radius_pixels = 1.5 * 96.0 / 25.4;
    for (const auto& feature : data->features) {
        if (feature.geometry.type == chart::parser::GeometryType::Point) {
            point_count++;
            uint32_t color_value = GetFeatureColor(feature.type);
            ogc::draw::Color stroke_color(
                (color_value >> 16) & 0xFF,
                (color_value >> 8) & 0xFF,
                (color_value >> 0) & 0xFF,
                255
            );
            ogc::draw::Color fill_color(
                ((color_value >> 16) & 0xFF) * 0.7,
                ((color_value >> 8) & 0xFF) * 0.7,
                ((color_value >> 0) & 0xFF) * 0.7,
                200
            );
            ogc::draw::Pen pen(stroke_color, 1.0);
            ogc::draw::Brush brush(fill_color);
            context->SetPen(pen);
            context->SetBrush(brush);
            if (!feature.geometry.points.empty()) {
                const auto& pt = feature.geometry.points[0];
                context->DrawCircle(pt.x, pt.y, point_radius_pixels / scale, true);
            }
        }
    }
    
    context->End();
    
    std::cout << "========== chart render statistics ==========" << std::endl;
    std::cout << "Total point count: " << point_count << std::endl;
    std::cout << "Total line count: " << line_count << std::endl;
    std::cout << "Total area count: " << area_count << std::endl;
    std::cout << "Total feature count: " << (point_count + line_count + area_count) << std::endl;
    std::cout << "=================================" << std::endl;
    
    return 0;
}

void ogc_chart_viewer_set_viewport(ogc_chart_viewer_t* viewer, double center_x, double center_y, double scale) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->viewport) {
        data->viewport->center_x = center_x;
        data->viewport->center_y = center_y;
        data->viewport->scale = scale;
    }
}

void ogc_chart_viewer_get_viewport(ogc_chart_viewer_t* viewer, double* center_x, double* center_y, double* scale) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->viewport) {
        if (center_x) *center_x = data->viewport->center_x;
        if (center_y) *center_y = data->viewport->center_y;
        if (scale) *scale = data->viewport->scale;
    } else {
        if (center_x) *center_x = 0.0;
        if (center_y) *center_y = 0.0;
        if (scale) *scale = 1.0;
    }
}

void ogc_chart_viewer_pan(ogc_chart_viewer_t* viewer, double dx, double dy) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->viewport) {
        data->viewport->center_x += dx;
        data->viewport->center_y += dy;
    }
}

void ogc_chart_viewer_zoom(ogc_chart_viewer_t* viewer, double factor, double center_x, double center_y) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->viewport) {
        double dx = center_x - data->viewport->center_x;
        double dy = center_y - data->viewport->center_y;
        data->viewport->center_x += dx * (1.0 - 1.0 / factor);
        data->viewport->center_y += dy * (1.0 - 1.0 / factor);
        data->viewport->scale *= factor;
    }
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

SDK_C_API ogc_viewport_t* ogc_chart_viewer_get_viewport_ptr(ogc_chart_viewer_t* viewer) {
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->viewport) {
        return reinterpret_cast<ogc_viewport_t*>(data->viewport);
    }
    return nullptr;
}

SDK_C_API int ogc_chart_viewer_get_full_extent(ogc_chart_viewer_t* viewer, double* min_x, double* min_y, double* max_x, double* max_y) {
    if (!viewer || !min_x || !min_y || !max_x || !max_y) {
        return -1;
    }
    
    ChartViewerData* data = GetChartViewerData(viewer);
    if (data && data->chart_loaded) {
        *min_x = data->full_extent.GetMinX();
        *min_y = data->full_extent.GetMinY();
        *max_x = data->full_extent.GetMaxX();
        *max_y = data->full_extent.GetMaxY();
        return 0;
    }
    
    *min_x = -180.0;
    *min_y = -90.0;
    *max_x = 180.0;
    *max_y = 90.0;
    
    return 0;
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

ogc_layer_t* ogc_layer_manager_get_layer(const ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        LayerItem* item = reinterpret_cast<LayerManager*>(const_cast<ogc_layer_manager_t*>(manager))->GetLayer(static_cast<int>(index));
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

ogc_layer_t* ogc_layer_manager_get_layer_by_name(const ogc_layer_manager_t* manager, const char* name) {
    return ogc_layer_manager_find_layer_by_name(const_cast<ogc_layer_manager_t*>(manager), name);
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

int ogc_layer_manager_get_layer_z_order(const ogc_layer_manager_t* manager, size_t index) {
    if (manager) {
        return reinterpret_cast<const LayerManager*>(manager)->GetLayerZOrder(static_cast<int>(index));
    }
    return 0;
}

void ogc_layer_manager_set_layer_z_order(ogc_layer_manager_t* manager, size_t index, int z_order) {
    if (manager) {
        reinterpret_cast<LayerManager*>(manager)->SetLayerZOrder(static_cast<int>(index), z_order);
    }
}

void ogc_layer_manager_sort_by_z_order(ogc_layer_manager_t* manager) {
    if (manager) {
        reinterpret_cast<LayerManager*>(manager)->SortLayersByZOrder();
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

ogc_transform_manager_t* ogc_transform_manager_create(void) {
    return reinterpret_cast<ogc_transform_manager_t*>(TransformManager::GetInstance().get());
}

void ogc_transform_manager_destroy(ogc_transform_manager_t* mgr) {
    (void)mgr;
}

void ogc_transform_manager_set_viewport(ogc_transform_manager_t* mgr, double center_x, double center_y, double scale, double rotation) {
    (void)mgr; (void)center_x; (void)center_y; (void)scale; (void)rotation;
}

void ogc_transform_manager_get_matrix(const ogc_transform_manager_t* mgr, double* matrix) {
    (void)mgr; (void)matrix;
}

ogc_hit_test_t* ogc_hit_test_create(void) {
    return reinterpret_cast<ogc_hit_test_t*>(HitTestResult::Create().get());
}

void ogc_hit_test_destroy(ogc_hit_test_t* hit_test) {
    (void)hit_test;
}

ogc_feature_t* ogc_hit_test_query_point(ogc_hit_test_t* hit_test, ogc_layer_t* layer, double x, double y, double tolerance) {
    (void)hit_test; (void)layer; (void)x; (void)y; (void)tolerance;
    return nullptr;
}

void ogc_hit_test_free_features(ogc_feature_t** features) {
    (void)features;
}

ogc_label_info_t* ogc_label_info_create(const char* text, double x, double y) {
    (void)text; (void)x; (void)y;
    return nullptr;
}

void ogc_label_info_destroy(ogc_label_info_t* info) {
    (void)info;
}

const char* ogc_label_info_get_text(const ogc_label_info_t* info) {
    (void)info;
    return "";
}

double ogc_label_info_get_x(const ogc_label_info_t* info) {
    (void)info;
    return 0.0;
}

double ogc_label_info_get_y(const ogc_label_info_t* info) {
    (void)info;
    return 0.0;
}

void ogc_label_info_set_text(ogc_label_info_t* info, const char* text) {
    (void)info; (void)text;
}

void ogc_label_info_set_position(ogc_label_info_t* info, double x, double y) {
    (void)info; (void)x; (void)y;
}

ogc_lod_manager_t* ogc_lod_manager_create(void) {
    return nullptr;
}

void ogc_lod_manager_destroy(ogc_lod_manager_t* mgr) {
    (void)mgr;
}

int ogc_lod_manager_get_current_level(const ogc_lod_manager_t* mgr) {
    (void)mgr;
    return 0;
}

void ogc_lod_manager_set_current_level(ogc_lod_manager_t* mgr, int level) {
    (void)mgr; (void)level;
}

int ogc_lod_manager_get_level_count(const ogc_lod_manager_t* mgr) {
    (void)mgr;
    return 0;
}

double ogc_lod_manager_get_scale_for_level(const ogc_lod_manager_t* mgr, int level) {
    (void)mgr; (void)level;
    return 0.0;
}

/* ============================================================================
 * ChartConfig Implementation
 * ============================================================================ */

namespace {

struct ChartConfigData {
    int display_mode;
    int show_grid;
    double dpi;
    
    ChartConfigData() : display_mode(0), show_grid(0), dpi(96.0) {}
};

}  

ogc_chart_config_t* ogc_chart_config_create(void) {
    return reinterpret_cast<ogc_chart_config_t*>(new ChartConfigData());
}

void ogc_chart_config_destroy(ogc_chart_config_t* config) {
    delete reinterpret_cast<ChartConfigData*>(config);
}

int ogc_chart_config_get_display_mode(const ogc_chart_config_t* config) {
    if (config) {
        return reinterpret_cast<const ChartConfigData*>(config)->display_mode;
    }
    return 0;
}

void ogc_chart_config_set_display_mode(ogc_chart_config_t* config, int mode) {
    if (config) {
        reinterpret_cast<ChartConfigData*>(config)->display_mode = mode;
    }
}

int ogc_chart_config_get_show_grid(const ogc_chart_config_t* config) {
    if (config) {
        return reinterpret_cast<const ChartConfigData*>(config)->show_grid;
    }
    return 0;
}

void ogc_chart_config_set_show_grid(ogc_chart_config_t* config, int show) {
    if (config) {
        reinterpret_cast<ChartConfigData*>(config)->show_grid = show ? 1 : 0;
    }
}

double ogc_chart_config_get_dpi(const ogc_chart_config_t* config) {
    if (config) {
        return reinterpret_cast<const ChartConfigData*>(config)->dpi;
    }
    return 96.0;
}

void ogc_chart_config_set_dpi(ogc_chart_config_t* config, double dpi) {
    if (config) {
        reinterpret_cast<ChartConfigData*>(config)->dpi = dpi;
    }
}

#ifdef __cplusplus
}
#endif
