#ifndef OGC_GRAPH_RENDER_DRAW_FACADE_H
#define OGC_GRAPH_RENDER_DRAW_FACADE_H

#include "ogc/graph/export.h"
#include "ogc/graph/render/draw_params.h"
#include <ogc/draw/draw_result.h>
#include <ogc/draw/draw_style.h>
#include <ogc/draw/draw_context.h>
#include <ogc/draw/draw_device.h>
#include <ogc/draw/draw_engine.h>
#include <ogc/draw/color.h>
#include <ogc/draw/font.h>
#include <ogc/draw/draw_types.h>
#include "ogc/geom/geometry.h"
#include "ogc/geom/envelope.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace graph {

using ogc::draw::DrawResult;

class OGC_GRAPH_API DrawFacade {
public:
    static DrawFacade& Instance();
    
    DrawResult Initialize();
    DrawResult Finalize();
    bool IsInitialized() const;
    
    ogc::draw::DrawContextPtr CreateContext();
    ogc::draw::DrawContextPtr CreateContext(ogc::draw::DrawDevicePtr device);
    ogc::draw::DrawContextPtr CreateContext(ogc::draw::DrawDevicePtr device, ogc::draw::DrawEnginePtr engine);
    
    ogc::draw::DrawDevicePtr CreateDevice(ogc::draw::DeviceType type, int width = 800, int height = 600);
    ogc::draw::DrawEnginePtr CreateEngine(ogc::draw::EngineType type);
    
    DrawResult RegisterDevice(const std::string& name, ogc::draw::DrawDevicePtr device);
    DrawResult RegisterEngine(const std::string& name, ogc::draw::DrawEnginePtr engine);
    
    ogc::draw::DrawDevicePtr GetDevice(const std::string& name);
    ogc::draw::DrawEnginePtr GetEngine(const std::string& name);
    
    DrawResult DrawGeometry(ogc::draw::DrawDevicePtr device, const Geometry* geometry, const ogc::draw::DrawStyle& style);
    DrawResult DrawGeometry(ogc::draw::DrawContextPtr context, const Geometry* geometry, const ogc::draw::DrawStyle& style);
    
    DrawResult RenderToImage(const Geometry* geometry, const DrawParams& params, const ogc::draw::DrawStyle& style, 
                              uint8_t* imageData, int width, int height, int channels);
    
    DrawResult RenderToImage(const std::vector<Geometry*>& geometries, const DrawParams& params,
                              const std::vector<ogc::draw::DrawStyle>& styles, uint8_t* imageData, 
                              int width, int height, int channels);
    
    void SetDefaultDrawStyle(const ogc::draw::DrawStyle& style);
    ogc::draw::DrawStyle GetDefaultDrawStyle() const;
    
    void SetDefaultFont(const ogc::draw::Font& font);
    ogc::draw::Font GetDefaultFont() const;
    
    void SetDefaultColor(const ogc::draw::Color& color);
    ogc::draw::Color GetDefaultColor() const;
    
    void SetDefaultDpi(double dpi);
    double GetDefaultDpi() const;
    
    void SetDefaultBackground(const ogc::draw::Color& color);
    ogc::draw::Color GetDefaultBackground() const;
    
    ogc::draw::DrawStyle CreateStrokeStyle(uint32_t color, double width);
    ogc::draw::DrawStyle CreateFillStyle(uint32_t color);
    ogc::draw::DrawStyle CreateStrokeFillStyle(uint32_t strokeColor, double strokeWidth, uint32_t fillColor);
    
    ogc::draw::Font CreateFont(const std::string& family, double size, bool bold = false, bool italic = false);
    
    ogc::draw::Color CreateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    ogc::draw::Color CreateColorFromHex(const std::string& hex);
    
    DrawParams CreateDrawParams(const Envelope& extent, int width, int height, double dpi = 96.0);
    
    static std::string GetVersion();
    static std::string GetBuildInfo();

private:
    DrawFacade();
    ~DrawFacade();
    
    DrawFacade(const DrawFacade&) = delete;
    DrawFacade& operator=(const DrawFacade&) = delete;
    
    bool m_initialized;
    ogc::draw::DrawStyle m_defaultStyle;
    ogc::draw::Font m_defaultFont;
    ogc::draw::Color m_defaultColor;
    ogc::draw::Color m_defaultBackground;
    double m_defaultDpi;
};

}  
}  

#endif  
