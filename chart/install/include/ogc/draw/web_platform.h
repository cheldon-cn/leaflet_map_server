#ifndef OGC_DRAW_WEB_PLATFORM_H
#define OGC_DRAW_WEB_PLATFORM_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_device.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/html5.h>
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

class OGC_DRAW_API WebPlatform {
public:
    static WebPlatform& Instance();
    
    bool Initialize();
    void Shutdown();
    
    bool IsWebGLAvailable() const;
    bool IsWebGL2Available() const;
    
    EngineType GetPreferredEngineType() const;
    std::vector<EngineType> GetSupportedEngineTypes() const;
    
    std::unique_ptr<DrawEngine> CreatePreferredEngine(DrawDevice* device);
    std::unique_ptr<DrawEngine> CreateEngine(DrawDevice* device, EngineType type);
    
    struct SystemInfo {
        std::string userAgent;
        std::string platform;
        std::string language;
        int screenWidth;
        int screenHeight;
        int devicePixelRatio;
        bool supportsWebGL;
        bool supportsWebGL2;
        bool supportsOffscreenCanvas;
        bool supportsWebWorkers;
        int maxTextureSize;
    };
    
    SystemInfo GetSystemInfo() const;
    
    void SetPreferredEngineType(EngineType type);
    void SetCanvasId(const std::string& id);
    
    bool IsInitialized() const { return m_initialized; }
    
    void SetOnResizeCallback(std::function<void(int, int)> callback);
    void SetOnVisibilityChangeCallback(std::function<void(bool)> callback);
    
    void ProcessEvents();

private:
    WebPlatform();
    ~WebPlatform();
    
    WebPlatform(const WebPlatform&) = delete;
    WebPlatform& operator=(const WebPlatform&) = delete;
    
    bool m_initialized = false;
    EngineType m_preferredEngineType = EngineType::WebGL;
    SystemInfo m_systemInfo;
    std::string m_canvasId = "canvas";
    
    std::function<void(int, int)> m_onResizeCallback;
    std::function<void(bool)> m_onVisibilityChangeCallback;
    
    static std::function<void(int, int)> s_resizeCallback;
    static std::function<void(bool)> s_visibilityCallback;
    
    void DetectSystemCapabilities();
    
    static EM_BOOL OnResize(int eventType, const EmscriptenUiEvent* uiEvent, void* userData);
    static EM_BOOL OnVisibilityChange(int eventType, const EmscriptenVisibilityChangeEvent* visibilityEvent, void* userData);
};

}
}

#endif

#endif
