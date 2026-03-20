cmake_minimum_required(VERSION 3.10)

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "GTest not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(ogc_geometry_tests
    test_coordinate.cpp
    test_envelope.cpp
    test_point.cpp
    test_linestring.cpp
    test_polygon.cpp
)

target_link_libraries(ogc_geometry_tests
    PRIVATE
        ogc_geometry
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ogc_geometry_tests)
