cmake_minimum_required(VERSION 3.10)

project(ogc_navi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../geom/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../database/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(POSITIONING_SOURCES
    src/positioning/nmea_parser.cpp
    src/positioning/position_filter.cpp
    src/positioning/coordinate_converter.cpp
    src/positioning/position_provider.cpp
    src/positioning/position_manager.cpp
)

set(ROUTE_SOURCES
    src/route/waypoint.cpp
    src/route/route.cpp
    src/route/route_planner.cpp
    src/route/route_manager.cpp
    src/route/ukc_calculator.cpp
)

set(NAVIGATION_SOURCES
    src/navigation/navigation_calculator.cpp
    src/navigation/off_course_detector.cpp
    src/navigation/navigation_alerter.cpp
    src/navigation/navigation_engine.cpp
)

set(TRACK_SOURCES
    src/track/track_point.cpp
    src/track/track.cpp
    src/track/track_recorder.cpp
    src/track/track_player.cpp
    src/track/track_manager.cpp
)

set(AIS_SOURCES
    src/ais/ais_target.cpp
    src/ais/ais_parser.cpp
    src/ais/ais_manager.cpp
    src/ais/collision_assessor.cpp
)

set(SOURCES
    ${POSITIONING_SOURCES}
    ${ROUTE_SOURCES}
    ${NAVIGATION_SOURCES}
    ${TRACK_SOURCES}
    ${AIS_SOURCES}
)

add_library(ogc_navi ${SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(ogc_navi PRIVATE OGC_NAVI_EXPORTS)
else()
    target_compile_definitions(ogc_navi PUBLIC OGC_NAVI_STATIC)
endif()

target_link_libraries(ogc_navi
    PUBLIC
        ogc_geometry
    PRIVATE
        ogc_database
)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()

set_target_properties(ogc_navi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(MSVC)
    set_target_properties(ogc_navi PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/test"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
    )
endif()

install(TARGETS ogc_navi
    EXPORT ogc_geometry-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install(DIRECTORY include/ogc/navi/
    DESTINATION include/ogc/navi
    FILES_MATCHING PATTERN "*.h")
