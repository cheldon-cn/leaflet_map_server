#ifndef SDK_C_API_INTERNAL_H
#define SDK_C_API_INTERNAL_H

#include <cstring>
#include <cstdlib>
#include <string>

namespace ogc {
namespace c_api {

inline std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

inline char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

inline void FreeString(char* str) {
    if (str) {
        std::free(str);
    }
}

using SafeStringFunc = std::string(*)(const char*);
using AllocStringFunc = char*(*)(const std::string&);

}
}

#define OGC_SAFE_STRING(str) ::ogc::c_api::SafeString(str)
#define OGC_ALLOC_STRING(str) ::ogc::c_api::AllocString(str)

#endif
