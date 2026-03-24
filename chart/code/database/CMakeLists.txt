cmake_minimum_required(VERSION 3.15)

project(ogc_database)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(LIBPQ_LIBRARY 
    NAMES libpq
    PATHS ${POSTGRESQL_LIB_DIR}
    NO_DEFAULT_PATH)

if(NOT LIBPQ_LIBRARY)
    message(WARNING "libpq not found at ${POSTGRESQL_LIB_DIR}, PostGIS features may be disabled")
endif()

find_library(SQLITE3_LIBRARY
    NAMES sqlite3
    PATHS ${SQLITE3_LIB_DIR}
    NO_DEFAULT_PATH)

if(NOT SQLITE3_LIBRARY)
    message(WARNING "SQLite3 not found at ${SQLITE3_LIB_DIR}, SpatiaLite features may be disabled")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../geom/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${POSTGRESQL_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIR}
)

link_directories(
    ${POSTGRESQL_LIB_DIR}
    ${SQLITE3_LIB_DIR}
)

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

add_library(ogc_database ${SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(ogc_database PRIVATE OGC_DATABASE_EXPORTS)
else()
    target_compile_definitions(ogc_database PUBLIC OGC_DATABASE_STATIC)
endif()

set(LIBS ws2_32 secur32)

if(LIBPQ_LIBRARY)
    list(APPEND LIBS ${LIBPQ_LIBRARY})
endif()

if(SQLITE3_LIBRARY)
    list(APPEND LIBS ${SQLITE3_LIBRARY})
endif()

target_link_libraries(ogc_database PRIVATE ${LIBS} ogc_geometry)

install(TARGETS ogc_database
    EXPORT ogc_geometry-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(DIRECTORY include/ogc/db/
    DESTINATION include/ogc/db
    FILES_MATCHING PATTERN "*.h")
