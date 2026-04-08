#ifndef OGC_DRAW_LIBRARY_COMPATIBILITY_H
#define OGC_DRAW_LIBRARY_COMPATIBILITY_H

#include "ogc/draw/export.h"
#include <string>
#include <vector>
#include <map>

namespace ogc {
namespace draw {

struct OGC_DRAW_API LibraryVersion {
    std::string name;
    int major = 0;
    int minor = 0;
    int patch = 0;
    
    LibraryVersion() = default;
    LibraryVersion(const std::string& n, int maj, int min, int pat)
        : name(n), major(maj), minor(min), patch(pat) {}
    
    bool operator>=(const LibraryVersion& other) const;
    bool operator<(const LibraryVersion& other) const;
    bool operator==(const LibraryVersion& other) const;
    std::string ToString() const;
    bool IsValid() const { return major > 0 || minor > 0 || patch > 0; }
    
    static LibraryVersion Parse(const std::string& versionStr, const std::string& name = "");
};

struct OGC_DRAW_API CompatibilityIssue {
    std::string libraryName;
    LibraryVersion detected;
    LibraryVersion required;
    std::string description;
    bool isCritical;
    
    std::string ToString() const;
};

class OGC_DRAW_API LibraryCompatibility {
public:
    static LibraryVersion GetQtVersion();
    static LibraryVersion GetCairoVersion();
    static LibraryVersion GetPodofoVersion();
    static LibraryVersion GetOpenGLVersion();
    static LibraryVersion GetFreetypeVersion();
    static LibraryVersion GetZlibVersion();
    static LibraryVersion GetLibPngVersion();
    static LibraryVersion GetLibJpegVersion();
    
    static LibraryVersion GetLibraryVersion(const std::string& libraryName);
    
    static bool CheckMinimumRequirements();
    static std::vector<std::string> GetCompatibilityIssues();
    static std::vector<CompatibilityIssue> GetDetailedCompatibilityIssues();
    static std::string GetCompatibilityReport();
    static std::string GetDetailedCompatibilityReport();
    
    static void SetMinimumVersion(const std::string& libraryName, const LibraryVersion& version);
    static LibraryVersion GetMinimumVersion(const std::string& libraryName);
    static void ResetMinimumVersions();
    
    static bool IsQtAvailable();
    static bool IsCairoAvailable();
    static bool IsPodofoAvailable();
    static bool IsOpenGLAvailable();
    static bool IsFreetypeAvailable();
    
    static std::vector<std::string> GetAvailableLibraries();
    static std::string GetSystemInfo();
    
    static void SetEnableLogging(bool enable);
    static bool IsLoggingEnabled();

private:
    static std::map<std::string, LibraryVersion> s_minimumVersions;
    static std::map<std::string, LibraryVersion> s_detectedVersions;
    static bool s_enableLogging;
    
    static void InitializeMinimumVersions();
    static void Log(const std::string& message);
    static LibraryVersion DetectQtVersion();
    static LibraryVersion DetectCairoVersion();
    static LibraryVersion DetectPodofoVersion();
    static LibraryVersion DetectOpenGLVersion();
    static LibraryVersion DetectFreetypeVersion();
    static LibraryVersion DetectZlibVersion();
    static LibraryVersion DetectLibPngVersion();
    static LibraryVersion DetectLibJpegVersion();
};

}
}

#endif
