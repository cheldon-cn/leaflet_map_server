cmake_minimum_required(VERSION 3.10)

set(LAYER_SOURCES
    src/layer_type.cpp
    src/datasource.cpp
    src/driver_manager.cpp
    src/connection_pool.cpp
    src/thread_safe_layer.cpp
    src/raster_layer.cpp
    src/feature_stream.cpp
    src/layer_group.cpp
    src/layer_observer.cpp
    src/layer_snapshot.cpp
    src/memory_layer.cpp
    src/vector_layer.cpp
    src/geojson_layer.cpp
    src/geopackage_layer.cpp
    src/postgis_layer.cpp
    src/shapefile_layer.cpp
    src/wfs_layer.cpp
    src/gdal_adapter.cpp
)

add_library(ogc_layer ${LAYER_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(ogc_layer PRIVATE OGC_LAYER_EXPORTS)
else()
    target_compile_definitions(ogc_layer PUBLIC OGC_LAYER_STATIC)
endif()

target_include_directories(ogc_layer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${GEOM_SOURCE_DIR}/include
        ${FEATURE_SOURCE_DIR}/include
)

target_link_libraries(ogc_layer
    PUBLIC
        ogc_geometry
        ogc_feature
)

set_target_properties(ogc_layer PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS ogc_layer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
