#include "android_adapter/android_engine.h"

#ifdef __ANDROID__

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdexcept>
#include <cstring>

namespace ogc {
namespace android {

AndroidEngine* AndroidEngine::s_instance = nullptr;

AndroidEngine* AndroidEngine::GetInstance() {
    if (!s_instance) {
        s_instance = new AndroidEngine();
    }
    return s_instance;
}

AndroidEngine::AndroidEngine() 
    : m_initialized(false)
    , m_display(EGL_NO_DISPLAY)
    , m_surface(EGL_NO_SURFACE)
    , m_context(EGL_NO_CONTEXT)
    , m_config(nullptr) {
}

AndroidEngine::~AndroidEngine() {
    Shutdown();
}

bool AndroidEngine::Initialize(const AndroidEngineConfig& config) {
    if (m_initialized) {
        return true;
    }
    
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY) {
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(m_display, &major, &minor)) {
        return false;
    }
    
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, config.colorDepth / 4,
        EGL_GREEN_SIZE, config.colorDepth / 4,
        EGL_BLUE_SIZE, config.colorDepth / 4,
        EGL_ALPHA_SIZE, config.colorDepth / 4,
        EGL_DEPTH_SIZE, config.depthBuffer,
        EGL_STENCIL_SIZE, config.stencilBuffer,
        EGL_SAMPLE_BUFFERS, config.multisample ? 1 : 0,
        EGL_SAMPLES, config.multisample ? config.sampleCount : 0,
        EGL_NONE
    };
    
    EGLint numConfigs;
    if (!eglChooseConfig(m_display, attribs, &m_config, 1, &numConfigs) || numConfigs < 1) {
        eglTerminate(m_display);
        return false;
    }
    
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, contextAttribs);
    if (m_context == EGL_NO_CONTEXT) {
        eglTerminate(m_display);
        return false;
    }
    
    m_config.width = config.width;
    m_config.height = config.height;
    m_initialized = true;
    
    return true;
}

void AndroidEngine::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    if (m_surface != EGL_NO_SURFACE) {
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
    
    if (m_context != EGL_NO_CONTEXT) {
        eglDestroyContext(m_display, m_context);
        m_context = EGL_NO_CONTEXT;
    }
    
    if (m_display != EGL_NO_DISPLAY) {
        eglTerminate(m_display);
        m_display = EGL_NO_DISPLAY;
    }
    
    m_initialized = false;
}

bool AndroidEngine::CreateSurface(ANativeWindow* window) {
    if (!m_initialized || !window) {
        return false;
    }
    
    if (m_surface != EGL_NO_SURFACE) {
        eglDestroySurface(m_display, m_surface);
    }
    
    m_surface = eglCreateWindowSurface(m_display, m_config, window, nullptr);
    if (m_surface == EGL_NO_SURFACE) {
        return false;
    }
    
    if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
        return false;
    }
    
    return true;
}

void AndroidEngine::DestroySurface() {
    if (m_surface != EGL_NO_SURFACE) {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
}

bool AndroidEngine::BeginFrame() {
    if (!m_initialized || m_surface == EGL_NO_SURFACE) {
        return false;
    }
    
    if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
        return false;
    }
    
    return true;
}

void AndroidEngine::EndFrame() {
}

void AndroidEngine::SwapBuffers() {
    if (m_initialized && m_surface != EGL_NO_SURFACE) {
        eglSwapBuffers(m_display, m_surface);
    }
}

void AndroidEngine::Resize(int width, int height) {
    m_config.width = width;
    m_config.height = height;
    glViewport(0, 0, width, height);
}

void AndroidEngine::SetClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void AndroidEngine::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void AndroidEngine::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

GLuint AndroidEngine::CreateShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint AndroidEngine::CreateProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentSource);
    
    if (!vertexShader || !fragmentShader) {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void AndroidEngine::DeleteShader(GLuint shader) {
    glDeleteShader(shader);
}

void AndroidEngine::DeleteProgram(GLuint program) {
    glDeleteProgram(program);
}

}  
}  

#endif
