#include "ogc/draw/text_symbolizer.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <cmath>

namespace ogc {
namespace draw {

TextSymbolizer::TextSymbolizer()
    : m_font("Arial", 12.0)
    , m_color(0xFF000000)
    , m_opacity(1.0)
    , m_horizontalAlignment(TextHorizontalAlignment::kCenter)
    , m_verticalAlignment(TextVerticalAlignment::kMiddle)
    , m_placement(TextPlacement::kPoint)
    , m_offsetX(0.0)
    , m_offsetY(0.0)
    , m_rotation(0.0)
    , m_maxAngleDelta(22.5)
    , m_followLine(false)
    , m_maxDisplacement(0.0)
    , m_repeatDistance(0.0)
    , m_haloColor(0x00FFFFFF)
    , m_haloRadius(0.0)
    , m_haloOpacity(0.0)
    , m_backgroundColor(0x00000000) {
}

TextSymbolizer::TextSymbolizer(const std::string& label, const Font& font, uint32_t color)
    : m_label(label)
    , m_font(font)
    , m_color(color)
    , m_opacity(1.0)
    , m_horizontalAlignment(TextHorizontalAlignment::kCenter)
    , m_verticalAlignment(TextVerticalAlignment::kMiddle)
    , m_placement(TextPlacement::kPoint)
    , m_offsetX(0.0)
    , m_offsetY(0.0)
    , m_rotation(0.0)
    , m_maxAngleDelta(22.5)
    , m_followLine(false)
    , m_maxDisplacement(0.0)
    , m_repeatDistance(0.0)
    , m_haloColor(0x00FFFFFF)
    , m_haloRadius(0.0)
    , m_haloOpacity(0.0)
    , m_backgroundColor(0x00000000) {
}

DrawResult TextSymbolizer::Symbolize(DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

DrawResult TextSymbolizer::Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) {
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
    
    std::string text = GetLabelText(geometry);
    if (text.empty()) {
        return DrawResult::kSuccess;
    }
    
    GeomType geomType = geometry->GetGeometryType();
    
    switch (m_placement) {
        case TextPlacement::kPoint:
            if (geomType == GeomType::kPoint) {
                const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
                if (point) {
                    return DrawTextAtPoint(context, point->GetX(), point->GetY(), text);
                }
            } else {
                ogc::Envelope env = geometry->GetEnvelope();
                double centerX = (env.GetMinX() + env.GetMaxX()) / 2.0;
                double centerY = (env.GetMinY() + env.GetMaxY()) / 2.0;
                return DrawTextAtPoint(context, centerX, centerY, text);
            }
            break;
            
        case TextPlacement::kLine:
            if (geomType == GeomType::kLineString) {
                const ogc::LineString* lineString = dynamic_cast<const ogc::LineString*>(geometry);
                if (lineString) {
                    return DrawTextAlongLine(context, lineString, text);
                }
            }
            break;
            
        case TextPlacement::kInterior:
            if (geomType == GeomType::kPolygon) {
                const ogc::Polygon* polygon = dynamic_cast<const ogc::Polygon*>(geometry);
                if (polygon) {
                    return DrawTextInPolygon(context, polygon, text);
                }
            }
            break;
    }
    
    return DrawResult::kInvalidParams;
}

bool TextSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kPoint || 
           geomType == GeomType::kLineString ||
           geomType == GeomType::kPolygon;
}

void TextSymbolizer::SetLabel(const std::string& label) {
    m_label = label;
}

std::string TextSymbolizer::GetLabel() const {
    return m_label;
}

void TextSymbolizer::SetLabelProperty(const std::string& propertyName) {
    m_labelProperty = propertyName;
}

std::string TextSymbolizer::GetLabelProperty() const {
    return m_labelProperty;
}

void TextSymbolizer::SetFont(const Font& font) {
    m_font = font;
}

Font TextSymbolizer::GetFont() const {
    return m_font;
}

void TextSymbolizer::SetColor(uint32_t color) {
    m_color = color;
}

uint32_t TextSymbolizer::GetColor() const {
    return m_color;
}

void TextSymbolizer::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double TextSymbolizer::GetOpacity() const {
    return m_opacity;
}

void TextSymbolizer::SetHorizontalAlignment(TextHorizontalAlignment alignment) {
    m_horizontalAlignment = alignment;
}

TextHorizontalAlignment TextSymbolizer::GetHorizontalAlignment() const {
    return m_horizontalAlignment;
}

void TextSymbolizer::SetVerticalAlignment(TextVerticalAlignment alignment) {
    m_verticalAlignment = alignment;
}

TextVerticalAlignment TextSymbolizer::GetVerticalAlignment() const {
    return m_verticalAlignment;
}

void TextSymbolizer::SetPlacement(TextPlacement placement) {
    m_placement = placement;
}

TextPlacement TextSymbolizer::GetPlacement() const {
    return m_placement;
}

void TextSymbolizer::SetOffset(double dx, double dy) {
    m_offsetX = dx;
    m_offsetY = dy;
}

void TextSymbolizer::GetOffset(double& dx, double& dy) const {
    dx = m_offsetX;
    dy = m_offsetY;
}

void TextSymbolizer::SetRotation(double angle) {
    m_rotation = angle;
}

double TextSymbolizer::GetRotation() const {
    return m_rotation;
}

void TextSymbolizer::SetMaxAngleDelta(double delta) {
    m_maxAngleDelta = delta;
}

double TextSymbolizer::GetMaxAngleDelta() const {
    return m_maxAngleDelta;
}

void TextSymbolizer::SetFollowLine(bool follow) {
    m_followLine = follow;
}

bool TextSymbolizer::GetFollowLine() const {
    return m_followLine;
}

void TextSymbolizer::SetMaxDisplacement(double displacement) {
    m_maxDisplacement = displacement;
}

double TextSymbolizer::GetMaxDisplacement() const {
    return m_maxDisplacement;
}

void TextSymbolizer::SetRepeatDistance(double distance) {
    m_repeatDistance = distance;
}

double TextSymbolizer::GetRepeatDistance() const {
    return m_repeatDistance;
}

void TextSymbolizer::SetHaloColor(uint32_t color) {
    m_haloColor = color;
}

uint32_t TextSymbolizer::GetHaloColor() const {
    return m_haloColor;
}

void TextSymbolizer::SetHaloRadius(double radius) {
    m_haloRadius = radius;
}

double TextSymbolizer::GetHaloRadius() const {
    return m_haloRadius;
}

void TextSymbolizer::SetHaloOpacity(double opacity) {
    m_haloOpacity = opacity;
}

double TextSymbolizer::GetHaloOpacity() const {
    return m_haloOpacity;
}

void TextSymbolizer::SetBackgroundColor(uint32_t color) {
    m_backgroundColor = color;
}

uint32_t TextSymbolizer::GetBackgroundColor() const {
    return m_backgroundColor;
}

SymbolizerPtr TextSymbolizer::Clone() const {
    auto sym = std::make_shared<TextSymbolizer>();
    sym->m_label = m_label;
    sym->m_labelProperty = m_labelProperty;
    sym->m_font = m_font;
    sym->m_color = m_color;
    sym->m_opacity = m_opacity;
    sym->m_horizontalAlignment = m_horizontalAlignment;
    sym->m_verticalAlignment = m_verticalAlignment;
    sym->m_placement = m_placement;
    sym->m_offsetX = m_offsetX;
    sym->m_offsetY = m_offsetY;
    sym->m_rotation = m_rotation;
    sym->m_maxAngleDelta = m_maxAngleDelta;
    sym->m_followLine = m_followLine;
    sym->m_maxDisplacement = m_maxDisplacement;
    sym->m_repeatDistance = m_repeatDistance;
    sym->m_haloColor = m_haloColor;
    sym->m_haloRadius = m_haloRadius;
    sym->m_haloOpacity = m_haloOpacity;
    sym->m_backgroundColor = m_backgroundColor;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    return sym;
}

TextSymbolizerPtr TextSymbolizer::Create() {
    return std::make_shared<TextSymbolizer>();
}

TextSymbolizerPtr TextSymbolizer::Create(const std::string& label, const Font& font, uint32_t color) {
    return std::make_shared<TextSymbolizer>(label, font, color);
}

DrawResult TextSymbolizer::DrawTextAtPoint(DrawContextPtr context, double x, double y, const std::string& text) {
    double drawX = x + m_offsetX;
    double drawY = y + m_offsetY;
    
    if (m_rotation != 0.0) {
        context->PushTransform();
        context->Translate(drawX, drawY);
        context->Rotate(m_rotation);
        context->Translate(-drawX, -drawY);
    }
    
    Color textColor(m_color);
    DrawResult result = context->DrawText(drawX, drawY, text, m_font, textColor);
    
    if (m_rotation != 0.0) {
        context->PopTransform();
    }
    
    return result;
}

DrawResult TextSymbolizer::DrawTextAlongLine(DrawContextPtr context, const ogc::LineString* lineString, const std::string& text) {
    if (!lineString || lineString->GetNumPoints() < 2) {
        return DrawResult::kInvalidParams;
    }
    
    size_t numPoints = lineString->GetNumPoints();
    size_t midIndex = numPoints / 2;
    
    ogc::Coordinate coord1 = lineString->GetPointN(midIndex - 1);
    ogc::Coordinate coord2 = lineString->GetPointN(midIndex);
    
    double x = (coord1.x + coord2.x) / 2.0;
    double y = (coord1.y + coord2.y) / 2.0;
    
    return DrawTextAtPoint(context, x, y, text);
}

DrawResult TextSymbolizer::DrawTextInPolygon(DrawContextPtr context, const ogc::Polygon* polygon, const std::string& text) {
    if (!polygon) {
        return DrawResult::kInvalidParams;
    }
    
    ogc::Envelope env = polygon->GetEnvelope();
    double centerX = (env.GetMinX() + env.GetMaxX()) / 2.0;
    double centerY = (env.GetMinY() + env.GetMaxY()) / 2.0;
    
    return DrawTextAtPoint(context, centerX, centerY, text);
}

std::string TextSymbolizer::GetLabelText(const Geometry* geometry) const {
    (void)geometry;
    return m_label;
}

}  
}  
