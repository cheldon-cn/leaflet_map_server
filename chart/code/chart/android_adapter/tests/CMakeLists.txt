cmake_minimum_required(VERSION 3.16)

find_package(GTest REQUIRED)

set(TEST_SOURCES
    test_android_adapter.cpp
)

add_executable(test_android_adapter ${TEST_SOURCES})

target_link_libraries(test_android_adapter
    PRIVATE
        ogc_android_adapter
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
)

target_include_directories(test_android_adapter
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

gtest_discover_tests(test_android_adapter
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTIES
        LABELS "android_adapter"
)
