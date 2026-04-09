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
if(NOT DEFINED INSTALL_INCLUDE_DIR)
    set(INSTALL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../install/include)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
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
)

target_link_libraries(ogc_chart_c_api
    PRIVATE
        ogc_base
        ogc_geom
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
        ogc_plugin
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
    EXPORT ogc_chart_c_api-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${CHART_C_API_HEADERS}
    DESTINATION include
)

install(EXPORT ogc_chart_c_api-targets
    FILE ogc_chart_c_api-config.cmake
    NAMESPACE ogc::
    DESTINATION lib/cmake/ogc_chart_c_api
)
