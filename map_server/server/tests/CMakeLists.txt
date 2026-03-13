# 测试配置
cmake_minimum_required(VERSION 3.10)
project(cycle-map-server-tests)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译器选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/thirdparty
    ${CMAKE_SOURCE_DIR}/thirdparty/jwt-cpp-v0.7.2/include
)

# 设置第三方库路径
set(THIRD_PARTY_DIR "F:/win/3rd")

# 查找依赖库
# SQLite3
find_path(SQLITE3_INCLUDE_DIR sqlite3.h
    PATHS ${THIRD_PARTY_DIR}/sqlite3/include
    NO_DEFAULT_PATH
)
find_library(SQLITE3_LIBRARY sqlite3
    PATHS ${THIRD_PARTY_DIR}/sqlite3/lib
    NO_DEFAULT_PATH
)

# PNG
find_path(PNG_INCLUDE_DIR png.h
    PATHS ${THIRD_PARTY_DIR}/libpng-1.6.37/include
    NO_DEFAULT_PATH
)
find_library(PNG_LIBRARY libpng16
    PATHS ${THIRD_PARTY_DIR}/libpng-1.6.37/lib
    NO_DEFAULT_PATH
)

# 查找 Google Test
find_package(GTest REQUIRED)

# 测试源文件（与实际文件匹配）
set(TEST_SOURCES
    test_config.cpp
    test_config_minimal.cpp
    test_config_encryption.cpp
    test_logger.cpp
    test_logger_minimal.cpp
    test_logger_simple.cpp
    test_auth.cpp
    test_cache.cpp
    test_renderer.cpp
    test_service.cpp
    test_security.cpp
    test_https_security.cpp
    test_performance.cpp
    test_performance_optimization.cpp
    test_integration.cpp
    test_integration_secure.cpp
)

# 创建测试可执行文件
add_executable(cycle-map-server-tests ${TEST_SOURCES})

# 链接库
target_link_libraries(cycle-map-server-tests
    GTest::gtest
    GTest::gtest_main
    ${SQLITE3_LIBRARY}
    ${PNG_LIBRARY}
)

# 添加测试
add_test(NAME config_test COMMAND cycle-map-server-tests --gtest_filter=ConfigTest.*)
add_test(NAME config_minimal_test COMMAND cycle-map-server-tests --gtest_filter=ConfigMinimalTest.*)
add_test(NAME config_encryption_test COMMAND cycle-map-server-tests --gtest_filter=ConfigEncryptionTest.*)
add_test(NAME logger_test COMMAND cycle-map-server-tests --gtest_filter=LoggerTest.*)
add_test(NAME logger_minimal_test COMMAND cycle-map-server-tests --gtest_filter=LoggerMinimalTest.*)
add_test(NAME logger_simple_test COMMAND cycle-map-server-tests --gtest_filter=LoggerSimpleTest.*)
add_test(NAME auth_test COMMAND cycle-map-server-tests --gtest_filter=AuthTest.*)
add_test(NAME cache_test COMMAND cycle-map-server-tests --gtest_filter=CacheTest.*)
add_test(NAME renderer_test COMMAND cycle-map-server-tests --gtest_filter=RendererTest.*)
add_test(NAME service_test COMMAND cycle-map-server-tests --gtest_filter=ServiceTest.*)
add_test(NAME security_test COMMAND cycle-map-server-tests --gtest_filter=SecurityTest.*)
add_test(NAME https_security_test COMMAND cycle-map-server-tests --gtest_filter=HttpsSecurityTest.*)
add_test(NAME performance_test COMMAND cycle-map-server-tests --gtest_filter=PerformanceTest.*)
add_test(NAME performance_optimization_test COMMAND cycle-map-server-tests --gtest_filter=PerformanceOptimizationTest.*)
add_test(NAME integration_test COMMAND cycle-map-server-tests --gtest_filter=IntegrationTest.*)
add_test(NAME integration_secure_test COMMAND cycle-map-server-tests --gtest_filter=IntegrationSecureTest.*)