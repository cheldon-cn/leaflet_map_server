#pragma once

/**
 * @file export.h
 * @brief Export macro definitions for SDK export
 */

#ifdef _WIN32
    #ifdef OGC_GEOM_EXPORTS
        #define OGC_GEOM_API __declspec(dllexport)
    #elif defined(OGC_GEOM_STATIC)
        #define OGC_GEOM_API
    #else
        #define OGC_GEOM_API __declspec(dllimport)
    #endif
#else
    #ifdef OGC_GEOM_EXPORTS
        #define OGC_GEOM_API __attribute__((visibility("default")))
    #else
        #define OGC_GEOM_API
    #endif
#endif
