#pragma once

/**
 * @file export.h
 * @brief 导出宏定义，用于SDK导出
 */

#ifdef _WIN32
    #ifdef OGC_FEATURE_EXPORTS
        #define OGC_FEATURE_API __declspec(dllexport)
    #else
        #define OGC_FEATURE_API __declspec(dllimport)
    #endif
#else
    #ifdef OGC_FEATURE_EXPORTS
        #define OGC_FEATURE_API __attribute__((visibility("default")))
    #else
        #define OGC_FEATURE_API
    #endif
#endif
