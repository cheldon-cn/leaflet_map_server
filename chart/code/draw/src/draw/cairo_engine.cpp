#ifdef OGC_HAS_CAIRO

#include "ogc/draw/cairo_engine.h"
#include "ogc/draw/raster_image_device.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

CairoEngine::CairoEngine(DrawDevice* device) 
    : Simple2DEngine(device) {
}

CairoEngine::~CairoEngine() {
    if (m_cr) {
        cairo_destroy(m_cr);
        m_cr = nullptr;
    }
    if (m_surface) {
        cairo_surface_destroy(m_surface);
        m_surface = nullptr;
    }
}

bool CairoEngine::Begin() {
    if (!m_device) {
        return false;
    }
    
    int width = m_device->GetWidth();
    int height = m_device->GetHeight();
    
    if (width <= 0 || height <= 0) {
        return false;
    }
    
    m_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    if (cairo_surface_status(m_surface) != CAIRO_STATUS_SUCCESS) {
        cairo_surface_destroy(m_surface);
        m_surface = nullptr;
        return false;
    }
    
    m_cr = cairo_create(m_surface);
    if (cairo_status(m_cr) != CAIRO_STATUS_SUCCESS) {
        cairo_destroy(m_cr);
        cairo_surface_destroy(m_surface);
        m_cr = nullptr;
        m_surface = nullptr;
        return false;
    }
    
    cairo_set_antialias(m_cr, CAIRO_ANTIALIAS_BEST);
    
    return true;
}

void CairoEngine::End() {
    if (m_cr) {
        cairo_destroy(m_cr);
        m_cr = nullptr;
    }
    if (m_surface) {
        cairo_surface_destroy(m_surface);
        m_surface = nullptr;
    }
}

void CairoEngine::SetSourceColor(const Color& color) {
    cairo_set_source_rgba(m_cr, 
                          color.GetRed() / 255.0, 
                          color.GetGreen() / 255.0, 
                          color.GetBlue() / 255.0, 
                          color.GetAlpha() / 255.0);
}

void CairoEngine::ApplyPen(const Pen& pen) {
    SetSourceColor(pen.color);
    cairo_set_line_width(m_cr, pen.width);
    
    switch (pen.style) {
        case PenStyle::Solid:
            cairo_set_dash(m_cr, nullptr, 0, 0);
            break;
        case PenStyle::Dash:
            {
                double dashes[] = {4.0, 2.0};
                cairo_set_dash(m_cr, dashes, 2, 0);
            }
            break;
        case PenStyle::Dot:
            {
                double dashes[] = {1.0, 1.0};
                cairo_set_dash(m_cr, dashes, 2, 0);
            }
            break;
        case PenStyle::DashDot:
            {
                double dashes[] = {4.0, 2.0, 1.0, 2.0};
                cairo_set_dash(m_cr, dashes, 4, 0);
            }
            break;
        case PenStyle::DashDotDot:
            {
                double dashes[] = {4.0, 2.0, 1.0, 2.0, 1.0, 2.0};
                cairo_set_dash(m_cr, dashes, 6, 0);
            }
            break;
        case PenStyle::Null:
            cairo_set_line_width(m_cr, 0);
            break;
    }
    
    switch (pen.cap) {
        case PenCap::Flat:
            cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_BUTT);
            break;
        case PenCap::Round:
            cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_ROUND);
            break;
        case PenCap::Square:
            cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_SQUARE);
            break;
    }
    
    switch (pen.join) {
        case PenJoin::Miter:
            cairo_set_line_join(m_cr, CAIRO_LINE_JOIN_MITER);
            break;
        case PenJoin::Round:
            cairo_set_line_join(m_cr, CAIRO_LINE_JOIN_ROUND);
            break;
        case PenJoin::Bevel:
            cairo_set_line_join(m_cr, CAIRO_LINE_JOIN_BEVEL);
            break;
    }
}

void CairoEngine::ApplyBrush(const Brush& brush) {
    SetSourceColor(brush.color);
}

void CairoEngine::ApplyStyle(const DrawStyle& style) {
    ApplyPen(style.pen);
}

DrawResult CairoEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) {
    if (!m_cr || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyPen(style.pen);
    
    double pointSize = style.pen.width > 0 ? style.pen.width : 1.0;
    
    for (int i = 0; i < count; ++i) {
        cairo_arc(m_cr, x[i], y[i], pointSize / 2.0, 0, 2 * M_PI);
        cairo_fill(m_cr);
    }
    
    return DrawResult::Success;
}

DrawResult CairoEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) {
    if (!m_cr || count <= 0) {
        return DrawResult::InvalidParameter;
    }
    
    ApplyPen(style.pen);
    
    for (int i = 0; i < count; ++i) {
        cairo_move_to(m_cr, x1[i], y1[i]);
        cairo_line_to(m_cr, x2[i], y2[i]);
    }
    
    cairo_stroke(m_cr);
    
    return DrawResult::Success;
}

DrawResult CairoEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill) {
    if (!m_cr || count < 3) {
        return DrawResult::InvalidParameter;
    }
    
    cairo_move_to(m_cr, x[0], y[0]);
    for (int i = 1; i < count; ++i) {
        cairo_line_to(m_cr, x[i], y[i]);
    }
    cairo_close_path(m_cr);
    
    if (fill && style.brush.style != BrushStyle::Null) {
        ApplyBrush(style.brush);
        cairo_fill_preserve(m_cr);
    }
    
    if (style.pen.style != PenStyle::Null) {
        ApplyPen(style.pen);
        cairo_stroke(m_cr);
    } else {
        cairo_new_path(m_cr);
    }
    
    return DrawResult::Success;
}

DrawResult CairoEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) {
    if (!m_cr || text.empty()) {
        return DrawResult::InvalidParameter;
    }
    
    SetSourceColor(color);
    
    cairo_select_font_face(m_cr, 
                           font.family.empty() ? "sans-serif" : font.family.c_str(),
                           font.italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
                           font.bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
    
    cairo_set_font_size(m_cr, font.size);
    
    cairo_move_to(m_cr, x, y);
    cairo_show_text(m_cr, text.c_str());
    
    return DrawResult::Success;
}

DrawResult CairoEngine::DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) {
    if (!m_cr || image.IsEmpty()) {
        return DrawResult::InvalidParameter;
    }
    
    int imgWidth = image.GetWidth();
    int imgHeight = image.GetHeight();
    const uint8_t* imgData = image.GetData();
    
    cairo_surface_t* imgSurface = cairo_image_surface_create_for_data(
        const_cast<uint8_t*>(imgData),
        CAIRO_FORMAT_ARGB32,
        imgWidth,
        imgHeight,
        imgWidth * 4
    );
    
    if (cairo_surface_status(imgSurface) != CAIRO_STATUS_SUCCESS) {
        cairo_surface_destroy(imgSurface);
        return DrawResult::InvalidParameter;
    }
    
    cairo_save(m_cr);
    
    cairo_translate(m_cr, x, y);
    cairo_scale(m_cr, scaleX, scaleY);
    
    cairo_set_source_surface(m_cr, imgSurface, 0, 0);
    cairo_paint(m_cr);
    
    cairo_restore(m_cr);
    cairo_surface_destroy(imgSurface);
    
    return DrawResult::Success;
}

void CairoEngine::SetTransform(const TransformMatrix& matrix) {
    if (!m_cr) {
        return;
    }
    
    cairo_matrix_t cairoMatrix;
    cairo_matrix_init(&cairoMatrix, 
                      matrix.m11, matrix.m12,
                      matrix.m21, matrix.m22,
                      matrix.dx, matrix.dy);
    
    cairo_set_matrix(m_cr, &cairoMatrix);
}

void CairoEngine::SetClipRegion(const Region& region) {
    if (!m_cr) {
        return;
    }
    
    cairo_reset_clip(m_cr);
    
    if (region.rects.empty()) {
        return;
    }
    
    for (const auto& rect : region.rects) {
        cairo_rectangle(m_cr, rect.x, rect.y, rect.width, rect.height);
    }
    
    cairo_clip(m_cr);
}

}
}

#endif
