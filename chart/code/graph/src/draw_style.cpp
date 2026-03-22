#include "ogc/draw/draw_style.h"
#include <cstring>

namespace ogc {
namespace draw {

StrokeStyle::StrokeStyle()
    : color(0xFF000000)
    , width(1.0)
    , cap(LineCapStyle::kButt)
    , join(LineJoinStyle::kMiter)
    , miter_limit(4.0)
    , dash_array(nullptr)
    , dash_count(0)
    , dash_offset(0.0)
    , visible(true) {
}

StrokeStyle::StrokeStyle(uint32_t c, double w)
    : color(c)
    , width(w)
    , cap(LineCapStyle::kButt)
    , join(LineJoinStyle::kMiter)
    , miter_limit(4.0)
    , dash_array(nullptr)
    , dash_count(0)
    , dash_offset(0.0)
    , visible(true) {
}

StrokeStyle::~StrokeStyle() {
    ClearDashArray();
}

void StrokeStyle::SetColor(uint32_t c) {
    color = c;
}

void StrokeStyle::SetWidth(double w) {
    width = w;
}

void StrokeStyle::SetDashArray(const double* dashes, int count) {
    ClearDashArray();
    if (dashes && count > 0) {
        dash_array = new double[count];
        dash_count = count;
        for (int i = 0; i < count; ++i) {
            dash_array[i] = dashes[i];
        }
    }
}

void StrokeStyle::ClearDashArray() {
    if (dash_array) {
        delete[] dash_array;
        dash_array = nullptr;
    }
    dash_count = 0;
}

void StrokeStyle::Reset() {
    color = 0xFF000000;
    width = 1.0;
    cap = LineCapStyle::kButt;
    join = LineJoinStyle::kMiter;
    miter_limit = 4.0;
    ClearDashArray();
    dash_offset = 0.0;
    visible = true;
}

FillStyle::FillStyle()
    : color(0xFFFFFFFF)
    , opacity(1.0)
    , visible(true)
    , gradient_colors(nullptr)
    , gradient_stops(nullptr)
    , gradient_count(0)
    , has_gradient(false) {
}

FillStyle::FillStyle(uint32_t c)
    : color(c)
    , opacity(1.0)
    , visible(true)
    , gradient_colors(nullptr)
    , gradient_stops(nullptr)
    , gradient_count(0)
    , has_gradient(false) {
}

FillStyle::~FillStyle() {
    ClearGradient();
}

void FillStyle::SetColor(uint32_t c) {
    color = c;
}

void FillStyle::SetOpacity(double op) {
    opacity = op;
}

void FillStyle::SetGradient(const uint32_t* colors, const double* stops, int count) {
    ClearGradient();
    if (colors && stops && count > 0) {
        gradient_colors = new uint32_t[count];
        gradient_stops = new double[count];
        gradient_count = count;
        for (int i = 0; i < count; ++i) {
            gradient_colors[i] = colors[i];
            gradient_stops[i] = stops[i];
        }
        has_gradient = true;
    }
}

void FillStyle::ClearGradient() {
    if (gradient_colors) {
        delete[] gradient_colors;
        gradient_colors = nullptr;
    }
    if (gradient_stops) {
        delete[] gradient_stops;
        gradient_stops = nullptr;
    }
    gradient_count = 0;
    has_gradient = false;
}

void FillStyle::Reset() {
    color = 0xFFFFFFFF;
    opacity = 1.0;
    visible = true;
    ClearGradient();
}

DrawStyle::DrawStyle()
    : opacity(1.0)
    , name("")
    , z_order(0)
    , enabled(true) {
}

DrawStyle::DrawStyle(const StrokeStyle& s, const FillStyle& f)
    : stroke(s)
    , fill(f)
    , opacity(1.0)
    , name("")
    , z_order(0)
    , enabled(true) {
}

void DrawStyle::SetStroke(const StrokeStyle& s) {
    stroke = s;
}

void DrawStyle::SetFill(const FillStyle& f) {
    fill = f;
}

void DrawStyle::SetOpacity(double op) {
    opacity = op;
}

void DrawStyle::SetZOrder(int order) {
    z_order = order;
}

void DrawStyle::SetName(const std::string& n) {
    name = n;
}

bool DrawStyle::HasStroke() const {
    return stroke.visible && stroke.width > 0;
}

bool DrawStyle::HasFill() const {
    return fill.visible;
}

bool DrawStyle::IsVisible() const {
    return enabled && (HasStroke() || HasFill());
}

void DrawStyle::Reset() {
    stroke.Reset();
    fill.Reset();
    opacity = 1.0;
    name = "";
    z_order = 0;
    enabled = true;
}

}  
}  
