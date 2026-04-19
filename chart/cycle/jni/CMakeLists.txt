cmake_minimum_required(VERSION 3.10)
project(ogc_chart_jni VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED JAVA_HOME)
    if(DEFINED ENV{JAVA_HOME})
        set(JAVA_HOME "$ENV{JAVA_HOME}")
    else()
        message(FATAL_ERROR "JAVA_HOME is not set. Please set JAVA_HOME environment variable or pass -DJAVA_HOME=...")
    endif()
endif()

set(JNI_INCLUDE_DIRS
    "${JAVA_HOME}/include"
    "${JAVA_HOME}/include/win32"
)

set(OGC_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../install")
set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../build")
set(BUILD_TEST_DIR "${BUILD_DIR}/test")
set(BUILD_LIB_DIR "${BUILD_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_LIB_DIR})

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BUILD_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BUILD_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BUILD_LIB_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BUILD_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BUILD_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BUILD_LIB_DIR}" CACHE PATH "" FORCE)
endif()

set(JNI_SOURCES
    src/jni_env_manager.cpp
    src/jni_converter.cpp
    src/jni_exception.cpp
    src/jni_reference_cache.cpp
    src/jni_onload.cpp
    src/native/chart_viewer_jni.cpp
    src/native/viewport_jni.cpp
    src/native/geometry_jni.cpp
    src/native/feature_jni.cpp
    src/native/layer_jni.cpp
    src/native/chart_config_jni.cpp
    src/native/adapter_jni.cpp
    src/native/logger_jni.cpp
    src/native/coord_transformer_jni.cpp
    src/native/ais_jni.cpp
    src/native/route_jni.cpp
    src/native/chart_parser_jni.cpp
    src/native/tile_cache_jni.cpp
    src/native/alert_jni.cpp
    src/native/navigation_engine_jni.cpp
    src/native/route_planner_jni.cpp
    src/native/track_jni.cpp
    src/native/nmea_position_jni.cpp
    src/native/geometry_ext_jni.cpp
    src/native/draw_style_jni.cpp
    src/native/symbology_jni.cpp
    src/native/offline_cache_jni.cpp
    src/native/datasource_jni.cpp
    src/native/recovery_jni.cpp
    src/native/health_jni.cpp
    src/native/loader_jni.cpp
    src/native/plugin_jni.cpp
    src/native/collision_deviation_jni.cpp
    src/native/encryption_jni.cpp
    src/native/render_perf_jni.cpp
    src/native/transform_matrix_jni.cpp
    src/native/geometry_spatial_jni.cpp
    src/native/draw_context_jni.cpp
    src/native/feature_layer_ext_jni.cpp
    src/native/navi_ext_jni.cpp
    src/native/symbology_ext_jni.cpp
    src/native/parser_ext_jni.cpp
    src/native/graph_ext_jni.cpp
    src/native/offline_cache_ext_jni.cpp
    src/native/util_ext_jni.cpp
    src/native/exception_jni.cpp
    src/native/secure_loader_jni.cpp
)

add_library(ogc_chart_jni SHARED ${JNI_SOURCES})

target_include_directories(ogc_chart_jni
    PRIVATE
        ${JNI_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        "${OGC_INSTALL_DIR}/include"
)

target_link_directories(ogc_chart_jni
    PRIVATE
        ${BUILD_LIB_DIR}
        ${OGC_INSTALL_DIR}/lib
)

target_link_libraries(ogc_chart_jni
    PRIVATE
        ogc_chart_c_api
)

target_compile_definitions(ogc_chart_jni PRIVATE OGC_CHART_C_API_EXPORTS)

install(TARGETS ogc_chart_jni
    RUNTIME DESTINATION ${OGC_INSTALL_DIR}/bin
    LIBRARY DESTINATION ${OGC_INSTALL_DIR}/bin
    ARCHIVE DESTINATION ${OGC_INSTALL_DIR}/lib
)
