#include "ogc/base/version.h"
#include <map>

namespace ogc {
namespace base {

static std::string g_lastError;
static bool g_initialized = false;
static std::map<std::string, std::string> g_options;

std::string GetErrorMessage(int error_code) {
    switch (error_code) {
        case 0: return "Success";
        case 1: return "Invalid parameter";
        case 2: return "Out of memory";
        case 3: return "Not initialized";
        case 4: return "Not supported";
        case 5: return "File not found";
        default: return "Unknown error";
    }
}

bool Initialize() {
    g_initialized = true;
    return true;
}

void Shutdown() {
    g_initialized = false;
    g_options.clear();
}

bool IsInitialized() {
    return g_initialized;
}

std::string GetLastError() {
    return g_lastError;
}

void ClearLastError() {
    g_lastError.clear();
}

bool SetOption(const std::string& key, const std::string& value) {
    g_options[key] = value;
    return true;
}

std::string GetOption(const std::string& key) {
    auto it = g_options.find(key);
    if (it != g_options.end()) {
        return it->second;
    }
    return "";
}

bool HasOption(const std::string& key) {
    return g_options.find(key) != g_options.end();
}

void ClearOptions() {
    g_options.clear();
}

}
}
