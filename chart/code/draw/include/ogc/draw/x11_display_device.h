#ifndef OGC_DRAW_X11_DISPLAY_DEVICE_H
#define OGC_DRAW_X11_DISPLAY_DEVICE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"

#ifdef OGC_HAS_X11

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace ogc {
namespace draw {

class OGC_DRAW_API X11DisplayDevice : public DrawDevice {
public:
    X11DisplayDevice();
    X11DisplayDevice(int width, int height);
    ~X11DisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::Display; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return m_dpi; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override;
    DeviceCapabilities GetCapabilities() const override;
    
    bool Initialize();
    bool Initialize(Window window);
    void Shutdown();
    
    Display* GetDisplay() const { return m_display; }
    Window GetWindow() const { return m_window; }
    int GetScreen() const { return m_screen; }
    
    void SetTitle(const std::string& title);
    void ProcessEvents();
    bool IsOpen() const { return m_isOpen; }
    
    void Resize(int width, int height);
    void SetDpi(double dpi) { m_dpi = dpi; }

private:
    Display* m_display = nullptr;
    Window m_window = 0;
    int m_screen = 0;
    int m_width = 800;
    int m_height = 600;
    double m_dpi = 96.0;
    bool m_isOpen = false;
    bool m_ownsWindow = false;
    
    void CreateWindow();
    void DestroyWindow();
};

}
}

#endif

#endif
