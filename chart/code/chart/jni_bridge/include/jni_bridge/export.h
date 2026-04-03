#ifndef OGC_JNI_BRIDGE_EXPORT_H
#define OGC_JNI_BRIDGE_EXPORT_H

#ifdef OGC_JNI_BRIDGE_STATIC
    #define OGC_JNI_BRIDGE_API
#else
    #ifdef _WIN32
        #ifdef OGC_JNI_BRIDGE_EXPORTS
            #define OGC_JNI_BRIDGE_API __declspec(dllexport)
        #else
            #define OGC_JNI_BRIDGE_API __declspec(dllimport)
        #endif
    #elif defined(__ANDROID__)
        #define OGC_JNI_BRIDGE_API __attribute__((visibility("default")))
    #else
        #define OGC_JNI_BRIDGE_API
    #endif
#endif

#endif
