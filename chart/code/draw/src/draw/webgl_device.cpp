#ifdef __EMSCRIPTEN__

#include "ogc/draw/webgl_device.h"
#include "ogc/draw/webgl_engine.h"
#include "ogc/draw/simple2d_engine.h"
#include <emscripten/html5.h>

namespace ogc {
namespace draw {

std::function<void()> WebGLDevice::s_contextLostCallback;
std::function<void()> WebGLDevice::s_contextRestoredCallback;

WebGLDevice::WebGLDevice() {
}

WebGLDevice::WebGLDevice(const std::string& canvasId) 
    : m_canvasId(canvasId) {
}

WebGLDevice::WebGLDevice(int width, int height) 
    : m_width(width), m_height(height) {
}

WebGLDevice::~WebGLDevice() {
    Shutdown();
}

bool WebGLDevice::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    if (!CreateContext()) {
        return false;
    }
    
    emscripten_set_webglcontextlost_callback(
        m_canvasId.c_str(), 
        this, 
        EM_FALSE, 
        OnContextLost
    );
    
    emscripten_set_webglcontextrestored_callback(
        m_canvasId.c_str(), 
        this, 
        EM_FALSE, 
        OnContextRestored
    );
    
    m_initialized = true;
    return true;
}

void WebGLDevice::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    DestroyContext();
    
    emscripten_set_webglcontextlost_callback(
        m_canvasId.c_str(), 
        nullptr, 
        EM_FALSE, 
        nullptr
    );
    
    emscripten_set_webglcontextrestored_callback(
        m_canvasId.c_str(), 
        nullptr, 
        EM_FALSE, 
        nullptr
    );
    
    m_initialized = false;
}

bool WebGLDevice::CreateContext() {
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    
    attrs.alpha = EM_TRUE;
    attrs.depth = EM_FALSE;
    attrs.stencil = EM_FALSE;
    attrs.antialias = EM_TRUE;
    attrs.premultipliedAlpha = EM_TRUE;
    attrs.preserveDrawingBuffer = EM_FALSE;
    attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;
    attrs.failIfMajorPerformanceCaveat = EM_FALSE;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.enableExtensionsByDefault = EM_TRUE;
    
    const char* canvasId = m_canvasId.empty() ? "#canvas" : m_canvasId.c_str();
    
    m_context = emscripten_webgl_create_context(canvasId, &attrs);
    
    if (m_context <= 0) {
        attrs.majorVersion = 1;
        m_context = emscripten_webgl_create_context(canvasId, &attrs);
        
        if (m_context <= 0) {
            return false;
        }
    }
    
    emscripten_webgl_make_context_current(m_context);
    
    double width, height;
    emscripten_get_element_css_size(canvasId, &width, &height);
    m_width = static_cast<int>(width);
    m_height = static_cast<int>(height);
    
    return true;
}

void WebGLDevice::DestroyContext() {
    if (m_context > 0) {
        emscripten_webgl_destroy_context(m_context);
        m_context = 0;
    }
}

void WebGLDevice::SetCanvasId(const std::string& id) {
    if (m_initialized) {
        Shutdown();
    }
    m_canvasId = id;
}

std::unique_ptr<DrawEngine> WebGLDevice::CreateEngine() {
    auto engine = std::unique_ptr<WebGLEngine>(new WebGLEngine(this));
    if (engine->Initialize()) {
        engine->SetViewport(m_width, m_height);
        return engine;
    }
    return std::unique_ptr<DrawEngine>(new Simple2DEngine(this));
}

std::vector<EngineType> WebGLDevice::GetSupportedEngineTypes() const {
    return {EngineType::WebGL, EngineType::Simple2D};
}

EngineType WebGLDevice::GetPreferredEngineType() const {
    return EngineType::WebGL;
}

DeviceCapabilities WebGLDevice::GetCapabilities() const {
    DeviceCapabilities caps;
    caps.hasGpuAcceleration = true;
    caps.hasDoubleBuffer = true;
    caps.hasAlphaChannel = true;
    caps.maxTextureSize = 4096;
    caps.supportedPixelFormats = {PixelFormat::RGBA32};
    return caps;
}

void WebGLDevice::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    
    if (m_initialized && m_context > 0) {
        emscripten_webgl_make_context_current(m_context);
    }
}

void WebGLDevice::SwapBuffers() {
}

void WebGLDevice::MakeCurrent() {
    if (m_context > 0) {
        emscripten_webgl_make_context_current(m_context);
    }
}

void WebGLDevice::SetContextLostCallback(std::function<void()> callback) {
    s_contextLostCallback = callback;
}

void WebGLDevice::SetContextRestoredCallback(std::function<void()> callback) {
    s_contextRestoredCallback = callback;
}

EM_BOOL WebGLDevice::OnContextLost(int eventType, const void* reserved, void* userData) {
    if (s_contextLostCallback) {
        s_contextLostCallback();
    }
    return EM_TRUE;
}

EM_BOOL WebGLDevice::OnContextRestored(int eventType, const void* reserved, void* userData) {
    WebGLDevice* device = static_cast<WebGLDevice*>(userData);
    if (device) {
        device->CreateContext();
    }
    
    if (s_contextRestoredCallback) {
        s_contextRestoredCallback();
    }
    return EM_TRUE;
}

}
}

#endif
