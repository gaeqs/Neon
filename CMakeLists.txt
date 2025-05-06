cmake_minimum_required(VERSION 3.14)

option(NEON_TESTS "Enables Neon tests" OFF)
option(NEON_USE_QT "Use QT implementation" OFF)

set(CMAKE_CXX_STANDARD 20)
set(LIBZIPPP_INSTALL ON)
set(NEON_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
include(CMake/CMakeRC.cmake)
include(FetchContent)
include(FetchContent)

project(Neon LANGUAGES CXX VERSION 1.0.1)

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
        GIT_TAG 9fdeb41e52cd4271601b85fadf7d36a62c6ed09e
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
)

# ImPlot
FetchContent_Declare(
        implot
        GIT_TAG 3da8bd34299965d3b0ab124df743fe3e076fa222
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

# glslang
FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG 68f073b19569b580ecc7ba13fa96be3ecf65a0f6
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glslang
)

# libzippp
FetchContent_Declare(
        libzippp
        GIT_REPOSITORY https://github.com/ctabin/libzippp.git
        GIT_TAG a6e6794bb8ebbdd180c0877a2503ad84dace3419
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libzippp
)

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(implot)
FetchContent_MakeAvailable(spirv_headers)
FetchContent_MakeAvailable(spirv_tools)
FetchContent_MakeAvailable(glslang)
FetchContent_MakeAvailable(libzippp)

if (TARGET glslang)
    target_compile_options(glslang PRIVATE -include cstdint)
endif ()

if (TARGET SPIRV)
    target_compile_options(SPIRV PRIVATE -include cstdint)
endif ()

list(APPEND LIBRARIES
        glfw
        ${GLFW_LIBRARIES}
        Vulkan::Vulkan
        assimp::assimp
        SPIRV
        glslang
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

# Create version.h

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
            COMMAND git rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
else ()
    set(GIT_COMMIT "unknown")
endif ()

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/CMake/version.h.in
        ${NEON_GENERATED_DIR}/neon/version.h
)


install(
        DIRECTORY ${CMAKE_BINARY_DIR}/generated/neon
        DESTINATION include
)