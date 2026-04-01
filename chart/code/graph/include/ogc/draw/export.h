#pragma once

#ifdef _WIN32
    #ifdef OGC_GRAPH_EXPORTS
        #define OGC_GRAPH_API __declspec(dllexport)
    #else
        #define OGC_GRAPH_API __declspec(dllimport)
    #endif
    #ifdef OGC_DRAW_EXPORTS
        #define OGC_DRAW_API __declspec(dllexport)
    #else
        #define OGC_DRAW_API __declspec(dllimport)
    #endif
#else
    #ifdef OGC_GRAPH_EXPORTS
        #define OGC_GRAPH_API __attribute__((visibility("default")))
    #else
        #define OGC_GRAPH_API
    #endif
    #ifdef OGC_DRAW_EXPORTS
        #define OGC_DRAW_API __attribute__((visibility("default")))
    #else
        #define OGC_DRAW_API
    #endif
#endif
