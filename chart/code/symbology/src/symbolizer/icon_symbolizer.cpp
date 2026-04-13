#include "ogc/symbology/symbolizer/icon_symbolizer.h"
#include "ogc/geom/point.h"
#include "ogc/geom/multipoint.h"
#include <cmath>

namespace ogc {
namespace symbology {

struct IconSymbolizer::Impl {
    std::string iconPath;
    std::vector<uint8_t> iconData;
    int dataWidth = 0;
    int dataHeight = 0;
    int dataChannels = 0;
    double width = 16.0;
    double height = 16.0;
    double opacity = 1.0;
    double rotation = 0.0;
    double anchorX = 0.5;
    double anchorY = 0.5;
    double displacementX = 0.0;
    double displacementY = 0.0;
    bool allowOverlap = false;
    uint32_t colorReplacement = 0;
    bool hasColorReplacement = false;
};

IconSymbolizer::IconSymbolizer() : impl_(std::make_unique<Impl>()) {
}

IconSymbolizer::IconSymbolizer(const std::string& iconPath) : impl_(std::make_unique<Impl>()) {
    impl_->iconPath = iconPath;
}

IconSymbolizer::~IconSymbolizer() = default;

std::string IconSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "IconSymbolizer" : name;
}

ogc::draw::DrawResult IconSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, GetDefaultStyle());
}

ogc::draw::DrawResult IconSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
    
    (void)style;
    
    GeomType geomType = geometry->GetGeometryType();
    
    if (geomType == GeomType::kPoint) {
        const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
        if (point) {
            double x = point->GetX() + impl_->displacementX;
            double y = point->GetY() + impl_->displacementY;
            return DrawIcon(context, x, y);
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
                    ogc::draw::DrawResult r = DrawIcon(context, x, y);
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

bool IconSymbolizer::CanSymbolize(GeomType geomType) const {
    return geomType == GeomType::kPoint || 
           geomType == GeomType::kMultiPoint;
}

void IconSymbolizer::SetIconPath(const std::string& path) {
    impl_->iconPath = path;
}

std::string IconSymbolizer::GetIconPath() const {
    return impl_->iconPath;
}

void IconSymbolizer::SetIconData(const uint8_t* data, size_t size, int width, int height, int channels) {
    impl_->iconData.assign(data, data + size);
    impl_->dataWidth = width;
    impl_->dataHeight = height;
    impl_->dataChannels = channels;
}

bool IconSymbolizer::HasIconData() const {
    return !impl_->iconData.empty();
}

void IconSymbolizer::SetSize(double width, double height) {
    impl_->width = width;
    impl_->height = height;
}

void IconSymbolizer::GetSize(double& width, double& height) const {
    width = impl_->width;
    height = impl_->height;
}

void IconSymbolizer::SetWidth(double width) {
    impl_->width = width;
}

double IconSymbolizer::GetWidth() const {
    return impl_->width;
}

void IconSymbolizer::SetHeight(double height) {
    impl_->height = height;
}

double IconSymbolizer::GetHeight() const {
    return impl_->height;
}

void IconSymbolizer::SetOpacity(double opacity) {
    impl_->opacity = opacity;
}

double IconSymbolizer::GetOpacity() const {
    return impl_->opacity;
}

void IconSymbolizer::SetRotation(double angle) {
    impl_->rotation = angle;
}

double IconSymbolizer::GetRotation() const {
    return impl_->rotation;
}

void IconSymbolizer::SetAnchorPoint(double x, double y) {
    impl_->anchorX = x;
    impl_->anchorY = y;
}

void IconSymbolizer::GetAnchorPoint(double& x, double& y) const {
    x = impl_->anchorX;
    y = impl_->anchorY;
}

void IconSymbolizer::SetDisplacement(double dx, double dy) {
    impl_->displacementX = dx;
    impl_->displacementY = dy;
}

void IconSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = impl_->displacementX;
    dy = impl_->displacementY;
}

void IconSymbolizer::SetAllowOverlap(bool allow) {
    impl_->allowOverlap = allow;
}

bool IconSymbolizer::GetAllowOverlap() const {
    return impl_->allowOverlap;
}

void IconSymbolizer::SetColorReplacement(uint32_t color) {
    impl_->colorReplacement = color;
    impl_->hasColorReplacement = true;
}

uint32_t IconSymbolizer::GetColorReplacement() const {
    return impl_->colorReplacement;
}

bool IconSymbolizer::HasColorReplacement() const {
    return impl_->hasColorReplacement;
}

void IconSymbolizer::ClearColorReplacement() {
    impl_->hasColorReplacement = false;
    impl_->colorReplacement = 0;
}

SymbolizerPtr IconSymbolizer::Clone() const {
    auto sym = std::make_shared<IconSymbolizer>();
    sym->impl_->iconPath = impl_->iconPath;
    sym->impl_->iconData = impl_->iconData;
    sym->impl_->dataWidth = impl_->dataWidth;
    sym->impl_->dataHeight = impl_->dataHeight;
    sym->impl_->dataChannels = impl_->dataChannels;
    sym->impl_->width = impl_->width;
    sym->impl_->height = impl_->height;
    sym->impl_->opacity = impl_->opacity;
    sym->impl_->rotation = impl_->rotation;
    sym->impl_->anchorX = impl_->anchorX;
    sym->impl_->anchorY = impl_->anchorY;
    sym->impl_->displacementX = impl_->displacementX;
    sym->impl_->displacementY = impl_->displacementY;
    sym->impl_->allowOverlap = impl_->allowOverlap;
    sym->impl_->colorReplacement = impl_->colorReplacement;
    sym->impl_->hasColorReplacement = impl_->hasColorReplacement;
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    return sym;
}

IconSymbolizerPtr IconSymbolizer::Create() {
    return std::make_shared<IconSymbolizer>();
}

IconSymbolizerPtr IconSymbolizer::Create(const std::string& iconPath) {
    return std::make_shared<IconSymbolizer>(iconPath);
}

ogc::draw::DrawResult IconSymbolizer::DrawIcon(ogc::draw::DrawContextPtr context, double x, double y) {
    double drawX = x - impl_->width * impl_->anchorX;
    double drawY = y - impl_->height * impl_->anchorY;
    
    if (impl_->rotation != 0.0) {
        context->Save();
        context->Translate(x, y);
        context->Rotate(impl_->rotation);
        context->Translate(-x, -y);
    }
    
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    
    if (!impl_->iconData.empty() && impl_->dataWidth > 0 && impl_->dataHeight > 0) {
        ogc::draw::Image image = ogc::draw::Image::FromData(impl_->dataWidth, impl_->dataHeight, impl_->dataChannels, impl_->iconData.data());
        result = context->DrawImage(drawX, drawY, image, impl_->width / impl_->dataWidth, impl_->height / impl_->dataHeight);
    } else {
        ogc::draw::DrawStyle placeholderStyle = ogc::draw::DrawStyle::StrokeAndFill(ogc::draw::Color(0xFF000000), 1.0, ogc::draw::Color(0xFF808080));
        context->Save();
        context->SetStyle(placeholderStyle);
        result = context->DrawRect(drawX, drawY, impl_->width, impl_->height);
        context->Restore();
    }
    
    if (impl_->rotation != 0.0) {
        context->Restore();
    }
    
    return result;
}

}
}
