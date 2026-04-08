#pragma once

/**
 * @file export.h
 * @brief 导出宏定义，用于SDK导出
 */

#ifdef _WIN32
    #ifdef OGC_LAYER_EXPORTS
        #define OGC_LAYER_API __declspec(dllexport)
    #else
        #define OGC_LAYER_API __declspec(dllimport)
    #endif
#else
    #ifdef OGC_LAYER_EXPORTS
        #define OGC_LAYER_API __attribute__((visibility("default")))
    #else
        #define OGC_LAYER_API
    #endif
#endif
