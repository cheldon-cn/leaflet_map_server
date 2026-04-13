cmake_minimum_required(VERSION 3.10)

project(ogc_chart_c_api VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

option(OGC_CHART_C_API_STATIC "Build static library" OFF)

if(OGC_CHART_C_API_STATIC)
    add_definitions(-DOGC_CHART_C_API_STATIC)
endif()

set(OGC_CHART_C_API_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INSTALL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../install/include")
set(INSTALL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../install/lib")

set(CHART_PARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../chart/parser/include")

set(POSTGRESQL_ROOT "E:/Program Files/postgresql/16" CACHE PATH "PostgreSQL root directory")
set(SQLITE3_ROOT "E:/xspace/3rd/sqlite3-3.35.5" CACHE PATH "SQLite3 root directory")
set(GEOS_ROOT "E:/xspace/3rd/GEOS3.10" CACHE PATH "GEOS root directory")
set(PROJ_ROOT "E:/xspace/3rd/PROJ" CACHE PATH "PROJ root directory")
set(CURL_ROOT "E:/xspace/3rd/libcurl_x64" CACHE PATH "cURL library root directory")
set(GTEST_ROOT "E:/xspace/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(GDAL_ROOT "E:/xspace/3rd/gdal-3.9.3" CACHE PATH "GDAL root directory")
set(LIBXML2_ROOT "E:/xspace/3rd/libxml2" CACHE PATH "libxml2 root directory")

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/test")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/test")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib")

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BUILD_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BUILD_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BUILD_DIR}/lib" CACHE PATH "" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BUILD_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BUILD_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BUILD_DIR}/lib" CACHE PATH "" FORCE)
endif()

set(CHART_C_API_SOURCES
    sdk_c_api_common.cpp
    sdk_c_api_version.cpp
    sdk_c_api_base.cpp
    sdk_c_api_geom.cpp
    sdk_c_api_feature.cpp
    sdk_c_api_layer.cpp
    sdk_c_api_draw.cpp
    sdk_c_api_graph.cpp
    sdk_c_api_cache.cpp
    sdk_c_api_symbology.cpp
    sdk_c_api_alert.cpp
    sdk_c_api_navi.cpp
    sdk_c_api_proj.cpp
    sdk_c_api_parser.cpp
    sdk_c_api_plugin.cpp
)

set(CHART_C_API_HEADERS
    sdk_c_api.h
    sdk_c_api_export.h
)

if(OGC_CHART_C_API_STATIC)
    add_library(ogc_chart_c_api STATIC ${CHART_C_API_SOURCES} ${CHART_C_API_HEADERS})
else()
    add_library(ogc_chart_c_api SHARED ${CHART_C_API_SOURCES} ${CHART_C_API_HEADERS})
    target_compile_definitions(ogc_chart_c_api PRIVATE OGC_CHART_C_API_EXPORTS)
endif()

target_include_directories(ogc_chart_c_api
    PUBLIC
        $<BUILD_INTERFACE:${OGC_CHART_C_API_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${INSTALL_INCLUDE_DIR}
        ${CHART_PARSER_INCLUDE_DIR}
)

set(OGC_LIB_DIR ${CMAKE_SOURCE_DIR}/../../build/lib)
set(GDAL_LIB_DIR "${GDAL_ROOT}/lib")
target_link_directories(ogc_chart_c_api PRIVATE 
    ${OGC_LIB_DIR}
    ${INSTALL_LIB_DIR}
    ${GDAL_LIB_DIR}
)

target_link_libraries(ogc_chart_c_api
    PRIVATE
        ogc_base
        ogc_geometry
        ogc_feature
        ogc_layer
        ogc_draw
        ogc_graph
        ogc_cache
        ogc_symbology
        ogc_alert
        ogc_navi
        ogc_proj
        chart_parser
        gdal
)

set_target_properties(ogc_chart_c_api PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "ogc_chart_c_api"
)

if(MSVC_IDE)
    set_target_properties(ogc_chart_c_api PROPERTIES
        FOLDER "Chart C API"
    )
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()

install(TARGETS ogc_chart_c_api
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${CHART_C_API_HEADERS}
    DESTINATION include
)
