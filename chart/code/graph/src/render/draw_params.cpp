#include "ogc/graph/render/draw_params.h"
#include <cmath>
#include <limits>

namespace ogc {
namespace graph {

DrawParams::DrawParams()
    : logical_width(0.0)
    , logical_height(0.0)
    , dpi(96.0)
    , scale(1.0)
    , rotation(0.0)
    , center_x(0.0)
    , center_y(0.0)
    , pixel_width(0)
    , pixel_height(0)
    , tolerance(0.0)
    , has_background(false)
    , background_color(0xFFFFFFFF) {
}

DrawParams::DrawParams(const Envelope& ext, int32_t width, int32_t height, double dpi_val)
    : extent(ext)
    , logical_width(0.0)
    , logical_height(0.0)
    , dpi(dpi_val)
    , scale(1.0)
    , rotation(0.0)
    , center_x(0.0)
    , center_y(0.0)
    , pixel_width(width)
    , pixel_height(height)
    , tolerance(0.0)
    , has_background(false)
    , background_color(0xFFFFFFFF) {
    UpdateScale();
    if (!extent.IsNull()) {
        Coordinate center = extent.GetCentre();
        center_x = center.x;
        center_y = center.y;
    }
}

void DrawParams::SetExtent(const Envelope& ext) {
    extent = ext;
    UpdateScale();
    if (!extent.IsNull()) {
        Coordinate center = extent.GetCentre();
        center_x = center.x;
        center_y = center.y;
    }
}

void DrawParams::SetSize(int32_t width, int32_t height) {
    pixel_width = width;
    pixel_height = height;
    UpdateScale();
}

void DrawParams::SetDpi(double dpi_val) {
    dpi = dpi_val;
    UpdateScale();
}

void DrawParams::SetRotation(double angle) {
    rotation = angle;
}

void DrawParams::SetCenter(double cx, double cy) {
    center_x = cx;
    center_y = cy;
}

void DrawParams::SetBackgroundColor(uint32_t color) {
    background_color = color;
    has_background = true;
}

void DrawParams::UpdateScale() {
    if (pixel_width <= 0 || pixel_height <= 0 || extent.IsNull()) {
        scale = 1.0;
        return;
    }
    
    double extent_width = extent.GetWidth();
    double extent_height = extent.GetHeight();
    
    if (extent_width <= 0 || extent_height <= 0) {
        scale = 1.0;
        return;
    }
    
    double scale_x = static_cast<double>(pixel_width) / extent_width;
    double scale_y = static_cast<double>(pixel_height) / extent_height;
    
    scale = (scale_x < scale_y) ? scale_x : scale_y;
    
    logical_width = extent_width * scale;
    logical_height = extent_height * scale;
}

void DrawParams::UpdateTransform() {
    UpdateScale();
}

double DrawParams::GetPixelSizeX() const {
    if (pixel_width <= 0 || extent.IsNull()) {
        return 1.0;
    }
    return extent.GetWidth() / static_cast<double>(pixel_width);
}

double DrawParams::GetPixelSizeY() const {
    if (pixel_height <= 0 || extent.IsNull()) {
        return 1.0;
    }
    return extent.GetHeight() / static_cast<double>(pixel_height);
}

bool DrawParams::IsValid() const {
    return pixel_width > 0 && pixel_height > 0 && dpi > 0 && scale > 0;
}

bool DrawParams::IsEmpty() const {
    return extent.IsNull() && pixel_width == 0 && pixel_height == 0;
}

void DrawParams::Reset() {
    extent.SetNull();
    logical_width = 0.0;
    logical_height = 0.0;
    dpi = 96.0;
    scale = 1.0;
    rotation = 0.0;
    center_x = 0.0;
    center_y = 0.0;
    pixel_width = 0;
    pixel_height = 0;
    tolerance = 0.0;
    has_background = false;
    background_color = 0xFFFFFFFF;
}

}  
}  
