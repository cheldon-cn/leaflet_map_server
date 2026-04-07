project(ogc_cache)

set(SOURCES
    src/tile/tile_key.cpp
    src/tile/tile_cache.cpp
    src/tile/memory_tile_cache.cpp
    src/tile/disk_tile_cache.cpp
    src/tile/multi_level_tile_cache.cpp
    src/offline/offline_storage_manager.cpp
    src/offline/offline_sync_manager.cpp
    src/offline/data_encryption.cpp
)

set(HEADERS
    include/ogc/cache/export.h
    include/ogc/cache/tile/tile_key.h
    include/ogc/cache/tile/tile_cache.h
    include/ogc/cache/tile/memory_tile_cache.h
    include/ogc/cache/tile/disk_tile_cache.h
    include/ogc/cache/tile/multi_level_tile_cache.h
    include/ogc/cache/offline/offline_storage_manager.h
    include/ogc/cache/offline/offline_sync_manager.h
    include/ogc/cache/offline/data_encryption.h
)

add_library(ogc_cache SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_cache
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ogc_cache PUBLIC cxx_std_11)

target_link_libraries(ogc_cache
    PUBLIC
        ogc_geometry
)

target_compile_definitions(ogc_cache PRIVATE OGC_CACHE_EXPORTS)

set_target_properties(ogc_cache PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    OUTPUT_NAME "ogc_cache"
)

install(TARGETS ogc_cache
    EXPORT ogc_cache-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
