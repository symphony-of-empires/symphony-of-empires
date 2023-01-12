message(STATUS "Fetching SDL2...")

IF(${E3D_SDL2_VENDORED})
    IF(BUILD_SHARED_LIBS)
        set(SDL_SHARED ON CACHE BOOL "" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    ELSE()
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC_PIC ON)
    ENDIF()

    set(SDL2_DISABLE_SDL2MAIN ON)
    set(SDL2_DISABLE_INSTALL ON)
    set(SDL2_DISABLE_UNINSTALL ON)
    set(SDL_TEST OFF)

    FetchContent_Declare(SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG 8c9beb0c873f6ca5efbd88f1ad2648bfc793b2ac # 2.24.0
    )

    FetchContent_MakeAvailable(SDL2)

    add_library(dependency_SDL2 INTERFACE)

    IF(BUILD_SHARED_LIBS)
        target_include_directories(dependency_SDL2 SYSTEM INTERFACE $<TARGET_PROPERTY:SDL2,INTERFACE_INCLUDE_DIRECTORIES>)
        target_link_libraries(dependency_SDL2 INTERFACE SDL2::SDL2)
    ELSE()
        target_include_directories(dependency_SDL2 SYSTEM INTERFACE $<TARGET_PROPERTY:SDL2-static,INTERFACE_INCLUDE_DIRECTORIES>)
        target_link_libraries(dependency_SDL2 INTERFACE SDL2-static)
    ENDIF()
ELSE()
    find_package(SDL2 REQUIRED)
    get_target_property(INCLUDE_DIRS SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
    add_library(dependency_SDL2 INTERFACE)
    target_include_directories(dependency_SDL2 SYSTEM INTERFACE ${INCLUDE_DIRS})
    target_link_libraries(dependency_SDL2 INTERFACE SDL2::SDL2)
ENDIF()
