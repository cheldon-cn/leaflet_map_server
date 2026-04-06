#include "ogc/symbology/symbolizer/symbolizer.h"

namespace ogc {
namespace symbology {

Symbolizer::Symbolizer()
    : m_enabled(true)
    , m_minScale(0.0)
    , m_maxScale(std::numeric_limits<double>::max())
    , m_zIndex(0)
    , m_opacity(1.0) {
}

void Symbolizer::SetDefaultStyle(const ogc::draw::DrawStyle& style) {
    m_defaultStyle = style;
}

ogc::draw::DrawStyle Symbolizer::GetDefaultStyle() const {
    return m_defaultStyle;
}

void Symbolizer::SetEnabled(bool enabled) {
    m_enabled = enabled;
}

bool Symbolizer::IsEnabled() const {
    return m_enabled;
}

void Symbolizer::SetName(const std::string& name) {
    m_name = name;
}

void Symbolizer::SetMinScale(double scale) {
    m_minScale = scale;
}

double Symbolizer::GetMinScale() const {
    return m_minScale;
}

void Symbolizer::SetMaxScale(double scale) {
    m_maxScale = scale;
}

double Symbolizer::GetMaxScale() const {
    return m_maxScale;
}

bool Symbolizer::IsVisibleAtScale(double scale) const {
    if (scale <= 0) {
        return true;
    }
    return scale >= m_minScale && scale <= m_maxScale;
}

void Symbolizer::SetZIndex(int index) {
    m_zIndex = index;
}

int Symbolizer::GetZIndex() const {
    return m_zIndex;
}

void Symbolizer::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double Symbolizer::GetOpacity() const {
    return m_opacity;
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

}
}
