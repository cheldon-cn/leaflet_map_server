#include "ogc/draw/driver_manager.h"
#include <algorithm>

namespace ogc {
namespace draw {

DriverManager& DriverManager::Instance() {
    static DriverManager instance;
    return instance;
}

DrawResult DriverManager::RegisterDriver(const std::string& name, DrawDriverPtr driver) {
    if (!driver) {
        return DrawResult::kInvalidParams;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_drivers[name] = driver;
    return DrawResult::kSuccess;
}

DrawResult DriverManager::UnregisterDriver(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_drivers.find(name);
    if (it == m_drivers.end()) {
        return DrawResult::kFileNotFound;
    }
    m_drivers.erase(it);
    return DrawResult::kSuccess;
}

DrawDriverPtr DriverManager::GetDriver(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_drivers.find(name);
    if (it != m_drivers.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> DriverManager::GetDriverNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> names;
    names.reserve(m_drivers.size());
    for (const auto& pair : m_drivers) {
        names.push_back(pair.first);
    }
    return names;
}

bool DriverManager::HasDriver(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_drivers.find(name) != m_drivers.end();
}

DrawResult DriverManager::RegisterDevice(const std::string& name, DrawDevicePtr device) {
    if (!device) {
        return DrawResult::kInvalidParams;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_devices[name] = device;
    return DrawResult::kSuccess;
}

DrawResult DriverManager::UnregisterDevice(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_devices.find(name);
    if (it == m_devices.end()) {
        return DrawResult::kFileNotFound;
    }
    m_devices.erase(it);
    return DrawResult::kSuccess;
}

DrawDevicePtr DriverManager::GetDevice(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_devices.find(name);
    if (it != m_devices.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> DriverManager::GetDeviceNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> names;
    names.reserve(m_devices.size());
    for (const auto& pair : m_devices) {
        names.push_back(pair.first);
    }
    return names;
}

bool DriverManager::HasDevice(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_devices.find(name) != m_devices.end();
}

DrawResult DriverManager::RegisterEngine(const std::string& name, DrawEnginePtr engine) {
    if (!engine) {
        return DrawResult::kInvalidParams;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_engines[name] = engine;
    return DrawResult::kSuccess;
}

DrawResult DriverManager::UnregisterEngine(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_engines.find(name);
    if (it == m_engines.end()) {
        return DrawResult::kFileNotFound;
    }
    m_engines.erase(it);
    return DrawResult::kSuccess;
}

DrawEnginePtr DriverManager::GetEngine(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_engines.find(name);
    if (it != m_engines.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> DriverManager::GetEngineNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> names;
    names.reserve(m_engines.size());
    for (const auto& pair : m_engines) {
        names.push_back(pair.first);
    }
    return names;
}

bool DriverManager::HasEngine(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engines.find(name) != m_engines.end();
}

DrawDriverPtr DriverManager::CreateDriverForDevice(DeviceType deviceType) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& pair : m_drivers) {
        if (pair.second->CanHandleDevice(deviceType)) {
            return pair.second;
        }
    }
    return nullptr;
}

DrawDriverPtr DriverManager::CreateDriverForEngine(EngineType engineType) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& pair : m_drivers) {
        if (pair.second->CanHandleEngine(engineType)) {
            return pair.second;
        }
    }
    return nullptr;
}

std::vector<DrawDriverPtr> DriverManager::GetDriversForDevice(DeviceType deviceType) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<DrawDriverPtr> result;
    for (const auto& pair : m_drivers) {
        if (pair.second->CanHandleDevice(deviceType)) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<DrawDriverPtr> DriverManager::GetDriversForEngine(EngineType engineType) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<DrawDriverPtr> result;
    for (const auto& pair : m_drivers) {
        if (pair.second->CanHandleEngine(engineType)) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void DriverManager::Clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_drivers.clear();
    m_devices.clear();
    m_engines.clear();
}

size_t DriverManager::GetDriverCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_drivers.size();
}

size_t DriverManager::GetDeviceCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_devices.size();
}

size_t DriverManager::GetEngineCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_engines.size();
}

}  
}  
