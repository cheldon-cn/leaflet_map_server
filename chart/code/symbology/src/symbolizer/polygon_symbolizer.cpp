#include "ogc/symbology/symbolizer/polygon_symbolizer.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipolygon.h"
#include <cmath>

namespace ogc {
namespace symbology {

PolygonSymbolizer::PolygonSymbolizer()
    : m_fillColor(0xFFCCCCCC)
    , m_fillOpacity(1.0)
    , m_strokeColor(0xFF000000)
    , m_strokeWidth(1.0)
    , m_strokeOpacity(1.0)
    , m_fillPattern(FillPattern::kSolid)
    , m_displacementX(0.0)
    , m_displacementY(0.0)
    , m_perpendicularOffset(0.0)
    , m_graphicFill(false)
    , m_graphicFillSize(0.0)
    , m_graphicFillSpacing(0.0) {
}

PolygonSymbolizer::PolygonSymbolizer(uint32_t fillColor)
    : m_fillColor(fillColor)
    , m_fillOpacity(1.0)
    , m_strokeColor(0xFF000000)
    , m_strokeWidth(1.0)
    , m_strokeOpacity(1.0)
    , m_fillPattern(FillPattern::kSolid)
    , m_displacementX(0.0)
    , m_displacementY(0.0)
    , m_perpendicularOffset(0.0)
    , m_graphicFill(false)
    , m_graphicFillSize(0.0)
    , m_graphicFillSpacing(0.0) {
}

PolygonSymbolizer::PolygonSymbolizer(uint32_t fillColor, uint32_t strokeColor, double strokeWidth)
    : m_fillColor(fillColor)
    , m_fillOpacity(1.0)
    , m_strokeColor(strokeColor)
    , m_strokeWidth(strokeWidth)
    , m_strokeOpacity(1.0)
    , m_fillPattern(FillPattern::kSolid)
    , m_displacementX(0.0)
    , m_displacementY(0.0)
    , m_perpendicularOffset(0.0)
    , m_graphicFill(false)
    , m_graphicFillSize(0.0)
    , m_graphicFillSpacing(0.0) {
}

ogc::draw::DrawResult PolygonSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

ogc::draw::DrawResult PolygonSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
    if (finalStyle.brush.color.GetAlpha() == 0) {
        finalStyle.brush = ogc::draw::Brush(ogc::draw::Color(m_fillColor));
    }
    if (finalStyle.pen.width == 0) {
        finalStyle.pen = ogc::draw::Pen(ogc::draw::Color(m_strokeColor), m_strokeWidth);
    }
    
    GeomType geomType = geometry->GetGeometryType();
    
    if (geomType == GeomType::kPolygon) {
        const ogc::Polygon* polygon = dynamic_cast<const ogc::Polygon*>(geometry);
        if (polygon) {
            return DrawPolygon(context, polygon, finalStyle);
        }
    } else if (geomType == GeomType::kMultiPolygon) {
        const ogc::MultiPolygon* multiPolygon = dynamic_cast<const ogc::MultiPolygon*>(geometry);
        if (multiPolygon) {
            return DrawMultiPolygon(context, multiPolygon, finalStyle);
        }
    }
    
    return ogc::draw::DrawResult::kInvalidParameter;
}

bool PolygonSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kPolygon || 
           geomType == GeomType::kMultiPolygon;
}

void PolygonSymbolizer::SetFillColor(uint32_t color) {
    m_fillColor = color;
}

uint32_t PolygonSymbolizer::GetFillColor() const {
    return m_fillColor;
}

void PolygonSymbolizer::SetFillOpacity(double opacity) {
    m_fillOpacity = opacity;
}

double PolygonSymbolizer::GetFillOpacity() const {
    return m_fillOpacity;
}

void PolygonSymbolizer::SetStrokeColor(uint32_t color) {
    m_strokeColor = color;
}

uint32_t PolygonSymbolizer::GetStrokeColor() const {
    return m_strokeColor;
}

void PolygonSymbolizer::SetStrokeWidth(double width) {
    m_strokeWidth = width;
}

double PolygonSymbolizer::GetStrokeWidth() const {
    return m_strokeWidth;
}

void PolygonSymbolizer::SetStrokeOpacity(double opacity) {
    m_strokeOpacity = opacity;
}

double PolygonSymbolizer::GetStrokeOpacity() const {
    return m_strokeOpacity;
}

void PolygonSymbolizer::SetFillPattern(FillPattern pattern) {
    m_fillPattern = pattern;
}

FillPattern PolygonSymbolizer::GetFillPattern() const {
    return m_fillPattern;
}

void PolygonSymbolizer::SetDisplacement(double dx, double dy) {
    m_displacementX = dx;
    m_displacementY = dy;
}

void PolygonSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = m_displacementX;
    dy = m_displacementY;
}

void PolygonSymbolizer::SetPerpendicularOffset(double offset) {
    m_perpendicularOffset = offset;
}

double PolygonSymbolizer::GetPerpendicularOffset() const {
    return m_perpendicularOffset;
}

void PolygonSymbolizer::SetGraphicFill(bool enabled) {
    m_graphicFill = enabled;
}

bool PolygonSymbolizer::HasGraphicFill() const {
    return m_graphicFill;
}

void PolygonSymbolizer::SetGraphicFillSize(double size) {
    m_graphicFillSize = size;
}

double PolygonSymbolizer::GetGraphicFillSize() const {
    return m_graphicFillSize;
}

void PolygonSymbolizer::SetGraphicFillSpacing(double spacing) {
    m_graphicFillSpacing = spacing;
}

double PolygonSymbolizer::GetGraphicFillSpacing() const {
    return m_graphicFillSpacing;
}

SymbolizerPtr PolygonSymbolizer::Clone() const {
    auto sym = std::make_shared<PolygonSymbolizer>();
    sym->m_fillColor = m_fillColor;
    sym->m_fillOpacity = m_fillOpacity;
    sym->m_strokeColor = m_strokeColor;
    sym->m_strokeWidth = m_strokeWidth;
    sym->m_strokeOpacity = m_strokeOpacity;
    sym->m_fillPattern = m_fillPattern;
    sym->m_displacementX = m_displacementX;
    sym->m_displacementY = m_displacementY;
    sym->m_perpendicularOffset = m_perpendicularOffset;
    sym->m_graphicFill = m_graphicFill;
    sym->m_graphicFillSize = m_graphicFillSize;
    sym->m_graphicFillSpacing = m_graphicFillSpacing;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    sym->m_opacity = m_opacity;
    return sym;
}

PolygonSymbolizerPtr PolygonSymbolizer::Create() {
    return std::make_shared<PolygonSymbolizer>();
}

PolygonSymbolizerPtr PolygonSymbolizer::Create(uint32_t fillColor) {
    return std::make_shared<PolygonSymbolizer>(fillColor);
}

PolygonSymbolizerPtr PolygonSymbolizer::Create(uint32_t fillColor, uint32_t strokeColor, double strokeWidth) {
    return std::make_shared<PolygonSymbolizer>(fillColor, strokeColor, strokeWidth);
}

ogc::draw::DrawResult PolygonSymbolizer::DrawPolygon(ogc::draw::DrawContextPtr context, const ogc::Polygon* polygon, const ogc::draw::DrawStyle& style) {
    if (!polygon) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    const ogc::LinearRing* exteriorRing = polygon->GetExteriorRing();
    if (!exteriorRing || exteriorRing->GetNumPoints() < 3) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    size_t numPoints = exteriorRing->GetNumPoints();
    std::vector<double> x(numPoints);
    std::vector<double> y(numPoints);
    
    for (size_t i = 0; i < numPoints; ++i) {
        ogc::Coordinate coord = exteriorRing->GetPointN(i);
        x[i] = coord.x + m_displacementX;
        y[i] = coord.y + m_displacementY;
    }
    
    context->Save();
    context->SetStyle(style);
    ogc::draw::DrawResult result = context->DrawPolygon(x.data(), y.data(), static_cast<int>(numPoints));
    context->Restore();
    return result;
}

ogc::draw::DrawResult PolygonSymbolizer::DrawMultiPolygon(ogc::draw::DrawContextPtr context, const ogc::MultiPolygon* multiPolygon, const ogc::draw::DrawStyle& style) {
    if (!multiPolygon) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    size_t numPolygons = multiPolygon->GetNumPolygons();
    
    for (size_t i = 0; i < numPolygons; ++i) {
        const ogc::Polygon* polygon = multiPolygon->GetPolygonN(i);
        if (polygon) {
            ogc::draw::DrawResult r = DrawPolygon(context, polygon, style);
            if (r != ogc::draw::DrawResult::kSuccess) {
                result = r;
            }
        }
    }
    
    return result;
}

}
}
