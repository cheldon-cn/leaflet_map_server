cmake_minimum_required(VERSION 3.16)

project(ogc_jni_bridge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OGC_JNI_BRIDGE_STATIC "Build as static library" OFF)
option(JNI_BRIDGE_BUILD_TESTS "Build tests" ON)

if(ANDROID)
    set(JNI_BRIDGE_SOURCES
        src/jni_env.cpp
        src/jni_convert.cpp
        src/jni_exception.cpp
        src/jni_memory.cpp
    )
    
    set(JNI_BRIDGE_HEADERS
        include/jni_bridge/export.h
        include/jni_bridge/jni_env.h
        include/jni_bridge/jni_convert.h
        include/jni_bridge/jni_exception.h
        include/jni_bridge/jni_memory.h
    )
    
    add_library(ogc_jni_bridge SHARED ${JNI_BRIDGE_SOURCES} ${JNI_BRIDGE_HEADERS})
    
    target_include_directories(ogc_jni_bridge
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )
    
    target_compile_definitions(ogc_jni_bridge
        PRIVATE
            OGC_JNI_BRIDGE_EXPORTS
    )
    
    if(OGC_JNI_BRIDGE_STATIC)
        target_compile_definitions(ogc_jni_bridge PUBLIC OGC_JNI_BRIDGE_STATIC)
    endif()
    
    target_link_libraries(ogc_jni_bridge
        PUBLIC
            android
            log
    )
    
    set_target_properties(ogc_jni_bridge PROPERTIES
        OUTPUT_NAME "ogc_jni_bridge"
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
    )
else()
    message(STATUS "JNI bridge module: Not building (not Android platform)")
    
    add_library(ogc_jni_bridge INTERFACE)
    target_include_directories(ogc_jni_bridge INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
endif()

if(JNI_BRIDGE_BUILD_TESTS AND ANDROID)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS ogc_jni_bridge
    EXPORT ogc_jni_bridge-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/jni_bridge
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
