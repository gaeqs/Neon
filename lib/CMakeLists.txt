# =================== IMGUI ===================

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(imgui STATIC
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

target_include_directories(imgui
        PUBLIC
        $<BUILD_INTERFACE:${IMGUI_DIR}>
        $<BUILD_INTERFACE:${IMGUI_DIR}/backends>
        $<INSTALL_INTERFACE:include/imgui>
)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw ${GLFW_LIBRARIES})
target_compile_features(imgui PUBLIC cxx_std_20)

install(TARGETS imgui
        EXPORT imguiTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/imgui
)

install(DIRECTORY ${IMGUI_DIR}/
        DESTINATION include/imgui
        FILES_MATCHING PATTERN "*.h*"
)

install(EXPORT imguiTargets
        FILE ImguiTargets.cmake
        NAMESPACE imgui::
        DESTINATION lib/cmake/imgui
)

# =================== IMPLOT ===================

set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)
add_library(implot STATIC
        ${IMPLOT_DIR}/implot.cpp
        ${IMPLOT_DIR}/implot_demo.cpp
        ${IMPLOT_DIR}/implot_items.cpp
)

target_include_directories(implot
        PUBLIC
        $<BUILD_INTERFACE:${IMPLOT_DIR}>
        $<INSTALL_INTERFACE:include/implot>
)

target_link_libraries(implot PUBLIC imgui)
target_compile_features(implot PUBLIC cxx_std_20)

install(TARGETS implot
        EXPORT implotTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/implot
)

install(DIRECTORY ${IMPLOT_DIR}/
        DESTINATION include/implot
        FILES_MATCHING PATTERN "*.h*"
)

install(EXPORT implotTargets
        FILE ImplotTargets.cmake
        NAMESPACE implot::
        DESTINATION lib/cmake/implot
)

add_subdirectory(stb)