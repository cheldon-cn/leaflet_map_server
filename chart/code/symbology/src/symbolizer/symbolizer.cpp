#include "ogc/symbology/symbolizer/symbolizer.h"

namespace ogc {
namespace symbology {

struct Symbolizer::Impl {
    std::string name;
    ogc::draw::DrawStyle defaultStyle;
    bool enabled = true;
    double minScale = 0.0;
    double maxScale = std::numeric_limits<double>::max();
    int zIndex = 0;
    double opacity = 1.0;
};

Symbolizer::Symbolizer() : impl_(std::make_unique<Impl>()) {
}

Symbolizer::~Symbolizer() = default;

std::string Symbolizer::GetName() const {
    return impl_->name;
}

void Symbolizer::SetDefaultStyle(const ogc::draw::DrawStyle& style) {
    impl_->defaultStyle = style;
}

ogc::draw::DrawStyle Symbolizer::GetDefaultStyle() const {
    return impl_->defaultStyle;
}

void Symbolizer::SetEnabled(bool enabled) {
    impl_->enabled = enabled;
}

bool Symbolizer::IsEnabled() const {
    return impl_->enabled;
}

void Symbolizer::SetName(const std::string& name) {
    impl_->name = name;
}

void Symbolizer::SetMinScale(double scale) {
    impl_->minScale = scale;
}

double Symbolizer::GetMinScale() const {
    return impl_->minScale;
}

void Symbolizer::SetMaxScale(double scale) {
    impl_->maxScale = scale;
}

double Symbolizer::GetMaxScale() const {
    return impl_->maxScale;
}

bool Symbolizer::IsVisibleAtScale(double scale) const {
    if (scale <= 0) {
        return true;
    }
    return scale >= impl_->minScale && scale <= impl_->maxScale;
}

void Symbolizer::SetZIndex(int index) {
    impl_->zIndex = index;
}

int Symbolizer::GetZIndex() const {
    return impl_->zIndex;
}

void Symbolizer::SetOpacity(double opacity) {
    impl_->opacity = opacity;
}

double Symbolizer::GetOpacity() const {
    return impl_->opacity;
}

ogc::draw::DrawStyle Symbolizer::MergeStyle(const ogc::draw::DrawStyle& base, const ogc::draw::DrawStyle& override) const {
    ogc::draw::DrawStyle result = base;
    
    if (override.pen.IsVisible()) {
        result.pen = override.pen;
    } else if (base.pen.IsVisible()) {
        result.pen = base.pen;
    }
    
    if (override.brush.IsVisible()) {
        result.brush = override.brush;
    } else if (base.brush.IsVisible()) {
        result.brush = base.brush;
    }
    
    return result;
}

SymbolizerPtr Symbolizer::Create(SymbolizerType type) {
    (void)type;
    return nullptr;
}

std::string& Symbolizer::NameRef() {
    return impl_->name;
}

ogc::draw::DrawStyle& Symbolizer::DefaultStyleRef() {
    return impl_->defaultStyle;
}

bool& Symbolizer::EnabledRef() {
    return impl_->enabled;
}

double& Symbolizer::MinScaleRef() {
    return impl_->minScale;
}

double& Symbolizer::MaxScaleRef() {
    return impl_->maxScale;
}

int& Symbolizer::ZIndexRef() {
    return impl_->zIndex;
}

double& Symbolizer::OpacityRef() {
    return impl_->opacity;
}

}
}
