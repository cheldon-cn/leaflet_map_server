cmake_minimum_required(VERSION 3.16)

find_package(GTest REQUIRED)

set(TEST_SOURCES
    test_jni_bridge.cpp
)

add_executable(test_jni_bridge ${TEST_SOURCES})

target_link_libraries(test_jni_bridge
    PRIVATE
        ogc_jni_bridge
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
)

target_include_directories(test_jni_bridge
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

gtest_discover_tests(test_jni_bridge
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTIES
        LABELS "jni_bridge"
)
