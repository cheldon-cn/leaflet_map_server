cmake_minimum_required(VERSION 3.15)

project(ogc_database)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# 第三方库路径配置
# 使用环境变量或默认值
# ============================================================
# PostgreSQL (libpq)
set(POSTGRESQL_INCLUDE_DIR "D:/program/PostgreSQL/13/include" CACHE PATH "PostgreSQL include directory")
set(POSTGRESQL_LIB_DIR "D:/program/PostgreSQL/13/lib" CACHE PATH "PostgreSQL lib directory")

# SQLite3
set(SQLITE3_INCLUDE_DIR "F:/win/3rd/sqlite3" CACHE PATH "SQLite3 include directory")
set(SQLITE3_LIB_DIR "F:/win/3rd/sqlite3" CACHE PATH "SQLite3 lib directory")

# SpatiaLite (mod_spatialite)
set(SPATIALITE_BIN_DIR "E:/pro/bin" CACHE PATH "SpatiaLite bin directory")

# ============================================================
# 查找 PostgreSQL libpq
# ============================================================
find_library(LIBPQ_LIBRARY 
    NAMES libpq
    PATHS ${POSTGRESQL_LIB_DIR}
    NO_DEFAULT_PATH)

if(NOT LIBPQ_LIBRARY)
    message(WARNING "libpq not found, PostGIS features may be disabled")
endif()

# ============================================================
# 查找 SQLite3
# ============================================================
find_library(SQLITE3_LIBRARY
    NAMES sqlite3
    PATHS ${SQLITE3_LIB_DIR}
    NO_DEFAULT_PATH)

if(NOT SQLITE3_LIBRARY)
    message(WARNING "SQLite3 not found, SpatiaLite features may be disabled")
endif()

# ============================================================
# 头文件目录
# ============================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../geom/include
    ${POSTGRESQL_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIR}
)

# ============================================================
# 库目录
# ============================================================
link_directories(
    ${POSTGRESQL_LIB_DIR}
    ${SQLITE3_LIB_DIR}
)

# ============================================================
# 源文件
# ============================================================
set(SOURCES
    src/wkb_converter.cpp
    src/postgis_connection.cpp
    src/postgis_batch.cpp
    src/sqlite_connection.cpp
    src/sqlite_spatial.cpp
    src/sqlite_transaction.cpp
    src/connection_pool.cpp
    src/srid_manager.cpp
    src/geojson_converter.cpp
    src/async_connection.cpp
)

# ============================================================
# 创建库
# ============================================================
add_library(ogc_database STATIC ${SOURCES})

# ============================================================
# 链接库
# ============================================================
set(LIBS ws2_32 secur32)

if(LIBPQ_LIBRARY)
    list(APPEND LIBS ${LIBPQ_LIBRARY})
endif()

if(SQLITE3_LIBRARY)
    list(APPEND LIBS ${SQLITE3_LIBRARY})
endif()

target_link_libraries(ogc_database PRIVATE ${LIBS})

# ============================================================
# 安装
# ============================================================
install(TARGETS ogc_database
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(DIRECTORY include/ogc/db 
    DESTINATION include/ogc
    FILES_MATCHING PATTERN "*.h")
