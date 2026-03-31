cmake_minimum_required(VERSION 3.16)

set(TEST_SOURCES
    test_main.cpp
    test_s57_parser.cpp
    test_s57_geometry_converter.cpp
    test_s57_attribute_parser.cpp
    test_parse_result.cpp
    test_parse_cache.cpp
    test_incremental_parser.cpp
    test_s100_s102_parser.cpp
    test_integration.cpp
    test_performance_benchmark.cpp
    test_gdal_initializer.cpp
    test_parse_config.cpp
    test_error_handler.cpp
    test_s57_feature_type_mapper.cpp
    test_data_converter.cpp
    test_performance.cpp
)

if(LIBXML2_FOUND)
    list(APPEND TEST_SOURCES
        test_s101_parser.cpp
        test_s101_gml_parser.cpp
    )
endif()

add_executable(chart_parser_tests ${TEST_SOURCES})

target_link_libraries(chart_parser_tests
    PRIVATE
        chart_parser
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
)

target_include_directories(chart_parser_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${GDAL_INCLUDE_DIR}
)

if(ENABLE_COVERAGE)
    target_link_libraries(chart_parser_tests PRIVATE gcov)
endif()

include(GoogleTest)
gtest_discover_tests(chart_parser_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PROPERTIES
        LABELS "unit"
)
