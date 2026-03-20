cmake_minimum_required(VERSION 3.10)

project(OGCGeometry VERSION 2.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

option(USE_GEOS "Enable GEOS backend for advanced operations" OFF)
option(USE_PROJ "Enable PROJ for coordinate transformation" OFF)
option(ENABLE_TESTS "Build unit tests" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)

add_library(ogc_geometry
    geom/src/coordinate.cpp
    geom/src/envelope.cpp
    geom/src/envelope3d.cpp
    geom/src/exception.cpp
    geom/src/geometry.cpp
    geom/src/geometry_pool.cpp
    geom/src/point.cpp
    geom/src/linestring.cpp
    geom/src/linearring.cpp
    geom/src/polygon.cpp
    geom/src/multipoint.cpp
    geom/src/multilinestring.cpp
    geom/src/multipolygon.cpp
    geom/src/geometrycollection.cpp
    geom/src/factory.cpp
    geom/src/precision.cpp
    geom/src/visitor.cpp
    geom/src/spatial_index.cpp
)

target_compile_definitions(ogc_geometry PRIVATE OGC_GEOMETRY_EXPORTS)

target_include_directories(ogc_geometry
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/geom/include>
        $<INSTALL_INTERFACE:include>
)

if(USE_GEOS)
    find_package(GEOS REQUIRED)
    target_link_libraries(ogc_geometry PUBLIC GEOS::geos_c)
    target_compile_definitions(ogc_geometry PUBLIC USE_GEOS)
endif()

if(USE_PROJ)
    find_package(PROJ REQUIRED)
    target_link_libraries(ogc_geometry PUBLIC PROJ::proj)
    target_compile_definitions(ogc_geometry PUBLIC USE_PROJ)
endif()

if(ENABLE_TESTS)
    add_subdirectory(geom/tests)
endif()

install(TARGETS ogc_geometry
    EXPORT ogc_geometry-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY geom/include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT ogc_geometry-targets
    FILE ogc_geometry-config.cmake
    NAMESPACE OGC::
    DESTINATION lib/cmake/ogc_geometry
)
