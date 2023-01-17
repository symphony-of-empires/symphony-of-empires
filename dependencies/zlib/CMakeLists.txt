message(STATUS "Fetching zlib...")

IF(E3D_ZLIB_VENDORED)
    FetchContent_Declare(zlib
        GIT_REPOSITORY "https://github.com/madler/zlib"
    )

    FetchContent_MakeAvailable(zlib)

    add_library(dependency_zlib INTERFACE)

    target_include_directories(dependency_zlib SYSTEM INTERFACE "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
    target_link_libraries(dependency_zlib INTERFACE zlibstatic)
ELSE()
    find_package(ZLIB REQUIRED)
    get_target_property(INCLUDE_DIRS ZLIB::ZLIB INTERFACE_INCLUDE_DIRECTORIES)
    add_library(dependency_zlib INTERFACE)
    target_include_directories(dependency_zlib SYSTEM INTERFACE ${INCLUDE_DIRS})
    target_link_libraries(dependency_zlib INTERFACE ZLIB::ZLIB)
ENDIF()
