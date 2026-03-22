cmake_minimum_required(VERSION 3.10)

project(OGCGeometry VERSION 2.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
option(USE_GEOS "Enable GEOS backend for advanced operations" OFF)
option(USE_PROJ "Enable PROJ for coordinate transformation" OFF)
option(ENABLE_TESTS "Build unit tests" ON)
option(ENABLE_DATABASE "Build database module" ON)
option(ENABLE_GRAPH "Build graph module" ON)
option(BUILD_SHARED_LIBS "Build shared library" ON)
set(POSTGRESQL_ROOT "E:/Program Files/postgresql/16" CACHE PATH "PostgreSQL root directory")
set(SQLITE3_ROOT "E:/3rd/sqlite3-3.35.5" CACHE PATH "SQLite3 root directory")
set(GEOS_ROOT "E:/3rd/GEOS3.10" CACHE PATH "GEOS root directory")
set(PROJ_ROOT "E:/3rd/PROJ" CACHE PATH "PROJ root directory")
set(GTEST_ROOT "E:/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(DATABASE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/database" CACHE PATH "Database source directory")
set(FEATURE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/feature" CACHE PATH "Feature source directory")
set(GEOM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/geom" CACHE PATH "Geometry source directory")
set(LAYER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/layer" CACHE PATH "Layer source directory")
set(GRAPH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/graph" CACHE PATH "Graph source directory")
set(POSTGRESQL_INCLUDE_DIR "${POSTGRESQL_ROOT}/include" CACHE PATH "PostgreSQL include directory")
set(POSTGRESQL_LIB_DIR "${POSTGRESQL_ROOT}/lib" CACHE PATH "PostgreSQL lib directory")
set(SQLITE3_INCLUDE_DIR "${SQLITE3_ROOT}" CACHE PATH "SQLite3 include directory")
set(SQLITE3_LIB_DIR "${SQLITE3_ROOT}/lib(x64)" CACHE PATH "SQLite3 lib directory")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build/tests/Release" CACHE PATH "Installation directory" FORCE)
endif()

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

if(ENABLE_DATABASE)
    add_subdirectory(database)
endif()

if(ENABLE_GRAPH)
    add_subdirectory(graph)
endif()
if(ENABLE_TESTS)
    add_subdirectory(feature)
    add_subdirectory(feature/tests)
    add_subdirectory(geom/tests)
    add_subdirectory(layer)
    add_subdirectory(layer/tests)
    if(ENABLE_DATABASE)
        add_subdirectory(database/test)
    endif()
    if(ENABLE_GRAPH)
        add_subdirectory(graph/tests)
    endif()
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

install(DIRECTORY layer/include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
install(EXPORT ogc_geometry-targets
    FILE ogc_geometry-config.cmake
    NAMESPACE OGC::
    DESTINATION lib/cmake/ogc_geometry
)
