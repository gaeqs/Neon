cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-DUSE_OPENGL)

include(CMake/CMakeRC.cmake)

project(RVTracking)

find_package(OpenCV CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# ARUCO
set(BUILD_UTILS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/aruco)
include_directories(${CMAKE_SOURCE_DIR}/lib/aruco/src)

# GLAD
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glad/)
include_directories(${CMAKE_SOURCE_DIR}/lib/glad/include)

# STB
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/stb/)
include_directories(${CMAKE_SOURCE_DIR}/lib/stb/src)


list(APPEND LIBRARIES opencv_core opencv_calib3d opencv_imgcodecs opencv_videoio opencv_imgproc opencv_features2d opencv_highgui ${THREADLIB})
list(APPEND LIBRARIES aruco)
list(APPEND LIBRARIES glad)
list(APPEND LIBRARIES glfw ${GLFW_LIBRARIES})

if (WIN32)
    list(APPEND LIBRARIES assimp::assimp)
else ()
    list(APPEND LIBRARIES assimp)
endif ()

include_directories(src)
add_subdirectory(src)

if (ENGINE_TESTS)
    add_subdirectory(test)
endif ()