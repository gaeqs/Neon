set(TEST_DIRECTORY ${CMAKE_BINARY_DIR}/neon_tests)

add_custom_target(neon_tests)
add_custom_command(TARGET neon_tests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resource ${TEST_DIRECTORY}/resource)

add_subdirectory(test1)
add_subdirectory(testCloth)
add_subdirectory(testPBR)
add_subdirectory(testLine)
add_subdirectory(testOctree)
add_subdirectory(testAsyncUpdate)
add_subdirectory(testGeometryShader)
add_subdirectory(testMeshShader)
add_subdirectory(unit)

if (NEON_USE_CEF)
    add_subdirectory(testCEF)
endif ()