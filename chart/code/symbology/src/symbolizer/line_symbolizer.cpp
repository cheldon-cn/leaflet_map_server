#include "ogc/symbology/symbolizer/line_symbolizer.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/multilinestring.h"
#include <cmath>

namespace ogc {
namespace symbology {

struct LineSymbolizer::Impl {
    double width = 1.0;
    uint32_t color = 0xFF000000;
    double opacity = 1.0;
    ogc::draw::LineCap capStyle = ogc::draw::LineCap::kFlat;
    ogc::draw::LineJoin joinStyle = ogc::draw::LineJoin::kMiter;
    DashStyle dashStyle = DashStyle::kSolid;
    std::vector<double> dashPattern;
    double dashOffset = 0.0;
    double offset = 0.0;
    double perpendicularOffset = 0.0;
    bool graphicStroke = false;
    double graphicStrokeSize = 0.0;
    double graphicStrokeSpacing = 0.0;
};

LineSymbolizer::LineSymbolizer() : impl_(std::make_unique<Impl>()) {
}

LineSymbolizer::LineSymbolizer(double width, uint32_t color) : impl_(std::make_unique<Impl>()) {
    impl_->width = width;
    impl_->color = color;
}

LineSymbolizer::~LineSymbolizer() = default;

std::string LineSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "LineSymbolizer" : name;
}

ogc::draw::DrawResult LineSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, GetDefaultStyle());
}

ogc::draw::DrawResult LineSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
    if (!context || !geometry) {
        return ogc::draw::DrawResult::kInvalidParameter;
    }
    
    if (!IsEnabled()) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    double scale = context->GetTransform().GetScaleX();
    if (!IsVisibleAtScale(scale)) {
        return ogc::draw::DrawResult::kSuccess;
    }
    
    ogc::draw::DrawStyle finalStyle = MergeStyle(GetDefaultStyle(), style);
    if (finalStyle.pen.width == 0) {
        finalStyle.pen = ogc::draw::Pen(ogc::draw::Color(impl_->color), impl_->width);
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
    impl_->width = width;
}

double LineSymbolizer::GetWidth() const {
    return impl_->width;
}

void LineSymbolizer::SetColor(uint32_t color) {
    impl_->color = color;
}

uint32_t LineSymbolizer::GetColor() const {
    return impl_->color;
}

void LineSymbolizer::SetOpacity(double opacity) {
    impl_->opacity = opacity;
}

double LineSymbolizer::GetOpacity() const {
    return impl_->opacity;
}

void LineSymbolizer::SetCapStyle(ogc::draw::LineCap style) {
    impl_->capStyle = style;
}

ogc::draw::LineCap LineSymbolizer::GetCapStyle() const {
    return impl_->capStyle;
}

void LineSymbolizer::SetJoinStyle(ogc::draw::LineJoin style) {
    impl_->joinStyle = style;
}

ogc::draw::LineJoin LineSymbolizer::GetJoinStyle() const {
    return impl_->joinStyle;
}

void LineSymbolizer::SetDashStyle(DashStyle style) {
    impl_->dashStyle = style;
    if (style != DashStyle::kCustom) {
        impl_->dashPattern = GetDefaultDashPattern(style);
    }
}

DashStyle LineSymbolizer::GetDashStyle() const {
    return impl_->dashStyle;
}

void LineSymbolizer::SetDashPattern(const std::vector<double>& pattern) {
    impl_->dashPattern = pattern;
    impl_->dashStyle = DashStyle::kCustom;
}

std::vector<double> LineSymbolizer::GetDashPattern() const {
    return impl_->dashPattern;
}

void LineSymbolizer::SetDashOffset(double offset) {
    impl_->dashOffset = offset;
}

double LineSymbolizer::GetDashOffset() const {
    return impl_->dashOffset;
}

void LineSymbolizer::SetOffset(double offset) {
    impl_->offset = offset;
}

double LineSymbolizer::GetOffset() const {
    return impl_->offset;
}

void LineSymbolizer::SetPerpendicularOffset(double offset) {
    impl_->perpendicularOffset = offset;
}

double LineSymbolizer::GetPerpendicularOffset() const {
    return impl_->perpendicularOffset;
}

void LineSymbolizer::SetGraphicStroke(bool enabled) {
    impl_->graphicStroke = enabled;
}

bool LineSymbolizer::HasGraphicStroke() const {
    return impl_->graphicStroke;
}

void LineSymbolizer::SetGraphicStrokeSize(double size) {
    impl_->graphicStrokeSize = size;
}

double LineSymbolizer::GetGraphicStrokeSize() const {
    return impl_->graphicStrokeSize;
}

void LineSymbolizer::SetGraphicStrokeSpacing(double spacing) {
    impl_->graphicStrokeSpacing = spacing;
}

double LineSymbolizer::GetGraphicStrokeSpacing() const {
    return impl_->graphicStrokeSpacing;
}

SymbolizerPtr LineSymbolizer::Clone() const {
    auto sym = std::make_shared<LineSymbolizer>();
    sym->impl_->width = impl_->width;
    sym->impl_->color = impl_->color;
    sym->impl_->opacity = impl_->opacity;
    sym->impl_->capStyle = impl_->capStyle;
    sym->impl_->joinStyle = impl_->joinStyle;
    sym->impl_->dashStyle = impl_->dashStyle;
    sym->impl_->dashPattern = impl_->dashPattern;
    sym->impl_->dashOffset = impl_->dashOffset;
    sym->impl_->offset = impl_->offset;
    sym->impl_->perpendicularOffset = impl_->perpendicularOffset;
    sym->impl_->graphicStroke = impl_->graphicStroke;
    sym->impl_->graphicStrokeSize = impl_->graphicStrokeSize;
    sym->impl_->graphicStrokeSpacing = impl_->graphicStrokeSpacing;
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    sym->SetOpacity(GetOpacity());
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
