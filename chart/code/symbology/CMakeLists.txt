project(ogc_symbology)

set(SOURCES
    src/filter/filter.cpp
    src/filter/comparison_filter.cpp
    src/filter/logical_filter.cpp
    src/filter/spatial_filter.cpp
    src/filter/symbolizer_rule.cpp
    src/filter/rule_engine.cpp
    src/style/sld_parser.cpp
    src/style/mapbox_style_parser.cpp
    src/style/s52_style_manager.cpp
    src/style/s52_symbol_renderer.cpp
    src/symbolizer/symbolizer.cpp
    src/symbolizer/point_symbolizer.cpp
    src/symbolizer/line_symbolizer.cpp
    src/symbolizer/polygon_symbolizer.cpp
    src/symbolizer/text_symbolizer.cpp
    src/symbolizer/raster_symbolizer.cpp
    src/symbolizer/icon_symbolizer.cpp
    src/symbolizer/composite_symbolizer.cpp
)

set(HEADERS
    include/ogc/symbology/export.h
    include/ogc/symbology/filter/filter.h
    include/ogc/symbology/filter/comparison_filter.h
    include/ogc/symbology/filter/logical_filter.h
    include/ogc/symbology/filter/spatial_filter.h
    include/ogc/symbology/filter/symbolizer_rule.h
    include/ogc/symbology/filter/rule_engine.h
    include/ogc/symbology/style/sld_parser.h
    include/ogc/symbology/style/mapbox_style_parser.h
    include/ogc/symbology/style/s52_style_manager.h
    include/ogc/symbology/style/s52_symbol_renderer.h
    include/ogc/symbology/symbolizer/symbolizer.h
    include/ogc/symbology/symbolizer/point_symbolizer.h
    include/ogc/symbology/symbolizer/line_symbolizer.h
    include/ogc/symbology/symbolizer/polygon_symbolizer.h
    include/ogc/symbology/symbolizer/text_symbolizer.h
    include/ogc/symbology/symbolizer/raster_symbolizer.h
    include/ogc/symbology/symbolizer/icon_symbolizer.h
    include/ogc/symbology/symbolizer/composite_symbolizer.h
)

add_library(ogc_symbology SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_symbology
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ogc_symbology PUBLIC cxx_std_11)

target_compile_definitions(ogc_symbology PRIVATE OGC_SYMBOLOGY_EXPORTS)

target_link_libraries(ogc_symbology
    PUBLIC
        ogc_geometry
        ogc_feature
        ogc_draw
)

set_target_properties(ogc_symbology PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    OUTPUT_NAME "ogc_symbology"
)

install(TARGETS ogc_symbology
    EXPORT ogc_symbology-targets
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
