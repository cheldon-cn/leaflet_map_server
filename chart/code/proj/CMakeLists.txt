project(ogc_proj)

set(SOURCES
    src/coordinate_transform.cpp
    src/coordinate_transformer.cpp
    src/proj_transformer.cpp
    src/coord_system_preset.cpp
)

set(HEADERS
    include/ogc/proj/export.h
    include/ogc/proj/transform_matrix.h
    include/ogc/proj/coordinate_transform.h
    include/ogc/proj/coordinate_transformer.h
    include/ogc/proj/proj_transformer.h
    include/ogc/proj/coord_system_preset.h
)

add_library(ogc_proj SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_proj
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_proj
    PUBLIC
        ogc_geometry
)

target_compile_features(ogc_proj PUBLIC cxx_std_11)

target_compile_definitions(ogc_proj PRIVATE OGC_PROJ_EXPORTS)

set_target_properties(ogc_proj PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    OUTPUT_NAME "ogc_proj"
)

install(TARGETS ogc_proj
    EXPORT ogc_proj-targets
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
