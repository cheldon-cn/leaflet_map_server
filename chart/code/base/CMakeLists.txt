project(ogc_base)

set(SOURCES
    src/log.cpp
    src/thread_safe.cpp
    src/performance_metrics.cpp
    src/performance_monitor.cpp
)

set(HEADERS
    include/ogc/base/export.h
    include/ogc/base/log.h
    include/ogc/base/thread_safe.h
    include/ogc/base/performance_metrics.h
    include/ogc/base/performance_monitor.h
)

add_library(ogc_base SHARED ${SOURCES} ${HEADERS})

target_include_directories(ogc_base
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ogc_base PUBLIC cxx_std_11)

target_compile_definitions(ogc_base PRIVATE OGC_BASE_EXPORTS)

set_target_properties(ogc_base PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    OUTPUT_NAME "ogc_base"
)

install(TARGETS ogc_base
    EXPORT ogc_base-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ogc
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
