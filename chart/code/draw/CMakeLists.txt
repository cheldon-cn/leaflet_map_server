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
)

set(OGC_DRAW_HEADERS
    include/ogc/draw/export.h
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
