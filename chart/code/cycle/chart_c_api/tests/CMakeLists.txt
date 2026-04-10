cmake_minimum_required(VERSION 3.10)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping chart_c_api tests")
    return()
endif()

message(STATUS "GoogleTest found for chart_c_api tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

include_directories(
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/install/include
)

set(TEST_SOURCES
    test_sdk_c_api_common.cpp
    test_sdk_c_api_geom.cpp
    test_sdk_c_api_feature.cpp
    test_sdk_c_api_layer.cpp
    test_sdk_c_api_cache.cpp
    test_sdk_c_api_alert.cpp
    test_sdk_c_api_navigation.cpp
)

add_executable(sdk_c_api_tests ${TEST_SOURCES})

set_target_properties(sdk_c_api_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(sdk_c_api_tests
    ogc_chart_c_api
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
)

add_test(NAME sdk_c_api_tests COMMAND sdk_c_api_tests)
