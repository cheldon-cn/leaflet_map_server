#ifndef OGC_DRAW_DRAW_FACADE_H
#define OGC_DRAW_DRAW_FACADE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_driver.h"
#include "ogc/draw/color.h"
#include "ogc/draw/font.h"
#include "ogc/draw/types.h"
#include "ogc/geometry.h"
#include "ogc/envelope.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace draw {

class OGC_GRAPH_API DrawFacade {
public:
    static DrawFacade& Instance();
    
    DrawResult Initialize();
    DrawResult Finalize();
    bool IsInitialized() const;
    
    DrawContextPtr CreateContext();
    DrawContextPtr CreateContext(DrawDevicePtr device);
    DrawContextPtr CreateContext(DrawDevicePtr device, DrawEnginePtr engine);
    
    DrawDevicePtr CreateDevice(DeviceType type, int width = 800, int height = 600);
    DrawEnginePtr CreateEngine(EngineType type);
    DrawDriverPtr CreateDriver(const std::string& name);
    
    DrawResult RegisterDevice(const std::string& name, DrawDevicePtr device);
    DrawResult RegisterEngine(const std::string& name, DrawEnginePtr engine);
    DrawResult RegisterDriver(const std::string& name, DrawDriverPtr driver);
    
    DrawDevicePtr GetDevice(const std::string& name);
    DrawEnginePtr GetEngine(const std::string& name);
    DrawDriverPtr GetDriver(const std::string& name);
    
    DrawResult DrawGeometry(DrawDevicePtr device, const Geometry* geometry, const DrawStyle& style);
    DrawResult DrawGeometry(DrawContextPtr context, const Geometry* geometry, const DrawStyle& style);
    
    DrawResult RenderToImage(const Geometry* geometry, const DrawParams& params, const DrawStyle& style, 
                              uint8_t* imageData, int width, int height, int channels);
    
    DrawResult RenderToImage(const std::vector<Geometry*>& geometries, const DrawParams& params,
                              const std::vector<DrawStyle>& styles, uint8_t* imageData, 
                              int width, int height, int channels);
    
    void SetDefaultDrawStyle(const DrawStyle& style);
    DrawStyle GetDefaultDrawStyle() const;
    
    void SetDefaultFont(const Font& font);
    Font GetDefaultFont() const;
    
    void SetDefaultColor(const Color& color);
    Color GetDefaultColor() const;
    
    void SetDefaultDpi(double dpi);
    double GetDefaultDpi() const;
    
    void SetDefaultBackground(const Color& color);
    Color GetDefaultBackground() const;
    
    DrawStyle CreateStrokeStyle(uint32_t color, double width);
    DrawStyle CreateFillStyle(uint32_t color);
    DrawStyle CreateStrokeFillStyle(uint32_t strokeColor, double strokeWidth, uint32_t fillColor);
    
    Font CreateFont(const std::string& family, double size, bool bold = false, bool italic = false);
    
    Color CreateColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    Color CreateColorFromHex(const std::string& hex);
    
    DrawParams CreateDrawParams(const Envelope& extent, int width, int height, double dpi = 96.0);
    
    static std::string GetVersion();
    static std::string GetBuildInfo();

private:
    DrawFacade();
    ~DrawFacade();
    
    DrawFacade(const DrawFacade&) = delete;
    DrawFacade& operator=(const DrawFacade&) = delete;
    
    bool m_initialized;
    DrawStyle m_defaultStyle;
    Font m_defaultFont;
    Color m_defaultColor;
    Color m_defaultBackground;
    double m_defaultDpi;
};

}  
}  

#endif  
