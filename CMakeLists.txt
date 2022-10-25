cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

include(CMake/CMakeRC.cmake)

project(neon)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# GLAD
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include)

# STB
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb/src)


list(APPEND LIBRARIES glad)
list(APPEND LIBRARIES glfw ${GLFW_LIBRARIES})

if (WIN32)
    list(APPEND LIBRARIES assimp::assimp)
else ()
    list(APPEND LIBRARIES assimp)
endif ()

include_directories(src)
add_subdirectory(src)

if (NEON_TESTS)
    add_subdirectory(test)
endif ()