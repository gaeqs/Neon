function(add_resources out_var)
    set(result)
    foreach (in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        set(out_f "${src_f}.o")
        add_custom_command(OUTPUT ${out_f}
                COMMAND ld -r -b binary -o ${out_f} ${src_f}
                DEPENDS ${out_spv_f}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Building GLSL object ${out_f}"
                VERBATIM
                )
        list(APPEND result ${out_f})
    endforeach ()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

cmake_minimum_required(VERSION 3.23)
project(RVTracking)

set(CMAKE_CXX_STANDARD 20)

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


list(APPEND LIBRARIES opencv_core opencv_calib3d opencv_imgcodecs opencv_videoio opencv_imgproc opencv_features2d opencv_highgui ${THREADLIB})
list(APPEND LIBRARIES aruco)
list(APPEND LIBRARIES glad)
list(APPEND LIBRARIES glfw ${GLFW_LIBRARIES})
list(APPEND LIBRARIES assimp::assimp)

add_subdirectory(src)

target_link_libraries(RVTracking ${LIBRARIES} ${CMAKE_DL_LIBS})
