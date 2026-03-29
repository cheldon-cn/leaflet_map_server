#ifndef OGC_DRAW_DEVICE_H
#define OGC_DRAW_DEVICE_H

#include "ogc/draw/device_type.h"
#include "ogc/draw/engine_type.h"
#include "ogc/draw/draw_result.h"
#include "ogc/draw/draw_types.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdint>

namespace ogc {
namespace draw {

class DrawEngine;

enum class DeviceState {
    kUninitialized = 0,
    kReady = 1,
    kActive = 2,
    kError = 3,
    kLost = 4
};

inline const char* DeviceStateToString(DeviceState state) {
    switch (state) {
        case DeviceState::kUninitialized: return "Uninitialized";
        case DeviceState::kReady:         return "Ready";
        case DeviceState::kActive:        return "Active";
        case DeviceState::kError:         return "Error";
        case DeviceState::kLost:          return "Lost";
        default:                          return "Unknown";
    }
}

struct DeviceCapabilities {
    bool supportsGPU;
    bool supportsMultithreading;
    bool supportsVSync;
    bool supportsDoubleBuffer;
    bool supportsPartialUpdate;
    bool supportsAlpha;
    bool supportsAntialiasing;
    int maxTextureSize;
    int maxRenderTargets;
    std::vector<std::string> supportedFormats;

    DeviceCapabilities()
        : supportsGPU(false)
        , supportsMultithreading(false)
        , supportsVSync(false)
        , supportsDoubleBuffer(false)
        , supportsPartialUpdate(false)
        , supportsAlpha(true)
        , supportsAntialiasing(true)
        , maxTextureSize(4096)
        , maxRenderTargets(1) {}
};

class DrawDevice {
public:
    virtual ~DrawDevice() = default;

    DrawDevice(const DrawDevice&) = delete;
    DrawDevice& operator=(const DrawDevice&) = delete;
    DrawDevice(DrawDevice&&) = default;
    DrawDevice& operator=(DrawDevice&&) = default;

protected:
    DrawDevice() = default;

public:
    virtual DeviceType GetType() const = 0;
    virtual std::string GetName() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual double GetDpi() const = 0;
    virtual void SetDpi(double dpi) = 0;
    virtual int GetColorDepth() const = 0;

    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    virtual DeviceState GetState() const = 0;
    virtual bool IsReady() const = 0;

    virtual std::unique_ptr<DrawEngine> CreateEngine() = 0;
    virtual std::vector<EngineType> GetSupportedEngineTypes() const = 0;
    virtual void SetPreferredEngineType(EngineType type) = 0;
    virtual EngineType GetPreferredEngineType() const = 0;

    virtual DeviceCapabilities QueryCapabilities() const = 0;
    virtual bool IsFeatureAvailable(const std::string& featureName) const = 0;

    virtual bool IsDeviceLost() const = 0;
    virtual DrawResult RecoverDevice() = 0;

    virtual Size GetSize() const {
        return Size(static_cast<double>(GetWidth()), 
                    static_cast<double>(GetHeight()));
    }

    virtual Rect GetBounds() const {
        return Rect(0, 0, static_cast<double>(GetWidth()), 
                    static_cast<double>(GetHeight()));
    }

    virtual bool IsValid() const {
        return GetWidth() > 0 && GetHeight() > 0 && IsReady();
    }
};

}  
}  

#endif  
