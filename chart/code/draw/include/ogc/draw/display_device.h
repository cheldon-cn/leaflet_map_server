#ifndef OGC_DRAW_DISPLAY_DEVICE_H
#define OGC_DRAW_DISPLAY_DEVICE_H

#include <ogc/draw/raster_image_device.h>
#include <ogc/draw/draw_types.h>
#include <ogc/draw/export.h>
#include <memory>
#include <vector>

namespace ogc {
namespace draw {

class OGC_DRAW_API DisplayDevice : public RasterImageDevice {
public:
    explicit DisplayDevice(void* nativeWindow);
    ~DisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    std::string GetName() const override { return "DisplayDevice"; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    
    DrawResult Initialize() override;
    DrawResult Finalize() override;
    
    void Present();
    void SetVSync(bool enabled);
    bool IsVSyncEnabled() const { return m_vsync; }
    
    void InvalidateRect(const Rect& rect);
    void InvalidateAll();
    std::vector<Rect> GetDirtyRects() const;
    void ClearDirtyRects();
    
    void* GetNativeWindow() const { return m_nativeWindow; }
    
    void Resize(int width, int height);
    
private:
    void UpdateWindowSize();
    
    void* m_nativeWindow = nullptr;
    bool m_vsync = true;
    
    std::vector<Rect> m_dirtyRects;
};

}
}

#endif
