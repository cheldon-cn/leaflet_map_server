cmake_minimum_required(VERSION 3.15)

project(ogc_database_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping database tests")
    return()
endif()

message(STATUS "GoogleTest found for database tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

include_directories(
    ${DATABASE_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIR}
    ${GEOM_SOURCE_DIR}/include
    ${POSTGRESQL_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIR}
)

set(TEST_SOURCES
    ${DATABASE_SOURCE_DIR}/test/wkb_converter_test.cpp
    ${DATABASE_SOURCE_DIR}/test/geojson_converter_test.cpp
    ${DATABASE_SOURCE_DIR}/test/async_connection_test.cpp
    ${DATABASE_SOURCE_DIR}/test/connection_pool_test.cpp
    ${DATABASE_SOURCE_DIR}/test/integration_test.cpp
    ${DATABASE_SOURCE_DIR}/test/performance_test.cpp
)

add_executable(ogc_database_test ${TEST_SOURCES})

set_target_properties(ogc_database_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
)

target_compile_definitions(ogc_database_test PRIVATE OGC_DATABASE_EXPORTS)

target_link_libraries(ogc_database_test PRIVATE
    ogc_database
    ogc_geometry
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
)

enable_testing()
add_test(NAME ogc_database_test COMMAND ogc_database_test)

install(TARGETS ogc_database_test
    RUNTIME DESTINATION bin
)
