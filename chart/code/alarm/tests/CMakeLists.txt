cmake_minimum_required(VERSION 3.10)

find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GTEST_INCLUDE_DIR OR NOT GTEST_LIBRARY OR NOT GTEST_MAIN_LIBRARY)
    message(STATUS "GoogleTest not found in ${GTEST_ROOT}, skipping alarm tests")
    return()
endif()

message(STATUS "GoogleTest found for alarm tests:")
message(STATUS "  Include: ${GTEST_INCLUDE_DIR}")
message(STATUS "  Library: ${GTEST_LIBRARY}")
message(STATUS "  Main: ${GTEST_MAIN_LIBRARY}")

set(TEST_SOURCES
    alert_types_test.cpp
    coordinate_test.cpp
    ship_info_test.cpp
    alert_test.cpp
    alert_context_test.cpp
    threshold_config_test.cpp
    alert_engine_factory_test.cpp
    depth_alert_engine_test.cpp
    collision_alert_engine_test.cpp
    weather_alert_engine_test.cpp
    channel_alert_engine_test.cpp
    alert_judge_service_test.cpp
    alert_push_service_test.cpp
    alert_config_service_test.cpp
    alert_data_access_test.cpp
    alert_repository_test.cpp
    external_data_gateway_test.cpp
    chart_data_adapter_test.cpp
    weather_data_adapter_test.cpp
    ais_data_adapter_test.cpp
    rest_controller_test.cpp
    websocket_service_test.cpp
    integration_test.cpp
    performance_test.cpp
)

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    
    target_include_directories(${TEST_NAME} PRIVATE
        ${GTEST_INCLUDE_DIR}
        ${ALERT_INCLUDE_DIR}
    )
    
    target_link_libraries(${TEST_NAME}
        alert_system
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
    )
    
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
