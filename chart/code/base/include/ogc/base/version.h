#ifndef OGC_BASE_VERSION_H
#define OGC_BASE_VERSION_H

#include "export.h"
#include <string>

namespace ogc {
namespace base {

class OGC_BASE_API Version {
public:
    static int GetMajor() { return 2; }
    static int GetMinor() { return 3; }
    static int GetPatch() { return 0; }
    static std::string GetString() { return "2.3.0"; }
    static std::string GetFullString() { return "OGC Chart SDK 2.3.0"; }
    static std::string GetBuildDate() { return __DATE__; }
    static std::string GetBuildTime() { return __TIME__; }
    static std::string GetCompiler() {
#ifdef _MSC_VER
        return "MSVC " + std::to_string(_MSC_VER);
#else
        return "Unknown";
#endif
    }
    static bool IsAtLeast(int major, int minor, int patch) {
        if (GetMajor() < major) return false;
        if (GetMajor() > major) return true;
        if (GetMinor() < minor) return false;
        if (GetMinor() > minor) return true;
        return GetPatch() >= patch;
    }
    static bool CheckCompatibility(int major, int minor, int patch) {
        return GetMajor() == major;
    }
};

OGC_BASE_API std::string GetErrorMessage(int error_code);
OGC_BASE_API bool Initialize();
OGC_BASE_API void Shutdown();
OGC_BASE_API bool IsInitialized();
OGC_BASE_API std::string GetLastError();
OGC_BASE_API void ClearLastError();
OGC_BASE_API bool SetOption(const std::string& key, const std::string& value);
OGC_BASE_API std::string GetOption(const std::string& key);
OGC_BASE_API bool HasOption(const std::string& key);
OGC_BASE_API void ClearOptions();

}
}

#endif
