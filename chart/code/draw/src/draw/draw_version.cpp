#include "ogc/draw/draw_version.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace draw {

Version Version::FromString(const std::string& str) {
    Version v;
    
    std::vector<int> parts;
    std::stringstream ss(str);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        try {
            parts.push_back(std::stoi(part));
        } catch (...) {
            break;
        }
    }
    
    if (parts.size() >= 1) v.major = parts[0];
    if (parts.size() >= 2) v.minor = parts[1];
    if (parts.size() >= 3) v.patch = parts[2];
    
    return v;
}

std::string Version::ToString() const {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

VersionManager& VersionManager::Instance() {
    static VersionManager instance;
    return instance;
}

VersionManager::VersionManager() {
}

VersionManager::~VersionManager() {
}

Version VersionManager::GetLibraryVersion() const {
    return Version::Current();
}

Version VersionManager::GetApiVersion() const {
    return Version::Current();
}

bool VersionManager::IsCompatible(const Version& required) const {
    return GetLibraryVersion().IsCompatible(required);
}

bool VersionManager::IsApiCompatible(const Version& required) const {
    return GetApiVersion().IsCompatible(required);
}

void VersionManager::RegisterDeprecation(const std::string& feature, 
                                          const Version& deprecatedIn,
                                          const Version& removedIn) {
    DeprecationInfo info;
    info.feature = feature;
    info.deprecatedIn = deprecatedIn;
    info.removedIn = removedIn;
    info.message = feature + " is deprecated since version " + deprecatedIn.ToString() + 
                   " and will be removed in version " + removedIn.ToString();
    m_deprecations.push_back(info);
}

bool VersionManager::IsDeprecated(const std::string& feature) const {
    auto current = GetLibraryVersion();
    for (const auto& dep : m_deprecations) {
        if (dep.feature == feature) {
            return current >= dep.deprecatedIn;
        }
    }
    return false;
}

bool VersionManager::IsRemoved(const std::string& feature) const {
    auto current = GetLibraryVersion();
    for (const auto& dep : m_deprecations) {
        if (dep.feature == feature) {
            return current >= dep.removedIn;
        }
    }
    return false;
}

std::string VersionManager::GetDeprecationMessage(const std::string& feature) const {
    for (const auto& dep : m_deprecations) {
        if (dep.feature == feature) {
            return dep.message;
        }
    }
    return "";
}

void VersionManager::SetWarningCallback(std::function<void(const std::string&)> callback) {
    m_warningCallback = callback;
}

std::vector<VersionManager::DeprecationInfo> VersionManager::GetDeprecations() const {
    return m_deprecations;
}

void VersionManager::CheckCompatibility(const Version& pluginVersion, 
                                         const std::string& pluginName) {
    if (!IsCompatible(pluginVersion)) {
        std::string msg = "Plugin '" + pluginName + "' version " + pluginVersion.ToString() + 
                          " is not compatible with library version " + GetLibraryVersion().ToString();
        if (m_warningCallback) {
            m_warningCallback(msg);
        }
    }
}

}
}
