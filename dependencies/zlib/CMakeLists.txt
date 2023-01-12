message(STATUS "Fetching zlib...")

IF(E3D_ZLIB_VENDORED)
    include(ExternalProject)
    ExternalProject_Add(zlib_project
        GIT_REPOSITORY    https://github.com/madler/zlib.git
        GIT_TAG           master
        SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/zlib-src"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/zlib-build"
        BUILD_BYPRODUCTS	"${CMAKE_CURRENT_BINARY_DIR}/zlib-build/zlibstaticd.lib"
        INSTALL_COMMAND	""
    )

    add_library(dependency_zlib INTERFACE)
    add_dependencies(dependency_zlib zlib_project)
    target_include_directories(dependency_zlib SYSTEM INTERFACE  "${CMAKE_CURRENT_BINARY_DIR}/zlib-src" "${CMAKE_CURRENT_BINARY_DIR}/zlib-build")
    IF(WIN32)
        target_link_libraries(dependency_zlib INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/zlib-build/zlibstaticd.lib")
    ELSE()
        target_link_libraries(dependency_zlib INTERFACE z)
    ENDIF()
ELSE()
    find_package(ZLIB REQUIRED)
    get_target_property(INCLUDE_DIRS ZLIB::ZLIB INTERFACE_INCLUDE_DIRECTORIES)
    add_library(dependency_zlib INTERFACE)
    target_include_directories(dependency_zlib SYSTEM INTERFACE ${INCLUDE_DIRS})
    target_link_libraries(dependency_zlib INTERFACE ZLIB::ZLIB)
ENDIF()
