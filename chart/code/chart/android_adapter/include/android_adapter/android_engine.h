#ifndef OGC_ANDROID_ADAPTER_ANDROID_ENGINE_H
#define OGC_ANDROID_ADAPTER_ANDROID_ENGINE_H

#include "android_adapter/export.h"

#ifdef __ANDROID__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <string>
#include <memory>
#include <functional>

namespace ogc {
namespace android {

struct AndroidEngineConfig {
    int width = 0;
    int height = 0;
    int colorDepth = 24;
    int depthBuffer = 16;
    int stencilBuffer = 0;
    bool multisample = false;
    int sampleCount = 0;
};

struct AndroidRenderContext {
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    EGLConfig config = nullptr;
    int width = 0;
    int height = 0;
};

class OGC_ANDROID_ADAPTER_API AndroidEngine {
public:
    using RenderCallback = std::function<void()>;
    using ResizeCallback = std::function<void(int, int)>;
    
    static AndroidEngine* GetInstance();
    
    bool Initialize(const AndroidEngineConfig& config);
    void Shutdown();
    
    bool BeginFrame();
    void EndFrame();
    void SwapBuffers();
    
    void SetRenderCallback(RenderCallback callback);
    void SetResizeCallback(ResizeCallback callback);
    
    void Resize(int width, int height);
    
    int GetWidth() const { return m_context.width; }
    int GetHeight() const { return m_context.height; }
    bool IsInitialized() const { return m_initialized; }
    
    GLuint CreateShader(GLenum type, const std::string& source);
    GLuint CreateProgram(const std::string& vertexSource, const std::string& fragmentSource);
    void DeleteShader(GLuint shader);
    void DeleteProgram(GLuint program);
    
    void SetClearColor(float r, float g, float b, float a);
    void Clear();
    void SetViewport(int x, int y, int width, int height);
    
    const AndroidRenderContext& GetContext() const { return m_context; }
    
private:
    AndroidEngine() = default;
    ~AndroidEngine() = default;
    
    AndroidEngine(const AndroidEngine&) = delete;
    AndroidEngine& operator=(const AndroidEngine&) = delete;
    
    bool InitializeEGL();
    void DestroyEGL();
    bool ChooseEGLConfig();
    
    static AndroidEngine* s_instance;
    
    AndroidRenderContext m_context;
    AndroidEngineConfig m_config;
    RenderCallback m_renderCallback;
    ResizeCallback m_resizeCallback;
    bool m_initialized = false;
    float m_clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};

}  
}  

#endif

#endif
