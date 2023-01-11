message(STATUS "Fetching SDL2 TTF...")

set(SDL2TTF_VENDORED ON)
set(SDL2TTF_BUILD_SHARED_LIBS OFF)
set(SDL2TTF_INSTALL OFF)

FetchContent_Declare(sdl2_ttf 
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf" 
    GIT_TAG 0a652b598625d16ea7016665095cb1e9bce9ef4f # 2.20.1
)

FetchContent_MakeAvailable(sdl2_ttf)

add_library(dependency_SDL2_ttf INTERFACE)

target_include_directories(dependency_SDL2_ttf SYSTEM INTERFACE "${sdl2_ttf_SOURCE_DIR}" "${sdl2_ttf_BINARY_DIR}")

target_link_libraries(dependency_SDL2_ttf INTERFACE SDL2_ttf)

add_library(dependency_freetype INTERFACE)

target_include_directories(dependency_freetype SYSTEM INTERFACE
        "${sdl2_ttf_SOURCE_DIR}/external/freetype"
        "${sdl2_ttf_SOURCE_DIR}/external/freetype/include"
        "${sdl2_ttf_BINARY_DIR}/external/freetype"
        "${sdl2_ttf_BINARY_DIR}/external/freetype/include"
        "${sdl2_ttf_BINARY_DIR}/external/freetype/include/config")
target_link_libraries(dependency_freetype INTERFACE freetype)

target_link_libraries(dependency_SDL2_ttf INTERFACE
        dependency_freetype)
