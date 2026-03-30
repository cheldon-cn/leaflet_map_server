#ifdef OGC_HAS_X11

#include "ogc/draw/x11_display_device.h"
#include "ogc/draw/simple2d_engine.h"
#include <cstring>

namespace ogc {
namespace draw {

X11DisplayDevice::X11DisplayDevice() {
}

X11DisplayDevice::X11DisplayDevice(int width, int height) 
    : m_width(width), m_height(height) {
}

X11DisplayDevice::~X11DisplayDevice() {
    Shutdown();
}

bool X11DisplayDevice::Initialize() {
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        return false;
    }
    
    m_screen = DefaultScreen(m_display);
    CreateWindow();
    
    return m_isOpen;
}

bool X11DisplayDevice::Initialize(Window window) {
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        return false;
    }
    
    m_screen = DefaultScreen(m_display);
    m_window = window;
    m_ownsWindow = false;
    m_isOpen = true;
    
    XWindowAttributes attrs;
    if (XGetWindowAttributes(m_display, m_window, &attrs)) {
        m_width = attrs.width;
        m_height = attrs.height;
    }
    
    return true;
}

void X11DisplayDevice::CreateWindow() {
    Window root = RootWindow(m_display, m_screen);
    
    unsigned long blackPixel = BlackPixel(m_display, m_screen);
    unsigned long whitePixel = WhitePixel(m_display, m_screen);
    
    m_window = XCreateSimpleWindow(
        m_display,
        root,
        0, 0,
        m_width, m_height,
        0,
        blackPixel,
        whitePixel
    );
    
    if (!m_window) {
        m_isOpen = false;
        return;
    }
    
    XSelectInput(m_display, m_window, 
                 ExposureMask | KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask | 
                 StructureNotifyMask);
    
    XMapWindow(m_display, m_window);
    XFlush(m_display);
    
    m_ownsWindow = true;
    m_isOpen = true;
}

void X11DisplayDevice::DestroyWindow() {
    if (m_window && m_ownsWindow) {
        XDestroyWindow(m_display, m_window);
        m_window = 0;
    }
}

void X11DisplayDevice::Shutdown() {
    if (m_display) {
        DestroyWindow();
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
    m_isOpen = false;
}

std::unique_ptr<DrawEngine> X11DisplayDevice::CreateEngine() {
    return std::unique_ptr<DrawEngine>(new Simple2DEngine(this));
}

std::vector<EngineType> X11DisplayDevice::GetSupportedEngineTypes() const {
    return {EngineType::Simple2D, EngineType::Cairo};
}

EngineType X11DisplayDevice::GetPreferredEngineType() const {
#ifdef OGC_HAS_CAIRO
    return EngineType::Cairo;
#else
    return EngineType::Simple2D;
#endif
}

DeviceCapabilities X11DisplayDevice::GetCapabilities() const {
    DeviceCapabilities caps;
    caps.hasGpuAcceleration = false;
    caps.hasDoubleBuffer = true;
    caps.hasAlphaChannel = true;
    caps.maxTextureSize = 4096;
    caps.supportedPixelFormats = {PixelFormat::RGBA32, PixelFormat::RGB24};
    return caps;
}

void X11DisplayDevice::SetTitle(const std::string& title) {
    if (m_display && m_window) {
        XStoreName(m_display, m_window, title.c_str());
        XFlush(m_display);
    }
}

void X11DisplayDevice::ProcessEvents() {
    if (!m_display) {
        return;
    }
    
    XEvent event;
    while (XPending(m_display) > 0) {
        XNextEvent(m_display, &event);
        
        switch (event.type) {
            case Expose:
                break;
            case ConfigureNotify:
                m_width = event.xconfigure.width;
                m_height = event.xconfigure.height;
                break;
            case DestroyNotify:
                m_isOpen = false;
                break;
        }
    }
}

void X11DisplayDevice::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    
    if (m_display && m_window) {
        XResizeWindow(m_display, m_window, width, height);
        XFlush(m_display);
    }
}

}
}

#endif
