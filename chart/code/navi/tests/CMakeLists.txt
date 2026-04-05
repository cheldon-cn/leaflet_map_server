cmake_minimum_required(VERSION 3.10)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib")

include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})

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

target_link_libraries(ogc_navi_tests
    PRIVATE
        ogc_navi
        ogc_geometry
        gtest
        gtest_main
)

enable_testing()
add_test(NAME ogc_navi_tests COMMAND ogc_navi_tests)

install(TARGETS ogc_navi_tests
    RUNTIME DESTINATION bin
)
