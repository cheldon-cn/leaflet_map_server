cmake_minimum_required(VERSION 3.10)

project(AlertSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS /W3)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo;Debug" CACHE STRING "" FORCE)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test" CACHE PATH "Output directory for executables")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test" CACHE PATH "Output directory for libraries")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for archives")
endif()

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib" CACHE PATH "" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib" CACHE PATH "" FORCE)
endif()

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(ENABLE_TESTS "Build unit tests" ON)

set(ALERT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "Alert include directory")
set(ALERT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "Alert source directory")

set(POSTGRESQL_ROOT "E:/Program Files/postgresql/16" CACHE PATH "PostgreSQL root directory")
set(SQLITE3_ROOT "E:/xspace/3rd/sqlite3-3.35.5" CACHE PATH "SQLite3 root directory")
set(GEOS_ROOT "E:/xspace/3rd/GEOS3.10" CACHE PATH "GEOS root directory")
set(PROJ_ROOT "E:/xspace/3rd/PROJ" CACHE PATH "PROJ root directory")
set(GTEST_ROOT "E:/xspace/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(BOOST_ROOT "E:/xspace/3rd/boost_1_78_0" CACHE PATH "Boost root directory")
set(CURL_ROOT "E:/xspace/3rd/curl" CACHE PATH "CURL root directory")
set(JSON_ROOT "E:/xspace/3rd/nlohmann_json" CACHE PATH "nlohmann_json root directory")
set(JWT_ROOT "F:/cycle/server/thirdparty/jwt-cpp-v0.7.2" CACHE PATH "jwt-cpp root directory")
set(OPENSSL_ROOT "E:/xspace/3rd/openssl3" CACHE PATH "OpenSSL root directory")
set(GDAL_ROOT "E:/xspace/3rd/gdal-3.9.3" CACHE PATH "GDAL root directory")
set(NGINX_ROOT "E:/Program Files/nginx-1.28.0" CACHE PATH "Nginx root directory")

set(HIREDIS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../3rd/hiredis" CACHE PATH "hiredis root directory")
set(AMQPCPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../3rd/amqp-cpp" CACHE PATH "amqp-cpp root directory")
set(WEBSOCKETPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../3rd/websocketpp" CACHE PATH "websocketpp root directory")
set(THIRDPARTY_TEMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../3rd/temp" CACHE PATH "Third-party build temp directory")

set(OGC_GEOMETRY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE PATH "OGC Geometry directory")
set(OGC_ALERT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../alert" CACHE PATH "OGC Alert directory")

include_directories(${ALERT_INCLUDE_DIR})
include_directories(${OGC_GEOMETRY_DIR}/geom/include)
include_directories(${OGC_ALERT_DIR}/include)

add_library(alert_system
    ${ALERT_SOURCE_DIR}/alert_types.cpp
    ${ALERT_SOURCE_DIR}/alert_exception.cpp
    ${ALERT_SOURCE_DIR}/coordinate.cpp
    ${ALERT_SOURCE_DIR}/ship_info.cpp
    ${ALERT_SOURCE_DIR}/alert.cpp
    ${ALERT_SOURCE_DIR}/alert_context.cpp
    ${ALERT_SOURCE_DIR}/threshold_config.cpp
    ${ALERT_SOURCE_DIR}/i_alert_engine.cpp
    ${ALERT_SOURCE_DIR}/alert_engine_factory.cpp
    ${ALERT_SOURCE_DIR}/depth_alert_engine.cpp
    ${ALERT_SOURCE_DIR}/collision_alert_engine.cpp
    ${ALERT_SOURCE_DIR}/weather_alert_engine.cpp
    ${ALERT_SOURCE_DIR}/channel_alert_engine.cpp
    ${ALERT_SOURCE_DIR}/alert_judge_service.cpp
    ${ALERT_SOURCE_DIR}/alert_push_service.cpp
    ${ALERT_SOURCE_DIR}/alert_config_service.cpp
    ${ALERT_SOURCE_DIR}/alert_data_access.cpp
    ${ALERT_SOURCE_DIR}/alert_repository.cpp
    ${ALERT_SOURCE_DIR}/external_data_gateway.cpp
    ${ALERT_SOURCE_DIR}/chart_data_adapter.cpp
    ${ALERT_SOURCE_DIR}/weather_data_adapter.cpp
    ${ALERT_SOURCE_DIR}/ais_data_adapter.cpp
    ${ALERT_SOURCE_DIR}/rest_controller.cpp
    ${ALERT_SOURCE_DIR}/websocket_service.cpp
    ${ALERT_SOURCE_DIR}/checker_adapter.cpp
    ${ALERT_SOURCE_DIR}/push_channel_adapter.cpp
    ${ALERT_SOURCE_DIR}/rest_adapter.cpp
    ${ALERT_SOURCE_DIR}/repository_adapter.cpp
)

target_compile_definitions(alert_system PRIVATE ALERT_SYSTEM_EXPORTS)

if(BUILD_SHARED_LIBS)
    set_target_properties(alert_system PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

target_include_directories(alert_system
    PUBLIC
        $<BUILD_INTERFACE:${ALERT_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(alert_system
    PUBLIC
        ogc_alert
        ogc_geometry
)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
