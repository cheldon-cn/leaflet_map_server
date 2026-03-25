cmake_minimum_required(VERSION 3.10)


include_directories(
    ${GTEST_ROOT}/include
    ${GTEST_ROOT}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/code/feature/include
    ${CMAKE_SOURCE_DIR}/code/geom/include
)

link_directories(
    ${GTEST_ROOT}/lib
)

set(TEST_SOURCES
    test_layer_type.cpp
    # test_utils.cpp
    test_memory_layer.cpp
    test_layer_group.cpp
    test_layer_utils.cpp
    test_layer_infra.cpp
    test_layer_types_extra.cpp
    test_datasource_driver.cpp
    test_untested_classes.cpp
    test_layer_formats.cpp
    test_layer_feature_integration.cpp
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
    gtest
    gtest_main
)

set_target_properties(ogc_layer_tests PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

add_test(NAME ogc_layer_tests COMMAND ogc_layer_tests)
