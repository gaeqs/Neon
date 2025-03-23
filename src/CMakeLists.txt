file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/neon/*.cpp")
file(GLOB_RECURSE VULKAN_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/vulkan/*.cpp")

add_library(neon STATIC ${SOURCES} ${VULKAN_SOURCES})

target_compile_features(neon PUBLIC cxx_std_20)

target_include_directories(neon PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:>
)

target_link_libraries(neon
        PUBLIC ${LIBRARIES} ${CMAKE_DL_LIBS}
        PRIVATE cmrc::base
)

target_compile_definitions(neon PUBLIC USE_VULKAN)

if (NEON_USE_QT)
    target_compile_definitions(neon PUBLIC USE_QT)
endif ()

install(TARGETS neon
        EXPORT neonTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include FILES_MATCHING PATTERN "*.h*")

install(EXPORT neonTargets
        FILE neonTargets.cmake
        NAMESPACE neon::
        DESTINATION lib/cmake/neon
)