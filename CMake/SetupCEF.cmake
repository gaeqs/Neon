macro(setup_cef VERSION DOWNLOAD_DIR)
    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        if ("${PROJECT_ARCH}" STREQUAL "arm64")
            set(CEF_PLATFORM "macosarm64")
        elseif ("${PROJECT_ARCH}" STREQUAL "x86_64")
            set(CEF_PLATFORM "macosx64")
        elseif ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(PROJECT_ARCH "arm64")
            set(CEF_PLATFORM "macosarm64")
        else ()
            set(PROJECT_ARCH "x86_64")
            set(CEF_PLATFORM "macosx64")
        endif ()
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            set(CEF_PLATFORM "linuxarm")
        elseif ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "linuxarm64")
        elseif (CMAKE_SIZEOF_VOID_P MATCHES 8)
            set(CEF_PLATFORM "linux64")
        else ()
            message(FATAL_ERROR "Linux x86 32-bit builds are discontinued.")
        endif ()
    elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        if ("${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "ARM64")
            set(CEF_PLATFORM "windowsarm64")
        elseif (CMAKE_SIZEOF_VOID_P MATCHES 8)
            set(CEF_PLATFORM "windows64")
        else ()
            set(CEF_PLATFORM "windows32")
        endif ()
    endif ()


    include(DownloadCEF)
    DownloadCEF(${CEF_PLATFORM} ${VERSION} ${DOWNLOAD_DIR})
    list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")
    find_package(CEF REQUIRED)
    add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

    include_directories(${CEF_INCLUDE_PATH})
    include_directories(${CEF_INCLUDE_PATH}/include)

    add_library(libcef SHARED IMPORTED)

    if(WIN32)
        set_target_properties(libcef PROPERTIES
                IMPORTED_LOCATION_DEBUG "${CEF_BINARY_DIR_DEBUG}/libcef.dll"
                IMPORTED_IMPLIB_DEBUG "${CEF_LIB_DEBUG}"
                IMPORTED_LOCATION_RELEASE "${CEF_BINARY_DIR_RELEASE}/libcef.dll"
                IMPORTED_IMPLIB_RELEASE "${CEF_LIB_RELEASE}"
        )
    elseif(APPLE)
        set_target_properties(libcef PROPERTIES
                IMPORTED_LOCATION_DEBUG "${CEF_BINARY_DIR_DEBUG}/libcef.dylib"
                IMPORTED_LOCATION_RELEASE "${CEF_BINARY_DIR_RELEASE}/libcef.dylib"
        )
    elseif(UNIX)  # Linux u otros POSIX
        set_target_properties(libcef PROPERTIES
                IMPORTED_LOCATION_DEBUG "${CEF_BINARY_DIR_DEBUG}/libcef.so"
                IMPORTED_LOCATION_RELEASE "${CEF_BINARY_DIR_RELEASE}/libcef.so"
        )
    endif()

    message(STATUS "CEF_LIB_DEBUG: ${CEF_LIB_DEBUG}")
    message(STATUS "CEF_LIB_RELEASE: ${CEF_LIB_RELEASE}")

endmacro()


function(copy_cef_resources TARGET OUT_DIR)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_ROOT}/Resources" "${OUT_DIR}"
            COMMENT "Copying CEF Resources"
    )
endfunction()

function(copy_cef_build TARGET OUT_DIR)
    add_custom_command(TARGET ${TARGET} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_ROOT}/$<CONFIG>" "${OUT_DIR}"
            COMMENT "Copying CEF "
    )
endfunction()


