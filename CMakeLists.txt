cmake_minimum_required(VERSION 3.10)
project(map_server VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 输出目录配置
set(MAP_SERVER_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output"
    CACHE PATH "Directory for generated map files")

# 确保输出目录存在
file(MAKE_DIRECTORY "${MAP_SERVER_OUTPUT_DIR}")

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/server/include
    ${CMAKE_CURRENT_SOURCE_DIR}/server/thirdparty
    ${CMAKE_SOURCE_DIR}/draw  # 现有绘图模块
)

# 查找libpng
find_package(PNG QUIET)
if(NOT PNG_FOUND)
    message(WARNING "libpng not found. PNG encoding will be disabled.")
    add_definitions(-DHAVE_LIBPNG=0)
else()
    add_definitions(-DHAVE_LIBPNG=1)
    message(STATUS "Found libpng: ${PNG_LIBRARIES}")
endif()

# 查找nlohmann/json（用于配置解析）
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    add_definitions(-DHAS_NLOHMANN_JSON=1)
    message(STATUS "Found nlohmann/json")
else()
    message(STATUS "nlohmann/json not found, using simple config parser")
endif()

# 使用FetchContent获取cpp-httplib
include(FetchContent)
FetchContent_Declare(
    cpp-httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.1
)
FetchContent_MakeAvailable(cpp-httplib)

if(cpp-httplib_POPULATED)
    add_definitions(-DHAS_CPP_HTTPLIB=1)
    message(STATUS "cpp-httplib available")
else()
    message(WARNING "cpp-httplib not available, HTTP server will be disabled")
endif()

# 链接现有库（从父项目）
# 假设父项目已构建sqlite_recovery_lib和feature_database_writer_lib
# 我们将通过target_link_libraries链接它们

# 源文件列表
set(SERVER_SOURCES
    server/src/Config.cpp
    server/src/DatabaseManager.cpp
    server/src/PngEncoder.cpp
    server/src/RenderEngine.cpp
    server/src/HttpServer.cpp
    server/src/main.cpp
)

# 添加可执行文件
add_executable(map_server ${SERVER_SOURCES})

# 链接库
if(PNG_FOUND)
    target_link_libraries(map_server ${PNG_LIBRARIES})
endif()

# 链接现有库（需要从父项目导出）
# 这些应该在父项目的CMakeLists.txt中通过add_subdirectory暴露
target_link_libraries(map_server
    sqlite_recovery_lib
    feature_database_writer_lib
)

# 添加编译器定义
target_compile_definitions(map_server PRIVATE
    MAP_SERVER_OUTPUT_DIR="${MAP_SERVER_OUTPUT_DIR}"
)

# 安装目标
install(TARGETS map_server DESTINATION bin)

# 创建配置示例
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/server/config/config.example.json"
    "${CMAKE_CURRENT_BINARY_DIR}/config.example.json"
    COPYONLY
)

message(STATUS "Map Server configured. Output directory: ${MAP_SERVER_OUTPUT_DIR}")