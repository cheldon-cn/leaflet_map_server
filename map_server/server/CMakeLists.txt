cmake_minimum_required(VERSION 3.12)
project(cycle-map-server VERSION 1.0.0 LANGUAGES CXX)

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

# 项目配置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置第三方库路径
set(THIRD_PARTY_DIR "F:/win/3rd")
set(THIRD_PG_DIR "D:/program/PostgreSQL/13")

# 查找依赖库
if(UNIX)
    find_package(PkgConfig REQUIRED)
    
    # SQLite3
    pkg_check_modules(SQLITE3 REQUIRED sqlite3)
    
    # PostgreSQL (可选)
    pkg_check_modules(POSTGRESQL libpq)
    if(POSTGRESQL_FOUND)
        add_definitions(-DHAVE_POSTGRESQL)
        message(STATUS "PostgreSQL support enabled")
    else()
        message(STATUS "PostgreSQL support disabled")
    endif()
    
    # PNG
    pkg_check_modules(PNG REQUIRED libpng)
    
    # WebP (可选)
    pkg_check_modules(WEBP libwebp)
    if(WEBP_FOUND)
        add_definitions(-DHAVE_WEBP)
        message(STATUS "WebP support enabled")
    else()
        message(STATUS "WebP support disabled")
    endif()
else()
    # Windows 环境下的依赖查找
    
    # SQLite3
    find_path(SQLITE3_INCLUDE_DIR sqlite3.h
        PATHS ${THIRD_PARTY_DIR}/sqlite3/include
        NO_DEFAULT_PATH
    )
    find_library(SQLITE3_LIBRARY sqlite3
        PATHS ${THIRD_PARTY_DIR}/sqlite3/lib(x64)
        NO_DEFAULT_PATH
    )
    if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
        set(SQLITE3_FOUND TRUE)
        set(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
        set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
        message(STATUS "SQLite3 found: ${SQLITE3_INCLUDE_DIR}")
    else()
        message(WARNING "SQLite3 not found - using stub implementation")
        set(SQLITE3_FOUND FALSE)
        add_definitions(-DNO_SQLITE3)
    endif()
    
    # PNG
    find_path(PNG_INCLUDE_DIR png.h
        PATHS ${THIRD_PARTY_DIR}/libpng-1.6.37/include
        NO_DEFAULT_PATH
    )
    find_library(PNG_LIBRARY libpng16
        PATHS ${THIRD_PARTY_DIR}/libpng-1.6.37/lib
        NO_DEFAULT_PATH
    )
    if(PNG_INCLUDE_DIR AND PNG_LIBRARY)
        set(PNG_FOUND TRUE)
        set(PNG_INCLUDE_DIRS ${PNG_INCLUDE_DIR})
        set(PNG_LIBRARIES ${PNG_LIBRARY})
        message(STATUS "PNG found: ${PNG_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "PNG not found - required for image encoding")
    endif()
    
    # PostgreSQL (可选)
    find_path(POSTGRESQL_INCLUDE_DIR libpq-fe.h
        PATHS ${THIRD_PG_DIR}/include
        NO_DEFAULT_PATH
    )
    find_library(POSTGRESQL_LIBRARY pqxx
        PATHS ${THIRD_PG_DIR}/lib
        NO_DEFAULT_PATH
    )
    if(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARY)
        set(POSTGRESQL_FOUND TRUE)
        set(POSTGRESQL_INCLUDE_DIRS ${POSTGRESQL_INCLUDE_DIR})
        set(POSTGRESQL_LIBRARIES ${POSTGRESQL_LIBRARY})
        add_definitions(-DHAVE_POSTGRESQL)
        message(STATUS "PostgreSQL support enabled")
    else()
        message(STATUS "PostgreSQL support disabled")
    endif()
    
    # WebP (可选)
    find_path(WEBP_INCLUDE_DIR webp/encode.h
        PATHS ${THIRD_PARTY_DIR}/libwebp-1.6.0/include
        NO_DEFAULT_PATH
    )
    find_library(WEBP_LIBRARY LibWebP
        PATHS ${THIRD_PARTY_DIR}/libwebp-1.6.0/lib
        NO_DEFAULT_PATH
    )
    if(WEBP_INCLUDE_DIR AND WEBP_LIBRARY)
        set(WEBP_FOUND TRUE)
        set(WEBP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
        set(WEBP_LIBRARIES ${WEBP_LIBRARY})
        add_definitions(-DHAVE_WEBP)
        message(STATUS "WebP support enabled")
    else()
        message(STATUS "WebP support disabled")
    endif()
endif()

# SSL (可选)
option(WITH_SSL "Build with SSL support" OFF)
if(WITH_SSL)
    find_library(SSL_LIBRARY ssl
        PATHS ${THIRD_PARTY_DIR}/ssl3/lib
        NO_DEFAULT_PATH
    )
    find_library(CRYPTO_LIBRARY crypto
        PATHS ${THIRD_PARTY_DIR}/ssl3/lib
        NO_DEFAULT_PATH
    )
    if(NOT SSL_LIBRARY OR NOT CRYPTO_LIBRARY)
        message(WARNING "SSL libraries not found, disabling SSL support")
        set(WITH_SSL OFF)
    else()
        message(STATUS "SSL support enabled")
    endif()
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdparty
    ${CMAKE_SOURCE_DIR}/thirdparty/jwt-cpp-v0.7.2/include
)

# 完整的源文件列表
set(SERVER_SOURCES
    src/main.cpp
    src/main_secure.cpp
    src/config/config.cpp
    src/utils/logger.cpp
    src/utils/file_system.cpp
    src/utils/config_encryptor.cpp
    src/database/sqlite_database.cpp
    src/database/postgresql_database.cpp
    src/database/database_factory.cpp
    src/encoder/png_encoder.cpp
    src/encoder/webp_encoder.cpp
    src/encoder/encoder_factory.cpp
    src/cache/memory_cache.cpp
    src/cache/disk_cache.cpp
    src/renderer/render_context.cpp
    src/renderer/renderer.cpp
    src/service/map_service.cpp
    src/server/http_server_secure.cpp
    src/auth/jwt_auth.cpp
    src/performance/performance_optimizer.cpp
)

# 创建静态库（用于测试）
add_library(cycle-map-server-lib ${SERVER_SOURCES})

# 设置第三方库文件目录
if(SQLITE3_LIBRARY)
    get_filename_component(SQLITE3_LIB_DIR ${SQLITE3_LIBRARY} DIRECTORY)
    link_directories(${SQLITE3_LIB_DIR})
endif()

if(PNG_LIBRARY)
    get_filename_component(PNG_LIB_DIR ${PNG_LIBRARY} DIRECTORY)
    link_directories(${PNG_LIB_DIR})
endif()

# 添加第三方库头文件目录
target_include_directories(cycle-map-server-lib PRIVATE
    ${SQLITE3_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
)

if(POSTGRESQL_FOUND)
    target_include_directories(cycle-map-server-lib PRIVATE ${POSTGRESQL_INCLUDE_DIRS})
endif()

if(WEBP_FOUND)
    target_include_directories(cycle-map-server-lib PRIVATE ${WEBP_INCLUDE_DIRS})
endif()

if(WITH_SSL)
    target_include_directories(cycle-map-server-lib PRIVATE ${OPENSSL_INCLUDE_DIR})
endif()

# 创建主程序可执行文件
add_executable(cycle-map-server src/main.cpp)

# 链接库
target_link_libraries(cycle-map-server-lib
    ${SQLITE3_LIBRARIES}
    ${PNG_LIBRARIES}
)

if(POSTGRESQL_FOUND)
    target_link_libraries(cycle-map-server-lib ${POSTGRESQL_LIBRARIES})
endif()

if(WEBP_FOUND)
    target_link_libraries(cycle-map-server-lib ${WEBP_LIBRARIES})
endif()

if(WITH_SSL)
    target_link_libraries(cycle-map-server-lib ${SSL_LIBRARY} ${CRYPTO_LIBRARY})
endif()

# 链接主程序
target_link_libraries(cycle-map-server cycle-map-server-lib)

# 安装规则
install(TARGETS cycle-map-server
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES config.json DESTINATION etc/cycle-map-server)
install(DIRECTORY data/ DESTINATION var/lib/cycle-map-server)
install(DIRECTORY logs/ DESTINATION var/log/cycle-map-server)

# 测试配置
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    
    # 使用项目自带的 Google Test
    add_subdirectory(thirdparty/gtest)
    
    # 创建日志测试可执行文件（独立，不依赖其他模块）
    set(LOGGER_TEST_SOURCES
        tests/test_logger_minimal.cpp
        src/utils/logger.cpp
    )
    add_executable(logger-test ${LOGGER_TEST_SOURCES})
    target_link_libraries(logger-test gtest)
    
    # 添加测试
    add_test(NAME logger_minimal_test COMMAND logger-test --gtest_filter=LoggerMinimalTest.*)
    
    # 安装测试
    install(TARGETS logger-test RUNTIME DESTINATION bin)
endif()

# 包配置
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cycle-map-server-config.cmake.in
    ${CMAKE_BINARY_DIR}/cycle-map-server-config.cmake
    @ONLY
)

# 版本信息
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cycle-map-server-version.cmake.in
    ${CMAKE_BINARY_DIR}/cycle-map-server-version.cmake
    @ONLY
)

# 输出信息
message(STATUS "Cycle Map Server Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  PostgreSQL Support: ${POSTGRESQL_FOUND}")
message(STATUS "  WebP Support: ${WEBP_FOUND}")
message(STATUS "  SSL Support: ${WITH_SSL}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")

if(NOT SQLITE3_FOUND)
    message(WARNING "SQLite3 library not found - core functionality will be limited")
endif()

if(NOT PNG_FOUND)
    message(WARNING "PNG library not found - image encoding will not work")
endif()