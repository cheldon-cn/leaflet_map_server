cmake_minimum_required(VERSION 3.10)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib")

include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIB_DIR})

set(TEST_SOURCES
    alert_test.cpp
    alert_extended_test.cpp
    integration_test.cpp
    alert_checker_test.cpp
    performance_test.cpp
    exception_path_test.cpp
    boundary_test.cpp
    cross_module_test.cpp
    test_data.cpp
)

add_executable(ogc_alert_test ${TEST_SOURCES})

target_include_directories(ogc_alert_test
    PRIVATE
        ${ALERT_INCLUDE_DIR}
        ${DATABASE_INCLUDE_DIR}
        ${GRAPH_INCLUDE_DIR}
        ${GEOM_INCLUDE_DIR}
)

target_link_libraries(ogc_alert_test
    PRIVATE
        ogc_alert
        ogc_geometry
        gtest
        gtest_main
)

enable_testing()
add_test(NAME ogc_alert_test COMMAND ogc_alert_test)

install(TARGETS ogc_alert_test
    RUNTIME DESTINATION bin
)
