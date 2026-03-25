cmake_minimum_required(VERSION 3.15)

project(ogc_database_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib")

include_directories(
    ${DATABASE_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIR}
    ${GEOM_SOURCE_DIR}/include
    ${POSTGRESQL_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIR}
)

link_directories(
    ${GTEST_LIB_DIR}
    ${POSTGRESQL_LIB_DIR}
    ${SQLITE3_LIB_DIR}
)

set(TEST_SOURCES
    ${DATABASE_SOURCE_DIR}/test/wkb_converter_test.cpp
    ${DATABASE_SOURCE_DIR}/test/geojson_converter_test.cpp
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
    gtest
    gtest_main
)

enable_testing()
add_test(NAME ogc_database_test COMMAND ogc_database_test)

install(TARGETS ogc_database_test
    RUNTIME DESTINATION bin
)
