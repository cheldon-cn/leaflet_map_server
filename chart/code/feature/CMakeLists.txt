cmake_minimum_required(VERSION 3.10)

set(FEATURE_SOURCES
    src/field_type.cpp
    src/datetime.cpp
    src/field_defn.cpp
    src/geom_field_defn.cpp
    src/feature_defn.cpp
    src/field_value.cpp
    src/feature.cpp
    src/feature_guard.cpp
    src/batch_processor.cpp
    src/spatial_query.cpp
    src/feature_collection.cpp
    src/feature_iterator.cpp
    src/geojson_converter.cpp
    src/wkb_wkt_converter.cpp
)

add_library(ogc_feature ${FEATURE_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(ogc_feature PRIVATE OGC_FEATURE_EXPORTS)
else()
    target_compile_definitions(ogc_feature PUBLIC OGC_FEATURE_STATIC)
endif()

target_include_directories(ogc_feature
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_feature
    PUBLIC
        ogc_geometry
)

set_target_properties(ogc_feature PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

install(TARGETS ogc_feature
    EXPORT ogc_geometry-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
