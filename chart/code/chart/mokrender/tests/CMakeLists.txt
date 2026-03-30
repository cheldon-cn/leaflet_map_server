cmake_minimum_required(VERSION 3.10)

project(ogc_mokrender_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TEST_SOURCES
    test_main.cpp
    test_data_generator.cpp
    test_point_generator.cpp
    test_line_generator.cpp
    test_polygon_generator.cpp
    test_annotation_generator.cpp
    test_raster_generator.cpp
    test_database_manager.cpp
    test_spatial_query.cpp
    test_symbolizer_factory.cpp
    test_render_context.cpp
    test_integration.cpp
    test_performance.cpp
)

add_executable(ogc_mokrender_tests ${TEST_SOURCES})

target_include_directories(ogc_mokrender_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${GTEST_ROOT}/include
)

target_link_libraries(ogc_mokrender_tests
    PRIVATE
        ogc_mokrender
        ${GTEST_ROOT}/lib/gtest.lib
        ${GTEST_ROOT}/lib/gtest_main.lib
)

set_target_properties(ogc_mokrender_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../temp"
)

if(MSVC)
    set_target_properties(ogc_mokrender_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../temp"
    )
endif()

enable_testing()

add_test(NAME MokRenderTests COMMAND ogc_mokrender_tests)
