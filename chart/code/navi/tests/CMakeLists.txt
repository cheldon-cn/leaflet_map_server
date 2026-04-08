cmake_minimum_required(VERSION 3.10)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping navi tests")
    return()
endif()

message(STATUS "GoogleTest found for navi tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

add_executable(ogc_navi_tests
    test_nmea_parser.cpp
    test_position_filter.cpp
    test_coordinate_converter.cpp
    test_waypoint.cpp
    test_route.cpp
    test_navigation_calculator.cpp
    test_off_course_detector.cpp
    test_track_point.cpp
    test_track.cpp
    test_ais_target.cpp
    test_integration.cpp
    test_performance.cpp
)

target_include_directories(ogc_navi_tests PRIVATE ${GTEST_INCLUDE_DIR})

target_link_libraries(ogc_navi_tests
    PRIVATE
        ogc_navi
        ogc_geometry
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
)

set_target_properties(ogc_navi_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
)

enable_testing()
add_test(NAME ogc_navi_tests COMMAND ogc_navi_tests)

install(TARGETS ogc_navi_tests
    RUNTIME DESTINATION bin
)
