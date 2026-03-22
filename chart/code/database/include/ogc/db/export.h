#pragma once

#ifdef OGC_DATABASE_STATIC
    #define OGC_DB_API
#else
    #ifdef _WIN32
        #ifdef OGC_DATABASE_EXPORTS
            #define OGC_DB_API __declspec(dllexport)
        #else
            #define OGC_DB_API __declspec(dllimport)
        #endif
    #else
        #ifdef OGC_DATABASE_EXPORTS
            #define OGC_DB_API __attribute__((visibility("default")))
        #else
            #define OGC_DB_API
        #endif
    #endif
#endif
