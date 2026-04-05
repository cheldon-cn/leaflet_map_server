cmake_minimum_required(VERSION 3.10)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/googletest/include" CACHE PATH "GoogleTest include directory")
set(GTEST_LIB_DIR "${GTEST_ROOT}/googletest/lib" CACHE PATH "GoogleTest library directory")

find_path(GTEST_INCLUDE NAMES gtest/gtest.h PATHS ${GTEST_INCLUDE_DIR})
find_library(GTEST_LIB NAMES gtest PATHS ${GTEST_LIB_DIR})
find_library(GTEST_MAIN_LIB NAMES gtest_main PATHS ${GTEST_LIB_DIR})

if(NOT GTEST_INCLUDE OR NOT GTEST_LIB OR NOT GTEST_MAIN_LIB)
    message(STATUS "GoogleTest not found, using fetched version")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

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
)

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    
    target_link_libraries(${TEST_NAME}
        alert_system
        gtest
        gtest_main
    )
    
    target_include_directories(${TEST_NAME} PRIVATE
        ${ALERT_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIR}
    )
    
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    )
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
