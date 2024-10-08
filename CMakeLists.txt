function(compile_shaders out_var)
    set(result)
    foreach (in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        set(out_spv_f "${PROJECT_BINARY_DIR}/${in_f}.spv")
        get_filename_component(parent ${out_spv_f} PATH)
        file(MAKE_DIRECTORY ${parent})
        add_custom_command(OUTPUT ${out_spv_f}
                COMMAND glslc ${src_f} -o ${out_spv_f}
                DEPENDS ${in_f}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Generating SPV file ${out_spv_f}"
                VERBATIM
                )
        list(APPEND result ${out_spv_f})
    endforeach ()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

option(NEON_TESTS "Enables Neon tests" OFF)
option(NEON_USE_QT "Use QT implementation" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
include(CMake/CMakeRC.cmake)

project(neon)

find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
    set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
    STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
endif ()

# STB
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb/src)

# SPIRV-TOOLS
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/spirv-headers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/spirv-tools)

# GLSLANG
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glslang)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glslang)

# RUSH
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/Rush)

if (WIN32)
    list(APPEND LIBRARIES assimp::assimp)

    include_directories(AFTER ${VULKAN_PATH}/Include)
    link_directories(${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib;)
    set(VULKAN_LIB_LIST "vulkan-1")
else ()
    list(APPEND LIBRARIES assimp)
    set(VULKAN_LIB_LIST "vulkan")
endif ()

list(APPEND LIBRARIES glfw ${GLFW_LIBRARIES})
list(APPEND LIBRARIES ${VULKAN_LIB_LIST})
list(APPEND LIBRARIES SPIRV glslang)
list(APPEND LIBRARIES IMGUI IMPLOT)
list(APPEND LIBRARIES rush)

# QT

if (NEON_USE_QT)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui)
    list(APPEND LIBRARIES Qt5::Core Qt5::Gui)
endif ()

if (RELEASE_DEBUG)
    add_compile_definitions(RELEASE_DEBUG)
endif ()

add_subdirectory(lib)
include_directories(src)
add_subdirectory(src)

if (NEON_TESTS)
    add_subdirectory(test)
endif ()