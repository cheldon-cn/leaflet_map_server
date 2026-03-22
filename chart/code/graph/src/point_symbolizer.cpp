#include "ogc/draw/point_symbolizer.h"
#include "ogc/point.h"
#include "ogc/multipoint.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace draw {

PointSymbolizer::PointSymbolizer()
    : m_symbolType(PointSymbolType::kCircle)
    , m_size(6.0)
    , m_color(0xFF0000FF)
    , m_strokeColor(0xFF000000)
    , m_strokeWidth(1.0)
    , m_rotation(0.0)
    , m_anchorX(0.5)
    , m_anchorY(0.5)
    , m_displacementX(0.0)
    , m_displacementY(0.0) {
}

PointSymbolizer::PointSymbolizer(double size, uint32_t color)
    : m_symbolType(PointSymbolType::kCircle)
    , m_size(size)
    , m_color(color)
    , m_strokeColor(0xFF000000)
    , m_strokeWidth(1.0)
    , m_rotation(0.0)
    , m_anchorX(0.5)
    , m_anchorY(0.5)
    , m_displacementX(0.0)
    , m_displacementY(0.0) {
}

DrawResult PointSymbolizer::Symbolize(DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

DrawResult PointSymbolizer::Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) {
    if (!context || !geometry) {
        return DrawResult::kInvalidParams;
    }
    
    if (!m_enabled) {
        return DrawResult::kSuccess;
    }
    
    double scale = context->GetScale();
    if (!IsVisibleAtScale(scale)) {
        return DrawResult::kSuccess;
    }
    
    DrawStyle finalStyle = MergeStyle(m_defaultStyle, style);
    if (!finalStyle.fill.visible) {
        finalStyle.fill.color = m_color;
        finalStyle.fill.visible = true;
    }
    if (!finalStyle.stroke.visible) {
        finalStyle.stroke.color = m_strokeColor;
        finalStyle.stroke.width = m_strokeWidth;
        finalStyle.stroke.visible = true;
    }
    
    GeomType geomType = geometry->GetGeometryType();
    
    if (geomType == GeomType::kPoint) {
        const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
        if (point) {
            double x = point->GetX() + m_displacementX;
            double y = point->GetY() + m_displacementY;
            return DrawPoint(context, x, y, finalStyle);
        }
    } else if (geomType == GeomType::kMultiPoint) {
        const ogc::MultiPoint* multiPoint = dynamic_cast<const ogc::MultiPoint*>(geometry);
        if (multiPoint) {
            DrawResult result = DrawResult::kSuccess;
            for (size_t i = 0; i < multiPoint->GetNumPoints(); ++i) {
                const ogc::Point* pt = multiPoint->GetPointN(i);
                if (pt) {
                    double x = pt->GetX() + m_displacementX;
                    double y = pt->GetY() + m_displacementY;
                    DrawResult r = DrawPoint(context, x, y, finalStyle);
                    if (r != DrawResult::kSuccess) {
                        result = r;
                    }
                }
            }
            return result;
        }
    }
    
    return DrawResult::kInvalidParams;
}

bool PointSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kPoint || 
           geomType == GeomType::kMultiPoint;
}

void PointSymbolizer::SetSymbolType(PointSymbolType type) {
    m_symbolType = type;
}

PointSymbolType PointSymbolizer::GetSymbolType() const {
    return m_symbolType;
}

void PointSymbolizer::SetSize(double size) {
    m_size = size;
}

double PointSymbolizer::GetSize() const {
    return m_size;
}

void PointSymbolizer::SetColor(uint32_t color) {
    m_color = color;
}

uint32_t PointSymbolizer::GetColor() const {
    return m_color;
}

void PointSymbolizer::SetStrokeColor(uint32_t color) {
    m_strokeColor = color;
}

uint32_t PointSymbolizer::GetStrokeColor() const {
    return m_strokeColor;
}

void PointSymbolizer::SetStrokeWidth(double width) {
    m_strokeWidth = width;
}

double PointSymbolizer::GetStrokeWidth() const {
    return m_strokeWidth;
}

void PointSymbolizer::SetRotation(double angle) {
    m_rotation = angle;
}

double PointSymbolizer::GetRotation() const {
    return m_rotation;
}

void PointSymbolizer::SetAnchorPoint(double x, double y) {
    m_anchorX = x;
    m_anchorY = y;
}

void PointSymbolizer::GetAnchorPoint(double& x, double& y) const {
    x = m_anchorX;
    y = m_anchorY;
}

void PointSymbolizer::SetDisplacement(double dx, double dy) {
    m_displacementX = dx;
    m_displacementY = dy;
}

void PointSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = m_displacementX;
    dy = m_displacementY;
}

SymbolizerPtr PointSymbolizer::Clone() const {
    auto sym = std::make_shared<PointSymbolizer>();
    sym->m_symbolType = m_symbolType;
    sym->m_size = m_size;
    sym->m_color = m_color;
    sym->m_strokeColor = m_strokeColor;
    sym->m_strokeWidth = m_strokeWidth;
    sym->m_rotation = m_rotation;
    sym->m_anchorX = m_anchorX;
    sym->m_anchorY = m_anchorY;
    sym->m_displacementX = m_displacementX;
    sym->m_displacementY = m_displacementY;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    sym->m_opacity = m_opacity;
    return sym;
}

PointSymbolizerPtr PointSymbolizer::Create() {
    return std::make_shared<PointSymbolizer>();
}

PointSymbolizerPtr PointSymbolizer::Create(double size, uint32_t color) {
    return std::make_shared<PointSymbolizer>(size, color);
}

DrawResult PointSymbolizer::DrawPoint(DrawContextPtr context, double x, double y, const DrawStyle& style) {
    return DrawSymbol(context, x, y, m_size, m_symbolType, style);
}

DrawResult PointSymbolizer::DrawSymbol(DrawContextPtr context, double x, double y, double size, PointSymbolType type, const DrawStyle& style) {
    double halfSize = size / 2.0;
    
    if (m_rotation != 0.0) {
        context->PushTransform();
        context->Translate(x, y);
        context->Rotate(m_rotation);
        context->Translate(-x, -y);
    }
    
    context->PushStyle(style);
    DrawResult result = DrawResult::kSuccess;
    
    switch (type) {
        case PointSymbolType::kCircle:
            result = context->DrawCircle(x, y, halfSize);
            break;
            
        case PointSymbolType::kSquare:
            result = context->DrawRect(x - halfSize, y - halfSize, size, size);
            break;
            
        case PointSymbolType::kTriangle: {
            double h = size * std::sqrt(3.0) / 2.0;
            double px[] = {x, x - halfSize, x + halfSize};
            double py[] = {y - h / 2.0, y + h / 2.0, y + h / 2.0};
            result = context->DrawPolygon(px, py, 3);
            break;
        }
        
        case PointSymbolType::kStar: {
            double outerR = halfSize;
            double innerR = halfSize * 0.4;
            double px[10], py[10];
            for (int i = 0; i < 10; ++i) {
                double angle = (i * 36.0 - 90.0) * M_PI / 180.0;
                double r = (i % 2 == 0) ? outerR : innerR;
                px[i] = x + r * std::cos(angle);
                py[i] = y + r * std::sin(angle);
            }
            result = context->DrawPolygon(px, py, 10);
            break;
        }
        
        case PointSymbolType::kCross: {
            double t = size * 0.2;
            double px[] = {x - t, x + t, x + t, x + halfSize, x + halfSize, x + t, 
                          x + t, x - t, x - t, x - halfSize, x - halfSize, x - t};
            double py[] = {y - halfSize, y - halfSize, y - t, y - t, 
                          y + t, y + t, y + halfSize, y + halfSize, 
                          y + t, y + t, y - t, y - t};
            result = context->DrawPolygon(px, py, 12);
            break;
        }
        
        case PointSymbolType::kDiamond: {
            double px[] = {x, x + halfSize, x, x - halfSize};
            double py[] = {y - halfSize, y, y + halfSize, y};
            result = context->DrawPolygon(px, py, 4);
            break;
        }
    }
    
    context->PopStyle();
    
    if (m_rotation != 0.0) {
        context->PopTransform();
    }
    
    return result;
}

}  
}  
