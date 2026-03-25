cmake_minimum_required(VERSION 3.10)

set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIB_DIR "${GTEST_ROOT}/lib")

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/code/feature/include)
include_directories(${CMAKE_SOURCE_DIR}/code/geom/include)
link_directories(${GTEST_LIB_DIR})

add_executable(ogc_feature_tests
    test_field_value.cpp
    test_field_type.cpp
    test_datetime.cpp
    test_feature_defn.cpp
    test_feature.cpp
    test_feature_collection.cpp
)

set_target_properties(ogc_feature_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/test"
)

target_link_libraries(ogc_feature_tests
    PRIVATE
        ogc_feature
        ogc_geometry
        gtest
        gtest_main
)

enable_testing()
add_test(NAME ogc_feature_tests COMMAND ogc_feature_tests)

install(TARGETS ogc_feature_tests
    RUNTIME DESTINATION bin
)
