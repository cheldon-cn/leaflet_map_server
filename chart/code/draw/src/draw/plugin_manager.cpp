#include "ogc/draw/plugin_manager.h"
#include <algorithm>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace ogc {
namespace draw {

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
    UnloadAll();
}

PluginManager& PluginManager::Instance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::LoadPlugin(const std::string& path) {
    PluginInfo info;
    info.path = path;
    
    if (!LoadPluginInfo(info, path)) {
        return false;
    }
    
    if (info.name.empty()) {
        UnloadPluginInfo(info);
        return false;
    }
    
    if (HasPlugin(info.name)) {
        UnloadPluginInfo(info);
        return false;
    }
    
    m_plugins[info.name] = info;
    return true;
}

void PluginManager::UnloadPlugin(const std::string& name) {
    auto it = m_plugins.find(name);
    if (it != m_plugins.end()) {
        UnloadPluginInfo(it->second);
        m_plugins.erase(it);
    }
}

void PluginManager::UnloadAll() {
    for (auto& pair : m_plugins) {
        UnloadPluginInfo(pair.second);
    }
    m_plugins.clear();
    m_engineFactories.clear();
    m_deviceFactories.clear();
}

void PluginManager::ScanPluginDirectory(const std::string& directory) {
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    std::string searchPath = directory + "\\*.dll";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string filePath = directory + "\\" + findData.cFileName;
            LoadPlugin(filePath);
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    (void)directory;
#endif
}

std::vector<std::string> PluginManager::GetLoadedPlugins() const {
    std::unordered_map<std::string, bool> allPlugins;
    for (const auto& pair : m_plugins) {
        allPlugins[pair.first] = true;
    }
    for (const auto& pair : m_engineFactories) {
        allPlugins[pair.first] = true;
    }
    for (const auto& pair : m_deviceFactories) {
        allPlugins[pair.first] = true;
    }
    
    std::vector<std::string> names;
    names.reserve(allPlugins.size());
    for (const auto& pair : allPlugins) {
        names.push_back(pair.first);
    }
    return names;
}

DrawEnginePlugin* PluginManager::GetEnginePlugin(const std::string& name) {
    auto it = m_plugins.find(name);
    if (it != m_plugins.end()) {
        return it->second.enginePlugin;
    }
    return nullptr;
}

DrawDevicePlugin* PluginManager::GetDevicePlugin(const std::string& name) {
    auto it = m_plugins.find(name);
    if (it != m_plugins.end()) {
        return it->second.devicePlugin;
    }
    return nullptr;
}

std::vector<DrawEnginePlugin*> PluginManager::GetAvailableEnginePlugins() {
    std::vector<DrawEnginePlugin*> plugins;
    for (const auto& pair : m_plugins) {
        if (pair.second.enginePlugin) {
            plugins.push_back(pair.second.enginePlugin);
        }
    }
    return plugins;
}

std::vector<DrawDevicePlugin*> PluginManager::GetAvailableDevicePlugins() {
    std::vector<DrawDevicePlugin*> plugins;
    for (const auto& pair : m_plugins) {
        if (pair.second.devicePlugin) {
            plugins.push_back(pair.second.devicePlugin);
        }
    }
    return plugins;
}

std::unique_ptr<DrawEngine> PluginManager::CreateEngine(const std::string& pluginName, DrawDevice* device) {
    auto plugin = GetEnginePlugin(pluginName);
    if (plugin) {
        return plugin->CreateEngine(device);
    }
    
    auto factoryIt = m_engineFactories.find(pluginName);
    if (factoryIt != m_engineFactories.end()) {
        return factoryIt->second();
    }
    
    return nullptr;
}

std::unique_ptr<DrawDevice> PluginManager::CreateDevice(const std::string& pluginName, int width, int height) {
    auto plugin = GetDevicePlugin(pluginName);
    if (plugin) {
        return plugin->CreateDevice(width, height);
    }
    
    auto factoryIt = m_deviceFactories.find(pluginName);
    if (factoryIt != m_deviceFactories.end()) {
        return factoryIt->second();
    }
    
    return nullptr;
}

bool PluginManager::HasPlugin(const std::string& name) const {
    if (m_plugins.find(name) != m_plugins.end()) {
        return true;
    }
    if (m_engineFactories.find(name) != m_engineFactories.end()) {
        return true;
    }
    if (m_deviceFactories.find(name) != m_deviceFactories.end()) {
        return true;
    }
    return false;
}

size_t PluginManager::GetPluginCount() const {
    std::unordered_map<std::string, bool> allPlugins;
    for (const auto& p : m_plugins) {
        allPlugins[p.first] = true;
    }
    for (const auto& p : m_engineFactories) {
        allPlugins[p.first] = true;
    }
    for (const auto& p : m_deviceFactories) {
        allPlugins[p.first] = true;
    }
    return allPlugins.size();
}

PluginInfo* PluginManager::FindPlugin(const std::string& name) {
    auto it = m_plugins.find(name);
    return it != m_plugins.end() ? &it->second : nullptr;
}

const PluginInfo* PluginManager::FindPlugin(const std::string& name) const {
    auto it = m_plugins.find(name);
    return it != m_plugins.end() ? &it->second : nullptr;
}

bool PluginManager::LoadPluginInfo(PluginInfo& info, const std::string& path) {
#ifdef _WIN32
    HMODULE handle = LoadLibraryA(path.c_str());
    if (!handle) {
        return false;
    }
    
    info.handle = handle;
    
    typedef const char* (*GetPluginNameFunc)();
    typedef const char* (*GetPluginVersionFunc)();
    typedef const char* (*GetPluginDescriptionFunc)();
    typedef DrawEnginePlugin* (*GetEnginePluginFunc)();
    typedef DrawDevicePlugin* (*GetDevicePluginFunc)();
    
    auto getName = (GetPluginNameFunc)GetProcAddress(handle, "GetPluginName");
    auto getVersion = (GetPluginVersionFunc)GetProcAddress(handle, "GetPluginVersion");
    auto getDescription = (GetPluginDescriptionFunc)GetProcAddress(handle, "GetPluginDescription");
    auto getEnginePlugin = (GetEnginePluginFunc)GetProcAddress(handle, "GetEnginePlugin");
    auto getDevicePlugin = (GetDevicePluginFunc)GetProcAddress(handle, "GetDevicePlugin");
    
    if (getName) {
        info.name = getName();
    }
    if (getVersion) {
        info.version = getVersion();
    }
    if (getDescription) {
        info.description = getDescription();
    }
    if (getEnginePlugin) {
        info.enginePlugin = getEnginePlugin();
    }
    if (getDevicePlugin) {
        info.devicePlugin = getDevicePlugin();
    }
    
    return true;
#else
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
        return false;
    }
    
    info.handle = handle;
    
    typedef const char* (*GetPluginNameFunc)();
    typedef const char* (*GetPluginVersionFunc)();
    typedef const char* (*GetPluginDescriptionFunc)();
    typedef DrawEnginePlugin* (*GetEnginePluginFunc)();
    typedef DrawDevicePlugin* (*GetDevicePluginFunc)();
    
    auto getName = (GetPluginNameFunc)dlsym(handle, "GetPluginName");
    auto getVersion = (GetPluginVersionFunc)dlsym(handle, "GetPluginVersion");
    auto getDescription = (GetPluginDescriptionFunc)dlsym(handle, "GetPluginDescription");
    auto getEnginePlugin = (GetEnginePluginFunc)dlsym(handle, "GetEnginePlugin");
    auto getDevicePlugin = (GetDevicePluginFunc)dlsym(handle, "GetDevicePlugin");
    
    if (getName) {
        info.name = getName();
    }
    if (getVersion) {
        info.version = getVersion();
    }
    if (getDescription) {
        info.description = getDescription();
    }
    if (getEnginePlugin) {
        info.enginePlugin = getEnginePlugin();
    }
    if (getDevicePlugin) {
        info.devicePlugin = getDevicePlugin();
    }
    
    return true;
#endif
}

void PluginManager::UnloadPluginInfo(PluginInfo& info) {
    if (info.handle) {
#ifdef _WIN32
        FreeLibrary((HMODULE)info.handle);
#else
        dlclose(info.handle);
#endif
        info.handle = nullptr;
    }
    info.enginePlugin = nullptr;
    info.devicePlugin = nullptr;
}

void PluginManager::RegisterEngineFactory(const std::string& name, EngineFactory factory) {
    m_engineFactories[name] = factory;
}

void PluginManager::RegisterDeviceFactory(const std::string& name, DeviceFactory factory) {
    m_deviceFactories[name] = factory;
}

void PluginManager::UnregisterEngineFactory(const std::string& name) {
    m_engineFactories.erase(name);
}

void PluginManager::UnregisterDeviceFactory(const std::string& name) {
    m_deviceFactories.erase(name);
}

}

}
