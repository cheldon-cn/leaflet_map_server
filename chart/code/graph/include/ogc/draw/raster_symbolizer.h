#ifndef OGC_DRAW_RASTER_SYMBOLIZER_H
#define OGC_DRAW_RASTER_SYMBOLIZER_H

#include "ogc/draw/symbolizer.h"
#include <ogc/draw/color.h>
#include <ogc/draw/draw_types.h>
#include <string>
#include <vector>

namespace ogc {
namespace draw {

enum class RasterChannelSelection {
    kRGB,
    kGrayscale,
    kPseudoColor,
    kCustom
};

struct ColorMapEntry {
    double value;
    uint32_t color;
    double opacity;
    std::string label;
    
    ColorMapEntry()
        : value(0.0)
        , color(0xFF000000)
        , opacity(1.0) {}
    
    ColorMapEntry(double v, uint32_t c, double o = 1.0, const std::string& l = "")
        : value(v)
        , color(c)
        , opacity(o)
        , label(l) {}
};

class RasterSymbolizer;
using RasterSymbolizerPtr = std::shared_ptr<RasterSymbolizer>;

class OGC_GRAPH_API RasterSymbolizer : public Symbolizer {
public:
    RasterSymbolizer();
    ~RasterSymbolizer() override = default;
    
    SymbolizerType GetType() const override { return SymbolizerType::kRaster; }
    std::string GetName() const override { return m_name.empty() ? "RasterSymbolizer" : m_name; }
    
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry) override;
    DrawResult Symbolize(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style) override;
    
    bool CanSymbolize(GeomType geomType) const override;
    
    SymbolizerPtr Clone() const override;
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    void SetChannelSelection(RasterChannelSelection selection);
    RasterChannelSelection GetChannelSelection() const;
    
    void SetRedChannel(int channel);
    int GetRedChannel() const;
    
    void SetGreenChannel(int channel);
    int GetGreenChannel() const;
    
    void SetBlueChannel(int channel);
    int GetBlueChannel() const;
    
    void SetGrayChannel(int channel);
    int GetGrayChannel() const;
    
    void SetContrastEnhancement(bool enabled);
    bool HasContrastEnhancement() const;
    
    void SetContrastValue(double value);
    double GetContrastValue() const;
    
    void SetBrightnessValue(double value);
    double GetBrightnessValue() const;
    
    void SetGammaValue(double value);
    double GetGammaValue() const;
    
    void AddColorMapEntry(const ColorMapEntry& entry);
    void ClearColorMap();
    std::vector<ColorMapEntry> GetColorMap() const;
    
    void SetColorMapType(RasterChannelSelection type);
    RasterChannelSelection GetColorMapType() const;
    
    void SetMinValue(double value);
    double GetMinValue() const;
    
    void SetMaxValue(double value);
    double GetMaxValue() const;
    
    void SetResampling(const std::string& method);
    std::string GetResampling() const;
    
    void SetOverlapBehavior(const std::string& behavior);
    std::string GetOverlapBehavior() const;
    
    static RasterSymbolizerPtr Create();

private:
    uint32_t InterpolateColor(double value, double minVal, double maxVal) const;
    
    double m_opacity;
    RasterChannelSelection m_channelSelection;
    int m_redChannel;
    int m_greenChannel;
    int m_blueChannel;
    int m_grayChannel;
    bool m_contrastEnhancement;
    double m_contrastValue;
    double m_brightnessValue;
    double m_gammaValue;
    std::vector<ColorMapEntry> m_colorMap;
    RasterChannelSelection m_colorMapType;
    double m_minValue;
    double m_maxValue;
    std::string m_resampling;
    std::string m_overlapBehavior;
};

}  
}  

#endif  
