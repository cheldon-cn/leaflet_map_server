#ifndef OGC_DRAW_RASTER_IMAGE_DEVICE_H
#define OGC_DRAW_RASTER_IMAGE_DEVICE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_types.h"
#include <memory>
#include <vector>
#include <string>

namespace ogc {
namespace draw {

enum class PixelFormat {
    kUnknown = 0,
    kGray8 = 1,
    kRGB888 = 3,
    kRGBA8888 = 4,
    kBGRA8888 = 5
};

enum class ImageFormat {
    kPNG,
    kJPEG,
    kBMP,
    kAuto
};

class OGC_DRAW_API RasterImageDevice : public DrawDevice {
public:
    RasterImageDevice(int width, int height, PixelFormat format = PixelFormat::kRGBA8888);
    ~RasterImageDevice() override;

    DeviceType GetType() const override { return DeviceType::kRasterImage; }
    std::string GetName() const override { return "RasterImageDevice"; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return m_dpi; }
    void SetDpi(double dpi) override { m_dpi = dpi; }
    int GetColorDepth() const override { return m_bytesPerPixel * 8; }

    DrawResult Initialize() override;
    DrawResult Finalize() override;
    DeviceState GetState() const override { return m_state; }
    bool IsReady() const override { return m_state == DeviceState::kReady && IsValid(); }

    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    void SetPreferredEngineType(EngineType type) override { m_preferredEngineType = type; }

    DeviceCapabilities QueryCapabilities() const override;
    bool IsFeatureAvailable(const std::string& featureName) const override;

    bool IsDeviceLost() const override { return m_state == DeviceState::kLost; }
    DrawResult RecoverDevice() override;

    uint8_t* GetPixelData() { return m_pixelData.get(); }
    const uint8_t* GetPixelData() const { return m_pixelData.get(); }
    int GetStride() const { return m_stride; }
    PixelFormat GetPixelFormat() const { return m_format; }
    int GetBytesPerPixel() const { return m_bytesPerPixel; }

    bool SaveToFile(const std::string& path, ImageFormat format = ImageFormat::kAuto);
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);

    void Clear(const Color& color = Color());

    bool IsValid() const { return m_pixelData != nullptr && m_width > 0 && m_height > 0; }
    size_t GetDataSize() const { return static_cast<size_t>(m_stride) * m_height; }

protected:
    void SetDimensions(int width, int height) {
        m_width = width;
        m_height = height;
        m_stride = width * m_bytesPerPixel;
    }
    
    void AllocateBuffer();

private:
    int GetBytesPerPixel(PixelFormat format) const;

    int m_width;
    int m_height;
    int m_stride;
    int m_bytesPerPixel;
    double m_dpi;
    PixelFormat m_format;
    std::unique_ptr<uint8_t[]> m_pixelData;
    DeviceState m_state;
    EngineType m_preferredEngineType;
};

}  
}  

#endif  
