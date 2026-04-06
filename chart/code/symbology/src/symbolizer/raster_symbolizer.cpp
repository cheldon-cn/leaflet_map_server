#include "ogc/symbology/symbolizer/raster_symbolizer.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace symbology {

RasterSymbolizer::RasterSymbolizer()
    : m_opacity(1.0)
    , m_channelSelection(RasterChannelSelection::kRGB)
    , m_redChannel(0)
    , m_greenChannel(1)
    , m_blueChannel(2)
    , m_grayChannel(0)
    , m_contrastEnhancement(false)
    , m_contrastValue(1.0)
    , m_brightnessValue(0.0)
    , m_gammaValue(1.0)
    , m_colorMapType(RasterChannelSelection::kGrayscale)
    , m_minValue(0.0)
    , m_maxValue(255.0)
    , m_resampling("nearest")
    , m_overlapBehavior("auto") {
}

ogc::draw::DrawResult RasterSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

ogc::draw::DrawResult RasterSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
    
    (void)style;
    
    return ogc::draw::DrawResult::kSuccess;
}

bool RasterSymbolizer::CanSymbolize(GeomType geomType) const {
    (void)geomType;
    return false;
}

void RasterSymbolizer::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double RasterSymbolizer::GetOpacity() const {
    return m_opacity;
}

void RasterSymbolizer::SetChannelSelection(RasterChannelSelection selection) {
    m_channelSelection = selection;
}

RasterChannelSelection RasterSymbolizer::GetChannelSelection() const {
    return m_channelSelection;
}

void RasterSymbolizer::SetRedChannel(int channel) {
    m_redChannel = channel;
}

int RasterSymbolizer::GetRedChannel() const {
    return m_redChannel;
}

void RasterSymbolizer::SetGreenChannel(int channel) {
    m_greenChannel = channel;
}

int RasterSymbolizer::GetGreenChannel() const {
    return m_greenChannel;
}

void RasterSymbolizer::SetBlueChannel(int channel) {
    m_blueChannel = channel;
}

int RasterSymbolizer::GetBlueChannel() const {
    return m_blueChannel;
}

void RasterSymbolizer::SetGrayChannel(int channel) {
    m_grayChannel = channel;
}

int RasterSymbolizer::GetGrayChannel() const {
    return m_grayChannel;
}

void RasterSymbolizer::SetContrastEnhancement(bool enabled) {
    m_contrastEnhancement = enabled;
}

bool RasterSymbolizer::HasContrastEnhancement() const {
    return m_contrastEnhancement;
}

void RasterSymbolizer::SetContrastValue(double value) {
    m_contrastValue = value;
}

double RasterSymbolizer::GetContrastValue() const {
    return m_contrastValue;
}

void RasterSymbolizer::SetBrightnessValue(double value) {
    m_brightnessValue = value;
}

double RasterSymbolizer::GetBrightnessValue() const {
    return m_brightnessValue;
}

void RasterSymbolizer::SetGammaValue(double value) {
    m_gammaValue = value;
}

double RasterSymbolizer::GetGammaValue() const {
    return m_gammaValue;
}

void RasterSymbolizer::AddColorMapEntry(const ColorMapEntry& entry) {
    m_colorMap.push_back(entry);
}

void RasterSymbolizer::ClearColorMap() {
    m_colorMap.clear();
}

std::vector<ColorMapEntry> RasterSymbolizer::GetColorMap() const {
    return m_colorMap;
}

void RasterSymbolizer::SetColorMapType(RasterChannelSelection type) {
    m_colorMapType = type;
}

RasterChannelSelection RasterSymbolizer::GetColorMapType() const {
    return m_colorMapType;
}

void RasterSymbolizer::SetMinValue(double value) {
    m_minValue = value;
}

double RasterSymbolizer::GetMinValue() const {
    return m_minValue;
}

void RasterSymbolizer::SetMaxValue(double value) {
    m_maxValue = value;
}

double RasterSymbolizer::GetMaxValue() const {
    return m_maxValue;
}

void RasterSymbolizer::SetResampling(const std::string& method) {
    m_resampling = method;
}

std::string RasterSymbolizer::GetResampling() const {
    return m_resampling;
}

void RasterSymbolizer::SetOverlapBehavior(const std::string& behavior) {
    m_overlapBehavior = behavior;
}

std::string RasterSymbolizer::GetOverlapBehavior() const {
    return m_overlapBehavior;
}

SymbolizerPtr RasterSymbolizer::Clone() const {
    auto sym = std::make_shared<RasterSymbolizer>();
    sym->m_opacity = m_opacity;
    sym->m_channelSelection = m_channelSelection;
    sym->m_redChannel = m_redChannel;
    sym->m_greenChannel = m_greenChannel;
    sym->m_blueChannel = m_blueChannel;
    sym->m_grayChannel = m_grayChannel;
    sym->m_contrastEnhancement = m_contrastEnhancement;
    sym->m_contrastValue = m_contrastValue;
    sym->m_brightnessValue = m_brightnessValue;
    sym->m_gammaValue = m_gammaValue;
    sym->m_colorMap = m_colorMap;
    sym->m_colorMapType = m_colorMapType;
    sym->m_minValue = m_minValue;
    sym->m_maxValue = m_maxValue;
    sym->m_resampling = m_resampling;
    sym->m_overlapBehavior = m_overlapBehavior;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    return sym;
}

RasterSymbolizerPtr RasterSymbolizer::Create() {
    return std::make_shared<RasterSymbolizer>();
}

uint32_t RasterSymbolizer::InterpolateColor(double value, double minVal, double maxVal) const {
    if (m_colorMap.empty()) {
        double t = (value - minVal) / (maxVal - minVal);
        t = std::max(0.0, std::min(1.0, t));
        uint8_t gray = static_cast<uint8_t>(t * 255.0);
        return ogc::draw::Color::FromRGBA(gray, gray, gray, 255).GetRGBA();
    }
    
    if (m_colorMap.size() == 1) {
        return m_colorMap[0].color;
    }
    
    for (size_t i = 0; i < m_colorMap.size() - 1; ++i) {
        if (value >= m_colorMap[i].value && value <= m_colorMap[i + 1].value) {
            double t = (value - m_colorMap[i].value) / 
                       (m_colorMap[i + 1].value - m_colorMap[i].value);
            
            ogc::draw::Color c1(m_colorMap[i].color);
            ogc::draw::Color c2(m_colorMap[i + 1].color);
            
            uint8_t r = static_cast<uint8_t>(c1.GetRed() + t * (c2.GetRed() - c1.GetRed()));
            uint8_t g = static_cast<uint8_t>(c1.GetGreen() + t * (c2.GetGreen() - c1.GetGreen()));
            uint8_t b = static_cast<uint8_t>(c1.GetBlue() + t * (c2.GetBlue() - c1.GetBlue()));
            uint8_t a = static_cast<uint8_t>(c1.GetAlpha() + t * (c2.GetAlpha() - c1.GetAlpha()));
            
            return ogc::draw::Color::FromRGBA(r, g, b, a).GetRGBA();
        }
    }
    
    if (value < m_colorMap[0].value) {
        return m_colorMap[0].color;
    }
    
    return m_colorMap.back().color;
}

}
}
