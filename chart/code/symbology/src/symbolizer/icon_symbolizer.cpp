#include "ogc/symbology/symbolizer/icon_symbolizer.h"
#include "ogc/geom/point.h"
#include "ogc/geom/multipoint.h"
#include <cmath>

namespace ogc {
namespace symbology {

IconSymbolizer::IconSymbolizer()
    : m_dataWidth(0)
    , m_dataHeight(0)
    , m_dataChannels(0)
    , m_width(16.0)
    , m_height(16.0)
    , m_opacity(1.0)
    , m_rotation(0.0)
    , m_anchorX(0.5)
    , m_anchorY(0.5)
    , m_displacementX(0.0)
    , m_displacementY(0.0)
    , m_allowOverlap(false)
    , m_colorReplacement(0)
    , m_hasColorReplacement(false) {
}

IconSymbolizer::IconSymbolizer(const std::string& iconPath)
    : m_iconPath(iconPath)
    , m_dataWidth(0)
    , m_dataHeight(0)
    , m_dataChannels(0)
    , m_width(16.0)
    , m_height(16.0)
    , m_opacity(1.0)
    , m_rotation(0.0)
    , m_anchorX(0.5)
    , m_anchorY(0.5)
    , m_displacementX(0.0)
    , m_displacementY(0.0)
    , m_allowOverlap(false)
    , m_colorReplacement(0)
    , m_hasColorReplacement(false) {
}

ogc::draw::DrawResult IconSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) {
    return Symbolize(context, geometry, m_defaultStyle);
}

ogc::draw::DrawResult IconSymbolizer::Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) {
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
    
    GeomType geomType = geometry->GetGeometryType();
    
    if (geomType == GeomType::kPoint) {
        const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
        if (point) {
            double x = point->GetX() + m_displacementX;
            double y = point->GetY() + m_displacementY;
            return DrawIcon(context, x, y);
        }
    } else if (geomType == GeomType::kMultiPoint) {
        const ogc::MultiPoint* multiPoint = dynamic_cast<const ogc::MultiPoint*>(geometry);
        if (multiPoint) {
            ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
            for (size_t i = 0; i < multiPoint->GetNumPoints(); ++i) {
                const ogc::Point* pt = multiPoint->GetPointN(i);
                if (pt) {
                    double x = pt->GetX() + m_displacementX;
                    double y = pt->GetY() + m_displacementY;
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
    m_iconPath = path;
}

std::string IconSymbolizer::GetIconPath() const {
    return m_iconPath;
}

void IconSymbolizer::SetIconData(const uint8_t* data, size_t size, int width, int height, int channels) {
    m_iconData.assign(data, data + size);
    m_dataWidth = width;
    m_dataHeight = height;
    m_dataChannels = channels;
}

bool IconSymbolizer::HasIconData() const {
    return !m_iconData.empty();
}

void IconSymbolizer::SetSize(double width, double height) {
    m_width = width;
    m_height = height;
}

void IconSymbolizer::GetSize(double& width, double& height) const {
    width = m_width;
    height = m_height;
}

void IconSymbolizer::SetWidth(double width) {
    m_width = width;
}

double IconSymbolizer::GetWidth() const {
    return m_width;
}

void IconSymbolizer::SetHeight(double height) {
    m_height = height;
}

double IconSymbolizer::GetHeight() const {
    return m_height;
}

void IconSymbolizer::SetOpacity(double opacity) {
    m_opacity = opacity;
}

double IconSymbolizer::GetOpacity() const {
    return m_opacity;
}

void IconSymbolizer::SetRotation(double angle) {
    m_rotation = angle;
}

double IconSymbolizer::GetRotation() const {
    return m_rotation;
}

void IconSymbolizer::SetAnchorPoint(double x, double y) {
    m_anchorX = x;
    m_anchorY = y;
}

void IconSymbolizer::GetAnchorPoint(double& x, double& y) const {
    x = m_anchorX;
    y = m_anchorY;
}

void IconSymbolizer::SetDisplacement(double dx, double dy) {
    m_displacementX = dx;
    m_displacementY = dy;
}

void IconSymbolizer::GetDisplacement(double& dx, double& dy) const {
    dx = m_displacementX;
    dy = m_displacementY;
}

void IconSymbolizer::SetAllowOverlap(bool allow) {
    m_allowOverlap = allow;
}

bool IconSymbolizer::GetAllowOverlap() const {
    return m_allowOverlap;
}

void IconSymbolizer::SetColorReplacement(uint32_t color) {
    m_colorReplacement = color;
    m_hasColorReplacement = true;
}

uint32_t IconSymbolizer::GetColorReplacement() const {
    return m_colorReplacement;
}

bool IconSymbolizer::HasColorReplacement() const {
    return m_hasColorReplacement;
}

void IconSymbolizer::ClearColorReplacement() {
    m_hasColorReplacement = false;
    m_colorReplacement = 0;
}

SymbolizerPtr IconSymbolizer::Clone() const {
    auto sym = std::make_shared<IconSymbolizer>();
    sym->m_iconPath = m_iconPath;
    sym->m_iconData = m_iconData;
    sym->m_dataWidth = m_dataWidth;
    sym->m_dataHeight = m_dataHeight;
    sym->m_dataChannels = m_dataChannels;
    sym->m_width = m_width;
    sym->m_height = m_height;
    sym->m_opacity = m_opacity;
    sym->m_rotation = m_rotation;
    sym->m_anchorX = m_anchorX;
    sym->m_anchorY = m_anchorY;
    sym->m_displacementX = m_displacementX;
    sym->m_displacementY = m_displacementY;
    sym->m_allowOverlap = m_allowOverlap;
    sym->m_colorReplacement = m_colorReplacement;
    sym->m_hasColorReplacement = m_hasColorReplacement;
    sym->m_name = m_name;
    sym->m_defaultStyle = m_defaultStyle;
    sym->m_enabled = m_enabled;
    sym->m_minScale = m_minScale;
    sym->m_maxScale = m_maxScale;
    sym->m_zIndex = m_zIndex;
    return sym;
}

IconSymbolizerPtr IconSymbolizer::Create() {
    return std::make_shared<IconSymbolizer>();
}

IconSymbolizerPtr IconSymbolizer::Create(const std::string& iconPath) {
    return std::make_shared<IconSymbolizer>(iconPath);
}

ogc::draw::DrawResult IconSymbolizer::DrawIcon(ogc::draw::DrawContextPtr context, double x, double y) {
    double drawX = x - m_width * m_anchorX;
    double drawY = y - m_height * m_anchorY;
    
    if (m_rotation != 0.0) {
        context->Save();
        context->Translate(x, y);
        context->Rotate(m_rotation);
        context->Translate(-x, -y);
    }
    
    ogc::draw::DrawResult result = ogc::draw::DrawResult::kSuccess;
    
    if (!m_iconData.empty() && m_dataWidth > 0 && m_dataHeight > 0) {
        ogc::draw::Image image = ogc::draw::Image::FromData(m_dataWidth, m_dataHeight, m_dataChannels, m_iconData.data());
        result = context->DrawImage(drawX, drawY, image, m_width / m_dataWidth, m_height / m_dataHeight);
    } else {
        ogc::draw::DrawStyle placeholderStyle = ogc::draw::DrawStyle::StrokeAndFill(ogc::draw::Color(0xFF000000), 1.0, ogc::draw::Color(0xFF808080));
        context->Save();
        context->SetStyle(placeholderStyle);
        result = context->DrawRect(drawX, drawY, m_width, m_height);
        context->Restore();
    }
    
    if (m_rotation != 0.0) {
        context->Restore();
    }
    
    return result;
}

}
}
