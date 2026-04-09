cmake_minimum_required(VERSION 3.16)
project(chart_parser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Build tests" ON)
option(ENABLE_COVERAGE "Enable coverage" OFF)

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()

set(GDAL_ROOT "F:/win/3rd/gdal-3.9.3" CACHE PATH "GDAL installation directory")
set(LIBXML2_ROOT "E:/3rd/libxml2" CACHE PATH "libxml2 installation directory")
set(GTEST_ROOT "E:/3rd/googletest" CACHE PATH "GoogleTest installation directory")

find_path(GDAL_INCLUDE_DIR NAMES gdal.h PATHS "${GDAL_ROOT}/include" NO_DEFAULT_PATH)
find_library(GDAL_LIBRARY NAMES gdal gdal_i PATHS "${GDAL_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT GDAL_INCLUDE_DIR OR NOT GDAL_LIBRARY)
    message(FATAL_ERROR "GDAL not found. Please set GDAL_ROOT to the GDAL installation directory.")
endif()

message(STATUS "GDAL include dir: ${GDAL_INCLUDE_DIR}")
message(STATUS "GDAL library: ${GDAL_LIBRARY}")

find_path(LIBXML2_INCLUDE_DIR NAMES libxml/parser.h PATHS "${LIBXML2}/include/libxml2" "${LIBXML2_ROOT}/include" NO_DEFAULT_PATH)
find_library(LIBXML2_LIBRARY NAMES xml2 libxml2 PATHS "${LIBXML2_ROOT}/lib" NO_DEFAULT_PATH)

if(NOT LIBXML2_INCLUDE_DIR OR NOT LIBXML2_LIBRARY)
    message(WARNING "libxml2 not found. S101 parsing will be disabled.")
    set(LIBXML2_FOUND FALSE)
else()
    message(STATUS "libxml2 include dir: ${LIBXML2_INCLUDE_DIR}")
    message(STATUS "libxml2 library: ${LIBXML2_LIBRARY}")
    set(LIBXML2_FOUND TRUE)
endif()

set(CHART_PARSER_SOURCES
    src/chart_parser.cpp
    src/iparser.cpp
    src/parse_result.cpp
    src/gdal_initializer.cpp
    src/s57_parser.cpp
    src/s57_geometry_converter.cpp
    src/s57_attribute_parser.cpp
    src/s57_feature_type_mapper.cpp
    src/error_handler.cpp
    src/data_converter.cpp
    src/ogr_data_converter.cpp
    src/parse_cache.cpp
    src/performance_benchmark.cpp
    src/s100_parser.cpp
    src/s102_parser.cpp
    src/incremental_parser.cpp
)

if(LIBXML2_FOUND)
    list(APPEND CHART_PARSER_SOURCES
        src/s101_parser.cpp
        src/s101_gml_parser.cpp
    )
endif()

set(CHART_PARSER_HEADERS
    include/parser/chart_parser.h
    include/parser/iparser.h
    include/parser/parse_result.h
    include/parser/chart_format.h
    include/parser/parse_config.h
    include/parser/gdal_initializer.h
    include/parser/s57_parser.h
    include/parser/s57_geometry_converter.h
    include/parser/s57_attribute_parser.h
    include/parser/s57_feature_type_mapper.h
    include/parser/error_handler.h
    include/parser/error_codes.h
    include/parser/data_converter.h
    include/parser/ogr_data_converter.h
    include/parser/parse_cache.h
    include/parser/performance_benchmark.h
    include/parser/s100_parser.h
    include/parser/s102_parser.h
    include/parser/incremental_parser.h
)

if(LIBXML2_FOUND)
    list(APPEND CHART_PARSER_HEADERS
        include/parser/s101_parser.h
        include/parser/s101_gml_parser.h
    )
endif()

add_library(chart_parser STATIC ${CHART_PARSER_SOURCES} ${CHART_PARSER_HEADERS})

target_include_directories(chart_parser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GDAL_INCLUDE_DIR}
)

target_link_libraries(chart_parser
    PUBLIC
        ${GDAL_LIBRARY}
)

if(LIBXML2_FOUND)
    target_include_directories(chart_parser PRIVATE ${LIBXML2_INCLUDE_DIR})
    target_link_libraries(chart_parser PUBLIC ${LIBXML2_LIBRARY})
    target_compile_definitions(chart_parser PRIVATE HAS_LIBXML2=1)
endif()

if(BUILD_TESTING)
    enable_testing()
    
    find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h PATHS "${GTEST_ROOT}/include" NO_DEFAULT_PATH)
    find_library(GTEST_LIBRARY NAMES gtest PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
    find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
    find_library(GMOCK_LIBRARY NAMES gmock PATHS "${GTEST_ROOT}/lib" NO_DEFAULT_PATH)
    
    if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY AND GTEST_MAIN_LIBRARY)
        message(STATUS "GTest include dir: ${GTEST_INCLUDE_DIR}")
        message(STATUS "GTest library: ${GTEST_LIBRARY}")
        message(STATUS "GTest main library: ${GTEST_MAIN_LIBRARY}")
        
        add_library(GTest::gtest STATIC IMPORTED)
        set_target_properties(GTest::gtest PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
            IMPORTED_LOCATION "${GTEST_LIBRARY}"
        )
        
        add_library(GTest::gtest_main STATIC IMPORTED)
        set_target_properties(GTest::gtest_main PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
            IMPORTED_LOCATION "${GTEST_MAIN_LIBRARY}"
        )
        
        if(GMOCK_LIBRARY)
            add_library(GTest::gmock STATIC IMPORTED)
            set_target_properties(GTest::gmock PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
                IMPORTED_LOCATION "${GMOCK_LIBRARY}"
            )
        endif()
        
        set(GTest_FOUND TRUE)
    else()
        message(STATUS "GTest not found at ${GTEST_ROOT}, trying find_package...")
        find_package(GTest QUIET)
    endif()
    
    if(NOT GTest_FOUND)
        message(STATUS "GTest not found, downloading...")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    
    add_subdirectory(tests)
endif()

option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_executable(parser_examples examples/parser_examples.cpp)
    target_link_libraries(parser_examples PRIVATE chart_parser)
    set_target_properties(parser_examples PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

install(TARGETS chart_parser
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/parser
    DESTINATION include
)
