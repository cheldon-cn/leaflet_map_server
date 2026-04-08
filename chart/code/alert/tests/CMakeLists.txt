cmake_minimum_required(VERSION 3.10)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping alert tests")
    return()
endif()

message(STATUS "GoogleTest found for alert tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

set(TEST_SOURCES
    alert_test.cpp
    alert_extended_test.cpp
    integration_test.cpp
    alert_checker_test.cpp
    performance_test.cpp
    exception_path_test.cpp
    boundary_test.cpp
    cross_module_test.cpp
    test_data.cpp
)

add_executable(ogc_alert_test ${TEST_SOURCES})

target_include_directories(ogc_alert_test
    PRIVATE
        ${GTEST_INCLUDE_DIR}
        ${ALERT_INCLUDE_DIR}
        ${DATABASE_INCLUDE_DIR}
        ${GRAPH_INCLUDE_DIR}
        ${GEOM_INCLUDE_DIR}
)

target_link_libraries(ogc_alert_test
    PRIVATE
        ogc_alert
        ogc_geometry
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
)

set_target_properties(ogc_alert_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
)

enable_testing()
add_test(NAME ogc_alert_test COMMAND ogc_alert_test)

install(TARGETS ogc_alert_test
    RUNTIME DESTINATION bin
)
