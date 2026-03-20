#pragma once

/**
 * @file export.h
 * @brief 导出宏定义，用于SDK导出
 */

#ifdef _WIN32
    #ifdef OGC_GEOMETRY_EXPORTS
        #define OGC_API __declspec(dllexport)
    #else
        #define OGC_API __declspec(dllimport)
    #endif
#else
    #ifdef OGC_GEOMETRY_EXPORTS
        #define OGC_API __attribute__((visibility("default")))
    #else
        #define OGC_API
    #endif
#endif
