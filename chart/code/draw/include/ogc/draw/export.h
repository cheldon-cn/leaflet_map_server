#ifndef OGC_DRAW_EXPORT_H
#define OGC_DRAW_EXPORT_H

#ifdef OGC_DRAW_STATIC
    #define OGC_DRAW_API
#else
    #ifdef _WIN32
        #ifdef OGC_DRAW_EXPORTS
            #define OGC_DRAW_API __declspec(dllexport)
        #else
            #define OGC_DRAW_API __declspec(dllimport)
        #endif
    #else
        #define OGC_DRAW_API
    #endif
#endif

#endif  
