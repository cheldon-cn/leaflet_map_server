#ifdef __EMSCRIPTEN__

#include "ogc/draw/web_platform.h"
#include "ogc/draw/webgl_engine.h"
#include "ogc/draw/simple2d_engine.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <cstdlib>

namespace ogc {
namespace draw {

std::function<void(int, int)> WebPlatform::s_resizeCallback;
std::function<void(bool)> WebPlatform::s_visibilityCallback;

WebPlatform& WebPlatform::Instance() {
    static WebPlatform instance;
    return instance;
}

WebPlatform::WebPlatform() {
}

WebPlatform::~WebPlatform() {
    Shutdown();
}

bool WebPlatform::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    DetectSystemCapabilities();
    
    if (m_systemInfo.supportsWebGL) {
        m_preferredEngineType = EngineType::WebGL;
    } else {
        m_preferredEngineType = EngineType::Simple2D;
    }
    
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, EM_FALSE, OnResize);
    emscripten_set_visibilitychange_callback(this, EM_FALSE, OnVisibilityChange);
    
    m_initialized = true;
    return true;
}

void WebPlatform::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, nullptr);
    emscripten_set_visibilitychange_callback(nullptr, EM_FALSE, nullptr);
    
    m_initialized = false;
}

void WebPlatform::DetectSystemCapabilities() {
    m_systemInfo = SystemInfo();
    
    m_systemInfo.screenWidth = EM_ASM_INT({
        return screen.width;
    });
    
    m_systemInfo.screenHeight = EM_ASM_INT({
        return screen.height;
    });
    
    m_systemInfo.devicePixelRatio = EM_ASM_INT({
        return window.devicePixelRatio || 1;
    });
    
    m_systemInfo.supportsWebGL = EM_ASM_INT({
        var canvas = document.createElement('canvas');
        var gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
        return gl ? 1 : 0;
    }) != 0;
    
    m_systemInfo.supportsWebGL2 = EM_ASM_INT({
        var canvas = document.createElement('canvas');
        var gl = canvas.getContext('webgl2');
        return gl ? 1 : 0;
    }) != 0;
    
    m_systemInfo.supportsOffscreenCanvas = EM_ASM_INT({
        return typeof OffscreenCanvas !== 'undefined' ? 1 : 0;
    }) != 0;
    
    m_systemInfo.supportsWebWorkers = EM_ASM_INT({
        return typeof Worker !== 'undefined' ? 1 : 0;
    }) != 0;
    
    m_systemInfo.maxTextureSize = 4096;
}

bool WebPlatform::IsWebGLAvailable() const {
    return m_systemInfo.supportsWebGL;
}

bool WebPlatform::IsWebGL2Available() const {
    return m_systemInfo.supportsWebGL2;
}

EngineType WebPlatform::GetPreferredEngineType() const {
    return m_preferredEngineType;
}

std::vector<EngineType> WebPlatform::GetSupportedEngineTypes() const {
    std::vector<EngineType> types;
    
    if (m_systemInfo.supportsWebGL) {
        types.push_back(EngineType::WebGL);
    }
    
    types.push_back(EngineType::Simple2D);
    
    return types;
}

std::unique_ptr<DrawEngine> WebPlatform::CreatePreferredEngine(DrawDevice* device) {
    return CreateEngine(device, m_preferredEngineType);
}

std::unique_ptr<DrawEngine> WebPlatform::CreateEngine(DrawDevice* device, EngineType type) {
    switch (type) {
        case EngineType::WebGL:
            if (m_systemInfo.supportsWebGL) {
                auto engine = std::unique_ptr<WebGLEngine>(new WebGLEngine(device));
                if (engine->Initialize()) {
                    return engine;
                }
            }
            break;
            
        case EngineType::Simple2D:
            return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
            
        default:
            break;
    }
    
    return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
}

WebPlatform::SystemInfo WebPlatform::GetSystemInfo() const {
    return m_systemInfo;
}

void WebPlatform::SetPreferredEngineType(EngineType type) {
    m_preferredEngineType = type;
}

void WebPlatform::SetCanvasId(const std::string& id) {
    m_canvasId = id;
}

void WebPlatform::SetOnResizeCallback(std::function<void(int, int)> callback) {
    m_onResizeCallback = callback;
}

void WebPlatform::SetOnVisibilityChangeCallback(std::function<void(bool)> callback) {
    m_onVisibilityChangeCallback = callback;
}

void WebPlatform::ProcessEvents() {
}

EM_BOOL WebPlatform::OnResize(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
    WebPlatform* platform = static_cast<WebPlatform*>(userData);
    
    if (platform && platform->m_onResizeCallback) {
        platform->m_onResizeCallback(uiEvent->windowInnerWidth, uiEvent->windowInnerHeight);
    }
    
    if (s_resizeCallback) {
        s_resizeCallback(uiEvent->windowInnerWidth, uiEvent->windowInnerHeight);
    }
    
    return EM_FALSE;
}

EM_BOOL WebPlatform::OnVisibilityChange(int eventType, const EmscriptenVisibilityChangeEvent* visibilityEvent, void* userData) {
    WebPlatform* platform = static_cast<WebPlatform*>(userData);
    
    bool isVisible = !visibilityEvent->hidden;
    
    if (platform && platform->m_onVisibilityChangeCallback) {
        platform->m_onVisibilityChangeCallback(isVisible);
    }
    
    if (s_visibilityCallback) {
        s_visibilityCallback(isVisible);
    }
    
    return EM_FALSE;
}

}
}

#endif
