#ifndef OGC_DRAW_WINDOWS_PLATFORM_H
#define OGC_DRAW_WINDOWS_PLATFORM_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_device.h"
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32

namespace ogc {
namespace draw {
namespace windows {

struct PlatformCapabilities {
    bool hasGDIPlus;
    bool hasDirect2D;
    bool hasDirectWrite;
    bool hasHardwareAcceleration;
    int maxTextureSize;
    std::string preferredEngine;
};

class OGC_DRAW_API WindowsPlatform {
public:
    static std::unique_ptr<DrawEngine> CreateOptimalEngine(DrawDevice* device);
    
    static std::unique_ptr<DrawEngine> CreateEngine(DrawDevice* device, 
                                                     const std::string& engineName);
    
    static bool IsDirect2DAvailable();
    static bool IsGDIPlusAvailable();
    
    static PlatformCapabilities GetCapabilities();
    
    static std::vector<std::string> GetAvailableEngines();
    
    static std::string GetRecommendedEngine(DrawDevice* device);
    
    static bool Initialize();
    static void Shutdown();
    
private:
    static bool s_initialized;
    static PlatformCapabilities s_capabilities;
};

}

}

}

#endif

#endif
