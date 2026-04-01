cmake_minimum_required(VERSION 3.10)

option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

set(GTEST_ROOT "E:/xspace/3rd/googletest" CACHE PATH "GoogleTest root directory")

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
    PATHS ${GTEST_ROOT}/include
    PATH_SUFFIXES include
)

find_library(GTEST_LIBRARY NAMES gtest gtestd
    PATHS ${GTEST_ROOT}/lib
    PATH_SUFFIXES lib lib/Release lib/Debug
)

find_library(GTEST_MAIN_LIBRARY NAMES gtest_main gtest_maind
    PATHS ${GTEST_ROOT}/lib
    PATH_SUFFIXES lib lib/Release lib/Debug
)

if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY AND GTEST_MAIN_LIBRARY)
    message(STATUS "Found GoogleTest: ${GTEST_INCLUDE_DIR}")
    message(STATUS "GTEST_LIBRARY: ${GTEST_LIBRARY}")
    message(STATUS "GTEST_MAIN_LIBRARY: ${GTEST_MAIN_LIBRARY}")
    set(GTest_FOUND TRUE)
    add_library(GTest::gtest STATIC IMPORTED)
    set_target_properties(GTest::gtest PROPERTIES
        IMPORTED_LOCATION "${GTEST_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
    )
    add_library(GTest::gtest_main STATIC IMPORTED)
    set_target_properties(GTest::gtest_main PROPERTIES
        IMPORTED_LOCATION "${GTEST_MAIN_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
    )
else()
    message(STATUS "GoogleTest not found, tests will be skipped")
    message(STATUS "GTEST_INCLUDE_DIR: ${GTEST_INCLUDE_DIR}")
    message(STATUS "GTEST_LIBRARY: ${GTEST_LIBRARY}")
    message(STATUS "GTEST_MAIN_LIBRARY: ${GTEST_MAIN_LIBRARY}")
    set(GTest_FOUND FALSE)
endif()

if(NOT GTest_FOUND)
    return()
endif()

set(TEST_SOURCES
    test_color.cpp
    test_geometry_types.cpp
    test_draw_types.cpp
    test_transform_matrix.cpp
    test_draw_style.cpp
    test_geometry.cpp
    test_draw_result.cpp
    test_state_serializer.cpp
    test_raster_image_device.cpp
    test_display_device.cpp
    test_simple2d_engine.cpp
    test_tile_device.cpp
    test_tile_based_engine.cpp
    test_tile_size_strategy.cpp
    test_async_render_task.cpp
    test_async_render_manager.cpp
    test_svg.cpp
    test_gpu_resource_manager.cpp
    test_render_memory_pool.cpp
    test_gpu_accelerated_engine.cpp
    test_texture_cache.cpp
    test_gpu_resource_wrapper.cpp
    test_thread_safe_engine.cpp
    test_pdf_device.cpp
    test_pdf_engine.cpp
    test_capability_negotiator.cpp
    test_gpu_device_selector.cpp
    test_draw_scope_guard.cpp
    test_library_compatibility.cpp
    test_qt_engine.cpp
    test_qt_display_device.cpp
    test_qt_platform.cpp
    test_gdiplus_engine.cpp
    test_direct2d_engine.cpp
    test_windows_platform.cpp
    test_plugin_manager.cpp
    test_draw_version.cpp
    test_engine_pool.cpp
    test_performance.cpp
    test_cairo_engine.cpp
    test_coregraphics_engine.cpp
    test_metal_engine.cpp
    test_webgl_engine.cpp
    test_boundary_values.cpp
)

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    
    set(FULL_TEST_NAME "draw_${TEST_NAME}")
    
    add_executable(${FULL_TEST_NAME} ${TEST_SRC})
    
    if(ENABLE_COVERAGE)
        target_compile_options(${FULL_TEST_NAME} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${FULL_TEST_NAME} PRIVATE --coverage)
    endif()
    
    target_link_libraries(${FULL_TEST_NAME}
        ogc_draw
        GTest::gtest
        GTest::gtest_main
    )
    
    target_include_directories(${FULL_TEST_NAME} PRIVATE
        ${OGC_DRAW_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIR}
    )
    
    add_test(NAME ${FULL_TEST_NAME} COMMAND ${FULL_TEST_NAME})
endforeach()

if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            COMMAND ctest --output-on-failure
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '*/tests/*' --output-file coverage.info
            COMMAND ${GENHTML_PATH} -o coverage_report coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()
