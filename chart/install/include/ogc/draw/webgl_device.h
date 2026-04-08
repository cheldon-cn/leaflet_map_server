#ifndef OGC_DRAW_WEBGL_DEVICE_H
#define OGC_DRAW_WEBGL_DEVICE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/html5.h>
#include <string>

namespace ogc {
namespace draw {

class OGC_DRAW_API WebGLDevice : public DrawDevice {
public:
    WebGLDevice();
    WebGLDevice(const std::string& canvasId);
    WebGLDevice(int width, int height);
    ~WebGLDevice() override;
    
    DeviceType GetType() const override { return DeviceType::Display; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return m_dpi; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override;
    DeviceCapabilities GetCapabilities() const override;
    
    bool Initialize();
    void Shutdown();
    
    void SetCanvasId(const std::string& id);
    std::string GetCanvasId() const { return m_canvasId; }
    
    void Resize(int width, int height);
    void SetDpi(double dpi) { m_dpi = dpi; }
    
    void SwapBuffers();
    void MakeCurrent();
    
    bool IsInitialized() const { return m_initialized; }
    
    static void SetContextLostCallback(std::function<void()> callback);
    static void SetContextRestoredCallback(std::function<void()> callback);

private:
    std::string m_canvasId;
    int m_width = 800;
    int m_height = 600;
    double m_dpi = 96.0;
    bool m_initialized = false;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_context = 0;
    
    static std::function<void()> s_contextLostCallback;
    static std::function<void()> s_contextRestoredCallback;
    
    static EM_BOOL OnContextLost(int eventType, const void* reserved, void* userData);
    static EM_BOOL OnContextRestored(int eventType, const void* reserved, void* userData);
    
    bool CreateContext();
    void DestroyContext();
};

}
}

#endif

#endif
