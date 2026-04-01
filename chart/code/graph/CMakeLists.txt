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

if(NOT PROJ_ROOT)
    set(PROJ_ROOT "E:/xspace/3rd/PROJ" CACHE PATH "PROJ library root directory")
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
    src/draw_params.cpp
    src/log.cpp
    src/draw_facade.cpp
    src/symbolizer.cpp
    src/point_symbolizer.cpp
    src/line_symbolizer.cpp
    src/polygon_symbolizer.cpp
    src/text_symbolizer.cpp
    src/raster_symbolizer.cpp
    src/composite_symbolizer.cpp
    src/icon_symbolizer.cpp
    src/filter.cpp
    src/comparison_filter.cpp
    src/logical_filter.cpp
    src/spatial_filter.cpp
    src/symbolizer_rule.cpp
    src/rule_engine.cpp
    src/label_engine.cpp
    src/label_placement.cpp
    src/label_conflict.cpp
    src/tile_key.cpp
    src/tile_cache.cpp
    src/memory_tile_cache.cpp
    src/disk_tile_cache.cpp
    src/multi_level_tile_cache.cpp
    src/tile_renderer.cpp
    src/coordinate_transformer.cpp
    src/coordinate_transform.cpp
    src/clipper.cpp
    src/transform_manager.cpp
    src/lod.cpp
    src/lod_manager.cpp
    src/render_task.cpp
    src/basic_render_task.cpp
    src/render_queue.cpp
    src/async_renderer.cpp
    src/performance_metrics.cpp
    src/performance_monitor.cpp
    src/proj_transformer.cpp
    src/thread_safe.cpp
    src/layer_manager.cpp
    src/image_draw.cpp
    src/sld_parser.cpp
    src/mapbox_style_parser.cpp
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
        ${PROJ_ROOT}/include
)

target_link_libraries(ogc_graph
    PUBLIC
        ogc_geometry
        ogc_feature
        ogc_layer
        ogc_draw
    PRIVATE
        ${PROJ_ROOT}/lib/proj.lib
)

target_compile_definitions(ogc_graph
    PRIVATE
        OGC_GRAPH_EXPORTS
)

install(TARGETS ogc_graph
    EXPORT ogc_geometry-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ogc/draw/
    DESTINATION include/ogc/draw
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.inl"
)
