cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

option(VULKAN_MODE "Enables Vulkan" OFF)

include(CMake/CMakeRC.cmake)

project(neon)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
    set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
    STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
endif ()

# GLAD
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include)

# STB
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb/src)

list(APPEND LIBRARIES glad)
list(APPEND LIBRARIES glfw ${GLFW_LIBRARIES})
list(APPEND LIBRARIES ${VULKAN_LIB_LIST})

if (WIN32)
    list(APPEND LIBRARIES assimp::assimp)
    include_directories(AFTER ${VULKAN_PATH}/Include)
    link_directories(${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib;)
    set(VULKAN_LIB_LIST "vulkan-1")
else ()
    list(APPEND LIBRARIES assimp)
    set(VULKAN_LIB_LIST "vulkan")
endif ()

include_directories(src)
add_subdirectory(src)

if (NEON_TESTS)
    add_subdirectory(test)
endif ()