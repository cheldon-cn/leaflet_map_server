cmake_minimum_required(VERSION 3.10)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping feature tests")
    return()
endif()

message(STATUS "GoogleTest found for feature tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/code/feature/include)
include_directories(${CMAKE_SOURCE_DIR}/code/geom/include)

add_executable(ogc_feature_tests
    test_field_value.cpp
    test_field_type.cpp
    test_datetime.cpp
    test_feature_defn.cpp
    test_feature.cpp
    test_feature_collection.cpp
    test_field_defn.cpp
    test_geom_field_defn.cpp
    test_batch_processor.cpp
    test_feature_guard.cpp
    test_feature_integration.cpp
    test_feature_iterator.cpp
    test_geojson_converter.cpp
    test_spatial_query.cpp
    test_wkb_wkt_converter.cpp
)

set_target_properties(ogc_feature_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
)

target_link_libraries(ogc_feature_tests
    PRIVATE
        ogc_feature
        ogc_geometry
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
)

enable_testing()
add_test(NAME ogc_feature_tests COMMAND ogc_feature_tests)

install(TARGETS ogc_feature_tests
    RUNTIME DESTINATION bin
)
