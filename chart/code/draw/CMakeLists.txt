cmake_minimum_required(VERSION 3.10)

project(ogc_draw VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

option(OGC_DRAW_STATIC "Build static library" OFF)
option(BUILD_TESTING "Build tests" ON)

if(OGC_DRAW_STATIC)
    add_definitions(-DOGC_DRAW_STATIC)
endif()

set(OGC_DRAW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GEOM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../geom/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
endif()

set(OGC_DRAW_SOURCES
    src/color.cpp
    src/device_type.cpp
    src/device_state.cpp
    src/engine_type.cpp
    src/draw_result.cpp
    src/font.cpp
    src/image.cpp
    src/region.cpp
    src/draw/draw_context.cpp
    src/draw/state_serializer.cpp
    src/draw/raster_image_device.cpp
    src/draw/display_device.cpp
    src/draw/simple2d_engine.cpp
    src/draw/tile_device.cpp
    src/draw/tile_based_engine.cpp
    src/draw/tile_size_strategy.cpp
    src/draw/async_render_task.cpp
    src/draw/async_render_manager.cpp
    src/draw/vector_engine.cpp
    src/draw/svg_device.cpp
    src/draw/svg_engine.cpp
    src/draw/geometry.cpp
    src/draw/gpu_resource_manager.cpp
    src/draw/render_memory_pool.cpp
    src/draw/gpu_accelerated_engine.cpp
    src/draw/texture_cache.cpp
    src/draw/gpu_resource_wrapper.cpp
    src/draw/thread_safe_engine.cpp
    src/draw/pdf_device.cpp
    src/draw/pdf_engine.cpp
    src/draw/capability_negotiator.cpp
    src/draw/gpu_device_selector.cpp
    src/draw/draw_scope_guard.cpp
    src/draw/library_compatibility.cpp
    src/draw/gdiplus_engine.cpp
    src/draw/direct2d_engine.cpp
    src/draw/windows_platform.cpp
    src/draw/plugin_manager.cpp
    src/draw/draw_version.cpp
    src/draw/engine_pool.cpp
    src/draw/batch_renderer.cpp
    src/draw/render_cache.cpp
    src/draw/lod_strategy.cpp
)

set(OGC_DRAW_HEADERS
    include/ogc/draw/export.h
    include/ogc/draw/color.h
    include/ogc/draw/geometry_types.h
    include/ogc/draw/device_type.h
    include/ogc/draw/engine_type.h
    include/ogc/draw/draw_types.h
    include/ogc/draw/draw_result.h
    include/ogc/draw/draw_style.h
    include/ogc/draw/transform_matrix.h
    include/ogc/draw/geometry.h
    include/ogc/draw/draw_device.h
    include/ogc/draw/draw_engine.h
    include/ogc/draw/draw_context.h
    include/ogc/draw/state_serializer.h
    include/ogc/draw/raster_image_device.h
    include/ogc/draw/display_device.h
    include/ogc/draw/simple2d_engine.h
    include/ogc/draw/tile_device.h
    include/ogc/draw/tile_based_engine.h
    include/ogc/draw/tile_size_strategy.h
    include/ogc/draw/async_render_task.h
    include/ogc/draw/async_render_manager.h
    include/ogc/draw/vector_engine.h
    include/ogc/draw/svg_device.h
    include/ogc/draw/svg_engine.h
    include/ogc/draw/gpu_resource_manager.h
    include/ogc/draw/render_memory_pool.h
    include/ogc/draw/gpu_accelerated_engine.h
    include/ogc/draw/texture_cache.h
    include/ogc/draw/gpu_resource_wrapper.h
    include/ogc/draw/thread_safe_engine.h
    include/ogc/draw/pdf_device.h
    include/ogc/draw/pdf_engine.h
    include/ogc/draw/capability_negotiator.h
    include/ogc/draw/gpu_device_selector.h
    include/ogc/draw/draw_scope_guard.h
    include/ogc/draw/library_compatibility.h
    include/ogc/draw/gdiplus_engine.h
    include/ogc/draw/direct2d_engine.h
    include/ogc/draw/windows_platform.h
    include/ogc/draw/plugin_manager.h
    include/ogc/draw/draw_engine_plugin.h
    include/ogc/draw/draw_device_plugin.h
    include/ogc/draw/draw_version.h
    include/ogc/draw/engine_pool.h
    include/ogc/draw/batch_renderer.h
    include/ogc/draw/render_cache.h
    include/ogc/draw/lod_strategy.h
)

if(OGC_DRAW_STATIC)
    add_library(ogc_draw STATIC ${OGC_DRAW_SOURCES} ${OGC_DRAW_HEADERS})
else()
    add_library(ogc_draw SHARED ${OGC_DRAW_SOURCES} ${OGC_DRAW_HEADERS})
    target_compile_definitions(ogc_draw PRIVATE OGC_DRAW_EXPORTS)
endif()

target_include_directories(ogc_draw
    PUBLIC
        $<BUILD_INTERFACE:${OGC_DRAW_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GEOM_INCLUDE_DIR}
)

target_link_libraries(ogc_draw PUBLIC ogc_geometry)

if(WIN32)
    target_link_libraries(ogc_draw PRIVATE gdiplus d2d1 dwrite)
endif()

option(DRAW_WITH_QT "Build with Qt support" OFF)

if(DRAW_WITH_QT)
    find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
    if(Qt6_FOUND)
        message(STATUS "Found Qt6: ${Qt6_VERSION}")
        set(QT_VERSION_MAJOR 6)
    else()
        find_package(Qt5 QUIET COMPONENTS Core Gui Widgets)
        if(Qt5_FOUND)
            message(STATUS "Found Qt5: ${Qt5_VERSION}")
            set(QT_VERSION_MAJOR 5)
        endif()
    endif()
    
    if(Qt5_FOUND OR Qt6_FOUND)
        list(APPEND OGC_DRAW_SOURCES src/draw/qt_engine.cpp)
        list(APPEND OGC_DRAW_HEADERS include/ogc/draw/qt_engine.h)
        list(APPEND OGC_DRAW_SOURCES src/draw/qt_display_device.cpp)
        list(APPEND OGC_DRAW_HEADERS include/ogc/draw/qt_display_device.h)
        
        if(QT_VERSION_MAJOR EQUAL 6)
            target_link_libraries(ogc_draw PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
        else()
            target_link_libraries(ogc_draw PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
        endif()
        
        target_compile_definitions(ogc_draw PRIVATE DRAW_HAS_QT)
    else()
        message(STATUS "Qt not found, QtEngine will not be built")
    endif()
endif()

set_target_properties(ogc_draw PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "ogc_draw"
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS ogc_draw
    EXPORT ogc_draw-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT ogc_draw-targets
    FILE ogc_draw-config.cmake
    NAMESPACE ogc::
    DESTINATION lib/cmake/ogc_draw
)
