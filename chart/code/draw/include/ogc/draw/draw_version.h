#ifndef OGC_DRAW_VERSION_H
#define OGC_DRAW_VERSION_H

#include "ogc/draw/export.h"
#include <string>
#include <vector>
#include <functional>

namespace ogc {
namespace draw {

struct OGC_DRAW_API Version {
    int major = 0;
    int minor = 0;
    int patch = 0;
    
    Version() = default;
    Version(int maj, int min, int pat) : major(maj), minor(min), patch(pat) {}
    
    static Version Current() { return Version(1, 0, 0); }
    static Version FromString(const std::string& str);
    
    std::string ToString() const;
    
    bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator!=(const Version& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Version& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
    
    bool operator<=(const Version& other) const {
        return *this < other || *this == other;
    }
    
    bool operator>(const Version& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const Version& other) const {
        return !(*this < other);
    }
    
    bool IsCompatible(const Version& other) const {
        return major == other.major && minor >= other.minor;
    }
};

class OGC_DRAW_API VersionManager {
public:
    static VersionManager& Instance();
    
    Version GetLibraryVersion() const;
    Version GetApiVersion() const;
    
    bool IsCompatible(const Version& required) const;
    bool IsApiCompatible(const Version& required) const;
    
    void RegisterDeprecation(const std::string& feature, const Version& deprecatedIn, 
                             const Version& removedIn);
    bool IsDeprecated(const std::string& feature) const;
    bool IsRemoved(const std::string& feature) const;
    
    std::string GetDeprecationMessage(const std::string& feature) const;
    
    void SetWarningCallback(std::function<void(const std::string&)> callback);
    
    struct DeprecationInfo {
        std::string feature;
        Version deprecatedIn;
        Version removedIn;
        std::string message;
    };
    
    std::vector<DeprecationInfo> GetDeprecations() const;
    
    void CheckCompatibility(const Version& pluginVersion, const std::string& pluginName);

private:
    VersionManager();
    ~VersionManager();
    
    VersionManager(const VersionManager&) = delete;
    VersionManager& operator=(const VersionManager&) = delete;
    
    std::vector<DeprecationInfo> m_deprecations;
    std::function<void(const std::string&)> m_warningCallback;
};

}

}

#endif
