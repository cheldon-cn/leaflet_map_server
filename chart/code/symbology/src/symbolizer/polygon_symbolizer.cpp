#include "ogc/symbology/symbolizer/polygon_symbolizer.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipolygon.h"
#include <cmath>

namespace ogc {
namespace symbology {

struct PolygonSymbolizer::Impl {
    uint32_t fillColor = 0xFFCCCCCC;
    double fillOpacity = 1.0;
    uint32_t strokeColor = 0xFF000000;
    double strokeWidth = 1.0;
    double strokeOpacity = 1.0;
    FillPattern fillPattern = FillPattern::kSolid;
    double displacementX = 0.0;
    double displacementY = 0.0;
    double perpendicularOffset = 0.0;
    bool graphicFill = false;
    double graphicFillSize = 0.0;
    double graphicFillSpacing = 0.0;
};

PolygonSymbolizer::PolygonSymbolizer() : impl_(std::make_unique<Impl>()) {
}

PolygonSymbolizer::PolygonSymbolizer(uint32_t fillColor) : impl_(std::make_unique<Impl>()) {
    impl_->fillColor = fillColor;
}

PolygonSymbolizer::PolygonSymbolizer(uint32_t fillColor, uint32_t strokeColor, double strokeWidth) 
    : impl_(std::make_unique<Impl>()) {
    impl_->fillColor = fillColor;
    impl_->strokeColor = strokeColor;
    impl_->strokeWidth = strokeWidth;
}

PolygonSymbolizer::~PolygonSymbolizer() = default;

std::string PolygonSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "PolygonSymbolizer" : name;
}

ogc::draw::DrawResult PolygonSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, GetDefaultStyle());
}

ogc::draw::DrawResult PolygonSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
    if (finalStyle.brush.color.GetAlpha() == 0) {
        finalStyle.brush = ogc::draw::Brush(ogc::draw::Color(impl_->fillColor));
    }
    if (finalStyle.pen.width == 0) {
        finalStyle.pen = ogc::draw::Pen(ogc::draw::Color(impl_->strokeColor), impl_->strokeWidth);
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
    impl_->fillColor = color;
}

uint32_t PolygonSymbolizer::GetFillColor() const {
    return impl_->fillColor;
}

void PolygonSymbolizer::SetFillOpacity(double opacity) {
    impl_->fillOpacity = opacity;
}

double PolygonSymbolizer::GetFillOpacity() const {
    return impl_->fillOpacity;
}

void PolygonSymbolizer::SetStrokeColor(uint32_t color) {
    impl_->strokeColor = color;
}

uint32_t PolygonSymbolizer::GetStrokeColor() const {
    return impl_->strokeColor;
}

void PolygonSymbolizer::SetStrokeWidth(double width) {
    impl_->strokeWidth = width;
}

double PolygonSymbolizer::GetStrokeWidth() const {
    return impl_->strokeWidth;
}

void PolygonSymbolizer::SetStrokeOpacity(double opacity) {
    impl_->strokeOpacity = opacity;
}

double PolygonSymbolizer::GetStrokeOpacity() const {
    return impl_->strokeOpacity;
}

void PolygonSymbolizer::SetFillPattern(FillPattern pattern) {
    impl_->fillPattern = pattern;
}

FillPattern PolygonSymbolizer::GetFillPattern() const {
    return impl_->fillPattern;
}

void PolygonSymbolizer::SetDisplacement(double dx, double dy) {
    impl_->displacementX = dx;
    impl_->displacementY = dy;
}

void PolygonSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = impl_->displacementX;
    dy = impl_->displacementY;
}

void PolygonSymbolizer::SetPerpendicularOffset(double offset) {
    impl_->perpendicularOffset = offset;
}

double PolygonSymbolizer::GetPerpendicularOffset() const {
    return impl_->perpendicularOffset;
}

void PolygonSymbolizer::SetGraphicFill(bool enabled) {
    impl_->graphicFill = enabled;
}

bool PolygonSymbolizer::HasGraphicFill() const {
    return impl_->graphicFill;
}

void PolygonSymbolizer::SetGraphicFillSize(double size) {
    impl_->graphicFillSize = size;
}

double PolygonSymbolizer::GetGraphicFillSize() const {
    return impl_->graphicFillSize;
}

void PolygonSymbolizer::SetGraphicFillSpacing(double spacing) {
    impl_->graphicFillSpacing = spacing;
}

double PolygonSymbolizer::GetGraphicFillSpacing() const {
    return impl_->graphicFillSpacing;
}

SymbolizerPtr PolygonSymbolizer::Clone() const {
    auto sym = std::make_shared<PolygonSymbolizer>();
    sym->impl_->fillColor = impl_->fillColor;
    sym->impl_->fillOpacity = impl_->fillOpacity;
    sym->impl_->strokeColor = impl_->strokeColor;
    sym->impl_->strokeWidth = impl_->strokeWidth;
    sym->impl_->strokeOpacity = impl_->strokeOpacity;
    sym->impl_->fillPattern = impl_->fillPattern;
    sym->impl_->displacementX = impl_->displacementX;
    sym->impl_->displacementY = impl_->displacementY;
    sym->impl_->perpendicularOffset = impl_->perpendicularOffset;
    sym->impl_->graphicFill = impl_->graphicFill;
    sym->impl_->graphicFillSize = impl_->graphicFillSize;
    sym->impl_->graphicFillSpacing = impl_->graphicFillSpacing;
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    sym->SetOpacity(GetOpacity());
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
        x[i] = coord.x + impl_->displacementX;
        y[i] = coord.y + impl_->displacementY;
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
