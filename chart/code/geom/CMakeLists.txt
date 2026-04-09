cmake_minimum_required(VERSION 3.10)
project(ogc_geom VERSION 2.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/test")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/test")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/lib")

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/../build/test" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/../build/test" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/../build/lib" CACHE PATH "" FORCE)
endif()

set(BUILD_SHARED_LIBS ON)

set(GEOM_SOURCES
    src/coordinate.cpp
    src/envelope.cpp
    src/envelope3d.cpp
    src/exception.cpp
    src/geometry.cpp
    src/geometry_pool.cpp
    src/point.cpp
    src/linestring.cpp
    src/linearring.cpp
    src/polygon.cpp
    src/multipoint.cpp
    src/multilinestring.cpp
    src/multipolygon.cpp
    src/geometrycollection.cpp
    src/factory.cpp
    src/precision.cpp
    src/visitor.cpp
    src/spatial_index.cpp
)

add_library(ogc_geom ${GEOM_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(ogc_geom PRIVATE OGC_GEOM_EXPORTS)
else()
    target_compile_definitions(ogc_geom PUBLIC OGC_GEOM_STATIC)
endif()

target_include_directories(ogc_geom
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(ogc_geom PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS ogc_geom
    EXPORT ogc_geometry-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
