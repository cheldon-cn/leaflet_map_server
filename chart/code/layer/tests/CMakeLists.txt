cmake_minimum_required(VERSION 3.10)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping layer tests")
    return()
endif()

message(STATUS "GoogleTest found for layer tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

include_directories(
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/code/feature/include
    ${CMAKE_SOURCE_DIR}/code/geom/include
)

set(TEST_SOURCES
    test_layer_type.cpp
    test_memory_layer.cpp
    test_layer_group.cpp
    test_layer_utils.cpp
    test_layer_infra.cpp
    test_layer_types_extra.cpp
    test_datasource_driver.cpp
    test_untested_classes.cpp
    test_layer_formats.cpp
    test_layer_feature_integration.cpp
    test_layer_boundary.cpp
    test_layer_performance.cpp
)

add_executable(ogc_layer_tests ${TEST_SOURCES})

set_target_properties(ogc_layer_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(ogc_layer_tests
    ogc_layer
    ogc_geometry
    ogc_feature
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
)

add_test(NAME ogc_layer_tests COMMAND ogc_layer_tests)
