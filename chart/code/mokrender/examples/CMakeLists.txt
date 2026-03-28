cmake_minimum_required(VERSION 3.10)

project(ogc_mokrender_demo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mokrender_demo demo.cpp)

target_link_libraries(mokrender_demo
    PRIVATE
        ogc_mokrender
)

set_target_properties(mokrender_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../temp"
)

if(MSVC)
    set_target_properties(mokrender_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../temp"
    )
endif()
