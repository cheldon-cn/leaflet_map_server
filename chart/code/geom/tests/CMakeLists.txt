cmake_minimum_required(VERSION 3.10)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib")

include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})

add_executable(ogc_geometry_tests
    test_coordinate.cpp
    test_envelope.cpp
    test_point.cpp
    test_linestring.cpp
    test_polygon.cpp
    test_common.cpp
)

target_link_libraries(ogc_geometry_tests
    PRIVATE
        ogc_geometry
        gtest
        gtest_main
)

enable_testing()
add_test(NAME ogc_geometry_tests COMMAND ogc_geometry_tests)

install(TARGETS ogc_geometry_tests
    RUNTIME DESTINATION bin
)
