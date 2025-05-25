add_library(vma STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/VmaUsage.cpp
)

target_include_directories(vma
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(vma PUBLIC Vulkan::Vulkan)
target_compile_features(vma PUBLIC cxx_std_20)

install(TARGETS vma
        EXPORT vmaTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/vma
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*"
)

install(EXPORT vmaTargets
        FILE VmaTargets.cmake
        NAMESPACE vma::
        DESTINATION lib/cmake/vma
)