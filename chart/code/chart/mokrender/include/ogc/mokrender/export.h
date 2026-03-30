#pragma once

#ifdef _WIN32
    #ifdef OGC_MOKRENDER_EXPORTS
        #define OGC_MOKRENDER_API __declspec(dllexport)
    #else
        #define OGC_MOKRENDER_API __declspec(dllimport)
    #endif
#else
    #ifdef OGC_MOKRENDER_EXPORTS
        #define OGC_MOKRENDER_API __attribute__((visibility("default")))
    #else
        #define OGC_MOKRENDER_API
    #endif
#endif

#ifdef OGC_MOKRENDER_STATIC
    #undef OGC_MOKRENDER_API
    #define OGC_MOKRENDER_API
#endif
