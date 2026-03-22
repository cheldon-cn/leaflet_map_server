cmake_minimum_required(VERSION 3.10)

set(GTEST_ROOT "E:/xspace/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include" CACHE PATH "GoogleTest include directory")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib" CACHE PATH "GoogleTest lib directory")

set(TEST_SOURCES
    test_main.cpp
    test_draw_result.cpp
    test_device_type.cpp
    test_engine_type.cpp
    test_draw_params.cpp
    test_draw_style.cpp
    test_color.cpp
    test_font.cpp
    test_transform_matrix.cpp
    test_draw_error.cpp
    test_tile_key.cpp
    test_raster_image_device.cpp
    test_draw_context.cpp
    test_tile_device.cpp
    test_coordinate_transform.cpp
    test_tile_cache.cpp
    test_disk_tile_cache.cpp
    test_multi_level_tile_cache.cpp
    test_tile_renderer.cpp
    test_symbolizer.cpp
    test_line_symbolizer.cpp
    test_polygon_symbolizer.cpp
    test_point_symbolizer.cpp
    test_text_symbolizer.cpp
    test_device_state.cpp
    test_image_draw.cpp
    test_it_basic_render.cpp
    test_it_transform_render.cpp
    test_it_tile_index.cpp
    test_it_draw_context.cpp
    test_it_image_output.cpp
)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping graph tests")
    message(STATUS "Please ensure GoogleTest is installed at E:/xspace/3rd/googletest")
    message(STATUS "Expected structure:")
    message(STATUS "  E:/xspace/3rd/googletest/include/gtest/gtest.h")
    message(STATUS "  E:/xspace/3rd/googletest/lib/gtest.lib")
    message(STATUS "  E:/xspace/3rd/googletest/lib/gtest_main.lib")
    return()
endif()

message(STATUS "GoogleTest found:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

add_library(GTest::gtest STATIC IMPORTED)
set_target_properties(GTest::gtest PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
    IMPORTED_LOCATION "${GTEST_LIBRARY}"
)

add_library(GTest::gtest_main STATIC IMPORTED)
set_target_properties(GTest::gtest_main PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
    IMPORTED_LOCATION "${GTEST_MAIN_LIBRARY}"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release")

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/tests/Release"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/tests/Release"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/tests/Release"
    )
    
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${GTEST_INCLUDE_DIR}
            ${OGC_GRAPH_INCLUDE_DIR}
            ${GEOM_SOURCE_DIR}/include
            ${FEATURE_SOURCE_DIR}/include
            ${LAYER_SOURCE_DIR}/include
    )
    
    target_link_libraries(${TEST_NAME}
        PRIVATE
            ogc_graph
            GTest::gtest
            GTest::gtest_main
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

if(ENABLE_COVERAGE)
    add_custom_target(coverage
        COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
        COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
        COMMAND genhtml coverage.info --output-directory coverage_report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report"
    )
endif()
