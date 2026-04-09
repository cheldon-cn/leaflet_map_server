#ifndef SDK_C_API_EXPORT_H
#define SDK_C_API_EXPORT_H

#ifdef _WIN32
    #ifdef OGC_CHART_C_API_EXPORTS
        #define SDK_C_API __declspec(dllexport)
    #else
        #define SDK_C_API __declspec(dllimport)
    #endif
#else
    #define SDK_C_API
#endif

#endif
