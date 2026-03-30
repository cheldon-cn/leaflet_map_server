#include "ogc/draw/windows_platform.h"

#ifdef _WIN32

#include "ogc/draw/gdiplus_engine.h"
#include "ogc/draw/direct2d_engine.h"
#include "ogc/draw/raster_image_device.h"

namespace ogc {
namespace draw {
namespace windows {

bool WindowsPlatform::s_initialized = false;
PlatformCapabilities WindowsPlatform::s_capabilities;

bool WindowsPlatform::Initialize() {
    if (s_initialized) {
        return true;
    }
    
    HRESULT d2dResult = Direct2DEngine::InitializeDirect2D();
    s_capabilities.hasDirect2D = SUCCEEDED(d2dResult);
    s_capabilities.hasDirectWrite = s_capabilities.hasDirect2D;
    
    s_capabilities.hasGDIPlus = GDIPlusEngine::IsGDIPlusAvailable();
    s_capabilities.hasHardwareAcceleration = s_capabilities.hasDirect2D;
    s_capabilities.maxTextureSize = 16384;
    
    if (s_capabilities.hasDirect2D) {
        s_capabilities.preferredEngine = "Direct2DEngine";
    } else if (s_capabilities.hasGDIPlus) {
        s_capabilities.preferredEngine = "GDIPlusEngine";
    } else {
        s_capabilities.preferredEngine = "";
    }
    
    s_initialized = true;
    return true;
}

void WindowsPlatform::Shutdown() {
    Direct2DEngine::ShutdownDirect2D();
    s_initialized = false;
}

std::unique_ptr<DrawEngine> WindowsPlatform::CreateOptimalEngine(DrawDevice* device) {
    if (!s_initialized) {
        Initialize();
    }
    
    if (s_capabilities.hasDirect2D) {
        auto engine = std::unique_ptr<DrawEngine>(new Direct2DEngine(device));
        if (engine) {
            return engine;
        }
    }
    
    if (s_capabilities.hasGDIPlus) {
        RasterImageDevice* rasterDevice = dynamic_cast<RasterImageDevice*>(device);
        if (rasterDevice) {
            return std::unique_ptr<DrawEngine>(new GDIPlusEngine(rasterDevice));
        }
    }
    
    return nullptr;
}

std::unique_ptr<DrawEngine> WindowsPlatform::CreateEngine(DrawDevice* device, 
                                                           const std::string& engineName) {
    if (!s_initialized) {
        Initialize();
    }
    
    if (engineName == "Direct2DEngine" && s_capabilities.hasDirect2D) {
        return std::unique_ptr<DrawEngine>(new Direct2DEngine(device));
    }
    
    if (engineName == "GDIPlusEngine" && s_capabilities.hasGDIPlus) {
        RasterImageDevice* rasterDevice = dynamic_cast<RasterImageDevice*>(device);
        if (rasterDevice) {
            return std::unique_ptr<DrawEngine>(new GDIPlusEngine(rasterDevice));
        }
    }
    
    return nullptr;
}

bool WindowsPlatform::IsDirect2DAvailable() {
    if (!s_initialized) {
        Initialize();
    }
    return s_capabilities.hasDirect2D;
}

bool WindowsPlatform::IsGDIPlusAvailable() {
    if (!s_initialized) {
        Initialize();
    }
    return s_capabilities.hasGDIPlus;
}

PlatformCapabilities WindowsPlatform::GetCapabilities() {
    if (!s_initialized) {
        Initialize();
    }
    return s_capabilities;
}

std::vector<std::string> WindowsPlatform::GetAvailableEngines() {
    if (!s_initialized) {
        Initialize();
    }
    
    std::vector<std::string> engines;
    if (s_capabilities.hasDirect2D) {
        engines.push_back("Direct2DEngine");
    }
    if (s_capabilities.hasGDIPlus) {
        engines.push_back("GDIPlusEngine");
    }
    return engines;
}

std::string WindowsPlatform::GetRecommendedEngine(DrawDevice* device) {
    if (!s_initialized) {
        Initialize();
    }
    
    if (!device) {
        return s_capabilities.preferredEngine;
    }
    
    DeviceType deviceType = device->GetType();
    
    if (deviceType == DeviceType::kRasterImage) {
        if (s_capabilities.hasDirect2D) {
            return "Direct2DEngine";
        }
        if (s_capabilities.hasGDIPlus) {
            return "GDIPlusEngine";
        }
    }
    
    if (deviceType == DeviceType::kDisplay) {
        if (s_capabilities.hasDirect2D) {
            return "Direct2DEngine";
        }
        if (s_capabilities.hasGDIPlus) {
            return "GDIPlusEngine";
        }
    }
    
    return s_capabilities.preferredEngine;
}

}

}

}

#endif
