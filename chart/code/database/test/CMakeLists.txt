cmake_minimum_required(VERSION 3.15)

project(ogc_database_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MDd")

set(GTEST_SOURCE_DIR "E:/program/trae/chart/code/build/_deps/googletest-src")
set(GTEST_LIB_DIR "E:/program/trae/chart/code/build/lib/Debug")
set(GTEST_BIN_DIR "E:/program/trae/chart/code/build/bin/Debug")

set(DATABASE_SOURCE_DIR "E:/program/trae/chart/code/database")
set(GEOM_SOURCE_DIR "E:/program/trae/chart/code/geom")

include_directories(
    ${DATABASE_SOURCE_DIR}/include
    ${GTEST_SOURCE_DIR}/googletest/include
    ${GTEST_SOURCE_DIR}/googlemock/include
    ${GEOM_SOURCE_DIR}/include
)

link_directories(
    ${GTEST_LIB_DIR}
    ${GEOM_SOURCE_DIR}/../build/Debug
)

set(TEST_SOURCES
    ${DATABASE_SOURCE_DIR}/test/wkb_converter_test.cpp
    ${DATABASE_SOURCE_DIR}/test/geojson_converter_test.cpp
)

add_executable(ogc_database_test ${TEST_SOURCES})

target_link_libraries(ogc_database_test PRIVATE
    ogc_geometry
    gtest
    gtest_main
)

target_include_directories(ogc_database_test PRIVATE
    ${DATABASE_SOURCE_DIR}/include
)

enable_testing()
add_test(NAME ogc_database_test COMMAND ogc_database_test)
