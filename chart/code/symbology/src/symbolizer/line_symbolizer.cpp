#include "ogc/symbology/symbolizer/line_symbolizer.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/multilinestring.h"
#include <cmath>

namespace ogc {
namespace symbology {

LineSymbolizer::LineSymbolizer()
    : m_width(1.0)
    , m_color(0xFF000000)
    , m_opacity(1.0)
    , m_capStyle(ogc::draw::LineCap::kFlat)
    , m_joinStyle(ogc::draw::LineJoin::kMiter)
    , m_dashStyle(DashStyle::kSolid)
    , m_dashOffset(0.0)
    , m_offset(0.0)
    , m_perpendicularOffset(0.0)
    , m_graphicStroke(false)
    , m_graphicStrokeSize(0.0)
    , m_graphicStrokeSpacing(0.0) {
}

LineSymbolizer::LineSymbolizer(double width, uint32_t color)
    : m_width(width)
    , m_color(color)
    , m_opacity(1.0)
    , m_capStyle(ogc::draw::LineCap::kFlat)
    , m_joinStyle(ogc::draw::LineJoin::kMiter)
    , m_dashStyle(DashStyle::kSolid)
    , m_dashOffset(0.0)
    , m_offset(0.0)
    , m_perpendicularOffset(0.0)
    , m_graphicStroke(false)
    , m_graphicStrokeSize(0.0)
    , m_graphicStrokeSpacing(0.0) {
}

ogc::draw::DrawResult LineSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

ogc::draw::DrawResult LineSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
    if (!context || !geometry) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    if (!m_enabled) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    double scale = context->GetTransform().GetScaleX();
    if (!IsVisibleAtScale(scale)) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    ogc::draw::DrawStyle finalStyle = MergeStyle(m_defaultStyle, style);
    if (finalStyle.pen.width == 0) {
        finalStyle.pen = ogc::draw::Pen(ogc::draw::Color(m_color), m_width);
    }
    
    GeomType geomType = geometry->GetGeometryType();
    
    if (geomType == GeomType::kLineString) {
        const ogc::LineString* lineString = dynamic_cast<const ogc::LineString*>(geometry);
        if (lineString) {
            return DrawLineString(context, lineString, finalStyle);
        }
    } else if (geomType == GeomType::kMultiLineString) {
        const ogc::MultiLineString* multiLineString = dynamic_cast<const ogc::MultiLineString*>(geometry);
        if (multiLineString) {
            return DrawMultiLineString(context, multiLineString, finalStyle);
        }
    }
    
    return ogc::draw::DrawResult::kInvalidParameter;
}

bool LineSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kLineString || 
           geomType == GeomType::kMultiLineString;
}

void LineSymbolizer::SetWidth(double width) {
    m_width = width;
}

double LineSymbolizer::GetWidth() const {
    return m_width;
}

void LineSymbolizer::SetColor(uint32_t color) {
    m_color = color;
}

uint32_t LineSymbolizer::GetColor() const {
    return m_color;
}

void LineSymbolizer::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double LineSymbolizer::GetOpacity() const {
    return m_opacity;
}

void LineSymbolizer::SetCapStyle(ogc::draw::LineCap style) {
    m_capStyle = style;
}

ogc::draw::LineCap LineSymbolizer::GetCapStyle() const {
    return m_capStyle;
}

void LineSymbolizer::SetJoinStyle(ogc::draw::LineJoin style) {
    m_joinStyle = style;
}

ogc::draw::LineJoin LineSymbolizer::GetJoinStyle() const {
    return m_joinStyle;
}

void LineSymbolizer::SetDashStyle(DashStyle style) {
    m_dashStyle = style;
    if (style != DashStyle::kCustom) {
        m_dashPattern = GetDefaultDashPattern(style);
    }
}

DashStyle LineSymbolizer::GetDashStyle() const {
    return m_dashStyle;
}

void LineSymbolizer::SetDashPattern(const std::vector<double>& pattern) {
    m_dashPattern = pattern;
    m_dashStyle = DashStyle::kCustom;
}

std::vector<double> LineSymbolizer::GetDashPattern() const {
    return m_dashPattern;
}

void LineSymbolizer::SetDashOffset(double offset) {
    m_dashOffset = offset;
}

double LineSymbolizer::GetDashOffset() const {
    return m_dashOffset;
}

void LineSymbolizer::SetOffset(double offset) {
    m_offset = offset;
}

double LineSymbolizer::GetOffset() const {
    return m_offset;
}

void LineSymbolizer::SetPerpendicularOffset(double offset) {
    m_perpendicularOffset = offset;
}

double LineSymbolizer::GetPerpendicularOffset() const {
    return m_perpendicularOffset;
}

void LineSymbolizer::SetGraphicStroke(bool enabled) {
    m_graphicStroke = enabled;
}

bool LineSymbolizer::HasGraphicStroke() const {
    return m_graphicStroke;
}

void LineSymbolizer::SetGraphicStrokeSize(double size) {
    m_graphicStrokeSize = size;
}

double LineSymbolizer::GetGraphicStrokeSize() const {
    return m_graphicStrokeSize;
}

void LineSymbolizer::SetGraphicStrokeSpacing(double spacing) {
    m_graphicStrokeSpacing = spacing;
}

double LineSymbolizer::GetGraphicStrokeSpacing() const {
    return m_graphicStrokeSpacing;
}

SymbolizerPtr LineSymbolizer::Clone() const {
    auto sym = std::make_shared<LineSymbolizer>();
    sym->m_width = m_width;
    sym->m_color = m_color;
    sym->m_opacity = m_opacity;
    sym->m_capStyle = m_capStyle;
    sym->m_joinStyle = m_joinStyle;
    sym->m_dashStyle = m_dashStyle;
    sym->m_dashPattern = m_dashPattern;
    sym->m_dashOffset = m_dashOffset;
    sym->m_offset = m_offset;
    sym->m_perpendicularOffset = m_perpendicularOffset;
    sym->m_graphicStroke = m_graphicStroke;
    sym->m_graphicStrokeSize = m_graphicStrokeSize;
    sym->m_graphicStrokeSpacing = m_graphicStrokeSpacing;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    sym->m_opacity = m_opacity;
    return sym;
}

LineSymbolizerPtr LineSymbolizer::Create() {
    return std::make_shared<LineSymbolizer>();
}

LineSymbolizerPtr LineSymbolizer::Create(double width, uint32_t color) {
    return std::make_shared<LineSymbolizer>(width, color);
}

ogc::draw::DrawResult LineSymbolizer::DrawLineString(ogc::draw::DrawContextPtr context, const ogc::LineString* lineString, const ogc::draw::DrawStyle& style) {
    if (!lineString || lineString->GetNumPoints() < 2) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    size_t numPoints = lineString->GetNumPoints();
    std::vector<double> x(numPoints);
    std::vector<double> y(numPoints);
    
    for (size_t i = 0; i < numPoints; ++i) {
        ogc::Coordinate coord = lineString->GetPointN(i);
        x[i] = coord.x;
        y[i] = coord.y;
    }
    
    context->Save();
    context->SetStyle(style);
    ogc::draw::DrawResult result = context->DrawLineString(x.data(), y.data(), static_cast<int>(numPoints));
    context->Restore();
    return result;
}

ogc::draw::DrawResult LineSymbolizer::DrawMultiLineString(ogc::draw::DrawContextPtr context, const ogc::MultiLineString* multiLineString, const ogc::draw::DrawStyle& style) {
    if (!multiLineString) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    size_t numLines = multiLineString->GetNumLineStrings();
    
    for (size_t i = 0; i < numLines; ++i) {
        const ogc::LineString* lineString = multiLineString->GetLineStringN(i);
        if (lineString) {
            ogc::draw::DrawResult r = DrawLineString(context, lineString, style);
            if (r != ogc::draw::DrawResult::kSuccess) {
                result = r;
            }
        }
    }
    
    return result;
}

std::vector<double> LineSymbolizer::GetDefaultDashPattern(DashStyle style) const {
    switch (style) {
        case DashStyle::kSolid:
            return {};
        case DashStyle::kDash:
            return {10.0, 5.0};
        case DashStyle::kDot:
            return {2.0, 5.0};
        case DashStyle::kDashDot:
            return {10.0, 5.0, 2.0, 5.0};
        case DashStyle::kDashDotDot:
            return {10.0, 5.0, 2.0, 5.0, 2.0, 5.0};
        default:
            return {};
    }
}

}
}
