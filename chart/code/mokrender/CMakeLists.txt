cmake_minimum_required(VERSION 3.10)

project(ogc_mokrender VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS /W3)
endif()

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(ENABLE_TESTS "Build unit tests" ON)

set(SQLITE3_ROOT "E:/xspace/3rd/sqlite3-3.35.5" CACHE PATH "SQLite3 root directory")
set(GTEST_ROOT "E:/xspace/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(SQLITE3_INCLUDE_DIR "${SQLITE3_ROOT}" CACHE PATH "SQLite3 include directory")

set(MOKRENDER_SOURCES
    src/data_generator.cpp
    src/point_generator.cpp
    src/line_generator.cpp
    src/polygon_generator.cpp
    src/annotation_generator.cpp
    src/raster_generator.cpp
    src/multipoint_generator.cpp
    src/multilinestring_generator.cpp
    src/multipolygon_generator.cpp
    src/database_manager.cpp
    src/spatial_query_engine.cpp
    src/symbolizer_factory.cpp
    src/render_context.cpp
    src/raster_device_output.cpp
    src/pdf_device_output.cpp
    src/display_device_output.cpp
)

set(GEOM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../geom" CACHE PATH "Geometry source directory")
set(FEATURE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../feature" CACHE PATH "Feature source directory")
set(LAYER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../layer" CACHE PATH "Layer source directory")
set(GRAPH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../graph" CACHE PATH "Graph source directory")
set(DATABASE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../database" CACHE PATH "Database source directory")
set(OGC_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../build_vs2015/lib" CACHE PATH "OGC library directory")

link_directories(${OGC_LIB_DIR})

add_library(ogc_mokrender ${MOKRENDER_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(ogc_mokrender PRIVATE OGC_MOKRENDER_EXPORTS)
else()
    target_compile_definitions(ogc_mokrender PUBLIC OGC_MOKRENDER_STATIC)
endif()

target_include_directories(ogc_mokrender
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${GEOM_SOURCE_DIR}/include
        ${FEATURE_SOURCE_DIR}/include
        ${LAYER_SOURCE_DIR}/include
        ${GRAPH_SOURCE_DIR}/include
        ${DATABASE_SOURCE_DIR}/include
        ${SQLITE3_INCLUDE_DIR}
)

target_link_libraries(ogc_mokrender
    PUBLIC
        ogc_geometry
        ogc_feature
        ogc_layer
        ogc_database
        ogc_graph
)

set_target_properties(ogc_mokrender PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/install"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/install"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/install"
)

if(MSVC)
    set_target_properties(ogc_mokrender PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/install"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/install"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/install"
    )
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()

add_subdirectory(examples)

install(TARGETS ogc_mokrender
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
