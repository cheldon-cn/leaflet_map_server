cmake_minimum_required(VERSION 3.10)

project(ogc_graph
    VERSION 1.0.0
    DESCRIPTION "OGC Graphics Drawing Library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build/tests/Release" CACHE PATH "Installation directory" FORCE)
endif()

if(MSVC)
    add_compile_options(/W4 /WX-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

set(OGC_GRAPH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

if(NOT GEOM_SOURCE_DIR)
    set(GEOM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../geom" CACHE PATH "Geometry library source directory")
endif()
if(NOT FEATURE_SOURCE_DIR)
    set(FEATURE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../feature" CACHE PATH "Feature library source directory")
endif()
if(NOT LAYER_SOURCE_DIR)
    set(LAYER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../layer" CACHE PATH "Layer library source directory")
endif()
if(NOT DRAW_SOURCE_DIR)
    set(DRAW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../draw" CACHE PATH "Draw library source directory")
endif()
if(NOT SYMBOLOGY_SOURCE_DIR)
    set(SYMBOLOGY_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../symbology" CACHE PATH "Symbology library source directory")
endif()
if(NOT BASE_SOURCE_DIR)
    set(BASE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../base" CACHE PATH "Base library source directory")
endif()
if(NOT PROJ_MODULE_SOURCE_DIR)
    set(PROJ_MODULE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../proj" CACHE PATH "Proj module source directory")
endif()
if(NOT CACHE_SOURCE_DIR)
    set(CACHE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../cache" CACHE PATH "Cache module source directory")
endif()

find_library(SQLITE3_LIBRARY
    NAMES sqlite3
    PATHS "${SQLITE3_ROOT}/lib(x64)"
    NO_DEFAULT_PATH)

if(NOT SQLITE3_LIBRARY)
    message(WARNING "SQLite3 not found at ${SQLITE3_ROOT}/lib(x64)")
endif()

find_library(CURL_LIBRARY
    NAMES libcurl_imp libcurl
    PATHS "${CURL_ROOT}/lib"
    NO_DEFAULT_PATH)

if(NOT CURL_LIBRARY)
    message(WARNING "cURL not found at ${CURL_ROOT}/lib")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
endif()

set(OGC_GRAPH_SOURCES
    src/render/draw_params.cpp
    src/render/draw_facade.cpp
    src/render/render_task.cpp
    src/render/basic_render_task.cpp
    src/render/render_queue.cpp
    src/render/async_renderer.cpp
    src/render/tile_renderer.cpp
    src/label/label_engine.cpp
    src/label/label_placement.cpp
    src/label/label_conflict.cpp
    src/interaction/interaction_handler.cpp
    src/interaction/interaction_feedback.cpp
    src/interaction/selection_handler.cpp
    src/interaction/pan_zoom_handler.cpp
    src/interaction/location_display_handler.cpp
    src/interaction/hit_test.cpp
    src/layer/layer_manager.cpp
    src/layer/layer_control_panel.cpp
    src/lod/lod.cpp
    src/lod/lod_manager.cpp
    src/util/image_draw.cpp
    src/util/transform_manager.cpp
    src/util/day_night_mode_manager.cpp
)

add_library(ogc_graph SHARED ${OGC_GRAPH_SOURCES})

set_target_properties(ogc_graph PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(MSVC)
    set_target_properties(ogc_graph PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
    )
endif()

target_include_directories(ogc_graph
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(ogc_graph
    PRIVATE
        ${GEOM_SOURCE_DIR}/include
        ${FEATURE_SOURCE_DIR}/include
        ${LAYER_SOURCE_DIR}/include
        ${DRAW_SOURCE_DIR}/include
        ${SYMBOLOGY_SOURCE_DIR}/include
        ${PROJ_ROOT}/include
        ${BASE_SOURCE_DIR}/include
        ${PROJ_MODULE_SOURCE_DIR}/include
        ${CACHE_SOURCE_DIR}/include
)

target_link_libraries(ogc_graph
    PUBLIC
        ogc_geometry
        ogc_feature
        ogc_layer
        ogc_draw
        ogc_symbology
        ogc_base
        ogc_proj
        ogc_cache
)

if(EXISTS "${PROJ_ROOT}/lib/proj.lib")
    target_link_libraries(ogc_graph PRIVATE "${PROJ_ROOT}/lib/proj.lib")
endif()

if(SQLITE3_LIBRARY)
    target_link_libraries(ogc_graph PRIVATE ${SQLITE3_LIBRARY})
endif()

if(CURL_LIBRARY)
    target_link_libraries(ogc_graph PRIVATE ${CURL_LIBRARY})
endif()

target_compile_definitions(ogc_graph
    PRIVATE
        OGC_GRAPH_EXPORTS
        OGC_USE_PROJ
)

install(TARGETS ogc_graph
    EXPORT ogc_graph-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ogc/draw/
    DESTINATION include/ogc/draw
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.inl"
)

install(DIRECTORY include/ogc/graph/
    DESTINATION include/ogc/graph
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.inl"
)
