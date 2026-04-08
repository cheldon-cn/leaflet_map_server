#ifndef OGC_DRAW_DEVICE_PLUGIN_H
#define OGC_DRAW_DEVICE_PLUGIN_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"
#include <string>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API DrawDevicePlugin {
public:
    virtual ~DrawDevicePlugin() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual DeviceType GetSupportedDeviceType() const = 0;
    
    virtual std::unique_ptr<DrawDevice> CreateDevice(int width, int height) = 0;
    virtual std::unique_ptr<DrawDevice> CreateDevice(void* nativeHandle, int width, int height) = 0;
    
    virtual bool IsAvailable() const = 0;
    virtual std::string GetRequiredDependency() const { return ""; }
};

}

}

#endif
