#ifndef OGC_DRAW_PLUGIN_MANAGER_H
#define OGC_DRAW_PLUGIN_MANAGER_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_engine_plugin.h"
#include "ogc/draw/draw_device_plugin.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace ogc {
namespace draw {

class DrawEnginePlugin;
class DrawDevicePlugin;

struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string path;
    void* handle;
    DrawEnginePlugin* enginePlugin;
    DrawDevicePlugin* devicePlugin;
};

class OGC_DRAW_API PluginManager {
public:
    static PluginManager& Instance();
    
    bool LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& name);
    void UnloadAll();
    
    void ScanPluginDirectory(const std::string& directory);
    
    std::vector<std::string> GetLoadedPlugins() const;
    
    DrawEnginePlugin* GetEnginePlugin(const std::string& name);
    DrawDevicePlugin* GetDevicePlugin(const std::string& name);
    
    std::vector<DrawEnginePlugin*> GetAvailableEnginePlugins();
    std::vector<DrawDevicePlugin*> GetAvailableDevicePlugins();
    
    std::unique_ptr<DrawEngine> CreateEngine(const std::string& pluginName, DrawDevice* device);
    std::unique_ptr<DrawDevice> CreateDevice(const std::string& pluginName, int width, int height);
    
    bool HasPlugin(const std::string& name) const;
    size_t GetPluginCount() const;
    
    using EngineFactory = std::function<std::unique_ptr<DrawEngine>()>;
    using DeviceFactory = std::function<std::unique_ptr<DrawDevice>()>;
    
    void RegisterEngineFactory(const std::string& name, EngineFactory factory);
    void RegisterDeviceFactory(const std::string& name, DeviceFactory factory);
    
    void UnregisterEngineFactory(const std::string& name);
    void UnregisterDeviceFactory(const std::string& name);

private:
    PluginManager();
    ~PluginManager();
    
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    PluginInfo* FindPlugin(const std::string& name);
    const PluginInfo* FindPlugin(const std::string& name) const;
    
    bool LoadPluginInfo(PluginInfo& info, const std::string& path);
    void UnloadPluginInfo(PluginInfo& info);
    
    std::unordered_map<std::string, PluginInfo> m_plugins;
    std::unordered_map<std::string, EngineFactory> m_engineFactories;
    std::unordered_map<std::string, DeviceFactory> m_deviceFactories;
};

}

}

#endif
