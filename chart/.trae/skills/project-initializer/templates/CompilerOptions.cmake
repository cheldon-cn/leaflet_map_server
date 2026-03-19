# Compiler Options Module

# MSVC options
if(MSVC)
    add_compile_options(
        /W4                 # Warning level 4
        /WX-                # Warnings not as errors (can enable with /WX)
        /utf-8              # UTF-8 source and execution character set
        /permissive-        # Strict conformance
        /Zc:__cplusplus     # Enable correct __cplusplus value
        /EHsc               # Exception handling
    )
    
    # Debug options
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1")
    
    # Release options
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif()

# GCC/Clang options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wnull-dereference
        -Wformat=2
    )
    
    # Debug options
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -fsanitize=address,undefined")
    
    # Release options
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG")
    
    # Link time optimization
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)
    if(LTO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif()
endif()

# Position Independent Code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Export compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++20 features
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Function to set common target properties
function(set_target_options target)
    target_compile_features(${target} PUBLIC cxx_std_20)
    
    if(MSVC)
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Debug>:/Od>
            $<$<CONFIG:Release>:/O2>
        )
    else()
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Debug>:-O0>
            $<$<CONFIG:Release>:-O3>
        )
    endif()
endfunction()
