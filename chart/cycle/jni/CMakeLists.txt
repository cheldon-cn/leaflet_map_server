cmake_minimum_required(VERSION 3.10)
project(ogc_chart_jni LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT JAVA_HOME)
    message(FATAL_ERROR "JAVA_HOME is not set. Please set JAVA_HOME environment variable.")
endif()

set(JNI_INCLUDE_DIRS
    "${JAVA_HOME}/include"
    "${JAVA_HOME}/include/win32"
)

set(OGC_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../install")
set(CODE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../code/build")
set(CODE_TEST_DIR "${CODE_BUILD_DIR}/test")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CODE_TEST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CODE_TEST_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CODE_BUILD_DIR}/lib)

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CODE_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CODE_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CODE_BUILD_DIR}/lib" CACHE PATH "" FORCE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CODE_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CODE_TEST_DIR}" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CODE_BUILD_DIR}/lib" CACHE PATH "" FORCE)
endif()

set(JNI_SOURCES
    src/jni_env_manager.cpp
    src/jni_converter.cpp
    src/jni_exception.cpp
    src/jni_reference_cache.cpp
    src/jni_onload.cpp
    src/native/chart_viewer_jni.cpp
    src/native/viewport_jni.cpp
    src/native/geometry_jni.cpp
    src/native/feature_jni.cpp
    src/native/layer_jni.cpp
    src/native/chart_config_jni.cpp
    src/native/adapter_jni.cpp
)

add_library(ogc_chart_jni SHARED ${JNI_SOURCES})

target_include_directories(ogc_chart_jni
    PRIVATE
        ${JNI_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        "${CMAKE_CURRENT_SOURCE_DIR}/../../install/include"
)

target_link_directories(ogc_chart_jni
    PRIVATE
        ${CODE_BUILD_DIR}/lib
        ${OGC_INSTALL_DIR}/lib
)

target_link_libraries(ogc_chart_jni
    PRIVATE
        ogc_chart_c_api
)

target_compile_definitions(ogc_chart_jni PRIVATE OGC_CHART_C_API_EXPORTS)

install(TARGETS ogc_chart_jni
    RUNTIME DESTINATION ${OGC_INSTALL_DIR}/bin
    LIBRARY DESTINATION ${OGC_INSTALL_DIR}/bin
    ARCHIVE DESTINATION ${OGC_INSTALL_DIR}/lib
)
