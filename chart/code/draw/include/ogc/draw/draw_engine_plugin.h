#ifndef OGC_DRAW_ENGINE_PLUGIN_H
#define OGC_DRAW_ENGINE_PLUGIN_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include <string>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API DrawEnginePlugin {
public:
    virtual ~DrawEnginePlugin() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual EngineType GetEngineType() const = 0;
    
    virtual std::unique_ptr<DrawEngine> CreateEngine(DrawDevice* device) = 0;
    
    virtual bool IsAvailable() const = 0;
    
    virtual std::vector<std::string> GetSupportedDeviceTypes() const = 0;
    
    virtual int GetPriority() const { return 0; }
    
    virtual bool SupportsDevice(DeviceType deviceType) const {
        auto supported = GetSupportedDeviceTypes();
        for (const auto& type : supported) {
            if (type == "RasterImage" && deviceType == DeviceType::kRasterImage) return true;
            if (type == "Display" && deviceType == DeviceType::kDisplay) return true;
            if (type == "Pdf" && deviceType == DeviceType::kPdf) return true;
            if (type == "Svg" && deviceType == DeviceType::kSvg) return true;
            if (type == "Tile" && deviceType == DeviceType::kTile) return true;
        }
        return false;
    }
};

}

}

#endif
