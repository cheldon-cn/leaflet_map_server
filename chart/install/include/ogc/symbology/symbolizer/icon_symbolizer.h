#ifndef OGC_SYMBOLOGY_ICON_SYMBOLIZER_H
#define OGC_SYMBOLOGY_ICON_SYMBOLIZER_H

#include "ogc/symbology/symbolizer/symbolizer.h"
#include <ogc/draw/color.h>
#include <ogc/draw/draw_types.h>
#include <string>

namespace ogc {
namespace symbology {

class IconSymbolizer;
using IconSymbolizerPtr = std::shared_ptr<IconSymbolizer>;

class OGC_SYMBOLOGY_API IconSymbolizer : public Symbolizer {
public:
    IconSymbolizer();
    explicit IconSymbolizer(const std::string& iconPath);
    ~IconSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kIcon; }
    std::string GetName() const override { return m_name.empty() ? "IconSymbolizer" : m_name; }
    
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry) override;
    ogc::draw::DrawResult Symbolize(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetIconPath(const std::string& path);
    std::string GetIconPath() const;
    
    void SetIconData(const uint8_t* data, size_t size, int width, int height, int channels);
    bool HasIconData() const;
    
    void SetSize(double width, double height);
    void GetSize(double& width, double& height) const;
    
    void SetWidth(double width);
    double GetWidth() const;
    
    void SetHeight(double height);
    double GetHeight() const;
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    void SetRotation(double angle);
    double GetRotation() const;
    
    void SetAnchorPoint(double x, double y);
    void GetAnchorPoint(double& x, double& y) const;
    
    void SetDisplacement(double dx, double dy);
    void GetDisplacement(double& dx, double& dy) const;
    
    void SetAllowOverlap(bool allow);
    bool GetAllowOverlap() const;
    
    void SetColorReplacement(uint32_t color);
    uint32_t GetColorReplacement() const;
    bool HasColorReplacement() const;
    
    void ClearColorReplacement();
    
    static IconSymbolizerPtr Create();
    static IconSymbolizerPtr Create(const std::string& iconPath);

private:
    ogc::draw::DrawResult DrawIcon(ogc::draw::DrawContextPtr context, double x, double y);
    
    std::string m_iconPath;
    std::vector<uint8_t> m_iconData;
    int m_dataWidth;
    int m_dataHeight;
    int m_dataChannels;
    double m_width;
    double m_height;
    double m_opacity;
    double m_rotation;
    double m_anchorX;
    double m_anchorY;
    double m_displacementX;
    double m_displacementY;
    bool m_allowOverlap;
    uint32_t m_colorReplacement;
    bool m_hasColorReplacement;
};

}
}

#endif
