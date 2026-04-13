#include "ogc/symbology/symbolizer/text_symbolizer.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/polygon.h"
#include <cmath>

namespace ogc {
namespace symbology {

struct TextSymbolizer::TextImpl {
    std::string label;
    std::string labelProperty;
    ogc::draw::Font font{"Arial", 12.0};
    uint32_t color = 0xFF000000;
    double opacity = 1.0;
    TextHorizontalAlignment horizontalAlignment = TextHorizontalAlignment::kCenter;
    TextVerticalAlignment verticalAlignment = TextVerticalAlignment::kMiddle;
    TextPlacement placement = TextPlacement::kPoint;
    double offsetX = 0.0;
    double offsetY = 0.0;
    double rotation = 0.0;
    double maxAngleDelta = 22.5;
    bool followLine = false;
    double maxDisplacement = 0.0;
    double repeatDistance = 0.0;
    uint32_t haloColor = 0x00FFFFFF;
    double haloRadius = 0.0;
    double haloOpacity = 0.0;
    uint32_t backgroundColor = 0x00000000;
    double perpendicularOffset = 0.0;
    double anchorX = 0.0;
    double anchorY = 0.0;
    double displacementX = 0.0;
    double displacementY = 0.0;
};

TextSymbolizer::TextSymbolizer()
    : textImpl_(std::make_unique<TextImpl>()) {
}

TextSymbolizer::TextSymbolizer(const std::string& label, const ogc::draw::Font& font, uint32_t color)
    : textImpl_(std::make_unique<TextImpl>()) {
    textImpl_->label = label;
    textImpl_->font = font;
    textImpl_->color = color;
}

TextSymbolizer::~TextSymbolizer() = default;

std::string TextSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "TextSymbolizer" : name;
}

ogc::draw::DrawResult TextSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, DefaultStyleRef());
}

ogc::draw::DrawResult TextSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
    if (!context || !geometry) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    if (!EnabledRef()) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    double scale = context->GetTransform().GetScaleX();
    if (!IsVisibleAtScale(scale)) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    std::string text = GetLabelText(geometry);
    if (text.empty()) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    (void)style;
    
    GeomType geomType = geometry->GetGeometryType();
    
    switch (textImpl_->placement) {
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
    
    return ogc::draw::DrawResult::kInvalidParameter;
}

bool TextSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kPoint || 
           geomType == GeomType::kLineString ||
           geomType == GeomType::kPolygon;
}

void TextSymbolizer::SetLabel(const std::string& label) {
    textImpl_->label = label;
}

std::string TextSymbolizer::GetLabel() const {
    return textImpl_->label;
}

void TextSymbolizer::SetLabelProperty(const std::string& propertyName) {
    textImpl_->labelProperty = propertyName;
}

std::string TextSymbolizer::GetLabelProperty() const {
    return textImpl_->labelProperty;
}

void TextSymbolizer::SetFont(const ogc::draw::Font& font) {
    textImpl_->font = font;
}

ogc::draw::Font TextSymbolizer::GetFont() const {
    return textImpl_->font;
}

void TextSymbolizer::SetColor(uint32_t color) {
    textImpl_->color = color;
}

uint32_t TextSymbolizer::GetColor() const {
    return textImpl_->color;
}

void TextSymbolizer::SetOpacity(double opacity) {
    textImpl_->opacity = opacity;
}

double TextSymbolizer::GetOpacity() const {
    return textImpl_->opacity;
}

void TextSymbolizer::SetHorizontalAlignment(TextHorizontalAlignment alignment) {
    textImpl_->horizontalAlignment = alignment;
}

TextHorizontalAlignment TextSymbolizer::GetHorizontalAlignment() const {
    return textImpl_->horizontalAlignment;
}

void TextSymbolizer::SetVerticalAlignment(TextVerticalAlignment alignment) {
    textImpl_->verticalAlignment = alignment;
}

TextVerticalAlignment TextSymbolizer::GetVerticalAlignment() const {
    return textImpl_->verticalAlignment;
}

void TextSymbolizer::SetPlacement(TextPlacement placement) {
    textImpl_->placement = placement;
}

TextPlacement TextSymbolizer::GetPlacement() const {
    return textImpl_->placement;
}

void TextSymbolizer::SetOffset(double dx, double dy) {
    textImpl_->offsetX = dx;
    textImpl_->offsetY = dy;
}

void TextSymbolizer::GetOffset(double& dx, double& dy) const {
    dx = textImpl_->offsetX;
    dy = textImpl_->offsetY;
}

void TextSymbolizer::SetRotation(double angle) {
    textImpl_->rotation = angle;
}

double TextSymbolizer::GetRotation() const {
    return textImpl_->rotation;
}

void TextSymbolizer::SetMaxAngleDelta(double delta) {
    textImpl_->maxAngleDelta = delta;
}

double TextSymbolizer::GetMaxAngleDelta() const {
    return textImpl_->maxAngleDelta;
}

void TextSymbolizer::SetFollowLine(bool follow) {
    textImpl_->followLine = follow;
}

bool TextSymbolizer::GetFollowLine() const {
    return textImpl_->followLine;
}

void TextSymbolizer::SetMaxDisplacement(double displacement) {
    textImpl_->maxDisplacement = displacement;
}

double TextSymbolizer::GetMaxDisplacement() const {
    return textImpl_->maxDisplacement;
}

void TextSymbolizer::SetRepeatDistance(double distance) {
    textImpl_->repeatDistance = distance;
}

double TextSymbolizer::GetRepeatDistance() const {
    return textImpl_->repeatDistance;
}

void TextSymbolizer::SetHaloColor(uint32_t color) {
    textImpl_->haloColor = color;
}

uint32_t TextSymbolizer::GetHaloColor() const {
    return textImpl_->haloColor;
}

void TextSymbolizer::SetHaloRadius(double radius) {
    textImpl_->haloRadius = radius;
}

double TextSymbolizer::GetHaloRadius() const {
    return textImpl_->haloRadius;
}

void TextSymbolizer::SetHaloOpacity(double opacity) {
    textImpl_->haloOpacity = opacity;
}

double TextSymbolizer::GetHaloOpacity() const {
    return textImpl_->haloOpacity;
}

void TextSymbolizer::SetBackgroundColor(uint32_t color) {
    textImpl_->backgroundColor = color;
}

uint32_t TextSymbolizer::GetBackgroundColor() const {
    return textImpl_->backgroundColor;
}

void TextSymbolizer::SetPerpendicularOffset(double offset) {
    textImpl_->perpendicularOffset = offset;
}

double TextSymbolizer::GetPerpendicularOffset() const {
    return textImpl_->perpendicularOffset;
}

void TextSymbolizer::SetAnchorPoint(double x, double y) {
    textImpl_->anchorX = x;
    textImpl_->anchorY = y;
}

void TextSymbolizer::GetAnchorPoint(double& x, double& y) const {
    x = textImpl_->anchorX;
    y = textImpl_->anchorY;
}

void TextSymbolizer::SetDisplacement(double dx, double dy) {
    textImpl_->displacementX = dx;
    textImpl_->displacementY = dy;
}

void TextSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = textImpl_->displacementX;
    dy = textImpl_->displacementY;
}

SymbolizerPtr TextSymbolizer::Clone() const {
    auto sym = std::make_shared<TextSymbolizer>();
    sym->textImpl_->label = textImpl_->label;
    sym->textImpl_->labelProperty = textImpl_->labelProperty;
    sym->textImpl_->font = textImpl_->font;
    sym->textImpl_->color = textImpl_->color;
    sym->textImpl_->opacity = textImpl_->opacity;
    sym->textImpl_->horizontalAlignment = textImpl_->horizontalAlignment;
    sym->textImpl_->verticalAlignment = textImpl_->verticalAlignment;
    sym->textImpl_->placement = textImpl_->placement;
    sym->textImpl_->offsetX = textImpl_->offsetX;
    sym->textImpl_->offsetY = textImpl_->offsetY;
    sym->textImpl_->rotation = textImpl_->rotation;
    sym->textImpl_->maxAngleDelta = textImpl_->maxAngleDelta;
    sym->textImpl_->followLine = textImpl_->followLine;
    sym->textImpl_->maxDisplacement = textImpl_->maxDisplacement;
    sym->textImpl_->repeatDistance = textImpl_->repeatDistance;
    sym->textImpl_->haloColor = textImpl_->haloColor;
    sym->textImpl_->haloRadius = textImpl_->haloRadius;
    sym->textImpl_->haloOpacity = textImpl_->haloOpacity;
    sym->textImpl_->backgroundColor = textImpl_->backgroundColor;
    sym->textImpl_->perpendicularOffset = textImpl_->perpendicularOffset;
    sym->textImpl_->anchorX = textImpl_->anchorX;
    sym->textImpl_->anchorY = textImpl_->anchorY;
    sym->textImpl_->displacementX = textImpl_->displacementX;
    sym->textImpl_->displacementY = textImpl_->displacementY;
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    return sym;
}

TextSymbolizerPtr TextSymbolizer::Create() {
    return std::make_shared<TextSymbolizer>();
}

TextSymbolizerPtr TextSymbolizer::Create(const std::string& label, const ogc::draw::Font& font, uint32_t color) {
    return std::make_shared<TextSymbolizer>(label, font, color);
}

ogc::draw::DrawResult TextSymbolizer::DrawTextAtPoint(ogc::draw::DrawContextPtr context, double x, double y, const std::string& text) {
    double drawX = x + textImpl_->offsetX;
    double drawY = y + textImpl_->offsetY;
    
    if (textImpl_->rotation != 0.0) {
        context->Save();
        context->Translate(drawX, drawY);
        context->Rotate(textImpl_->rotation);
        context->Translate(-drawX, -drawY);
    }
    
    ogc::draw::Color textColor(textImpl_->color);
    ogc::draw::DrawResult result = context->DrawText(drawX, drawY, text, textImpl_->font, textColor);
    
    if (textImpl_->rotation != 0.0) {
        context->Restore();
    }
    
    return result;
}

ogc::draw::DrawResult TextSymbolizer::DrawTextAlongLine(ogc::draw::DrawContextPtr context, const ogc::LineString* lineString, const std::string& text) {
    if (!lineString || lineString->GetNumPoints() < 2) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    size_t numPoints = lineString->GetNumPoints();
    size_t midIndex = numPoints / 2;
    
    ogc::Coordinate coord1 = lineString->GetPointN(midIndex - 1);
    ogc::Coordinate coord2 = lineString->GetPointN(midIndex);
    
    double x = (coord1.x + coord2.x) / 2.0;
    double y = (coord1.y + coord2.y) / 2.0;
    
    return DrawTextAtPoint(context, x, y, text);
}

ogc::draw::DrawResult TextSymbolizer::DrawTextInPolygon(ogc::draw::DrawContextPtr context, const ogc::Polygon* polygon, const std::string& text) {
    if (!polygon) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    ogc::Envelope env = polygon->GetEnvelope();
    double centerX = (env.GetMinX() + env.GetMaxX()) / 2.0;
    double centerY = (env.GetMinY() + env.GetMaxY()) / 2.0;
    
    return DrawTextAtPoint(context, centerX, centerY, text);
}

std::string TextSymbolizer::GetLabelText(const Geometry* geometry) const {
    (void)geometry;
    return textImpl_->label;
}

}
}
