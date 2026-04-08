#ifndef OGC_DRAW_MACOS_PLATFORM_H
#define OGC_DRAW_MACOS_PLATFORM_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_device.h"

#ifdef __APPLE__

#include <string>
#include <memory>
#include <vector>

namespace ogc {
namespace draw {

class OGC_DRAW_API MacOSPlatform {
public:
    static MacOSPlatform& Instance();
    
    bool Initialize();
    void Shutdown();
    
    bool IsMetalAvailable() const;
    bool IsCoreGraphicsAvailable() const;
    
    EngineType GetPreferredEngineType() const;
    std::vector<EngineType> GetSupportedEngineTypes() const;
    
    std::unique_ptr<DrawEngine> CreatePreferredEngine(DrawDevice* device);
    std::unique_ptr<DrawEngine> CreateEngine(DrawDevice* device, EngineType type);
    
    struct SystemInfo {
        std::string macOSVersion;
        std::string metalVersion;
        int gpuMemoryMB;
        bool supportsMetal;
        bool supportsCoreGraphics;
        int displayCount;
        int primaryDisplayWidth;
        int primaryDisplayHeight;
        double primaryDisplayDpi;
    };
    
    SystemInfo GetSystemInfo() const;
    
    void SetPreferredEngineType(EngineType type);
    
    bool IsInitialized() const { return m_initialized; }

private:
    MacOSPlatform();
    ~MacOSPlatform();
    
    MacOSPlatform(const MacOSPlatform&) = delete;
    MacOSPlatform& operator=(const MacOSPlatform&) = delete;
    
    bool m_initialized = false;
    EngineType m_preferredEngineType = EngineType::CoreGraphics;
    SystemInfo m_systemInfo;
    
    void DetectSystemCapabilities();
};

}
}

#endif

#endif
