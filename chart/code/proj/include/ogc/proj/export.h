#ifndef OGC_PROJ_EXPORT_H
#define OGC_PROJ_EXPORT_H

#ifdef _WIN32
    #ifdef OGC_PROJ_EXPORTS
        #define OGC_PROJ_API __declspec(dllexport)
    #else
        #define OGC_PROJ_API __declspec(dllimport)
    #endif
#else
    #define OGC_PROJ_API
#endif

#endif
