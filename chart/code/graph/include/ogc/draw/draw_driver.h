#ifndef OGC_DRAW_DRAW_DRIVER_H
#define OGC_DRAW_DRAW_DRIVER_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_params.h"
#include "ogc/draw/types.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <functional>

namespace ogc {
namespace draw {

using ProgressCallback = std::function<void(double progress, const std::string& message)>;

class OGC_GRAPH_API DrawDriver {
public:
    virtual ~DrawDriver() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual DeviceType GetSupportedDeviceType() const = 0;
    virtual EngineType GetSupportedEngineType() const = 0;
    
    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    
    virtual bool IsInitialized() const = 0;
    
    virtual DrawResult SetDevice(DrawDevicePtr device) = 0;
    virtual DrawDevicePtr GetDevice() const = 0;
    
    virtual DrawResult SetEngine(DrawEnginePtr engine) = 0;
    virtual DrawEnginePtr GetEngine() const = 0;
    
    virtual DrawResult BeginDraw(const DrawParams& params) = 0;
    virtual DrawResult EndDraw() = 0;
    virtual DrawResult AbortDraw() = 0;
    
    virtual bool IsDrawing() const = 0;
    
    virtual DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) = 0;
    
    virtual DrawResult Render(const DrawParams& params, ProgressCallback callback = nullptr) = 0;
    
    virtual void SetProgressCallback(ProgressCallback callback) = 0;
    
    virtual bool CanHandleDevice(DeviceType deviceType) const = 0;
    virtual bool CanHandleEngine(EngineType engineType) const = 0;
    
    virtual bool SupportsAsyncRendering() const { return false; }
    virtual bool SupportsCancellation() const { return false; }
    
    virtual void Cancel() = 0;
    virtual bool IsCancelled() const = 0;
    
    static DrawDriverPtr Create(const std::string& name);
};

}  
}  

#endif  
