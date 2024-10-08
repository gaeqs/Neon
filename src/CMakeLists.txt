
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/neon/*.cpp")
file(GLOB_RECURSE VULKAN_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/vulkan/*.cpp")

cmrc_add_resource_library(cmrc_dummy_resource_library)

add_library(neon STATIC ${SOURCES} ${VULKAN_SOURCES})

target_link_libraries(neon PUBLIC ${LIBRARIES} ${CMAKE_DL_LIBS} cmrc_dummy_resource_library)
target_compile_definitions(neon PUBLIC USE_VULKAN)

if (NEON_USE_QT)
    target_compile_definitions(neon PUBLIC USE_QT)
endif ()