#pragma once

#ifdef OGC_NAVI_STATIC
    #define OGC_NAVI_API
#else
    #ifdef _WIN32
        #ifdef OGC_NAVI_EXPORTS
            #define OGC_NAVI_API __declspec(dllexport)
        #else
            #define OGC_NAVI_API __declspec(dllimport)
        #endif
    #else
        #ifdef OGC_NAVI_EXPORTS
            #define OGC_NAVI_API __attribute__((visibility("default")))
        #else
            #define OGC_NAVI_API
        #endif
    #endif
#endif
