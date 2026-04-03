#ifndef OGC_ANDROID_ADAPTER_EXPORT_H
#define OGC_ANDROID_ADAPTER_EXPORT_H

#ifdef OGC_ANDROID_ADAPTER_STATIC
    #define OGC_ANDROID_ADAPTER_API
#else
    #ifdef _WIN32
        #ifdef OGC_ANDROID_ADAPTER_EXPORTS
            #define OGC_ANDROID_ADAPTER_API __declspec(dllexport)
        #else
            #define OGC_ANDROID_ADAPTER_API __declspec(dllimport)
        #endif
    #elif defined(__ANDROID__)
        #define OGC_ANDROID_ADAPTER_API __attribute__((visibility("default")))
    #else
        #define OGC_ANDROID_ADAPTER_API
    #endif
#endif

#endif
