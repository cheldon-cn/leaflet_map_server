cmake_minimum_required(VERSION 3.16)

project(ogc_chart_jni)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ogc_chart_jni SHARED
    jni_chart_viewer.cpp
    jni_convert.cpp
)

target_include_directories(ogc_chart_jni
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ogc_chart_jni
    PRIVATE
        android
        log
        EGL
        GLESv2
)

set_target_properties(ogc_chart_jni PROPERTIES
    OUTPUT_NAME "ogc_chart_jni"
)
