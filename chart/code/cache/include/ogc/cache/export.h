#ifndef OGC_CACHE_EXPORT_H
#define OGC_CACHE_EXPORT_H

#ifdef _WIN32
    #ifdef OGC_CACHE_EXPORTS
        #define OGC_CACHE_API __declspec(dllexport)
    #else
        #define OGC_CACHE_API __declspec(dllimport)
    #endif
#else
    #define OGC_CACHE_API
#endif

#endif
