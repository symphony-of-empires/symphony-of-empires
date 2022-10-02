message(STATUS "Fetching SDL2 TTF...")

set(SDL2TTF_VENDORED ON)
set(SDL2TTF_INSTALL OFF)
set(SDL2TTF_SAMPLES OFF)
set(SDL2_SHARED OFF)
set(INTERFACE_SDL2_SHARED OFF)

FetchContent_Declare(sdl2_ttf 
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf" 
    GIT_TAG 0a652b598625d16ea7016665095cb1e9bce9ef4f # 2.20.1
)

FetchContent_MakeAvailable(sdl2_ttf)

add_library(dependency_SDL2_ttf INTERFACE)

target_include_directories(dependency_SDL2_ttf SYSTEM INTERFACE "${sdl2_ttf_SOURCE_DIR}")
target_link_libraries(dependency_SDL2_ttf INTERFACE SDL2_ttf)
# target_include_directories(dependency_SDL2 SYSTEM INTERFACE $<TARGET_PROPERTY:SDL2-static,INTERFACE_INCLUDE_DIRECTORIES>)
# target_link_libraries(dependency_SDL2 INTERFACE SDL2-static SDL2main)