set(TEST_DIRECTORY ${CMAKE_BINARY_DIR}/neon_tests)

add_custom_target(neon_tests)
add_custom_command(TARGET neon_tests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resource ${TEST_DIRECTORY}/resource)

add_subdirectory(test1)
add_subdirectory(testCloth)
add_subdirectory(testPBR)