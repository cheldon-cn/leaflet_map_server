#ifdef __APPLE__

#include "ogc/draw/macos_platform.h"
#include "ogc/draw/coregraphics_engine.h"
#include "ogc/draw/metal_engine.h"
#include "ogc/draw/simple2d_engine.h"

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>

namespace ogc {
namespace draw {

MacOSPlatform& MacOSPlatform::Instance() {
    static MacOSPlatform instance;
    return instance;
}

MacOSPlatform::MacOSPlatform() {
    m_preferredEngineType = EngineType::CoreGraphics;
}

MacOSPlatform::~MacOSPlatform() {
    Shutdown();
}

bool MacOSPlatform::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    DetectSystemCapabilities();
    
    if (m_systemInfo.supportsMetal) {
        m_preferredEngineType = EngineType::Metal;
    } else if (m_systemInfo.supportsCoreGraphics) {
        m_preferredEngineType = EngineType::CoreGraphics;
    } else {
        m_preferredEngineType = EngineType::Simple2D;
    }
    
    m_initialized = true;
    return true;
}

void MacOSPlatform::Shutdown() {
    m_initialized = false;
}

void MacOSPlatform::DetectSystemCapabilities() {
    m_systemInfo = SystemInfo();
    
    @autoreleasepool {
        NSProcessInfo* processInfo = [NSProcessInfo processInfo];
        NSOperatingSystemVersion version = [processInfo operatingSystemVersion];
        m_systemInfo.macOSVersion = std::to_string(version.majorVersion) + "." +
                                    std::to_string(version.minorVersion) + "." +
                                    std::to_string(version.patchVersion);
        
        id<MTLDevice> metalDevice = MTLCreateSystemDefaultDevice();
        m_systemInfo.supportsMetal = (metalDevice != nil);
        
        if (metalDevice) {
            m_systemInfo.metalVersion = "2.0";
            m_systemInfo.gpuMemoryMB = 0;
            [metalDevice release];
        }
        
        m_systemInfo.supportsCoreGraphics = true;
        
        NSArray<NSScreen*>* screens = [NSScreen screens];
        m_systemInfo.displayCount = static_cast<int>([screens count]);
        
        if ([screens count] > 0) {
            NSScreen* mainScreen = [screens objectAtIndex:0];
            NSRect frame = [mainScreen frame];
            m_systemInfo.primaryDisplayWidth = static_cast<int>(frame.size.width);
            m_systemInfo.primaryDisplayHeight = static_cast<int>(frame.size.height);
            
            CGFloat dpi = 72.0;
            if ([mainScreen respondsToSelector:@selector(backingScaleFactor)]) {
                dpi = 72.0 * [mainScreen backingScaleFactor];
            }
            m_systemInfo.primaryDisplayDpi = dpi;
        }
    }
}

bool MacOSPlatform::IsMetalAvailable() const {
    return m_systemInfo.supportsMetal;
}

bool MacOSPlatform::IsCoreGraphicsAvailable() const {
    return m_systemInfo.supportsCoreGraphics;
}

EngineType MacOSPlatform::GetPreferredEngineType() const {
    return m_preferredEngineType;
}

std::vector<EngineType> MacOSPlatform::GetSupportedEngineTypes() const {
    std::vector<EngineType> types;
    
    if (m_systemInfo.supportsMetal) {
        types.push_back(EngineType::Metal);
    }
    
    types.push_back(EngineType::CoreGraphics);
    types.push_back(EngineType::Simple2D);
    
    return types;
}

std::unique_ptr<DrawEngine> MacOSPlatform::CreatePreferredEngine(DrawDevice* device) {
    return CreateEngine(device, m_preferredEngineType);
}

std::unique_ptr<DrawEngine> MacOSPlatform::CreateEngine(DrawDevice* device, EngineType type) {
    switch (type) {
        case EngineType::Metal:
            if (m_systemInfo.supportsMetal) {
                auto engine = std::unique_ptr<MetalEngine>(new MetalEngine(device));
                if (engine->Initialize()) {
                    return engine;
                }
            }
            break;
            
        case EngineType::CoreGraphics:
            return std::unique_ptr<DrawEngine>(new CoreGraphicsEngine(device));
            
        case EngineType::Simple2D:
            return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
            
        default:
            break;
    }
    
    return std::unique_ptr<DrawEngine>(new Simple2DEngine(device));
}

MacOSPlatform::SystemInfo MacOSPlatform::GetSystemInfo() const {
    return m_systemInfo;
}

void MacOSPlatform::SetPreferredEngineType(EngineType type) {
    m_preferredEngineType = type;
}

}
}

#endif
