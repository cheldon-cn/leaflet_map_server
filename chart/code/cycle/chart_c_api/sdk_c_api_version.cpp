/**
 * @file sdk_c_api_version.cpp
 * @brief OGC Chart SDK C API - Version Information and Utility Functions Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/base/version.h>

#include <cstring>
#include <cstdlib>
#include <string>

using namespace ogc::base;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

}  

int ogc_version_get_major(void) {
    return Version::GetMajor();
}

int ogc_version_get_minor(void) {
    return Version::GetMinor();
}

int ogc_version_get_patch(void) {
    return Version::GetPatch();
}

char* ogc_version_get_string(void) {
    return AllocString(Version::GetString());
}

char* ogc_version_get_full_string(void) {
    return AllocString(Version::GetFullString());
}

char* ogc_version_get_build_date(void) {
    return AllocString(Version::GetBuildDate());
}

char* ogc_version_get_build_time(void) {
    return AllocString(Version::GetBuildTime());
}

char* ogc_version_get_compiler(void) {
    return AllocString(Version::GetCompiler());
}

int ogc_version_is_at_least(int major, int minor, int patch) {
    return Version::IsAtLeast(major, minor, patch) ? 1 : 0;
}

int ogc_version_check_compatibility(int major, int minor, int patch) {
    return Version::CheckCompatibility(major, minor, patch) ? 1 : 0;
}

char* ogc_sdk_get_version(void) {
    return AllocString(Version::GetString());
}

char* ogc_sdk_get_build_date(void) {
    return AllocString(Version::GetBuildDate());
}

int ogc_sdk_get_version_major(void) {
    return Version::GetMajor();
}

int ogc_sdk_get_version_minor(void) {
    return Version::GetMinor();
}

int ogc_sdk_get_version_patch(void) {
    return Version::GetPatch();
}

void ogc_free_string(char* str) {
    std::free(str);
}

void ogc_free_memory(void* ptr) {
    std::free(ptr);
}

char* ogc_get_error_message(int error_code) {
    return AllocString(GetErrorMessage(error_code));
}

int ogc_initialize(void) {
    return Initialize() ? 1 : 0;
}

void ogc_shutdown(void) {
    Shutdown();
}

int ogc_is_initialized(void) {
    return IsInitialized() ? 1 : 0;
}

char* ogc_get_last_error(void) {
    return AllocString(GetLastError());
}

void ogc_clear_last_error(void) {
    ClearLastError();
}

int ogc_set_option(const char* key, const char* value) {
    if (key && value) {
        return SetOption(std::string(key), std::string(value)) ? 1 : 0;
    }
    return 0;
}

char* ogc_get_option(const char* key) {
    if (key) {
        return AllocString(GetOption(std::string(key)));
    }
    return nullptr;
}

int ogc_has_option(const char* key) {
    if (key) {
        return HasOption(std::string(key)) ? 1 : 0;
    }
    return 0;
}

void ogc_clear_options(void) {
    ClearOptions();
}

#ifdef __cplusplus
}
#endif
