cmake_minimum_required(VERSION 3.0)
project(stb)

add_library(stb INTERFACE)

target_include_directories(stb
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:>
)

install(TARGETS stb
        EXPORT stbTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/stb
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/
        FILES_MATCHING PATTERN "*.h*"
)

install(EXPORT stbTargets
        FILE stbTargets.cmake
        NAMESPACE stb::
        DESTINATION lib/cmake/stb
)
