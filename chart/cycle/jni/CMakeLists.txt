cmake_minimum_required(VERSION 3.10)
project(ogc_chart_jni LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT JAVA_HOME)
    message(FATAL_ERROR "JAVA_HOME is not set. Please set JAVA_HOME environment variable.")
endif()

set(JNI_INCLUDE_DIRS
    "${JAVA_HOME}/include"
    "${JAVA_HOME}/include/win32"
)

set(OGC_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../install")

set(JNI_SOURCES
    src/jni_env_manager.cpp
    src/jni_converter.cpp
    src/jni_exception.cpp
    src/jni_reference_cache.cpp
    src/jni_onload.cpp
    src/native/chart_viewer_jni.cpp
    src/native/geometry_jni.cpp
    src/native/feature_jni.cpp
    src/native/layer_jni.cpp
)

add_library(ogc_chart_jni SHARED ${JNI_SOURCES})

target_include_directories(ogc_chart_jni
    PRIVATE
        ${JNI_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        "${OGC_INSTALL_DIR}/include/ogc/capi"
)

target_link_libraries(ogc_chart_jni
    PRIVATE
        "${OGC_INSTALL_DIR}/lib/ogc_chart_c_api.lib"
)

if(MSVC)
    set_target_properties(ogc_chart_jni PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OGC_INSTALL_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OGC_INSTALL_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OGC_INSTALL_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OGC_INSTALL_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OGC_INSTALL_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OGC_INSTALL_DIR}/lib"
    )
else()
    set_target_properties(ogc_chart_jni PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${OGC_INSTALL_DIR}/bin"
    )
endif()

target_compile_definitions(ogc_chart_jni PRIVATE OGC_CHART_C_API_EXPORTS)
