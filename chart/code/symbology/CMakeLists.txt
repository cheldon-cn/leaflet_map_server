project(ogc_symbology)

option(ENABLE_PORTRAYAL_PARSER "Enable portrayal catalogue XML parsing (requires compatible libxml2)" ON)

if(ENABLE_PORTRAYAL_PARSER)
    find_path(LIBXML2_INCLUDE_DIR NAMES libxml/parser.h PATHS "${LIBXML2_ROOT}/include/libxml2" "${LIBXML2_ROOT}/include" NO_DEFAULT_PATH)
    find_library(LIBXML2_LIBRARY NAMES xml2 libxml2 PATHS "${LIBXML2_ROOT}/lib" NO_DEFAULT_PATH)

    set(LIBICONV_ROOT "E:/xspace/3rd/libiconv" CACHE PATH "libiconv root directory")
    find_path(LIBICONV_INCLUDE_DIR NAMES iconv.h PATHS "${LIBICONV_ROOT}/include" NO_DEFAULT_PATH)

    if(NOT LIBXML2_INCLUDE_DIR OR NOT LIBXML2_LIBRARY)
        message(WARNING "libxml2 not found. Portrayal catalogue parsing will be disabled.")
        set(LIBXML2_FOUND FALSE)
    else()
        message(STATUS "libxml2 include dir: ${LIBXML2_INCLUDE_DIR}")
        message(STATUS "libxml2 library: ${LIBXML2_LIBRARY}")
        set(LIBXML2_FOUND TRUE)
    endif()
else()
    set(LIBXML2_FOUND FALSE)
endif()

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
    src/portrayal/model/description.cpp
    src/portrayal/model/file_reference.cpp
    src/portrayal/model/rule_file.cpp
    src/portrayal/model/viewing_group.cpp
    src/portrayal/model/portrayal_catalog.cpp
    src/portrayal/utils/string_utils.cpp
    src/portrayal/utils/file_utils.cpp
    src/portrayal/utils/string_pool.cpp
)

if(LIBXML2_FOUND)
    list(APPEND SOURCES
        src/portrayal/parser/libxml2_reader.cpp
        src/portrayal/parser/symbol_parser.cpp
        src/portrayal/parser/line_style_parser.cpp
        src/portrayal/parser/area_fill_parser.cpp
        src/portrayal/parser/rule_file_parser.cpp
        src/portrayal/parser/viewing_group_parser.cpp
        src/portrayal/parser/alert_catalog_parser.cpp
        src/portrayal/parser/color_profile_parser.cpp
        src/portrayal/parser/style_sheet_parser.cpp
        src/portrayal/parser/parser_registry.cpp
        src/portrayal/parser/portrayal_catalog_parser.cpp
    )
endif()

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
    include/ogc/portrayal/model/description.h
    include/ogc/portrayal/model/file_reference.h
    include/ogc/portrayal/model/symbol.h
    include/ogc/portrayal/model/line_style.h
    include/ogc/portrayal/model/area_fill.h
    include/ogc/portrayal/model/style_sheet.h
    include/ogc/portrayal/model/alert_catalog.h
    include/ogc/portrayal/model/color_profile.h
    include/ogc/portrayal/model/rule_file.h
    include/ogc/portrayal/model/viewing_group.h
    include/ogc/portrayal/model/portrayal_catalog.h
    include/ogc/portrayal/utils/string_utils.h
    include/ogc/portrayal/utils/file_utils.h
    include/ogc/portrayal/utils/string_pool.h
)

if(LIBXML2_FOUND)
    list(APPEND HEADERS
        include/ogc/portrayal/parser/xml_reader.h
        include/ogc/portrayal/parser/libxml2_reader.h
        include/ogc/portrayal/parser/element_parser.h
        include/ogc/portrayal/parser/symbol_parser.h
        include/ogc/portrayal/parser/line_style_parser.h
        include/ogc/portrayal/parser/area_fill_parser.h
        include/ogc/portrayal/parser/rule_file_parser.h
        include/ogc/portrayal/parser/viewing_group_parser.h
        include/ogc/portrayal/parser/alert_catalog_parser.h
        include/ogc/portrayal/parser/color_profile_parser.h
        include/ogc/portrayal/parser/style_sheet_parser.h
        include/ogc/portrayal/parser/parser_registry.h
        include/ogc/portrayal/parser/portrayal_catalog_parser.h
    )
endif()

add_library(ogc_symbology SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_symbology
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(LIBXML2_FOUND)
    target_include_directories(ogc_symbology PRIVATE ${LIBXML2_INCLUDE_DIR})
    if(LIBICONV_INCLUDE_DIR)
        target_include_directories(ogc_symbology PRIVATE ${LIBICONV_INCLUDE_DIR})
    endif()
endif()

target_compile_features(ogc_symbology PUBLIC cxx_std_11)

target_compile_definitions(ogc_symbology PRIVATE OGC_SYMBOLOGY_EXPORTS)

if(LIBXML2_FOUND)
    target_compile_definitions(ogc_symbology PRIVATE HAS_LIBXML2=1)
endif()

target_link_libraries(ogc_symbology
    PUBLIC
        ogc_geometry
        ogc_feature
        ogc_draw
)

if(LIBXML2_FOUND)
    target_link_libraries(ogc_symbology PUBLIC ${LIBXML2_LIBRARY})
endif()

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
