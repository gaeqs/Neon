
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/neon/*.cpp")
file(GLOB_RECURSE VULKAN_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/vulkan/*.cpp")

if (NEON_USE_QT)
    #list(APPEND SOURCES
    #        neon/render/QTSwapChainFrameBuffer.cpp
    #        neon/render/QTSwapChainFrameBuffer.h)
    #list(APPEND VULKAN_SOURCES
    #        vulkan/QTApplication.h
    #        vulkan/QTApplication.cpp
    #        vulkan/render/VKQTSwapChainFrameBuffer.cpp
    #        vulkan/render/VKQTSwapChainFrameBuffer.h)
endif ()

cmrc_add_resource_library(dummy)

add_library(neon STATIC ${SOURCES} ${VULKAN_SOURCES})

target_link_libraries(neon PUBLIC ${LIBRARIES} ${CMAKE_DL_LIBS} dummy)
target_compile_definitions(neon PUBLIC USE_VULKAN)

if (NEON_USE_QT)
    target_compile_definitions(neon PUBLIC USE_QT)
endif ()