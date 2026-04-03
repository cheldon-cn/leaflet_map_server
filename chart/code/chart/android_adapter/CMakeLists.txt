cmake_minimum_required(VERSION 3.16)

project(ogc_android_adapter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OGC_ANDROID_ADAPTER_STATIC "Build as static library" OFF)
option(ANDROID_ADAPTER_BUILD_TESTS "Build tests" ON)

if(ANDROID)
    set(ANDROID_ADAPTER_SOURCES
        src/android_engine.cpp
        src/android_touch_handler.cpp
        src/android_lifecycle.cpp
        src/android_permission.cpp
        src/android_platform.cpp
    )
    
    set(ANDROID_ADAPTER_HEADERS
        include/android_adapter/export.h
        include/android_adapter/android_engine.h
        include/android_adapter/android_touch_handler.h
        include/android_adapter/android_lifecycle.h
        include/android_adapter/android_permission.h
        include/android_adapter/android_platform.h
    )
    
    add_library(ogc_android_adapter SHARED ${ANDROID_ADAPTER_SOURCES} ${ANDROID_ADAPTER_HEADERS})
    
    target_include_directories(ogc_android_adapter
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )
    
    target_compile_definitions(ogc_android_adapter
        PRIVATE
            OGC_ANDROID_ADAPTER_EXPORTS
    )
    
    if(OGC_ANDROID_ADAPTER_STATIC)
        target_compile_definitions(ogc_android_adapter PUBLIC OGC_ANDROID_ADAPTER_STATIC)
    endif()
    
    target_link_libraries(ogc_android_adapter
        PUBLIC
            android
            log
            EGL
            GLESv2
    )
    
    set_target_properties(ogc_android_adapter PROPERTIES
        OUTPUT_NAME "ogc_android_adapter"
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
    )
else()
    message(STATUS "Android adapter module: Not building (not Android platform)")
    
    add_library(ogc_android_adapter INTERFACE)
    target_include_directories(ogc_android_adapter INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
endif()

if(ANDROID_ADAPTER_BUILD_TESTS AND ANDROID)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS ogc_android_adapter
    EXPORT ogc_android_adapter-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/android_adapter
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
