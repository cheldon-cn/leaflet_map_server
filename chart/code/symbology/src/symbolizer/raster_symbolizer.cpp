#include "ogc/symbology/symbolizer/raster_symbolizer.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace symbology {

struct RasterSymbolizer::Impl {
    double opacity = 1.0;
    RasterChannelSelection channelSelection = RasterChannelSelection::kRGB;
    int redChannel = 0;
    int greenChannel = 1;
    int blueChannel = 2;
    int grayChannel = 0;
    bool contrastEnhancement = false;
    double contrastValue = 1.0;
    double brightnessValue = 0.0;
    double gammaValue = 1.0;
    std::vector<ColorMapEntry> colorMap;
    RasterChannelSelection colorMapType = RasterChannelSelection::kGrayscale;
    double minValue = 0.0;
    double maxValue = 255.0;
    std::string resampling = "nearest";
    std::string overlapBehavior = "auto";
};

RasterSymbolizer::RasterSymbolizer() : impl_(std::make_unique<Impl>()) {
}

RasterSymbolizer::~RasterSymbolizer() = default;

std::string RasterSymbolizer::GetName() const {
    std::string name = Symbolizer::GetName();
    return name.empty() ? "RasterSymbolizer" : name;
}

ogc::draw::DrawResult RasterSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, GetDefaultStyle());
}

ogc::draw::DrawResult RasterSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
    
    return ogc::draw::DrawResult::kSuccess;
}

bool RasterSymbolizer::CanSymbolize(GeomType geomType) const {
    (void)geomType;
    return false;
}

void RasterSymbolizer::SetOpacity(double opacity) {
    impl_->opacity = opacity;
}

double RasterSymbolizer::GetOpacity() const {
    return impl_->opacity;
}

void RasterSymbolizer::SetChannelSelection(RasterChannelSelection selection) {
    impl_->channelSelection = selection;
}

RasterChannelSelection RasterSymbolizer::GetChannelSelection() const {
    return impl_->channelSelection;
}

void RasterSymbolizer::SetRedChannel(int channel) {
    impl_->redChannel = channel;
}

int RasterSymbolizer::GetRedChannel() const {
    return impl_->redChannel;
}

void RasterSymbolizer::SetGreenChannel(int channel) {
    impl_->greenChannel = channel;
}

int RasterSymbolizer::GetGreenChannel() const {
    return impl_->greenChannel;
}

void RasterSymbolizer::SetBlueChannel(int channel) {
    impl_->blueChannel = channel;
}

int RasterSymbolizer::GetBlueChannel() const {
    return impl_->blueChannel;
}

void RasterSymbolizer::SetGrayChannel(int channel) {
    impl_->grayChannel = channel;
}

int RasterSymbolizer::GetGrayChannel() const {
    return impl_->grayChannel;
}

void RasterSymbolizer::SetContrastEnhancement(bool enabled) {
    impl_->contrastEnhancement = enabled;
}

bool RasterSymbolizer::HasContrastEnhancement() const {
    return impl_->contrastEnhancement;
}

void RasterSymbolizer::SetContrastValue(double value) {
    impl_->contrastValue = value;
}

double RasterSymbolizer::GetContrastValue() const {
    return impl_->contrastValue;
}

void RasterSymbolizer::SetBrightnessValue(double value) {
    impl_->brightnessValue = value;
}

double RasterSymbolizer::GetBrightnessValue() const {
    return impl_->brightnessValue;
}

void RasterSymbolizer::SetGammaValue(double value) {
    impl_->gammaValue = value;
}

double RasterSymbolizer::GetGammaValue() const {
    return impl_->gammaValue;
}

void RasterSymbolizer::AddColorMapEntry(const ColorMapEntry& entry) {
    impl_->colorMap.push_back(entry);
}

void RasterSymbolizer::ClearColorMap() {
    impl_->colorMap.clear();
}

std::vector<ColorMapEntry> RasterSymbolizer::GetColorMap() const {
    return impl_->colorMap;
}

void RasterSymbolizer::SetColorMapType(RasterChannelSelection type) {
    impl_->colorMapType = type;
}

RasterChannelSelection RasterSymbolizer::GetColorMapType() const {
    return impl_->colorMapType;
}

void RasterSymbolizer::SetMinValue(double value) {
    impl_->minValue = value;
}

double RasterSymbolizer::GetMinValue() const {
    return impl_->minValue;
}

void RasterSymbolizer::SetMaxValue(double value) {
    impl_->maxValue = value;
}

double RasterSymbolizer::GetMaxValue() const {
    return impl_->maxValue;
}

void RasterSymbolizer::SetResampling(const std::string& method) {
    impl_->resampling = method;
}

std::string RasterSymbolizer::GetResampling() const {
    return impl_->resampling;
}

void RasterSymbolizer::SetOverlapBehavior(const std::string& behavior) {
    impl_->overlapBehavior = behavior;
}

std::string RasterSymbolizer::GetOverlapBehavior() const {
    return impl_->overlapBehavior;
}

SymbolizerPtr RasterSymbolizer::Clone() const {
    auto sym = std::make_shared<RasterSymbolizer>();
    sym->impl_->opacity = impl_->opacity;
    sym->impl_->channelSelection = impl_->channelSelection;
    sym->impl_->redChannel = impl_->redChannel;
    sym->impl_->greenChannel = impl_->greenChannel;
    sym->impl_->blueChannel = impl_->blueChannel;
    sym->impl_->grayChannel = impl_->grayChannel;
    sym->impl_->contrastEnhancement = impl_->contrastEnhancement;
    sym->impl_->contrastValue = impl_->contrastValue;
    sym->impl_->brightnessValue = impl_->brightnessValue;
    sym->impl_->gammaValue = impl_->gammaValue;
    sym->impl_->colorMap = impl_->colorMap;
    sym->impl_->colorMapType = impl_->colorMapType;
    sym->impl_->minValue = impl_->minValue;
    sym->impl_->maxValue = impl_->maxValue;
    sym->impl_->resampling = impl_->resampling;
    sym->impl_->overlapBehavior = impl_->overlapBehavior;
    sym->SetName(GetName());
    sym->SetDefaultStyle(GetDefaultStyle());
    sym->SetEnabled(IsEnabled());
    sym->SetMinScale(GetMinScale());
    sym->SetMaxScale(GetMaxScale());
    sym->SetZIndex(GetZIndex());
    return sym;
}

RasterSymbolizerPtr RasterSymbolizer::Create() {
    return std::make_shared<RasterSymbolizer>();
}

uint32_t RasterSymbolizer::InterpolateColor(double value, double minVal, double maxVal) const {
    if (impl_->colorMap.empty()) {
        double t = (value - minVal) / (maxVal - minVal);
        t = std::max(0.0, std::min(1.0, t));
        uint8_t gray = static_cast<uint8_t>(t * 255.0);
        return ogc::draw::Color::FromRGBA(gray, gray, gray, 255).GetRGBA();
    }
    
    if (impl_->colorMap.size() == 1) {
        return impl_->colorMap[0].color;
    }
    
    for (size_t i = 0; i < impl_->colorMap.size() - 1; ++i) {
        if (value >= impl_->colorMap[i].value && value <= impl_->colorMap[i + 1].value) {
            double t = (value - impl_->colorMap[i].value) / 
                       (impl_->colorMap[i + 1].value - impl_->colorMap[i].value);
            
            ogc::draw::Color c1(impl_->colorMap[i].color);
            ogc::draw::Color c2(impl_->colorMap[i + 1].color);
            
            uint8_t r = static_cast<uint8_t>(c1.GetRed() + t * (c2.GetRed() - c1.GetRed()));
            uint8_t g = static_cast<uint8_t>(c1.GetGreen() + t * (c2.GetGreen() - c1.GetGreen()));
            uint8_t b = static_cast<uint8_t>(c1.GetBlue() + t * (c2.GetBlue() - c1.GetBlue()));
            uint8_t a = static_cast<uint8_t>(c1.GetAlpha() + t * (c2.GetAlpha() - c1.GetAlpha()));
            
            return ogc::draw::Color::FromRGBA(r, g, b, a).GetRGBA();
        }
    }
    
    return 0xFF000000;
}

}
}
