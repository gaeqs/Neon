#=================== IMGUI ===================

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(IMGUI STATIC)

target_sources( IMGUI
        PRIVATE
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui.cpp

        PRIVATE
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        )

target_include_directories( IMGUI
        PUBLIC ${IMGUI_DIR}
        PUBLIC ${IMGUI_DIR}/backends
        )

target_link_libraries(IMGUI PUBLIC ${VULKAN_LIB_LIST} glfw ${GLFW_LIBRARIES})


set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)
add_library(IMPLOT STATIC)

target_sources( IMPLOT
        PRIVATE
        ${IMPLOT_DIR}/implot.cpp
        ${IMPLOT_DIR}/implot_demo.cpp
        ${IMPLOT_DIR}/implot_items.cpp
        )

target_include_directories( IMPLOT PUBLIC ${IMPLOT_DIR})

target_link_libraries(IMPLOT IMGUI)
