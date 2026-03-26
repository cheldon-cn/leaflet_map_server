#ifndef OGC_DRAW_DRIVER_MANAGER_H
#define OGC_DRAW_DRIVER_MANAGER_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_driver.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/types.h"
#include <map>
#include <string>
#include <vector>
#include <mutex>

namespace ogc {
namespace draw {

class OGC_GRAPH_API DriverManager {
public:
    static DriverManager& Instance();
    
    DrawResult RegisterDriver(const std::string& name, DrawDriverPtr driver);
    DrawResult UnregisterDriver(const std::string& name);
    
    DrawDriverPtr GetDriver(const std::string& name) const;
    std::vector<std::string> GetDriverNames() const;
    bool HasDriver(const std::string& name) const;
    
    DrawResult RegisterDevice(const std::string& name, DrawDevicePtr device);
    DrawResult UnregisterDevice(const std::string& name);
    
    DrawDevicePtr GetDevice(const std::string& name) const;
    std::vector<std::string> GetDeviceNames() const;
    bool HasDevice(const std::string& name) const;
    
    DrawResult RegisterEngine(const std::string& name, DrawEnginePtr engine);
    DrawResult UnregisterEngine(const std::string& name);
    
    DrawEnginePtr GetEngine(const std::string& name) const;
    std::vector<std::string> GetEngineNames() const;
    bool HasEngine(const std::string& name) const;
    
    DrawDriverPtr CreateDriverForDevice(DeviceType deviceType);
    DrawDriverPtr CreateDriverForEngine(EngineType engineType);
    
    std::vector<DrawDriverPtr> GetDriversForDevice(DeviceType deviceType) const;
    std::vector<DrawDriverPtr> GetDriversForEngine(EngineType engineType) const;
    
    void Clear();
    size_t GetDriverCount() const;
    size_t GetDeviceCount() const;
    size_t GetEngineCount() const;
    
private:
    DriverManager() = default;
    ~DriverManager() = default;
    
    DriverManager(const DriverManager&) = delete;
    DriverManager& operator=(const DriverManager&) = delete;
    
    mutable std::mutex m_mutex;
    std::map<std::string, DrawDriverPtr> m_drivers;
    std::map<std::string, DrawDevicePtr> m_devices;
    std::map<std::string, DrawEnginePtr> m_engines;
};

}  
}  

#endif  
