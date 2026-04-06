cmake_minimum_required(VERSION 3.10)

project(ogc_alert
    VERSION 1.0.0
    DESCRIPTION "OGC Alert System Library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
endif()

set(POSTGRESQL_ROOT "E:/Program Files/postgresql/16" CACHE PATH "PostgreSQL root directory")
set(SQLITE3_ROOT "E:/xspace/3rd/sqlite3-3.35.5" CACHE PATH "SQLite3 root directory")
set(GEOS_ROOT "E:/xspace/3rd/GEOS3.10" CACHE PATH "GEOS root directory")
set(GTEST_ROOT "E:/xspace/3rd/googletest" CACHE PATH "GoogleTest root directory")

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

set(ALERT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(DATABASE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../database/include")
set(GRAPH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../graph/include")
set(GEOM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../geom/include")

set(ALERT_SOURCES
    src/types.cpp
    src/alert_engine.cpp
    src/alert_checker.cpp
    src/alert_processor.cpp
    src/scheduler.cpp
    src/data_source_manager.cpp
    src/alert_repository.cpp
    src/push_service.cpp
    src/config_service.cpp
    src/query_service.cpp
    src/acknowledge_service.cpp
    src/depth_alert_checker.cpp
    src/collision_alert_checker.cpp
    src/weather_alert_checker.cpp
    src/channel_alert_checker.cpp
    src/deviation_alert_checker.cpp
    src/speed_alert_checker.cpp
    src/restricted_area_checker.cpp
    src/ukc_calculator.cpp
    src/cpa_calculator.cpp
    src/deduplicator.cpp
    src/threshold_manager.cpp
    src/rest_controller.cpp
    src/weather_fusion.cpp
    src/notice_parser.cpp
    src/deviation_calculator.cpp
    src/speed_zone_matcher.cpp
    src/spatial_distance.cpp
    src/push_strategy.cpp
    src/user_config_store.cpp
    src/websocket_service.cpp
    src/feedback_service.cpp
    src/performance_benchmark.cpp
)

set(ALERT_HEADERS
    include/ogc/alert/export.h
    include/ogc/alert/types.h
    include/ogc/alert/alert_engine.h
    include/ogc/alert/alert_checker.h
    include/ogc/alert/alert_processor.h
    include/ogc/alert/scheduler.h
    include/ogc/alert/data_source_manager.h
    include/ogc/alert/alert_repository.h
    include/ogc/alert/push_service.h
    include/ogc/alert/config_service.h
    include/ogc/alert/query_service.h
    include/ogc/alert/acknowledge_service.h
    include/ogc/alert/depth_alert_checker.h
    include/ogc/alert/collision_alert_checker.h
    include/ogc/alert/weather_alert_checker.h
    include/ogc/alert/channel_alert_checker.h
    include/ogc/alert/deviation_alert_checker.h
    include/ogc/alert/speed_alert_checker.h
    include/ogc/alert/restricted_area_checker.h
    include/ogc/alert/cache.h
    include/ogc/alert/exception.h
    include/ogc/alert/ukc_calculator.h
    include/ogc/alert/cpa_calculator.h
    include/ogc/alert/deduplicator.h
    include/ogc/alert/threshold_manager.h
    include/ogc/alert/rest_controller.h
    include/ogc/alert/weather_fusion.h
    include/ogc/alert/notice_parser.h
    include/ogc/alert/deviation_calculator.h
    include/ogc/alert/speed_zone_matcher.h
    include/ogc/alert/spatial_distance.h
    include/ogc/alert/push_strategy.h
    include/ogc/alert/user_config_store.h
    include/ogc/alert/websocket_service.h
    include/ogc/alert/feedback_service.h
    include/ogc/alert/performance_benchmark.h
)

add_library(ogc_alert SHARED ${ALERT_SOURCES} ${ALERT_HEADERS})

set_target_properties(ogc_alert PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(MSVC)
    set_target_properties(ogc_alert PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib"
    )
endif()

target_include_directories(ogc_alert
    PUBLIC
        $<BUILD_INTERFACE:${ALERT_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(ogc_alert
    PRIVATE
        ${DATABASE_INCLUDE_DIR}
        ${GRAPH_INCLUDE_DIR}
        ${GEOM_INCLUDE_DIR}
        ${POSTGRESQL_ROOT}/include
        ${SQLITE3_ROOT}/include
        ${GEOS_ROOT}/include
)

target_compile_definitions(ogc_alert PRIVATE OGC_ALERT_EXPORTS)

find_library(SQLITE3_LIB NAMES sqlite3 sqlite3_lib
    PATHS ${SQLITE3_ROOT}/lib ${SQLITE3_ROOT}
    NO_DEFAULT_PATH
)

find_library(PQ_LIB NAMES pq libpq
    PATHS ${POSTGRESQL_ROOT}/lib
    NO_DEFAULT_PATH
)

find_library(GEOS_LIB NAMES geos geos_c
    PATHS ${GEOS_ROOT}/lib
    NO_DEFAULT_PATH
)

target_link_libraries(ogc_alert
    PUBLIC
        ogc_geometry
    PRIVATE
        ogc_graph
        ogc_base
        ${SQLITE3_LIB}
        ${PQ_LIB}
        ${GEOS_LIB}
)

install(TARGETS ogc_alert
    EXPORT ogc_alert-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ogc/alert/
    DESTINATION include/ogc/alert
    FILES_MATCHING PATTERN "*.h"
)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
