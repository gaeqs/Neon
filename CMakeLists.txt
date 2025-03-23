cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)

option(NEON_TESTS "Enables Neon tests" OFF)
option(NEON_USE_QT "Use QT implementation" OFF)
option(NEON_PLUGINS "Include the plugin framework" ON)

set(LIBZIPPP_INSTALL ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
include(CMake/CMakeRC.cmake)
include(FetchContent)

project(Neon)

find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Vulkan REQUIRED)


# Rush
FetchContent_Declare(
        rush
        GIT_REPOSITORY https://github.com/gaeqs/Rush.git
        GIT_TAG master
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/rush
)

FetchContent_MakeAvailable(rush)

# ImGui ("docking")
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG b4c96355c9b51b54c4deb52e7d7cdfc7bf79bc2f
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
)

# ImPlot
FetchContent_Declare(
        implot
        GIT_TAG 18c72431f8265e2b0b5378a3a73d8a883b2175ff
        GIT_REPOSITORY https://github.com/epezent/implot.git
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot
)

# SPIRV-Headers
FetchContent_Declare(
        spirv_headers
        GIT_TAG cfbe4feef20c3c0628712c2792624f0221e378ac
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/spirv_headers
)

# SPIRV-Tools
FetchContent_Declare(
        spirv_tools
        GIT_TAG 6f276e05ccab210584996bc40a0bef82b91f4f40
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/spirv_tools
)

# Glslang
FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG 68f073b19569b580ecc7ba13fa96be3ecf65a0f6
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glslang
)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# libzippp
FetchContent_Declare(
        libzippp
        GIT_REPOSITORY https://github.com/ctabin/libzippp.git
        GIT_TAG a6e6794bb8ebbdd180c0877a2503ad84dace3419
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libzippp
)

if (NEON_PLUGINS)
    FetchContent_Declare(
            extism_cpp
            GIT_REPOSITORY https://github.com/gaeqs/cpp-sdk.git
            GIT_TAG 82264495bcef3cd60c398658d07cb016705ab61b
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/extism_cpp
    )
    FetchContent_MakeAvailable(extism_cpp)
    list(APPEND LIBRARIES extism-cpp bcrypt)
endif ()

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(implot)
FetchContent_MakeAvailable(spirv_headers)
FetchContent_MakeAvailable(spirv_tools)
FetchContent_MakeAvailable(glslang)
FetchContent_MakeAvailable(libzippp)

list(APPEND LIBRARIES
        glfw
        ${GLFW_LIBRARIES}
        Vulkan::Vulkan
        assimp::assimp
        glslang
        glslang-default-resource-limits
        SPIRV
        SPIRV-Tools
        libzippp
        nlohmann_json::nlohmann_json
        imgui
        implot
        rush
        stb
)

# QT
if (NEON_USE_QT)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui)
    list(APPEND LIBRARIES Qt5::Core Qt5::Gui)
endif ()

if (RELEASE_DEBUG)
    add_compile_definitions(RELEASE_DEBUG)
endif ()

add_subdirectory(lib) # Add ImGUI and ImPlot libraries
add_subdirectory(src)

if (NEON_TESTS)
    add_subdirectory(test)
endif ()