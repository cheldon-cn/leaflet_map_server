#include "ogc/symbology/symbolizer/point_symbolizer.h"
#include "ogc/geom/point.h"
#include "ogc/geom/multipoint.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace ogc {
namespace symbology {

struct PointSymbolizer::Impl {
    PointSymbolType symbolType = PointSymbolType::kCircle;
    double size = 6.0;
    uint32_t color = 0xFF0000FF;
    uint32_t strokeColor = 0xFF000000;
    double strokeWidth = 1.0;
    double rotation = 0.0;
    double anchorX = 0.5;
    double anchorY = 0.5;
    double displacementX = 0.0;
    double displacementY = 0.0;
};

PointSymbolizer::PointSymbolizer() : impl_(std::make_unique<Impl>()) {
}

PointSymbolizer::PointSymbolizer(double size, uint32_t color) : impl_(std::make_unique<Impl>()) {
    impl_->size = size;
    impl_->color = color;
}

PointSymbolizer::~PointSymbolizer() = default;

std::string PointSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "PointSymbolizer" : name;
}

ogc::draw::DrawResult PointSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, GetDefaultStyle());
}

ogc::draw::DrawResult PointSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
        finalStyle.brush = ogc::draw::Brush(ogc::draw::Color(impl_->color));
    }
    if (finalStyle.pen.width == 0) {
        finalStyle.pen = ogc::draw::Pen(ogc::draw::Color(impl_->strokeColor), impl_->strokeWidth);
    }
    
    GeomType geomType = geometry->GetGeometryType();
    
    if (geomType == GeomType::kPoint) {
        const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
        if (point) {
            double x = point->GetX() + impl_->displacementX;
            double y = point->GetY() + impl_->displacementY;
            return DrawPoint(context, x, y, finalStyle);
        }
    } else if (geomType == GeomType::kMultiPoint) {
        const ogc::MultiPoint* multiPoint = dynamic_cast<const ogc::MultiPoint*>(geometry);
        if (multiPoint) {
            ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
            for (size_t i = 0; i < multiPoint->GetNumPoints(); ++i) {
                const ogc::Point* pt = multiPoint->GetPointN(i);
                if (pt) {
                    double x = pt->GetX() + impl_->displacementX;
                    double y = pt->GetY() + impl_->displacementY;
                    ogc::draw::DrawResult r = DrawPoint(context, x, y, finalStyle);
                    if (r != ogc::draw::DrawResult::kSuccess) {
                        result = r;
                    }
                }
            }
            return result;
        }
    }
    
    return ogc::draw::DrawResult::kInvalidParameter;
}

bool PointSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kPoint || 
           geomType == GeomType::kMultiPoint;
}

void PointSymbolizer::SetSymbolType(PointSymbolType type) {
    impl_->symbolType = type;
}

PointSymbolType PointSymbolizer::GetSymbolType() const {
    return impl_->symbolType;
}

void PointSymbolizer::SetSize(double size) {
    impl_->size = size;
}

double PointSymbolizer::GetSize() const {
    return impl_->size;
}

void PointSymbolizer::SetColor(uint32_t color) {
    impl_->color = color;
}

uint32_t PointSymbolizer::GetColor() const {
    return impl_->color;
}

void PointSymbolizer::SetStrokeColor(uint32_t color) {
    impl_->strokeColor = color;
}

uint32_t PointSymbolizer::GetStrokeColor() const {
    return impl_->strokeColor;
}

void PointSymbolizer::SetStrokeWidth(double width) {
    impl_->strokeWidth = width;
}

double PointSymbolizer::GetStrokeWidth() const {
    return impl_->strokeWidth;
}

void PointSymbolizer::SetRotation(double angle) {
    impl_->rotation = angle;
}

double PointSymbolizer::GetRotation() const {
    return impl_->rotation;
}

void PointSymbolizer::SetAnchorPoint(double x, double y) {
    impl_->anchorX = x;
    impl_->anchorY = y;
}

void PointSymbolizer::GetAnchorPoint(double& x, double& y) const {
    x = impl_->anchorX;
    y = impl_->anchorY;
}

void PointSymbolizer::SetDisplacement(double dx, double dy) {
    impl_->displacementX = dx;
    impl_->displacementY = dy;
}

void PointSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = impl_->displacementX;
    dy = impl_->displacementY;
}

SymbolizerPtr PointSymbolizer::Clone() const {
    auto sym = std::make_shared<PointSymbolizer>();
    sym->impl_->symbolType = impl_->symbolType;
    sym->impl_->size = impl_->size;
    sym->impl_->color = impl_->color;
    sym->impl_->strokeColor = impl_->strokeColor;
    sym->impl_->strokeWidth = impl_->strokeWidth;
    sym->impl_->rotation = impl_->rotation;
    sym->impl_->anchorX = impl_->anchorX;
    sym->impl_->anchorY = impl_->anchorY;
    sym->impl_->displacementX = impl_->displacementX;
    sym->impl_->displacementY = impl_->displacementY;
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    sym->SetOpacity(GetOpacity());
    return sym;
}

PointSymbolizerPtr PointSymbolizer::Create() {
    return std::make_shared<PointSymbolizer>();
}

PointSymbolizerPtr PointSymbolizer::Create(double size, uint32_t color) {
    return std::make_shared<PointSymbolizer>(size, color);
}

ogc::draw::DrawResult PointSymbolizer::DrawPoint(ogc::draw::DrawContextPtr context, double x, double y, const ogc::draw::DrawStyle& style) {
    return DrawSymbol(context, x, y, impl_->size, impl_->symbolType, style);
}

ogc::draw::DrawResult PointSymbolizer::DrawSymbol(ogc::draw::DrawContextPtr context, double x, double y, double size, PointSymbolType type, const ogc::draw::DrawStyle& style) {
    double halfSize = size / 2.0;
    
    if (impl_->rotation != 0.0) {
        context->Save();
        context->Translate(x, y);
        context->Rotate(impl_->rotation);
        context->Translate(-x, -y);
    }
    
    context->Save();
    context->SetStyle(style);
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    
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
    
    context->Restore();
    
    if (impl_->rotation != 0.0) {
        context->Restore();
    }
    
    return result;
}

}
}
