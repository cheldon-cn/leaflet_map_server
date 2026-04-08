#ifndef OGC_BASE_EXPORT_H
#define OGC_BASE_EXPORT_H

#ifdef _WIN32
    #ifdef OGC_BASE_EXPORTS
        #define OGC_BASE_API __declspec(dllexport)
    #else
        #define OGC_BASE_API __declspec(dllimport)
    #endif
#else
    #define OGC_BASE_API
#endif

#endif
