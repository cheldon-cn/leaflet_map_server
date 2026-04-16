cmake_minimum_required(VERSION 3.10)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include" CACHE PATH "GoogleTest include directory")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib" CACHE PATH "GoogleTest lib directory")

set(TEST_SOURCES
    test_main.cpp
    test_draw_result.cpp
    test_device_type.cpp
    test_engine_type.cpp
    test_draw_params.cpp
    test_draw_error.cpp
    test_tile_renderer.cpp
    test_image_draw.cpp
    test_clipper.cpp
    test_label_placement.cpp
    test_label_conflict.cpp
    test_label_engine.cpp
    test_layer_manager.cpp
    test_transform_manager.cpp
    test_rule_engine.cpp
    test_track_recorder.cpp
    test_lod.cpp
    test_lod_manager.cpp
    test_render_task.cpp
    test_render_queue.cpp
    test_async_renderer.cpp
    test_basic_render_task.cpp
    test_draw_facade.cpp
    test_graph_boundary.cpp
    test_graph_concurrent.cpp
    test_graph_performance.cpp
    test_it_basic_render.cpp
    test_it_transform_render.cpp
    test_it_tile_index.cpp
    test_it_draw_context.cpp
    test_it_image_output.cpp
    test_it_tile_render.cpp
    test_it_memory_cache.cpp
    test_it_disk_cache.cpp
    test_it_multi_level_cache.cpp
    test_it_line_symbolizer.cpp
    test_it_polygon_symbolizer.cpp
    test_it_point_symbolizer.cpp
    test_it_text_symbolizer.cpp
    test_it_layer_manager.cpp
    test_it_label_conflict.cpp
    test_it_clipper.cpp
    test_it_icon_symbolizer.cpp
    test_it_raster_symbolizer.cpp
    test_it_async_render.cpp
    test_it_rule_engine_render.cpp
    test_it_lod_render.cpp
    test_it_performance_monitor.cpp
    test_it_proj_transform_render.cpp
    test_it_pdf_output.cpp
    test_it_draw_facade.cpp
    test_it_sld_render.cpp
    test_it_mapbox_style_render.cpp
    test_interaction.cpp
    test_pan_zoom_handler.cpp
    test_selection_handler.cpp
    test_location_display_handler.cpp
    test_layer_control_panel.cpp
    test_day_night_mode_manager.cpp
    test_it_geometry.cpp
    test_it_database.cpp
    test_it_rendering.cpp
    test_it_end_to_end.cpp
    test_performance_benchmark.cpp
    test_memory_leak_detection.cpp
    test_acceptance.cpp
)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping graph tests")
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test")

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
    )
    
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${GTEST_INCLUDE_DIR}
            ${OGC_GRAPH_INCLUDE_DIR}
            ${GEOM_SOURCE_DIR}/include
            ${FEATURE_SOURCE_DIR}/include
            ${LAYER_SOURCE_DIR}/include
            ${DRAW_SOURCE_DIR}/include
            ${BASE_SOURCE_DIR}/include
            ${SYMBOLOGY_SOURCE_DIR}/include
            ${CACHE_SOURCE_DIR}/include
            ${PROJ_MODULE_SOURCE_DIR}/include
            ${NAVI_SOURCE_DIR}/include
    )
    
    target_link_libraries(${TEST_NAME}
        PRIVATE
            ogc_graph
            ogc_geometry
            ogc_draw
            ogc_proj
            ogc_cache
            ogc_symbology
            ogc_navi
            GTest::gtest
            GTest::gtest_main
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
