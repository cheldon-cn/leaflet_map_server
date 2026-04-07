# InstallConfiguration.cmake
# Unified installation configuration for OGC Geometry project
#
# Install directory structure:
#   ${CMAKE_INSTALL_PREFIX}/
#     ├── include/        - Header files (organized by module)
#     ├── lib/            - Static libraries and import libraries
#     ├── bin/            - Dynamic libraries and executables
#     └── share/          - Documentation and CMake config files

# Set default install prefix if not specified
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../install" CACHE PATH "Installation directory" FORCE)
endif()

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Define install directories
set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(INSTALL_SHARE_DIR "${CMAKE_INSTALL_PREFIX}/share")

# Create install directories at configure time
file(MAKE_DIRECTORY 
    "${INSTALL_INCLUDE_DIR}"
    "${INSTALL_LIB_DIR}"
    "${INSTALL_BIN_DIR}"
    "${INSTALL_SHARE_DIR}"
)

# Function to install a library with proper configuration
function(ogc_install_library TARGET_NAME)
    # Install the library
    install(TARGETS ${TARGET_NAME}
        EXPORT ogc-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
    
    # Get target properties
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
    
    # Copy DLL to bin directory after build (for Windows)
    if(WIN32 AND TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${TARGET_NAME}>"
            "${INSTALL_BIN_DIR}/$<TARGET_FILE_NAME:${TARGET_NAME}>"
            COMMENT "Copying ${TARGET_NAME} to ${INSTALL_BIN_DIR}"
        )
    endif()
endfunction()

# Function to install header files for a module
function(ogc_install_headers MODULE_NAME HEADER_DIR)
    install(DIRECTORY ${HEADER_DIR}/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*.inl"
    )
endfunction()

# Function to install documentation
function(ogc_install_docs DOC_FILES)
    install(FILES ${DOC_FILES}
        DESTINATION share/doc
    )
endfunction()

# Function to install CMake config files
function(ogc_install_cmake_config)
    install(EXPORT ogc-targets
        FILE ogc-config.cmake
        NAMESPACE OGC::
        DESTINATION lib/cmake/ogc
    )
endfunction()

# Function to copy third-party DLLs
function(ogc_copy_third_party_dlls)
    if(WIN32)
        set(THIRD_PARTY_DLLS
            "${GEOS_ROOT}/bin/geos.dll"
            "${GEOS_ROOT}/bin/geos_c.dll"
            "${SQLITE3_ROOT}/sqlite3.dll"
            "${POSTGRESQL_ROOT}/bin/libpq.dll"
            "${POSTGRESQL_ROOT}/bin/libcrypto-1_1-x64.dll"
            "${POSTGRESQL_ROOT}/bin/libssl-1_1-x64.dll"
            "${POSTGRESQL_ROOT}/bin/libintl-9.dll"
            "${GTEST_ROOT}/bin/gtest.dll"
            "${GTEST_ROOT}/bin/gtest_main.dll"
        )
        
        foreach(DLL ${THIRD_PARTY_DLLS})
            if(EXISTS ${DLL})
                install(FILES ${DLL}
                    DESTINATION bin
                )
            endif()
        endforeach()
        
        set(BUILD_DLLS
            "${CMAKE_BINARY_DIR}/test/spatialite5.dll"
            "${CMAKE_BINARY_DIR}/test/mod_spatialite5.dll"
            "${CMAKE_BINARY_DIR}/test/librttopo.dll"
            "${CMAKE_BINARY_DIR}/test/freexl.dll"
            "${CMAKE_BINARY_DIR}/test/gdal.dll"
            "${CMAKE_BINARY_DIR}/test/libssl-3-x64.dll"
            "${CMAKE_BINARY_DIR}/test/libpq.dll"
            "${CMAKE_BINARY_DIR}/test/libintl-9.dll"
            "${CMAKE_BINARY_DIR}/test/libcrypto-3-x64.dll"
            "${CMAKE_BINARY_DIR}/test/hdf5.dll"
            "${CMAKE_BINARY_DIR}/test/libexpat.dll"
            "${CMAKE_BINARY_DIR}/test/gtest_main.dll"
            "${CMAKE_BINARY_DIR}/test/gtest.dll"
            "${CMAKE_BINARY_DIR}/test/libiconv-2.dll"
            "${CMAKE_BINARY_DIR}/test/libcharset-1.dll"
            "${CMAKE_BINARY_DIR}/test/sqlite3.dll"
            "${CMAKE_BINARY_DIR}/test/tiff.dll"
            "${CMAKE_BINARY_DIR}/test/geos_c.dll"
            "${CMAKE_BINARY_DIR}/test/geos.dll"
            "${CMAKE_BINARY_DIR}/test/libcurl.dll"
            "${CMAKE_BINARY_DIR}/test/libxml2-2.dll"
            "${CMAKE_BINARY_DIR}/test/zlib1.dll"
        )
        
        foreach(DLL ${BUILD_DLLS})
            if(EXISTS ${DLL})
                install(FILES ${DLL}
                    DESTINATION bin
                )
            endif()
        endforeach()
    endif()
endfunction()

# Function to install index.md files for each module
function(ogc_install_index_files)
    set(INDEX_FILES
        "${CMAKE_SOURCE_DIR}/geom/include/index_geom.md"
        "${CMAKE_SOURCE_DIR}/base/include/index_base.md"
        "${CMAKE_SOURCE_DIR}/proj/include/index_proj.md"
        "${CMAKE_SOURCE_DIR}/database/include/index_database.md"
        "${CMAKE_SOURCE_DIR}/feature/include/index_feature.md"
        "${CMAKE_SOURCE_DIR}/layer/include/index_layer.md"
        "${CMAKE_SOURCE_DIR}/cache/include/index_cache.md"
        "${CMAKE_SOURCE_DIR}/symbology/include/index_symbology.md"
        "${CMAKE_SOURCE_DIR}/draw/include/index_draw.md"
        "${CMAKE_SOURCE_DIR}/graph/include/index_graph.md"
        "${CMAKE_SOURCE_DIR}/alert/include/index_alert.md"
        "${CMAKE_SOURCE_DIR}/navi/include/index_navi.md"
        "${CMAKE_SOURCE_DIR}/navi/include/ogc/navi/index.md"
    )
    
    foreach(INDEX_FILE ${INDEX_FILES})
        if(EXISTS "${INDEX_FILE}")
            get_filename_component(FILENAME ${INDEX_FILE} NAME)
            install(FILES "${INDEX_FILE}"
                DESTINATION include/ogc
            )
            message(STATUS "Installing index: ${INDEX_FILE} -> include/ogc/${FILENAME}")
        endif()
    endforeach()
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/index_all.md")
        install(FILES "${CMAKE_SOURCE_DIR}/index_all.md"
            DESTINATION include/ogc
        )
        message(STATUS "Installing index: ${CMAKE_SOURCE_DIR}/index_all.md -> include/ogc/index_all.md")
    endif()
endfunction()

# Print install summary
function(ogc_print_install_summary)
    message(STATUS "")
    message(STATUS "Installation Configuration:")
    message(STATUS "  Prefix:     ${CMAKE_INSTALL_PREFIX}")
    message(STATUS "  Headers:    ${INSTALL_INCLUDE_DIR}")
    message(STATUS "  Libraries:  ${INSTALL_LIB_DIR}")
    message(STATUS "  Binaries:   ${INSTALL_BIN_DIR}")
    message(STATUS "  Docs/Share: ${INSTALL_SHARE_DIR}")
    message(STATUS "")
endfunction()
